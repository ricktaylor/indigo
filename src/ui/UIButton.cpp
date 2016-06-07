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

#include "../Common.h"

#include "../../include/indigo/ui/UIButton.h"

#include "../../include/indigo/Render.h"

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

Indigo::UIButton::UIButton(UIGroup* parent, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& caption, const CreateParams& params) :
		UIWidget(parent,params),
		m_text(caption),
		m_style(params.m_style),
		m_current_style(NULL)
{
	if (!m_style)
		LOG_ERROR(("Invalid style passed to UIButton constructor"));

	update_sizes();

	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
}

Indigo::UIButton::UIButton(UIGroup* parent, const char* sz, size_t len, const CreateParams& params) :
	UIWidget(parent,params),
	m_style(params.m_style),
	m_current_style(NULL)
{
	if (!m_style)
		LOG_ERROR(("Invalid style passed to UIButton constructor"));

	if (!m_text.assign(sz,len))
		LOG_ERROR(("Failed to assign text: %s",OOBase::system_error_text()));

	update_sizes();

	if (params.m_size == glm::uvec2(0))
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
	glm::uvec2 back_size(0);
	glm::uvec4 margins(0);
	glm::uvec2 text(0);

	if (style.m_background)
	{
		margins = style.m_background->margins();
		back_size = style.m_background->ideal_size();
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

	return glm::max(back_size,text);
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

bool Indigo::UIButton::style_create(Indigo::Render::UIGroup* group, StyleState& style, RenderStyleState& rs, bool visible)
{
	glm::uvec2 sz = size();
	glm::uvec4 margins(0);

	if (style.m_font && style.m_font_size == 0)
		style.m_font_size = style.m_font->line_height();

	StyleState* styles[] = { &m_style->m_normal, &m_style->m_active, &m_style->m_pressed, &m_style->m_disabled };
	RenderStyleState* render_styles[] = { &m_normal, &m_active, &m_pressed, &m_disabled };
	for (size_t i = 0; i < sizeof(styles)/sizeof(styles[0]); ++i)
	{
		if (styles[i] != &style)
		{
			if (!rs.m_background && styles[i]->m_background == style.m_background &&
					styles[i]->m_background_colour == style.m_background_colour)
			{
				rs.m_background = render_styles[i]->m_background;

				if (visible)
					rs.m_background->show(true);
			}

			if (!rs.m_caption && styles[i]->m_font == style.m_font &&
					styles[i]->m_font_size == style.m_font_size &&
					styles[i]->m_drop == style.m_drop &&
					styles[i]->m_shadow == style.m_shadow &&
					styles[i]->m_text_colour == style.m_text_colour)
			{
				rs.m_caption = render_styles[i]->m_caption;

				if (visible)
					rs.m_caption->show(true);
			}

			if (styles[i] > &style)
				break;
		}
	}

	if (!rs.m_background && style.m_background)
	{
		OOBase::SharedPtr<Render::UIDrawable> bk = style.m_background->make_drawable(visible,glm::ivec2(0),sz,style.m_background_colour);
		if (!bk)
			return false;

		if (!group->add_drawable(bk))
			return false;

		rs.m_background = bk.get();

		margins = style.m_background->margins();
		sz.x -= margins.x + margins.z;
		sz.y -= margins.y + margins.w;
	}

	if (!rs.m_caption && style.m_font)
	{
		unsigned int caption_height = 0;
		unsigned int caption_width = 0;

		if (!m_text.empty() && style.m_font)
		{
			caption_height = style.m_font_size;
			caption_width = static_cast<unsigned int>(ceil(caption_height * style.m_font->measure_text(m_text.c_str(),m_text.length())));
		}

		glm::ivec2 pos(margins.x,margins.w);
		if (style.m_style_flags & UIButton::align_right)
			pos.x += sz.x - caption_width;
		else if (style.m_style_flags & UIButton::align_hcentre)
			pos.x += (sz.x - caption_width) / 2;

		if (style.m_style_flags & UIButton::align_top)
			pos.y += sz.y - caption_height;
		else if (style.m_style_flags & UIButton::align_vcentre)
			pos.y += (sz.y - caption_height) / 2;

		OOBase::SharedPtr<Render::UILabel> caption;
		if (style.m_drop.x && style.m_drop.y && style.m_shadow.a > 0.f)
			caption = OOBase::allocate_shared<Render::UIShadowLabel,OOBase::ThreadLocalAllocator>(style.m_font->render_font(),m_text.c_str(),m_text.length(),style.m_font_size,style.m_text_colour,style.m_shadow,style.m_drop,visible,pos);
		else
			caption = OOBase::allocate_shared<Render::UILabel,OOBase::ThreadLocalAllocator>(style.m_font->render_font(),m_text.c_str(),m_text.length(),style.m_font_size,style.m_text_colour,visible,pos);
		if (!caption)
			LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);

		if (!group->add_drawable(caption))
			return false;

		rs.m_caption = caption.get();
	}

	if (visible)
		m_current_style = &rs;

	return true;
}

bool Indigo::UIButton::on_render_create(Indigo::Render::UIGroup* group)
{
	if (!m_style)
		LOG_ERROR_RETURN(("Invalid style passed to UIButton constructor"),false);

	bool normal = false;
	bool pressed = false;
	bool disabled = false;

	OOBase::uint32_t state = UIWidget::state();
	if (!(state & eWS_enabled))
		disabled = true;
	else if (state & eBS_pressed)
		pressed = true;
	else
		normal = true;

	return (style_create(group,m_style->m_normal,m_normal,normal) &&
		style_create(group,m_style->m_active,m_active,false) &&
		style_create(group,m_style->m_pressed,m_pressed,pressed) &&
		style_create(group,m_style->m_disabled,m_disabled,disabled));
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

		glm::ivec2 pos(margins.x,margins.w);
		if (style.m_style_flags & UIButton::align_right)
			pos.x += cap_size.x - caption_width;
		else if (style.m_style_flags & UIButton::align_hcentre)
			pos.x += (cap_size.x - caption_width) / 2;

		if (style.m_style_flags & UIButton::align_top)
			pos.y += cap_size.y - caption_height;
		else if (style.m_style_flags & UIButton::align_vcentre)
			pos.y += (cap_size.y - caption_height) / 2;

		rs.m_caption->position(pos);
	}
}

