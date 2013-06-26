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

// This points to the current out cmd buffer
static OOBase::Buffer** s_pcmd_buffer = NULL;


// Change this to a send_and_receive model...

static bool parse_command(OOBase::Buffer* cmd_buffer)
{
	for (OOBase::CDRStream input(cmd_buffer);input.length();)
	{
		OOBase::uint8_t op_code;
		if (!input.read(op_code))
			LOG_ERROR_RETURN(("Failed to read op_code: %s",OOBase::system_error_text(input.last_error())),false);

		switch (op_code)
		{
		case 0:	// Abort
			return false;

		case 1:
			{
				// Free buffer...
				OOBase::Buffer* buf = NULL;
				if (!input.read(buf))
					LOG_ERROR_RETURN(("Failed to read op_code: %s",OOBase::system_error_text(input.last_error())),false);

				buf->release();
			}
			break;

		default:
			LOG_ERROR_RETURN(("Invalid op_code: %u",op_code),false);
		}
	}
	return true;
}

bool logic_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args, Indigo::Queue& draw_queue, Indigo::Queue& logic_queue)
{
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
		if (draw_queue.dequeue_block(cmd_buffer,err))
		{
			// Parse command block
			if (!parse_command(cmd_buffer))
				return false;
		}
		else if (err)
			LOG_ERROR_RETURN(("Failed to dequeue draw packet: %s",OOBase::system_error_text(err)),false);

		if (cmd_buffer)
		{
			// Push cmd block into out queue (it's not ours to free)
			OOBase::CDRStream output(*s_pcmd_buffer);
			if (!output.write(OOBase::uint8_t(1)) || !output.write(cmd_buffer))
				LOG_ERROR_RETURN(("Failed to write message: %s",OOBase::system_error_text(output.last_error())),false);
		}

		// If we have a command buffer, enqueue it...
		if ((*s_pcmd_buffer)->length() > 0)
		{
			err = logic_queue.enqueue(*s_pcmd_buffer);
			if (err)
				LOG_ERROR(("Failed to enqueue command: %s",OOBase::system_error_text(err)));
			else
				*s_pcmd_buffer = NULL;
		}
	}

	return true;
}
