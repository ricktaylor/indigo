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

#include "GUIWidget.h"
#include "Render.h"

Indigo::Render::GUI::Widget::Widget() : m_visible(false), m_enabled(true), m_focused(false), m_hilighted(false), m_max_size(-1)
{

}

Indigo::Render::GUI::Widget::~Widget()
{
	OOBase::SharedPtr<Widget> parent(m_parent);
	if (parent)
		parent->remove_child(shared_from_this());
}

bool Indigo::Render::GUI::Widget::create(const OOBase::SharedPtr<Widget>& parent, const OOBase::SharedPtr<Style>& style, const glm::i16vec2& pos, const glm::u16vec2& size)
{
	if (parent && !parent->add_child(shared_from_this()))
		return false;

	m_position = pos;
	m_min_size = size;
	m_size = min_size();
	m_parent = parent;
	m_style = style;

	return true;
}

const OOBase::SharedPtr<OOGL::Window>& Indigo::Render::GUI::Widget::window() const
{
	return m_parent.lock()->window();
}

void Indigo::Render::GUI::Widget::position(const glm::i16vec2& pos)
{
	m_position = pos;
}

glm::u16vec2 Indigo::Render::GUI::Widget::size(const glm::u16vec2& sz)
{
	if (m_size != sz)
	{
		m_size = sz;

		if (m_min_size.x != glm::u16vec2::value_type(-1) && m_size.x < m_min_size.x)
			m_size.x = m_min_size.x;

		if (m_min_size.y != glm::u16vec2::value_type(-1) && m_size.y < m_min_size.y)
			m_size.y = m_min_size.y;

		if (m_size.x > m_max_size.x)
			m_size.x = m_max_size.x;

		if (m_size.y > m_max_size.y)
			m_size.y = m_max_size.y;
	}

	return m_size;
}

glm::u16vec2 Indigo::Render::GUI::Widget::min_size() const
{
	glm::u16vec2 min_size(m_min_size);
	if (min_size.x == glm::u16vec2::value_type(-1) || min_size.y == glm::u16vec2::value_type(-1))
	{
		glm::u16vec2 ideal(ideal_size());
		if (min_size.x == glm::u16vec2::value_type(-1))
			min_size.x = ideal.x;

		if (min_size.y == glm::u16vec2::value_type(-1))
			min_size.y = ideal.y;
	}

	return min_size;
}

glm::u16vec2 Indigo::Render::GUI::Widget::min_size(const glm::u16vec2& sz)
{
	if (m_min_size != sz)
	{
		m_min_size = sz;

		if (m_min_size.x != glm::u16vec2::value_type(-1))
		{
			if (m_max_size.x < m_min_size.x)
				m_max_size.x = m_min_size.x;

			if (m_size.x < m_min_size.x)
				m_size.x = m_min_size.x;
		}

		if (m_min_size.y != glm::u16vec2::value_type(-1))
		{
			if (m_max_size.y < m_min_size.y)
				m_max_size.y = m_min_size.y;

			if (m_size.y < m_min_size.y)
				m_size.y = m_min_size.y;
		}
	}

	return m_size;
}

glm::u16vec2 Indigo::Render::GUI::Widget::max_size(const glm::u16vec2& sz)
{
	if (m_max_size != sz)
	{
		m_max_size = sz;

		if (m_min_size.x != glm::u16vec2::value_type(-1) && m_max_size.x < m_min_size.x)
			m_max_size.x = m_min_size.x;

		if (m_min_size.y != glm::u16vec2::value_type(-1) && m_max_size.y < m_min_size.y)
			m_max_size.y = m_min_size.y;

		if (m_size.x > m_max_size.x)
			m_size.x = m_max_size.x;

		if (m_size.y > m_max_size.y)
			m_size.y = m_max_size.y;
	}

	return m_size;
}

glm::u16vec2 Indigo::Render::GUI::Widget::ideal_size() const
{
	glm::u16vec2 sz(0);
	if (m_min_size.x != glm::u16vec2::value_type(-1))
		sz.x = m_min_size.x;

	if (m_min_size.y != glm::u16vec2::value_type(-1))
		sz.y = m_min_size.y;

	if (sz.x > m_max_size.x)
		sz.x = m_max_size.x;

	if (sz.y > m_max_size.y)
		sz.y = m_max_size.y;

	return sz;
}

bool Indigo::Render::GUI::Widget::shown() const
{
	if (!visible())
		return false;

	OOBase::SharedPtr<Widget> parent(m_parent);
	return !parent || parent->shown();
}

bool Indigo::Render::GUI::Widget::visible() const
{
	return m_visible;
}

bool Indigo::Render::GUI::Widget::visible(bool show)
{
	if (show != m_visible)
	{
		if (!can_show(show))
			return false;
	
		m_visible = show;
	}
	return true;
}

bool Indigo::Render::GUI::Widget::enabled() const
{
	OOBase::SharedPtr<Widget> parent(m_parent);
	return m_enabled && shown() && (!parent || parent->enabled());
}

bool Indigo::Render::GUI::Widget::enable(bool enable)
{
	if (enable != m_enabled)
	{
		if (!shown() || !can_enable(enable))
			return false;

		m_enabled = enable;
	}
	return true;
}

bool Indigo::Render::GUI::Widget::focused() const
{
	return m_focused && enabled();
}

bool Indigo::Render::GUI::Widget::focus(bool focused)
{
	if (focused != m_focused)
	{
		if (!enabled() || !can_focus(focused))
			return false;

		m_focused = focused;
	}
	return true;
}

