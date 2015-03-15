///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

namespace
{
	static bool isGLversion(GLFWwindow* win, int major, int minor)
	{
		int actual_major = glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR);
		int actual_minor = glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MINOR);

		return (actual_major > major || (actual_major == major && actual_minor >= minor));
	}

	static bool isGLversion(int major, int minor)
	{
		GLFWwindow* win = glfwGetCurrentContext();
		if (!win)
			LOG_ERROR_RETURN(("No current context!"),false);

		return isGLversion(win,major,minor);
	}
}

OOGL::StateFns::StateFns() :
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
		m_fn_glUseProgram(NULL),
		m_fn_glActiveTexture(NULL),
		m_thunk_glBindTextureUnit(&StateFns::check_glBindTextureUnit),
		m_fn_glBindTextureUnit(NULL),
		m_fn_glTexImage3D(NULL),
		m_thunk_glTextureStorage1D(&StateFns::check_glTextureStorage1D),
		m_fn_glTextureStorage1D(NULL),
		m_thunk_glTextureStorage2D(&StateFns::check_glTextureStorage2D),
		m_fn_glTextureStorage2D(NULL),
		m_thunk_glTextureStorage3D(&StateFns::check_glTextureStorage3D),
		m_fn_glTextureStorage3D(NULL),
		m_thunk_glTextureSubImage1D(&StateFns::check_glTextureSubImage1D),
		m_fn_glTextureSubImage1D(NULL),
		m_thunk_glTextureSubImage2D(&StateFns::check_glTextureSubImage2D),
		m_fn_glTextureSubImage2D(NULL),
		m_thunk_glTextureSubImage3D(&StateFns::check_glTextureSubImage3D),
		m_fn_glTextureSubImage3D(NULL),
		m_thunk_glTextureParameterf(&StateFns::check_glTextureParameterf),
		m_fn_glTextureParameterf(NULL),
		m_thunk_glTextureParameterfv(&StateFns::check_glTextureParameterfv),
		m_fn_glTextureParameterfv(NULL),
		m_thunk_glTextureParameteri(&StateFns::check_glTextureParameteri),
		m_fn_glTextureParameteri(NULL),
		m_thunk_glTextureParameteriv(&StateFns::check_glTextureParameteriv),
		m_fn_glTextureParameteriv(NULL),
		m_thunk_glGenerateTextureMipmap(&StateFns::check_glGenerateTextureMipmap),
		m_fn_glGenerateTextureMipmap(NULL),
		m_fn_glGenBuffers(NULL),
		m_fn_glBindBuffer(NULL),
		m_fn_glDeleteBuffers(NULL),
		m_thunk_glBufferData(&StateFns::check_glBufferData),
		m_fn_glBufferData(NULL),
		m_thunk_glMapBufferRange(&StateFns::check_glMapBufferRange),
		m_fn_glMapBufferRange(NULL),
		m_thunk_glUnmapBuffer(&StateFns::check_glUnmapBuffer),
		m_fn_glUnmapBuffer(NULL),
		m_thunk_glBufferSubData(&StateFns::check_glBufferSubData),
		m_fn_glBufferSubData(NULL),
		m_thunk_glCopyBufferSubData(&StateFns::check_glCopyBufferSubData),
		m_fn_glCopyBufferSubData(NULL),
		m_fn_glGenVertexArrays(NULL),
		m_fn_glDeleteVertexArrays(NULL),
		m_fn_glBindVertexArray(NULL),
		m_thunk_glMultiDrawArrays(&StateFns::check_glMultiDrawArrays),
		m_fn_glMultiDrawArrays(NULL),
		m_thunk_glDrawArraysInstanced(&StateFns::check_glDrawArraysInstanced),
		m_fn_glDrawArraysInstanced(NULL),
		m_thunk_glDrawArraysInstancedBaseInstance(&StateFns::check_glDrawArraysInstancedBaseInstance),
		m_fn_glDrawArraysInstancedBaseInstance(NULL),
		m_thunk_glDrawRangeElements(&StateFns::check_glDrawRangeElements),
		m_fn_glDrawRangeElements(NULL),
		m_thunk_glDrawRangeElementsBaseVertex(&StateFns::check_glDrawRangeElementsBaseVertex),
		m_fn_glDrawRangeElementsBaseVertex(NULL),
		m_thunk_glDrawElementsBaseVertex(&StateFns::check_glDrawElementsBaseVertex),
		m_fn_glDrawElementsBaseVertex(NULL),
		m_thunk_glMultiDrawElements(&StateFns::check_glMultiDrawElements),
		m_fn_glMultiDrawElements(NULL),
		m_thunk_glMultiDrawElementsBaseVertex(&StateFns::check_glMultiDrawElementsBaseVertex),
		m_fn_glMultiDrawElementsBaseVertex(NULL),
		m_thunk_glDrawElementsInstanced(&StateFns::check_glDrawElementsInstanced),
		m_fn_glDrawElementsInstanced(NULL),
		m_thunk_glDrawElementsInstancedBaseVertex(&StateFns::check_glDrawElementsInstancedBaseVertex),
		m_fn_glDrawElementsInstancedBaseVertex(NULL),
		m_thunk_glDrawElementsInstancedBaseInstance(&StateFns::check_glDrawElementsInstancedBaseInstance),
		m_fn_glDrawElementsInstancedBaseInstance(NULL),
		m_thunk_glDrawElementsInstancedBaseVertexBaseInstance(&StateFns::check_glDrawElementsInstancedBaseVertexBaseInstance),
		m_fn_glDrawElementsInstancedBaseVertexBaseInstance(NULL)
{
}

OOBase::SharedPtr<OOGL::StateFns> OOGL::StateFns::get_current()
{
	GLFWwindow* win = glfwGetCurrentContext();
	if (!win)
	{
		LOG_ERROR(("No current context"));
		return OOBase::SharedPtr<StateFns>();
	}

	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (!window)
	{
		LOG_ERROR(("No current window"));
		return OOBase::SharedPtr<StateFns>();
	}

	return window->m_state_fns;
}

void OOGL::StateFns::glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
	if (!m_fn_glGenFramebuffers)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");

		if (!m_fn_glGenFramebuffers && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffersEXT");
	}

	if (!m_fn_glGenFramebuffers)
		LOG_ERROR(("No glGenFramebuffers function"));
	else
		(*m_fn_glGenFramebuffers)(n,framebuffers);
}

void OOGL::StateFns::glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
	if (!m_fn_glDeleteFramebuffers)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");

		if (!m_fn_glDeleteFramebuffers && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
	}

	if (!m_fn_glDeleteFramebuffers)
		LOG_ERROR(("No glDeleteFramebuffers function"));
	else
		(*m_fn_glDeleteFramebuffers)(n,framebuffers);
}

void OOGL::StateFns::glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	if (!m_fn_glBindFramebuffer)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");

		if (!m_fn_glBindFramebuffer && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferEXT");
	}

	if (!m_fn_glBindFramebuffer)
		LOG_ERROR(("No glBindFramebuffer function"));
	else
		(*m_fn_glBindFramebuffer)(target,framebuffer);
}

GLenum OOGL::StateFns::glCheckFramebufferStatus(GLenum target)
{
	if (!m_fn_glCheckFramebufferStatus)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatus");

		if (!m_fn_glCheckFramebufferStatus && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)glfwGetProcAddress("glCheckFramebufferStatusEXT");

		if (!m_fn_glCheckFramebufferStatus)
			LOG_ERROR_RETURN(("No glCheckFramebufferStatus function"),GL_FRAMEBUFFER_UNSUPPORTED);
	}

	return (*m_fn_glCheckFramebufferStatus)(target);
}

GLuint OOGL::StateFns::glCreateShader(GLenum shaderType)
{
	if (!m_fn_glCreateShader)
	{
		m_fn_glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
		if (!m_fn_glCreateShader)
			LOG_ERROR_RETURN(("No glCreateShader function"),0);
	}

	return (*m_fn_glCreateShader)(shaderType);
}

