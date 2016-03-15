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

#include "UIButton.h"

Indigo::UIButton::UIButton(UIWidget* parent, const OOBase::SharedPtr<Style>& style, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& caption, const glm::ivec2& position, const glm::uvec2& size) :
		UIWidget(parent,position,size),
		m_text(caption),
		m_style(style)
{
	if (size == glm::uvec2(0))
		this->size(ideal_size());
}

Indigo::UIButton::UIButton(UIWidget* parent, const OOBase::SharedPtr<Style>& style, const char* sz, size_t len, const glm::ivec2& position, const glm::uvec2& size) :
	UIWidget(parent,position,size),
	m_style(style)
{
	if (!m_text.assign(sz,len))
		LOG_ERROR(("Failed to assign text: %s",OOBase::system_error_text()));

	if (size == glm::uvec2(0))
		this->size(ideal_size());
}

glm::uvec2 Indigo::UIButton::min_size() const
{
	glm::uvec4 margins = m_style->m_background.margins();

	unsigned int height = m_style->m_font.line_height();
	unsigned int width = static_cast<unsigned int>(ceil(height * m_style->m_font.measure_text(m_text.c_str(),m_text.length())));

	return glm::uvec2(width + margins.x + margins.z,height + margins.y + margins.w);
}

glm::uvec2 Indigo::UIButton::ideal_size() const
{
	glm::uvec4 margins = m_style->m_background.margins();

	unsigned int height = m_style->m_font.line_height();
	unsigned int width = static_cast<unsigned int>(ceil(height * m_style->m_font.measure_text(m_text.c_str(),m_text.length())));

	return glm::uvec2(width + margins.x + margins.z + 2 * height,height + margins.y + margins.w);
}

bool Indigo::UIButton::on_render_create(Indigo::Render::UIGroup* group)
{
	glm::uvec2 sz = size();
	glm::ivec2 pos(0);

	m_background = m_style->m_background.make_drawable(pos,sz);
	if (!m_background)
		return false;

	if (!group->add_drawable(m_background,0))
		return false;

	glm::uvec4 margins = m_style->m_background.margins();
	sz.x -= margins.x + margins.z;
	sz.y -= margins.y + margins.w;

	unsigned int caption_height = m_style->m_font.line_height();
	unsigned int caption_width = static_cast<unsigned int>(ceil(caption_height * m_style->m_font.measure_text(m_text.c_str(),m_text.length())));

	pos.x = (sz.x - caption_width) / 2 + margins.x;
	pos.y = (sz.y - caption_height) / 2 + margins.w;

	m_caption = OOBase::allocate_shared<Render::ShadowLabel,OOBase::ThreadLocalAllocator>(m_style->m_font.render_font(),m_text.c_str(),m_text.length(),0.f,m_style->m_colour,m_style->m_shadow,pos,m_style->m_drop);
	if (!m_caption)
		LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);
		
	if (!group->add_drawable(m_caption,10))
		return false;

	m_background->show();
	m_caption->show();

	return true;
}

void Indigo::UIButton::do_size(glm::uvec2 sz)
{
	if (m_background)
		m_background->size(sz);

	if (m_caption)
	{
		glm::uvec4 margins = m_style->m_background.margins();
		glm::uvec2 cap_size(sz.x - (margins.x + margins.z),sz.y - (margins.y + margins.w));

		unsigned int caption_height = m_style->m_font.line_height();
		unsigned int caption_width = static_cast<unsigned int>(ceil(caption_height * m_style->m_font.measure_text(m_text.c_str(),m_text.length())));

		m_caption->position(glm::ivec2((cap_size.x - caption_width) / 2 + margins.x,(cap_size.y - caption_height) / 2 + margins.w));
	}
}

void Indigo::UIButton::on_size(const glm::uvec2& sz)
{
	if (valid())
		render_pipe()->post(OOBase::make_delegate(this,&UIButton::do_size),sz);
}
