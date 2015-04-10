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

#include "Shader.h"
#include "StateFns.h"

#include <OOBase/StackAllocator.h>

#include <glm/gtc/type_ptr.hpp>

static const GLchar* s_gstap =
		"#ifndef GSTAP_H\n"
		"#define GSTAP_H\n"
		"\n"
		"// gstap.h -- useful for glsl migration\n"
		"// from:\n"
		"//		Mike Bailey and Steve Cunningham\n"
		"//		\"Graphics Shaders: Theory and Practice\",\n"
		"//		Second Edition, AK Peters, 2011.\n"
		"\n"
		"// we are assuming that the compatibility #version line\n"
		"// is given in the source file, for example:\n"
		"// #version 400 compatibility\n"
		"\n"
		"// uniform variables:\n"
		"\n"
		"#define uModelViewMatrix           gl_ModelViewMatrix\n"
		"#define uProjectionMatrix          gl_ProjectionMatrix\n"
		"#define uModelViewProjectionMatrix gl_ModelViewProjectionMatrix\n"
		"#define uNormalMatrix              gl_NormalMatrix\n"
		"#define uModelViewMatrixInverse    gl_ModelViewMatrixInverse\n"
		"\n"
		"// attribute variables:\n"
		"\n"
		"#define aColor   gl_Color\n"
		"#define aNormal  gl_Normal\n"
		"#define aVertex  gl_Vertex\n"
		"\n"
		"#define aTexCoord0   gl_MultiTexCoord0\n"
		"#define aTexCoord1   gl_MultiTexCoord1\n"
		"#define aTexCoord2   gl_MultiTexCoord2\n"
		"#define aTexCoord3   gl_MultiTexCoord3\n"
		"#define aTexCoord4   gl_MultiTexCoord4\n"
		"#define aTexCoord5   gl_MultiTexCoord5\n"
		"#define aTexCoord6   gl_MultiTexCoord6\n"
		"#define aTexCoord7   gl_MultiTexCoord7\n"
		"\n"
		"#endif	// #ifndef GSTAP_H\n";

OOGL::Shader::Shader(GLenum shaderType) : m_id(StateFns::get_current()->glCreateShader(shaderType))
{
}

OOGL::Shader::~Shader()
{
	StateFns::get_current()->glDeleteShader(m_id);
}

const GLchar* OOGL::Shader::get_gstap()
{
	return s_gstap;
}

void OOGL::Shader::compile(const GLchar* sz, GLint len)
{
	if (len)
		compile(&sz,&len,1);
	else
		compile(&sz,NULL,1);
}

void OOGL::Shader::compile(const GLchar *const *strings, GLsizei count)
{
	compile(strings,NULL,count);
}

void OOGL::Shader::compile(const GLchar *const *strings, const GLint* lengths, GLsizei count)
{
	OOBase::SharedPtr<OOGL::StateFns> fns = StateFns::get_current();

	fns->glShaderSource(m_id,count,strings,lengths);
	fns->glCompileShader(m_id);
}

bool OOGL::Shader::compile_status() const
{
	GLint status = GL_FALSE;
	StateFns::get_current()->glGetShaderiv(m_id,GL_COMPILE_STATUS,&status);
	return (status == GL_TRUE);
}

OOBase::String OOGL::Shader::info_log() const
{
	OOBase::SharedPtr<OOGL::StateFns> fns = StateFns::get_current();

	OOBase::String ret;
	GLint len = 0;
	fns->glGetShaderiv(m_id,GL_INFO_LOG_LENGTH,&len);
	if (len)
	{
		OOBase::StackAllocator<256> allocator;
		char* buf = static_cast<char*>(allocator.allocate(len,16));
		if (!buf)
			LOG_ERROR(("Failed to allocate buffer"));
		else
		{
			fns->glGetShaderInfoLog(m_id,len,NULL,buf);
			if (!ret.assign(buf,len))
				LOG_ERROR(("Failed to assign string: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));

			allocator.free(buf);
		}
	}

	return ret;
}

OOGL::Program::Program() :
		m_id(StateFns::get_current()->glCreateProgram())
{
}

OOGL::Program::~Program()
{
	if (m_id)
		StateFns::get_current()->glDeleteProgram(m_id);
}

bool OOGL::Program::link_status() const
{
	GLint status;
	StateFns::get_current()->glGetProgramiv(m_id,GL_LINK_STATUS,&status);
	return (status == GL_TRUE);
}

OOBase::String OOGL::Program::info_log() const
{
	OOBase::SharedPtr<OOGL::StateFns> fns = StateFns::get_current();

	OOBase::String ret;
	GLint len = 0;
	fns->glGetProgramiv(m_id,GL_INFO_LOG_LENGTH,&len);
	if (len)
	{
		OOBase::StackAllocator<256> allocator;
		char* buf = static_cast<char*>(allocator.allocate(len,1));
		if (!buf)
			LOG_ERROR(("Failed to allocate buffer"));
		else
		{
			fns->glGetProgramInfoLog(m_id,len,NULL,buf);
			if (!ret.assign(buf,len))
				LOG_ERROR(("Failed to assign string: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));

			allocator.free(buf);
		}
	}

	return ret;
}

void OOGL::Program::link(const OOBase::SharedPtr<Shader>* shaders, size_t count)
{
	OOBase::SharedPtr<OOGL::StateFns> fns = StateFns::get_current();

	for (size_t i=0;i<count;++i)
		fns->glAttachShader(m_id,shaders[i]->m_id);

	fns->glLinkProgram(m_id);

	for (size_t i=0;i<count;++i)
		fns->glDetachShader(m_id,shaders[i]->m_id);
}

bool OOGL::Program::in_use() const
{
	GLint id = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM,&id);
	return (static_cast<GLuint>(id) == m_id);
}

void OOGL::Program::internal_use() const
{
	StateFns::get_current()->glUseProgram(m_id);
}

GLint OOGL::Program::uniform_location(const char* name) const
{
	map_t::iterator i = m_mapUniforms.find(name);
	if (i != m_mapUniforms.end())
		return i->second;

	GLint l = StateFns::get_current()->glGetUniformLocation(m_id,name);
	if (l != -1)
	{
		OOBase::String s;
		if (s.assign(name))
			m_mapUniforms.insert(s,l);
	}
	return l;
}

GLint OOGL::Program::attribute_location(const char* name) const
{
	map_t::iterator i = m_mapAttributes.find(name);
	if (i != m_mapAttributes.end())
		return i->second;

	GLint l = StateFns::get_current()->glGetAttribLocation(m_id,name);
	if (l != -1)
	{
		OOBase::String s;
		if (s.assign(name))
			m_mapAttributes.insert(s,l);
	}
	return l;
}

void OOGL::Program::uniform(GLint location, const glm::mat4& v, bool transpose)
{
	StateFns::get_current()->glUniformMatrix4fv(shared_from_this(),location,1,transpose ? GL_TRUE : GL_FALSE,glm::value_ptr(v));
}
