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

#include "Common.h"
#include "Image.h"

extern "C"
{
	static void* wrap_malloc(size_t sz)
	{
		return OOBase::ThreadLocalAllocator::allocate(sz);
	}

	static void* wrap_realloc(void* p, size_t sz)
	{
		return OOBase::ThreadLocalAllocator::reallocate(p,sz);
	}

	static void wrap_free(void* p)
	{
		OOBase::ThreadLocalAllocator::free(p);
	}

	#define STBI_MALLOC(sz)    wrap_malloc(sz)
	#define STBI_REALLOC(p,sz) wrap_realloc(p,sz)
	#define STBI_FREE(p)       wrap_free(p)

	#define STB_IMAGE_IMPLEMENTATION
	#include "../3rdparty/stb/stb_image.h"
}

Indigo::Image::Image() :
		m_width(0),
		m_height(0),
		m_components(0),
		m_pixels(NULL)
{
}

Indigo::Image::~Image()
{
	unload();
}

bool Indigo::Image::valid() const
{
	return m_pixels != NULL;
}

bool Indigo::Image::load(const ResourceBundle& resource, const char* name, int components)
{
	OOBase::SharedPtr<unsigned char> buffer = resource.load<unsigned char,OOBase::ThreadLocalAllocator>(name);
	if (!buffer)
		return false;

	return this->load(buffer.get(),resource.size(name),components);
}

bool Indigo::Image::load(const unsigned char* buffer, size_t len, int components)
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
		LOG_ERROR(("Failed to load image: %s\n",stbi_failure_reason()));
	else
	{
		m_pixels = p;
		m_width = x;
		m_height = y;
		m_components = c;
	}

	return (p != NULL);
}

glm::vec4 Indigo::Image::pixel(const glm::uvec2& pos) const
{
	glm::vec4 pixel(0.f);
	if (m_pixels)
	{
		const char* p = static_cast<const char*>(m_pixels) + (m_width * pos.y + pos.x) * m_components;
		pixel.r = p[0]/256.f;
		if (m_components > 1)
			pixel.g = p[1]/256.f;
		if (m_components > 2)
			pixel.b = p[2]/256.f;
		if (m_components > 3)
			pixel.a = p[3]/256.f;
	}
	return pixel;
}

void Indigo::Image::unload()
{
	if (m_pixels)
	{
		stbi_image_free(m_pixels);
		m_pixels = NULL;
	}
}

OOBase::SharedPtr<OOGL::Texture> Indigo::Image::make_texture(GLenum internalFormat, GLsizei levels) const
{
	OOBase::SharedPtr<OOGL::Texture> tex;
	if (!m_pixels)
		LOG_ERROR(("Invalid image for make_texture\n"));
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
			LOG_ERROR(("Invalid image for make_texture\n"));
			return tex;
		}

		tex = OOBase::allocate_shared<OOGL::Texture,OOBase::ThreadLocalAllocator>(GL_TEXTURE_2D,levels,internalFormat,m_width,m_height,format,GL_UNSIGNED_BYTE,m_pixels);
	}
	return tex;
}
