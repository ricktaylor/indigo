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

#include "Shader.h"
#include "StateFns.h"

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

Indigo::Shader::Shader(GLenum shaderType) : m_id(StateFns::get_current()->glCreateShader(shaderType))
{
}

Indigo::Shader::~Shader()
{
	StateFns::get_current()->glDeleteShader(m_id);
}

const GLchar* Indigo::Shader::get_gstap()
{
	return s_gstap;
}

void Indigo::Shader::compile(const GLchar* sz, GLint len)
{
	if (len)
		compile(&sz,&len,1);
	else
		compile(&sz,NULL,1);
}

void Indigo::Shader::compile(const GLchar *const *strings, GLsizei count)
{
	compile(strings,NULL,count);
}

void Indigo::Shader::compile(const GLchar *const *strings, const GLint* lengths, GLsizei count)
{
	OOBase::SharedPtr<Indigo::StateFns> fns = StateFns::get_current();

	fns->glShaderSource(m_id,count,strings,lengths);
	fns->glCompileShader(m_id);
}

bool Indigo::Shader::compile_status() const
{
	GLint status = GL_FALSE;
	StateFns::get_current()->glGetShaderiv(m_id,GL_COMPILE_STATUS,&status);
	return (status == GL_TRUE);
}

OOBase::String Indigo::Shader::info_log() const
{
	OOBase::SharedPtr<Indigo::StateFns> fns = StateFns::get_current();

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

Indigo::Program::Program() : m_id(0)
{
}

bool Indigo::Program::link_status() const
{
	GLint status;
	StateFns::get_current()->glGetProgramiv(m_id,GL_LINK_STATUS,&status);
	return (status == GL_TRUE);
}

OOBase::String Indigo::Program::info_log() const
{
	OOBase::SharedPtr<Indigo::StateFns> fns = StateFns::get_current();

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

void Indigo::Program::link(const OOBase::SharedPtr<Shader>* shaders, size_t count)
{
	OOBase::SharedPtr<Indigo::StateFns> fns = StateFns::get_current();

	for (size_t i=0;i<count;++i)
		fns->glAttachShader(m_id,shaders[i]->m_id);

	fns->glLinkProgram(m_id);

	for (size_t i=0;i<count;++i)
		fns->glDetachShader(m_id,shaders[i]->m_id);
}

bool Indigo::Program::in_use() const
{
	GLint id = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM,&id);
	return (static_cast<GLuint>(id) == m_id);
}

void Indigo::Program::use()
{
	StateFns::get_current()->glUseProgram(m_id);
}
