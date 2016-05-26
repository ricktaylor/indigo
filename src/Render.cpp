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

#include "Common.h"
#include "Render.h"
#include "Window.h"
#include "Thread.h"

namespace Indigo
{
	void render_init(Pipe* pipe);

	bool run(const char* name, void (*fn)(void*), void* param);
}

namespace
{
	struct RenderModule
	{};

	typedef OOBase::TLSSingleton<OOBase::SharedPtr<Indigo::Pipe>,RenderModule> RENDER_PIPE;

	struct LogicModule
	{};

	typedef OOBase::TLSSingleton<OOBase::SharedPtr<Indigo::Pipe>,LogicModule> LOGIC_PIPE;
}

const OOBase::SharedPtr<Indigo::Pipe>& Indigo::render_pipe()
{
	OOBase::SharedPtr<Indigo::Pipe>& pipe = RENDER_PIPE::instance();
	if (!pipe)
		pipe = thread_pipe()->open("render");

	return pipe;
}

const OOBase::SharedPtr<Indigo::Pipe>& Indigo::logic_pipe()
{
	OOBase::SharedPtr<Indigo::Pipe>& pipe = LOGIC_PIPE::instance();
	if (!pipe)
		pipe = thread_pipe()->open("logic");

	return pipe;
}

static void on_glfw_error(int code, const char* message)
{
	OOBase::Logger::log(OOBase::Logger::Error,"GLFW error %d: %s",code,message);
}

bool Indigo::run(const char* name, void (*fn)(void*), void* param)
{
	// Create render comms pipe
	Indigo::Pipe pipe("render");
	render_init(&pipe);

	// Not sure if we need to set this first...
	glfwSetErrorCallback(&on_glfw_error);

	if (!glfwInit())
		LOG_ERROR_RETURN(("glfwInit failed"),false);

	// Set defaults
	glfwDefaultWindowHints();

	bool ret = false;
	OOBase::SharedPtr<OOBase::Thread> thread;
	OOBase::SharedPtr<Indigo::Pipe> logic_pipe = Indigo::start_thread(name,thread);
	if (logic_pipe)
	{
		ret = logic_pipe->call(OOBase::make_delegate(fn),param);
		if (ret)
			thread->join();
	}

	glfwTerminate();

	return ret;
}