void OOGL::StateFns::glDeleteShader(GLuint shader)
{
	if (!m_fn_glDeleteShader)
		m_fn_glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");

	if (!m_fn_glDeleteShader)
		LOG_ERROR(("No glDeleteShader function"));
	else
		(*m_fn_glDeleteShader)(shader);
}

void OOGL::StateFns::glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
	if (!m_fn_glShaderSource)
		m_fn_glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");

	if (!m_fn_glShaderSource)
		LOG_ERROR(("No glShaderSource function"));
	else
		(*m_fn_glShaderSource)(shader,count,string,length);
}

void OOGL::StateFns::glCompileShader(GLuint shader)
{
	if (!m_fn_glCompileShader)
		m_fn_glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");

	if (!m_fn_glCompileShader)
		LOG_ERROR(("No glCompileShader function"));
	else
		(*m_fn_glCompileShader)(shader);
}

void OOGL::StateFns::glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	if (!m_fn_glGetShaderiv)
		m_fn_glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");

	if (!m_fn_glGetShaderiv)
		LOG_ERROR(("No glGetShaderiv function"));
	else
		(*m_fn_glGetShaderiv)(shader,pname,params);
}

void OOGL::StateFns::glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	if (!m_fn_glGetShaderInfoLog)
		m_fn_glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");

	if (!m_fn_glGetShaderInfoLog)
		LOG_ERROR(("No glGetShaderInfoLog function"));
	else
		(*m_fn_glGetShaderInfoLog)(shader,maxLength,length,infoLog);
}

void OOGL::StateFns::glGetProgramiv(GLuint shader, GLenum pname, GLint* params)
{
	if (!m_fn_glGetProgramiv)
		m_fn_glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");

	if (!m_fn_glGetProgramiv)
		LOG_ERROR(("No glGetProgramiv function"));
	else
		(*m_fn_glGetProgramiv)(shader,pname,params);
}

void OOGL::StateFns::glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	if (!m_fn_glGetProgramInfoLog)
		m_fn_glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");

	if (!m_fn_glGetProgramInfoLog)
		LOG_ERROR(("No glGetProgramInfoLog function"));
	else
		(*m_fn_glGetProgramInfoLog)(program,maxLength,length,infoLog);
}

void OOGL::StateFns::glUseProgram(GLuint program)
{
	if (!m_fn_glUseProgram)
		m_fn_glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");

	if (!m_fn_glUseProgram)
		LOG_ERROR(("No glUseProgram function"));
	else
		(*m_fn_glUseProgram)(program);
}

void OOGL::StateFns::glAttachShader(GLuint program, GLuint shader)
{
	if (!m_fn_glAttachShader)
		m_fn_glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");

	if (!m_fn_glAttachShader)
		LOG_ERROR(("No glAttachShader function"));
	else
		(*m_fn_glAttachShader)(program,shader);
}

void OOGL::StateFns::glDetachShader(GLuint program, GLuint shader)
{
	if (!m_fn_glDetachShader)
		m_fn_glDetachShader = (PFNGLDETACHSHADERPROC)glfwGetProcAddress("glDetachShader");

	if (!m_fn_glDetachShader)
		LOG_ERROR(("No glDetachShader function"));
	else
		(*m_fn_glDetachShader)(program,shader);
}

void OOGL::StateFns::glLinkProgram(GLuint program)
{
	if (!m_fn_glLinkProgram)
		m_fn_glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");

	if (!m_fn_glLinkProgram)
		LOG_ERROR(("No glLinkProgram function"));
	else
		(*m_fn_glLinkProgram)(program);
}

void OOGL::StateFns::glActiveTexture(GLenum texture)
{
	if (!m_fn_glActiveTexture)
		m_fn_glActiveTexture = (PFNGLACTIVETEXTUREPROC)glfwGetProcAddress("glActiveTexture");

	if (!m_fn_glActiveTexture)
		LOG_ERROR(("No glActiveTexture function"));
	else
		(*m_fn_glActiveTexture)(texture);
}

void OOGL::StateFns::check_glBindTextureUnit(State& state, GLenum unit, GLenum target, GLuint texture)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glBindTextureUnit = glfwGetProcAddress("glBindTextureUnit");
		if (m_fn_glBindTextureUnit)
			m_thunk_glBindTextureUnit = &StateFns::call_glBindTextureUnit;
	}

	if (!m_fn_glBindTextureUnit && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glBindTextureUnit = glfwGetProcAddress("glBindMultiTextureEXT");
		if (m_fn_glBindTextureUnit)
			m_thunk_glBindTextureUnit = &StateFns::call_glBindTextureUnit;
	}

	if (!m_fn_glBindTextureUnit)
		m_thunk_glBindTextureUnit = &StateFns::emulate_glBindTextureUnit;
	
	(this->*m_thunk_glBindTextureUnit)(state,unit,target,texture);
}

void OOGL::StateFns::call_glBindTextureUnit(State&, GLenum unit, GLenum target, GLuint texture)
{
	(*((PFNGLBINDTEXTUREUNITPROC)m_fn_glBindTextureUnit))(unit,texture);
}

void OOGL::StateFns::emulate_glBindTextureUnit(State& state, GLenum unit, GLenum target, GLuint texture)
{
	state.activate_texture_unit(unit);
	glBindTexture(target,texture);
}

void OOGL::StateFns::call_glMultiBindTexture(State& state, GLenum unit, GLenum target, GLuint texture)
{
	(*((PFNGLBINDMULTITEXTUREEXTPROC)m_fn_glBindTextureUnit))(unit,target,texture);
}

void OOGL::StateFns::glBindTextureUnit(State& state, GLenum unit, GLenum target, GLuint texture)
{
	(this->*m_thunk_glBindTextureUnit)(state,unit,target,texture);
}

bool OOGL::StateFns::check_glTexImage3D()
{
	if (!m_fn_glTexImage3D)
		m_fn_glTexImage3D = (PFNGLTEXIMAGE3DPROC)glfwGetProcAddress("glTexImage3D");

	return (m_fn_glTexImage3D != NULL);
}

void OOGL::StateFns::glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
	if (!check_glTexImage3D())
		LOG_ERROR(("No glTexImage3D function"));
	else
		(*m_fn_glTexImage3D)(target,level,internalformat,width,height,depth,border,format,type,pixels);
}

bool OOGL::StateFns::check_glTextureStorage()
{
	if (!m_fn_glTextureStorage1D)
	{
		if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureStorage1D = glfwGetProcAddress("glTextureStorage1D");
			if (m_fn_glTextureStorage1D)
				m_thunk_glTextureStorage1D = &StateFns::call_glTextureStorage1D;
		}

		if (!m_fn_glTextureStorage1D && glfwExtensionSupported("GL_ARB_texture_storage") == GL_TRUE)
		{
			if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
			{
				m_fn_glTextureStorage1D = glfwGetProcAddress("glTextureStorage1DEXT");
				if (m_fn_glTextureStorage1D)
					m_thunk_glTextureStorage1D = &StateFns::call_glTextureStorage1DEXT;
			}

			if (!m_fn_glTextureStorage1D)
			{
				m_fn_glTextureStorage1D = glfwGetProcAddress("glTexStorage1D");
				if (m_fn_glTextureStorage1D)
					m_thunk_glTextureStorage1D = &StateFns::call_glTexStorage1D;
			}
		}
	}

	return (m_fn_glTextureStorage1D != NULL);
}

void OOGL::StateFns::check_glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	if (!check_glTextureStorage())
		LOG_ERROR(("No glTexStorage1D function"));
	else
		(this->*m_thunk_glTextureStorage1D)(state,texture,target,levels,internalFormat,width);
}

