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

#include <OOBase/Logger.h>

#include "../include/OOGL/BufferObject.h"
#include "../include/OOGL/Shader.h"
#include "../include/OOGL/Texture.h"
#include "../include/OOGL/VertexArrayObject.h"
#include "../include/OOGL/Window.h"

namespace
{
	template <typename FN>
	bool load_proc(FN& fn, const char* name)
	{
		if (!fn)
		{
			fn = (FN)glfwGetProcAddress(name);
			if (!fn)
				LOG_ERROR(("No %s function",name));
		}
		return (fn != NULL);
	}

	void on_debug1(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* param)
	{
		OOBase::ScopedString str;
		if (length > 0)
			str.printf("%.*s",length,message);
		else
			str.printf("%s",message);

		size_t l = str.length();
		if (l)
		{
			for (;l > 0 && (str[l-1] == '\n' || str[l-1] == '\r' || str[l-1] == '\0');)
				str[--l] = '\0';
		}

		OOBase::Logger::Priority p = OOBase::Logger::Information;
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
			p = OOBase::Logger::Error;
			break;

		case GL_DEBUG_SEVERITY_MEDIUM:
			p = OOBase::Logger::Warning;
			break;

		case GL_DEBUG_SEVERITY_LOW:
			p = OOBase::Logger::Debug;
			break;

		default:
			break;
		}

		if (type == GL_DEBUG_TYPE_ERROR)
			p = OOBase::Logger::Error;

		OOBase::Logger::log(p,"[OpenGL] %s",str.c_str());
	}

	void on_debug2(GLuint id, GLenum category, GLenum severity, GLsizei length, const GLchar *message, void *userParam)
	{
		on_debug1(category,GL_DEBUG_TYPE_OTHER,id,severity,length,message,userParam);
	}
}

OOGL::StateFns::StateFns() :
		m_gl_major(1),
		m_gl_minor(0),
		m_logging(false),
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
		m_fn_glCreateProgram(NULL),
		m_fn_glDeleteProgram(NULL),
		m_fn_glGetProgramiv(NULL),
		m_fn_glGetProgramInfoLog(NULL),
		m_fn_glAttachShader(NULL),
		m_fn_glDetachShader(NULL),
		m_fn_glLinkProgram(NULL),
		m_fn_glUseProgram(NULL),
		m_fn_glGetAttribLocation(NULL),
		m_fn_glGetUniformLocation(NULL),
		m_thunk_glUniform3fv(&StateFns::check_glUniform3fv),
		m_thunk_glUniform4fv(&StateFns::check_glUniform4fv),
		m_thunk_glUniformMatrix4fv(&StateFns::check_glUniformMatrix4fv),
		m_fn_glUniformMatrix4fv(NULL),
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
		m_thunk_glTextureSubImage1D_2(&StateFns::check_glTextureSubImage1D_2),
		m_fn_glTextureSubImage1D(NULL),
		m_thunk_glTextureSubImage2D(&StateFns::check_glTextureSubImage2D),
		m_thunk_glTextureSubImage2D_2(&StateFns::check_glTextureSubImage2D_2),
		m_fn_glTextureSubImage2D(NULL),
		m_thunk_glTextureSubImage3D(&StateFns::check_glTextureSubImage3D),
		m_thunk_glTextureSubImage3D_2(&StateFns::check_glTextureSubImage3D_2),
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
		m_fn_glVertexAttribPointer(NULL),
		m_fn_glVertexAttribIPointer(NULL),
		m_thunk_glEnableVertexArrayAttrib(&StateFns::check_glEnableVertexArrayAttrib),
		m_fn_glEnableVertexArrayAttrib(NULL),
		m_thunk_glDisableVertexArrayAttrib(&StateFns::check_glDisableVertexArrayAttrib),
		m_fn_glDisableVertexArrayAttrib(NULL),
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
	GLFWwindow* win = glfwGetCurrentContext();
	if (!win)
		LOG_ERROR(("No current context!"));
	else
	{
		m_gl_major = glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MAJOR);
		m_gl_minor = glfwGetWindowAttrib(win,GLFW_CONTEXT_VERSION_MINOR);
	}

	OOBase::Logger::log(OOBase::Logger::Information,"OpenGL version: %s",glGetString(GL_VERSION));
	OOBase::Logger::log(OOBase::Logger::Information,"OpenGL renderer: %s",glGetString(GL_RENDERER));
	OOBase::Logger::log(OOBase::Logger::Information,"OpenGL vendor: %s",glGetString(GL_VENDOR));

	if (!isGLversion(2,0))
	{
		OOBase::Logger::log(OOBase::Logger::Information,"GLSL version: %s",glGetString(GL_SHADING_LANGUAGE_VERSION_ARB));
		OOBase::Logger::log(OOBase::Logger::Warning,"OpenGL version: %d.%d is highly unlikely to work",m_gl_major,m_gl_minor);
	}
	else
		OOBase::Logger::log(OOBase::Logger::Information,"GLSL version: %s",glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void OOGL::StateFns::enable_logging()
{
	if (!m_logging)
	{
		if (isGLversion(4,5) || glfwExtensionSupported("GL_KHR_debug") == GL_TRUE)
		{
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

			PFNGLDEBUGMESSAGECALLBACKPROC pfn = NULL;
			if (load_proc(pfn,"glDebugMessageCallback"))
				(*pfn)(&on_debug1,NULL);
		}
		else if (glfwExtensionSupported("GL_ARB_debug_output") == GL_TRUE)
		{
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);

			PFNGLDEBUGMESSAGECALLBACKARBPROC pfn = NULL;
			if (load_proc(pfn,"glDebugMessageCallbackARB"))
				(*pfn)(&on_debug1,NULL);
		}
		else if (glfwExtensionSupported("GL_AMD_debug_output") == GL_TRUE)
		{
			PFNGLDEBUGMESSAGECALLBACKAMDPROC pfn = NULL;
			if (load_proc(pfn,"glDebugMessageCallbackAMD"))
				(*pfn)(&on_debug2,NULL);
		}

		m_logging = true;
	}
}

bool OOGL::StateFns::isGLversion(int major, int minor)
{
	return (m_gl_major > major || (m_gl_major == major && m_gl_minor >= minor));
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

		if (!m_fn_glGenFramebuffers)
			LOG_ERROR(("No glGenFramebuffers function"));
	}

	if (m_fn_glGenFramebuffers)
	{
		(*m_fn_glGenFramebuffers)(n,framebuffers);

		OOGL_CHECK("glGenFramebuffers");
	}
}

void OOGL::StateFns::glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
	if (!m_fn_glDeleteFramebuffers)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");

		if (!m_fn_glDeleteFramebuffers && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");

		if (!m_fn_glDeleteFramebuffers)
			LOG_ERROR(("No glDeleteFramebuffers function"));
	}

	if (m_fn_glDeleteFramebuffers)
	{
		(*m_fn_glDeleteFramebuffers)(n,framebuffers);

		OOGL_CHECK("glDeleteFramebuffers");
	}
}

