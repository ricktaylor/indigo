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
		m_thunk_glActiveTexture(&StateFns::check_glActiveTexture),
		m_fn_glActiveTexture(NULL),
		m_thunk_glBindMultiTexture(&StateFns::check_glBindMultiTexture),
		m_fn_glBindMultiTexture(NULL),
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
		m_fn_glTextureParameteriv(NULL)
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

void Indigo::StateFns::check_glActiveTexture(GLenum texture)
{
	if (!m_fn_glActiveTexture)
		m_fn_glActiveTexture = (PFNGLACTIVETEXTUREPROC)glfwGetProcAddress("glActiveTexture");

	if (!m_fn_glActiveTexture)
		LOG_ERROR(("No glActiveTexture function"));
	else
		(*m_fn_glActiveTexture)(texture);
}

void Indigo::StateFns::call_glActiveTexture(GLenum texture)
{
	(*m_fn_glActiveTexture)(texture);
}

void Indigo::StateFns::glActiveTexture(GLenum texture)
{
	(this->*m_thunk_glActiveTexture)(texture);
}

void Indigo::StateFns::check_glBindMultiTexture(State& state, GLenum unit, GLenum target, GLuint texture)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		m_fn_glBindMultiTexture = (PFNGLBINDMULTITEXTUREEXTPROC)glfwGetProcAddress("glBindMultiTextureEXT");

	if (!m_fn_glBindMultiTexture)
		m_thunk_glBindMultiTexture = &StateFns::emulate_glBindMultiTexture;
	else
		m_thunk_glBindMultiTexture = &StateFns::call_glBindMultiTexture;

	(this->*m_thunk_glBindMultiTexture)(state,unit,target,texture);
}

void Indigo::StateFns::call_glBindMultiTexture(State&, GLenum unit, GLenum target, GLuint texture)
{
	(*m_fn_glBindMultiTexture)(unit,target,texture);
}

void Indigo::StateFns::emulate_glBindMultiTexture(State& state, GLenum unit, GLenum target, GLuint texture)
{
	state.activate_texture_unit(unit);
	glBindTexture(target,texture);
}

void Indigo::StateFns::glBindMultiTexture(State& state, GLenum unit, GLenum target, GLuint texture)
{
	(this->*m_thunk_glBindMultiTexture)(state,unit,target,texture);
}

void Indigo::StateFns::glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
	if (!m_fn_glTexImage3D)
		m_fn_glTexImage3D = (PFNGLTEXIMAGE3DPROC)glfwGetProcAddress("glTexImage3D");

	if (!m_fn_glTexImage3D)
		LOG_ERROR(("No glTexImage3D function"));
	else
		(*m_fn_glTexImage3D)(target,level,internalformat,width,height,depth,border,format,type,pixels);
}

void Indigo::StateFns::check_glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	if (glfwExtensionSupported("GL_ARB_texture_storage") == GL_TRUE)
	{
		if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureStorage1D = glfwGetProcAddress("glTextureStorage1DEXT");
			if (m_thunk_glTextureStorage1D)
				m_thunk_glTextureStorage1D = &StateFns::call_glTextureStorage1DEXT;
		}

		if (!m_fn_glTextureStorage1D)
		{
			m_fn_glTextureStorage1D = glfwGetProcAddress("glTexStorage1D");
			if (m_fn_glTextureStorage1D)
				m_thunk_glTextureStorage1D = &StateFns::call_glTexStorage1D;
		}
	}

	if (!m_fn_glTextureStorage1D)
		m_thunk_glTextureStorage1D = &StateFns::emulate_glTextureStorage1D;

	(this->*m_thunk_glTextureStorage1D)(state,texture,target,levels,internalFormat,width);
}

void Indigo::StateFns::emulate_glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	// Keep in line with Texture::init
	for (GLsizei i = 0; i < levels; ++i)
	{
		glTexImage1D(target,i,internalFormat,width,0,0,0,NULL);
		width /= 2;
		if (!width)
			width = 1;
	}
}

void Indigo::StateFns::call_glTextureStorage1DEXT(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(*((PFNGLTEXTURESTORAGE1DEXTPROC)m_fn_glTextureStorage1D))(texture,target,levels,internalFormat,width);
}

void Indigo::StateFns::call_glTexStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLTEXSTORAGE1DPROC)m_fn_glTextureStorage1D))(target,levels,internalFormat,width);
}

void Indigo::StateFns::glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(this->*m_thunk_glTextureStorage1D)(state,texture,target,levels,internalFormat,width);
}

