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

#include "../../include/indigo/ui/UILabel.h"

#include "../../include/indigo/Font.h"
#include "../../include/indigo/Render.h"

#include "../Common.h"

Indigo::Render::UILabel::UILabel(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len, unsigned int font_size, const glm::vec4& colour, bool visible, const glm::ivec2& position, const glm::uvec2& size) :
		Text(font,sz,len),
		UIDrawable(visible,position,size),
		m_colour(colour),
		m_font_size(static_cast<float>(font_size))
{
	if (font_size == 0)
		m_font_size = static_cast<float>(font->line_height());
}

void Indigo::Render::UILabel::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_colour.a > 0.0f)
		Text::draw(glState,glm::scale(mvp,glm::vec3(m_font_size)),m_colour);
}

Indigo::Render::UIShadowLabel::UIShadowLabel(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len, unsigned int size, const glm::vec4& colour, const glm::vec4& shadow, const glm::ivec2& drop, bool visible, const glm::ivec2& position) :
		UILabel(font,sz,len,size,colour,visible,position),
		m_shadow(shadow),
		m_drop(drop)
{
}

void Indigo::Render::UIShadowLabel::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_drop != glm::ivec2(0) && m_shadow.a > 0.f)
		Text::draw(glState,glm::scale(glm::translate(mvp,glm::vec3(m_drop.x,m_drop.y,0.f)),glm::vec3(m_font_size)),m_shadow);

	if (m_colour.a > 0.0f)
		Text::draw(glState,glm::scale(mvp,glm::vec3(m_font_size)),m_colour);
}

Indigo::UILabel::UILabel(UIGroup* parent, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& caption, const CreateParams& params) :
		UIWidget(parent,params),
		m_text(caption),
		m_font(params.m_font),
		m_font_size(params.m_font_size),
		m_style(params.m_style),
		m_colour(params.m_colour),
		m_caption(NULL)
{
	if (!m_font || !m_font->valid())
		LOG_ERROR(("Invalid font passed to UILabel constructor"));
	else if (!m_font_size)
		m_font_size = m_font->line_height();

	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
}

Indigo::UILabel::UILabel(UIGroup* parent, const char* sz, size_t len, const CreateParams& params) :
		UIWidget(parent,params),
		m_font(params.m_font),
		m_font_size(params.m_font_size),
		m_style(params.m_style),
		m_colour(params.m_colour),
		m_caption(NULL)
{
	if (!m_font || !m_font->valid())
		LOG_ERROR(("Invalid font passed to UILabel constructor"));
	else if (!m_font_size)
		m_font_size = m_font->line_height();

	if (!m_text.assign(sz,len))
		LOG_ERROR(("Failed to assign text: %s",OOBase::system_error_text()));

	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
}

bool Indigo::UILabel::on_render_create(Indigo::Render::UIGroup* group)
{
	unsigned int caption_height = 0;
	unsigned int caption_width = 0;

	if (!m_text.empty())
	{
		caption_height = m_font_size;
		caption_width = static_cast<unsigned int>(ceil(caption_height * m_font->measure_text(m_text.c_str(),m_text.length())));
	}

	glm::uvec2 sz = size();
	glm::ivec2 pos(0);

	if (m_style & UILabel::align_right)
		pos.x = sz.x - caption_width;
	else if (m_style & UILabel::align_hcentre)
		pos.x = (sz.x - caption_width) / 2;

	if (m_style & UILabel::align_top)
		pos.y = sz.y - caption_height;
	else if (m_style & UILabel::align_vcentre)
		pos.y = (sz.y - caption_height) / 2;

	OOBase::SharedPtr<Render::UIDrawable> caption = OOBase::allocate_shared<Render::UILabel,OOBase::ThreadLocalAllocator>(m_font->render_font(),m_text.c_str(),m_text.length(),m_font_size,m_colour,true,pos);
	if (!caption)
		LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);

	if (!group->add_drawable(caption))
		return false;

	m_caption = caption.get();
	return true;
}

glm::uvec2 Indigo::UILabel::ideal_size() const
{
	unsigned int caption_height = 0;
	unsigned int caption_width = 0;

	if (!m_text.empty())
	{
		caption_height = m_font_size;
		caption_width = static_cast<unsigned int>(ceil(caption_height * m_font->measure_text(m_text.c_str(),m_text.length())));
	}

	return glm::uvec2(caption_width,caption_height);
}

void Indigo::UILabel::on_size(glm::uvec2& sz)
{
	unsigned int caption_height = 0;
	unsigned int caption_width = 0;

	if (!m_text.empty())
	{
		caption_height = m_font_size;
		caption_width = static_cast<unsigned int>(ceil(caption_height * m_font->measure_text(m_text.c_str(),m_text.length())));
	}

	glm::ivec2 pos(0);
	if (m_style & UILabel::align_right)
		pos.x = sz.x - caption_width;
	else if (m_style & UILabel::align_hcentre)
		pos.x = (sz.x - caption_width) / 2;

	if (m_style & UILabel::align_top)
		pos.y = sz.y - caption_height;
	else if (m_style & UILabel::align_vcentre)
		pos.y = (sz.y - caption_height) / 2;

	if (m_caption)
		render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_caption,&Render::UIDrawable::position),pos);
}