void OOGL::StateFns::glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	if (!m_fn_glBindFramebuffer)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_framebuffer_object") == GL_TRUE)
			m_fn_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");

		if (!m_fn_glBindFramebuffer && glfwExtensionSupported("GL_EXT_framebuffer_object") == GL_TRUE)
			m_fn_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebufferEXT");

		if (!m_fn_glBindFramebuffer)
			LOG_ERROR(("No glBindFramebuffer function"));
	}

	if (m_fn_glBindFramebuffer)
	{
		(*m_fn_glBindFramebuffer)(target,framebuffer);

		OOGL_CHECK("glBindFramebuffer");
	}
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

	GLenum r = (*m_fn_glCheckFramebufferStatus)(target);

	OOGL_CHECK("glCheckFrameBufferStatus");

	return r;
}

GLuint OOGL::StateFns::glCreateShader(GLenum shaderType)
{
	GLuint r = 0;
	if (load_proc(m_fn_glCreateShader,"glCreateShader"))
	{
		r = (*m_fn_glCreateShader)(shaderType);

		OOGL_CHECK("glCreateShader");
	}
	return r;
}

void OOGL::StateFns::glDeleteShader(GLuint shader)
{
	if (load_proc(m_fn_glDeleteShader,"glDeleteShader"))
	{
		(*m_fn_glDeleteShader)(shader);

		OOGL_CHECK("glDeleteShader");
	}
}

void OOGL::StateFns::glShaderSource(GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)
{
	if (load_proc(m_fn_glShaderSource,"glShaderSource"))
	{
		(*m_fn_glShaderSource)(shader,count,string,length);

		OOGL_CHECK("glShaderSource");
	}
}

void OOGL::StateFns::glCompileShader(GLuint shader)
{
	if (load_proc(m_fn_glCompileShader,"glCompileShader"))
	{
		(*m_fn_glCompileShader)(shader);

		OOGL_CHECK("glCompileShader");
	}
}

void OOGL::StateFns::glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	if (load_proc(m_fn_glGetShaderiv,"glGetShaderiv"))
	{
		(*m_fn_glGetShaderiv)(shader,pname,params);

		OOGL_CHECK("glGetShaderiv");
	}
}

void OOGL::StateFns::glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	if (load_proc(m_fn_glGetShaderInfoLog,"glGetShaderInfoLog"))
	{
		(*m_fn_glGetShaderInfoLog)(shader,maxLength,length,infoLog);

		OOGL_CHECK("glGetShaderInfoLog");
	}
}

GLuint OOGL::StateFns::glCreateProgram()
{
	GLuint r = 0;
	if (load_proc(m_fn_glCreateProgram,"glCreateProgram"))
	{
		r = (*m_fn_glCreateProgram)();

		OOGL_CHECK("glCreateProgram");
	}
	return r;
}

void OOGL::StateFns::glDeleteProgram(GLuint program)
{
	if (load_proc(m_fn_glDeleteProgram,"glDeleteProgram"))
	{
		(*m_fn_glDeleteProgram)(program);

		OOGL_CHECK("glDeleteProgram");
	}
}

void OOGL::StateFns::glGetProgramiv(GLuint shader, GLenum pname, GLint* params)
{
	if (load_proc(m_fn_glGetProgramiv,"glGetProgramiv"))
	{
		(*m_fn_glGetProgramiv)(shader,pname,params);

		OOGL_CHECK("glGetProgramiv");
	}
}

void OOGL::StateFns::glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)
{
	if (load_proc(m_fn_glGetProgramInfoLog,"glGetProgramInfoLog"))
	{
		(*m_fn_glGetProgramInfoLog)(program,maxLength,length,infoLog);

		OOGL_CHECK("glGetProgramInfoLog");
	}
}

void OOGL::StateFns::glUseProgram(GLuint program)
{
	if (load_proc(m_fn_glUseProgram,"glUseProgram"))
	{
		(*m_fn_glUseProgram)(program);

		OOGL_CHECK("glUseProgram");
	}
}

void OOGL::StateFns::glAttachShader(GLuint program, GLuint shader)
{
	if (load_proc(m_fn_glAttachShader,"glAttachShader"))
	{
		(*m_fn_glAttachShader)(program,shader);

		OOGL_CHECK("glAttachShader");
	}
}

void OOGL::StateFns::glDetachShader(GLuint program, GLuint shader)
{
	if (load_proc(m_fn_glDetachShader,"glDetachShader"))
	{
		(*m_fn_glDetachShader)(program,shader);

		OOGL_CHECK("glDetachShader");
	}
}

void OOGL::StateFns::glLinkProgram(GLuint program)
{
	if (load_proc(m_fn_glLinkProgram,"glLinkProgram"))
	{
		(*m_fn_glLinkProgram)(program);

		OOGL_CHECK("glLinkProgram");
	}
}

GLint OOGL::StateFns::glGetAttribLocation(GLuint program, const char* name)
{
	GLint r = -1;
	if (load_proc(m_fn_glGetAttribLocation,"glGetAttribLocation"))
	{
		r = (*m_fn_glGetAttribLocation)(program,name);

		OOGL_CHECK("glGetAttribLocation");
	}
	return r;
}

GLint OOGL::StateFns::glGetUniformLocation(GLuint program, const char* name)
{
	GLint r = -1;
	if (load_proc(m_fn_glGetUniformLocation,"glGetUniformLocation"))
	{
		r = (*m_fn_glGetUniformLocation)(program,name);

		OOGL_CHECK("glGetUniformLocation");
	}
	return r;
}

void OOGL::StateFns::check_glUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	if (isGLversion(4,1) || glfwExtensionSupported("GL_ARB_separate_shader_objects") == GL_TRUE)
	{
		m_fn_glUniform3fv = glfwGetProcAddress("glProgramUniform3fv");
		if (m_fn_glUniform3fv)
			m_thunk_glUniform3fv = &StateFns::call_glProgramUniform3fv;
	}

	if (!m_fn_glUniform3fv && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glUniform3fv = glfwGetProcAddress("glProgramUniform3fvEXT");
		if (m_fn_glUniform3fv)
			m_thunk_glUniform3fv = &StateFns::call_glProgramUniform3fv;
	}

	if (!m_fn_glUniform3fv)
	{
		m_fn_glUniform3fv = glfwGetProcAddress("glUniform3fv");
		if (m_fn_glUniform3fv)
			m_thunk_glUniform3fv = &StateFns::call_glUniform3fv;
	}

	if (!m_fn_glUniform3fv)
		LOG_ERROR(("No glUniform3fv function"));
	else
		(this->*m_thunk_glUniform3fv)(program,location,count,v);
}

void OOGL::StateFns::call_glProgramUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	(*((PFNGLPROGRAMUNIFORM3FVPROC)m_fn_glUniform3fv))(program->m_id,location,count,v);

	OOGL_CHECK("glProgramUniform3fv");
}

void OOGL::StateFns::call_glUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	State::get_current()->use(program);

	(*((PFNGLUNIFORM3FVPROC)m_fn_glUniform3fv))(location,count,v);

	OOGL_CHECK("glUniform3fv");
}

