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

#ifndef INDIGO_SHADER_H_INCLUDED
#define INDIGO_SHADER_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class ShaderBase : public OOBase::EnableSharedFromThis<ShaderBase>
	{
	public:
		virtual ~ShaderBase();

		void load(const GLchar* sz, bool add_gstap = false);
		void load(const GLchar* sz, GLint len, bool add_gstap = false);
		void load(const GLchar *const *strings, GLsizei count);
		void load(const GLchar *const *strings, const GLint* lengths, GLsizei count);

		template <GLsizei S>
		void load(const GLchar *const strings[S], const GLint lengths[S])
		{
			load(strings,lengths,S);
		}

		template <GLsizei S>
		void load(const GLchar *const strings[S])
		{
			load(strings,S);
		}

		static const GLchar* get_gstap();

	protected:
		ShaderBase(GLenum shaderType);

		GLuint m_id;
	};

	class VertexShader : public ShaderBase
	{
	public:
		VertexShader() : ShaderBase(GL_VERTEX_SHADER)
		{}
	};

	class FragmentShader : public ShaderBase
	{
	public:
		FragmentShader() : ShaderBase(GL_FRAGMENT_SHADER)
		{}
	};

	class ShaderProgram : public OOBase::EnableSharedFromThis<ShaderProgram>
	{
	public:
		ShaderProgram();

	private:
		GLuint m_id;
	};
}

#endif // INDIGO_SHADER_H_INCLUDED
