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

	#define STBI_NO_STDIO
	#define STBI_SUPPORT_ZLIB
	#define STB_IMAGE_IMPLEMENTATION
	#include "../3rdparty/stb/stb_image.h"
}

Indigo::Image::Image() :
		m_width(0),
		m_height(0),
		m_components(0),
		m_valid_components(0),
		m_pixels(NULL)
{
}

Indigo::Image::~Image()
{
	unload();
}

bool Indigo::Image::valid() const
{
	return m_pixels && m_width && m_height && m_valid_components;
}

bool Indigo::Image::load(const ResourceBundle& resource, const char* name, int components)
{
	OOBase::SharedPtr<const unsigned char> buffer = resource.load<unsigned char>(name);
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
		m_valid_components = 0;
		m_pixels = 0;
	}
	
	int x,y,c = 0;
	void* p = stbi_load_from_memory(buffer,(int)len,&x,&y,&c,components);
	if (!p)
		LOG_ERROR_RETURN(("Failed to load image: %s",stbi_failure_reason()),false);

	m_pixels = p;
	m_width = x;
	m_height = y;
	m_valid_components = c;

	if (!components)
		m_components = c;
	else
		m_components = components;

	return true;
}

bool Indigo::Image::create(const glm::vec4& colour)
{
	if (m_pixels)
	{
		stbi_image_free(m_pixels);

		m_width = 0;
		m_height = 0;
		m_components = 0;
		m_valid_components = 0;
		m_pixels = 0;
	}

	void* p = wrap_malloc(4);
	if (!p)
		LOG_ERROR_RETURN(("Failed to allocate image: %s",OOBase::system_error_text()),false);

	unsigned char* rgba = static_cast<unsigned char*>(p);
	glm::vec4 col = glm::clamp(colour,glm::vec4(0.f),glm::vec4(1.f)) * 256.f;
	rgba[0] = static_cast<unsigned char>(col.r);
	rgba[1] = static_cast<unsigned char>(col.g);
	rgba[2] = static_cast<unsigned char>(col.b);
	rgba[3] = static_cast<unsigned char>(col.a);

	m_width = 1;
	m_height = 1;
	m_components = 4;
	m_valid_components = 4;
	m_pixels = p;

	return true;
}

glm::vec4 Indigo::Image::pixel(const glm::uvec2& pos) const
{
	glm::vec4 pixel(0.f);
	if (m_pixels)
	{
		const char* p = static_cast<const char*>(m_pixels) + (m_width * pos.y + pos.x) * m_components;
		pixel.r = p[0]/255.f;
		if (m_components > 1)
			pixel.g = p[1]/255.f;
		if (m_components > 2)
			pixel.b = p[2]/255.f;
		if (m_components > 3)
			pixel.a = p[3]/255.f;
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

OOBase::SharedPtr<OOGL::Texture> Indigo::Image::make_texture(GLenum internalFormat, bool& cached, GLsizei levels) const
{
	OOBase::SharedPtr<OOGL::Texture> tex;
	if (!m_pixels)
		LOG_ERROR_RETURN(("Invalid image for make_texture"),tex);

	if (cached)
	{
		tex = m_texture.lock();
		if (tex)
			return tex;

		cached = false;
	}

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
		LOG_ERROR(("Invalid image for make_texture"));
		return tex;
	}

	tex = OOBase::allocate_shared<OOGL::Texture,OOBase::ThreadLocalAllocator>(GL_TEXTURE_2D,levels,internalFormat,m_width,m_height,format,GL_UNSIGNED_BYTE,m_pixels);
	if (!tex)
		LOG_ERROR_RETURN(("Failed to allocate texture"),tex);

	m_texture = tex;
	return tex;
}
