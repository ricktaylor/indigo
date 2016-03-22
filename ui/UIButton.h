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

#include "Label.h"
#include "NinePatch.h"

namespace Indigo
{
	class UIButton : public UIWidget
	{
	public:
		struct StyleState
		{
			OOBase::SharedPtr<Font> m_font;
			OOBase::SharedPtr<NinePatch> m_background;
			glm::vec4 m_colour;
			glm::vec4 m_shadow;
			glm::ivec2 m_drop;

			void unload();
		};

		struct Style
		{
			StyleState m_normal;
			StyleState m_active;
			StyleState m_pressed;

			void unload();
		};

		UIButton(UIGroup* parent, const OOBase::SharedPtr<Style>& style, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& caption, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0));
		UIButton(UIGroup* parent, const OOBase::SharedPtr<Style>& style, const char* sz, size_t len = -1, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0));

	protected:
		virtual bool valid() const;

		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Indigo::Render::UIGroup* group);
		virtual void on_size(const glm::uvec2& sz);

		virtual bool can_enable(bool enabled) { return true; }
		virtual bool can_focus(bool focused) { return true; }
		virtual bool can_hilight(bool hilighted) { return true; }

	private:
		OOBase::SharedString<OOBase::ThreadLocalAllocator> m_text;
		OOBase::SharedPtr<Style> m_style;

		struct RenderStyleState
		{
			OOBase::SharedPtr<Render::NinePatch> m_background;
			OOBase::SharedPtr<Render::ShadowLabel> m_caption;
		};
		RenderStyleState m_normal;
		RenderStyleState m_active;
		RenderStyleState m_pressed;
		RenderStyleState* m_current_style;

		glm::uvec2 min_style_size(const StyleState& style) const;
		glm::uvec2 ideal_style_size(const StyleState& style) const;
		void do_size(glm::uvec2 sz);
		bool style_create(Indigo::Render::UIGroup* group, const StyleState& style, RenderStyleState& rs);
		void do_style_size(const glm::uvec2& sz, const StyleState& style, RenderStyleState& rs);
	};
}

#endif /* INDIGO_UIBUTTON_H_INCLUDED */
