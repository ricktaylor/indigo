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

#ifndef INDIGO_Label_H_INCLUDED
#define INDIGO_Label_H_INCLUDED

#include "UIWidget.h"

#include "../Font.h"

namespace Indigo
{
	namespace Render
	{
		class UILabel : public Text, public UIDrawable
		{
		public:
			UILabel(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len = -1, unsigned int font_size = 0, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), bool visible = true, const glm::ivec2& position = glm::ivec2(), const glm::uvec2& size = glm::uvec2());

		protected:
			glm::vec4 m_colour;
			float m_font_size;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};

		class UIShadowLabel : public UILabel
		{
		public:
			UIShadowLabel(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len = -1, unsigned int font_size = 0, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), const glm::vec4& shadow = glm::vec4(.5f), const glm::ivec2& drop = glm::ivec2(0,-1), bool visible = true, const glm::ivec2& position = glm::ivec2());

		private:
			glm::vec4 m_shadow;
			glm::ivec2 m_drop;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}

	class UILabel : public UIWidget
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

		struct CreateParams : public UIWidget::CreateParams
		{
			CreateParams(OOBase::uint32_t state = eWS_visible,
					const glm::ivec2& position = glm::ivec2(),
					const glm::uvec2& size = glm::uvec2(),
					const OOBase::SharedPtr<Font>& font = OOBase::SharedPtr<Font>(),
					unsigned int font_size = 0,
					unsigned int style = (align_left | align_vcentre),
					const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f)
			) :
				UIWidget::CreateParams(state,position,size),
				m_font(font),
				m_font_size(font_size),
				m_style(style),
				m_colour(colour)
			{}

			OOBase::SharedPtr<Font> m_font;
			unsigned int            m_font_size;
			unsigned int            m_style;
			glm::vec4               m_colour;
		};

		UILabel(UIGroup* parent, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& caption, const CreateParams& params = CreateParams());
		UILabel(UIGroup* parent, const char* sz, size_t len = -1, const CreateParams& params = CreateParams());

	protected:
		virtual glm::uvec2 min_size() const { return glm::uvec2(); }
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Render::UIGroup* group);

	private:
		OOBase::SharedString<OOBase::ThreadLocalAllocator> m_text;
		OOBase::SharedPtr<Font> m_font;
		unsigned int m_font_size;
		unsigned int m_style;
		glm::vec4 m_colour;
		Render::UIDrawable* m_caption;

		void on_size(glm::uvec2& sz);
	};
}

#endif // INDIGO_Label_H_INCLUDED
