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

Indigo::State& Indigo::Window::make_current() const
{
	if (glfwGetCurrentContext() != m_glfw_window)
		glfwMakeContextCurrent(m_glfw_window);

	return *m_state;
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
		pThis->signal_sized.fire(pThis->shared_from_this(),glm::ivec2(width,height));
}

void Indigo::Window::on_close(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis)
		pThis->signal_close.fire(pThis->shared_from_this());
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
	if (!m_default_fb)
		const_cast<Window*>(this)->m_default_fb = Framebuffer::get_default(const_cast<Window*>(this)->shared_from_this());

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
	// Make this context current
	if (!m_glfw_window || !is_visible() || is_iconified())
		return false;

	make_current();

	m_state->bind(get_default_frame_buffer());

	// Always clear everything
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	signal_draw.fire(shared_from_this(),*m_state);

	return true;
}

void Indigo::Window::swap()
{
	if (m_glfw_window)
		glfwSwapBuffers(m_glfw_window);
}
