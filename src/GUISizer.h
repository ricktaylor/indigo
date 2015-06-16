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

#ifndef INDIGO_GUISIZER_H_INCLUDED
#define INDIGO_GUISIZER_H_INCLUDED

#include "GUIWidget.h"

namespace Indigo
{
	namespace Render
	{
		namespace GUI
		{
			class Sizer;
		}
	}

	namespace GUI
	{
		class Panel;

		class Sizer : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Sizer>
		{
			friend class Panel;

		public:
			Sizer();
			virtual ~Sizer();

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

			bool destroy();

			bool fit(Panel& panel);
			bool layout(const Panel& panel);

		protected:
			bool create(OOBase::uint16_t hspace, OOBase::uint16_t vspace);
			bool create(OOBase::uint16_t hspace, OOBase::uint16_t vspace, const ItemLayout& default_layout);

			template <typename T>
			OOBase::SharedPtr<T> render_sizer() const
			{
				return OOBase::static_pointer_cast<T>(m_sizer);
			}

			inline static OOBase::SharedPtr<Indigo::Render::GUI::Widget> render_widget(const OOBase::SharedPtr<Widget>& widget)
			{
				return widget->m_render_widget;
			}

			virtual OOBase::SharedPtr<Indigo::Render::GUI::Sizer> create_render_sizer() = 0;

			const ItemLayout* default_layout() const { return &m_default_layout; }

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
	}

	namespace Render
	{
		namespace GUI
		{
			class Panel;

			class Sizer : public OOBase::NonCopyable
			{
				friend class Indigo::GUI::Sizer;

			public:
				Sizer();
				virtual ~Sizer();

				virtual bool fit(Panel& panel) = 0;
				virtual bool layout(const Panel& panel) = 0;

				const glm::u16vec2& padding() const;
				void padding(const glm::u16vec2& p);

				virtual glm::u16vec2 ideal_size() const  = 0;

			protected:
				glm::u16vec2 m_padding;
			};
		}
	}
}

#endif // INDIGO_GUISIZER_H_INCLUDED
