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

OOBase::SharedPtr<Indigo::Texture> Indigo::Texture::create(GLenum target)
{
	return OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(target);
}

OOBase::SharedPtr<Indigo::Texture> Indigo::Texture::create(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	OOBase::SharedPtr<Texture> tex = OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(target);
	if (tex)
		tex->do_create(levels,internalFormat,width);
	return tex;
}

OOBase::SharedPtr<Indigo::Texture> Indigo::Texture::create(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	OOBase::SharedPtr<Texture> tex = OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(target);
	if (tex)
		tex->do_create(levels,internalFormat,width,height);
	return tex;
}

OOBase::SharedPtr<Indigo::Texture> Indigo::Texture::create(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	OOBase::SharedPtr<Texture> tex = OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(target);
	if (tex)
		tex->do_create(levels,internalFormat,width,height,depth);
	return tex;
}

Indigo::Texture::Texture(GLenum target) :
		m_tex(0),
		m_target(target)
{
	glGenTextures(1,&m_tex);
}

Indigo::Texture::~Texture()
{
	glDeleteTextures(1,&m_tex);
}

void Indigo::Texture::do_create(GLsizei levels, GLenum internalFormat, GLsizei width)
{
	State::get_current()->texture_storage(m_tex,m_target,levels,internalFormat,width);
}

void Indigo::Texture::do_create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	State::get_current()->texture_storage(m_tex,m_target,levels,internalFormat,width,height);
}

void Indigo::Texture::do_create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	State::get_current()->texture_storage(m_tex,m_target,levels,internalFormat,width,height,depth);
}

void Indigo::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width)
{
	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	OOBase::SharedPtr<State> state = State::get_current();
	state->bind(state->active_texture_unit(),shared_from_this());

	glTexParameteri(m_target,GL_TEXTURE_BASE_LEVEL,0);
	glTexParameteri(m_target,GL_TEXTURE_MAX_LEVEL,levels-1);

	// Keep in line with StateFns::emulate_glTextureStorage1D
	for (GLsizei i = 0; i < levels; ++i)
	{
		glTexImage1D(m_target,i,internalFormat,width,0,0,0,NULL);
		width /= 2;
		if (!width)
			width = 1;
	}
}

void Indigo::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height)
{
	OOBase::SharedPtr<State> state = State::get_current();
	state->bind(state->active_texture_unit(),shared_from_this());

	glTexParameteri(m_target,GL_TEXTURE_BASE_LEVEL,0);
	glTexParameteri(m_target,GL_TEXTURE_MAX_LEVEL,levels-1);

	// Keep in line with StateFns::emulate_glTextureStorage2D
	for (GLsizei i = 0; i < levels; ++i)
	{
		if (m_target == GL_TEXTURE_CUBE_MAP)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,i,internalFormat,width,height,0,0,0,NULL);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,i,internalFormat,width,height,0,0,0,NULL);
		}
		else
			glTexImage2D(m_target,i,internalFormat,width,height,0,0,0,NULL);

		width /= 2;
		if (!width)
			width = 1;

		if (m_target != GL_TEXTURE_1D && m_target != GL_TEXTURE_1D_ARRAY)
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

	glTexParameteri(m_target,GL_TEXTURE_BASE_LEVEL,0);
	glTexParameteri(m_target,GL_TEXTURE_MAX_LEVEL,levels-1);

	// Keep in line with StateFns::emulate_glTextureStorage3D
	for (GLsizei i = 0; i < levels; ++i)
	{
		fns->glTexImage3D(m_target,i,internalFormat,width,height,depth,0,0,0,NULL);

		width /= 2;
		if (!width)
			width = 1;

		height /= 2;
		if (!height)
			height = 1;

		if (m_target != GL_TEXTURE_2D_ARRAY && m_target != GL_PROXY_TEXTURE_2D_ARRAY && m_target != GL_TEXTURE_CUBE_MAP_ARRAY && m_target != GL_PROXY_TEXTURE_CUBE_MAP_ARRAY)
		{
			depth /= 2;
			if (!depth)
				depth = 1;
		}
	}
}

void Indigo::Texture::image(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_target,level,xoffset,width,format,type,pixels);
}

void Indigo::Texture::image(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::Texture::image(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::Texture::image(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

GLenum Indigo::Texture::target() const
{
	return m_target;
}

bool Indigo::Texture::is_valid() const
{
	return (glIsTexture(m_tex) == GL_TRUE);
}

void Indigo::Texture::bind(State& state, GLenum unit) const
{
	state.bind_multi_texture(unit,m_target,m_tex);
}

void Indigo::Texture::parameter(GLenum name, GLfloat val)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,val);
}

void Indigo::Texture::parameter(GLenum name, const GLfloat* pval)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,pval);
}

void Indigo::Texture::parameter(GLenum name, GLint val)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,val);
}

void Indigo::Texture::parameter(GLenum name, const GLint* pval)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,pval);
}
