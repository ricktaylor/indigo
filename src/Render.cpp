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
#include "../lib/State.h"
#include "../lib/StateFns.h"

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

			if (!m_queue.push(Item(callback,param)))
				LOG_ERROR_RETURN(("Failed to enqueue command: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

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
		bool dequeue()
		{
			OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);
			for (;;)
			{
				while (m_queue.empty())
					m_cond.wait(m_lock);

				for (Item item;m_queue.pop(&item);)
				{
					guard.release();

					// NULL callback means response
					if (!item.m_callback)
						return item.m_param == reinterpret_cast<void*>(1);

					if (!(*item.m_callback)(item.m_param))
						break;

					guard.acquire();
				}
			}
			return false;
		}
	};

	class RenderQueue : public Queue
	{
	public:
		bool dequeue(bool& stop, const OOBase::Timeout& timeout = OOBase::Timeout())
		{
			do
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
						stop = true;
					else if (!(*item.m_callback)(item.m_param))
						return false;

					if (!guard.acquire(timeout))
						return true;
				}
			}
			while (!stop);

			return true;
		}
	};

#if !defined(NDEBUG)
	static const OOBase::Thread* s_render_thread = NULL;
#endif

	static EventQueue* s_event_queue = NULL;
	static RenderQueue* s_render_queue = NULL;
	static OOBase::Vector<OOBase::WeakPtr<OOGL::Window>,OOBase::ThreadLocalAllocator>* s_vecWindows;

	struct thread_info
	{
		OOBase::Event* m_started;
		const OOBase::Table<OOBase::String,OOBase::String>* m_config;
		bool (*m_fn)(const OOBase::Table<OOBase::String,OOBase::String>& args);
	};

	struct render_call_info
	{
		Queue::callback_t m_fn;
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

static bool draw_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	OOBase::Vector<OOBase::WeakPtr<OOGL::Window>,OOBase::ThreadLocalAllocator> vecWindows;
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
	bool stop = false;
	OOBase::Clock draw_clock;
	float draw_rate = 15000.f;
	const float sixty_fps = (1.f / 60) * 1000000;
	OOBase::Timeout wait;
	while (res && !stop)
	{
		draw_clock.reset();

		// Draw all windows
		bool visible_window = false;
		for (OOBase::Vector<OOBase::WeakPtr<OOGL::Window>,OOBase::ThreadLocalAllocator>::iterator i=vecWindows.begin();i;)
		{
			if (i->expired())
				i = vecWindows.erase(i);
			else
			{
				if (i->lock()->draw())
					visible_window = true;
				++i;
			}
		}

		// Update animations


		// Accumulate average draw rate
		draw_rate = ((draw_rate*3) + draw_clock.microseconds()) / 4.f;
		if (draw_rate > 15000.f)
			draw_rate = 15000.f;

		wait.reset(0,sixty_fps - draw_rate);

		// Get render commands
		res = s_render_queue->dequeue(stop,wait);

		// Poll for UI events
		if (!visible_window)
			glfwWaitEvents();
		else
		{
			do
			{
				glfwPollEvents();
			}
			while (!wait.has_expired());

			// Swap all windows (this collects events)
			for (OOBase::Vector<OOBase::WeakPtr<OOGL::Window>,OOBase::ThreadLocalAllocator>::iterator i=vecWindows.begin();i;)
			{
				if (i->expired())
					i = vecWindows.erase(i);
				else
				{
					i->lock()->swap();
					++i;
				}
			}
		}
	}

	glfwTerminate();

	return res;
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
	if (render_queue.enqueue(NULL))
		s_event_queue->dequeue();

	return err;
}

bool Indigo::start_render_thread(bool (*logic_thread)(const OOBase::Table<OOBase::String,OOBase::String>& args), const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	OOBase::Event started(false,false);

#if !defined(NDEBUG)
	s_render_thread = OOBase::Thread::self();
#endif

	thread_info ti;
	ti.m_started = &started;
	ti.m_config = &config_args;
	ti.m_fn = logic_thread;

	// Force creation of event queue here
	EventQueue event_queue;
	s_event_queue = &event_queue;

	int err = 0;
	OOBase::SharedPtr<OOBase::Thread> logic = OOBase::Thread::run(&logic_thread_start,&ti,err);
	if (!logic)
		LOG_ERROR_RETURN(("Failed to start thread: %s",OOBase::system_error_text(err)),false);

	started.wait();

	// Now run the draw_thread (it must be the main thread)
	bool res = draw_thread(config_args);
	if (res)
		s_event_queue->enqueue(NULL,reinterpret_cast<void*>(1));

	logic->join();

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
#if !defined(NDEBUG)
	assert(s_render_thread != OOBase::Thread::self());
#endif

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

	ret->m_buffer->addref();
	OOBase::CDRStream stream(ret->m_buffer);
	(*ret->m_fn)(stream);

	return s_render_queue->enqueue(&raise_event_cleanup,ret);
}

bool Indigo::raise_event(void (*fn)(OOBase::CDRStream&), OOBase::CDRStream& stream)
{
#if !defined(NDEBUG)
	assert(s_render_thread == OOBase::Thread::self());
#endif

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

static bool do_quit_loop(void* p)
{
	return s_event_queue->enqueue(NULL,p);
}

bool Indigo::quit_loop()
{
	return render_call(&do_quit_loop,reinterpret_cast<void*>(1));
}

bool Indigo::monitor_window(const OOBase::WeakPtr<OOGL::Window>& win)
{
	return s_vecWindows->push_back(win);
}
