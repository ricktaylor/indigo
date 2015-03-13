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

#include "Texture.h"
#include "State.h"
#include "StateFns.h"

OOGL::Texture::Texture(GLenum target) :
		m_tex(0),
		m_target(target)
{
	glGenTextures(1,&m_tex);
}

OOGL::Texture::Texture(GLuint tex, GLenum target) :
		m_tex(tex),
		m_target(target)
{
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width) :
		m_tex(0),
		m_target(target)
{
	create(levels,internalFormat,width);
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) :
		m_tex(0),
		m_target(target)
{
	create(levels,internalFormat,width,height);
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth) :
		m_tex(0),
		m_target(target)
{
	create(levels,internalFormat,width,height,depth);
}

OOGL::Texture::~Texture()
{
	if (m_tex)
		glDeleteTextures(1,&m_tex);
}

OOBase::SharedPtr<OOGL::Texture> OOGL::Texture::none(GLenum target)
{
	return OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(0,target);
}

void OOGL::Texture::create(GLsizei levels, GLenum internalFormat, GLsizei width)
{
	State::get_current()->texture_storage(m_tex,m_target,levels,internalFormat,width);
}

void OOGL::Texture::create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	State::get_current()->texture_storage(m_tex,m_target,levels,internalFormat,width,height);
}

void OOGL::Texture::create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	State::get_current()->texture_storage(m_tex,m_target,levels,internalFormat,width,height,depth);
}

void OOGL::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width)
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

void OOGL::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height)
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

void OOGL::Texture::init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth)
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

void OOGL::Texture::image(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_target,level,xoffset,width,format,type,pixels);
}

void OOGL::Texture::image(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::Texture::image(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::Texture::image(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	State::get_current()->texture_subimage(m_tex,m_target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

GLenum OOGL::Texture::target() const
{
	return m_target;
}

void OOGL::Texture::bind(State& state, GLenum unit) const
{
	state.bind_texture(unit,m_target,m_tex);
}

void OOGL::Texture::parameter(GLenum name, GLfloat val)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,val);
}

void OOGL::Texture::parameter(GLenum name, const GLfloat* pval)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,pval);
}

void OOGL::Texture::parameter(GLenum name, GLint val)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,val);
}

void OOGL::Texture::parameter(GLenum name, const GLint* pval)
{
	State::get_current()->texture_parameter(m_tex,m_target,name,pval);
}
