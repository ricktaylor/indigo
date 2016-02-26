///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// OOGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OOGL.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#include "Thread.h"

namespace Indigo
{
	void render_init(Pipe* pipe);
}

namespace
{
	struct ThreadStart
	{
		OOBase::Event m_started;
		const char* m_name;
	};

	typedef OOBase::TLSSingleton<Indigo::Pipe*> THREAD_PIPE;
}

void Indigo::render_init(Pipe* pipe)
{
	THREAD_PIPE::instance() = pipe;
}

Indigo::Pipe* const Indigo::thread_pipe()
{
	return THREAD_PIPE::instance();
}

static int thread_start(void* param)
{
	ThreadStart* ts = static_cast<ThreadStart*>(param);

	// Create render comms pipe
	Indigo::Pipe pipe(ts->m_name);
	THREAD_PIPE::instance() = &pipe;

	ts->m_started.set();

	pipe.poll();

	return 0;
}

OOBase::SharedPtr<Indigo::Pipe> Indigo::start_thread(const char* name)
{
	OOBase::SharedPtr<Indigo::Pipe> pipe;

	ThreadStart ts;
	ts.m_name = name;

	int err = 0;
	OOBase::SharedPtr<OOBase::Thread> thread = OOBase::Thread::run(&thread_start,&ts,err);
	if (!thread)
		LOG_ERROR_RETURN(("Failed to start thread: %s",OOBase::system_error_text(err)),pipe);

	ts.m_started.wait();

	pipe = THREAD_PIPE::instance()->open("logic");
	if (!pipe)
		thread->abort();

	return pipe;
}