void OOGL::StateFns::call_glTextureStorage1D(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(*((PFNGLTEXTURESTORAGE1DPROC)m_fn_glTextureStorage1D))(texture,levels,internalFormat,width);
}

void OOGL::StateFns::call_glTextureStorage1DEXT(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(*((PFNGLTEXTURESTORAGE1DEXTPROC)m_fn_glTextureStorage1D))(texture,target,levels,internalFormat,width);
}

void OOGL::StateFns::call_glTexStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLTEXSTORAGE1DPROC)m_fn_glTextureStorage1D))(target,levels,internalFormat,width);
}

void OOGL::StateFns::glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(this->*m_thunk_glTextureStorage1D)(state,texture,target,levels,internalFormat,width);
}

void OOGL::StateFns::check_glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureStorage2D = glfwGetProcAddress("glTextureStorage2D");
		if (m_fn_glTextureStorage2D)
			m_thunk_glTextureStorage2D = &StateFns::call_glTextureStorage2D;
	}

	if (!m_fn_glTextureStorage2D && glfwExtensionSupported("GL_ARB_texture_storage") == GL_TRUE)
	{
		if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureStorage2D = glfwGetProcAddress("glTextureStorage2DEXT");
			if (m_fn_glTextureStorage2D)
				m_thunk_glTextureStorage2D = &StateFns::call_glTextureStorage2DEXT;
		}

		if (!m_fn_glTextureStorage2D)
		{
			m_fn_glTextureStorage2D = glfwGetProcAddress("glTexStorage2D");
			if (m_fn_glTextureStorage2D)
				m_thunk_glTextureStorage2D = &StateFns::call_glTexStorage2D;
		}
	}

	if (!m_fn_glTextureStorage2D)
		LOG_ERROR(("No glTexStorage2D function"));
	else
		(this->*m_thunk_glTextureStorage2D)(state,texture,target,levels,internalFormat,width,height);
}

void OOGL::StateFns::call_glTextureStorage2D(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(*((PFNGLTEXTURESTORAGE2DPROC)m_fn_glTextureStorage2D))(texture,levels,internalFormat,width,height);
}

void OOGL::StateFns::call_glTextureStorage2DEXT(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(*((PFNGLTEXTURESTORAGE2DEXTPROC)m_fn_glTextureStorage2D))(texture,target,levels,internalFormat,width,height);
}

void OOGL::StateFns::call_glTexStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLTEXSTORAGE2DPROC)m_fn_glTextureStorage2D))(target,levels,internalFormat,width,height);
}

void OOGL::StateFns::glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(this->*m_thunk_glTextureStorage2D)(state,texture,target,levels,internalFormat,width,height);
}

void OOGL::StateFns::check_glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureStorage3D = glfwGetProcAddress("glTextureStorage3D");
		if (m_fn_glTextureStorage3D)
			m_thunk_glTextureStorage3D = &StateFns::call_glTextureStorage3D;
	}

	if (!m_fn_glTextureStorage3D && glfwExtensionSupported("GL_ARB_texture_storage") == GL_TRUE)
	{
		if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureStorage3D = glfwGetProcAddress("glTextureStorage3DEXT");
			if (m_fn_glTextureStorage3D)
				m_thunk_glTextureStorage3D = &StateFns::call_glTextureStorage3DEXT;
		}

		if (!m_fn_glTextureStorage3D)
		{
			m_fn_glTextureStorage3D = glfwGetProcAddress("glTexStorage3D");
			if (m_fn_glTextureStorage3D)
				m_thunk_glTextureStorage3D = &StateFns::call_glTexStorage3D;
		}
	}

	if (!m_fn_glTextureStorage3D)
		LOG_ERROR(("No glTexStorage3D function"));
	else
		(this->*m_thunk_glTextureStorage3D)(state,texture,target,levels,internalFormat,width,height,depth);
}

void OOGL::StateFns::call_glTextureStorage3D(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(*((PFNGLTEXTURESTORAGE3DPROC)m_fn_glTextureStorage3D))(texture,levels,internalFormat,width,height,depth);
}

void OOGL::StateFns::call_glTextureStorage3DEXT(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(*((PFNGLTEXTURESTORAGE3DEXTPROC)m_fn_glTextureStorage3D))(texture,target,levels,internalFormat,width,height,depth);
}

void OOGL::StateFns::call_glTexStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLTEXSTORAGE3DPROC)m_fn_glTextureStorage3D))(target,levels,internalFormat,width,height,depth);
}

void OOGL::StateFns::glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(this->*m_thunk_glTextureStorage3D)(state,texture,target,levels,internalFormat,width,height,depth);
}

void OOGL::StateFns::check_glTextureSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage1D = glfwGetProcAddress("glTextureSubImage1D");
		if (m_fn_glTextureSubImage1D)
			m_thunk_glTextureSubImage1D = &StateFns::call_glTextureSubImage1D;
	}

	if (!m_fn_glTextureSubImage1D && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage1D = glfwGetProcAddress("glTextureSubImage1DEXT");
		if (m_fn_glTextureSubImage1D)
			m_thunk_glTextureSubImage1D = &StateFns::call_glTextureSubImage1DEXT;
	}

	if (!m_fn_glTextureSubImage1D)
		m_thunk_glTextureSubImage1D = &StateFns::call_glTexSubImage1D;

	(this->*m_thunk_glTextureSubImage1D)(state,texture,target,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage1D(State&, GLuint texture, GLenum, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE1DPROC)m_fn_glTextureSubImage1D))(texture,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage1DEXT(State&, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE1DEXTPROC)m_fn_glTextureSubImage1D))(texture,target,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::call_glTexSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	glTexSubImage1D(target,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::glTextureSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage1D)(state,texture,target,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::check_glTextureSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage2D = glfwGetProcAddress("glTextureSubImage2D");
		if (m_fn_glTextureSubImage2D)
			m_thunk_glTextureSubImage2D = &StateFns::call_glTextureSubImage2D;
	}

	if (!m_fn_glTextureSubImage2D && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage2D = glfwGetProcAddress("glTextureSubImage2DEXT");
		if (m_fn_glTextureSubImage2D)
			m_thunk_glTextureSubImage2D = &StateFns::call_glTextureSubImage2DEXT;
	}

	if (!m_fn_glTextureSubImage2D)
		m_thunk_glTextureSubImage2D = &StateFns::call_glTexSubImage2D;

	(this->*m_thunk_glTextureSubImage2D)(state,texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage2D(State&, GLuint texture, GLenum, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE2DPROC)m_fn_glTextureSubImage2D))(texture,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage2DEXT(State&, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE2DEXTPROC)m_fn_glTextureSubImage2D))(texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::call_glTexSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::glTextureSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage2D)(state,texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::check_glTextureSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage3D = glfwGetProcAddress("glTextureSubImage3D");
		if (m_fn_glTextureSubImage3D)
			m_thunk_glTextureSubImage3D = &StateFns::call_glTextureSubImage3D;
	}

	if (!m_fn_glTextureSubImage3D && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage3D = glfwGetProcAddress("glTextureSubImage3DEXT");
		if (m_fn_glTextureSubImage3D)
			m_thunk_glTextureSubImage3D = &StateFns::call_glTextureSubImage3DEXT;
	}

	if (!m_fn_glTextureSubImage3D)
	{
		m_fn_glTextureSubImage3D = glfwGetProcAddress("glTexSubImage3D");
		if (!m_fn_glTextureSubImage3D)
		{
			LOG_ERROR(("No glTexSubImage3D function"));
			return;
		}

		m_thunk_glTextureSubImage3D = &StateFns::call_glTexSubImage3D;
	}

	(this->*m_thunk_glTextureSubImage3D)(state,texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage3D(State&, GLuint texture, GLenum, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE3DPROC)m_fn_glTextureSubImage3D))(texture,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage3DEXT(State&, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE3DEXTPROC)m_fn_glTextureSubImage3D))(texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::call_glTexSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLTEXSUBIMAGE3DPROC)m_fn_glTextureSubImage3D))(target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::glTextureSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage3D)(state,texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::check_glTextureParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameterf = glfwGetProcAddress("glTextureParameterf");
		if (m_fn_glTextureParameterf)
			m_thunk_glTextureParameterf = &StateFns::call_glTextureParameterf;
	}

	if (!m_fn_glTextureParameterf && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameterf = glfwGetProcAddress("glTextureParameterfEXT");
		if (m_fn_glTextureParameterf)
			m_thunk_glTextureParameterf = &StateFns::call_glTextureParameterfEXT;
	}

	if (!m_fn_glTextureParameterf)
		m_thunk_glTextureParameterf = &StateFns::call_glTexParameterf;

	(this->*m_thunk_glTextureParameterf)(state,texture,target,name,val);
}

