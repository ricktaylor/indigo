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

#include "Font.h"
#include "Shader.h"
#include "StateFns.h"
#include "Image.h"
#include "VertexArrayObject.h"
#include "BufferObject.h"

#include <OOBase/ByteSwap.h>
#include <OOBase/Logger.h>

namespace
{
	static OOBase::uint32_t read_uint32(const unsigned char*& data)
	{
		OOBase::uint32_t r = 0;
		memcpy(&r,data,4);
		data += 4;

#if (OOBASE_BYTE_ORDER == OOBASE_BIG_ENDIAN)
		OOBase::byte_swap(r);
#endif
		return r;
	}

	static OOBase::uint16_t read_uint16(const unsigned char*& data)
	{
		OOBase::uint16_t r = 0;
		memcpy(&r,data,2);
		data += 2;

#if (OOBASE_BYTE_ORDER == OOBASE_BIG_ENDIAN)
		OOBase::byte_swap(r);
#endif
		return r;
	}

	const char utf8_data[256] =
	{
		// Key:
		//  0 = Invalid first byte
		//  1 = Single byte
		//  2 = 2 byte sequence
		//  3 = 3 byte sequence
		//  4 = 4 byte sequence

		// -1 = Continuation byte
		// -2 = Overlong 2 byte sequence
		// -3 = 3 byte overlong check (0x80..0x9F) as next byte fail
		// -4 = 3 byte reserved check (0xA0..0xBF) as next byte fail
		// -5 = 4 byte overlong check (0x80..0x8F) as next byte fail
		// -6 = 4 byte reserved check (0x90..0xBF) as next byte fail

		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x00..0x0F
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x10..0x1F
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x20..0x2F
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x30..0x3F
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x40..0x4F
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x50..0x5F
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x60..0x6F
		 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 0x70..0x7F
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  // 0x80..0x8F
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  // 0x90..0x9F
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  // 0xA0..0xAF
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  // 0xB0..0xBF
		-2,-2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xC0..0xCF
		 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 0xD0..0xDF
		-3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,-4, 3, 3,  // 0xE0..0xEF
		-5, 4, 4, 4,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0   // 0xF0..0xFF
	};

	const OOBase::uint32_t utf_subst_val = 0xFFFD;

	bool utf8_to_glyphs(const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s, OOBase::Vector<OOBase::uint32_t,OOBase::ThreadLocalAllocator>& glyphs)
	{
		const unsigned char* sz = reinterpret_cast<const unsigned char*>(s.c_str());
		const unsigned char* end = sz + s.length();

		while (sz < end)
		{
			// Default to substitution value
			OOBase::uint32_t wide_val = utf_subst_val;
			int l = 0;

			unsigned char c = *sz++;
			switch (utf8_data[c])
			{
				case 1:
					wide_val = c;
					break;

				case 2:
					wide_val = (c & 0x1F);
					l = 1;
					break;

				case -2:
					++sz;
					break;

				case 3:
					wide_val = (c & 0x0F);
					l = 2;
					break;

				case -3:
					if (*sz >= 0x80 && *sz <= 0x9F)
						sz += 2;
					else
					{
						wide_val = (c & 0x0F);
						l = 2;
					}
					break;

				case 4:
					wide_val = (c & 0x07);
					l = 3;
					break;

				case -4:
					if (*sz >= 0xA0 && *sz <= 0xBF)
						sz += 2;
					else
					{
						wide_val = (c & 0x0F);
						l = 2;
					}
					break;

				case -5:
					if (*sz >= 0x80 && *sz <= 0x8F)
						sz += 3;
					else
					{
						wide_val = (c & 0x07);
						l = 3;
					}
					break;

				case -6:
					if (*sz >= 0x90 && *sz <= 0xBF)
						sz += 3;
					else
					{
						wide_val = (c & 0x07);
						l = 3;
					}
					break;

				default:
					break;
			}

			while (l-- > 0)
			{
				c = *sz++;
				if (utf8_data[c] != -1)
				{
					wide_val = utf_subst_val;
					sz += l;
					break;
				}

				wide_val <<= 6;
				wide_val |= (c & 0x3F);
			}

			if (!glyphs.push_back(wide_val))
				return false;
		}

		return true;
	}

