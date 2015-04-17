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
#include "../lib/Shader.h"
#include "../lib/StateFns.h"
#include "../lib/Image.h"
#include "../lib/VertexArrayObject.h"
#include "../lib/BufferObject.h"

#include <OOBase/ByteSwap.h>

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

	bool ok = true;
	for (data += 4;ok && data < end;)
	{
		unsigned int type = *data++;
		len = read_uint32(data);

		switch (type)
		{
		case 1:
			OOBase::Logger::log(OOBase::Logger::Information,"Loading font: %s %u",data + 14,((unsigned int)data[1] << 8) | data[0]);
			data += len;
			break;

		case 2:
			assert(len == 15);
			m_line_height = read_uint16(data);
			data += 2;
			m_tex_width = read_uint16(data);
			m_tex_height = read_uint16(data);
			m_pages = read_uint16(data);
			if (!m_pages)
			{
				LOG_ERROR(("No textures in font!"));
				ok = false;
			}
			else if (m_pages == 1)
			{
				const unsigned char* page_data = va_arg(textures,const unsigned char*);
				size_t page_len = va_arg(textures,size_t);

				OOGL::Image img;
				if ((ok = img.load(page_data,page_len)))
					ok = (m_ptrTexture = img.make_texture(GL_R8));
			}
			else
			{
				if (!StateFns::get_current()->check_glTextureArray())
				{
					LOG_ERROR(("Multiple textures in font, no texture array support"));
					ok = false;
				}
				for (unsigned int p=0;ok && p<m_pages;++p)
				{
					// Load page
					const unsigned char* page_data = va_arg(textures,const unsigned char*);
					size_t page_len = va_arg(textures,size_t);

					OOGL::Image img;
					if ((ok = img.load(page_data,page_len)))
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
				OOBase::uint32_t packing = read_uint32(data);
				if (packing != 0x04040400)
				{
					// TODO: Funky packing
				}
				else
					ok = load_8bit_shader();
			}
			break;

		case 3:
			data += len;
			break;

		case 4:
			for (size_t c = 0;ok && c < len / 20; ++c)
			{
				struct char_info ci;
				OOBase::uint32_t id = read_uint32(data);
				ci.x = read_uint16(data);
				ci.y = read_uint16(data);
				ci.width = read_uint16(data);
				ci.height = read_uint16(data);
				ci.xoffset = read_uint16(data);
				ci.yoffset = read_uint16(data);
				ci.xadvance = read_uint16(data);
				ci.page = *data++;
				ci.chnl = *data++;
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
				OOBase::int16_t offset = read_uint16(data);
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

bool OOGL::Font::prep_text(const char* sz, size_t len)
{
	if (len)
	{
		if (!m_ptrVAO)
		{
			m_ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();
			if (!m_ptrVAO)
				LOG_ERROR_RETURN(("Failed to allocate VAO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

			m_ptrVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,len * 4 * 3 * sizeof(float));
			GLint a = m_ptrProgram->attribute_location("in_Position");
			m_ptrVAO->attribute(a,m_ptrVertices,3,GL_FLOAT,false);
			m_ptrVAO->enable_attribute(a);

			m_ptrTexCoords = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,len * 4 * 2 * sizeof(float));
			a = m_ptrProgram->attribute_location("in_TexCoord");
			if (a != -1)
			{
				m_ptrVAO->attribute(a,m_ptrTexCoords,2,GL_FLOAT,false);
				m_ptrVAO->enable_attribute(a);
			}

			m_ptrElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,len * 6 * sizeof(GLuint));
			m_ptrElements->bind();
		}
		else
		{
			// TODO:: Grow!!
		}

		OOBase::SharedPtr<float> vx = m_ptrVertices->auto_map<float>(GL_MAP_WRITE_BIT);
		float* v = vx.get();
		OOBase::SharedPtr<float> tx = m_ptrTexCoords->auto_map<float>(GL_MAP_WRITE_BIT);
		float* t = tx.get();
		OOBase::SharedPtr<GLuint> ei = m_ptrElements->auto_map<GLuint>(GL_MAP_WRITE_BIT);
		GLuint* e = ei.get();
		float advance = 0.f;
		GLuint idx = 0;
		for (size_t p=0;p<len;++p)
		{
			char_map_t::iterator i = m_mapCharInfo.find(sz[p]);

			t[0] = i->second.x / (float)m_tex_width;
			t[1] = i->second.y / (float)m_tex_height;
			t[2] = t[0];
			t[3] = t[1] + (i->second.height / (float)m_tex_height);
			t[4] = t[0] + (i->second.width / (float)m_tex_width);
			t[5] = t[1];
			t[6] = t[4];
			t[7] = t[3];
			t += 8;

			v[0] = advance + (i->second.xoffset / 64.f);
			v[1] = 1.f - (i->second.yoffset / 64.f);
			v[2] = 0.f;
			v[3] = v[0];
			v[4] = v[1] - (i->second.height / 64.f);
			v[5] = 0.f;
			v[6] = v[0] + (i->second.width / 64.f);
			v[7] = v[1];
			v[8] = 0.f;
			v[9] = v[6];
			v[10] = v[4];
			v[11] = 0.f;
			v += 12;

			e[0] = idx + 0;
			e[1] = idx + 1;
			e[2] = idx + 2;
			e[3] = idx + 2;
			e[4] = idx + 1;
			e[5] = idx + 3;
			e += 6;
			idx += 4;

			advance += (i->second.xadvance / 64.f);
		}
	}

	return true;
}

void OOGL::Font::draw(State& state, const glm::mat4& mvp, const glm::vec4& colour)
{
	m_ptrProgram->uniform("in_Colour",colour);
	m_ptrProgram->uniform("MVP",mvp);

	state.use(m_ptrProgram);

	state.bind(GL_TEXTURE0,m_ptrTexture);

	m_ptrVAO->draw_elements(GL_TRIANGLES,6 * 4,GL_UNSIGNED_INT);
}

OOGL::Text::Text(const OOBase::SharedPtr<Font>& font, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s) :
		m_font(font), m_str(s)
{
	m_font->prep_text(s.c_str(),s.length());
}

OOGL::Text::~Text()
{

}

void OOGL::Text::text(const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s)
{

}

OOBase::SharedString<OOBase::ThreadLocalAllocator> OOGL::Text::text() const
{
	return m_str;
}

void OOGL::Text::draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, size_t start, size_t end)
{
	m_font->draw(state,mvp,colour);
}
