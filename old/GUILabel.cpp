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

#include "../old/GUILabel.h"

#include "../src/NinePatch.h"
#include "Font.h"
#include "Render.h"

namespace
{
	class Label : public Indigo::Render::GUI::Widget
	{
	public:
		Label();

		bool set_text(const OOBase::String* text);

	private:
		OOBase::String m_string;
		OOBase::SharedPtr<Indigo::Render::Text> m_text;
		OOBase::SharedPtr<Indigo::Render::NinePatch> m_border;

		void draw(OOGL::State& glState, const glm::mat4& mvp);

		glm::u16vec2 size(const glm::u16vec2& sz);
		glm::u16vec2 ideal_size() const;
		glm::u16vec2 text_size() const;

		void style(const OOBase::SharedPtr<Indigo::Render::GUI::Style>& s);
		bool refresh_border();
	};
}

Label::Label()
{
}

void Label::style(const OOBase::SharedPtr<Indigo::Render::GUI::Style>& new_style)
{
	const OOBase::SharedPtr<Indigo::Render::GUI::Style>& old_style = Widget::style();
	bool refresh_text = false;
	bool refresh_bdr = false;
	if (!old_style || !new_style)
	{
		refresh_text = true;
		refresh_bdr = true;
	}
	else
	{
		if (old_style->font() != new_style->font())
			refresh_text = true;

		if (new_style->borders() != old_style->borders() || new_style->border_image_size() != old_style->border_image_size())
			refresh_bdr = true;
	}

	Widget::style(new_style);

	if (refresh_bdr)
		refresh_border();

	if (refresh_text)
	{
		OOBase::SharedPtr<Indigo::Render::Text> render_text;

		if (!m_string.empty())
		{
			// Font change, reallocate text
			OOBase::SharedPtr<Indigo::Render::Font> render_font = new_style->font();
			if (render_font)
			{
				window()->make_current();

				render_text = OOBase::allocate_shared<Indigo::Render::Text,OOBase::ThreadLocalAllocator>(render_font,m_string.c_str(),m_string.length());
				if (!render_text)
					LOG_ERROR(("Failed to allocate Text: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));
			}
		}
		render_text.swap(m_text);
	}
}

bool Label::refresh_border()
{
	//if (m_style_flags & Indigo::GUI::Panel::show_border)
	{
		window()->make_current();

		const OOBase::SharedPtr<Indigo::Render::GUI::Style>& style = Widget::style();
		if (!m_border)
		{
			m_border = OOBase::allocate_shared<Indigo::Render::NinePatch,OOBase::ThreadLocalAllocator>(Widget::size(),style->borders(),style->border_image_size());
			if (!m_border)
				LOG_ERROR_RETURN(("Failed to allocate NinePatch: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}
		else
			m_border->layout(Widget::size(),style->borders(),style->border_image_size());
	}
	return true;
}

glm::u16vec2 Label::size(const glm::u16vec2& sz)
{
	glm::u16vec2 old_sz = Widget::size();
	glm::u16vec2 new_sz = Widget::size(sz);
	if (old_sz != new_sz)
		refresh_border();

	return new_sz;
}

glm::u16vec2 Label::ideal_size() const
{
	glm::u16vec2 sz(Widget::ideal_size());
	glm::u16vec2 text_sz(text_size());

	if (Widget::style())
	{
		const glm::u16vec4& borders = Widget::style()->borders();
		text_sz.x += borders.x + borders.z;
		text_sz.y += borders.y + borders.w;
	}

	if (text_sz.x < sz.x)
		text_sz.x = sz.x;

	if (text_sz.y < sz.y)
		text_sz.y = sz.y;

	return text_sz;
}

glm::u16vec2 Label::text_size() const
{
	glm::u16vec2 sz(0);
	if (m_text)
	{
		double line_height = m_text->font()->line_height();
		sz.y = static_cast<OOBase::uint16_t>(line_height);
		sz.x = static_cast<OOBase::uint16_t>(m_text->length() * line_height);
	}
	return sz;
}

bool Label::set_text(const OOBase::String* text)
{
	if (text->empty())
		m_text.reset();
	else if (!m_text)
	{
		window()->make_current();

		// Font change, reallocate text
		OOBase::SharedPtr<Indigo::Render::Font> render_font = Widget::style()->font();
		if (render_font)
		{
			OOBase::SharedPtr<Indigo::Render::Text> render_text = OOBase::allocate_shared<Indigo::Render::Text,OOBase::ThreadLocalAllocator>(render_font,text->c_str(),text->length());
			if (!render_text)
				LOG_ERROR_RETURN(("Failed to allocate Text: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

			render_text.swap(m_text);
		}
	}
	else
	{
		window()->make_current();

		// Text change
		if (!m_text->text(text->c_str(),text->length()))
			return false;
	}

	m_string = *text;
	return true;
}

void Label::draw(OOGL::State& glState, const glm::mat4& mvp)
{
	glm::i16vec2 p = position();
	glm::mat4 child_mvp = mvp * glm::translate(glm::mat4(1),glm::vec3(p.x,p.y,0));

	if (m_border && m_border->valid())
		m_border->draw(glState,Widget::style()->border_colour(),Widget::style()->border_image(),child_mvp);

	if (m_text)
	{
		glm::u16vec2 sz = Widget::size();

		if (Widget::style())
		{
			const glm::u16vec4& borders = Widget::style()->borders();
			if (borders.x || borders.w)
				child_mvp = glm::translate(child_mvp,glm::vec3(borders.x,borders.w,0));

			sz.x -= borders.x + borders.z;
			sz.y -= borders.y + borders.w;
		}

		child_mvp = glm::scale(child_mvp,glm::vec3(sz.x/m_text->length(),sz.y,0.f));

		m_text->draw(glState,child_mvp,Widget::style()->foreground_colour());
	}
}

Indigo::GUI::Label::Label()
{
}

OOBase::SharedPtr<Indigo::Render::GUI::Widget> Indigo::GUI::Label::create_render_widget()
{
	OOBase::SharedPtr< ::Label> label = OOBase::allocate_shared< ::Label,OOBase::ThreadLocalAllocator>();
	if (!label)
		LOG_ERROR(("Failed to allocate label: %s",OOBase::system_error_text()));

	return label;
}

bool Indigo::GUI::Label::create(Widget* parent, const OOBase::String& text, const glm::u16vec2& min_size, const glm::i16vec2& pos)
{
	if (!Widget::create(parent,min_size,pos))
		return false;

	return common_create(text);
}

bool Indigo::GUI::Label::create(Widget* parent, const OOBase::SharedPtr<Style>& style, const OOBase::String& text, const glm::u16vec2& min_size, const glm::i16vec2& pos)
{
	if (!Widget::create(parent,style,min_size,pos))
		return false;

	return common_create(text);
}

bool Indigo::GUI::Label::common_create(const OOBase::String& text)
{
	if (!text.empty())
	{
		bool ret = false;
		if (!render_call(OOBase::make_delegate(this,&Label::set_text),&ret,&text) || !ret)
			return false;
	}

	m_text = text;
	return true;
}

const OOBase::String& Indigo::GUI::Label::text() const
{
	return m_text;
}

bool Indigo::GUI::Label::text(const OOBase::String& text)
{
	if (text != m_text)
	{
		bool ret = false;
		if (!render_call(OOBase::make_delegate(this,&Indigo::GUI::Label::set_text),&ret,&text) || !ret)
			return false;

		m_text = text;
	}
	return true;
}

void Indigo::GUI::Label::set_text(bool* ret_val, const OOBase::String* text)
{
	*ret_val = render_widget< ::Label>()->set_text(text);
}
