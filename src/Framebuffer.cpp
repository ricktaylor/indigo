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

#include "Viewport.h"
#include "Framebuffer.h"
#include "Window.h"

static void genFrameBuffers(GLsizei n, GLuint *framebuffers)
{
	static PFNGLGENFRAMEBUFFERSPROC s_fn = NULL;
	if (!s_fn)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
		{
			LOG_ERROR(("No current context!"));
			return;
		}

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");

		if (!s_fn && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffersEXT");
	}

	if (!s_fn)
		LOG_ERROR(("No glGenFramebuffers function"));
	else
		(*s_fn)(n,framebuffers);
}

static void deleteFrameBuffers(GLsizei n, GLuint *framebuffers)
{
	static PFNGLDELETEFRAMEBUFFERSPROC s_fn = NULL;
	if (!s_fn)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
		{
			LOG_ERROR(("No current context!"));
			return;
		}

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");

		if (!s_fn && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
	}

	if (!s_fn)
		LOG_ERROR(("No glDeleteFramebuffers function"));
	else
		(*s_fn)(n,framebuffers);
}

static void bindFrameBuffer(GLenum target, GLuint framebuffer)
{
	static PFNGLBINDFRAMEBUFFERPROC s_fn = NULL;
	if (!s_fn)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
		{
			LOG_ERROR(("No current context!"));
			return;
		}

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");

		if (!s_fn && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferEXT");
	}

	if (!s_fn)
		LOG_ERROR(("No glBindFramebuffer function"));
	else
		(*s_fn)(target,framebuffer);
}

static GLenum checkFrameBufferStatus(GLenum target)
{
	static PFNGLCHECKFRAMEBUFFERSTATUSPROC s_fn = NULL;
	if (!s_fn)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
			LOG_ERROR_RETURN(("No current context!"),GL_FRAMEBUFFER_UNSUPPORTED);

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");

		if (!s_fn && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			s_fn = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatusEXT");
	}

	if (!s_fn)
		LOG_ERROR_RETURN(("No glCheckFramebufferStatus function"),GL_FRAMEBUFFER_UNSUPPORTED);

	return (*s_fn)(target);
}

Indigo::Framebuffer::Framebuffer(const OOBase::SharedPtr<Window>& window) :
		m_window(window),
		m_id(GL_INVALID_VALUE),
		m_default(false),
		m_clear_bits(GL_COLOR_BUFFER_BIT),
		m_clear_colour(),
		m_clear_depth(1.0f),
		m_clear_stencil(0)
{
	genFrameBuffers(1,&m_id);
}

Indigo::Framebuffer::Framebuffer(const OOBase::SharedPtr<Window>& window, GLuint id) :
		m_window(window),
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
	if (!m_default)
		deleteFrameBuffers(1,&m_id);
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
	return checkFrameBufferStatus(m_id);
}

void Indigo::Framebuffer::clear_colour(const glm::vec4& rgba)
{
	m_clear_colour = rgba;
}

glm::vec4 Indigo::Framebuffer::clear_colour() const
{
	return m_clear_colour;
}

void Indigo::Framebuffer::clear_depth(GLfloat depth)
{
	m_clear_depth = depth;
}

GLfloat Indigo::Framebuffer::clear_depth()
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

const Indigo::Framebuffer::viewports_t& Indigo::Framebuffer::viewports() const
{
	return m_viewports;
}

OOBase::SharedPtr<Indigo::Viewport> Indigo::Framebuffer::add_viewport(const glm::ivec2& lower_left, const glm::ivec2& size)
{
	OOBase::SharedPtr<Viewport> v = OOBase::allocate_shared<Viewport,OOBase::ThreadLocalAllocator>(shared_from_this(),lower_left,size);
	if (v)
	{
		int err = m_viewports.push_back(v);
		if (err)
		{
			v.reset();
			LOG_ERROR(("Failed to insert viewport: %s",OOBase::system_error_text(err)));
		}
	}
	return v;
}

void Indigo::Framebuffer::bind()
{
	bindFrameBuffer(GL_FRAMEBUFFER,m_id);
}

void Indigo::Framebuffer::render(State& gl_state)
{
	if (m_id == GL_INVALID_VALUE)
		return;

	// Bind this FB
	gl_state.bind(shared_from_this());

	// Clear
	if (m_clear_bits)
	{
		if (m_clear_bits & GL_STENCIL_BUFFER_BIT)
			gl_state.clear_stencil(m_clear_stencil);
		if (m_clear_bits & GL_DEPTH_BUFFER_BIT)
			gl_state.clear_depth(m_clear_depth);
		if (m_clear_bits & GL_COLOR_BUFFER_BIT)
			gl_state.clear_colour(m_clear_colour);

		glClear(m_clear_bits);
	}

	// For each viewport
	for (viewports_t::iterator v = m_viewports.begin();v != m_viewports.end(); ++v)
		(*v)->render(gl_state);
}
