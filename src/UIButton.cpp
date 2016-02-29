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

Indigo::UIButton::UIButton(const OOBase::SharedPtr<Style>& style, const char* sz, size_t len, const glm::ivec2& position, const glm::uvec2& size) :
	UIWidget(position,size),
	m_style(style)
{
	if (!m_text.assign(sz,len))
		LOG_ERROR(("Failed to assign text: %s",OOBase::system_error_text()));

	if (size == glm::uvec2(0))
		this->size(ideal_size());
}

glm::uvec2 Indigo::UIButton::ideal_size() const
{
	glm::u16vec4 margins = m_style->m_background.margins();

	unsigned int height = m_style->m_font.line_height();
	unsigned int width = static_cast<unsigned int>(ceil(height * m_style->m_font.measure_text(m_text.c_str(),m_text.length())));

	return glm::uvec2(width + margins.x + margins.z,height + margins.y + margins.w);
}

bool Indigo::UIButton::on_render_create(Indigo::Render::UIGroup* group)
{
	m_background = m_style->m_background.make_drawable(glm::ivec2(0),size());
	if (!m_background)
		return false;

	if (!group->add_drawable(OOBase::static_pointer_cast<Render::UIDrawable>(m_background),0))
		return false;

	glm::u16vec4 margins = m_style->m_background.margins();
	m_caption = OOBase::allocate_shared<Render::UIShadowText,OOBase::ThreadLocalAllocator>(m_style->m_font.render_font(),m_text,0.f,m_style->m_colour,m_style->m_shadow,glm::ivec2(margins.x,margins.y),m_style->m_drop);
	if (!m_caption)
		LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);
		
	return group->add_drawable(OOBase::static_pointer_cast<Render::UIDrawable>(m_caption),10);
}
