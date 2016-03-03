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

#ifndef INDIGO_FONT_H_INCLUDED
#define INDIGO_FONT_H_INCLUDED

#include "../oogl/Window.h"
#include "../oogl/BufferObject.h"
#include "../oogl/Texture.h"
#include "../oogl/VertexArrayObject.h"

#include "Resource.h"

#include "../ui/UILayer.h"

namespace Indigo
{
	class Image;

	namespace Render
	{
		class Font;
	}

	class Font : public OOBase::NonCopyable
	{
		friend class Render::Font;

	public:
		Font();
		~Font();

		bool valid() const { return m_render_font; }
		
		bool load(const ResourceBundle& resource, const char* name);
		void unload();

		float measure_text(const char* sz, size_t len = -1);

		OOBase::uint16_t line_height() const { return m_info->m_line_height; }
		OOBase::uint16_t base_height() const { return m_info->m_base_height; }

		const OOBase::SharedPtr<Render::Font>& render_font() const;

	private:
		struct Info
		{
			OOBase::uint16_t m_line_height;
			OOBase::uint16_t m_base_height;
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
			typedef OOBase::HashTable<OOBase::uint32_t,struct char_info> char_map_t;
			char_map_t m_mapCharInfo;

			typedef OOBase::HashTable<OOBase::Pair<OOBase::uint32_t,OOBase::uint32_t>,float> kern_map_t;
			kern_map_t m_mapKerning;
		};

		OOBase::SharedPtr<Indigo::Font::Info> m_info;
		OOBase::SharedPtr<Render::Font> m_render_font;

		void do_load(OOBase::SharedPtr<Indigo::Image>* pages, size_t page_count, bool* ret_val);
		void do_unload();
	};

	namespace Render
	{
		class Text;

		class Font : public OOBase::NonCopyable
		{
			friend class Indigo::Font;
			friend class Text;

		public:
			Font(const OOBase::SharedPtr<Indigo::Font::Info>& info);
			~Font();

			OOBase::uint16_t line_height() const { return m_info->m_line_height; }
			OOBase::uint16_t base_height() const { return m_info->m_base_height; }

		private:
			typedef OOBase::Table<GLsizei,GLsizei,OOBase::Less<GLsizei>,OOBase::ThreadLocalAllocator> free_list_t;
			free_list_t m_listFree;
			GLsizei m_allocated;

			OOBase::SharedPtr<OOGL::Texture> m_ptrTexture;
			OOBase::SharedPtr<OOGL::VertexArrayObject> m_ptrVAO;
			OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices;
			OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements;
			OOBase::SharedPtr<OOGL::Program> m_ptrProgram;

			OOBase::SharedPtr<Indigo::Font::Info> m_info;

			bool load(const OOBase::SharedPtr<Indigo::Image>* pages, size_t page_count);

			bool alloc_text(Text& text, const char* sz, size_t len);
			void free_text(Text& text);

			bool font_program(OOBase::uint32_t packing);

			void draw(OOGL::State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start, GLsizei len);
		};

		class Text : public OOBase::NonCopyable
		{
			friend class Font;

		public:
			template <typename Allocator>
			Text(const OOBase::SharedPtr<Font>& font, const OOBase::SharedString<Allocator>& text) :
					m_font(font), m_glyph_start(0), m_glyph_len(0)
			{
				m_font->alloc_text(*this,text.c_str(),text.length());
			}

			Text(const OOBase::SharedPtr<Font>& font, const char* sz = NULL, size_t len = -1);
			~Text();

			const OOBase::SharedPtr<Font>& font() const;

			void text(const char* sz, size_t len = -1);

			void draw(OOGL::State& state, const glm::mat4& mvp, const glm::vec4& colour, GLsizei start = 0, GLsizei length = -1) const;

		private:
			OOBase::SharedPtr<Font> m_font;
			GLsizei m_glyph_start;
			GLsizei m_glyph_len;
		};
	}
}

#endif // INDIGO_FONT_H_INCLUDED
