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
#include "Thread.h"

#include "../src/App.h"

#include <stdlib.h>

namespace Indigo
{
	bool run_render_loop(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args);

	void render_init(Pipe* pipe);
}

namespace
{
	struct RenderModule
	{};

	typedef OOBase::TLSSingleton<OOBase::SharedPtr<Indigo::Pipe>,RenderModule> RENDER_PIPE;

	struct LogicModule
	{};

	typedef OOBase::TLSSingleton<OOBase::SharedPtr<Indigo::Pipe>,LogicModule> LOGIC_PIPE;
}

const OOBase::SharedPtr<Indigo::Pipe>& Indigo::render_pipe()
{
	OOBase::SharedPtr<Indigo::Pipe>& pipe = RENDER_PIPE::instance();
	if (!pipe)
		pipe = thread_pipe()->open("render");

	return pipe;
}

const OOBase::SharedPtr<Indigo::Pipe>& Indigo::logic_pipe()
{
	OOBase::SharedPtr<Indigo::Pipe>& pipe = LOGIC_PIPE::instance();
	if (!pipe)
		pipe = thread_pipe()->open("logic");

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
				logic_pipe->post(OOBase::make_delegate(&Indigo::Application::start),wnd,&options,&args);
		}
	}

	return logic_pipe;
}

bool Indigo::run_render_loop(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args)
{
	static const float monitor_refresh = 1000000.f / 60;

	// Create render comms pipe
	Indigo::Pipe pipe("render");
	render_init(&pipe);

	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

	// Set defaults
	glfwDefaultWindowHints();

	// Start the logic thread
	OOBase::WeakPtr<OOGL::Window> main_wnd;
	OOBase::SharedPtr<Indigo::Pipe> logic_pipe = start_logic_thread(pipe,main_wnd,options,args);
	if (logic_pipe)
	{
		for (;;)
		{
			OOBase::Clock draw_clock;

			OOBase::SharedPtr<OOGL::Window> wnd(main_wnd.lock());
			if (!wnd)
				break;

			if (wnd->visible() && !wnd->iconified())
			{
				// Update animations


				// Draw window
				wnd->draw();

				// Swap window
				wnd->swap();
			}

			// Poll for UI events
			glfwPollEvents();

			// Drain render commands
			pipe.drain();

			// If we have cycles spare, wait a bit
			if (draw_clock.microseconds() < monitor_refresh - 5000)
			{
				OOBase::Timeout wait(0,static_cast<unsigned int>(monitor_refresh - draw_clock.microseconds()));
				while (!wait.has_expired())
				{
					glfwPollEvents();

					if (!wait.has_expired())
						pipe.poll(OOBase::Timeout(0,1000));
				}
			}
		}

		logic_pipe->call(OOBase::make_delegate(&Application::stop));
	}

	glfwTerminate();

	return true;
}
