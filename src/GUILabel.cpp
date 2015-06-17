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

#include "GUILabel.h"
#include "Render.h"
#include "Font.h"

namespace
{
	class Label : public Indigo::Render::GUI::Widget
	{
	public:
		Label();

		bool set_text(const OOBase::String* text);

	private:
		OOBase::SharedPtr<Indigo::Render::Text> m_text;

		void draw(OOGL::State& glState, const glm::mat4& mvp);

		glm::u16vec2 ideal_size() const;
		glm::u16vec2 text_size() const;
	};
}

Label::Label()
{

}

glm::u16vec2 Label::ideal_size() const
{
	glm::u16vec2 sz(Widget::ideal_size());
	glm::u16vec2 text_sz(text_size());

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
		OOBase::SharedPtr<Indigo::Render::Font> render_font = style()->font();
		if (!render_font)
			LOG_ERROR_RETURN(("Label font not loaded"),false);

		OOBase::SharedPtr<Indigo::Render::Text> render_text = OOBase::allocate_shared<Indigo::Render::Text,OOBase::ThreadLocalAllocator>(render_font,text->c_str(),text->length());
		if (!render_text)
			LOG_ERROR_RETURN(("Failed to allocate Text: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		render_text.swap(m_text);
	}
	else
	{
		window()->make_current();

		// Text change
		if (!m_text->text(text->c_str(),text->length()))
			return false;
	}
	return true;
}

void Label::draw(OOGL::State& glState, const glm::mat4& mvp)
{
	if (m_text)
	{
		glm::i16vec2 p = position();
		glm::u16vec2 sz = size();

		glm::mat4 model = glm::translate(glm::mat4(1),glm::vec3(p.x,p.y,0.f));
		model = glm::scale(model,glm::vec3(sz.x/m_text->length(),sz.y,0.f));

		m_text->draw(glState,mvp * model,style()->foreground_colour());
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
