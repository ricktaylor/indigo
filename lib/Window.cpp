///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 Rick Taylor
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

#include "Window.h"

#include <OOBase/Logger.h>

OOGL::Window::Window(int width, int height, const char* title, unsigned int style, GLFWmonitor* monitor) :
		m_glfw_window(NULL)
{
	glfwWindowHint(GLFW_VISIBLE,(style & eWSvisible) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE,(style & eWSresizable) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_DECORATED,(style & eWSdecorated) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,(style & eWSdebug_context) ? GL_TRUE : GL_FALSE);

	// Now try to create the window
	m_glfw_window = glfwCreateWindow(width,height,title,monitor,NULL);
	if (!m_glfw_window)
		LOG_ERROR(("Failed to create window"));
	else
	{
		glfwMakeContextCurrent(m_glfw_window);

		m_state_fns = OOBase::allocate_shared<StateFns,OOBase::ThreadLocalAllocator>();
		if (!m_state_fns)
			LOG_ERROR(("Failed to allocate GL state functions object"));
		else
		{
			m_state = OOBase::allocate_shared<State,OOBase::ThreadLocalAllocator>(OOBase::Ref<StateFns>(*m_state_fns.get()));
			if (!m_state)
				LOG_ERROR(("Failed to allocate GL state object"));
			else
			{
				m_default_fb = Framebuffer::get_default();

				glfwSetWindowUserPointer(m_glfw_window,this);
				glfwSetFramebufferSizeCallback(m_glfw_window,&cb_on_size);
				glfwSetWindowPosCallback(m_glfw_window,&cb_on_move);
				glfwSetWindowCloseCallback(m_glfw_window,&cb_on_close);
				glfwSetWindowFocusCallback(m_glfw_window,&cb_on_focus);
				glfwSetWindowIconifyCallback(m_glfw_window,&cb_on_iconify);
				glfwSetWindowRefreshCallback(m_glfw_window,&cb_on_refresh);
				glfwSetCharModsCallback(m_glfw_window,&cb_on_character);
				glfwSetKeyCallback(m_glfw_window,&cb_on_key);
			}
		}
	}
}

OOGL::Window::~Window()
{
	// Destroy all GL state before we kill the window
	m_default_fb.reset();
	m_state->reset();

	if (m_glfw_window)
		glfwDestroyWindow(m_glfw_window);
}

bool OOGL::Window::is_visible() const
{
	if (!m_glfw_window)
		return false;

	return glfwGetWindowAttrib(m_glfw_window,GLFW_VISIBLE) != 0;
}

void OOGL::Window::visible(bool show)
{
	if (m_glfw_window)
	{
		if (show)
			glfwShowWindow(m_glfw_window);
		else
			glfwHideWindow(m_glfw_window);
	}
}

bool OOGL::Window::is_iconified() const
{
	if (!m_glfw_window)
		return false;

	return glfwGetWindowAttrib(m_glfw_window,GLFW_ICONIFIED) != 0;
}

void OOGL::Window::iconify(bool minimize)
{
	if (m_glfw_window)
	{
		if (minimize)
			glfwIconifyWindow(m_glfw_window);
		else
			glfwRestoreWindow(m_glfw_window);
	}
}

void OOGL::Window::cb_on_move(GLFWwindow* window, int left, int top)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->m_on_moved)
		pThis->m_on_moved.invoke(*pThis,glm::ivec2(left,top));
}

void OOGL::Window::cb_on_size(GLFWwindow* window, int width, int height)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->m_on_sized)
		pThis->m_on_sized.invoke(*pThis,glm::ivec2(width,height));
}

void OOGL::Window::cb_on_close(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->m_on_close)
		pThis->m_on_close.invoke(*pThis);
}

void OOGL::Window::cb_on_focus(GLFWwindow* window, int focused)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void OOGL::Window::cb_on_iconify(GLFWwindow* window, int iconified)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void OOGL::Window::cb_on_refresh(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->draw())
		pThis->swap();
}

void OOGL::Window::cb_on_character(GLFWwindow* window, unsigned int codepoint, int mods)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->m_on_character)
		pThis->m_on_character.invoke(*pThis,codepoint,mods);
}

void OOGL::Window::cb_on_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->m_on_keystroke)
	{
		key_stroke_t keystroke = {key, scancode, action, mods};
		pThis->m_on_keystroke.invoke(*pThis,keystroke);
	}
}

