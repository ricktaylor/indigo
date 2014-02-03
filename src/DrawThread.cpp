///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 Rick Taylor
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
#include "Queue.h"

#include <GLFW/glfw3.h>

// Forward declare the windowing functions
bool have_windows();
bool render_windows();

static void on_glfw_error(int code, const char* message)
{
	OOBase::Logger::log(OOBase::Logger::Error,"GLFW error %d: %s",code,message);
}

static bool parse_command(const OOBase::RefPtr<OOBase::Buffer>& cmd_buffer, const OOBase::RefPtr<OOBase::Buffer>& event_buffer, bool& bStop)
{
	OOBase::CDRStream input(cmd_buffer);
	OOBase::CDRStream output(event_buffer);

	while (input.length())
	{
		bool (*callback)(OOBase::CDRStream& input, OOBase::CDRStream& output);
		if (!input.read(callback))
			LOG_ERROR_RETURN(("Failed to read callback: %s",OOBase::system_error_text(input.last_error())),false);

		if (!callback)
		{
			bStop = true;
			break;
		}

		if (!(*callback)(input,output))
			return false;
	}

	return true;
}

bool draw_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	Indigo::Queue& logic_queue = Indigo::LOGIC_QUEUE::instance();

	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

//	if (!Indigo::is_debug())
//		glfwSwapInterval(1);

	for (;;)
	{
		if (have_windows() ? !logic_queue.dequeue() : !logic_queue.dequeue_block())
			break;

		// Update animations

		// Render all windows (this collects events)
		if (!render_windows())
			break;


	}

	glfwTerminate();

	return true;
}
