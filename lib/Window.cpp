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

OOGL::Window::Window(int width, int height, const char* title, unsigned int style, GLFWmonitor* monitor) :
		m_glfw_window(NULL)
{
	glfwWindowHint(GLFW_VISIBLE,(style & eWSvisible) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE,(style & eWSresizable) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_DECORATED,(style & eWSdecorated) ? GL_TRUE : GL_FALSE);

#if !defined(NDEBUG)
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE);
#endif

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
				glfwSetFramebufferSizeCallback(m_glfw_window,&on_size);
				glfwSetWindowCloseCallback(m_glfw_window,&on_close);
				glfwSetWindowFocusCallback(m_glfw_window,&on_focus);
				glfwSetWindowIconifyCallback(m_glfw_window,&on_iconify);
				glfwSetWindowRefreshCallback(m_glfw_window,&on_refresh);
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

void OOGL::Window::on_size(GLFWwindow* window, int width, int height)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis)
		pThis->signal_sized.fire(*pThis,glm::ivec2(width,height));
}

void OOGL::Window::on_close(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis)
		pThis->signal_close.fire(*pThis);
}

void OOGL::Window::on_focus(GLFWwindow* window, int focused)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void OOGL::Window::on_iconify(GLFWwindow* window, int iconified)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void OOGL::Window::on_refresh(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->draw())
		pThis->swap();
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
	if (!m_glfw_window || !is_visible() || is_iconified())
		return false;

	// Make this context current
	glfwMakeContextCurrent(m_glfw_window);

	signal_draw.fire(*this,*m_state);

	return true;
}

void OOGL::Window::swap()
{
	if (m_glfw_window)
		glfwSwapBuffers(m_glfw_window);
}
