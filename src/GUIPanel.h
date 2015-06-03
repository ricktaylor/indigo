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

#ifndef INDIGO_GUIPANEL_H_INCLUDED
#define INDIGO_GUIPANEL_H_INCLUDED

#include "GUIWidget.h"
#include "Image.h"
#include "NinePatch.h"

namespace Indigo
{
	namespace GUI
	{
		class Panel;
	}

	namespace Render
	{
		namespace GUI
		{
			class Sizer;

			class Panel : public Widget
			{
				friend class Indigo::GUI::Panel;

			public:
				void layout();

				const glm::u16vec4& borders() const;
				void borders(const glm::u16vec4& b);

				const OOBase::SharedPtr<OOGL::Texture>& texture() const;
				bool texture(const OOBase::SharedPtr<OOGL::Texture>& tex, const glm::u16vec2& tex_size);

				virtual glm::u16vec2 size() const { return Widget::size(); }
				virtual glm::u16vec2 size(const glm::u16vec2& sz);

				virtual glm::u16vec2 client_size() const;
				virtual glm::u16vec2 client_size(const glm::u16vec2& sz);

				virtual glm::u16vec2 ideal_size() const;

			protected:
				virtual bool add_child(const OOBase::SharedPtr<Widget>& child);
				virtual void remove_child(const OOBase::SharedPtr<Widget>& child);

				virtual void draw(OOGL::State& glState, const glm::mat4& mvp);

			private:
				OOBase::SharedPtr<Sizer> m_sizer;
				NinePatch m_background;
				OOBase::SharedPtr<OOGL::Texture> m_texture;
				glm::u16vec2 m_tex_size;
				glm::u16vec4 m_borders;
				OOBase::Vector<OOBase::SharedPtr<Widget>,OOBase::ThreadLocalAllocator> m_children;

				void layout_background(const glm::u16vec2& sz);
			};
		}
	}

	namespace GUI
	{
		class Sizer : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Sizer>
		{
			friend class Panel;

		public:
			Sizer();
			~Sizer();

			struct ItemLayout
			{
				enum eLayoutFlags
				{
					align_left = 0,
					align_right = 1,
					align_hcentre = 2,
					align_bottom = 0,
					align_top = 1 << 2,
					align_vcentre = 2 << 2,
					align_centre = align_hcentre | align_vcentre,
					expand_horiz = 1 << 4,
					expand_vert = 2 << 4,
					expand = expand_horiz | expand_vert
				};
				OOBase::uint16_t m_flags;
				OOBase::uint16_t m_proportion;
			};

			bool create(OOBase::uint16_t hspace, OOBase::uint16_t vspace);
			bool create(OOBase::uint16_t hspace, OOBase::uint16_t vspace, const ItemLayout& default_layout);
			bool destroy();

			bool fit(Panel& panel);
			bool layout(const Panel& panel);

			bool add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column = 0, const ItemLayout* layout = NULL);
			bool add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column = 0, const ItemLayout* layout = NULL);

			bool remove_item(unsigned int row, unsigned int column);
			bool remove_widget(const OOBase::SharedPtr<Widget>& widget);

		private:
			OOBase::SharedPtr<Indigo::Render::GUI::Sizer> m_sizer;
			ItemLayout m_default_layout;

			void do_create(bool* ret_val, const glm::u16vec2* space);
			void do_destroy();
			void do_fit(bool* ret_val, Widget* panel);
			void do_layout(bool* ret_val, const Widget* panel);
			void do_add_widget(bool* ret_val, OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget, unsigned int row, unsigned int column, const ItemLayout* layout);
			void do_add_spacer(bool* ret_val, OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>* pos, OOBase::Pair<unsigned int,unsigned int>* idx, const ItemLayout* layout);
			void do_remove_widget(bool* ret_val, Indigo::Render::GUI::Widget* widget);
			void do_remove_item(bool* ret_val, unsigned int row, unsigned int column);
		};

		class Panel : public Widget
		{
		public:
			bool create(Widget* parent, const glm::u16vec2& min_size = glm::u16vec2(-1), const glm::i16vec2& pos = glm::i16vec2(0));

			const OOBase::SharedPtr<Sizer>& sizer() const;
			bool sizer(const OOBase::SharedPtr<Sizer>& s);

			bool fit();
			bool layout();

			const OOBase::SharedPtr<Indigo::Image>& background() const;
			bool background(const OOBase::SharedPtr<Indigo::Image>& image);
			bool background(const OOGL::ResourceBundle& resource, const char* name);

			glm::u16vec4 borders() const;
			bool borders(OOBase::uint16_t left, OOBase::uint16_t top, OOBase::uint16_t right, OOBase::uint16_t bottom);

		private:
			OOBase::SharedPtr<Sizer> m_sizer;
			OOBase::SharedPtr<Indigo::Image> m_background;

			void do_sizer(Sizer* s, bool* ret_val);
			OOBase::SharedPtr<Render::GUI::Widget> create_widget();
			void do_background(Image* image, bool* ret_val);
			void do_set_borders(glm::u16vec4* borders, bool* ret_val);
			void do_get_borders(glm::u16vec4* borders);
		};
	}
}

#endif // INDIGO_GUIPANEL_H_INCLUDED
