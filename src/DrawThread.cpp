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

#include <GLFW/glfw3.h>

// This points to the current out cmd buffer
static OOBase::Buffer** s_pcmd_buffer = NULL;

// The vector of windows
static OOBase::Vector<GLFWwindow*,OOBase::AllocatorInstance> s_vecWindows(OOBase::ThreadLocalAllocator::instance());


static void on_glfw_error(int code, const char* message)
{
	LOG_ERROR(("GLFW error %d: %s",code,message));
}

static bool parse_command(OOBase::Buffer* cmd_buffer)
{
	for (OOBase::CDRStream input(cmd_buffer);input.length();)
	{
		OOBase::uint8_t op_code;
		if (!input.read(op_code))
			LOG_ERROR_RETURN(("Failed to read op_code: %s",OOBase::system_error_text(input.last_error())),false);

		switch (op_code)
		{
		case 0: // Quit...
			break;

		default:
			LOG_ERROR_RETURN(("Invalid op_code: %u",op_code),false);
		}
	}
	return true;
}

bool Indigo::draw_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args, Queue& draw_queue, Queue& logic_queue)
{
	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

	for (bool bStop = false; !bStop;)
	{
		if (!*s_pcmd_buffer)
		{
			*s_pcmd_buffer = OOBase::Buffer::create(OOBase::ThreadLocalAllocator::instance(),OOBase::CDRStream::MaxAlignment);
			if (!*s_pcmd_buffer)
				LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}

		// Get next cmd block from in queue
		int err = 0;
		OOBase::Buffer* cmd_buffer = NULL;
		bool have_msg = s_vecWindows.empty() ? logic_queue.dequeue_block(cmd_buffer,err) : logic_queue.dequeue(cmd_buffer,err);
		if (have_msg)
		{
			// Parse command block
			if (!parse_command(cmd_buffer))
				return false;
		}
		else if (err)
			LOG_ERROR_RETURN(("Failed to dequeue logic packet: %s",OOBase::system_error_text(err)),false);

		// Update animations

		if (!s_vecWindows.empty())
		{
			for (OOBase::Vector<GLFWwindow*,OOBase::AllocatorInstance>::iterator i=s_vecWindows.begin();i!=s_vecWindows.end();++i)
			{
				// for each camera

				// Cull
				// Sort
				// Draw

				glfwSwapBuffers(*i);
			}

			glfwPollEvents();
		}

		if (cmd_buffer)
		{
			// Push cmd block into out queue (it's not ours to free)

		}

		// If we have a command buffer, enqueue it...
		if ((*s_pcmd_buffer)->length() > 0)
		{
			err = draw_queue.enqueue(*s_pcmd_buffer);
			if (err)
				LOG_ERROR(("Failed to enqueue command: %s",OOBase::system_error_text(err)));
			else
				*s_pcmd_buffer = NULL;
		}
	}

	glfwTerminate();

	return true;
}
