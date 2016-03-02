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

#ifndef INDIGO_UITEXT_H_INCLUDED
#define INDIGO_UITEXT_H_INCLUDED

#include "../core/Font.h"

namespace Indigo
{
	namespace Render
	{
		class UIText : public UIDrawable
		{
		public:
			template <typename Allocator>
			UIText(const OOBase::SharedPtr<Font>& font, const OOBase::SharedString<Allocator>& text, float scale = 0.f, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), const glm::ivec2& position = glm::ivec2(0), bool visible = true) :
					UIDrawable(position,visible),
					m_text(font,text),
					m_colour(colour),
					m_scale(scale)
			{
				if (m_scale <= 0.f)
					m_scale = font->line_height();
			}

			UIText(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len = -1, float scale = 0.f, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), const glm::ivec2& position = glm::ivec2(0), bool visible = true);

			void colour(const glm::vec4& colour) { m_colour = colour; }

		protected:
			Text m_text;
			glm::vec4 m_colour;
			float m_scale;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};

		class UIShadowText : public UIText
		{
		public:
			template <typename Allocator>
			UIShadowText(const OOBase::SharedPtr<Font>& font, const OOBase::SharedString<Allocator>& text, float scale = 0.f, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), const glm::vec4& shadow = glm::vec4(0.f,0.f,0.f,.5f), const glm::ivec2& position = glm::ivec2(0), const glm::ivec2& drop = glm::ivec2(0,-1), bool visible = true) :
					UIText(font,text,scale,colour,position,visible),
					m_shadow(shadow),
					m_drop(drop)
			{
			}

			UIShadowText(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len = -1, float scale = 0.f, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), const glm::vec4& shadow = glm::vec4(.5f), const glm::ivec2& position = glm::ivec2(0), const glm::ivec2& drop = glm::ivec2(0,-1), bool visible = true);

			void shadow(const glm::vec4& shadow) { m_shadow = shadow; }

		private:
			glm::vec4 m_shadow;
			glm::ivec2 m_drop;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}
}

#endif // INDIGO_UITEXT_H_INCLUDED
