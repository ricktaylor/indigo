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

#include "Common.h"

namespace Indigo
{
	class Queue
	{
	public:
		typedef bool (*callback_t)(OOBase::CDRStream& input);

		bool enqueue(callback_t callback, const OOBase::RefPtr<OOBase::Buffer>& buffer = OOBase::RefPtr<OOBase::Buffer>());
		bool dequeue();
		bool dequeue_block(const OOBase::Timeout& timeout = OOBase::Timeout());

	private:
		OOBase::Condition::Mutex m_lock;
		OOBase::Condition        m_cond;

		struct Item
		{
			Item() : m_callback(), m_buffer()
			{}

			Item(callback_t c, const OOBase::RefPtr<OOBase::Buffer>& b) : m_callback(c), m_buffer(b)
			{}

			callback_t m_callback;
			OOBase::RefPtr<OOBase::Buffer> m_buffer;
		};

		OOBase::Queue<Item,OOBase::ThreadLocalAllocator> m_queue;

		bool dequeue_i();
	};

	namespace detail
	{
		class LogicQueue : public Queue
		{
		};

		class DrawQueue : public Queue
		{
		};
	}

	typedef OOBase::Singleton<detail::LogicQueue> LOGIC_QUEUE;
	typedef OOBase::Singleton<detail::DrawQueue> DRAW_QUEUE;
}

#endif // INDIGO_QUEUE_H_INCLUDED
