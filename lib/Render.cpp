///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// Indigo is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Indigo is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Indigo.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#include "Render.h"
#include "State.h"
#include "Window.h"

#include <stdlib.h>

namespace
{
	class Queue
	{
	public:
		typedef bool (*callback_t)(void* p);

		bool enqueue(callback_t callback, void* param = NULL)
		{
			OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);

			int err = m_queue.push(Item(callback,param));
			if (err)
				LOG_ERROR_RETURN(("Failed to enqueue command: %s",OOBase::system_error_text(err)),false);

			m_cond.signal();
			return true;
		}

		void swap(Queue& rhs)
		{
			OOBase::swap(m_lock,rhs.m_lock);
			OOBase::swap(m_cond,rhs.m_cond);
			OOBase::swap(m_queue,rhs.m_queue);
		}

	protected:
		OOBase::Condition::Mutex m_lock;
		OOBase::Condition        m_cond;

		struct Item
		{
			Item() : m_callback(NULL), m_param(NULL)
			{}

			Item(callback_t c, void* p) : m_callback(c), m_param(p)
			{}

			callback_t m_callback;
			void*      m_param;
		};

		OOBase::Queue<Item,OOBase::ThreadLocalAllocator> m_queue;
	};

	class EventQueue : public Queue
	{
	public:
		bool dequeue(const OOBase::Timeout& timeout = OOBase::Timeout())
		{
			OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);
			for (;;)
			{
				while (m_queue.empty())
				{
					if (!m_cond.wait(m_lock,timeout))
						return false;
				}

				for (Item item;m_queue.pop(&item);)
				{
					guard.release();

					// NULL callback means response
					if (!item.m_callback)
						return item.m_param == reinterpret_cast<void*>(1);

					if (!(*item.m_callback)(item.m_param))
						return false;

					guard.acquire();
				}
			}
		}
	};

	class RenderQueue : public Queue
	{
	public:
		bool dequeue()
		{
			OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock,false);
			if (!guard.try_acquire())
				return true;

			// Get next buffer
			for (Item item;m_queue.pop(&item);)
			{
				guard.release();

				if (!item.m_callback)
					break;

				if (!(*item.m_callback)(item.m_param))
					return false;

				if (!guard.try_acquire())
					break;
			}
			return true;
		}

		bool dequeue_block(const OOBase::Timeout& timeout = OOBase::Timeout())
		{
			OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);
			while (m_queue.empty())
			{
				if (!m_cond.wait(m_lock,timeout))
					return true;
			}

			// Get next buffer
			for (Item item;m_queue.pop(&item);)
			{
				guard.release();

				if (!item.m_callback)
					break;

				if (!(*item.m_callback)(item.m_param))
					return false;

				guard.acquire();
			}
			return true;
		}
	};

	static EventQueue* s_event_queue = NULL;
	static RenderQueue* s_render_queue = NULL;
	static OOBase::Vector<Indigo::Window*,OOBase::ThreadLocalAllocator>* s_vecWindows;

	struct thread_info
	{
		OOBase::Event* m_started;
		const OOBase::Table<OOBase::String,OOBase::String>* m_config;
		bool (*m_fn)(const OOBase::Table<OOBase::String,OOBase::String>& args);
	};

	struct render_call_info
	{
		bool (*m_fn)(void*);
		void* m_param;
	};

	struct raise_event_thunk
	{
		void (*m_fn)(OOBase::CDRStream& stream);
		OOBase::Buffer* m_buffer;
	};
}

static void on_glfw_error(int code, const char* message)
{
	OOBase::Logger::log(OOBase::Logger::Error,"GLFW error %d: %s",code,message);
}

static bool stop_thread(void*)
{
	return false;
}

static int logic_thread_start(void* param)
{
	RenderQueue render_queue;
	s_render_queue = &render_queue;

	// Signal we have started
	thread_info ti = *reinterpret_cast<thread_info*>(param);
	ti.m_started->set();
	ti.m_started = NULL;

	// Run the logic loop
	int err = (*ti.m_fn)(*ti.m_config) ? EXIT_SUCCESS : EXIT_FAILURE;

	// Tell the render thread we have finished
	render_queue.enqueue(&stop_thread);

	if (err != EXIT_SUCCESS)
	{
		// Wait for the render thread to stop using render_queue
		s_event_queue->dequeue();
	}

	return err;
}

bool Indigo::start_render_thread(bool (*logic_thread)(const OOBase::Table<OOBase::String,OOBase::String>& args), const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	OOBase::Event started(false,false);

	thread_info ti;
	ti.m_started = &started;
	ti.m_config = &config_args;
	ti.m_fn = logic_thread;

	// Force creation of event queue here
	EventQueue event_queue;
	s_event_queue = &event_queue;

	OOBase::Thread logic(false);
	int err = logic.run(&logic_thread_start,&ti);
	if (err)
		LOG_ERROR_RETURN(("Failed to start thread: %s",OOBase::system_error_text(err)),false);

	started.wait();

	// Now run the draw_thread (it must be the main thread)
	bool res = Window::draw_thread(config_args);

	// Send a quit to the logic_thread
	if (event_queue.enqueue(NULL,reinterpret_cast<void*>(1)))
	{
		// Wait for logic thread to end
		logic.join();
	}

	return res;
}