void OOGL::StateFns::call_glTextureParameterf(State&, GLuint texture, GLenum, GLenum name, GLfloat val)
{
	(*((PFNGLTEXTUREPARAMETERFPROC)m_fn_glTextureParameterf))(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameterfEXT(State&, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	(*((PFNGLTEXTUREPARAMETERFEXTPROC)m_fn_glTextureParameterf))(texture,target,name,val);
}

void OOGL::StateFns::call_glTexParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	glTexParameterf(target,name,val);
}

void OOGL::StateFns::glTextureParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	(this->*m_thunk_glTextureParameterf)(state,texture,target,name,val);
}

void OOGL::StateFns::check_glTextureParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameterfv = glfwGetProcAddress("glTextureParameterfv");
		if (m_fn_glTextureParameterfv)
			m_thunk_glTextureParameterfv = &StateFns::call_glTextureParameterfv;
	}

	if (!m_fn_glTextureParameterfv && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameterfv = glfwGetProcAddress("glTextureParameterfvEXT");
		if (m_fn_glTextureParameterfv)
			m_thunk_glTextureParameterfv = &StateFns::call_glTextureParameterfvEXT;
	}

	if (!m_fn_glTextureParameterfv)
		m_thunk_glTextureParameterfv = &StateFns::call_glTexParameterfv;

	(this->*m_thunk_glTextureParameterfv)(state,texture,target,name,val);
}

void OOGL::StateFns::call_glTextureParameterfv(State&, GLuint texture, GLenum, GLenum name, const GLfloat* val)
{
	(*((PFNGLTEXTUREPARAMETERFVPROC)m_fn_glTextureParameterfv))(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameterfvEXT(State&, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	(*((PFNGLTEXTUREPARAMETERFVEXTPROC)m_fn_glTextureParameterfv))(texture,target,name,val);
}

void OOGL::StateFns::call_glTexParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	glTexParameterfv(target,name,val);
}

void OOGL::StateFns::glTextureParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	(this->*m_thunk_glTextureParameterfv)(state,texture,target,name,val);
}

void OOGL::StateFns::check_glTextureParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameteri = glfwGetProcAddress("glTextureParameteri");
		if (m_fn_glTextureParameteri)
			m_thunk_glTextureParameteri = &StateFns::call_glTextureParameteri;
	}

	if (!m_fn_glTextureParameteri && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameteri = glfwGetProcAddress("glTextureParameteriEXT");
		if (m_fn_glTextureParameteri)
			m_thunk_glTextureParameteri = &StateFns::call_glTextureParameteriEXT;
	}

	if (!m_fn_glTextureParameteri)
		m_thunk_glTextureParameteri = &StateFns::call_glTexParameteri;

	(this->*m_thunk_glTextureParameteri)(state,texture,target,name,val);
}

void OOGL::StateFns::call_glTextureParameteri(State&, GLuint texture, GLenum, GLenum name, GLint val)
{
	(*((PFNGLTEXTUREPARAMETERIPROC)m_fn_glTextureParameteri))(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameteriEXT(State&, GLuint texture, GLenum target, GLenum name, GLint val)
{
	(*((PFNGLTEXTUREPARAMETERIEXTPROC)m_fn_glTextureParameteri))(texture,target,name,val);
}

void OOGL::StateFns::call_glTexParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	glTexParameteri(target,name,val);
}

void OOGL::StateFns::glTextureParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val)
{
	(this->*m_thunk_glTextureParameteri)(state,texture,target,name,val);
}

void OOGL::StateFns::check_glTextureParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameteriv = glfwGetProcAddress("glTextureParameteriv");
		if (m_fn_glTextureParameteriv)
			m_thunk_glTextureParameteriv = &StateFns::call_glTextureParameteriv;
	}

	if (!m_fn_glTextureParameteriv && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameteriv = glfwGetProcAddress("glTextureParameterivEXT");
		if (m_fn_glTextureParameteriv)
			m_thunk_glTextureParameteriv = &StateFns::call_glTextureParameterivEXT;
	}

	if (!m_fn_glTextureParameteriv)
		m_thunk_glTextureParameteriv = &StateFns::call_glTexParameteriv;

	(this->*m_thunk_glTextureParameteriv)(state,texture,target,name,val);
}

void OOGL::StateFns::call_glTextureParameteriv(State&, GLuint texture, GLenum, GLenum name, const GLint* val)
{
	(*((PFNGLTEXTUREPARAMETERIVPROC)m_fn_glTextureParameteriv))(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameterivEXT(State&, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	(*((PFNGLTEXTUREPARAMETERIVEXTPROC)m_fn_glTextureParameteriv))(texture,target,name,val);
}

void OOGL::StateFns::call_glTexParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	glTexParameteriv(target,name,val);
}

void OOGL::StateFns::glTextureParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	(this->*m_thunk_glTextureParameteriv)(state,texture,target,name,val);
}

bool OOGL::StateFns::check_glGenerateMipmap()
{
	if (!m_fn_glGenerateTextureMipmap)
	{
		if (glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
		{
			m_fn_glGenerateTextureMipmap = glfwGetProcAddress("glGenerateTextureMipmap");
			if (m_fn_glGenerateTextureMipmap)
				m_thunk_glGenerateTextureMipmap = &StateFns::call_glGenerateTextureMipmap;
		}

		if (!m_fn_glGenerateTextureMipmap && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glGenerateTextureMipmap = glfwGetProcAddress("glGenerateTextureMipmapEXT");
			if (m_fn_glGenerateTextureMipmap)
				m_thunk_glGenerateTextureMipmap = &StateFns::call_glGenerateTextureMipmapEXT;
		}

		if (!m_fn_glGenerateTextureMipmap && (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE))
		{
			m_fn_glGenerateTextureMipmap = glfwGetProcAddress("glGenerateMipmap");
			if (m_fn_glGenerateTextureMipmap)
				m_thunk_glGenerateTextureMipmap = &StateFns::call_glGenerateMipmap;
		}
		
		if (!m_fn_glGenerateTextureMipmap && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
		{
			m_fn_glGenerateTextureMipmap = glfwGetProcAddress("glGenerateMipmapEXT");
			if (m_fn_glGenerateTextureMipmap)
				m_thunk_glGenerateTextureMipmap = &StateFns::call_glGenerateMipmap;
		}
	}

	return (m_fn_glGenerateTextureMipmap != NULL);
}

void OOGL::StateFns::check_glGenerateTextureMipmap(State& state, GLuint texture, GLenum target)
{
	if (!check_glGenerateMipmap())
		LOG_ERROR(("No glGenerateMipmap function!"));
	else
		(this->*m_thunk_glGenerateTextureMipmap)(state,texture,target);
}

void OOGL::StateFns::call_glGenerateTextureMipmap(State&, GLuint texture, GLenum)
{
	(*((PFNGLGENERATETEXTUREMIPMAPPROC)m_fn_glGenerateTextureMipmap))(texture);
}

void OOGL::StateFns::call_glGenerateTextureMipmapEXT(State&, GLuint texture, GLenum target)
{
	(*((PFNGLGENERATETEXTUREMIPMAPEXTPROC)m_fn_glGenerateTextureMipmap))(texture,target);
}

void OOGL::StateFns::call_glGenerateMipmap(State& state, GLuint texture, GLenum target)
{
	state.bind_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLGENERATEMIPMAPPROC)m_fn_glGenerateTextureMipmap))(target);
}

