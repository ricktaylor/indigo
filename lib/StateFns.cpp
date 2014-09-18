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

#include "Window.h"

Indigo::StateFns::StateFns() :
		m_fn_glGenFramebuffers(NULL),
		m_fn_glDeleteFramebuffers(NULL),
		m_fn_glBindFramebuffer(NULL),
		m_fn_glCheckFramebufferStatus(NULL),
		m_fn_glCreateShader(NULL),
		m_fn_glDeleteShader(NULL),
		m_fn_glShaderSource(NULL),
		m_fn_glCompileShader(NULL),
		m_fn_glGetShaderiv(NULL),
		m_fn_glGetShaderInfoLog(NULL),
		m_fn_glGetProgramiv(NULL),
		m_fn_glGetProgramInfoLog(NULL),
		m_fn_glAttachShader(NULL),
		m_fn_glDetachShader(NULL),
		m_fn_glLinkProgram(NULL),
		m_thunk_glUseProgram(&StateFns::check_glUseProgram),
		m_fn_glUseProgram(NULL),
		m_thunk_glBindMultiTexture(&StateFns::check_glBindMultiTexture),
		m_fn_glBindMultiTexture(NULL)
{
}

OOBase::SharedPtr<Indigo::StateFns> Indigo::StateFns::get_current()
{
	GLFWwindow* win = glfwGetCurrentContext();
	if (!win)
	{
		LOG_ERROR(("No current context!"));
		return OOBase::SharedPtr<StateFns>();
	}

	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (!window)
	{
		LOG_ERROR(("No current window!"));
		return OOBase::SharedPtr<StateFns>();
	}

	return window->m_state_fns;
}

void Indigo::StateFns::glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
	if (!m_fn_glGenFramebuffers)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
		{
			LOG_ERROR(("No current context!"));
			return;
		}

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");

		if (!m_fn_glGenFramebuffers && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffersEXT");
	}

	if (!m_fn_glGenFramebuffers)
		LOG_ERROR(("No glGenFramebuffers function"));
	else
		(*m_fn_glGenFramebuffers)(n,framebuffers);
}

void Indigo::StateFns::glDeleteFramebuffers(GLsizei n, GLuint *framebuffers)
{
	if (!m_fn_glDeleteFramebuffers)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
		{
			LOG_ERROR(("No current context!"));
			return;
		}

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");

		if (!m_fn_glDeleteFramebuffers && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
	}

	if (!m_fn_glDeleteFramebuffers)
		LOG_ERROR(("No glDeleteFramebuffers function"));
	else
		(*m_fn_glDeleteFramebuffers)(n,framebuffers);
}

void Indigo::StateFns::glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	if (!m_fn_glBindFramebuffer)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
		{
			LOG_ERROR(("No current context!"));
			return;
		}

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");

		if (!m_fn_glBindFramebuffer && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferEXT");
	}

	if (!m_fn_glBindFramebuffer)
		LOG_ERROR(("No glBindFramebuffer function"));
	else
		(*m_fn_glBindFramebuffer)(target,framebuffer);
}

GLenum Indigo::StateFns::glCheckFramebufferStatus(GLenum target)
{
	if (!m_fn_glCheckFramebufferStatus)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
			LOG_ERROR_RETURN(("No current context!"),GL_FRAMEBUFFER_UNSUPPORTED);

		if (glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR) >= 3 || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");

		if (!m_fn_glCheckFramebufferStatus && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatusEXT");

		if (!m_fn_glCheckFramebufferStatus)
			LOG_ERROR_RETURN(("No glCheckFramebufferStatus function"),GL_FRAMEBUFFER_UNSUPPORTED);
	}

	return (*m_fn_glCheckFramebufferStatus)(target);
}

GLuint Indigo::StateFns::glCreateShader(GLenum shaderType)
{
	if (!m_fn_glCreateShader)
	{
		m_fn_glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
		if (!m_fn_glCreateShader)
			LOG_ERROR_RETURN(("No glCreateShader function"),0);
	}

	return (*m_fn_glCreateShader)(shaderType);
}

void Indigo::StateFns::glDeleteShader(GLuint shader)
{
	if (!m_fn_glDeleteShader)
		m_fn_glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");

	if (!m_fn_glDeleteShader)
		LOG_ERROR(("No glDeleteShader function"));
	else
		(*m_fn_glDeleteShader)(shader);
}

void Indigo::StateFns::glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
	if (!m_fn_glShaderSource)
		m_fn_glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");

	if (!m_fn_glShaderSource)
		LOG_ERROR(("No glShaderSource function"));
	else
		(*m_fn_glShaderSource)(shader,count,string,length);
}

