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

#include <GLFW/glfw3.h>

#include "Window.h"

namespace
{
	class RenderWindow
	{
	public:
		RenderWindow(OOBase::uint16_t id, GLFWwindow* w);

		void swap_buffers();

	private:
		OOBase::uint16_t m_id;
		GLFWwindow*      m_glfw_window;

		static void on_pos(GLFWwindow* window, int xpos, int ypos);
		static void on_size(GLFWwindow* window, int width, int height);
		static void on_close(GLFWwindow* window);
		static void on_focus(GLFWwindow* window, int focused);
		static void on_iconify(GLFWwindow* window, int iconified);
	};
}

static OOBase::HashTable<OOBase::uint16_t,RenderWindow*,OOBase::ThreadLocalAllocator> s_mapRenderWindows;

static OOBase::CDRStream* s_event_stream = NULL;

RenderWindow::RenderWindow(OOBase::uint16_t id, GLFWwindow* win) : m_id(id), m_glfw_window(win)
{
	glfwSetWindowUserPointer(win,this);
	glfwSetWindowPosCallback(win,&on_pos);
	glfwSetWindowSizeCallback(win,&on_size);
	glfwSetWindowCloseCallback(win,&on_close);
	glfwSetWindowFocusCallback(win,&on_focus);
	glfwSetWindowIconifyCallback(win,&on_iconify);
}

void RenderWindow::swap_buffers()
{
	glfwSwapBuffers(m_glfw_window);
}

void RenderWindow::on_pos(GLFWwindow* window, int xpos, int ypos)
{
	/*RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	s_event_stream->write(Indigo::Protocol::Response_t(Indigo::Protocol::Response::WindowMsg));
	s_event_stream->write(WindowEvent_t(WindowEvent::PositionChanged));
	s_event_stream->write(pThis->m_id);
	s_event_stream->write(xpos);
	s_event_stream->write(ypos);*/
}

void RenderWindow::on_size(GLFWwindow* window, int width, int height)
{
	/*RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	s_event_stream->write(Indigo::Protocol::Response_t(Indigo::Protocol::Response::WindowMsg));
	s_event_stream->write(WindowEvent_t(WindowEvent::SizeChanged));
	s_event_stream->write(pThis->m_id);
	s_event_stream->write(width);
	s_event_stream->write(height);*/
}

void RenderWindow::on_close(GLFWwindow* window)
{
	/*RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	s_event_stream->write(Indigo::Protocol::Response_t(Indigo::Protocol::Response::WindowMsg));
	s_event_stream->write(WindowEvent_t(WindowEvent::CloseRequest));
	s_event_stream->write(pThis->m_id);*/
}

void RenderWindow::on_focus(GLFWwindow* window, int focused)
{
	/*RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	s_event_stream->write(Indigo::Protocol::Response_t(Indigo::Protocol::Response::WindowMsg));
	s_event_stream->write(WindowEvent_t(WindowEvent::FocusChanged));
	s_event_stream->write(pThis->m_id);
	s_event_stream->write(focused);*/
}

void RenderWindow::on_iconify(GLFWwindow* window, int iconified)
{
	/*RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
	s_event_stream->write(Indigo::Protocol::Response_t(Indigo::Protocol::Response::WindowMsg));
	s_event_stream->write(WindowEvent_t(WindowEvent::Iconified));
	s_event_stream->write(pThis->m_id);
	s_event_stream->write(iconified);*/
}

static bool return_error(OOBase::CDRStream& output, int err)
{
	if (!output.write(err))
		LOG_ERROR_RETURN(("Failed to write response: %s",OOBase::system_error_text(output.last_error())),false);
	return true;
}

static bool create_window(OOBase::CDRStream& input, OOBase::CDRStream& output)
{
	OOBase::uint16_t window_id = 0;

	input.read(window_id);
	if (input.last_error())
		LOG_ERROR_RETURN(("Failed to read request: %s",OOBase::system_error_text(input.last_error())),false);

	// Check if we already have a window with this id
	if (s_mapRenderWindows.exists(window_id))
		return return_error(output,EINVAL);

	// Set defaults
	glfwDefaultWindowHints();

	// Set up the window hints
	size_t hints = 0;
	if (!input.read(hints))
		LOG_ERROR_RETURN(("Failed to read request: %s",OOBase::system_error_text(input.last_error())),false);
	while (hints--)
	{
		int target,hint;
		if (!input.read(target) || !input.read(hint))
			LOG_ERROR_RETURN(("Failed to read request: %s",OOBase::system_error_text(input.last_error())),false);

		glfwWindowHint(target,hint);
	}

	// Fullscreen?
	bool fullscreen = false;
	if (!input.read(fullscreen))
		LOG_ERROR_RETURN(("Failed to read request: %s",OOBase::system_error_text(input.last_error())),false);

	GLFWmonitor* monitor = NULL;
	if (fullscreen)
		monitor = glfwGetPrimaryMonitor();

	OOBase::LocalString strTitle(OOBase::ThreadLocalAllocator::instance());
	if (!input.read_string(strTitle))
		LOG_ERROR_RETURN(("Failed to read request: %s",OOBase::system_error_text(input.last_error())),false);

	int width,height;
	if (!input.read(width) || !input.read(height))
		LOG_ERROR_RETURN(("Failed to read request: %s",OOBase::system_error_text(input.last_error())),false);

	// Now try to create the window
	GLFWwindow* win = glfwCreateWindow(width,height,strTitle.c_str(),monitor,NULL);
	if (!win)
		return return_error(output,EINVAL);

	RenderWindow* rwin = NULL;
	if (!OOBase::ThreadLocalAllocator::allocate_new(rwin,window_id,win))
	{
		glfwDestroyWindow(win);
		return return_error(output,ERROR_OUTOFMEMORY);
	}

	return return_error(output,0);
}

bool have_windows()
{
	return !s_mapRenderWindows.empty();
}

bool render_windows(OOBase::CDRStream& output)
{
	for (OOBase::HashTable<OOBase::uint16_t,RenderWindow*,OOBase::ThreadLocalAllocator>::iterator i=s_mapRenderWindows.begin();i!=s_mapRenderWindows.end();++i)
	{
		// for each camera

		// Cull
		// Sort
		// Draw

		i->value->swap_buffers();
	}

	// Stash the event stream so the callbacks can update it
	s_event_stream = &output;

	glfwPollEvents();

	if (output.last_error())
		LOG_ERROR_RETURN(("Failed to write events to queue: %s",OOBase::system_error_text(output.last_error())),false);

	return true;
}