void Indigo::StateFns::check_glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	if (glfwExtensionSupported("GL_ARB_texture_storage") == GL_TRUE)
	{
		if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureStorage2D = glfwGetProcAddress("glTextureStorage2DEXT");
			if (m_thunk_glTextureStorage2D)
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
		m_thunk_glTextureStorage2D = &StateFns::emulate_glTextureStorage2D;

	(this->*m_thunk_glTextureStorage2D)(state,texture,target,levels,internalFormat,width,height);
}

void Indigo::StateFns::emulate_glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	// Keep in line with Texture::init
	for (GLsizei i = 0; i < levels; ++i)
	{
		if (target == GL_TEXTURE_CUBE_MAP)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,i,internalFormat,width,height,0,0,0,NULL);
		}
		else
			glTexImage2D(target,i,internalFormat,width,height,0,0,0,NULL);

		width /= 2;
		if (!width)
			width = 1;

		if (target != GL_TEXTURE_1D && target != GL_TEXTURE_1D_ARRAY)
		{
			height /= 2;
			if (!height)
				height = 1;
		}
	}
}

void Indigo::StateFns::call_glTextureStorage2DEXT(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(*((PFNGLTEXTURESTORAGE2DEXTPROC)m_fn_glTextureStorage2D))(texture,target,levels,internalFormat,width,height);
}

void Indigo::StateFns::call_glTexStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLTEXSTORAGE2DPROC)m_fn_glTextureStorage2D))(target,levels,internalFormat,width,height);
}

void Indigo::StateFns::glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(this->*m_thunk_glTextureStorage2D)(state,texture,target,levels,internalFormat,width,height);
}

void Indigo::StateFns::check_glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	if (glfwExtensionSupported("GL_ARB_texture_storage") == GL_TRUE)
	{
		if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureStorage3D = glfwGetProcAddress("glTextureStorage3DEXT");
			if (m_thunk_glTextureStorage3D)
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
		m_thunk_glTextureStorage3D = &StateFns::emulate_glTextureStorage3D;

	(this->*m_thunk_glTextureStorage3D)(state,texture,target,levels,internalFormat,width,height,depth);
}

void Indigo::StateFns::emulate_glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	// Keep in line with Texture::init
	for (GLsizei i = 0; i < levels; ++i)
	{
		this->glTexImage3D(target,i,internalFormat,width,height,depth,0,0,0,NULL);

		width /= 2;
		if (!width)
			width = 1;

		height /= 2;
		if (!height)
			height = 1;

		if (target != GL_TEXTURE_2D_ARRAY && target != GL_PROXY_TEXTURE_2D_ARRAY && target != GL_TEXTURE_CUBE_MAP_ARRAY && target != GL_PROXY_TEXTURE_CUBE_MAP_ARRAY)
		{
			depth /= 2;
			if (!depth)
				depth = 1;
		}
	}
}

void Indigo::StateFns::call_glTextureStorage3DEXT(State&, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(*((PFNGLTEXTURESTORAGE3DEXTPROC)m_fn_glTextureStorage3D))(texture,target,levels,internalFormat,width,height,depth);
}

void Indigo::StateFns::call_glTexStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	(*((PFNGLTEXSTORAGE3DPROC)m_fn_glTextureStorage3D))(target,levels,internalFormat,width,height,depth);
}

void Indigo::StateFns::glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(this->*m_thunk_glTextureStorage3D)(state,texture,target,levels,internalFormat,width,height,depth);
}

void Indigo::StateFns::check_glTextureSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage1D = glfwGetProcAddress("glTextureSubImage1DEXT");
		if (m_fn_glTextureSubImage1D)
			m_thunk_glTextureSubImage1D = &StateFns::call_glTextureSubImage1DEXT;
	}

	if (!m_fn_glTextureSubImage1D)
		m_thunk_glTextureSubImage1D = &StateFns::call_glTexSubImage1D;

	(this->*m_thunk_glTextureSubImage1D)(state,texture,target,level,xoffset,width,format,type,pixels);
}

void Indigo::StateFns::call_glTextureSubImage1DEXT(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE1DEXTPROC)m_fn_glTextureSubImage1D))(texture,target,level,xoffset,width,format,type,pixels);
}

void Indigo::StateFns::call_glTexSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	glTexSubImage1D(target,level,xoffset,width,format,type,pixels);
}

void Indigo::StateFns::glTextureSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage1D)(state,texture,target,level,xoffset,width,format,type,pixels);
}