void OOGL::StateFns::glGenerateTextureMipmap(State& state, GLuint texture, GLenum target)
{
	(this->*m_thunk_glGenerateTextureMipmap)(state,texture,target);
}

void OOGL::StateFns::glGenBuffers(GLsizei n, GLuint* buffers)
{
	if (!m_fn_glGenBuffers)
		m_fn_glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");

	if (!m_fn_glGenBuffers)
		LOG_ERROR(("No glGenBuffers function"));
	else
		(*m_fn_glGenBuffers)(n,buffers);
}

void OOGL::StateFns::glBindBuffer(GLenum target, GLuint buffer)
{
	if (!m_fn_glBindBuffer)
		m_fn_glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");

	if (!m_fn_glBindBuffer)
		LOG_ERROR(("No glBindBuffer function"));
	else
		(*m_fn_glBindBuffer)(target,buffer);
}

void OOGL::StateFns::glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	if (!m_fn_glDeleteBuffers)
		m_fn_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers");

	if (!m_fn_glDeleteBuffers)
		LOG_ERROR(("No glDeleteBuffers function"));
	else
		(*m_fn_glDeleteBuffers)(n,buffers);
}

void OOGL::StateFns::check_glBufferData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLsizei size, const void *data, GLenum usage)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glBufferData = glfwGetProcAddress("glNamedBufferData");
		if (m_fn_glBufferData)
			m_thunk_glBufferData = &StateFns::call_glNamedBufferData;
	}

	if (!m_fn_glBufferData && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glBufferData = glfwGetProcAddress("glNamedBufferDataEXT");
		if (m_fn_glBufferData)
			m_thunk_glBufferData = &StateFns::call_glNamedBufferData;
	}

	if (!m_fn_glBufferData)
	{
		m_fn_glBufferData = glfwGetProcAddress("glBufferData");
		if (!m_fn_glBufferData)
			LOG_ERROR(("No glBufferData function"));
		else
			m_thunk_glBufferData = &StateFns::call_glBufferData;
	}

	(this->*m_thunk_glBufferData)(state,buffer,size,data,usage);
}

void OOGL::StateFns::call_glNamedBufferData(State&, const OOBase::SharedPtr<BufferObject>& buffer, GLsizei size, const void *data, GLenum usage)
{
	(*((PFNGLNAMEDBUFFERDATAPROC)m_fn_glBufferData))(buffer->m_buffer,size,data,usage);
}

void OOGL::StateFns::call_glBufferData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLsizei size, const void *data, GLenum usage)
{
	state.bind(buffer);

	(*((PFNGLBUFFERDATAPROC)m_fn_glBufferData))(buffer->m_target,size,data,usage);
}

void OOGL::StateFns::glBufferData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLsizei size, const void *data, GLenum usage)
{
	(this->*m_thunk_glBufferData)(state,buffer,size,data,usage);
}

void* OOGL::StateFns::check_glMapBufferRange(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizei orig_size, GLbitfield access)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glMapBufferRange = glfwGetProcAddress("glMapNamedBufferRange");
		if (m_fn_glMapBufferRange)
			m_thunk_glMapBufferRange = &StateFns::call_glMapNamedBufferRange;
	}

	if (!m_fn_glMapBufferRange && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glMapBufferRange = glfwGetProcAddress("glMapNamedBufferRangeEXT");
		if (m_fn_glMapBufferRange)
			m_thunk_glMapBufferRange = &StateFns::call_glMapNamedBufferRange;
	}

	if (!m_fn_glMapBufferRange)
	{
		m_fn_glMapBufferRange = glfwGetProcAddress("glMapBufferRange");
		if (m_fn_glMapBufferRange)
			m_thunk_glMapBufferRange = &StateFns::call_glMapBufferRange;
	}

	if (!m_fn_glMapBufferRange)
	{
		m_fn_glMapBufferRange = glfwGetProcAddress("glMapBuffer");
		if (!m_fn_glMapBufferRange)
			LOG_ERROR(("No glMapBuffer function"));
		else
			m_thunk_glMapBufferRange = &StateFns::call_glMapBuffer;
	}

	return (this->*m_thunk_glMapBufferRange)(state,buffer,offset,length,orig_usage,orig_size,access);
}

void* OOGL::StateFns::call_glMapNamedBufferRange(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizei orig_size, GLbitfield access)
{
	return (*((PFNGLMAPNAMEDBUFFERRANGEPROC)m_fn_glMapBufferRange))(buffer->m_buffer,offset,length,access);
}

void* OOGL::StateFns::call_glMapBufferRange(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizei orig_size, GLbitfield access)
{
	state.bind(buffer);

	return (*((PFNGLMAPBUFFERRANGEPROC)m_fn_glMapBufferRange))(buffer->m_target,offset,length,access);
}

void* OOGL::StateFns::call_glMapBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizei orig_size, GLbitfield access)
{
	if (access & GL_MAP_INVALIDATE_BUFFER_BIT)
		glBufferData(state,buffer,orig_size,NULL,orig_usage);
	
	state.bind(buffer);
	
	OOBase::uint8_t* ret = static_cast<OOBase::uint8_t*>((*((PFNGLMAPBUFFERPROC)m_fn_glMapBufferRange))(buffer->m_target,access));
	if (ret)
		ret += offset;

	return ret;
}

void* OOGL::StateFns::glMapBufferRange(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizei orig_size, GLbitfield access)
{
	return (this->*m_thunk_glMapBufferRange)(state,buffer,offset,length,orig_usage,orig_size,access);
}

bool OOGL::StateFns::check_glUnmapBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glUnmapBuffer = glfwGetProcAddress("glUnmapNamedBuffer");
		if (m_fn_glUnmapBuffer)
			m_thunk_glUnmapBuffer = &StateFns::call_glUnmapNamedBuffer;
	}

	if (!m_fn_glUnmapBuffer && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glUnmapBuffer = glfwGetProcAddress("glUnmapNamedBufferEXT");
		if (m_fn_glUnmapBuffer)
			m_thunk_glUnmapBuffer = &StateFns::call_glUnmapNamedBuffer;
	}

	if (!m_fn_glUnmapBuffer)
	{
		m_fn_glUnmapBuffer = glfwGetProcAddress("glUnmapBuffer");
		if (!m_fn_glUnmapBuffer)
			LOG_ERROR(("No glUnmapBuffer function"));
		else
			m_thunk_glUnmapBuffer = &StateFns::call_glUnmapBuffer;
	}

	return (this->*m_thunk_glUnmapBuffer)(state,buffer);
}

bool OOGL::StateFns::call_glUnmapNamedBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer)
{
	return (*((PFNGLUNMAPNAMEDBUFFERPROC)m_fn_glUnmapBuffer))(buffer->m_buffer) == GL_TRUE;
}

bool OOGL::StateFns::call_glUnmapBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer)
{
	state.bind(buffer);

	return (*((PFNGLUNMAPBUFFERPROC)m_fn_glUnmapBuffer))(buffer->m_target) == GL_TRUE;
}

bool OOGL::StateFns::glUnmapBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer)
{
	return (this->*m_thunk_glUnmapBuffer)(state,buffer);
}

