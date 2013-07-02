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

int Indigo::Queue::enqueue(OOBase::Buffer* buffer)
{
	int err = 0;
	if (buffer)
	{
		OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);

		err = m_queue.push(buffer);
		if (!err)
			m_cond.signal();
	}

	return err;
}

bool Indigo::Queue::dequeue(OOBase::Buffer*& buffer, int& err)
{
	OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock,false);
	if (guard.try_acquire() && !m_queue.empty())
	{
		err = m_queue.pop(&buffer);
		return true;
	}
	return false;
}

bool Indigo::Queue::dequeue_block(OOBase::Buffer*& buffer, int& err, const OOBase::Timeout& timeout)
{
	OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);
	while (m_queue.empty())
	{
		if (!m_cond.wait(m_lock,timeout))
			return false;
	}

	err = m_queue.pop(&buffer);
	return true;
}
