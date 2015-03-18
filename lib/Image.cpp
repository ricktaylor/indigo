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

#include "Image.h"

extern "C"
{
	static void* wrap_malloc(size_t sz);
	static void* wrap_realloc(void* p, size_t sz);
	static void wrap_free(void* p);

	#define STBI_MALLOC(sz)    wrap_malloc(sz)
	#define STBI_REALLOC(p,sz) wrap_realloc(p,sz)
	#define STBI_FREE(p)       wrap_free(p)

	#define STB_IMAGE_IMPLEMENTATION
	#include "../3rdparty/stb/stb_image.h"
}

static void* wrap_malloc(size_t sz)
{
	return OOBase::CrtAllocator::allocate(sz);
}

static void* wrap_realloc(void* p, size_t sz)
{
	return OOBase::CrtAllocator::reallocate(p,sz);
}

static void wrap_free(void* p)
{
	OOBase::CrtAllocator::free(p);
}

OOGL::Image::Image() :
		m_width(0),
		m_height(0),
		m_components(0),
		m_pixels(NULL)
{
}

OOGL::Image::~Image()
{
	if (m_pixels)
		stbi_image_free(m_pixels);
}

bool OOGL::Image::load(const char* filename, int components)
{
	if (m_pixels)
	{
		stbi_image_free(m_pixels);

		m_width = 0;
		m_height = 0;
		m_components = 0;
		m_pixels = 0;
	}

	int x,y,c = 0;
	void* p = stbi_load(filename,&x,&y,&c,components);
	if (!p)
		LOG_WARNING(("Failed to load image: %s\n",stbi_failure_reason()));
	else
	{
		m_pixels = p;
		m_width = x;
		m_height = y;
		m_components = c;
	}

	return (p != NULL);
}

bool OOGL::Image::load(const unsigned char* buffer, int len, int components)
{
	if (m_pixels)
	{
		stbi_image_free(m_pixels);

		m_width = 0;
		m_height = 0;
		m_components = 0;
		m_pixels = 0;
	}
	
	int x,y,c = 0;
	void* p = stbi_load_from_memory(buffer,len,&x,&y,&c,components);
	if (!p)
		LOG_WARNING(("Failed to load image: %s\n",stbi_failure_reason()));
	else
	{
		m_pixels = p;
		m_width = x;
		m_height = y;
		m_components = c;
	}

	return (p != NULL);
}

OOBase::SharedPtr<OOGL::Texture> OOGL::Image::make_texture(GLenum internalFormat) const
{
	OOBase::SharedPtr<Texture> tex;
	if (!m_pixels)
		LOG_WARNING(("Invalid image for make_texture\n"));
	else
	{
		GLenum format = 0;
		switch (m_components)
		{
		case 1:
			format = GL_RED;
			break;

		case 2:
			format = GL_RG;
			break;

		case 3:
			format = GL_RGB;
			break;

		case 4:
			format = GL_RGBA;
			break;

		default:
			LOG_WARNING(("Invalid image for make_texture\n"));
			return tex;
		}

		tex = OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(GL_TEXTURE_2D,0,internalFormat,m_width,m_height,format,GL_UNSIGNED_BYTE,m_pixels);
	}
	return tex;
}

/*OOBase::SharedPtr<OOGL::Texture> OOGL::load_targa(const unsigned char* data, size_t len)
{
	GLint old_align = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT,&old_align);

	if ((width * bpp) % old_align == 0)
		old_align = 0;

	if (bpp == 2)
	{
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,2);
		tex->sub_image(0,0,0,width,height,GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,data);
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,old_align);
	}
	else if (bpp == 3)
	{
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		tex->sub_image(0,0,0,width,height,GL_BGR,GL_UNSIGNED_BYTE,data);
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,old_align);
	}
	else
	{
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		tex->sub_image(0,0,0,width,height,GL_BGRA,GL_UNSIGNED_BYTE,data);
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,old_align);
	}

	return tex;
}*/
