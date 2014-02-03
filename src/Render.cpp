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

#include "Common.h"
#include "Render.h"

#include <stdlib.h>

#include <GLFW/glfw3.h>

// Forward declare the windowing functions
bool have_windows();
bool render_windows();

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
						return true;
				}

				for (Item item;m_queue.pop(&item);)
				{
					guard.release();

					// NULL callback means response
					if (!item.m_callback)
						return true;

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

	typedef OOBase::Singleton<EventQueue> EVENT_QUEUE;
	typedef OOBase::Singleton<RenderQueue> RENDER_QUEUE;

	struct thread_info
	{
		OOBase::Event* m_started;
		const OOBase::Table<OOBase::String,OOBase::String>* m_config;
		bool (*m_fn)(const OOBase::Table<OOBase::String,OOBase::String>& args);
	};
}

static void on_glfw_error(int code, const char* message)
{
	OOBase::Logger::log(OOBase::Logger::Error,"GLFW error %d: %s",code,message);
}

static bool draw_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	// Get the render_queue instance up front
	RenderQueue& render_queue = RENDER_QUEUE::instance();

	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

//	if (!Indigo::is_debug())
//		glfwSwapInterval(1);

	for (;;)
	{
		// Get render commands
		if (!(have_windows() ? render_queue.dequeue() : render_queue.dequeue_block()))
			break;

		// Update animations

		// Render all windows (this collects events)
		if (!render_windows())
			break;
	}

	// Some kind of cleanup?

	glfwTerminate();

	return true;
}

static bool stop_thread(void*)
{
	return false;
}

static int logic_thread_start(void* param)
{
	thread_info ti = *reinterpret_cast<thread_info*>(param);

	// Force creation of render queue here
	RENDER_QUEUE::instance();

	// Signal we have started
	ti.m_started->set();
	ti.m_started = NULL;

	// Run the logic loop
	int err = (*ti.m_fn)(*ti.m_config) ? EXIT_SUCCESS : EXIT_FAILURE;

	// Clean out the event queue
	EVENT_QUEUE::instance().dequeue();

	// Tell the render thread we have finished
	RENDER_QUEUE::instance().enqueue(&stop_thread);

	return err;
}

bool start_render_thread(bool (*logic_thread)(const OOBase::Table<OOBase::String,OOBase::String>& args), const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	OOBase::Event started(false,false);

	thread_info ti;
	ti.m_started = &started;
	ti.m_config = &config_args;
	ti.m_fn = logic_thread;

	// Force creation of event queue here
	EVENT_QUEUE::instance();

	OOBase::Thread logic(false);
	int err = logic.run(&logic_thread_start,&ti);
	if (err)
		LOG_ERROR_RETURN(("Failed to start thread: %s",OOBase::system_error_text(err)),false);

	started.wait();

	// Now run the draw_thread (it must be the main thread)
	bool res = draw_thread(config_args);

	// Send an abort to the logic_thread
	EVENT_QUEUE::instance().enqueue(&stop_thread);

	// Wait for logic thread to end
	logic.join();

	return res;
}

bool render_call(bool (*fn)(void*), void* param)
{
	if (!RENDER_QUEUE::instance().enqueue(fn,param))
		return false;

	return EVENT_QUEUE::instance().dequeue();
}

bool raise_event(bool (*fn)(void*), void* param)
{
	return EVENT_QUEUE::instance().enqueue(fn,param);
}
