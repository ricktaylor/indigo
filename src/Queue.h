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

#ifndef INDIGO_QUEUE_H_INCLUDED
#define INDIGO_QUEUE_H_INCLUDED

namespace Indigo
{
	class Queue
	{
	public:
		Queue(OOBase::AllocatorInstance& allocator);

		int enqueue(OOBase::Buffer* buffer);
		bool dequeue(OOBase::Buffer*& buffer, int& err);
		bool dequeue_block(OOBase::Buffer*& buffer, int& err, const OOBase::Timeout& timeout = OOBase::Timeout());

	private:
		OOBase::Condition::Mutex m_lock;
		OOBase::Condition        m_cond;

		OOBase::Queue<OOBase::Buffer*,OOBase::AllocatorInstance> m_queue;
	};
}

#endif // INDIGO_QUEUE_H_INCLUDED
