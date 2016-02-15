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

#ifndef INDIGO_GUISTYLE_H_INCLUDED
#define INDIGO_GUISTYLE_H_INCLUDED

#include "Font.h"
#include "Image.h"

#include "../lib/Window.h"

namespace Indigo
{
	namespace GUI
	{
		class Widget;
		class Style;
	}

	namespace Render
	{
		namespace GUI
		{
			class Style : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Style>
			{
				friend class Indigo::GUI::Style;

			public:
				Style();

				const OOBase::SharedPtr<Font>& font() const { return m_font; }
				const glm::vec4& foreground_colour() const { return m_foreground_colour; }
				const glm::vec4& border_colour() const { return m_border_colour; }
				const OOBase::SharedPtr<OOGL::Texture>& border_image() const { return m_border_image; }
				const glm::u16vec2& border_image_size() const { return m_border_image_size; }
				const glm::u16vec4& borders() const { return m_borders; }

			private:
				OOBase::SharedPtr<Font> m_font;
				glm::vec4 m_foreground_colour;
				glm::vec4 m_border_colour;
				OOBase::SharedPtr<OOGL::Texture> m_border_image;
				glm::u16vec2 m_border_image_size;
				glm::u16vec4 m_borders;
			};
		}
	}

	namespace GUI
	{
		class Style : public OOBase::NonCopyable
		{
			friend class Widget;

		public:
			Style();
			~Style();

			bool create(const OOBase::SharedPtr<OOGL::Window>& wnd);
			bool clone(const OOBase::SharedPtr<Style>& orig);
			bool destroy();

			const OOBase::SharedPtr<Font>& font() const { return m_font; }
			bool font(const OOBase::SharedPtr<Font>& font);
			bool font(const OOGL::ResourceBundle& resource, const char* name);

			const glm::u16vec4& borders() const { return m_borders; }
			bool borders(OOBase::uint16_t left, OOBase::uint16_t top, OOBase::uint16_t right, OOBase::uint16_t bottom);
			bool borders(const glm::u16vec4& b);

			const OOBase::SharedPtr<Indigo::Image>& border_image() const;
			bool border_image(const OOBase::SharedPtr<Indigo::Image>& image);
			bool border_image(const OOGL::ResourceBundle& resource, const char* name);

			const glm::u8vec4& foreground_colour() const { return m_foreground_colour; }
			bool foreground_colour(const glm::u8vec4& col);

			const glm::u8vec4& border_colour() const { return m_border_colour; }
			bool border_colour(const glm::u8vec4& col);

		private:
			OOBase::SharedPtr<OOGL::Window> m_wnd;
			OOBase::SharedPtr<Render::GUI::Style> m_render_style;
			OOBase::SharedPtr<Font> m_font;
			glm::u8vec4 m_foreground_colour;
			glm::u8vec4 m_border_colour;
			glm::u16vec4 m_borders;
			OOBase::SharedPtr<Indigo::Image> m_border_image;

			void do_create(bool* ret_val);
			void do_destroy();
			void do_clone(bool* ret_val, Render::GUI::Style* orig);

			void set_font(bool* ret_val, Font* font);
			void set_foreground_colour(bool* ret_val, glm::vec4* c);
			void set_border_colour(bool* ret_val, glm::vec4* c);
			void set_borders(bool* ret_val, const glm::u16vec4* borders);
			void set_border_image(bool* ret_val, Image* image);
		};
	}
}

#endif // INDIGO_GUISTYLE_H_INCLUDED
