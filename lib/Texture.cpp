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

#include "Texture.h"
#include "State.h"

Indigo::Texture::Texture(GLenum type) : m_tex(0), m_type(type)
{
	glGenTextures(1,&m_tex);
}

Indigo::Texture::~Texture()
{
	glDeleteTextures(1,&m_tex);
}

GLenum Indigo::Texture::type() const
{
	return m_type;
}

bool Indigo::Texture::is_valid() const
{
	return (glIsTexture(m_tex) == GL_TRUE);
}

void Indigo::Texture::bind(State& state, GLenum unit) const
{
	state.bind_multi_texture(unit,m_type,m_tex);
}
