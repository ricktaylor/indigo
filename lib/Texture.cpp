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
#include "StateFns.h"

Indigo::Texture::Texture(GLenum type) :
		m_tex(0),
		m_type(type),
		m_levels(0),
		m_internalFormat(0),
		m_width(0),
		m_height(0),
		m_depth(0)
{
	glGenTextures(1,&m_tex);
}

Indigo::Texture::~Texture()
{
	glDeleteTextures(1,&m_tex);
}

void Indigo::Texture::create(GLsizei levels, GLenum internalFormat, GLsizei width)
{
	State::get_current()->texture_storage(m_tex,m_type,m_levels,internalFormat,width);

	m_levels = levels;
	m_internalFormat = internalFormat;
	m_width = width;
}

void Indigo::Texture::create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	State::get_current()->texture_storage(m_tex,m_type,m_levels,internalFormat,width,height);

	m_levels = levels;
	m_internalFormat = internalFormat;
	m_width = width;
	m_height = height;
}

void Indigo::Texture::create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	State::get_current()->texture_storage(m_tex,m_type,m_levels,internalFormat,width,height,depth);

	m_levels = levels;
	m_internalFormat = internalFormat;
	m_width = width;
	m_height = height;
	m_depth = depth;
}

void Indigo::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width)
{
	OOBase::SharedPtr<State> state = State::get_current();
	state->bind(state->active_texture_unit(),shared_from_this());

	m_levels = levels;
	m_internalFormat = internalFormat;
	m_width = width;

	// Keep in line with StateFns::emulate_glTextureStorage1D
	for (GLsizei i = 0; i < levels; ++i)
	{
		glTexImage1D(m_type,i,internalFormat,width,0,0,0,NULL);
		width /= 2;
		if (!width)
			width = 1;
	}
}

void Indigo::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height)
{
	OOBase::SharedPtr<State> state = State::get_current();
	state->bind(state->active_texture_unit(),shared_from_this());

	m_levels = levels;
	m_internalFormat = internalFormat;
	m_width = width;
	m_height = height;

	// Keep in line with StateFns::emulate_glTextureStorage2D
	for (GLsizei i = 0; i < levels; ++i)
	{
		if (m_type == GL_TEXTURE_CUBE_MAP)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,i,internalFormat,width,height,0,0,0,NULL);
		}
		else
			glTexImage2D(m_type,i,internalFormat,width,height,0,0,0,NULL);

		width /= 2;
		if (!width)
			width = 1;

		if (m_type != GL_TEXTURE_1D && m_type != GL_TEXTURE_1D_ARRAY)
		{
			height /= 2;
			if (!height)
				height = 1;
		}
	}
}

void Indigo::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	OOBase::SharedPtr<State> state = State::get_current();
	state->bind(state->active_texture_unit(),shared_from_this());

	m_levels = levels;
	m_internalFormat = internalFormat;
	m_width = width;
	m_height = height;
	m_depth = depth;

	// Keep in line with StateFns::emulate_glTextureStorage3D
	for (GLsizei i = 0; i < levels; ++i)
	{
		fns->glTexImage3D(m_type,i,internalFormat,width,height,depth,0,0,0,NULL);

		width /= 2;
		if (!width)
			width = 1;

		height /= 2;
		if (!height)
			height = 1;

		if (m_type != GL_TEXTURE_2D_ARRAY && m_type != GL_PROXY_TEXTURE_2D_ARRAY && m_type != GL_TEXTURE_CUBE_MAP_ARRAY && m_type != GL_PROXY_TEXTURE_CUBE_MAP_ARRAY)
		{
			depth /= 2;
			if (!depth)
				depth = 1;
		}
	}
}

void Indigo::Texture::sub_image(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_type,level,xoffset,width,format,type,pixels);
}

void Indigo::Texture::sub_image(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_type,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::Texture::sub_image_cube(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::Texture::sub_image(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_type,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
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
