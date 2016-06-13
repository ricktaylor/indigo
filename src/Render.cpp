///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#include "../include/indigo/Render.h"
#include "../include/indigo/Window.h"
#include "../include/indigo/Thread.h"

#include "Common.h"

namespace Indigo
{
	void render_init(Pipe* pipe);

	bool run(const char* name, void (*fn)(void*), void* param);
}

namespace
{
	struct PipeRack
	{
		OOBase::SharedPtr<Indigo::Pipe> m_render_pipe;
		OOBase::SharedPtr<Indigo::Pipe> m_logic_pipe;
	};

	typedef OOBase::TLSSingleton<PipeRack> PIPE_RACK;
}

const OOBase::SharedPtr<Indigo::Pipe>& Indigo::render_pipe()
{
	OOBase::SharedPtr<Indigo::Pipe>& pipe = PIPE_RACK::instance().m_render_pipe;
	if (!pipe)
		pipe = thread_pipe()->open("render");

	return pipe;
}

const OOBase::SharedPtr<Indigo::Pipe>& Indigo::logic_pipe()
{
	OOBase::SharedPtr<Indigo::Pipe>& pipe = PIPE_RACK::instance().m_logic_pipe;
	if (!pipe)
		pipe = thread_pipe()->open("logic");

	return pipe;
}

#if !defined(NDEBUG)
void Indigo::assert_render_thread()
{
	assert(render_pipe()->is_local());
}
#endif

static void on_glfw_error(int code, const char* message)
{
	OOBase::Logger::log(OOBase::Logger::Error,"GLFW error %d: %s",code,message);
}

bool Indigo::run(const char* name, void (*fn)(void*), void* param)
{
	// Create render comms pipe
	PIPE_RACK::instance().m_render_pipe =  OOBase::allocate_shared<Indigo::Pipe,OOBase::ThreadLocalAllocator>("render");
	if (!PIPE_RACK::instance().m_render_pipe)
		LOG_ERROR_RETURN(("Failed to allocate render pipe: %s",OOBase::system_error_text()),false);

	render_init(PIPE_RACK::instance().m_render_pipe.get());

	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

	// Set defaults
	glfwDefaultWindowHints();

	bool ret = false;
	OOBase::SharedPtr<OOBase::Thread> thread;
	PIPE_RACK::instance().m_logic_pipe = Indigo::start_thread(name,thread);
	if (PIPE_RACK::instance().m_logic_pipe)
	{
		ret = PIPE_RACK::instance().m_logic_pipe->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(fn),param);
		if (ret)
			thread->join();
	}

	glfwTerminate();

	return ret;
}
