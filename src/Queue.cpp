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

bool Indigo::Queue::enqueue(callback_t callback, const OOBase::RefPtr<OOBase::Buffer>& buffer)
{
	OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);

	int err = m_queue.push(Item(callback,buffer));
	if (err)
		LOG_ERROR_RETURN(("Failed to enqueue command: %s",OOBase::system_error_text(err)),false);

	m_cond.signal();
	return true;
}

bool Indigo::Queue::dequeue()
{
	OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock,false);
	if (!guard.try_acquire())
		return true;

	return dequeue_i();
}

bool Indigo::Queue::dequeue_block(const OOBase::Timeout& timeout)
{
	OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);
	while (m_queue.empty())
	{
		if (!m_cond.wait(m_lock,timeout))
			return true;
	}

	return dequeue_i();
}

static bool release_buffer(OOBase::CDRStream& input)
{
	OOBase::Buffer* buf = NULL;
	if (!input.read(buf))
		LOG_ERROR_RETURN(("Failed to read free buffer: %s",OOBase::system_error_text(input.last_error())),false);

	if (buf)
		buf->release();

	return 0;
}

bool Indigo::Queue::dequeue_i()
{
	// Get next buffer
	for (Item item;m_queue.pop(&item);)
	{
		// Enqueue buffer relase
		if (item.m_buffer)
		{
			// Push cmd buffer into out queue (it's not ours to free)
			OOBase::CDRStream output;
			if (!output.write(item.m_buffer.addref()))
				LOG_ERROR_RETURN(("Failed to write buffer release message: %s",OOBase::system_error_text(output.last_error())),false);

			if (!Indigo::DRAW_QUEUE::instance().enqueue(&release_buffer,output.buffer()))
				return false;
		}

		OOBase::CDRStream input(item.m_buffer);
		if (!item.m_callback || !(*item.m_callback)(input))
			return false;
	}
	return true;
}