bool Indigo::Render::GUI::Widget::hilighted() const
{
	return m_hilighted && enabled();
}

bool Indigo::Render::GUI::Widget::hilight(bool hilighted)
{
	if (hilighted != m_hilighted)
	{
		if (!enabled() || !can_hilight(hilighted))
			return false;

		m_hilighted = hilighted;
	}
	return true;
}

Indigo::GUI::Widget::Widget()
{
}

Indigo::GUI::Widget::~Widget()
{
	destroy();
}

bool Indigo::GUI::Widget::create(Widget* parent, const glm::u16vec2& min_size, const glm::i16vec2& pos)
{
	if (!parent)
		LOG_ERROR_RETURN(("Widget::Create called with NULL parent and no style"),false);

	return create(parent,parent->style(),min_size,pos);
}

bool Indigo::GUI::Widget::create(Widget* parent, const OOBase::SharedPtr<Style>& style, const glm::u16vec2& min_size, const glm::i16vec2& pos)
{
	if (m_render_widget)
		LOG_ERROR_RETURN(("Widget::Create called twice"),false);

	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Widget::do_create),&ret,parent,&style,&pos,&min_size) && ret;
}

void Indigo::GUI::Widget::do_create(bool* ret_val, Widget* parent, const OOBase::SharedPtr<Style>* style, const glm::i16vec2* pos, const glm::u16vec2* min_size)
{
	OOBase::SharedPtr<Render::GUI::Widget> widget = create_render_widget();
	if (!widget)
	{
		LOG_ERROR(("Failed to allocate Widget: %s",OOBase::system_error_text()));
		*ret_val = false;
	}
	else
	{
		OOBase::SharedPtr<Render::GUI::Widget> render_parent;
		if (parent)
			render_parent = parent->m_render_widget;

		if (!widget->create(render_parent,(*style)->m_render_style,*pos,*min_size))
			*ret_val = false;
		else
		{
			widget.swap(m_render_widget);
			m_style = *style;
			*ret_val = true;
		}
	}
}

bool Indigo::GUI::Widget::destroy()
{
	return !m_render_widget || render_call(OOBase::make_delegate(this,&Widget::do_destroy));
}

void Indigo::GUI::Widget::do_destroy()
{
	m_render_widget.reset();
}

bool Indigo::GUI::Widget::shown() const
{
	bool shown = false;
	return m_render_widget && render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_shown),&shown) && shown;
}

void Indigo::GUI::Widget::get_shown(bool* shown)
{
	*shown = m_render_widget->shown();
}

bool Indigo::GUI::Widget::visible() const
{
	bool visible = false;
	return m_render_widget && render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_visible),&visible) && visible;
}

void Indigo::GUI::Widget::get_visible(bool* visible)
{
	*visible = m_render_widget->visible();
}

bool Indigo::GUI::Widget::visible(bool show)
{
	return m_render_widget && render_call(OOBase::make_delegate(this,&Widget::set_visible),&show) && show;
}

void Indigo::GUI::Widget::set_visible(bool* visible)
{
	*visible = m_render_widget->visible(*visible);
}

bool Indigo::GUI::Widget::enabled() const
{
	bool enabled = false;
	return m_render_widget && render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_enabled),&enabled) && enabled;
}

void Indigo::GUI::Widget::get_enabled(bool* enabled)
{
	*enabled = m_render_widget->enabled();
}

bool Indigo::GUI::Widget::enable(bool enabled)
{
	return m_render_widget && render_call(OOBase::make_delegate(this,&Widget::set_enable),&enabled) && enabled;
}

void Indigo::GUI::Widget::set_enable(bool* enabled)
{
	*enabled = m_render_widget->enable(*enabled);
}

bool Indigo::GUI::Widget::focused() const
{
	bool focused = false;
	return m_render_widget && render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_focused),&focused) && focused;
}

void Indigo::GUI::Widget::get_focused(bool* focused)
{
	*focused = m_render_widget->focused();
}

bool Indigo::GUI::Widget::focus(bool focus)
{
	return m_render_widget && render_call(OOBase::make_delegate(this,&Widget::set_focus),&focus) && focus;
}

void Indigo::GUI::Widget::set_focus(bool* focused)
{
	*focused = m_render_widget->focus(*focused);
}

bool Indigo::GUI::Widget::hilighted() const
{
	bool hilighted = false;
	return m_render_widget && render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_hilighted),&hilighted) && hilighted;
}

void Indigo::GUI::Widget::get_hilighted(bool* hilighted)
{
	*hilighted = m_render_widget->hilighted();
}

bool Indigo::GUI::Widget::hilight(bool hilight)
{
	return m_render_widget && render_call(OOBase::make_delegate(this,&Widget::set_hilight),&hilight) && hilight;
}

void Indigo::GUI::Widget::set_hilight(bool* hilighted)
{
	*hilighted = m_render_widget->hilight(*hilighted);
}

bool Indigo::GUI::Widget::style(const OOBase::SharedPtr<Style>& s)
{
	if (!s)
		LOG_ERROR_RETURN(("Widget::apply_style with NULL style"),false);

	return m_render_widget && s->m_render_style && render_call(OOBase::make_delegate(this,&Widget::set_style),&s->m_render_style);
}

void Indigo::GUI::Widget::set_style(OOBase::SharedPtr<Render::GUI::Style>* style)
{
	m_render_widget->m_style = *style;
}