void OOGL::StateFns::glUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	(this->*m_thunk_glUniform3fv)(program,location,count,v);
}

void OOGL::StateFns::check_glUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	if (isGLversion(4,1) || glfwExtensionSupported("GL_ARB_separate_shader_objects") == GL_TRUE)
	{
		m_fn_glUniform4fv = glfwGetProcAddress("glProgramUniform4fv");
		if (m_fn_glUniform4fv)
			m_thunk_glUniform4fv = &StateFns::call_glProgramUniform4fv;
	}

	if (!m_fn_glUniform4fv && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glUniform4fv = glfwGetProcAddress("glProgramUniform4fvEXT");
		if (m_fn_glUniform4fv)
			m_thunk_glUniform4fv = &StateFns::call_glProgramUniform4fv;
	}

	if (!m_fn_glUniform4fv)
	{
		m_fn_glUniform4fv = glfwGetProcAddress("glUniform4fv");
		if (m_fn_glUniform4fv)
			m_thunk_glUniform4fv = &StateFns::call_glUniform4fv;
	}

	if (!m_fn_glUniform4fv)
		LOG_ERROR(("No glUniform4fv function"));
	else
		(this->*m_thunk_glUniform4fv)(program,location,count,v);
}

void OOGL::StateFns::call_glProgramUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	(*((PFNGLPROGRAMUNIFORM4FVPROC)m_fn_glUniform4fv))(program->m_id,location,count,v);

	OOGL_CHECK("glProgramUniform4fv");
}

void OOGL::StateFns::call_glUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	State::get_current()->use(program);

	(*((PFNGLUNIFORM4FVPROC)m_fn_glUniform4fv))(location,count,v);

	OOGL_CHECK("glUniform4fv");
}

void OOGL::StateFns::glUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v)
{
	(this->*m_thunk_glUniform4fv)(program,location,count,v);
}

void OOGL::StateFns::check_glUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v)
{
	if (isGLversion(4,1) || glfwExtensionSupported("GL_ARB_separate_shader_objects") == GL_TRUE)
	{
		m_fn_glUniformMatrix4fv = glfwGetProcAddress("glProgramUniformMatrix4fv");
		if (m_fn_glUniformMatrix4fv)
			m_thunk_glUniformMatrix4fv = &StateFns::call_glProgramUniformMatrix4fv;
	}

	if (!m_fn_glUniformMatrix4fv && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
	{
		m_fn_glUniformMatrix4fv = glfwGetProcAddress("glProgramUniformMatrix4fvEXT");
		if (m_fn_glUniformMatrix4fv)
			m_thunk_glUniformMatrix4fv = &StateFns::call_glProgramUniformMatrix4fv;
	}

	if (!m_fn_glUniformMatrix4fv)
	{
		m_fn_glUniformMatrix4fv = glfwGetProcAddress("glUniformMatrix4fv");
		if (m_fn_glUniformMatrix4fv)
			m_thunk_glUniformMatrix4fv = &StateFns::call_glUniformMatrix4fv;
	}

	if (!m_fn_glUniformMatrix4fv)
		LOG_ERROR(("No glUniformMatrix4fv function"));
	else
		(this->*m_thunk_glUniformMatrix4fv)(program,location,count,transpose,v);
}

void OOGL::StateFns::call_glProgramUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v)
{
	(*((PFNGLPROGRAMUNIFORMMATRIX4FVPROC)m_fn_glUniformMatrix4fv))(program->m_id,location,count,transpose,v);

	OOGL_CHECK("glProgramUniformMatrix4fv");
}

void OOGL::StateFns::call_glUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v)
{
	State::get_current()->use(program);

	(*((PFNGLUNIFORMMATRIX4FVPROC)m_fn_glUniformMatrix4fv))(location,count,transpose,v);

	OOGL_CHECK("glUniformMatrix4fv");
}

void OOGL::StateFns::glUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v)
{
	(this->*m_thunk_glUniformMatrix4fv)(program,location,count,transpose,v);
}

void OOGL::StateFns::glActiveTexture(GLuint unit)
{
	if (load_proc(m_fn_glActiveTexture,"glActiveTexture"))
	{
		(*m_fn_glActiveTexture)(GL_TEXTURE0 + unit);

		OOGL_CHECK("glActiveTexture");
	}
}

void OOGL::StateFns::check_glBindTextureUnit(State& state, GLuint unit, GLenum target, GLuint texture)
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

void OOGL::StateFns::call_glBindTextureUnit(State&, GLuint unit, GLenum, GLuint texture)
{
	(*((PFNGLBINDTEXTUREUNITPROC)m_fn_glBindTextureUnit))(unit,texture);

	OOGL_CHECK("glBindTextureUnit");
}

void OOGL::StateFns::emulate_glBindTextureUnit(State& state, GLuint unit, GLenum target, GLuint texture)
{
	state.activate_texture_unit(unit);

	glBindTexture(target,texture);

	OOGL_CHECK("glBindTexture");
}

void OOGL::StateFns::call_glMultiBindTexture(State& state, GLuint unit, GLenum target, GLuint texture)
{
	(*((PFNGLBINDMULTITEXTUREEXTPROC)m_fn_glBindTextureUnit))(GL_TEXTURE0 + unit,target,texture);

	OOGL_CHECK("glBindMultiTextureEXT");
}

void OOGL::StateFns::glBindTextureUnit(State& state, GLuint unit, GLenum target, GLuint texture)
{
	(this->*m_thunk_glBindTextureUnit)(state,unit,target,texture);
}

bool OOGL::StateFns::check_glTextureArray()
{
	return isGLversion(3,0) || glfwExtensionSupported("GL_EXT_texture_array") == GL_TRUE;
}

bool OOGL::StateFns::check_glTexture3D()
{
	if (!m_fn_glTexImage3D)
		m_fn_glTexImage3D = (PFNGLTEXIMAGE3DPROC)glfwGetProcAddress("glTexImage3D");

	return (m_fn_glTexImage3D != NULL);
}

void OOGL::StateFns::glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels)
{
	if (!check_glTexture3D())
		LOG_ERROR(("No glTexImage3D function"));
	else
	{
		(*m_fn_glTexImage3D)(target,level,internalformat,width,height,depth,border,format,type,pixels);

		OOGL_CHECK("glTexImage3D");
	}
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

void OOGL::StateFns::check_glTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	if (!check_glTextureStorage())
		LOG_ERROR(("No glTexStorage1D function"));
	else
		(this->*m_thunk_glTextureStorage1D)(texture,target,levels,internalFormat,width);
}

void OOGL::StateFns::call_glTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(*((PFNGLTEXTURESTORAGE1DPROC)m_fn_glTextureStorage1D))(texture,levels,internalFormat,width);

	OOGL_CHECK("glTextureStorage1D");
}

void OOGL::StateFns::call_glTextureStorage1DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(*((PFNGLTEXTURESTORAGE1DEXTPROC)m_fn_glTextureStorage1D))(texture,target,levels,internalFormat,width);

	OOGL_CHECK("glTextureStorage1DEXT");
}

