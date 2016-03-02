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

#ifndef INDIGO_UIBUTTON_H_INCLUDED
#define INDIGO_UIBUTTON_H_INCLUDED

#include "UIText.h"
#include "UI9Patch.h"

namespace Indigo
{
	class UIButton : public UIWidget
	{
	public:
		class Style
		{
		public:
			Font m_font;
			UI9Patch m_background;
			glm::vec4 m_colour;
			glm::vec4 m_shadow;
			glm::uvec2 m_drop;
		};

		template <typename Allocator>
		UIButton(const OOBase::SharedPtr<Style>& style, const OOBase::SharedString<Allocator>& caption, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0)) :
				UIWidget(position,size),
				m_text(caption),
				m_style(style)
		{
			if (size == glm::uvec2(0))
				this->size(ideal_size());
		}

		UIButton(const OOBase::SharedPtr<Style>& style, const char* sz, size_t len = -1, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0));

	protected:
		virtual glm::uvec2 min_size() const { return glm::uvec2(0); }
		virtual glm::uvec2 max_size() const { return glm::uvec2(-1); }
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Indigo::Render::UIGroup* group);
		virtual void on_size(const glm::uvec2& sz) { }

		virtual bool can_enable(bool enabled) { return true; }
		virtual bool can_focus(bool focused) { return true; }
		virtual bool can_hilight(bool hilighted) { return true; }

	private:
		OOBase::SharedString<OOBase::ThreadLocalAllocator> m_text;
		OOBase::SharedPtr<Style> m_style;

		OOBase::SharedPtr<Render::UI9Patch> m_background;
		OOBase::SharedPtr<Render::UIShadowText> m_caption;

	};
}

#endif /* INDIGO_UIBUTTON_H_INCLUDED */
