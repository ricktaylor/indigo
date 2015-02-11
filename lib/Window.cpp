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

#include "Window.h"

Indigo::Window::Window(int width, int height, const char* title, unsigned int style, GLFWmonitor* monitor) :
		m_glfw_window(NULL)
{
	glfwWindowHint(GLFW_VISIBLE,(style & eWSvisible) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE,(style & eWSresizable) ? GL_TRUE : GL_FALSE);
	glfwWindowHint(GLFW_DECORATED,(style & eWSdecorated) ? GL_TRUE : GL_FALSE);

	// Now try to create the window
	m_glfw_window = glfwCreateWindow(width,height,title,monitor,NULL);
	if (!m_glfw_window)
		LOG_ERROR(("Failed to create window"));
	else
	{
		// Wait for window manager to do its thing
		//glfwWaitEvents();

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

Indigo::Window::~Window()
{
	if (m_glfw_window)
		glfwDestroyWindow(m_glfw_window);
}

bool Indigo::Window::is_visible() const
{
	if (!m_glfw_window)
		return false;

	return glfwGetWindowAttrib(m_glfw_window,GLFW_VISIBLE) != 0;
}

void Indigo::Window::visible(bool show)
{
	if (m_glfw_window)
	{
		if (show)
			glfwShowWindow(m_glfw_window);
		else
			glfwHideWindow(m_glfw_window);
	}
}

bool Indigo::Window::is_iconified() const
{
	if (!m_glfw_window)
		return false;

	return glfwGetWindowAttrib(m_glfw_window,GLFW_ICONIFIED) != 0;
}

void Indigo::Window::iconify(bool minimize)
{
	if (m_glfw_window)
	{
		if (minimize)
			glfwIconifyWindow(m_glfw_window);
		else
			glfwRestoreWindow(m_glfw_window);
	}
}

void Indigo::Window::on_size(GLFWwindow* window, int width, int height)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis)
		pThis->signal_sized.fire(*pThis,glm::ivec2(width,height));
}

void Indigo::Window::on_close(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis)
		pThis->signal_close.fire(*pThis);
}

void Indigo::Window::on_focus(GLFWwindow* window, int focused)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void Indigo::Window::on_iconify(GLFWwindow* window, int iconified)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void Indigo::Window::on_refresh(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis && pThis->draw())
		pThis->swap();
}

bool Indigo::Window::is_valid() const
{
	return m_glfw_window != NULL;
}

const OOBase::SharedPtr<Indigo::Framebuffer>& Indigo::Window::get_default_frame_buffer() const
{
	return m_default_fb;
}

glm::ivec2 Indigo::Window::size() const
{
	int width,height;
	glfwGetFramebufferSize(m_glfw_window,&width,&height);

	return glm::ivec2(width,height);
}

bool Indigo::Window::draw()
{
	if (!m_glfw_window || !is_visible() || is_iconified())
		return false;

	// Make this context current
	glfwMakeContextCurrent(m_glfw_window);

	signal_draw.fire(*this,*m_state);

	return true;
}

void Indigo::Window::swap()
{
	if (m_glfw_window)
		glfwSwapBuffers(m_glfw_window);
}
