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

namespace
{
	GLsizei calc_levels(GLsizei width)
	{
		GLsizei levels = 1;
		while (width >>= 1)
			++levels;
		return levels;
	}

	GLsizei calc_levels(GLsizei width, GLsizei height)
	{
		return calc_levels(width > height ? width : height);
	}

	GLsizei calc_levels(GLsizei width, GLsizei height, GLsizei depth)
	{
		return calc_levels(width > height ? width : height,depth);
	}
}

OOGL::Texture::Texture(GLenum target) :
		m_tex(0),
		m_target(target)
{
	init();
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
	if (!levels)
		levels = calc_levels(width);
	
	init();

	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	if (!fns->check_glTextureStorage())
		init_mutable(levels,internalFormat,width,0,0,NULL);
	else
		fns->glTextureStorage1D(*State::get_current(),m_tex,m_target,levels,internalFormat,width);
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) :
		m_tex(0),
		m_target(target)
{
	if (!levels)
		levels = calc_levels(width,height);

	init();

	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	if (!fns->check_glTextureStorage())
		init_mutable(levels,internalFormat,width,height,0,0,NULL);
	else
		fns->glTextureStorage2D(*State::get_current(),m_tex,m_target,levels,internalFormat,width,height);
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth) :
		m_tex(0),
		m_target(target)
{
	if (!levels)
		levels = calc_levels(width,height,depth);

	init();

	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	if (!fns->check_glTextureStorage())
		init_mutable(levels,internalFormat,width,height,depth,0,0,NULL);
	else
		fns->glTextureStorage3D(*State::get_current(),m_tex,m_target,levels,internalFormat,width,height,depth);
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* pixels) :
		m_tex(0),
		m_target(target)
{
	if (!levels)
		levels = calc_levels(width);

	init();

	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	if (!fns->check_glTextureStorage())
		init_mutable(levels,internalFormat,width,format,type,pixels);
	else
	{
		OOBase::SharedPtr<State> state = State::get_current();
		fns->glTextureStorage1D(*state,m_tex,m_target,levels,internalFormat,width);
		fns->glTextureSubImage1D(*state,m_tex,m_target,0,0,width,format,type,pixels);

		if (levels > 1)
			fns->glGenerateTextureMipmap(*state,m_tex,m_target);
		else
		{
			parameter(GL_TEXTURE_BASE_LEVEL,0);
			parameter(GL_TEXTURE_MAX_LEVEL,0);
		}
	}
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) :
		m_tex(0),
		m_target(target)
{
	if (!levels)
		levels = calc_levels(width,height);

	init();

	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	if (!fns->check_glTextureStorage())
		init_mutable(levels,internalFormat,width,height,format,type,pixels);
	else
	{
		OOBase::SharedPtr<State> state = State::get_current();
		fns->glTextureStorage2D(*state,m_tex,m_target,levels,internalFormat,width,height);
		fns->glTextureSubImage2D(*state,m_tex,m_target,0,0,0,width,height,format,type,pixels);

		if (levels > 1)
			fns->glGenerateTextureMipmap(*state,m_tex,m_target);
		else
		{
			parameter(GL_TEXTURE_BASE_LEVEL,0);
			parameter(GL_TEXTURE_MAX_LEVEL,0);
		}
	}
}

OOGL::Texture::Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels) :
		m_tex(0),
		m_target(target)
{
	if (!levels)
		levels = calc_levels(width,height,depth);

	init();

	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	if (!fns->check_glTextureStorage())
		init_mutable(levels,internalFormat,width,height,depth,format,type,pixels);
	else
	{
		OOBase::SharedPtr<State> state = State::get_current();
		fns->glTextureStorage3D(*state,m_tex,m_target,levels,internalFormat,width,height,depth);
		fns->glTextureSubImage3D(*state,m_tex,m_target,0,0,0,0,width,height,depth,format,type,pixels);

		if (levels > 1)
			fns->glGenerateTextureMipmap(*state,m_tex,m_target);
		else
		{
			parameter(GL_TEXTURE_BASE_LEVEL,0);
			parameter(GL_TEXTURE_MAX_LEVEL,0);
		}
	}
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

void OOGL::Texture::init()
{
	glGenTextures(1,&m_tex);
}

void OOGL::Texture::init_mutable(GLsizei levels, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	OOBase::SharedPtr<State> state = State::get_current();
	bind(*state,state->active_texture_unit());
	
	if (!pixels || levels == 1)
	{
		parameter(GL_TEXTURE_BASE_LEVEL,0);
		parameter(GL_TEXTURE_MAX_LEVEL,levels-1);
	
		for (GLsizei i = 0; i < levels; ++i)
		{
			glTexImage1D(m_target,0,internalFormat,width,0,format,type,pixels);
			width /= 2;
			if (!width)
				width = 1;
		}
	}
	else
	{
		OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
		if (fns->check_glGenerateMipmap())
		{
			glTexImage1D(m_target,0,internalFormat,width,0,format,type,pixels);
			fns->glGenerateTextureMipmap(*state,m_tex,m_target);
		}
		else
		{
			parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); 
			parameter(GL_GENERATE_MIPMAP,GL_TRUE);
			glTexImage1D(m_target,0,internalFormat,width,0,format,type,pixels);
			parameter(GL_GENERATE_MIPMAP,GL_FALSE);
		}
	}
}