void OOGL::StateFns::check_glBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei size, const void* data)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glBufferSubData = glfwGetProcAddress("glNamedBufferSubData");
		if (m_fn_glBufferSubData)
			m_thunk_glBufferSubData = &StateFns::call_glNamedBufferSubData;
	}

	if (!m_fn_glBufferSubData && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glBufferSubData = glfwGetProcAddress("glNamedBufferSubDataEXT");
		if (m_fn_glBufferSubData)
			m_thunk_glBufferSubData = &StateFns::call_glNamedBufferSubData;
	}

	if (!m_fn_glBufferSubData)
	{
		m_fn_glBufferSubData = glfwGetProcAddress("glBufferSubData");
		if (!m_fn_glBufferSubData)
			LOG_ERROR(("No glBufferSubData function"));
		else
			m_thunk_glBufferSubData = &StateFns::call_glBufferSubData;
	}

	(this->*m_thunk_glBufferSubData)(state,buffer,offset,size,data);
}

void OOGL::StateFns::call_glNamedBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei size, const void* data)
{
	(*((PFNGLNAMEDBUFFERSUBDATAPROC)m_fn_glBufferSubData))(buffer->m_buffer,offset,size,data);
}

void OOGL::StateFns::call_glBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei size, const void* data)
{
	state.bind(buffer);

	(*((PFNGLBUFFERSUBDATAPROC)m_fn_glBufferSubData))(buffer->m_target,offset,size,data);
}

void OOGL::StateFns::glBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei size, const void* data)
{
	(this->*m_thunk_glBufferSubData)(state,buffer,offset,size,data);
}

void OOGL::StateFns::check_glCopyBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size)
{
	if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
	{
		m_fn_glCopyBufferSubData = glfwGetProcAddress("glCopyNamedBufferSubData");
		if (m_fn_glCopyBufferSubData)
			m_thunk_glCopyBufferSubData = &StateFns::call_glCopyNamedBufferSubData;
	}

	if (!m_fn_glCopyBufferSubData && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glCopyBufferSubData = glfwGetProcAddress("glCopyNamedBufferSubDataEXT");
		if (m_fn_glCopyBufferSubData)
			m_thunk_glCopyBufferSubData = &StateFns::call_glCopyNamedBufferSubData;
	}

	if (!m_fn_glCopyBufferSubData)
	{
		m_fn_glCopyBufferSubData = glfwGetProcAddress("glCopyBufferSubData");
		if (m_fn_glCopyBufferSubData)
			m_thunk_glCopyBufferSubData = &StateFns::call_glCopyBufferSubData;
	}

	if (!m_fn_glCopyBufferSubData)
	{
		LOG_DEBUG(("Using emulated glCopyBufferSubData function"));
		m_thunk_glCopyBufferSubData = &StateFns::emulate_glCopyBufferSubData;
	}

	(this->*m_thunk_glCopyBufferSubData)(state,write,writeoffset,read,readoffset,size);
}

void OOGL::StateFns::call_glCopyNamedBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size)
{
	(*((PFNGLCOPYNAMEDBUFFERSUBDATAPROC)m_fn_glCopyBufferSubData))(read->m_buffer,write->m_buffer,readoffset,writeoffset,size);
}

void OOGL::StateFns::call_glCopyBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size)
{
	state.bind(read,GL_COPY_READ_BUFFER);
	state.bind(write,GL_COPY_WRITE_BUFFER);

	(*((PFNGLCOPYBUFFERSUBDATAPROC)m_fn_glCopyBufferSubData))(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,readoffset,writeoffset,size);
}

void OOGL::StateFns::emulate_glCopyBufferSubData(State&, const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size)
{
	OOBase::SharedPtr<OOBase::uint8_t> r = read->auto_map<OOBase::uint8_t>(GL_READ_ONLY,readoffset);
	OOBase::SharedPtr<OOBase::uint8_t> w = write->auto_map<OOBase::uint8_t>(GL_WRITE_ONLY,writeoffset);

	memcpy(w.get(),r.get(),size);
}

void OOGL::StateFns::glCopyBufferSubData(State& state, const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size)
{
	(this->*m_thunk_glCopyBufferSubData)(state,write,writeoffset,read,readoffset,size);
}

void OOGL::StateFns::glGenVertexArrays(GLsizei n, GLuint* arrays)
{
	if (!m_fn_glGenVertexArrays)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_vertex_array_object") == GL_TRUE)
			m_fn_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	}

	if (!m_fn_glGenVertexArrays)
		LOG_ERROR(("No glGenVertexArrays function"));
	else
		(*m_fn_glGenVertexArrays)(n,arrays);
}

void OOGL::StateFns::glBindVertexArray(GLuint array)
{
	if (!m_fn_glBindVertexArray)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_vertex_array_object") == GL_TRUE)
			m_fn_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	}

	if (!m_fn_glBindVertexArray)
		LOG_ERROR(("No glBindVertexArray function"));
	else
		(*m_fn_glBindVertexArray)(array);
}

void OOGL::StateFns::glDeleteVertexArrays(GLsizei n, const GLuint* arrays)
{
	if (!m_fn_glDeleteVertexArrays)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_vertex_array_object") == GL_TRUE)
			m_fn_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
	}

	if (!m_fn_glDeleteVertexArrays)
		LOG_ERROR(("No glDeleteVertexArrays function"));
	else
		(*m_fn_glDeleteVertexArrays)(n,arrays);
}

void OOGL::StateFns::check_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
{
	m_fn_glMultiDrawArrays = glfwGetProcAddress("glMultiDrawArrays");
	if (m_fn_glMultiDrawArrays)
		m_thunk_glMultiDrawArrays = &StateFns::call_glMultiDrawArrays;
	
	if (!m_fn_glMultiDrawArrays && glfwExtensionSupported("GL_EXT_multi_draw_arrays") == GL_TRUE)
	{
		m_fn_glMultiDrawArrays = glfwGetProcAddress("glMultiDrawArraysEXT");
		if (m_fn_glMultiDrawArrays)
			m_thunk_glMultiDrawArrays = &StateFns::call_glMultiDrawArrays;
	}

	if (!m_fn_glMultiDrawArrays)
		m_thunk_glMultiDrawArrays = &StateFns::emulate_glMultiDrawArrays;
	
	(this->*m_thunk_glMultiDrawArrays)(mode,first,count,primcount);
}

void OOGL::StateFns::call_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
{
	(*((PFNGLMULTIDRAWARRAYSPROC)m_fn_glMultiDrawArrays))(mode,first,count,primcount);
}

void OOGL::StateFns::emulate_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
{
	for (GLsizei i = 0; i < primcount; ++i)
	{
		if (count[i] > 0)
			glDrawArrays(mode,first[i],count[i]);
	}
}

void OOGL::StateFns::glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount)
{
	(this->*m_thunk_glMultiDrawArrays)(mode,first,count,primcount);
}

bool OOGL::StateFns::check_glDrawInstanced()
{
	if (!m_fn_glDrawArraysInstanced && isGLversion(3,1))
	{
		m_fn_glDrawArraysInstanced = glfwGetProcAddress("glDrawArraysInstanced");
		if (m_fn_glDrawArraysInstanced)
			m_thunk_glDrawArraysInstanced = &StateFns::call_glDrawArraysInstanced;
	}

	if (!m_fn_glDrawArraysInstanced && glfwExtensionSupported("GL_ARB_draw_instanced") == GL_TRUE)
	{
		m_fn_glDrawArraysInstanced = glfwGetProcAddress("glDrawArraysInstancedARB");
		if (m_fn_glDrawArraysInstanced)
			m_thunk_glDrawArraysInstanced = &StateFns::call_glDrawArraysInstanced;
	}

	if (!m_fn_glDrawArraysInstanced && glfwExtensionSupported("GL_EXT_draw_instanced") == GL_TRUE)
	{
		m_fn_glDrawArraysInstanced = glfwGetProcAddress("glDrawArraysInstancedEXT");
		if (m_fn_glDrawArraysInstanced)
			m_thunk_glDrawArraysInstanced = &StateFns::call_glDrawArraysInstanced;
	}

	if (!m_fn_glDrawArraysInstanced && glfwExtensionSupported("GL_NV_draw_instanced") == GL_TRUE)
	{
		m_fn_glDrawArraysInstanced = glfwGetProcAddress("glDrawArraysInstancedNV");
		if (m_fn_glDrawArraysInstanced)
			m_thunk_glDrawArraysInstanced = &StateFns::call_glDrawArraysInstanced;
	}

	return (m_fn_glDrawArraysInstanced != NULL);
}

