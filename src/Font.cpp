///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
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

#include "Font.h"
#include "Render.h"

#include "../lib/Shader.h"
#include "../lib/StateFns.h"
#include "../lib/Image.h"

#include <OOBase/TLSSingleton.h>

namespace Indigo
{
	OOGL::ResourceBundle& static_resources();
}

namespace
{
	OOBase::uint32_t read_uint32(const unsigned char*& data)
	{
		OOBase::uint32_t r = 0;
		memcpy(&r,data,4);
		data += 4;

#if (OOBASE_BYTE_ORDER == OOBASE_BIG_ENDIAN)
		OOBase::byte_swap(r);
#endif
		return r;
	}

	OOBase::uint16_t read_uint16(const unsigned char*& data)
	{
		OOBase::uint16_t r = 0;
		memcpy(&r,data,2);
		data += 2;

#if (OOBASE_BYTE_ORDER == OOBASE_BIG_ENDIAN)
		OOBase::byte_swap(r);
#endif
		return r;
	}

	OOBase::int16_t read_int16(const unsigned char*& data)
	{
		return static_cast<OOBase::int16_t>(read_uint16(data));
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

	GLsizei utf8_to_glyphs(const char* s, size_t s_len, OOBase::Vector<OOBase::uint32_t,OOBase::ThreadLocalAllocator>& glyphs)
	{
		GLsizei drawable = 0;
		const unsigned char* sz = reinterpret_cast<const unsigned char*>(s);
		const unsigned char* end = sz + s_len;

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
				break;

			if (wide_val > 31 && wide_val != 127 && (wide_val < 128 || wide_val > 159))
				++drawable;
		}

		return drawable;
	}

	class FontProgram
	{
	public:
		FontProgram()
		{}

		OOBase::SharedPtr<OOGL::Program> program(OOBase::uint32_t packing);

	private:
		OOBase::SharedPtr<OOGL::Program> m_ptr_ch1_8_Program;

		bool load_8bit_shader();
	};

	struct vertex_data
	{
		float x;
		float y;
		GLushort u;
		GLushort v;
	};

	static const unsigned int vertices_per_glyph = 4;
	static const unsigned int elements_per_glyph = 6;
}

OOBase::SharedPtr<OOGL::Program> FontProgram::program(OOBase::uint32_t packing)
{
	OOBase::SharedPtr<OOGL::Program> program;
	switch (packing)
	{
	case 0x04040400:
		if (!m_ptr_ch1_8_Program)
			load_8bit_shader();
		program = m_ptr_ch1_8_Program;
		break;

	default:
		break;
	}
	return program;
}

