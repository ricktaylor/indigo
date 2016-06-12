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

#include "UILabel.h"
#include "UINinePatch.h"

namespace Indigo
{
	class UIButton : public UIWidget
	{
	public:
		enum eStyleFlags
		{
			align_left = 0,
			align_right = 1,
			align_hcentre = 2,
			align_bottom = 0,
			align_top = 1 << 2,
			align_vcentre = 2 << 2,
			align_centre = align_hcentre | align_vcentre
		};

		struct StyleState
		{
			OOBase::SharedPtr<Font> m_font;
			unsigned int m_style_flags;
			unsigned int m_font_size;
			OOBase::SharedPtr<NinePatch> m_background;
			glm::vec4 m_background_colour;
			glm::vec4 m_text_colour;
			glm::vec4 m_shadow;
			glm::ivec2 m_drop;

			StyleState() :
				m_style_flags(UIButton::align_centre),
				m_font_size(0)
			{}

			void unload();
		};

		struct Style
		{
			StyleState m_normal;
			StyleState m_active;
			StyleState m_pressed;
			StyleState m_disabled;

			void unload();
		};

		enum State
		{
			eBS_pressed = 0x4,
			eBS_cursorover = 0x8
		};

		struct CreateParams : public UIWidget::CreateParams
		{
			CreateParams(OOBase::uint32_t state = eWS_enabled | eWS_visible,
					const glm::ivec2& position = glm::ivec2(),
					const glm::uvec2& size = glm::uvec2(),
					const OOBase::SharedPtr<Style>& style = OOBase::SharedPtr<Style>()
			) :
				UIWidget::CreateParams(state,position,size),
				m_style(style)
			{}

			OOBase::SharedPtr<Style> m_style;
		};

		UIButton(UIGroup* parent, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& caption, const CreateParams& params = CreateParams());
		UIButton(UIGroup* parent, const char* sz, size_t len = -1, const CreateParams& params = CreateParams());

		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> on_click(const OOBase::Delegate0<void,OOBase::ThreadLocalAllocator>& delegate);

	protected:
		virtual glm::uvec2 min_size() const { return m_min_size; }
		virtual glm::uvec2 ideal_size() const { return m_ideal_size; }

		virtual bool on_render_create(Render::UIGroup* group);
		virtual void on_size(glm::uvec2& sz);
		virtual void on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask);
		virtual void on_cursorenter(bool enter);
		virtual bool on_mousebutton(const OOGL::Window::mouse_click_t& click);

	private:
		OOBase::SharedString<OOBase::ThreadLocalAllocator> m_text;
		OOBase::SharedPtr<Style> m_style;
		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> m_on_click;

		struct RenderStyleState
		{
			RenderStyleState() : m_background(NULL), m_caption(NULL)
			{}

			Render::UIDrawable* m_background;
			Render::UIDrawable* m_caption;
		};
		RenderStyleState m_normal;
		RenderStyleState m_active;
		RenderStyleState m_pressed;
		RenderStyleState m_disabled;
		RenderStyleState* m_current_style;

		glm::uvec2 m_min_size;
		glm::uvec2 m_ideal_size;

		void update_sizes();
		glm::uvec2 min_style_size(const StyleState& style) const;
		glm::uvec2 ideal_style_size(const StyleState& style) const;
		void do_size(const glm::uvec2& sz);
		bool style_create(Render::UIGroup* group, StyleState& style, RenderStyleState& rs, bool visible);
		void do_style_size(const glm::uvec2& sz, const StyleState& style, RenderStyleState& rs);
		void do_style_change(RenderStyleState* new_style);
	};
}

#endif /* INDIGO_UIBUTTON_H_INCLUDED */
