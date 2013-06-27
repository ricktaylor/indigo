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
#include "Protocol.h"

#include <GLFW/glfw3.h>

// Forward declare the windowing functions
bool have_windows();
bool handle_event(OOBase::CDRStream& input, OOBase::CDRStream& output);
bool render_windows(OOBase::CDRStream& output);

static void on_glfw_error(int code, const char* message)
{
	LOG_ERROR(("GLFW error %d: %s",code,message));
}

static bool parse_command(OOBase::Buffer* cmd_buffer, OOBase::Buffer* event_buffer, bool& bStop)
{
	OOBase::CDRStream input(cmd_buffer);
	OOBase::CDRStream output(event_buffer);

	while (input.length())
	{
		Indigo::Protocol::Request_t op_code;
		if (!input.read(op_code))
			LOG_ERROR_RETURN(("Failed to read op_code: %s",OOBase::system_error_text(input.last_error())),false);

		switch (op_code)
		{
		case Indigo::Protocol::Request::Quit:
			bStop = true;
			break;

		case Indigo::Protocol::Request::ReleaseBuffer:
			{
				OOBase::Buffer* buf = NULL;
				if (!input.read(buf))
					LOG_ERROR_RETURN(("Failed to read free buffer: %s",OOBase::system_error_text(input.last_error())),false);

				buf->release();
			}
			break;

		case Indigo::Protocol::Request::WindowMsg:
			if (!handle_event(input,output))
				return false;
			break;

		default:
			LOG_ERROR_RETURN(("Invalid op_code: %u",op_code),false);
		}
	}
	return true;
}

bool draw_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args, Indigo::Queue& draw_queue, Indigo::Queue& logic_queue)
{
	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

	if (!Indigo::is_debug())
		glfwSwapInterval(1);

	OOBase::RefPtr<OOBase::Buffer> event_buffer;
	for (bool bStop = false;!bStop;)
	{
		if (!event_buffer)
		{
			event_buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>(OOBase::CDRStream::MaxAlignment);
			if (!event_buffer)
				LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}
		OOBase::CDRStream output(event_buffer);

		// Get next cmd block from in queue
		int err = 0;
		OOBase::Buffer* cmd_buffer = NULL;
		if (have_windows() ? logic_queue.dequeue_block(cmd_buffer,err) : logic_queue.dequeue(cmd_buffer,err))
		{
			// Parse command block
			if (!parse_command(cmd_buffer,event_buffer,bStop))
				return false;
		}
		else if (err)
			LOG_ERROR_RETURN(("Failed to dequeue logic packet: %s",OOBase::system_error_text(err)),false);

		// Update animations

		// Render all windows (this collects events)
		if (!render_windows(output))
			return false;

		if (cmd_buffer)
		{
			// Push cmd block into out queue (it's not ours to free)
			if (!output.write(Indigo::Protocol::Request_t(Indigo::Protocol::Request::ReleaseBuffer)) || !output.write(cmd_buffer))
				LOG_ERROR_RETURN(("Failed to write message: %s",OOBase::system_error_text(output.last_error())),false);
		}

		// If we have a command buffer, enqueue it...
		if (event_buffer->length() > 0)
		{
			err = draw_queue.enqueue(event_buffer);
			if (err)
				LOG_ERROR(("Failed to enqueue command: %s",OOBase::system_error_text(err)));
			else
			{
				event_buffer->addref();
				event_buffer = NULL;
			}
		}
	}

	glfwTerminate();

	return true;
}
