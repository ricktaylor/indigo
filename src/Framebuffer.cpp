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

Indigo::Framebuffer::Framebuffer(const Window& win, GLuint id) :
		m_id(id),
		m_fn_delFrameBuffers(NULL),
		m_fn_bindFrameBuffer(NULL),
		m_fn_checkFrameBufferStatus(NULL)
{
	if (id == GL_INVALID_VALUE)
	{
		PFNGLGENFRAMEBUFFERSPROC fn_genFrameBuffers = NULL;
		if (win.make_current())
		{
			if (glfwGetWindowAttrib(win.get_glfw_window(),GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			{
				fn_genFrameBuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
				m_fn_delFrameBuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");
			}
			if ((!fn_genFrameBuffers || !m_fn_delFrameBuffers) && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			{
				fn_genFrameBuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffersEXT");
				m_fn_delFrameBuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
			}
			if (!fn_genFrameBuffers || !m_fn_delFrameBuffers)
				OOBase_CallCriticalFailure("Failed to load OpenGL FBO support");

			if (fn_genFrameBuffers)
				(*fn_genFrameBuffers)(1,&m_id);
		}
	}

	if (glfwGetWindowAttrib(win.get_glfw_window(),GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
	{
		m_fn_bindFrameBuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
		m_fn_checkFrameBufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");
	}
	if (!m_fn_bindFrameBuffer && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
	{
		m_fn_bindFrameBuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferEXT");
		m_fn_checkFrameBufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatusEXT");
	}
	if (!m_fn_bindFrameBuffer || !m_fn_checkFrameBufferStatus)
		OOBase_CallCriticalFailure("Failed to load OpenGL FBO support");
}

Indigo::Framebuffer::~Framebuffer()
{
	if (m_id != GL_INVALID_VALUE && m_fn_delFrameBuffers)
		(*m_fn_delFrameBuffers)(1,&m_id);
}

Indigo::Framebuffer::operator Indigo::Framebuffer::bool_type() const
{
	return m_id != GL_INVALID_VALUE ? &SafeBoolean::this_type_does_not_support_comparisons : NULL;
}

bool Indigo::Framebuffer::bind(GLenum target) const
{
	if (m_id == GL_INVALID_VALUE || !m_fn_bindFrameBuffer)
		return false;

	(*m_fn_bindFrameBuffer)(target,m_id);
	return true;
}

GLenum Indigo::Framebuffer::check() const
{
	if (m_id == GL_INVALID_VALUE || !m_fn_checkFrameBufferStatus)
		return GL_FRAMEBUFFER_UNDEFINED;

	return (*m_fn_checkFrameBufferStatus)(m_id);
}
