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
#include "GUISizer.h"
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
			class Panel : public Widget
			{
				friend class Indigo::GUI::Panel;

			public:
				Panel();

				void layout();

				virtual glm::u16vec2 size(const glm::u16vec2& sz);

				virtual glm::u16vec2 min_size(const glm::u16vec2& sz);

				virtual glm::u16vec2 client_size() const;
				virtual glm::u16vec2 client_size(const glm::u16vec2& sz);

				virtual glm::u16vec2 ideal_size() const;

			protected:
				virtual bool add_child(const OOBase::SharedPtr<Widget>& child);
				virtual void remove_child(const OOBase::SharedPtr<Widget>& child);

				virtual void draw(OOGL::State& glState, const glm::mat4& mvp);

			private:
				OOBase::SharedPtr<Sizer> m_sizer;
				unsigned int m_style_flags;
				NinePatch m_background;
				OOBase::Vector<OOBase::SharedPtr<Widget>,OOBase::ThreadLocalAllocator> m_children;

				bool set_style_flags(unsigned int flags, bool refresh);
			};
		}
	}

	namespace GUI
	{
		class Panel : public Widget
		{
		public:
			enum PanelStyleFlags
			{
				no_border = 0,
				show_border = 1,
				colour_border = 2
			};

			bool create(Widget* parent, unsigned int style_flags = show_border, const glm::u16vec2& min_size = glm::u16vec2(-1), const glm::i16vec2& pos = glm::i16vec2(0));
			bool create(Widget* parent, const OOBase::SharedPtr<Style>& style, unsigned int style_flags = show_border, const glm::u16vec2& min_size = glm::u16vec2(-1), const glm::i16vec2& pos = glm::i16vec2(0));

			const OOBase::SharedPtr<Sizer>& sizer() const;
			bool sizer(const OOBase::SharedPtr<Sizer>& s);

			unsigned int style_flags() const;
			bool style_flags(unsigned int flags);

			bool fit();
			bool layout();

		private:
			OOBase::SharedPtr<Sizer> m_sizer;

			bool common_create(unsigned int style_flags);

			OOBase::SharedPtr<Render::GUI::Widget> create_render_widget();

			void do_sizer(bool* ret_val, Sizer* s);
			void set_style_flags(bool* ret_val, unsigned int flags);
		};
	}
}

#endif // INDIGO_GUIPANEL_H_INCLUDED