void Indigo::StateFns::glCompileShader(GLuint shader)
{
	if (!m_fn_glCompileShader)
		m_fn_glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");

	if (!m_fn_glCompileShader)
		LOG_ERROR(("No glCompileShader function"));
	else
		(*m_fn_glCompileShader)(shader);
}

void Indigo::StateFns::glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	if (!m_fn_glGetShaderiv)
		m_fn_glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");

	if (!m_fn_glGetShaderiv)
		LOG_ERROR(("No glGetShaderiv function"));
	else
		(*m_fn_glGetShaderiv)(shader,pname,params);
}

void Indigo::StateFns::glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	if (!m_fn_glGetShaderInfoLog)
		m_fn_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");

	if (!m_fn_glGetShaderInfoLog)
		LOG_ERROR(("No glGetShaderInfoLog function"));
	else
		(*m_fn_glGetShaderInfoLog)(shader,maxLength,length,infoLog);
}

void Indigo::StateFns::glGetProgramiv(GLuint shader, GLenum pname, GLint* params)
{
	if (!m_fn_glGetProgramiv)
		m_fn_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");

	if (!m_fn_glGetProgramiv)
		LOG_ERROR(("No glGetProgramiv function"));
	else
		(*m_fn_glGetProgramiv)(shader,pname,params);
}

void Indigo::StateFns::glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	if (!m_fn_glGetProgramInfoLog)
		m_fn_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");

	if (!m_fn_glGetProgramInfoLog)
		LOG_ERROR(("No glGetProgramInfoLog function"));
	else
		(*m_fn_glGetProgramInfoLog)(program,maxLength,length,infoLog);
}

void Indigo::StateFns::check_glUseProgram(GLuint program)
{
	if (!m_fn_glUseProgram)
		m_fn_glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");

	if (!m_fn_glUseProgram)
		LOG_ERROR(("No glUseProgram function"));
	else
		(*m_fn_glUseProgram)(program);
}

void Indigo::StateFns::call_glUseProgram(GLuint program)
{
	(*m_fn_glUseProgram)(program);
}

void Indigo::StateFns::glUseProgram(GLuint program)
{
	(this->*m_thunk_glUseProgram)(program);
}

void Indigo::StateFns::glAttachShader(GLuint program, GLuint shader)
{
	if (!m_fn_glAttachShader)
		m_fn_glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");

	if (!m_fn_glAttachShader)
		LOG_ERROR(("No glAttachShader function"));
	else
		(*m_fn_glAttachShader)(program,shader);
}

void Indigo::StateFns::glDetachShader(GLuint program, GLuint shader)
{
	if (!m_fn_glDetachShader)
		m_fn_glDetachShader = (PFNGLDETACHSHADERPROC)glfwGetProcAddress("glDetachShader");

	if (!m_fn_glDetachShader)
		LOG_ERROR(("No glDetachShader function"));
	else
		(*m_fn_glDetachShader)(program,shader);
}

void Indigo::StateFns::glLinkProgram(GLuint program)
{
	if (!m_fn_glLinkProgram)
		m_fn_glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");

	if (!m_fn_glLinkProgram)
		LOG_ERROR(("No glLinkProgram function"));
	else
		(*m_fn_glLinkProgram)(program);
}

void Indigo::StateFns::check_glBindMultiTexture(State* state, GLenum unit, GLenum target, GLuint texture)
{
	GLFWwindow* win = glfwGetCurrentContext();
	if (!win)
	{
		LOG_ERROR(("No current context!"));
		return;
	}

	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		m_fn_glBindMultiTexture = (PFNGLBINDMULTITEXTUREEXTPROC)glfwGetProcAddress("glBindMultiTextureEXT");

	if (!m_fn_glBindMultiTexture)
		m_thunk_glBindMultiTexture = &StateFns::emulate_glBindMultiTexture;
	else
		m_thunk_glBindMultiTexture = &StateFns::call_glBindMultiTexture;

	(this->*m_thunk_glBindMultiTexture)(state,unit,target,texture);
}

void Indigo::StateFns::call_glBindMultiTexture(State*, GLenum unit, GLenum target, GLuint texture)
{
	(*m_fn_glBindMultiTexture)(unit,target,texture);
}

void Indigo::StateFns::emulate_glBindMultiTexture(State* state, GLenum unit, GLenum target, GLuint texture)
{
	state->activate_texture_unit(unit);
	glBindTexture(target,texture);
}

void Indigo::StateFns::glBindMultiTexture(State* state, GLenum unit, GLenum target, GLuint texture)
{
	(this->*m_thunk_glBindMultiTexture)(state,unit,target,texture);
}
