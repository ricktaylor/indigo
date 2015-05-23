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

Indigo::Render::GUI::Widget::Widget() : m_visible(false), m_enabled(true), m_focused(false), m_hilighted(false)
{

}

Indigo::Render::GUI::Widget::~Widget()
{
	OOBase::SharedPtr<Widget> parent(m_parent);
	if (parent)
		parent->remove_child(shared_from_this());
}

bool Indigo::Render::GUI::Widget::create(const OOBase::SharedPtr<Widget>& parent)
{
	if (parent && !parent->add_child(shared_from_this()))
		return false;

	m_parent = parent;
	return true;
}

bool Indigo::Render::GUI::Widget::visible() const
{
	OOBase::SharedPtr<Widget> parent(m_parent);
	return m_visible && (!parent || parent->visible());
}

bool Indigo::Render::GUI::Widget::visible(bool show)
{
	if (!on_show(show))
		return false;
	m_visible = show;
	
	OOBase::SharedPtr<Widget> parent(m_parent);
	if (parent)
		parent->on_refresh_layout();

	return true;
}

bool Indigo::Render::GUI::Widget::enabled() const
{
	OOBase::SharedPtr<Widget> parent(m_parent);
	return m_enabled && visible() && (!parent || parent->enabled());
}

bool Indigo::Render::GUI::Widget::enable(bool enable)
{
	if (!visible() || !on_enable(enable))
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

bool Indigo::GUI::Widget::create(Widget* parent)
{
	if (m_ptrWidget)
		LOG_ERROR_RETURN(("Widget::Create called twice"),false);

	return render_call(OOBase::make_delegate(this,&Widget::do_create),parent);
}

bool Indigo::GUI::Widget::do_create(Widget* parent)
{
	OOBase::SharedPtr<Render::GUI::Widget> widget = create_widget();
	if (!widget)
		LOG_ERROR_RETURN(("Failed to allocate Widget: %s",OOBase::system_error_text()),false);

	OOBase::SharedPtr<Render::GUI::Widget> render_parent;
	if (parent)
		render_parent = parent->m_ptrWidget;

	if (!widget->create(render_parent))
		return false;

	widget.swap(m_ptrWidget);
	return true;
}

bool Indigo::GUI::Widget::destroy()
{
	return !m_ptrWidget || render_call(OOBase::make_delegate(this,&Widget::do_destroy));
}

bool Indigo::GUI::Widget::do_destroy()
{
	m_ptrWidget.reset();
	return true;
}

bool Indigo::GUI::Widget::visible() const
{
	bool visible = false;
	if (!render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_visible),&visible))
		return false;
	return visible;
}

bool Indigo::GUI::Widget::get_visible(bool* visible)
{
	*visible = m_ptrWidget->visible();
	return true;
}

bool Indigo::GUI::Widget::visible(bool show)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_visible),show);
}

bool Indigo::GUI::Widget::set_visible(bool visible)
{
	return m_ptrWidget->visible(visible);
}

bool Indigo::GUI::Widget::enabled() const
{
	bool enabled = false;
	if (!render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_enabled),&enabled))
		return false;
	return enabled;
}

bool Indigo::GUI::Widget::get_enabled(bool* enabled)
{
	*enabled = m_ptrWidget->enabled();
	return true;
}

bool Indigo::GUI::Widget::enable(bool enabled)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_enable),enabled);
}

bool Indigo::GUI::Widget::set_enable(bool enabled)
{
	return m_ptrWidget->enable(enabled);
}

bool Indigo::GUI::Widget::focused() const
{
	bool focused = false;
	if (!render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_focused),&focused))
		return false;
	return focused;
}

bool Indigo::GUI::Widget::get_focused(bool* focused)
{
	*focused = m_ptrWidget->focused();
	return true;
}

bool Indigo::GUI::Widget::focus(bool focus)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_focus),focus);
}

bool Indigo::GUI::Widget::set_focus(bool focused)
{
	return m_ptrWidget->focus(focused);
}

bool Indigo::GUI::Widget::hilighted() const
{
	bool hilighted = false;
	if (!render_call(OOBase::make_delegate(const_cast<Widget*>(this),&Widget::get_hilighted),&hilighted))
		return false;
	return hilighted;
}

bool Indigo::GUI::Widget::get_hilighted(bool* hilighted)
{
	*hilighted = m_ptrWidget->hilighted();
	return true;
}

bool Indigo::GUI::Widget::hilight(bool hilight)
{
	return render_call(OOBase::make_delegate(this,&Widget::set_hilight),hilight);
}

bool Indigo::GUI::Widget::set_hilight(bool hilighted)
{
	return m_ptrWidget->hilight(hilighted);
}
