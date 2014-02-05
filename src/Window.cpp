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

void Indigo::Window::render()
{
	if (!make_current())
		return;

	// for each camera

	// Cull
	// Sort


	// Draw

	glfwSwapBuffers(m_glfw_window);
}

bool Indigo::Window::make_current() const
{
	if (!m_glfw_window)
		return false;

	if (glfwGetCurrentContext() != m_glfw_window)
		glfwMakeContextCurrent(m_glfw_window);

	return true;
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

void Indigo::Window::on_pos(GLFWwindow* window, int xpos, int ypos)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void Indigo::Window::on_size(GLFWwindow* window, int width, int height)
{
	//RenderWindow* pThis = static_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
}

void Indigo::Window::on_close(GLFWwindow* window)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis)
		pThis->signal_close.fire();
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
	if (pThis && pThis->is_visible() && !pThis->is_iconified())
		pThis->render();
}

Indigo::Window::operator Indigo::Window::bool_type() const
{
	return m_glfw_window != NULL ? &SafeBoolean::this_type_does_not_support_comparisons : NULL;
}

const OOBase::SharedPtr<Indigo::Framebuffer>& Indigo::Window::get_default_frame_buffer() const
{
	if (!m_default_fb && make_current())
	{
		GLint fb_id = GL_INVALID_VALUE;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING,&fb_id);
		if (fb_id != GL_INVALID_VALUE)
			m_default_fb = OOBase::allocate_shared<Framebuffer,OOBase::ThreadLocalAllocator>(const_cast<Window*>(this)->shared_from_this(),fb_id);
	}
	return m_default_fb;
}
