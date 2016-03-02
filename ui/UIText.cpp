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

#include "../core/Common.h"

#include "UIText.h"

Indigo::Render::UIText::UIText(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len, float scale, const glm::vec4& colour, const glm::ivec2& position, bool visible) :
		UIDrawable(position,visible),
		m_text(font,sz,len),
		m_colour(colour),
		m_scale(scale)
{
	if (m_scale <= 0.f)
		m_scale = font->line_height();
}

void Indigo::Render::UIText::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	m_text.draw(glState,glm::scale(mvp,glm::vec3(m_scale)),m_colour);
}

Indigo::Render::UIShadowText::UIShadowText(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len, float scale, const glm::vec4& colour, const glm::vec4& shadow, const glm::ivec2& position, const glm::ivec2& drop, bool visible) :
		UIText(font,sz,len,scale,colour,position,visible),
		m_shadow(shadow),
		m_drop(drop)
{
}

void Indigo::Render::UIShadowText::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_drop != glm::ivec2(0) && m_shadow.a > 0.f)
		m_text.draw(glState,glm::scale(glm::translate(mvp,glm::vec3(m_drop.x,m_drop.y,0.f)),glm::vec3(m_scale)),m_shadow);

	m_text.draw(glState,glm::scale(mvp,glm::vec3(m_scale)),m_colour);
}

