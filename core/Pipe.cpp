///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
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
#include "Pipe.h"

namespace
{
	struct PipeTable
	{
		typedef OOBase::Table<OOBase::String,OOBase::SharedPtr<Indigo::detail::IPC::Queue> > table_t;

		table_t m_queues;
	};

	typedef OOBase::Singleton<PipeTable> PIPE_TABLE;
}

static OOBase::SharedPtr<Indigo::detail::IPC::Queue> register_queue(const char* name)
{
	OOBase::SharedPtr<Indigo::detail::IPC::Queue> queue;

	PipeTable::table_t::iterator i = PIPE_TABLE::instance().m_queues.find(name);
	if (i == PIPE_TABLE::instance().m_queues.end())
	{
		OOBase::String str;
		if (!str.assign(name))
			LOG_ERROR_RETURN(("Failed to assign string: %s",OOBase::system_error_text()),queue);

		queue = OOBase::allocate_shared<Indigo::detail::IPC::Queue>();
		if (!queue)
			LOG_ERROR_RETURN(("Failed to allocate queue: %s",OOBase::system_error_text()),queue);

		i = PIPE_TABLE::instance().m_queues.insert(str,queue);
		if (i == PIPE_TABLE::instance().m_queues.end())
		{
			LOG_ERROR(("Failed to insert queue: %s",OOBase::system_error_text()));
			queue.reset();
		}
	}
	return i->second;
}

bool Indigo::detail::IPC::Queue::enqueue(callback_t callback, void* param)
{
	OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);

	if (!m_queue.push(Item(callback,param)))
		LOG_ERROR_RETURN(("Failed to enqueue command: %s",OOBase::system_error_text()),false);

	m_cond.signal();
	return true;
}

bool Indigo::detail::IPC::Queue::drain()
{
	OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock);

	// Get next buffer
	for (Item item;m_queue.pop(&item);)
	{
		guard.release();

		if (!(*item.m_callback)(item.m_param))
			return false;

		guard.acquire();
	}

	return true;
}

bool Indigo::detail::IPC::Queue::dequeue(bool call_blocked, const OOBase::Timeout& timeout)
{
	for (;;)
	{
		OOBase::Guard<OOBase::Condition::Mutex> guard(m_lock,false);
		if (!guard.acquire(timeout))
			return true;

		while (m_queue.empty())
		{
			if (!m_cond.wait(m_lock,timeout))
				return true;
		}

		// Get next buffer
		for (Item item;m_queue.pop(&item);)
		{
			guard.release();

			if (!item.m_callback)
				return call_blocked;

			if (!(*item.m_callback)(item.m_param))
				return false;

			if (!guard.acquire(timeout))
				return true;
		}
	}
	return false;
}

Indigo::Pipe::Pipe(const char* name)
{
	m_recv_queue = register_queue(name);
}

Indigo::Pipe::Pipe(const OOBase::SharedPtr<detail::IPC::Queue>& send_queue, const OOBase::SharedPtr<detail::IPC::Queue>& recv_queue) :
		m_recv_queue(recv_queue),
		m_send_queue(send_queue)
{
}

OOBase::SharedPtr<Indigo::Pipe> Indigo::Pipe::open(const char* remote)
{
	OOBase::SharedPtr<Indigo::Pipe> pipe;
	if (!m_recv_queue)
		LOG_ERROR_RETURN(("Can't open closed IPC pipe"),pipe);

	PipeTable::table_t::iterator i = PIPE_TABLE::instance().m_queues.find(remote);
	if (i == PIPE_TABLE::instance().m_queues.end())
		LOG_ERROR_RETURN(("Failed to find remote IPC pipe"),pipe);

	pipe = OOBase::allocate_shared<Indigo::Pipe,OOBase::ThreadLocalAllocator>(i->second,m_recv_queue);
	if (!pipe)
		LOG_ERROR(("Failed to allocate pipe: %s",OOBase::system_error_text()));

	return pipe;
}

void Indigo::Pipe::close()
{
	// Send a close to ourselves
	if (m_recv_queue)
		m_recv_queue->enqueue(NULL,NULL);
}

bool Indigo::Pipe::make_call(void* param)
{
	CallInfo* ci = static_cast<CallInfo*>(param);

	(*ci->m_fn)(ci->m_param);

	return ci->m_reply->enqueue(NULL,NULL);
}

bool Indigo::Pipe::call(void (*fn)(void*), void* param)
{
	if (!m_send_queue || !m_recv_queue)
		return false;

	CallInfo ci;
	ci.m_reply = m_recv_queue;
	ci.m_fn = fn;
	ci.m_param = param;

	if (!m_send_queue->enqueue(&Pipe::make_call,&ci))
		return false;

	return m_recv_queue->dequeue(true);
}

bool Indigo::Pipe::post_cleanup(void* param)
{
	CallInfo* ci = static_cast<CallInfo*>(param);

	if (ci->m_fn_cleanup)
		(*ci->m_fn_cleanup)(ci->m_param);

	OOBase::ThreadLocalAllocator::delete_free(static_cast<CallInfo*>(param));
	return true;
}

bool Indigo::Pipe::do_post(void* param)
{
	CallInfo* ci = static_cast<CallInfo*>(param);

	(*ci->m_fn)(ci->m_param);

	return ci->m_reply->enqueue(&Pipe::post_cleanup,ci);
}

bool Indigo::Pipe::post(void (*fn)(void*), void* param, void (*fn_cleanup)(void*))
{
	if (!m_send_queue || !m_recv_queue)
		return false;

	CallInfo* ci;
	if (!OOBase::ThreadLocalAllocator::allocate_new(ci))
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	ci->m_reply = m_recv_queue;
	ci->m_fn = fn;
	ci->m_fn_cleanup = fn_cleanup;
	ci->m_param = param;

	if (!m_send_queue->enqueue(&Pipe::do_post,ci))
	{
		OOBase::ThreadLocalAllocator::delete_free(ci);
		return false;
	}

	return true;
}

bool Indigo::Pipe::poll(const OOBase::Timeout& timeout)
{
	return m_recv_queue && m_recv_queue->dequeue(false,timeout);
}

bool Indigo::Pipe::drain()
{
	return m_recv_queue && m_recv_queue->drain();
}