void OOGL::StateFns::check_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instances)
{
	if (!check_glDrawInstanced())
		LOG_ERROR(("No glDrawArraysInstanced function"));
	else
		(this->*m_thunk_glDrawArraysInstanced)(mode,first,count,instances);
}

void OOGL::StateFns::call_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instances)
{
	(*((PFNGLDRAWARRAYSINSTANCEDPROC)m_fn_glDrawArraysInstanced))(mode,first,count,instances);
}

void OOGL::StateFns::glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instances)
{
	(this->*m_thunk_glDrawArraysInstanced)(mode,first,count,instances);
}

bool OOGL::StateFns::check_glDrawInstancedBaseInstance()
{
	if (!m_fn_glDrawArraysInstancedBaseInstance && (isGLversion(4,2) || glfwExtensionSupported("GL_ARB_draw_instanced") == GL_TRUE))
	{
		m_fn_glDrawArraysInstancedBaseInstance = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)glfwGetProcAddress("glDrawArraysInstancedBaseInstance");
		if (m_fn_glDrawArraysInstancedBaseInstance)
			m_thunk_glDrawArraysInstancedBaseInstance = &StateFns::call_glDrawArraysInstancedBaseInstance;
	}

	return (m_fn_glDrawArraysInstancedBaseInstance != NULL);
}

void OOGL::StateFns::check_glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance)
{
	if (!check_glDrawInstancedBaseInstance())
		LOG_ERROR(("No glDrawArraysInstancedBaseInstance function"));
	else
		(this->*m_thunk_glDrawArraysInstancedBaseInstance)(mode,first,count,instances,baseinstance);
}

void OOGL::StateFns::call_glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance)
{
	(*m_fn_glDrawArraysInstancedBaseInstance)(mode,first,count,instances,baseinstance);
}

void OOGL::StateFns::glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance)
{
	(this->*m_thunk_glDrawArraysInstancedBaseInstance)(mode,first,count,instances,baseinstance);
}

void OOGL::StateFns::check_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset)
{
	m_fn_glDrawRangeElements = glfwGetProcAddress("glDrawRangeElements");
	if (m_fn_glDrawRangeElements)
		m_thunk_glDrawRangeElements = &StateFns::call_glDrawRangeElements;
	
	if (!m_fn_glDrawRangeElements && glfwExtensionSupported("GL_EXT_draw_range_elements") == GL_TRUE)
	{
		m_fn_glDrawRangeElements = glfwGetProcAddress("glDrawRangeElementsEXT");
		if (m_fn_glDrawRangeElements)
			m_thunk_glDrawRangeElements = &StateFns::call_glDrawRangeElements;
	}

	if (!m_fn_glDrawRangeElements)
		LOG_ERROR(("No glDrawRangeElements function"));
	else
		(this->*m_thunk_glDrawRangeElements)(mode,start,end,count,type,offset);
}

void OOGL::StateFns::call_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset)
{
	(*((PFNGLDRAWRANGEELEMENTSPROC)m_fn_glDrawRangeElements))(mode,start,end,count,type,(const void*)offset);
}

void OOGL::StateFns::glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset)
{
	(this->*m_thunk_glDrawRangeElements)(mode,start,end,count,type,offset);
}

bool OOGL::StateFns::check_glDrawRangeElementsBaseVertex()
{
	if (!m_fn_glDrawRangeElementsBaseVertex && (isGLversion(3,2) || glfwExtensionSupported("GL_ARB_draw_elements_base_vertex") == GL_TRUE))
	{
		m_fn_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)glfwGetProcAddress("glDrawRangeElementsBaseVertex");
		if (m_fn_glDrawRangeElementsBaseVertex)
			m_thunk_glDrawRangeElementsBaseVertex = &StateFns::call_glDrawRangeElementsBaseVertex;
	}

	return (m_fn_glDrawRangeElementsBaseVertex != NULL);
}

void OOGL::StateFns::check_glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	if (!check_glDrawRangeElementsBaseVertex())
		LOG_ERROR(("No glDrawRangeElementsBaseVertex function"));
	else
		(this->*m_thunk_glDrawRangeElementsBaseVertex)(mode,start,end,count,type,offset,basevertex);
}

void OOGL::StateFns::call_glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	(*m_fn_glDrawRangeElementsBaseVertex)(mode,start,end,count,type,(const void*)offset,basevertex);
}

void OOGL::StateFns::glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	(this->*m_thunk_glDrawRangeElementsBaseVertex)(mode,start,end,count,type,offset,basevertex);
}

bool OOGL::StateFns::check_glDrawElementsBaseVertex()
{
	if (!m_fn_glDrawElementsBaseVertex && (isGLversion(3,2) || glfwExtensionSupported("GL_ARB_draw_elements_base_vertex") == GL_TRUE))
	{
		m_fn_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)glfwGetProcAddress("glDrawElementsBaseVertex");
		if (m_fn_glDrawElementsBaseVertex)
			m_thunk_glDrawElementsBaseVertex = &StateFns::call_glDrawElementsBaseVertex;
	}

	return (m_fn_glDrawElementsBaseVertex != NULL);
}

void OOGL::StateFns::check_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	if (!check_glDrawElementsBaseVertex())
		LOG_ERROR(("No glDrawElementsBaseVertex function"));
	else
		(this->*m_thunk_glDrawElementsBaseVertex)(mode,count,type,offset,basevertex);
}

void OOGL::StateFns::call_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	(*m_fn_glDrawElementsBaseVertex)(mode,count,type,(const void*)offset,basevertex);
}

void OOGL::StateFns::glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	(this->*m_thunk_glDrawElementsBaseVertex)(mode,count,type,offset,basevertex);
}

void OOGL::StateFns::check_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount)
{
	if (!m_fn_glMultiDrawElements)
	{
		m_fn_glMultiDrawElements = glfwGetProcAddress("glMultiDrawElements");
		if (m_fn_glMultiDrawElements)
			m_thunk_glMultiDrawElements = &StateFns::call_glMultiDrawElements;
	}

	if (!m_fn_glMultiDrawElements && glfwExtensionSupported("GL_EXT_multi_draw_arrays") == GL_TRUE)
	{
		m_fn_glMultiDrawElements = glfwGetProcAddress("glMultiDrawElementsEXT");
		if (m_fn_glMultiDrawElements)
			m_thunk_glMultiDrawElements = &StateFns::call_glMultiDrawElements;
	}

	if (!m_fn_glMultiDrawElements)
		m_thunk_glMultiDrawElements = &StateFns::emulate_glMultiDrawElements;
	
	(this->*m_thunk_glMultiDrawElements)(mode,count,type,offsets,primcount);
}

void OOGL::StateFns::call_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount)
{
	(*((PFNGLMULTIDRAWELEMENTSPROC)m_fn_glMultiDrawElements))(mode,count,type,(const void* const*)offsets,primcount);
}

void OOGL::StateFns::emulate_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount)
{
	for (GLsizei i = 0; i < primcount; ++i)
	{
		if (count[i] > 0)
			glDrawElements(mode,count[i],type,(const void*)(offsets[i]));
	}
}

void OOGL::StateFns::glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount)
{
	(this->*m_thunk_glMultiDrawElements)(mode,count,type,offsets,primcount);
}