void OOGL::StateFns::call_glTexStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	State::get_current()->bind_texture(texture,target);

	(*((PFNGLTEXSTORAGE1DPROC)m_fn_glTextureStorage1D))(target,levels,internalFormat,width);

	OOGL_CHECK("glTexStorage1D");
}

void OOGL::StateFns::glTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	(this->*m_thunk_glTextureStorage1D)(texture,target,levels,internalFormat,width);
}

void OOGL::StateFns::check_glTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
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
		(this->*m_thunk_glTextureStorage2D)(texture,target,levels,internalFormat,width,height);
}

void OOGL::StateFns::call_glTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(*((PFNGLTEXTURESTORAGE2DPROC)m_fn_glTextureStorage2D))(texture,levels,internalFormat,width,height);

	OOGL_CHECK("glTextureStorage2D");
}

void OOGL::StateFns::call_glTextureStorage2DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(*((PFNGLTEXTURESTORAGE2DEXTPROC)m_fn_glTextureStorage2D))(texture,target,levels,internalFormat,width,height);

	OOGL_CHECK("glTextureStorage2DEXT");
}

void OOGL::StateFns::call_glTexStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	State::get_current()->bind_texture(texture,target);

	(*((PFNGLTEXSTORAGE2DPROC)m_fn_glTextureStorage2D))(target,levels,internalFormat,width,height);

	OOGL_CHECK("glTexStorage2D");
}

void OOGL::StateFns::glTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	(this->*m_thunk_glTextureStorage2D)(texture,target,levels,internalFormat,width,height);
}

void OOGL::StateFns::check_glTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
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
		(this->*m_thunk_glTextureStorage3D)(texture,target,levels,internalFormat,width,height,depth);
}

void OOGL::StateFns::call_glTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(*((PFNGLTEXTURESTORAGE3DPROC)m_fn_glTextureStorage3D))(texture,levels,internalFormat,width,height,depth);

	OOGL_CHECK("glTextureStorage3D");
}

void OOGL::StateFns::call_glTextureStorage3DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(*((PFNGLTEXTURESTORAGE3DEXTPROC)m_fn_glTextureStorage3D))(texture,target,levels,internalFormat,width,height,depth);

	OOGL_CHECK("glTextureStorage3DEXT");
}

void OOGL::StateFns::call_glTexStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	State::get_current()->bind_texture(texture,target);

	(*((PFNGLTEXSTORAGE3DPROC)m_fn_glTextureStorage3D))(target,levels,internalFormat,width,height,depth);

	OOGL_CHECK("glTexStorage3D");
}

void OOGL::StateFns::glTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	(this->*m_thunk_glTextureStorage3D)(texture,target,levels,internalFormat,width,height,depth);
}

void OOGL::StateFns::check_glTextureSubImage1D()
{
	if (!m_fn_glTextureSubImage1D)
	{
		if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureSubImage1D = glfwGetProcAddress("glTextureSubImage1D");
			if (m_fn_glTextureSubImage1D)
			{
				m_thunk_glTextureSubImage1D = &StateFns::call_glTextureSubImage1D;
				m_thunk_glTextureSubImage1D_2 = &StateFns::call_glTextureSubImage1D_2;
			}
		}

		if (!m_fn_glTextureSubImage1D && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureSubImage1D = glfwGetProcAddress("glTextureSubImage1DEXT");
			if (m_fn_glTextureSubImage1D)
			{
				m_thunk_glTextureSubImage1D = &StateFns::call_glTextureSubImage1DEXT;
				m_thunk_glTextureSubImage1D_2 = &StateFns::call_glTextureSubImage1DEXT_2;
			}
		}

		if (!m_fn_glTextureSubImage1D)
		{
			m_thunk_glTextureSubImage1D = &StateFns::call_glTexSubImage1D;
			m_thunk_glTextureSubImage1D_2 = &StateFns::call_glTexSubImage1D_2;
		}
	}
}

void OOGL::StateFns::check_glTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	check_glTextureSubImage1D();

	(this->*m_thunk_glTextureSubImage1D)(texture,target,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::check_glTextureSubImage1D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	check_glTextureSubImage1D();

	(this->*m_thunk_glTextureSubImage1D_2)(texture,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage1D(GLuint texture, GLenum, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE1DPROC)m_fn_glTextureSubImage1D))(texture,level,xoffset,width,format,type,pixels);

	OOGL_CHECK("glTextureSubImage1D");
}

void OOGL::StateFns::call_glTextureSubImage1D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	call_glTextureSubImage1D(texture->m_tex,0,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE1DEXTPROC)m_fn_glTextureSubImage1D))(texture,target,level,xoffset,width,format,type,pixels);

	OOGL_CHECK("glTextureSubImage1DEXT");
}

void OOGL::StateFns::call_glTextureSubImage1DEXT_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	call_glTextureSubImage1DEXT(texture->m_tex,texture->m_target,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::call_glTexSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->bind_texture(texture,target);

	glTexSubImage1D(target,level,xoffset,width,format,type,pixels);

	OOGL_CHECK("glTexSubImage1D");
}

void OOGL::StateFns::call_glTexSubImage1D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->bind_texture_active_unit(texture);

	glTexSubImage1D(texture->m_target,level,xoffset,width,format,type,pixels);

	OOGL_CHECK("glTexSubImage1D");
}

void OOGL::StateFns::glTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage1D)(texture,target,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::glTextureSubImage1D(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage1D_2)(texture,level,xoffset,width,format,type,pixels);
}

void OOGL::StateFns::check_glTextureSubImage2D()
{
	if (!m_fn_glTextureSubImage2D)
	{
		if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureSubImage2D = glfwGetProcAddress("glTextureSubImage2D");
			if (m_fn_glTextureSubImage2D)
			{
				m_thunk_glTextureSubImage2D = &StateFns::call_glTextureSubImage2D;
				m_thunk_glTextureSubImage2D_2 = &StateFns::call_glTextureSubImage2D_2;
			}
		}

		if (!m_fn_glTextureSubImage2D && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureSubImage2D = glfwGetProcAddress("glTextureSubImage2DEXT");
			if (m_fn_glTextureSubImage2D)
			{
				m_thunk_glTextureSubImage2D = &StateFns::call_glTextureSubImage2DEXT;
				m_thunk_glTextureSubImage2D_2 = &StateFns::call_glTextureSubImage2DEXT_2;
			}
		}

		if (!m_fn_glTextureSubImage2D)
		{
			m_thunk_glTextureSubImage2D = &StateFns::call_glTexSubImage2D;
			m_thunk_glTextureSubImage2D_2 = &StateFns::call_glTexSubImage2D_2;
		}
	}
}

void OOGL::StateFns::check_glTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	check_glTextureSubImage2D();

	(this->*m_thunk_glTextureSubImage2D)(texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::check_glTextureSubImage2D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	check_glTextureSubImage2D();

	(this->*m_thunk_glTextureSubImage2D_2)(texture,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage2D(GLuint texture, GLenum, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE2DPROC)m_fn_glTextureSubImage2D))(texture,level,xoffset,yoffset,width,height,format,type,pixels);

	OOGL_CHECK("glTextureSubImage2D");
}

