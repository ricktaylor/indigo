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

#ifndef INDIGO_GUILABEL_H_INCLUDED
#define INDIGO_GUILABEL_H_INCLUDED

#include "GUIWidget.h"

namespace Indigo
{
	namespace GUI
	{
		class Label : public Widget
		{
		public:
			Label();

			bool create(Widget* parent, const OOBase::String& text = OOBase::String(), const glm::u16vec2& min_size = glm::u16vec2(-1), const glm::i16vec2& pos = glm::i16vec2(0));
			bool create(Widget* parent, const OOBase::SharedPtr<Style>& style, const OOBase::String& text = OOBase::String(), const glm::u16vec2& min_size = glm::u16vec2(-1), const glm::i16vec2& pos = glm::i16vec2(0));

			const OOBase::String& text() const;
			bool text(const OOBase::String& text);

		private:
			OOBase::String m_text;

			bool common_create(const OOBase::String& text);

			OOBase::SharedPtr<Render::GUI::Widget> create_render_widget();
			void set_text(bool* ret_val, const OOBase::String* text);
		};
	}
}

#endif // INDIGO_GUILABEL_H_INCLUDED