void Indigo::StateFns::check_glTextureSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureSubImage2D = glfwGetProcAddress("glTextureSubImage2DEXT");
		if (m_fn_glTextureSubImage2D)
			m_thunk_glTextureSubImage2D = &StateFns::call_glTextureSubImage2DEXT;
	}

	if (!m_fn_glTextureSubImage2D)
		m_thunk_glTextureSubImage2D = &StateFns::call_glTexSubImage2D;

	(this->*m_thunk_glTextureSubImage2D)(state,texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::StateFns::call_glTextureSubImage2DEXT(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE2DEXTPROC)m_fn_glTextureSubImage2D))(texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::StateFns::call_glTexSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::StateFns::glTextureSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage2D)(state,texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::StateFns::check_glTextureSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
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

void Indigo::StateFns::call_glTextureSubImage3DEXT(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE3DEXTPROC)m_fn_glTextureSubImage3D))(texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void Indigo::StateFns::call_glTexSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	glTexSubImage3D(target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void Indigo::StateFns::glTextureSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage3D)(state,texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void Indigo::StateFns::check_glTextureParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameterf = glfwGetProcAddress("glTextureParameterfEXT");
		if (m_fn_glTextureParameterf)
			m_thunk_glTextureParameterf = &StateFns::call_glTextureParameterfEXT;
	}

	if (!m_fn_glTextureParameterf)
		m_thunk_glTextureParameterf = &StateFns::call_glTexParameterf;

	(this->*m_thunk_glTextureParameterf)(state,texture,target,name,val);
}

void Indigo::StateFns::call_glTextureParameterfEXT(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	(*((PFNGLTEXTUREPARAMETERFEXTPROC)m_fn_glTextureParameterf))(texture,target,name,val);
}

void Indigo::StateFns::call_glTexParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	glTexParameterf(target,name,val);
}

void Indigo::StateFns::glTextureParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	(this->*m_thunk_glTextureParameterf)(state,texture,target,name,val);
}

void Indigo::StateFns::check_glTextureParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameterfv = glfwGetProcAddress("glTextureParameterfvEXT");
		if (m_fn_glTextureParameterfv)
			m_thunk_glTextureParameterfv = &StateFns::call_glTextureParameterfvEXT;
	}

	if (!m_fn_glTextureParameterfv)
		m_thunk_glTextureParameterfv = &StateFns::call_glTexParameterfv;

	(this->*m_thunk_glTextureParameterfv)(state,texture,target,name,val);
}

void Indigo::StateFns::call_glTextureParameterfvEXT(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	(*((PFNGLTEXTUREPARAMETERFVEXTPROC)m_fn_glTextureParameterfv))(texture,target,name,val);
}

void Indigo::StateFns::call_glTexParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	glTexParameterfv(target,name,val);
}

void Indigo::StateFns::glTextureParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* val)
{
	(this->*m_thunk_glTextureParameterfv)(state,texture,target,name,val);
}

void Indigo::StateFns::check_glTextureParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameteri = glfwGetProcAddress("glTextureParameteriEXT");
		if (m_fn_glTextureParameteri)
			m_thunk_glTextureParameteri = &StateFns::call_glTextureParameteriEXT;
	}

	if (!m_fn_glTextureParameteri)
		m_thunk_glTextureParameteri = &StateFns::call_glTexParameteri;

	(this->*m_thunk_glTextureParameteri)(state,texture,target,name,val);
}

void Indigo::StateFns::call_glTextureParameteriEXT(State& state, GLuint texture, GLenum target, GLenum name, GLint val)
{
	(*((PFNGLTEXTUREPARAMETERIEXTPROC)m_fn_glTextureParameteri))(texture,target,name,val);
}

void Indigo::StateFns::call_glTexParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	glTexParameteri(target,name,val);
}

void Indigo::StateFns::glTextureParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val)
{
	(this->*m_thunk_glTextureParameteri)(state,texture,target,name,val);
}

void Indigo::StateFns::check_glTextureParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	if (glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glTextureParameteriv = glfwGetProcAddress("glTextureParameterivEXT");
		if (m_fn_glTextureParameteriv)
			m_thunk_glTextureParameteriv = &StateFns::call_glTextureParameterivEXT;
	}

	if (!m_fn_glTextureParameteriv)
		m_thunk_glTextureParameteriv = &StateFns::call_glTexParameteriv;

	(this->*m_thunk_glTextureParameteriv)(state,texture,target,name,val);
}

void Indigo::StateFns::call_glTextureParameterivEXT(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	(*((PFNGLTEXTUREPARAMETERIVEXTPROC)m_fn_glTextureParameteriv))(texture,target,name,val);
}

void Indigo::StateFns::call_glTexParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	state.bind_multi_texture(state.m_active_texture_unit,target,texture);

	glTexParameteriv(target,name,val);
}

void Indigo::StateFns::glTextureParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val)
{
	(this->*m_thunk_glTextureParameteriv)(state,texture,target,name,val);
}
