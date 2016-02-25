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

#include "Common.h"
#include "UIButton.h"

Indigo::UIButton::UIButton(const OOBase::SharedPtr<UIStyle>& style, const glm::ivec2& position, const glm::uvec2& size) :
	UIWidget(position,size),
	m_style(style)
{
}

glm::uvec2 Indigo::UIButton::ideal_size() const
{
	glm::u16vec4 margins = m_style->m_background.margins();

	return glm::uvec2(margins.x + margins.z,margins.y + margins.w);
}

bool Indigo::UIButton::on_render_create(Indigo::Render::UIGroup* group)
{
	m_background = m_style->m_background.make_drawable(position(),size());
	if (!m_background)
		LOG_ERROR_RETURN(("Failed to allocate background image: %s",OOBase::system_error_text()),false);

	return group->add_drawable(OOBase::static_pointer_cast<Render::UIDrawable>(m_background),0);
}
