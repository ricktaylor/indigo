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
#include "Window.h"
#include "App.h"
#include "Thread.h"

#include <stdlib.h>

namespace Indigo
{
	bool run_render_loop(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args);

	void render_init(Pipe* const pipe);
}

namespace
{
	typedef OOBase::TLSSingleton<OOBase::SharedPtr<Indigo::Pipe> > RENDER_PIPE;
}

const OOBase::SharedPtr<Indigo::Pipe>& Indigo::render_pipe()
{
	OOBase::SharedPtr<Indigo::Pipe>& pipe = RENDER_PIPE::instance();
	if (!pipe)
		pipe = thread_pipe()->open("render");

	return pipe;
}

static void on_glfw_error(int code, const char* message)
{
	OOBase::Logger::log(OOBase::Logger::Error,"GLFW error %d: %s",code,message);
}

static OOBase::SharedPtr<Indigo::Pipe> start_logic_thread(Indigo::Pipe& pipe, OOBase::WeakPtr<OOGL::Window>& main_wnd, const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args)
{
	OOBase::SharedPtr<Indigo::Pipe> logic_pipe;

	// Create the frame window
	OOBase::SharedPtr<Indigo::Window> wnd = OOBase::allocate_shared<Indigo::Window>();
	if (!wnd)
		LOG_ERROR(("Failed to create window: %s",OOBase::system_error_text()));
	else
	{
		main_wnd = wnd->create();
		if (main_wnd)
		{
			logic_pipe = Indigo::start_thread("logic");
			if (logic_pipe)
				logic_pipe->call(OOBase::make_delegate(Indigo::App::instance_ptr(),&Indigo::Application::start),wnd,&options,&args);
		}
	}

	return logic_pipe;
}

bool Indigo::run_render_loop(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args)
{
	static const float sixty_fps = (1.f / 60) * 1000000;

	// Create render comms pipe
	Indigo::Pipe pipe("render");
	render_init(&pipe);

	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

	// Set defaults
	glfwDefaultWindowHints();

	if (!Indigo::is_debug())
		glfwSwapInterval(1);

	// Start the logic thread
	OOBase::WeakPtr<OOGL::Window> main_wnd;
	OOBase::SharedPtr<Indigo::Pipe> logic_pipe = start_logic_thread(pipe,main_wnd,options,args);
	if (logic_pipe)
	{
		for (float draw_rate = 15000.f;;)
		{
			OOBase::Clock draw_clock;
			OOBase::SharedPtr<OOGL::Window> wnd(main_wnd.lock());
			if (!wnd)
				break;

			// Update animations


			// Draw window
			wnd->draw();

			// Swap window (this collects events)
			wnd->swap();

			// Accumulate average draw rate
			draw_rate = ((draw_rate*3) + draw_clock.microseconds()) / 4.f;
			if (draw_rate > 15000.f)
				draw_rate = 15000.f;

			for (OOBase::Timeout wait(0,static_cast<unsigned int>(sixty_fps - draw_rate));!wait.has_expired();)
			{
				// Poll for UI events
				glfwPollEvents();

				// Drain render commands
				pipe.drain();
			}
		}

		logic_pipe->call(OOBase::make_delegate(Indigo::App::instance_ptr(),&Indigo::Application::stop));
	}

	glfwTerminate();

	return true;
}

/*

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
*/