void Indigo::UIButton::do_size(const glm::uvec2& sz)
{
	do_style_size(sz,m_style->m_normal,m_normal);
	do_style_size(sz,m_style->m_active,m_active);
	do_style_size(sz,m_style->m_pressed,m_pressed);
	do_style_size(sz,m_style->m_disabled,m_disabled);
}

void Indigo::UIButton::on_size(const glm::uvec2& sz)
{
	if (valid())
		render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&UIButton::do_size),sz);
}

void Indigo::UIButton::do_style_change(RenderStyleState* new_style)
{
	if (m_current_style && new_style && m_current_style != new_style)
	{
		if (m_current_style->m_background != new_style->m_background)
		{
			if (m_current_style->m_background)
				m_current_style->m_background->show(false);

			if (new_style->m_background)
				new_style->m_background->show(true);
		}

		if (m_current_style->m_caption != new_style->m_caption)
		{
			if (m_current_style->m_caption)
				m_current_style->m_caption->show(false);

			if (new_style->m_caption)
				new_style->m_caption->show(true);
		}
	}

	m_current_style = new_style;
}

void Indigo::UIButton::on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask)
{
	if (change_mask & (eWS_enabled | eBS_pressed | eBS_mouseover))
	{
		bool enabled = (state & eWS_enabled) == eWS_enabled;
		bool pressed = (state & eBS_pressed) == eBS_pressed;
		bool mouseover = (state & eBS_mouseover) == eBS_mouseover;

		RenderStyleState* new_style = NULL;
		if (!enabled)
			new_style = &m_disabled;
		else if (pressed)
			new_style = &m_pressed;
		else if (mouseover)
			new_style = &m_active;
		else
			new_style = &m_normal;

		if (valid() && new_style)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&UIButton::do_style_change),new_style);
	}

	UIWidget::on_state_change(state,change_mask);
}

void Indigo::UIButton::on_mouseenter(bool enter)
{
	toggle_state(OOBase::uint32_t(enter ? eBS_mouseover : 0),eBS_mouseover | eBS_pressed);
}

bool Indigo::UIButton::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	if (click.button == GLFW_MOUSE_BUTTON_LEFT)
	{
		OOBase::uint32_t curr_state = state();
		if (!click.down && (curr_state & eBS_pressed))
		{
			if (m_on_click)
				thread_pipe()->post(m_on_click);
		}

		toggle_state(click.down,eBS_pressed);

		return true;
	}

	return UIWidget::on_mousebutton(click);
}

OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> Indigo::UIButton::on_click(const OOBase::Delegate0<void,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> prev = m_on_click;
	m_on_click = delegate;
	return prev;
}
