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

void Indigo::Window::on_size(GLFWwindow* window, int width, int height)
{
	Window* pThis = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (pThis)
		pThis->get_default_frame_buffer()->signal_sized.fire(glm::ivec2(width,height));
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
	if (pThis && pThis->draw())
		pThis->swap();
}

Indigo::Window::operator Indigo::Window::bool_type() const
{
	return m_glfw_window != NULL ? &SafeBoolean::this_type_does_not_support_comparisons : NULL;
}

void Indigo::Window::init_default_fb()
{
	if (make_current())
	{
		m_fb_fns = OOBase::allocate_shared<detail::FramebufferFunctions,OOBase::ThreadLocalAllocator>();
		if (m_fb_fns)
		{
			m_fb_fns->init(m_glfw_window);
			m_default_fb = Framebuffer::get_default(shared_from_this());
		}
	}
}

const OOBase::SharedPtr<Indigo::Framebuffer>& Indigo::Window::get_default_frame_buffer() const
{
	if (!m_default_fb)
		const_cast<Window*>(this)->init_default_fb();

	return m_default_fb;
}

bool Indigo::Window::draw()
{
	// Make this context current
	if (!make_current() || !is_visible() || is_iconified())
		return false;

	// Render the default FB
	if (m_default_fb)
		m_default_fb->render();

	return true;
}

void Indigo::Window::swap()
{
	if (m_glfw_window)
		glfwSwapBuffers(m_glfw_window);
}