void OOGL::StateFns::call_glTextureSubImage2D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	call_glTextureSubImage2D(texture->m_tex,0,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE2DEXTPROC)m_fn_glTextureSubImage2D))(texture,target,level,xoffset,yoffset,width,height,format,type,pixels);

	OOGL_CHECK("glTextureSubImage2DEXT");
}

void OOGL::StateFns::call_glTextureSubImage2DEXT_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	call_glTextureSubImage2DEXT(texture->m_tex,texture->m_target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::call_glTexSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->bind_texture(texture,target);

	glTexSubImage2D(target,level,xoffset,yoffset,width,height,format,type,pixels);

	OOGL_CHECK("glTexSubImage2D");
}

void OOGL::StateFns::call_glTexSubImage2D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->bind_texture_active_unit(texture);

	glTexSubImage2D(texture->m_target,level,xoffset,yoffset,width,height,format,type,pixels);

	OOGL_CHECK("glTexSubImage2D");
}

void OOGL::StateFns::glTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage2D)(texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::StateFns::glTextureSubImage2D(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage2D_2)(texture,level,xoffset,yoffset,width,height,format,type,pixels);
}

bool OOGL::StateFns::check_glTextureSubImage3D()
{
	if (!m_fn_glTextureSubImage3D)
	{
		if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureSubImage3D = glfwGetProcAddress("glTextureSubImage3D");
			if (m_fn_glTextureSubImage3D)
			{
				m_thunk_glTextureSubImage3D = &StateFns::call_glTextureSubImage3D;
				m_thunk_glTextureSubImage3D_2 = &StateFns::call_glTextureSubImage3D_2;
			}
		}

		if (!m_fn_glTextureSubImage3D && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
		{
			m_fn_glTextureSubImage3D = glfwGetProcAddress("glTextureSubImage3DEXT");
			if (m_fn_glTextureSubImage3D)
			{
				m_thunk_glTextureSubImage3D = &StateFns::call_glTextureSubImage3DEXT;
				m_thunk_glTextureSubImage3D_2 = &StateFns::call_glTextureSubImage3DEXT_2;
			}
		}

		if (!m_fn_glTextureSubImage3D)
		{
			m_fn_glTextureSubImage3D = glfwGetProcAddress("glTexSubImage3D");
			if (!m_fn_glTextureSubImage3D)
				LOG_ERROR_RETURN(("No glTexSubImage3D function"),false);

			m_thunk_glTextureSubImage3D = &StateFns::call_glTexSubImage3D;
			m_thunk_glTextureSubImage3D_2 = &StateFns::call_glTexSubImage3D_2;
		}
	}

	return true;
}

void OOGL::StateFns::check_glTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	if (check_glTextureSubImage3D())
		(this->*m_thunk_glTextureSubImage3D)(texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::check_glTextureSubImage3D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	if (check_glTextureSubImage3D())
		(this->*m_thunk_glTextureSubImage3D_2)(texture,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage3D(GLuint texture, GLenum, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE3DPROC)m_fn_glTextureSubImage3D))(texture,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);

	OOGL_CHECK("glTextureSubImage3D");
}

void OOGL::StateFns::call_glTextureSubImage3D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	call_glTextureSubImage3D(texture->m_tex,0,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::call_glTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(*((PFNGLTEXTURESUBIMAGE3DEXTPROC)m_fn_glTextureSubImage3D))(texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);

	OOGL_CHECK("glTextureSubImage3DEXT");
}

void OOGL::StateFns::call_glTextureSubImage3DEXT_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	call_glTextureSubImage3DEXT(texture->m_tex,texture->m_target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::call_glTexSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->bind_texture(texture,target);

	(*((PFNGLTEXSUBIMAGE3DPROC)m_fn_glTextureSubImage3D))(target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);

	OOGL_CHECK("glTexSubImage3D");
}

void OOGL::StateFns::call_glTexSubImage3D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->bind_texture_active_unit(texture);

	(*((PFNGLTEXSUBIMAGE3DPROC)m_fn_glTextureSubImage3D))(texture->m_target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);

	OOGL_CHECK("glTexSubImage3D");
}

void OOGL::StateFns::glTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage3D)(texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::glTextureSubImage3D(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	(this->*m_thunk_glTextureSubImage3D_2)(texture,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void OOGL::StateFns::check_glTextureParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val)
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

	(this->*m_thunk_glTextureParameterf)(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val)
{
	(*((PFNGLTEXTUREPARAMETERFPROC)m_fn_glTextureParameterf))(texture->m_tex,name,val);

	OOGL_CHECK("glTextureParameterf");
}

void OOGL::StateFns::call_glTextureParameterfEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val)
{
	(*((PFNGLTEXTUREPARAMETERFEXTPROC)m_fn_glTextureParameterf))(texture->m_tex,texture->m_target,name,val);

	OOGL_CHECK("glTextureParameterfEXT");
}

void OOGL::StateFns::call_glTexParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val)
{
	State::get_current()->bind_texture_active_unit(texture);

	glTexParameterf(texture->m_target,name,val);

	OOGL_CHECK("glTexParameterf");
}

void OOGL::StateFns::glTextureParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val)
{
	(this->*m_thunk_glTextureParameterf)(texture,name,val);
}

void OOGL::StateFns::check_glTextureParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* val)
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

	(this->*m_thunk_glTextureParameterfv)(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* val)
{
	(*((PFNGLTEXTUREPARAMETERFVPROC)m_fn_glTextureParameterfv))(texture->m_tex,name,val);

	OOGL_CHECK("glTextureParameterfv");
}

void OOGL::StateFns::call_glTextureParameterfvEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* val)
{
	(*((PFNGLTEXTUREPARAMETERFVEXTPROC)m_fn_glTextureParameterfv))(texture->m_tex,texture->m_target,name,val);

	OOGL_CHECK("glTextureParameterfvEXT");
}

void OOGL::StateFns::call_glTexParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* val)
{
	State::get_current()->bind_texture_active_unit(texture);

	glTexParameterfv(texture->m_target,name,val);

	OOGL_CHECK("glTexParameterfv");
}

void OOGL::StateFns::glTextureParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* val)
{
	(this->*m_thunk_glTextureParameterfv)(texture,name,val);
}

void OOGL::StateFns::check_glTextureParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val)
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

	(this->*m_thunk_glTextureParameteri)(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val)
{
	(*((PFNGLTEXTUREPARAMETERIPROC)m_fn_glTextureParameteri))(texture->m_tex,name,val);

	OOGL_CHECK("glTextureParameteri");
}

void OOGL::StateFns::call_glTextureParameteriEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val)
{
	(*((PFNGLTEXTUREPARAMETERIEXTPROC)m_fn_glTextureParameteri))(texture->m_tex,texture->m_target,name,val);

	OOGL_CHECK("glTextureParameteriEXT");
}

