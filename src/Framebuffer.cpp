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

#include "Framebuffer.h"
#include "Window.h"

void Indigo::detail::FramebufferFunctions::init(GLFWwindow* win)
{
	if (!win)
		return;

	if (glfwGetCurrentContext() != win)
		glfwMakeContextCurrent(win);

	if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
	{
		m_fn_genFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
		m_fn_delFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");
		m_fn_bindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
		m_fn_checkFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");
	}
	if (!m_fn_genFramebuffers && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
	{
		m_fn_genFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffersEXT");
		m_fn_delFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
		m_fn_bindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferEXT");
		m_fn_checkFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatusEXT");
	}

	if (!m_fn_genFramebuffers)
		OOBase_CallCriticalFailure("Failed to load OpenGL FBO support");
}

Indigo::Framebuffer::Framebuffer(const OOBase::SharedPtr<Window>& window, GLuint id) : m_fns(window->m_fb_fns), m_id(id), m_destroy(true)
{
	if (m_id == GL_INVALID_VALUE)
	{
		if (m_fns && m_fns->m_fn_genFramebuffers)
			(*m_fns->m_fn_genFramebuffers)(1,&m_id);
	}
	else
		m_destroy = false;
}

Indigo::Framebuffer::~Framebuffer()
{
	if (m_destroy && m_id != GL_INVALID_VALUE && m_fns && m_fns->m_fn_delFramebuffers)
		(*m_fns->m_fn_delFramebuffers)(1,&m_id);
}

Indigo::Framebuffer::operator Indigo::Framebuffer::bool_type() const
{
	return m_id != GL_INVALID_VALUE ? &SafeBoolean::this_type_does_not_support_comparisons : NULL;
}

bool Indigo::Framebuffer::bind(GLenum target) const
{
	if (m_id == GL_INVALID_VALUE || !m_fns || !m_fns->m_fn_bindFramebuffer)
		return false;

	(*m_fns->m_fn_bindFramebuffer)(target,m_id);
	return true;
}

GLenum Indigo::Framebuffer::check() const
{
	if (m_id == GL_INVALID_VALUE || !m_fns || !m_fns->m_fn_checkFramebufferStatus)
		return GL_FRAMEBUFFER_UNDEFINED;

	return (*m_fns->m_fn_checkFramebufferStatus)(m_id);
}
