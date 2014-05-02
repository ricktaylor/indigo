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

Indigo::ShaderBase::ShaderBase(GLenum shaderType) : m_id(StateFns::get_current()->glCreateShader(shaderType))
{
}

Indigo::ShaderBase::~ShaderBase()
{
	StateFns::get_current()->glDeleteShader(m_id);
}

const GLchar* Indigo::ShaderBase::get_gstap()
{
	return s_gstap;
}

void Indigo::ShaderBase::load(const GLchar* sz, bool add_gstap)
{
	if (add_gstap)
	{
		const GLchar* strings[] = { s_gstap, sz };
		const GLint lengths[] = { -1, -1 };
		load(strings,lengths,2);
	}
	else
		load(&sz,NULL,1);
}

void Indigo::ShaderBase::load(const GLchar* sz, GLint len, bool add_gstap)
{
	if (add_gstap)
	{
		const GLchar* strings[] = { s_gstap, sz };
		const GLint lengths[] = { -1, len };
		load(strings,lengths,2);
	}
	else
		load(&sz,&len,1);
}

void Indigo::ShaderBase::load(const GLchar *const *strings, GLsizei count)
{
	load(strings,NULL,count);
}

void Indigo::ShaderBase::load(const GLchar *const *strings, const GLint* lengths, GLsizei count)
{
	StateFns::get_current()->glShaderSource(m_id,count,strings,lengths);
}
