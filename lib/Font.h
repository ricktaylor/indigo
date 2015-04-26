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

#include "State.h"
#include "Resource.h"

#include <OOBase/String.h>
#include <OOBase/List.h>

namespace OOGL
{
	class Text;

	class Font : public OOBase::NonCopyable
	{
		friend class Text;

	public:
		Font();
		~Font();

		bool load(ResourceBundle& resource, const char* name);

	private:
		OOBase::uint16_t m_size;
		float m_line_height;
		OOBase::uint32_t m_packing;

		struct char_info
		{
			OOBase::uint16_t u0;
			OOBase::uint16_t v0;
			OOBase::uint16_t u1;
			OOBase::uint16_t v1;
			float left;
			float top;
			float right;
			float bottom;
			float xadvance;
			OOBase::uint8_t page;
			OOBase::uint8_t channel;
		};
		typedef OOBase::Table<OOBase::uint32_t,struct char_info,OOBase::Less<OOBase::uint32_t>,OOBase::ThreadLocalAllocator> char_map_t;
		char_map_t m_mapCharInfo;

		typedef OOBase::Table<OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t>,float,OOBase::Less<OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t> >,OOBase::ThreadLocalAllocator> kern_map_t;
		kern_map_t m_mapKerning;

		typedef OOBase::Table<GLsizei,GLsizei,OOBase::Less<GLsizei>,OOBase::ThreadLocalAllocator> free_list_t;
		free_list_t m_listFree;
		GLsizei m_allocated;

		OOBase::SharedPtr<OOGL::Texture> m_ptrTexture;
		OOBase::SharedPtr<OOGL::VertexArrayObject> m_ptrVAO;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements;

		bool alloc_text(Text& text, const char* sz, size_t len);
		void free_text(Text& text);

		void draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start, GLsizei len);
	};

	class Text : public OOBase::NonCopyable
	{
		friend class Font;

	public:
		Text(const OOBase::SharedPtr<Font>& font, const char* sz = NULL, size_t len = size_t(-1));
		~Text();

		const OOBase::SharedPtr<Font>& font() const;
		bool text(const char* sz, size_t len = size_t(-1));
		
		float length() const;

		void draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start = 0, GLsizei length = GLsizei(-1));

	private:
		OOBase::SharedPtr<Font> m_font;
		GLsizei m_glyph_start;
		GLsizei m_glyph_len;
		float m_length;
	};
}

#endif // INDIGO_FONT_H_INCLUDED
