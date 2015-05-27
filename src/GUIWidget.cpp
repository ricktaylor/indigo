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

bool Indigo::Render::GUI::Widget::create(const OOBase::SharedPtr<Widget>& parent, const glm::i16vec2& pos, const glm::u16vec2& size)
{
	if (parent && !parent->add_child(shared_from_this()))
		return false;

	m_pos = pos;
	m_min_size = size;
	m_size = min_size();

	m_parent = parent;
	return true;
}

glm::u16vec2 Indigo::Render::GUI::Widget::size() const
{
	return m_size;
}

void Indigo::Render::GUI::Widget::size(const glm::u16vec2& sz, bool layout)
{
	bool changed = (m_size != sz);

	if ((m_min_size.x != -1 && m_size.x < m_min_size.x) || 
		(m_min_size.y != -1 && m_size.y < m_min_size.y))
	{
		if (m_min_size.x != -1 && m_size.x < m_min_size.x)
			m_size.x = m_min_size.x;

		if (m_min_size.y != -1 && m_size.y < m_min_size.y)
			m_size.y = m_min_size.y;

		changed = true;
	}

	if (m_size.x > m_max_size.x || m_size.y > m_max_size.y)
	{
		if (m_size.x > m_max_size.x)
			m_size.x = m_max_size.x;

		if (m_size.y > m_max_size.y)
			m_size.y = m_max_size.y;

		changed = true;
	}

	if (layout && changed && shown())
	{
		OOBase::SharedPtr<Widget> parent(m_parent);
		if (parent)
			parent->refresh_layout();
	}
}

glm::u16vec2 Indigo::Render::GUI::Widget::min_size() const
{
	glm::u16vec2 min_size(m_min_size);
	if (min_size.x == -1 || min_size.y == -1)
	{
		glm::u16vec2 best(do_get_best_size());

		if (min_size.x == -1)
			min_size.x = best.x;

		if (min_size.y == -1)
			min_size.y = best.y;
	}
	return min_size;
}

void Indigo::Render::GUI::Widget::min_size(const glm::u16vec2& sz)
{
	m_min_size = sz;

	if (m_min_size.x != -1 && m_max_size.x < m_min_size.x)
		m_max_size.x = m_min_size.x;

	if (m_min_size.y != -1 && m_max_size.y < m_min_size.y)
		m_max_size.y = m_min_size.y;

	if ((m_min_size.x != -1 && m_size.x < m_min_size.x) || 
		(m_min_size.y != -1 && m_size.y < m_min_size.y))
	{
		if (m_min_size.x != -1 && m_size.x < m_min_size.x)
			m_size.x = m_min_size.x;

		if (m_min_size.y != -1 && m_size.y < m_min_size.y)
			m_size.y = m_min_size.y;

		if (shown())
		{
			OOBase::SharedPtr<Widget> parent(m_parent);
			if (parent)
				parent->refresh_layout();
		}
	}
}

glm::u16vec2 Indigo::Render::GUI::Widget::max_size() const
{
	return m_max_size;
}

void Indigo::Render::GUI::Widget::max_size(const glm::u16vec2& sz)
{
	m_max_size = sz;

	if (m_min_size.x != -1 && m_max_size.x < m_min_size.x)
		m_max_size.x = m_min_size.x;

	if (m_min_size.y != -1 && m_max_size.y < m_min_size.y)
		m_max_size.y = m_min_size.y;

	if (m_size.x > m_max_size.x || m_size.y > m_max_size.y)
	{
		if (m_size.x > m_max_size.x)
			m_size.x = m_max_size.x;

		if (m_size.y > m_max_size.y)
			m_size.y = m_max_size.y;

		if (shown())
		{
			OOBase::SharedPtr<Widget> parent(m_parent);
			if (parent)
				parent->refresh_layout();
		}
	}
}

