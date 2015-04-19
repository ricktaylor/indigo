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

#ifndef INDIGO_FONT_H_INCLUDED
#define INDIGO_FONT_H_INCLUDED

#include "../lib/State.h"

#include <stdarg.h>

namespace OOGL
{
	class Font : public OOBase::NonCopyable
	{
		friend class Text;

	public:
		Font();
		~Font();

		bool load(const unsigned char* data, size_t len, ...);

	private:
		OOBase::uint16_t m_size;
		OOBase::uint16_t m_line_height;
		OOBase::uint16_t m_tex_width;
		OOBase::uint16_t m_tex_height;
		OOBase::uint16_t m_pages;

		struct char_info
		{
			OOBase::uint16_t x;
			OOBase::uint16_t y;
			OOBase::uint16_t width;
			OOBase::uint16_t height;
			OOBase::uint16_t xoffset;
			OOBase::uint16_t yoffset;
			OOBase::uint16_t xadvance;
			OOBase::uint8_t page;
			OOBase::uint8_t chnl;
		};
		typedef OOBase::Table<OOBase::uint32_t,struct char_info,OOBase::Less<OOBase::uint32_t>,OOBase::ThreadLocalAllocator> char_map_t;
		char_map_t m_mapCharInfo;

		typedef OOBase::Table<OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t>,OOBase::int16_t,OOBase::Less<OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t> >,OOBase::ThreadLocalAllocator> kern_map_t;
		kern_map_t m_mapKerning;

		OOBase::SharedPtr<OOGL::Texture> m_ptrTexture;
		OOBase::SharedPtr<OOGL::Program> m_ptrProgram;
		OOBase::SharedPtr<OOGL::VertexArrayObject> m_ptrVAO;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements;

		bool load_8bit_shader();
		bool prep_text(const char* sz, size_t len);

		void draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, size_t start, size_t len);
	};

	class Text : public OOBase::NonCopyable
	{
	public:
		Text(const OOBase::SharedPtr<Font>& font, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s);
		~Text();

		void text(const OOBase::SharedString<OOBase::ThreadLocalAllocator>& s);
		OOBase::SharedString<OOBase::ThreadLocalAllocator> text() const;

		void draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, size_t start = 0, size_t end = size_t(-1));

	private:
		OOBase::SharedPtr<Font> m_font;
		OOBase::SharedString<OOBase::ThreadLocalAllocator> m_str;
	};
}

#endif // INDIGO_FONT_H_INCLUDED
