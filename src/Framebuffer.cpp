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

Indigo::Framebuffer::Framebuffer(const OOBase::SharedPtr<Window>& window) :
		m_window(window),
		m_fns(window->m_fb_fns),
		m_id(GL_INVALID_VALUE),
		m_default(false),
		m_clear_bits(GL_COLOR_BUFFER_BIT),
		m_clear_colour(),
		m_clear_depth(1.0f),
		m_clear_stencil(0)
{
	if (m_fns && m_fns->m_fn_genFramebuffers)
		(*m_fns->m_fn_genFramebuffers)(1,&m_id);
}

Indigo::Framebuffer::Framebuffer(const OOBase::SharedPtr<Window>& window, GLuint id) :
		m_window(window),
		m_fns(window->m_fb_fns),
		m_id(id),
		m_default(true),
		m_clear_bits(GL_COLOR_BUFFER_BIT),
		m_clear_colour(),
		m_clear_depth(1.0f),
		m_clear_stencil(0)
{
}

OOBase::SharedPtr<Indigo::Framebuffer> Indigo::Framebuffer::get_default(const OOBase::SharedPtr<Window>& window)
{
	OOBase::SharedPtr<Framebuffer> ret;
	GLint fb_id = GL_INVALID_VALUE;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING,&fb_id);
	if (fb_id != GL_INVALID_VALUE)
		ret = OOBase::allocate_shared<Framebuffer,OOBase::ThreadLocalAllocator>(window,fb_id);
	return ret;
}

Indigo::Framebuffer::~Framebuffer()
{
	if (!m_default && m_id != GL_INVALID_VALUE && m_fns && m_fns->m_fn_delFramebuffers)
		(*m_fns->m_fn_delFramebuffers)(1,&m_id);
}

Indigo::Framebuffer::operator Indigo::Framebuffer::bool_type() const
{
	return m_id != GL_INVALID_VALUE ? &SafeBoolean::this_type_does_not_support_comparisons : NULL;
}

OOBase::SharedPtr<Indigo::Window> Indigo::Framebuffer::window() const
{
	return m_window.lock();
}

GLenum Indigo::Framebuffer::check() const
{
	if (m_id == GL_INVALID_VALUE || !m_fns || !m_fns->m_fn_checkFramebufferStatus)
		return GL_FRAMEBUFFER_UNDEFINED;

	return (*m_fns->m_fn_checkFramebufferStatus)(m_id);
}

void Indigo::Framebuffer::clear_colour(const glm::vec4& rgba)
{
	m_clear_colour = rgba;
}

glm::vec4 Indigo::Framebuffer::clear_colour() const
{
	return m_clear_colour;
}

void Indigo::Framebuffer::clear_depth(GLdouble depth)
{
	m_clear_depth = depth;
}

GLdouble Indigo::Framebuffer::clear_depth()
{
	return m_clear_depth;
}

void Indigo::Framebuffer::clear_stencil(GLint s)
{
	m_clear_stencil = s;
}

GLint Indigo::Framebuffer::clear_stencil() const
{
	return m_clear_stencil;
}

void Indigo::Framebuffer::clear_bits(GLbitfield bits)
{
	m_clear_bits = bits;
}

GLbitfield Indigo::Framebuffer::clear_bits() const
{
	return m_clear_bits;
}

void Indigo::Framebuffer::render()
{
	if (m_id == GL_INVALID_VALUE || !m_fns || !m_fns->m_fn_bindFramebuffer)
		return;

	// Get the previous binding
	GLint old_fb = GL_INVALID_VALUE;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING,&old_fb);

	// Bind this FB
	if (static_cast<GLuint>(old_fb) != m_id)
		(*m_fns->m_fn_bindFramebuffer)(GL_FRAMEBUFFER,m_id);

	// Clear
	if (m_clear_bits)
	{
		if (m_clear_bits & GL_STENCIL_BUFFER_BIT)
			glClearStencil(m_clear_stencil);
		if (m_clear_bits & GL_DEPTH_BUFFER_BIT)
			glClearDepth(m_clear_depth);
		if (m_clear_bits & GL_COLOR_BUFFER_BIT)
			glClearColor(m_clear_colour.r,m_clear_colour.g,m_clear_colour.b,m_clear_colour.a);

		glClear(m_clear_bits);
	}

	// Render more stuff
	void* TODO;

	// For each viewport
	{
		// for each camera
		{
			// Cull
			// Sort
			// Draw
		}
	}

	// Bind the previous FB
	if (static_cast<GLuint>(old_fb) != m_id)
		(*m_fns->m_fn_bindFramebuffer)(GL_FRAMEBUFFER,old_fb);
}
