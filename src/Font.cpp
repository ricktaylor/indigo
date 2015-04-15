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
			{
				unsigned int pages = read_uint16(data);
				if (pages > 1 && !StateFns::get_current()->check_glTextureArray())
				{
					LOG_ERROR(("Multiple textures in font, no texture array support"));
					ok = false;
				}
				for (unsigned int p=0;ok && p<pages;++p)
				{
					// Load page
					const unsigned char* page_data = va_arg(textures,const unsigned char*);
					size_t page_len = va_arg(textures,size_t);
				}
			}
			data += 5;
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
				ok = m_mapCharInfo.insert(id,ci);
				if (!ok)
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
				ok = m_mapKerning.insert(ch,offset);
				if (!ok)
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

OOGL::Text::Text(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len) :
		m_font(font)
{

}

void OOGL::Text::draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, size_t start, size_t end)
{


}