glm::u16vec2 Indigo::Render::GUI::Widget::do_get_best_size() const
{
	glm::u16vec2 sz(get_best_size());

	if (m_min_size.x != -1 && m_min_size.x > sz.x)
		sz.x = m_min_size.x;

	if (m_min_size.y != -1 && m_min_size.y > sz.y)
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
	if (!on_show(show))
		return false;

	bool changed = (m_visible != show);
	m_visible = show;
	
	if (changed)
	{
		OOBase::SharedPtr<Widget> parent(m_parent);
		if (parent)
			parent->refresh_layout();
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
	if (!shown() || !on_enable(enable))
		return false;

	m_enabled = enable;
	return true;
}

bool Indigo::Render::GUI::Widget::focused() const
{
	return m_focused && enabled();
}

bool Indigo::Render::GUI::Widget::focus(bool focused)
{
	if (!enabled() || !on_focus(focused))
		return false;

	m_focused = focused;
	return true;
}

bool Indigo::Render::GUI::Widget::hilighted() const
{
	return m_hilighted && enabled();
}

bool Indigo::Render::GUI::Widget::hilight(bool hilighted)
{
	if (!enabled() || !on_hilight(hilighted))
		return false;

	m_hilighted = hilighted;
	return true;
}

Indigo::GUI::Widget::Widget()
{
}

Indigo::GUI::Widget::~Widget()
{
	destroy();
}

bool Indigo::GUI::Widget::create(Widget* parent, const glm::i16vec2& pos, const glm::u16vec2& min_size)
{
	if (m_render_widget)
		LOG_ERROR_RETURN(("Widget::Create called twice"),false);

	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Widget::do_create),&ret,parent,&pos,&min_size) && ret;
}

void Indigo::GUI::Widget::do_create(bool* ret_val, Widget* parent, const glm::i16vec2* pos, const glm::u16vec2* min_size)
{
	OOBase::SharedPtr<Render::GUI::Widget> widget = create_widget();
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

		if (!widget->create(render_parent,*pos,*min_size))
			*ret_val = false;
		else
		{
			widget.swap(m_render_widget);
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
	return render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_shown),&shown) && shown;
}

void Indigo::GUI::Widget::get_shown(bool* shown)
{
	*shown = m_render_widget->shown();
}

bool Indigo::GUI::Widget::visible() const
{
	bool visible = false;
	return render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_visible),&visible) && visible;
}

void Indigo::GUI::Widget::get_visible(bool* visible)
{
	*visible = m_render_widget->visible();
}

bool Indigo::GUI::Widget::visible(bool show)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_visible),&show) && show;
}

void Indigo::GUI::Widget::set_visible(bool* visible)
{
	*visible = m_render_widget->visible(visible);
}

bool Indigo::GUI::Widget::enabled() const
{
	bool enabled = false;
	return render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_enabled),&enabled) && enabled;
}

void Indigo::GUI::Widget::get_enabled(bool* enabled)
{
	*enabled = m_render_widget->enabled();
}

bool Indigo::GUI::Widget::enable(bool enabled)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_enable),&enabled) && enabled;
}

void Indigo::GUI::Widget::set_enable(bool* enabled)
{
	*enabled = m_render_widget->enable(*enabled);
}

bool Indigo::GUI::Widget::focused() const
{
	bool focused = false;
	return render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_focused),&focused) && focused;
}

void Indigo::GUI::Widget::get_focused(bool* focused)
{
	*focused = m_render_widget->focused();
}

bool Indigo::GUI::Widget::focus(bool focus)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_focus),&focus) && focus;
}

void Indigo::GUI::Widget::set_focus(bool* focused)
{
	*focused = m_render_widget->focus(focused);
}

bool Indigo::GUI::Widget::hilighted() const
{
	bool hilighted = false;
	return render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_hilighted),&hilighted) && hilighted;
}

void Indigo::GUI::Widget::get_hilighted(bool* hilighted)
{
	*hilighted = m_render_widget->hilighted();
}

bool Indigo::GUI::Widget::hilight(bool hilight)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_hilight),&hilight) && hilight;
}

void Indigo::GUI::Widget::set_hilight(bool* hilighted)
{
	*hilighted = m_render_widget->hilight(hilighted);
}