void OOGL::StateFns::call_glTexParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val)
{
	State::get_current()->bind_texture_active_unit(texture);

	glTexParameteri(texture->m_target,name,val);

	OOGL_CHECK("glTexParameteri");
}

void OOGL::StateFns::glTextureParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val)
{
	(this->*m_thunk_glTextureParameteri)(texture,name,val);
}

void OOGL::StateFns::check_glTextureParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* val)
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

	(this->*m_thunk_glTextureParameteriv)(texture,name,val);
}

void OOGL::StateFns::call_glTextureParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* val)
{
	(*((PFNGLTEXTUREPARAMETERIVPROC)m_fn_glTextureParameteriv))(texture->m_tex,name,val);

	OOGL_CHECK("glTextureParameteriv");
}

void OOGL::StateFns::call_glTextureParameterivEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* val)
{
	(*((PFNGLTEXTUREPARAMETERIVEXTPROC)m_fn_glTextureParameteriv))(texture->m_tex,texture->m_target,name,val);

	OOGL_CHECK("glTextureParameterivEXT");
}

void OOGL::StateFns::call_glTexParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* val)
{
	State::get_current()->bind_texture_active_unit(texture);

	glTexParameteriv(texture->m_target,name,val);

	OOGL_CHECK("glTexParameteriv");
}

void OOGL::StateFns::glTextureParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* val)
{
	(this->*m_thunk_glTextureParameteriv)(texture,name,val);
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

void OOGL::StateFns::check_glGenerateTextureMipmap(GLuint texture, GLenum target)
{
	if (!check_glGenerateMipmap())
		LOG_ERROR(("No glGenerateMipmap function!"));
	else
		(this->*m_thunk_glGenerateTextureMipmap)(texture,target);
}

void OOGL::StateFns::call_glGenerateTextureMipmap(GLuint texture, GLenum)
{
	(*((PFNGLGENERATETEXTUREMIPMAPPROC)m_fn_glGenerateTextureMipmap))(texture);

	OOGL_CHECK("glGenerateTextureMipmap");
}

void OOGL::StateFns::call_glGenerateTextureMipmapEXT(GLuint texture, GLenum target)
{
	(*((PFNGLGENERATETEXTUREMIPMAPEXTPROC)m_fn_glGenerateTextureMipmap))(texture,target);

	OOGL_CHECK("glGenerateTextureMipmapEXT");
}

void OOGL::StateFns::call_glGenerateMipmap(GLuint texture, GLenum target)
{
	(*((PFNGLGENERATEMIPMAPPROC)m_fn_glGenerateTextureMipmap))(target);

	OOGL_CHECK("glGenerateMipmap");
}

void OOGL::StateFns::glGenerateTextureMipmap(GLuint texture, GLenum target)
{
	(this->*m_thunk_glGenerateTextureMipmap)(texture,target);
}

void OOGL::StateFns::glGenBuffers(GLsizei n, GLuint* buffers)
{
	if (load_proc(m_fn_glGenBuffers,"glGenBuffers"))
	{
		(*m_fn_glGenBuffers)(n,buffers);

		OOGL_CHECK("glGenBuffers");
	}
}

void OOGL::StateFns::glBindBuffer(GLenum target, GLuint buffer)
{
	if (load_proc(m_fn_glBindBuffer,"glBindBuffer"))
	{
		(*m_fn_glBindBuffer)(target,buffer);

		OOGL_CHECK("glBindBuffer");
	}
}

void OOGL::StateFns::glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	if (load_proc(m_fn_glDeleteBuffers,"glDeleteBuffers"))
	{
		(*m_fn_glDeleteBuffers)(n,buffers);

		OOGL_CHECK("glDeleteBuffers");
	}
}

void OOGL::StateFns::check_glBufferData(GLuint buffer, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
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

	(this->*m_thunk_glBufferData)(buffer,target,size,data,usage);
}

void OOGL::StateFns::call_glNamedBufferData(GLuint buffer, GLenum, GLsizeiptr size, const void *data, GLenum usage)
{
	(*((PFNGLNAMEDBUFFERDATAPROC)m_fn_glBufferData))(buffer,size,data,usage);

	OOGL_CHECK("glNamedBufferData");
}

void OOGL::StateFns::call_glBufferData(GLuint buffer, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
	State::get_current()->bind_buffer(buffer,target);

	(*((PFNGLBUFFERDATAPROC)m_fn_glBufferData))(target,size,data,usage);

	OOGL_CHECK("glBufferData");
}

void OOGL::StateFns::glBufferData(GLuint buffer, GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
	(this->*m_thunk_glBufferData)(buffer,target,size,data,usage);
}

void* OOGL::StateFns::check_glMapBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access)
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

	return (this->*m_thunk_glMapBufferRange)(buffer,offset,length,orig_usage,orig_size,access);
}

void* OOGL::StateFns::call_glMapNamedBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access)
{
	void* r = (*((PFNGLMAPNAMEDBUFFERRANGEPROC)m_fn_glMapBufferRange))(buffer->m_buffer,offset,length,access);

	OOGL_CHECK("glMapNamedBufferRange");

	return r;
}

void* OOGL::StateFns::call_glMapBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access)
{
	State::get_current()->bind(buffer);

	void* r = (*((PFNGLMAPBUFFERRANGEPROC)m_fn_glMapBufferRange))(buffer->m_target,offset,length,access);

	OOGL_CHECK("glMapBufferRange");

	return r;
}

void* OOGL::StateFns::call_glMapBuffer(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access)
{
	if (access & GL_MAP_INVALIDATE_BUFFER_BIT)
		glBufferData(buffer->m_buffer,buffer->m_target,orig_size,NULL,orig_usage);
	
	State::get_current()->bind(buffer);
	
	OOBase::uint8_t* ret = static_cast<OOBase::uint8_t*>((*((PFNGLMAPBUFFERPROC)m_fn_glMapBufferRange))(buffer->m_target,access));
	if (ret)
		ret += offset;

	return ret;
}

void* OOGL::StateFns::glMapBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access)
{
	return (this->*m_thunk_glMapBufferRange)(buffer,offset,length,orig_usage,orig_size,access);
}

bool OOGL::StateFns::check_glUnmapBuffer(const OOBase::SharedPtr<BufferObject>& buffer)
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

	return (this->*m_thunk_glUnmapBuffer)(buffer);
}

bool OOGL::StateFns::call_glUnmapNamedBuffer(const OOBase::SharedPtr<BufferObject>& buffer)
{
	bool r = ((*((PFNGLUNMAPNAMEDBUFFERPROC)m_fn_glUnmapBuffer))(buffer->m_buffer) == GL_TRUE);

	OOGL_CHECK("glUnmapNamedBuffer");

	return r;
}

bool OOGL::StateFns::call_glUnmapBuffer(const OOBase::SharedPtr<BufferObject>& buffer)
{
	State::get_current()->bind(buffer);

	bool r = ((*((PFNGLUNMAPBUFFERPROC)m_fn_glUnmapBuffer))(buffer->m_target) == GL_TRUE);

	OOGL_CHECK("glUnmapBuffer");

	return r;
}