bool OOGL::Window::is_valid() const
{
	return m_glfw_window != NULL;
}

const OOBase::SharedPtr<OOGL::Framebuffer>& OOGL::Window::get_default_frame_buffer() const
{
	return m_default_fb;
}

glm::ivec2 OOGL::Window::size() const
{
	int width,height;
	glfwGetFramebufferSize(m_glfw_window,&width,&height);

	return glm::ivec2(width,height);
}

GLFWmonitor* OOGL::Window::monitor() const
{
	GLFWmonitor* m = glfwGetWindowMonitor(m_glfw_window);
	if (!m)
	{
		// Try to find by position

		int width, height;
		glfwGetWindowSize(m_glfw_window,&width,&height);

		int left,top,right,bottom;
		glfwGetWindowFrameSize(m_glfw_window,&left,&top,&right,&bottom);
		width += left + right;
		height += top + bottom;

		int xpos, ypos;
		glfwGetWindowPos(m_glfw_window,&xpos,&ypos);

		glm::ivec2 centre(xpos - left + width/2,ypos - top + height/2);

		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		for (int i = 0;i < count;++i)
		{
			glfwGetMonitorPos(monitors[i], &xpos, &ypos);
			if (centre.x >= xpos && centre.y >= ypos)
			{
				const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);
				if (centre.x < xpos + mode->width && centre.y < ypos + mode->height)
				{
					m = monitors[0];
					break;
				}
			}
		}
	}
	return m;
}

glm::vec2 OOGL::Window::dots_per_mm() const
{
	glm::vec2 res(0,0);
	GLFWmonitor* m = monitor();
	if (m)
	{
		int width, height;
		glfwGetMonitorPhysicalSize(m, &width, &height);

		const GLFWvidmode* mode = glfwGetVideoMode(m);
		res.x = static_cast<float>(mode->width) / width;
		res.y = static_cast<float>(mode->height) / height;
	}
	return res;
}

bool OOGL::Window::draw()
{
	if (!m_glfw_window || !is_visible() || is_iconified() || !m_on_draw)
		return false;

	// Make this context current
	glfwMakeContextCurrent(m_glfw_window);

	m_on_draw.invoke(*this,*m_state);

	return true;
}

void OOGL::Window::swap()
{
	if (m_glfw_window)
		glfwSwapBuffers(m_glfw_window);
}

OOBase::Delegate1<void,const OOGL::Window&,OOBase::ThreadLocalAllocator> OOGL::Window::on_close(const OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator> prev = m_on_close;
	m_on_close = delegate;
	return prev;
}

OOBase::Delegate2<void,const OOGL::Window&,OOGL::State&,OOBase::ThreadLocalAllocator> OOGL::Window::on_draw(const OOBase::Delegate2<void,const Window&,State&,OOBase::ThreadLocalAllocator>& delegate)
{
	const OOBase::Delegate2<void,const Window&,State&,OOBase::ThreadLocalAllocator> prev = m_on_draw;
	m_on_draw = delegate;
	return prev;
}

OOBase::Delegate2<void,const OOGL::Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator> OOGL::Window::on_sized(const OOBase::Delegate2<void,const Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate2<void,const Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator> prev = m_on_sized;
	m_on_sized = delegate;
	return prev;
}

OOBase::Delegate2<void,const OOGL::Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator> OOGL::Window::on_moved(const OOBase::Delegate2<void,const Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate2<void,const Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator> prev = m_on_moved;
	m_on_moved = delegate;
	return prev;
}

OOBase::Delegate3<void,const OOGL::Window&,unsigned int,int,OOBase::ThreadLocalAllocator> OOGL::Window::on_character(const OOBase::Delegate3<void,const Window&,unsigned int,int,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate3<void,const Window&,unsigned int,int,OOBase::ThreadLocalAllocator> prev = m_on_character;
	m_on_character = delegate;
	return prev;
}

OOBase::Delegate2<void,const OOGL::Window&,const OOGL::Window::key_stroke_t&,OOBase::ThreadLocalAllocator> OOGL::Window::on_keystroke(const OOBase::Delegate2<void,const Window&,const key_stroke_t&,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate2<void,const Window&,const key_stroke_t&,OOBase::ThreadLocalAllocator> prev = m_on_keystroke;
	m_on_keystroke = delegate;
	return prev;
}