bool FontProgram::load_8bit_shader()
{
	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_VERTEX_SHADER);
	if (!shaders[0]->compile(static_cast<const GLchar*>(Indigo::static_resources().load("Font_8bit.vert")),static_cast<GLint>(Indigo::static_resources().size("Font_8bit.vert"))))
		LOG_ERROR_RETURN(("Failed to compile vertex shader: %s",shaders[0]->info_log().c_str()),false);
	
	shaders[1] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_FRAGMENT_SHADER);
	if (!shaders[1]->compile(static_cast<const GLchar*>(Indigo::static_resources().load("Font_8bit.frag")),static_cast<GLint>(Indigo::static_resources().size("Font_8bit.frag"))))
		LOG_ERROR_RETURN(("Failed to compile vertex shader: %s",shaders[1]->info_log().c_str()),false);
	
	OOBase::SharedPtr<OOGL::Program> program = OOBase::allocate_shared<OOGL::Program,OOBase::ThreadLocalAllocator>();
	if (!program)
		LOG_ERROR_RETURN(("Failed to allocate shader program: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!program->link(shaders,2))
		LOG_ERROR_RETURN(("Failed to link shaders: %s",program->info_log().c_str()),false);
	
	m_ptr_ch1_8_Program = program;
	return true;
}

Indigo::Render::Font::Font() : m_line_height(0.f), m_packing(0), m_allocated(0)
{
}

Indigo::Render::Font::~Font()
{
}

bool Indigo::Render::Font::load(const OOGL::ResourceBundle& resource, const char* name)
{
	if (!resource.exists(name))
		LOG_ERROR_RETURN(("Failed to find resource %s",name),false);

	const unsigned char* data = static_cast<const unsigned char*>(resource.load(name));
	size_t len = resource.size(name);

	// BMF\0x3
	if (data[0] != 66 || data[1] != 77 || data[2] != 70 || data[3] != 3)
		LOG_ERROR_RETURN(("Failed to load font data: Format not recognised"),false);

	const unsigned char* end = data + len;
	float tex_width, tex_height;
	unsigned int pages;
	OOBase::Vector<const char*,OOBase::ThreadLocalAllocator> vecPages;

	bool ok = true;
	for (data += 4;ok && data < end;)
	{
		unsigned int type = *data++;
		len = read_uint32(data);

		switch (type)
		{
		case 1:
			{
				OOBase::int16_t size = read_int16(data);
				if (size >= 0)
					OOBase::Logger::log(OOBase::Logger::Information,"Loading font: %s %dpx",data + 12,size);
				else
					OOBase::Logger::log(OOBase::Logger::Information,"Loading font: %s %dpt",data + 12,-size);
				data += len - 2;
			}
			break;

		case 2:
			assert(len == 15);
			m_line_height = read_uint16(data);
			data += 2;
			tex_width = read_uint16(data);
			tex_height = read_uint16(data);
			pages = read_uint16(data);
			if (*data++ == 1)
			{
				// TODO: Packed data
				data += 4;
			}
			else
			{
				m_packing = read_uint32(data);
				if (m_packing == 0x04040400)
					ok = (!!OOGL::ContextSingleton<FontProgram>::instance().program(m_packing));
				else
				{
					// TODO: Funky packing
				}
			}
			if (!pages)
			{
				LOG_ERROR(("No textures in font!"));
				ok = false;
			}
			break;

		case 3:
			if (pages == 1)
			{
				OOGL::Image img;
				if ((ok = img.load(resource,reinterpret_cast<const char*>(data))))
					ok = (m_ptrTexture = img.make_texture(GL_R8));

				data += len;
			}
			else
			{
				if (!OOGL::StateFns::get_current()->check_glTextureArray())
				{
					LOG_ERROR(("Multiple textures in font, no texture array support"));
					ok = false;
				}
				for (unsigned int p=0;ok && p<pages;++p)
				{
					OOGL::Image img;
					if ((ok = img.load(resource,reinterpret_cast<const char*>(data))))
					{
						// TODO: Load into array texture
					}

					data += len/pages;
				}
			}
			break;

		case 4:
			for (size_t c = 0;ok && c < len / 20; ++c)
			{
				unsigned int ushort_max = 0xFFFF;
				struct char_info ci;
				OOBase::uint32_t id = read_uint32(data);
				OOBase::uint32_t u = read_uint16(data);
				OOBase::uint32_t v = read_uint16(data);
				float width = read_uint16(data);
				float height = read_uint16(data);
				OOBase::int32_t x = read_int16(data);
				OOBase::int32_t y = read_int16(data);
				ci.u0 = static_cast<OOBase::uint16_t>(u / tex_width * ushort_max);
				ci.v0 = static_cast<OOBase::uint16_t>(v / tex_height * ushort_max);
				ci.u1 = static_cast<OOBase::uint16_t>((u + width) / tex_width * ushort_max);
				ci.v1 = static_cast<OOBase::uint16_t>((v + height) / tex_height * ushort_max);
				ci.left = x / m_line_height;
				ci.top = 1.0f - (y / m_line_height);
				ci.right = (x + width) / m_line_height;
				ci.bottom = 1.0f - ((y + height) / m_line_height);

				ci.xadvance = read_int16(data) / m_line_height;
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
				float offset = read_int16(data) / m_line_height;
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

	if (!ok)
		return false;

	// Set up texture
	if (m_ptrTexture)
	{
		m_ptrTexture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		m_ptrTexture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		m_ptrTexture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		m_ptrTexture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}

	if (data != end)
		LOG_WARNING(("Extra bytes at end of font data"));

	return true;
}

bool Indigo::Render::Font::alloc_text(Text& text, const char* sz, size_t s_len)
{
	text.m_glyph_len = 0;
	text.m_glyph_start = 0;
	
	OOBase::Vector<OOBase::uint32_t,OOBase::ThreadLocalAllocator> glyphs;
	GLsizei len = utf8_to_glyphs(sz,s_len,glyphs);
	if (!len)
		return true;

	bool found = false;	
	for (free_list_t::iterator i=m_listFree.begin(); i; ++i)
	{
		if (i->second == len)
		{
			text.m_glyph_start = i->first;
			m_listFree.erase(i);
			found = true;
			break;
		}
		else if (i->second > len)
		{
			text.m_glyph_start = i->first;
			i->first += len;
			i->second -= len;
			found = true;
			break;
		}
	}
	
	if (!found)
	{
		GLsizei new_size = 8;
		while (new_size < m_allocated + len)
			new_size *= 2;

		OOBase::SharedPtr<OOGL::BufferObject> ptrNewVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW,new_size * vertices_per_glyph * sizeof(vertex_data));
		OOBase::SharedPtr<OOGL::BufferObject> ptrNewElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_DYNAMIC_DRAW,new_size * elements_per_glyph * sizeof(GLuint));
		if (!ptrNewVertices || !ptrNewElements)
			LOG_ERROR_RETURN(("Failed to allocate VBO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		if (m_ptrVertices)
			ptrNewVertices->copy(0,m_ptrVertices,0,m_allocated * vertices_per_glyph * sizeof(vertex_data));

		if (m_ptrElements)
			ptrNewElements->copy(0,m_ptrElements,0,m_allocated * elements_per_glyph * sizeof(GLuint));

		m_ptrVertices.swap(ptrNewVertices);
		m_ptrElements.swap(ptrNewElements);

		free_list_t::iterator last = m_listFree.back();
		if (last)
			last->second += new_size - m_allocated;
		else
			last = m_listFree.insert(m_allocated,new_size - m_allocated);
		m_allocated = new_size;
		
		if (!m_ptrVAO)
		{
			m_ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();
			if (!m_ptrVAO)
				LOG_ERROR_RETURN(("Failed to allocate VAO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}

		OOBase::SharedPtr<OOGL::Program> ptrProgram = OOGL::ContextSingleton<FontProgram>::instance().program(m_packing);
		if (!ptrProgram)
			return false;

		text.m_glyph_start = last->first;
		last->first += len;
		last->second -= len;

		GLint a = ptrProgram->attribute_location("in_Position");
		m_ptrVAO->attribute(a,m_ptrVertices,2,GL_FLOAT,false,sizeof(vertex_data),offsetof(vertex_data,x));
		m_ptrVAO->enable_attribute(a);

		a = ptrProgram->attribute_location("in_TexCoord");
		if (a != -1)
		{
			m_ptrVAO->attribute(a,m_ptrVertices,2,GL_UNSIGNED_SHORT,true,sizeof(vertex_data),offsetof(vertex_data,u));
			m_ptrVAO->enable_attribute(a);
		}

		m_ptrVAO->element_array(m_ptrElements);

		m_ptrVAO->unbind();
	}

	OOBase::SharedPtr<vertex_data> attribs = m_ptrVertices->auto_map<vertex_data>(GL_MAP_WRITE_BIT,text.m_glyph_start * vertices_per_glyph * sizeof(vertex_data),len * vertices_per_glyph * sizeof(vertex_data));
	vertex_data* a = attribs.get();
	OOBase::SharedPtr<GLuint> ei = m_ptrElements->auto_map<GLuint>(GL_MAP_WRITE_BIT,text.m_glyph_start * elements_per_glyph * sizeof(GLuint),len * elements_per_glyph * sizeof(GLuint));
	GLuint* e = ei.get();
	GLuint idx = text.m_glyph_start * vertices_per_glyph;

	OOBase::uint32_t prev_glyph = OOBase::uint32_t(-1);
	const OOBase::uint32_t* start = glyphs.data();
	const OOBase::uint32_t* end = start + len;

	text.m_length = 0.0f;
	text.m_glyph_len = len;

	while (start < end)
	{
		OOBase::uint32_t glyph = *start++;
		if (glyph > 31 && glyph != 127 && (glyph < 128 || glyph > 159))
		{
			if (prev_glyph != OOBase::uint32_t(-1))
			{
				kern_map_t::iterator k = m_mapKerning.find(OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t>(prev_glyph,glyph));
				if (k)
					text.m_length += k->second;
			}
			prev_glyph = glyph;

			char_map_t::iterator i = m_mapCharInfo.find(glyph);
			if (!i)
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

			a += vertices_per_glyph;
			e += elements_per_glyph;
			idx += vertices_per_glyph;

			text.m_length += i->second.xadvance;
		}
		else
		{
			prev_glyph = OOBase::uint32_t(-1);
		}
	}
	
	return true;
}

void Indigo::Render::Font::free_text(Text& text)
{
	free_list_t::iterator i = m_listFree.insert(text.m_glyph_start,text.m_glyph_len);
	if (i)
	{
		while (i != m_listFree.begin())
		{
			free_list_t::iterator j = i+1;
			if (j && j->first == i->first + i->second)
			{
				// Merge i with j
				i->second += j->second;
				m_listFree.erase(j);
			}
			else
			{
				--i;
			}
		}
	}
}

void Indigo::Render::Font::draw(OOGL::State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start, GLsizei len)
{
	if (len && m_packing)
	{
		OOBase::SharedPtr<OOGL::Program> ptrProgram = OOGL::ContextSingleton<FontProgram>::instance().program(m_packing);
	
		state.use(ptrProgram);
		state.bind(0,m_ptrTexture);

		ptrProgram->uniform("in_Colour",colour);
		ptrProgram->uniform("MVP",mvp);

		m_ptrVAO->draw_elements(GL_TRIANGLES,elements_per_glyph * len,GL_UNSIGNED_INT,start * elements_per_glyph * sizeof(GLuint));
	}
}

Indigo::Render::Text::Text(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len) :
		m_font(font), m_glyph_start(0), m_glyph_len(0), m_length(0.0f)
{
	if (len == size_t(-1))
		len = (sz ? strlen(sz) : 0);

	m_font->alloc_text(*this,sz,len);
}

Indigo::Render::Text::~Text()
{
	m_font->free_text(*this);
}

bool Indigo::Render::Text::text(const char* sz, size_t len)
{
	if (len == size_t(-1))
		len = (sz ? strlen(sz) : 0);

	bool ret = true;
	m_font->free_text(*this);
	
	m_glyph_start = 0;
	m_glyph_len = 0;

	if (len)
		ret = m_font->alloc_text(*this,sz,len);
			
	return ret;
}

const OOBase::SharedPtr<Indigo::Render::Font>& Indigo::Render::Text::font() const
{
	return m_font;
}

float Indigo::Render::Text::length() const
{
	return m_length;
}

void Indigo::Render::Text::draw(OOGL::State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start, GLsizei length)
{
	if (start > m_glyph_len)
		start = m_glyph_len;

	if (length == GLsizei(-1))
		length = m_glyph_len - start;

	if (start + length > m_glyph_len)
		length = m_glyph_len - start;

	m_font->draw(state,mvp,colour,m_glyph_start + start,length);
}

Indigo::Font::Font(const OOBase::SharedPtr<OOGL::Window>& wnd) : m_wnd(wnd)
{
	assert(wnd && wnd->valid());
}

Indigo::Font::~Font()
{
	destroy();
}

bool Indigo::Font::load(const OOGL::ResourceBundle& resource, const char* name)
{
	if (m_font)
		return false;

	bool ret = false;
	return render_pipe()->call(OOBase::make_delegate(this,&Indigo::Font::do_load),&ret,&resource,name) && ret;
}

void Indigo::Font::do_load(bool* ret_val, const OOGL::ResourceBundle* resource, const char* name)
{
	OOBase::SharedPtr<Indigo::Render::Font> font = OOBase::allocate_shared<Indigo::Render::Font>();
	if (!font)
	{
		LOG_ERROR(("Failed to allocate render font: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));
		*ret_val = false;
	}
	else
	{
		m_wnd->make_current();

		*ret_val = font->load(*resource,name);
	}

	if (*ret_val)
		font.swap(m_font);
}

bool Indigo::Font::destroy()
{
	return !m_font || render_pipe()->call(OOBase::make_delegate(this,&Indigo::Font::do_destroy));
}

void Indigo::Font::do_destroy()
{
	m_wnd->make_current();
	m_font.reset();
}

const OOBase::SharedPtr<Indigo::Render::Font>& Indigo::Font::render_font() const
{
	return m_font;
}