bool OOGL::StateFns::glUnmapBuffer(const OOBase::SharedPtr<BufferObject>& buffer)
{
	return (this->*m_thunk_glUnmapBuffer)(buffer);
}

void OOGL::StateFns::check_glBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data)
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

	(this->*m_thunk_glBufferSubData)(buffer,offset,size,data);
}

void OOGL::StateFns::call_glNamedBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data)
{
	(*((PFNGLNAMEDBUFFERSUBDATAPROC)m_fn_glBufferSubData))(buffer->m_buffer,offset,size,data);

	OOGL_CHECK("glNamedBufferSubData");
}

void OOGL::StateFns::call_glBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data)
{
	State::get_current()->bind(buffer);

	(*((PFNGLBUFFERSUBDATAPROC)m_fn_glBufferSubData))(buffer->m_target,offset,size,data);

	OOGL_CHECK("glBufferSubData");
}

void OOGL::StateFns::glBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data)
{
	(this->*m_thunk_glBufferSubData)(buffer,offset,size,data);
}

void OOGL::StateFns::check_glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size)
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
		m_thunk_glCopyBufferSubData = &StateFns::emulate_glCopyBufferSubData;

	(this->*m_thunk_glCopyBufferSubData)(write,writeoffset,read,readoffset,size);
}

void OOGL::StateFns::call_glCopyNamedBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size)
{
	(*((PFNGLCOPYNAMEDBUFFERSUBDATAPROC)m_fn_glCopyBufferSubData))(read->m_buffer,write->m_buffer,readoffset,writeoffset,size);

	OOGL_CHECK("glCopyNamedBufferSubData");
}

void OOGL::StateFns::call_glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size)
{
	OOBase::SharedPtr<State> ptrState = State::get_current();
	ptrState->bind_buffer_target(read,GL_COPY_READ_BUFFER);
	ptrState->bind_buffer_target(write,GL_COPY_WRITE_BUFFER);

	(*((PFNGLCOPYBUFFERSUBDATAPROC)m_fn_glCopyBufferSubData))(GL_COPY_READ_BUFFER,GL_COPY_WRITE_BUFFER,readoffset,writeoffset,size);

	OOGL_CHECK("glCopyBufferSubData");
}

void OOGL::StateFns::emulate_glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size)
{
	OOBase::SharedPtr<OOBase::uint8_t> r = read->auto_map<OOBase::uint8_t>(GL_READ_ONLY,readoffset);
	OOBase::SharedPtr<OOBase::uint8_t> w = write->auto_map<OOBase::uint8_t>(GL_WRITE_ONLY,writeoffset);

	memcpy(w.get(),r.get(),size);
}

void OOGL::StateFns::glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size)
{
	(this->*m_thunk_glCopyBufferSubData)(write,writeoffset,read,readoffset,size);
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
	{
		(*m_fn_glGenVertexArrays)(n,arrays);

		OOGL_CHECK("glGenVertexArrays");
	}
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
	{
		(*m_fn_glBindVertexArray)(array);

		OOGL_CHECK("glBindVertexArray");
	}
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
	{
		(*m_fn_glDeleteVertexArrays)(n,arrays);

		OOGL_CHECK("glDeleteVertexArrays");
	}
}

void OOGL::StateFns::check_glEnableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	if (!m_fn_glEnableVertexArrayAttrib)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_vertex_array_object") == GL_TRUE)
		{
			if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
			{
				m_fn_glEnableVertexArrayAttrib = glfwGetProcAddress("glEnableVertexArrayAttrib");
				if (m_fn_glEnableVertexArrayAttrib)
					m_thunk_glEnableVertexArrayAttrib = &StateFns::call_glEnableVertexArrayAttrib;
			}

			if (!m_fn_glEnableVertexArrayAttrib && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
			{
				m_fn_glEnableVertexArrayAttrib = glfwGetProcAddress("glEnableVertexArrayAttribEXT");
				if (m_fn_glEnableVertexArrayAttrib)
					m_thunk_glEnableVertexArrayAttrib = &StateFns::call_glEnableVertexArrayAttrib;
			}

			if (!m_fn_glEnableVertexArrayAttrib)
			{
				m_fn_glEnableVertexArrayAttrib = glfwGetProcAddress("glEnableVertexAttribArray");
				if (!m_fn_glEnableVertexArrayAttrib)
					m_fn_glEnableVertexArrayAttrib = glfwGetProcAddress("glEnableVertexAttribArrayARB");

				if (m_fn_glEnableVertexArrayAttrib)
					m_thunk_glEnableVertexArrayAttrib = &StateFns::call_glEnableVertexAttribArray;
			}
		}
	}

	if (!m_fn_glEnableVertexArrayAttrib)
		LOG_ERROR(("No glEnableVertexAttribArray function"));
	else
		(this->*m_thunk_glEnableVertexArrayAttrib)(vao,index);
}

void OOGL::StateFns::call_glEnableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	(*((PFNGLENABLEVERTEXARRAYATTRIBPROC)m_fn_glEnableVertexArrayAttrib))(vao->m_array,index);

	OOGL_CHECK("glEnableVertexArrayAttrib");
}

void OOGL::StateFns::call_glEnableVertexAttribArray(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	vao->bind();

	(*((PFNGLENABLEVERTEXATTRIBARRAYPROC)m_fn_glEnableVertexArrayAttrib))(index);

	OOGL_CHECK("glEnableVertexAttribArray");
}

void OOGL::StateFns::glEnableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	(this->*m_thunk_glEnableVertexArrayAttrib)(vao,index);
}

void OOGL::StateFns::check_glDisableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	if (!m_fn_glDisableVertexArrayAttrib)
	{
		if (isGLversion(3,0) || glfwExtensionSupported("GL_ARB_vertex_array_object") == GL_TRUE)
		{
			if (isGLversion(4,5) || glfwExtensionSupported("GL_ARB_direct_state_access") == GL_TRUE)
			{
				m_fn_glDisableVertexArrayAttrib = glfwGetProcAddress("glDisableVertexArrayAttrib");
				if (m_fn_glDisableVertexArrayAttrib)
					m_thunk_glDisableVertexArrayAttrib = &StateFns::call_glDisableVertexArrayAttrib;
			}

			if (!m_fn_glDisableVertexArrayAttrib && glfwExtensionSupported("GL_EXT_direct_state_access") == GL_TRUE)
			{
				m_fn_glDisableVertexArrayAttrib = glfwGetProcAddress("glDisableVertexArrayAttribEXT");
				if (m_fn_glDisableVertexArrayAttrib)
					m_thunk_glDisableVertexArrayAttrib = &StateFns::call_glDisableVertexArrayAttrib;
			}

			if (!m_fn_glDisableVertexArrayAttrib)
			{
				m_fn_glDisableVertexArrayAttrib = glfwGetProcAddress("glDisableVertexAttribArray");
				if (!m_fn_glDisableVertexArrayAttrib)
					m_fn_glDisableVertexArrayAttrib = glfwGetProcAddress("glDisableVertexAttribArrayARB");

				if (m_fn_glDisableVertexArrayAttrib)
					m_thunk_glDisableVertexArrayAttrib = &StateFns::call_glDisableVertexAttribArray;
			}
		}
	}

	if (!m_fn_glDisableVertexArrayAttrib)
		LOG_ERROR(("No glDisableVertexAttribArray function"));
	else
		(this->*m_thunk_glDisableVertexArrayAttrib)(vao,index);
}

