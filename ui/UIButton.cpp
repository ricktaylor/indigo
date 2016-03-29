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

void Indigo::UIButton::StyleState::unload()
{
	if (m_background)
		m_background->unload();
}

void Indigo::UIButton::Style::unload()
{
	m_normal.unload();
	m_active.unload();
	m_pressed.unload();
}

Indigo::UIButton::UIButton(UIGroup* parent, const OOBase::SharedPtr<Style>& style, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& caption, uint32_t state, const glm::ivec2& position, const glm::uvec2& size) :
		UIWidget(parent,state,position,size),
		m_text(caption),
		m_style(style),
		m_current_style(NULL)
{
	if (!m_style)
		LOG_ERROR(("Invalid style passed to UIButton constructor"));

	update_sizes();

	if (size == glm::uvec2(0))
		this->size(ideal_size());
}

Indigo::UIButton::UIButton(UIGroup* parent, const OOBase::SharedPtr<Style>& style, const char* sz, size_t len, uint32_t state, const glm::ivec2& position, const glm::uvec2& size) :
	UIWidget(parent,state,position,size),
	m_style(style),
	m_current_style(NULL)
{
	if (!m_style)
		LOG_ERROR(("Invalid style passed to UIButton constructor"));

	if (!m_text.assign(sz,len))
		LOG_ERROR(("Failed to assign text: %s",OOBase::system_error_text()));

	update_sizes();

	if (size == glm::uvec2(0))
		this->size(ideal_size());
}

bool Indigo::UIButton::valid() const
{
	return UIWidget::valid() && m_style;
}

glm::uvec2 Indigo::UIButton::min_style_size(const StyleState& style) const
{
	glm::uvec2 min_size(0);
	glm::uvec4 margins(0);
	glm::uvec2 text(0);

	if (style.m_background)
	{
		margins = style.m_background->margins();
		min_size = style.m_background->min_size();
	}

	if (!m_text.empty() && style.m_font)
	{
		unsigned int height = style.m_font_size;
		if (height == 0)
			height = style.m_font->line_height();
		unsigned int width = static_cast<unsigned int>(ceil(height * style.m_font->measure_text(m_text.c_str(),m_text.length())));

		text.x = width + margins.x + margins.z;
		text.y = height + margins.y + margins.w;
	}

	return glm::max(min_size,text);
}

glm::uvec2 Indigo::UIButton::ideal_style_size(const StyleState& style) const
{
	glm::uvec2 min_size(0);
	glm::uvec4 margins(0);
	glm::uvec2 text(0);

	if (style.m_background)
	{
		margins = style.m_background->margins();
		min_size = style.m_background->min_size();
	}

	if (!m_text.empty() && style.m_font)
	{
		unsigned int height = style.m_font_size;
		if (height == 0)
			height = style.m_font->line_height();
		unsigned int width = static_cast<unsigned int>(ceil(height * style.m_font->measure_text(m_text.c_str(),m_text.length())));

		text.x = width + margins.x + margins.z + 2 * height;
		text.y = height + margins.y + margins.w;
	}

	return glm::max(min_size,text);
}

void Indigo::UIButton::update_sizes()
{
	if (m_style)
	{
		m_min_size = glm::max(min_style_size(m_style->m_normal),
					glm::max(min_style_size(m_style->m_active),min_style_size(m_style->m_pressed)));

		m_ideal_size = glm::max(ideal_style_size(m_style->m_normal),
					glm::max(ideal_style_size(m_style->m_active),ideal_style_size(m_style->m_pressed)));
	}
}

bool Indigo::UIButton::style_create(Indigo::Render::UIGroup* group, StyleState& style, RenderStyleState& rs)
{
	glm::uvec2 sz = size();
	glm::uvec4 margins(0);

	if (style.m_background)
	{
		rs.m_background = style.m_background->make_drawable(glm::ivec2(0),sz,style.m_background_colour);
		if (!rs.m_background)
			return false;

		if (!group->add_drawable(rs.m_background,0))
			return false;

		margins = style.m_background->margins();
		sz.x -= margins.x + margins.z;
		sz.y -= margins.y + margins.w;
	}

	if (style.m_font)
	{
		if (style.m_font_size == 0)
			style.m_font_size = style.m_font->line_height();

		unsigned int caption_height = style.m_font_size;
		unsigned int caption_width = static_cast<unsigned int>(ceil(caption_height * style.m_font->measure_text(m_text.c_str(),m_text.length())));

		glm::ivec2 pos((sz.x - caption_width) / 2 + margins.x,(sz.y - caption_height) / 2 + margins.w);

		rs.m_caption = OOBase::allocate_shared<Render::ShadowLabel,OOBase::ThreadLocalAllocator>(style.m_font->render_font(),m_text.c_str(),m_text.length(),style.m_font_size,style.m_text_colour,style.m_shadow,pos,style.m_drop);
		if (!rs.m_caption)
			LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);

		if (!group->add_drawable(rs.m_caption,10))
			return false;
	}

	return true;
}

bool Indigo::UIButton::on_render_create(Indigo::Render::UIGroup* group)
{
	if (!m_style)
		LOG_ERROR_RETURN(("Invalid style passed to UIButton constructor"),false);

	if (!style_create(group,m_style->m_normal,m_normal) ||
		!style_create(group,m_style->m_active,m_active) ||
		!style_create(group,m_style->m_pressed,m_pressed))
	{
		return false;
	}

	if (m_normal.m_background)
		m_normal.m_background->show();

	if (m_normal.m_caption)
		m_normal.m_caption->show();

	return true;
}

void Indigo::UIButton::do_style_size(const glm::uvec2& sz, const StyleState& style, RenderStyleState& rs)
{
	if (rs.m_background)
		rs.m_background->size(sz);

	if (rs.m_caption)
	{
		glm::uvec4 margins(0);
		if (style.m_background)
			margins = style.m_background->margins();

		glm::uvec2 cap_size(sz.x - (margins.x + margins.z),sz.y - (margins.y + margins.w));

		unsigned int caption_height = 0;
		unsigned int caption_width = 0;

		if (!m_text.empty() && style.m_font)
		{
			caption_height = style.m_font_size;
			caption_width = static_cast<unsigned int>(ceil(caption_height * style.m_font->measure_text(m_text.c_str(),m_text.length())));
		}

		rs.m_caption->position(glm::ivec2((cap_size.x - caption_width) / 2 + margins.x,(cap_size.y - caption_height) / 2 + margins.w));
	}
}

void Indigo::UIButton::do_size(glm::uvec2 sz)
{
	do_style_size(sz,m_style->m_normal,m_normal);
	do_style_size(sz,m_style->m_active,m_active);
	do_style_size(sz,m_style->m_pressed,m_pressed);
}

void Indigo::UIButton::on_size(const glm::uvec2& sz)
{
	if (valid())
		render_pipe()->post(OOBase::make_delegate(this,&UIButton::do_size),sz);
}