static bool make_render_call(void* param)
{
	struct render_call_info* rci = static_cast<struct render_call_info*>(param);

	void* ret = (*rci->m_fn)(rci->m_param) ? reinterpret_cast<void*>(1) : static_cast<void*>(0);

	return s_event_queue->enqueue(NULL,ret);
}

bool Indigo::render_call(bool (*fn)(void*), void* param)
{
	struct render_call_info rci;
	rci.m_fn = fn;
	rci.m_param = param;

	if (!s_render_queue->enqueue(&make_render_call,&rci))
		return false;

	return s_event_queue->dequeue();
}

static bool raise_raw_event(bool (*fn)(void*), void* param)
{
	return s_event_queue->enqueue(fn,param);
}

static bool raise_event_cleanup(void* param)
{
	struct raise_event_thunk* ret = static_cast<struct raise_event_thunk*>(param);

	ret->m_buffer->release();
	OOBase::ThreadLocalAllocator::delete_free(ret);

	return true;
}

static bool raise_event_i(void* param)
{
	struct raise_event_thunk* ret = static_cast<struct raise_event_thunk*>(param);

	OOBase::CDRStream stream(ret->m_buffer);
	(*ret->m_fn)(stream);

	return s_render_queue->enqueue(&raise_event_cleanup,ret);
}

bool Indigo::raise_event(void (*fn)(OOBase::CDRStream&), OOBase::CDRStream& stream)
{
	struct raise_event_thunk* ret = NULL;
	OOBase::ThreadLocalAllocator::allocate_new(ret);
	if (!ret)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	ret->m_fn = fn;
	ret->m_buffer = stream.buffer().addref();

	if (!raise_raw_event(&raise_event_i,ret))
	{
		ret->m_buffer->release();
		OOBase::ThreadLocalAllocator::free(ret);
		return false;
	}
	return true;
}

bool Indigo::handle_events()
{
	return s_event_queue->dequeue();
}

Indigo::Window::Window(int width, int height, const char* title, unsigned int style, GLFWmonitor* monitor) :
		m_glfw_window(NULL)
{
	glfwWindowHint(GLFW_VISIBLE,(style & eWSvisible) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE,(style & eWSresizable) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_DECORATED,(style & eWSdecorated) ? GL_TRUE : GL_FALSE);

	// Now try to create the window
	m_glfw_window = glfwCreateWindow(width,height,title,monitor,NULL);
	if (!m_glfw_window)
		LOG_ERROR(("Failed to create window"));
	else
	{
		// Wait for window manager to do its thing
		glfwWaitEvents();

		glfwMakeContextCurrent(m_glfw_window);

		m_state = OOBase::allocate_shared<State,OOBase::ThreadLocalAllocator>(shared_from_this());
		if (!m_state)
			LOG_ERROR(("Failed to allocate GL state object"));

		glfwSetWindowUserPointer(m_glfw_window,this);
		glfwSetFramebufferSizeCallback(m_glfw_window,&on_size);
		glfwSetWindowCloseCallback(m_glfw_window,&on_close);
		glfwSetWindowFocusCallback(m_glfw_window,&on_focus);
		glfwSetWindowIconifyCallback(m_glfw_window,&on_iconify);
		glfwSetWindowRefreshCallback(m_glfw_window,&on_refresh);

		s_vecWindows->push_back(this);
	}
}

Indigo::Window::~Window()
{
	if (m_glfw_window)
	{
		s_vecWindows->erase(this);

		glfwDestroyWindow(m_glfw_window);
	}
}

bool Indigo::Window::draw_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	OOBase::Vector<Indigo::Window*,OOBase::ThreadLocalAllocator> vecWindows;
	s_vecWindows = &vecWindows;

	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

	// Set defaults
	glfwDefaultWindowHints();

//	if (!Indigo::is_debug())
//		glfwSwapInterval(1);

	// Loop blocking until we have windows
	bool res = true;
	while (vecWindows.empty())
	{
		res = s_render_queue->dequeue_block();
		if (!res)
			break;
	}

	while (res)
	{
		// Draw all windows
		bool visible_window = false;
		for (OOBase::Vector<Indigo::Window*,OOBase::ThreadLocalAllocator>::iterator i=vecWindows.begin();i!=vecWindows.end();++i)
		{
			if ((*i)->draw())
				visible_window = true;
		}

		// Update animations

		// Get render commands
		if (vecWindows.empty() || !(res = s_render_queue->dequeue()))
			break;

		// Swap all windows (this collects events)
		for (OOBase::Vector<Indigo::Window*,OOBase::ThreadLocalAllocator>::iterator i=vecWindows.begin();i!=vecWindows.end();++i)
			(*i)->swap();

		// Poll for UI events
		if (visible_window)
			glfwPollEvents();
		else
			glfwWaitEvents();
	}

	glfwTerminate();

	return res;
}