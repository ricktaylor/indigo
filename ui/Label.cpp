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

#include "Label.h"

Indigo::Render::Label::Label(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len, unsigned int size, const glm::vec4& colour, const glm::ivec2& position) :
		Text(font,sz,len),
		UIDrawable(position),
		m_colour(colour),
		m_size(size)
{
	if (m_size == 0)
		m_size = font->line_height();
}

void Indigo::Render::Label::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_colour.a > 0.0f)
		Text::draw(glState,glm::scale(mvp,glm::vec3(m_size)),m_colour);
}

Indigo::Render::ShadowLabel::ShadowLabel(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len, unsigned int size, const glm::vec4& colour, const glm::vec4& shadow, const glm::ivec2& position, const glm::ivec2& drop) :
		Label(font,sz,len,size,colour,position),
		m_shadow(shadow),
		m_drop(drop)
{
}

void Indigo::Render::ShadowLabel::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_drop != glm::ivec2(0) && m_shadow.a > 0.f)
		Text::draw(glState,glm::scale(glm::translate(mvp,glm::vec3(m_drop.x,m_drop.y,0.f)),glm::vec3(m_size)),m_shadow);

	if (m_colour.a > 0.0f)
		Text::draw(glState,glm::scale(mvp,glm::vec3(m_size)),m_colour);
}