void OOGL::Texture::init_mutable(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	OOBase::SharedPtr<State> state = State::get_current();
	bind(*state,state->active_texture_unit());

	if (!pixels || levels == 1)
	{
		parameter(GL_TEXTURE_BASE_LEVEL,0);
		parameter(GL_TEXTURE_MAX_LEVEL,levels-1);

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
				glTexImage2D(m_target,0,internalFormat,width,height,0,format,type,pixels);

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
	else
	{
		OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
		if (fns->check_glGenerateMipmap())
		{
			glTexImage2D(m_target,0,internalFormat,width,height,0,format,type,pixels);
			fns->glGenerateTextureMipmap(*state,m_tex,m_target);
		}
		else
		{
			if (m_target == GL_TEXTURE_2D)
				glEnable(GL_TEXTURE_2D);

			parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); 
			parameter(GL_GENERATE_MIPMAP,GL_TRUE);
			glTexImage2D(m_target,0,internalFormat,width,height,0,format,type,pixels);
			parameter(GL_GENERATE_MIPMAP,GL_FALSE);
		}
	}
}

void OOGL::Texture::init_mutable(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	OOBase::SharedPtr<State> state = State::get_current();
	bind(*state,state->active_texture_unit());

	if (!pixels || levels == 1)
	{
		parameter(GL_TEXTURE_BASE_LEVEL,0);
		parameter(GL_TEXTURE_MAX_LEVEL,levels-1);

		for (GLsizei i = 0; i < levels; ++i)
		{
			StateFns::get_current()->glTexImage3D(m_target,0,internalFormat,width,height,depth,0,format,type,pixels);

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
	else
	{
		OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
		if (fns->check_glGenerateMipmap())
		{
			fns->glTexImage3D(m_target,0,internalFormat,width,height,depth,0,format,type,pixels);
			fns->glGenerateTextureMipmap(*state,m_tex,m_target);
		}
		else
		{
			parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR); 
			parameter(GL_GENERATE_MIPMAP,GL_TRUE);
			fns->glTexImage3D(m_target,0,internalFormat,width,height,depth,0,format,type,pixels);
			parameter(GL_GENERATE_MIPMAP,GL_FALSE);
		}
	}
}

void OOGL::Texture::sub_image(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	StateFns::get_current()->glTextureSubImage1D(*State::get_current(),m_tex,m_target,level,xoffset,width,format,type,pixels);
}

void OOGL::Texture::sub_image(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	StateFns::get_current()->glTextureSubImage2D(*State::get_current(),m_tex,m_target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::Texture::sub_image(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	StateFns::get_current()->glTextureSubImage2D(*State::get_current(),m_tex,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void OOGL::Texture::sub_image(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	StateFns::get_current()->glTextureSubImage3D(*State::get_current(),m_tex,m_target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

GLenum OOGL::Texture::target() const
{
	return m_target;
}

void OOGL::Texture::bind(State& state, GLenum unit) const
{
	StateFns::get_current()->glBindTextureUnit(state,unit,m_target,m_tex);
}

void OOGL::Texture::parameter(GLenum name, GLfloat val)
{
	StateFns::get_current()->glTextureParameterf(*State::get_current(),m_tex,m_target,name,val);
}

void OOGL::Texture::parameter(GLenum name, const GLfloat* pval)
{
	StateFns::get_current()->glTextureParameterfv(*State::get_current(),m_tex,m_target,name,pval);
}

void OOGL::Texture::parameter(GLenum name, GLint val)
{
	StateFns::get_current()->glTextureParameteri(*State::get_current(),m_tex,m_target,name,val);
}

void OOGL::Texture::parameter(GLenum name, const GLint* pval)
{
	StateFns::get_current()->glTextureParameteriv(*State::get_current(),m_tex,m_target,name,pval);
}

OOGL::MutableTexture::MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width) : 
		Texture(target)
{
	if (!levels)
		levels = calc_levels(width);

	init_mutable(levels,internalFormat,width,0,0,NULL);
}

OOGL::MutableTexture::MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height) : 
		Texture(target)
{
	if (!levels)
		levels = calc_levels(width,height);

	init_mutable(levels,internalFormat,width,height,0,0,NULL);
}

OOGL::MutableTexture::MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth) : 
		Texture(target)
{
	if (!levels)
		levels = calc_levels(width,height,depth);

	init_mutable(levels,internalFormat,width,height,depth,0,0,NULL);
}

OOGL::MutableTexture::MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* pixels) : 
		Texture(target)
{
	if (!levels)
		levels = calc_levels(width);

	init_mutable(levels,internalFormat,width,format,type,pixels);
}

OOGL::MutableTexture::MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels) : 
		Texture(target)
{
	if (!levels)
		levels = calc_levels(width,height);

	init_mutable(levels,internalFormat,width,height,format,type,pixels);
}

OOGL::MutableTexture::MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels) : 
		Texture(target)
{
	if (!levels)
		levels = calc_levels(width,height,depth);

	init_mutable(levels,internalFormat,width,height,depth,format,type,pixels);
}
