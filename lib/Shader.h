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
		void load(const GLchar* sz, GLint len = -1);

	protected:
		ShaderBase() : m_id(0)
		{}

		void create(GLenum shaderType);

		GLuint m_id;
	};

	class VertexShader : public ShaderBase
	{
	public:
		VertexShader()
		{
			create(GL_VERTEX_SHADER);
		}
	};
}


#endif // INDIGO_SHADER_H_INCLUDED