void OOGL::StateFns::check_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint *basevertex)
{
	if (isGLversion(3,2) || glfwExtensionSupported("GL_ARB_draw_elements_base_vertex") == GL_TRUE)
	{
		m_fn_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)glfwGetProcAddress("glMultiDrawElementsBaseVertex");
		if (m_fn_glMultiDrawElementsBaseVertex)
			m_thunk_glMultiDrawElementsBaseVertex = &StateFns::call_glMultiDrawElementsBaseVertex;
	}

	if (!m_fn_glMultiDrawElementsBaseVertex)
		m_thunk_glMultiDrawElementsBaseVertex = &StateFns::emulate_glMultiDrawElementsBaseVertex;
	
	(this->*m_thunk_glMultiDrawElementsBaseVertex)(mode,count,type,offsets,primcount,basevertex);
}

void OOGL::StateFns::call_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint *basevertex)
{
	(*m_fn_glMultiDrawElementsBaseVertex)(mode,count,type,(const void* const*)offsets,primcount,basevertex);
}

void OOGL::StateFns::emulate_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint *basevertex)
{
	for (GLsizei i = 0; i < primcount; ++i)
	{
		if (count[i] > 0)
			glDrawElementsBaseVertex(mode,count[i],type,offsets[i],basevertex[i]);
	}
}

void OOGL::StateFns::glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint *basevertex)
{
	(this->*m_thunk_glMultiDrawElementsBaseVertex)(mode,count,type,offsets,primcount,basevertex);
}

void OOGL::StateFns::check_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances)
{
	if (isGLversion(3,1))
	{
		m_fn_glDrawElementsInstanced = glfwGetProcAddress("glDrawElementsInstanced");
		if (m_fn_glDrawElementsInstanced)
			m_thunk_glDrawElementsInstanced = &StateFns::call_glDrawElementsInstanced;
	}

	if (!m_fn_glDrawElementsInstanced && glfwExtensionSupported("GL_ARB_draw_instanced") == GL_TRUE)
	{
		m_fn_glDrawElementsInstanced = glfwGetProcAddress("glDrawElementsInstancedARB");
		if (m_fn_glDrawElementsInstanced)
			m_thunk_glDrawElementsInstanced = &StateFns::call_glDrawElementsInstanced;
	}

	if (!m_fn_glDrawElementsInstanced && glfwExtensionSupported("GL_EXT_draw_instanced") == GL_TRUE)
	{
		m_fn_glDrawElementsInstanced = glfwGetProcAddress("glDrawElementsInstancedEXT");
		if (m_fn_glDrawElementsInstanced)
			m_thunk_glDrawElementsInstanced = &StateFns::call_glDrawElementsInstanced;
	}

	if (!m_fn_glDrawElementsInstanced && glfwExtensionSupported("GL_NV_draw_instanced") == GL_TRUE)
	{
		m_fn_glDrawElementsInstanced = glfwGetProcAddress("glDrawElementsInstancedNV");
		if (m_fn_glDrawElementsInstanced)
			m_thunk_glDrawElementsInstanced = &StateFns::call_glDrawElementsInstanced;
	}

	if (!m_fn_glDrawElementsInstanced)
		LOG_ERROR(("No glDrawElementsInstanced function"));
	else
		(this->*m_thunk_glDrawElementsInstanced)(mode,count,type,offset,instances);
}

void OOGL::StateFns::call_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances)
{
	(*((PFNGLDRAWELEMENTSINSTANCEDPROC)m_fn_glDrawElementsInstanced))(mode,count,type,(const void*)offset,instances);
}

void OOGL::StateFns::glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances)
{
	(this->*m_thunk_glDrawElementsInstanced)(mode,count,type,offset,instances);
}

bool OOGL::StateFns::check_glDrawElementsInstancedBaseVertex()
{
	if (!m_fn_glDrawElementsInstancedBaseVertex && (isGLversion(3,2) || glfwExtensionSupported("GL_ARB_draw_elements_base_vertex") == GL_TRUE))
	{
		m_fn_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)glfwGetProcAddress("glDrawElementsInstancedBaseVertex");
		if (m_fn_glDrawElementsInstancedBaseVertex)
			m_thunk_glDrawElementsInstancedBaseVertex = &StateFns::call_glDrawElementsInstancedBaseVertex;
	}

	return (m_fn_glDrawElementsInstancedBaseVertex != NULL);
}

void OOGL::StateFns::check_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex)
{
	if (!check_glDrawElementsInstancedBaseVertex())
		LOG_ERROR(("No glDrawElementsInstancedBaseVertex function"));
	else
		(this->*m_thunk_glDrawElementsInstancedBaseVertex)(mode,count,type,offset,instancecount,basevertex);
}

void OOGL::StateFns::call_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex)
{
	(*m_fn_glDrawElementsInstancedBaseVertex)(mode,count,type,(const void*)offset,instancecount,basevertex);
}

void OOGL::StateFns::glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex)
{
	(this->*m_thunk_glDrawElementsInstancedBaseVertex)(mode,count,type,offset,instancecount,basevertex);
}

bool OOGL::StateFns::check_glDrawElementsInstancedBaseInstance()
{
	if (!m_fn_glDrawElementsInstancedBaseInstance && (isGLversion(4,2) || glfwExtensionSupported("GL_ARB_base_instance") == GL_TRUE))
	{
		m_fn_glDrawElementsInstancedBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC)glfwGetProcAddress("glDrawElementsInstancedBaseInstance");
		if (m_fn_glDrawElementsInstancedBaseInstance)
			m_thunk_glDrawElementsInstancedBaseInstance = &StateFns::call_glDrawElementsInstancedBaseInstance;
	}

	return (m_fn_glDrawElementsInstancedBaseInstance != NULL);
}

void OOGL::StateFns::check_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLuint baseinstance)
{
	if (!check_glDrawElementsInstancedBaseInstance())
		LOG_ERROR(("No glDrawElementsInstancedBaseInstance function"));
	else
		(this->*m_thunk_glDrawElementsInstancedBaseInstance)(mode,count,type,offset,instancecount,baseinstance);
}

void OOGL::StateFns::call_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLuint baseinstance)
{
	(*m_fn_glDrawElementsInstancedBaseInstance)(mode,count,type,(const void*)offset,instancecount,baseinstance);
}

void OOGL::StateFns::glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLuint baseinstance)
{
	(this->*m_thunk_glDrawElementsInstancedBaseInstance)(mode,count,type,offset,instancecount,baseinstance);
}

bool OOGL::StateFns::check_glDrawElementsInstancedBaseVertexBaseInstance()
{
	if (!m_fn_glDrawElementsInstancedBaseVertexBaseInstance && (isGLversion(4,2) || glfwExtensionSupported("GL_ARB_base_instance") == GL_TRUE))
	{
		m_fn_glDrawElementsInstancedBaseVertexBaseInstance = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC)glfwGetProcAddress("glDrawElementsInstancedBaseVertexBaseInstance");
		if (m_fn_glDrawElementsInstancedBaseVertexBaseInstance)
			m_thunk_glDrawElementsInstancedBaseVertexBaseInstance = &StateFns::call_glDrawElementsInstancedBaseVertexBaseInstance;
	}

	return (m_fn_glDrawElementsInstancedBaseVertexBaseInstance != NULL);
}

void OOGL::StateFns::check_glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex, GLuint baseinstance)
{
	if (!check_glDrawElementsInstancedBaseInstance())
		LOG_ERROR(("No glDrawElementsInstancedBaseVertexBaseInstance function"));
	else
		(this->*m_thunk_glDrawElementsInstancedBaseVertexBaseInstance)(mode,count,type,offset,instancecount,basevertex,baseinstance);
}

void OOGL::StateFns::call_glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex, GLuint baseinstance)
{
	(*m_fn_glDrawElementsInstancedBaseVertexBaseInstance)(mode,count,type,(const void*)offset,instancecount,basevertex,baseinstance);
}

void OOGL::StateFns::glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex, GLuint baseinstance)
{
	(this->*m_thunk_glDrawElementsInstancedBaseVertexBaseInstance)(mode,count,type,offset,instancecount,basevertex,baseinstance);
}