	struct attrib_data
	{
		float x;
		float y;
		GLushort u;
		GLushort v;
	};
}

OOGL::Font::Font()
{
}

OOGL::Font::~Font()
{
}

bool OOGL::Font::load(const unsigned char* data, size_t len, ...)
{
	// BMF\0x3
	if (data[0] != 66 || data[1] != 77 || data[2] != 70 || data[3] != 3)
		LOG_ERROR_RETURN(("Failed to load font data: Format not recognised"),false);

	const unsigned char* end = data + len;
	va_list textures;
	va_start(textures,len);

	unsigned int tex_width, tex_height, pages;

	bool ok = true;
	for (data += 4;ok && data < end;)
	{
		unsigned int type = *data++;
		len = read_uint32(data);

		switch (type)
		{
		case 1:
			m_size = read_uint16(data);
			OOBase::Logger::log(OOBase::Logger::Information,"Loading font: %s %u",data + 12,m_size);
			data += len - 2;
			break;

		case 2:
			assert(len == 15);
			m_line_height = read_uint16(data) / static_cast<float>(m_size);
			data += 2;
			tex_width = read_uint16(data);
			tex_height = read_uint16(data);
			pages = read_uint16(data);
			if (!pages)
			{
				LOG_ERROR(("No textures in font!"));
				ok = false;
			}
			else if (pages == 1)
			{
				const unsigned char* page_data = va_arg(textures,const unsigned char*);
				size_t page_len = va_arg(textures,size_t);

				OOGL::Image img;
				if ((ok = img.load(page_data,static_cast<int>(page_len))))
					ok = (m_ptrTexture = img.make_texture(GL_R8));
			}
			else
			{
				if (!StateFns::get_current()->check_glTextureArray())
				{
					LOG_ERROR(("Multiple textures in font, no texture array support"));
					ok = false;
				}
				for (unsigned int p=0;ok && p<pages;++p)
				{
					// Load page
					const unsigned char* page_data = va_arg(textures,const unsigned char*);
					size_t page_len = va_arg(textures,size_t);

					OOGL::Image img;
					if ((ok = img.load(page_data,static_cast<int>(page_len))))
					{
						// TODO: Load into array texture
					}
				}
			}
			if (*data++ == 1)
			{
				// TODO: Packed data
				data += 4;
			}
			else
			{
				m_packing = read_uint32(data);
				if (m_packing == 0x04040400)
					ok = load_8bit_shader();
				else
				{
					// TODO: Funky packing
				}
			}
			break;

		case 3:
			data += len;
			break;

		case 4:
			for (size_t c = 0;ok && c < len / 20; ++c)
			{
				unsigned int ushort_max = 0x10000;
				struct char_info ci;
				OOBase::uint32_t id = read_uint32(data);
				ci.u0 = read_uint16(data) * (ushort_max / tex_width);
				ci.v0 = read_uint16(data) * (ushort_max / tex_height);
				OOBase::uint16_t width = read_uint16(data);
				OOBase::uint16_t height = read_uint16(data);
				ci.u1 = ci.u0 + (width  * (ushort_max / tex_width));
				ci.v1 = ci.v0 + (height  * (ushort_max / tex_height));
				ci.left = read_uint16(data) / static_cast<float>(m_size);
				ci.top = 1.0f - (read_uint16(data) / static_cast<float>(m_size));
				ci.right = ci.left + (width / static_cast<float>(m_size));
				ci.bottom = ci.top - (height / static_cast<float>(m_size));

				ci.xadvance = read_uint16(data) / static_cast<float>(m_size);
				ci.page = *data++;
				ci.channel = *data++;

				if (!(ok = m_mapCharInfo.insert(id,ci)))
					LOG_ERROR(("Failed to add character to table: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));
			}
			break;

		case 5:
			for (size_t c = 0;ok && c < len / 10; ++c)
			{
				OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t> ch;
				ch.first = read_uint32(data);
				ch.second = read_uint32(data);
				float offset = read_uint16(data) / static_cast<float>(m_size);
				if (!(ok = m_mapKerning.insert(ch,offset)))
					LOG_ERROR(("Failed to add character to kerning table: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));
			}
			break;

		default:
			LOG_WARNING(("Unknown block type found in file: %u",type));
			data += len;
			ok = false;
			break;
		}
	}

	va_end(textures);

	if (!ok)
		return false;

	// Set up texture
	if (m_ptrTexture)
	{
		m_ptrTexture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		m_ptrTexture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	}

	if (data != end)
		LOG_WARNING(("Extra bytes at end of font data"));

	return true;
}

bool OOGL::Font::load_8bit_shader()
{
	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_VERTEX_SHADER);
	shaders[0]->compile(
			"#version 120\n"
			"attribute vec3 in_Position;\n"
			"attribute vec2 in_TexCoord;\n"
			"uniform vec4 in_Colour;\n"
			"uniform mat4 MVP;\n"
			"varying vec4 pass_Colour;\n"
			"varying vec2 pass_TexCoord;\n"
			"void main() {\n"
			"	pass_Colour = in_Colour;\n"
			"	pass_TexCoord = in_TexCoord;\n"
			"	vec4 v = vec4(in_Position,1.0);\n"
			"	gl_Position = MVP * v;\n"
			"}\n");
	OOBase::SharedString<OOBase::ThreadLocalAllocator> s = shaders[0]->info_log();
	if (!s.empty())
		LOG_ERROR_RETURN(("Failed to compile vertex shader: %s",s.c_str()),false);

	shaders[1] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_FRAGMENT_SHADER);
	shaders[1]->compile(
			"#version 120\n"
			"uniform sampler2D texture0;\n"
			"varying vec4 pass_Colour;\n"
			"varying vec2 pass_TexCoord;\n"
			"void main() {\n"
			"	gl_FragColor = texture2D(texture0,pass_TexCoord).rrrr * pass_Colour;\n"
			"}\n");
	s = shaders[1]->info_log();
	if (!s.empty())
		LOG_ERROR_RETURN(("Failed to compile fragment shader: %s",s.c_str()),false);

	m_ptrProgram = OOBase::allocate_shared<OOGL::Program,OOBase::ThreadLocalAllocator>();
	m_ptrProgram->link(shaders,2);
	s = m_ptrProgram->info_log();
	if (!s.empty())
		LOG_ERROR_RETURN(("Failed to link shaders: %s",s.c_str()),false);

	return true;
}

bool OOGL::Font::alloc_text(Text& text, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s)
{
	text.m_glyph_len = 0;

	if (s.empty())
		return true;

	OOBase::Vector<OOBase::uint32_t,OOBase::ThreadLocalAllocator> glyphs;
	if (!utf8_to_glyphs(s,glyphs))
		LOG_ERROR_RETURN(("Failed to parse UTF-8 text: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	size_t len = glyphs.size();

	if (!m_ptrVAO)
	{
		m_ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();
		if (!m_ptrVAO)
			LOG_ERROR_RETURN(("Failed to allocate VAO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		m_ptrVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,len * 4 * sizeof(attrib_data));
		GLint a = m_ptrProgram->attribute_location("in_Position");
		m_ptrVAO->attribute(a,m_ptrVertices,2,GL_FLOAT,false,sizeof(attrib_data),offsetof(attrib_data,x));
		m_ptrVAO->enable_attribute(a);

		a = m_ptrProgram->attribute_location("in_TexCoord");
		if (a != -1)
		{
			m_ptrVAO->attribute(a,m_ptrVertices,2,GL_UNSIGNED_SHORT,true,sizeof(attrib_data),offsetof(attrib_data,u));
			m_ptrVAO->enable_attribute(a);
		}

		m_ptrElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,len * 6 * sizeof(GLuint));
		m_ptrElements->bind();

		text.m_glyph_start = 0;
	}
	else
	{
		// TODO:: Grow!!
	}

	OOBase::SharedPtr<attrib_data> attribs = m_ptrVertices->auto_map<attrib_data>(GL_MAP_WRITE_BIT);
	attrib_data* a = attribs.get();
	OOBase::SharedPtr<GLuint> ei = m_ptrElements->auto_map<GLuint>(GL_MAP_WRITE_BIT);
	GLuint* e = ei.get();
	GLuint idx = 0;
	OOBase::uint32_t prev_glyph = OOBase::uint32_t(-1);
	const OOBase::uint32_t* start = glyphs.data();
	const OOBase::uint32_t* end = start + len;

	text.m_length = 0.0f;

	while (start < end)
	{
		OOBase::uint32_t glyph = *start++;
		if (prev_glyph != OOBase::uint32_t(-1))
		{
			kern_map_t::iterator k = m_mapKerning.find(OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t>(prev_glyph,glyph));
			if (k != m_mapKerning.end())
				text.m_length += k->second;
		}
		prev_glyph = glyph;

		char_map_t::iterator i = m_mapCharInfo.find(glyph);
		if (i == m_mapCharInfo.end())
			i = m_mapCharInfo.find(static_cast<OOBase::uint8_t>('?'));

		a[0].x = text.m_length + i->second.left;
		a[0].y = i->second.top;
		a[1].x = a[0].x;
		a[1].y = i->second.bottom;
		a[2].x = text.m_length + i->second.right;
		a[2].y = a[0].y;
		a[3].x = a[2].x;
		a[3].y = a[1].y;

		a[0].u = i->second.u0;
		a[0].v = i->second.v0;
		a[1].u = a[0].u;
		a[1].v = i->second.v1;
		a[2].u = i->second.u1;
		a[2].v = a[0].v;
		a[3].u = a[2].u;
		a[3].v = a[1].v;
				
		e[0] = idx + 0;
		e[1] = idx + 1;
		e[2] = idx + 2;
		e[3] = idx + 2;
		e[4] = idx + 1;
		e[5] = idx + 3;

		a += 4;
		e += 6;
		idx += 4;

		text.m_length += i->second.xadvance;

		++text.m_glyph_len;
	}
	
	return true;
}

void OOGL::Font::free_text(Text& text)
{

}

void OOGL::Font::draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start, GLsizei len)
{
	state.use(m_ptrProgram);
	state.bind(0,m_ptrTexture);

	m_ptrProgram->uniform("in_Colour",colour);
	m_ptrProgram->uniform("MVP",mvp);

	m_ptrVAO->draw_elements(GL_TRIANGLES,6 * len,GL_UNSIGNED_INT,start);
}

OOGL::Text::Text(const OOBase::SharedPtr<Font>& font, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s) :
		m_font(font), m_str(s), m_glyph_start(0), m_glyph_len(0), m_length(0.0f)
{
	m_font->alloc_text(*this,s);
}

OOGL::Text::~Text()
{
	m_font->free_text(*this);
}

OOBase::SharedString<OOBase::ThreadLocalAllocator> OOGL::Text::text() const
{
	return m_str;
}

bool OOGL::Text::text(const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s)
{
	bool ret = true;
	if (m_str != s)
	{
		m_font->free_text(*this);
		if ((ret = m_font->alloc_text(*this,s)))
			m_str = s;
	}
	return ret;
}

float OOGL::Text::length() const
{
	return m_length;
}

void OOGL::Text::draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start, GLsizei length)
{
	if (start > m_glyph_len)
		start = m_glyph_len;

	if (length == GLsizei(-1))
		length = m_glyph_len - start;

	if (start + length > m_glyph_len)
		length = m_glyph_len - start;

	m_font->draw(state,mvp,colour,m_glyph_start + start,length);
}