void OOGL::StateFns::call_glDisableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	(*((PFNGLDISABLEVERTEXARRAYATTRIBPROC)m_fn_glDisableVertexArrayAttrib))(vao->m_array,index);

	OOGL_CHECK("glDisableVertexArrayAttrib");
}

void OOGL::StateFns::call_glDisableVertexAttribArray(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	vao->bind();

	(*((PFNGLDISABLEVERTEXATTRIBARRAYPROC)m_fn_glDisableVertexArrayAttrib))(index);

	OOGL_CHECK("glDisableVertexAttribArray");
}

void OOGL::StateFns::glDisableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index)
{
	(this->*m_thunk_glDisableVertexArrayAttrib)(vao,index);
}

void OOGL::StateFns::glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
{
	if (load_proc(m_fn_glVertexAttribPointer,"glVertexAttribPointer"))
	{
		(*m_fn_glVertexAttribPointer)(index,size,type,normalized,stride,pointer);

		OOGL_CHECK("glVertexAttribPointer");
	}
}

bool OOGL::StateFns::check_glVertexAttribIPointer()
{
	if (!m_fn_glVertexAttribIPointer)
	{
		if (isGLversion(3,0))
			m_fn_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)glfwGetProcAddress("glVertexAttribIPointer");

		if (!m_fn_glVertexAttribIPointer &&
				glfwExtensionSupported("GL_ARB_vertex_array_object") == GL_TRUE &&
				(glfwExtensionSupported("GL_EXT_gpu_shader4") == GL_TRUE || glfwExtensionSupported("NV_gpu_program4") == GL_TRUE))
		{
			m_fn_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)glfwGetProcAddress("glVertexAttribIPointerEXT");
		}

		if (!m_fn_glVertexAttribIPointer)
			LOG_ERROR(("No glVertexAttribIPointer function"));
	}

	return m_fn_glVertexAttribIPointer != NULL;
}

void OOGL::StateFns::glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer)
{
	if (check_glVertexAttribIPointer())
	{
		(*m_fn_glVertexAttribIPointer)(index,size,type,stride,pointer);

		OOGL_CHECK("glVertexAttribIPointer");
	}
}

void OOGL::StateFns::check_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount)
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

	(this->*m_thunk_glMultiDrawArrays)(mode,first,count,drawcount);
}

void OOGL::StateFns::call_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount)
{
	(*((PFNGLMULTIDRAWARRAYSPROC)m_fn_glMultiDrawArrays))(mode,first,count,drawcount);

	OOGL_CHECK("glMultiDrawArrays");
}

void OOGL::StateFns::emulate_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount)
{
	for (GLsizei i = 0; i < drawcount; ++i)
	{
		if (count[i] > 0)
		{
			glDrawArrays(mode,first[i],count[i]);

			OOGL_CHECK("glDrawArrays");
		}
	}
}

void OOGL::StateFns::glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount)
{
	(this->*m_thunk_glMultiDrawArrays)(mode,first,count,drawcount);

	OOGL_CHECK("glMultiDrawArrays");
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

	OOGL_CHECK("glDrawArraysInstanced");
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

	OOGL_CHECK("glDrawArraysInstancedBaseInstance");
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

	OOGL_CHECK("glDrawRangeElements");
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

	OOGL_CHECK("glDrawRangeElementsBaseVertex");
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

	OOGL_CHECK("glDrawElementsBaseVertex");
}

void OOGL::StateFns::glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	(this->*m_thunk_glDrawElementsBaseVertex)(mode,count,type,offset,basevertex);
}

void OOGL::StateFns::check_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount)
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
	
	(this->*m_thunk_glMultiDrawElements)(mode,count,type,offsets,drawcount);
}

void OOGL::StateFns::call_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount)
{
	(*((PFNGLMULTIDRAWELEMENTSPROC)m_fn_glMultiDrawElements))(mode,count,type,(const void* const*)offsets,drawcount);

	OOGL_CHECK("glMultiDrawElements");
}

void OOGL::StateFns::emulate_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount)
{
	for (GLsizei i = 0; i < drawcount; ++i)
	{
		if (count[i] > 0)
		{
			glDrawElements(mode,count[i],type,(const void*)(offsets[i]));

			OOGL_CHECK("glDrawElements");
		}
	}
}

void OOGL::StateFns::glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount)
{
	(this->*m_thunk_glMultiDrawElements)(mode,count,type,offsets,drawcount);
}

void OOGL::StateFns::check_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex)
{
	if (isGLversion(3,2) || glfwExtensionSupported("GL_ARB_draw_elements_base_vertex") == GL_TRUE)
	{
		m_fn_glMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)glfwGetProcAddress("glMultiDrawElementsBaseVertex");
		if (m_fn_glMultiDrawElementsBaseVertex)
			m_thunk_glMultiDrawElementsBaseVertex = &StateFns::call_glMultiDrawElementsBaseVertex;
	}

	if (!m_fn_glMultiDrawElementsBaseVertex)
		m_thunk_glMultiDrawElementsBaseVertex = &StateFns::emulate_glMultiDrawElementsBaseVertex;
	
	(this->*m_thunk_glMultiDrawElementsBaseVertex)(mode,count,type,offsets,drawcount,basevertex);
}

void OOGL::StateFns::call_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex)
{
	(*m_fn_glMultiDrawElementsBaseVertex)(mode,count,type,(const void* const*)offsets,drawcount,basevertex);

	OOGL_CHECK("glMultiDrawElementsBaseVertex");
}

void OOGL::StateFns::emulate_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex)
{
	for (GLsizei i = 0; i < drawcount; ++i)
	{
		if (count[i] > 0)
			glDrawElementsBaseVertex(mode,count[i],type,offsets[i],basevertex[i]);
	}
}

void OOGL::StateFns::glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex)
{
	(this->*m_thunk_glMultiDrawElementsBaseVertex)(mode,count,type,offsets,drawcount,basevertex);
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

	OOGL_CHECK("glDrawElementsInstanced");
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

	OOGL_CHECK("glDrawElementsInstancedBaseVertex");
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

	OOGL_CHECK("glDrawElementsInstancedBaseInstance");
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

	OOGL_CHECK("glDrawElementsInstancedBaseVertexBaseInstance");
}

void OOGL::StateFns::glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex, GLuint baseinstance)
{
	(this->*m_thunk_glDrawElementsInstancedBaseVertexBaseInstance)(mode,count,type,offset,instancecount,basevertex,baseinstance);
}
