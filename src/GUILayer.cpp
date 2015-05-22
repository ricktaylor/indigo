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

#include "MainWindow.h"
#include "Render.h"

Indigo::Render::GUIWidget::GUIWidget(const OOBase::SharedPtr<GUIWidget>& parent) : m_parent(parent)
{
}

Indigo::Render::GUILayer* Indigo::Render::GUIWidget::layer() const
{
	return m_parent ? m_parent->layer() : NULL;
}

OOBase::SharedPtr<Indigo::Render::GUIWidget> Indigo::Render::GUIWidget::parent() const
{
	return m_parent;
}

bool Indigo::Render::GUIWidget::add_widget(const OOBase::SharedPtr<GUIWidget>& widget)
{
	return m_children.push_back(widget) != m_children.end();
}

bool Indigo::Render::GUIWidget::remove_widget(const OOBase::SharedPtr<GUIWidget>& widget)
{
	return m_children.erase(widget) != 0;
}

void Indigo::Render::GUIWidget::on_draw(OOGL::State& glState)
{
	for (OOBase::Vector<OOBase::SharedPtr<GUIWidget>,OOBase::ThreadLocalAllocator>::iterator i=m_children.begin();i!=m_children.end();++i)
		(*i)->on_draw(glState);
}

Indigo::Render::GUILayer::GUILayer() : GUIWidget(OOBase::SharedPtr<GUIWidget>()), m_next_handle(1)
{
	m_handles.insert(0,shared_from_this());
}

Indigo::Render::GUILayer* Indigo::Render::GUILayer::layer() const
{
	return const_cast<GUILayer*>(this);
}

OOBase::uint32_t Indigo::Render::GUILayer::register_widget(const OOBase::SharedPtr<GUIWidget>& widget)
{
	OOBase::uint32_t h = 0;
	while (m_handles.exists(h))
		h = m_next_handle++;

	if (m_handles.insert(h,widget) == m_handles.end())
		LOG_ERROR_RETURN(("Failed to add widget to GUILayer"),0);

	return h;
}

bool Indigo::Render::GUILayer::unregister_widget(OOBase::uint32_t handle)
{
	if (!handle)
		return false;

	return m_handles.remove(handle);
}

OOBase::SharedPtr<Indigo::Render::GUIWidget> Indigo::Render::GUILayer::lookup_widget(OOBase::uint32_t handle) const
{
	OOBase::HashTable<OOBase::uint32_t,OOBase::SharedPtr<GUIWidget>,OOBase::ThreadLocalAllocator>::const_iterator i = m_handles.find(handle);
	if (i != m_handles.end())
		return i->value;

	return OOBase::SharedPtr<GUIWidget>();
}

void Indigo::Render::GUILayer::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	GUIWidget::on_draw(glState);
}

bool Indigo::GUILayer::create(const OOBase::SharedPtr<Render::MainWindow>& wnd)
{
	OOBase::SharedPtr<Render::GUILayer> layer = OOBase::allocate_shared<Render::GUILayer,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR_RETURN(("Failed to allocate GUILayer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!wnd->add_layer(layer))
		return false;

	layer.swap(m_layer);
	return true;
}

void Indigo::GUILayer::destroy()
{
	m_layer.reset();
}

OOBase::uint32_t Indigo::GUILayer::create_widget(const OOBase::Delegate2<OOBase::SharedPtr<Render::GUIWidget>,const OOBase::SharedPtr<Render::GUIWidget>&,const GUIWidget::CreateParams*>& delegate, const GUIWidget::CreateParams* p)
{
	WidgetCreateParams params;
	params.m_handle = 0;
	params.m_params = p;
	params.m_delegate = &delegate;

	if (!render_call(OOBase::make_delegate(this,&GUILayer::do_create_widget),&params))
		return 0;

	return params.m_handle;
}

bool Indigo::GUILayer::do_create_widget(WidgetCreateParams* p)
{
	OOBase::SharedPtr<Render::GUIWidget> parent = m_layer->lookup_widget(p->m_params->m_parent);
	OOBase::SharedPtr<Render::GUIWidget> widget = p->m_delegate->invoke(parent,p->m_params);
	if (!widget)
		return false;

	p->m_handle = m_layer->register_widget(widget);
	if (p->m_handle && !parent->add_widget(widget))
	{
		m_layer->unregister_widget(p->m_handle);
		return false;
	}
	return true;
}

bool Indigo::GUILayer::destroy_widget(OOBase::uint32_t handle)
{
	return render_call(OOBase::make_delegate(this,&GUILayer::do_destroy_widget),handle);
}

bool Indigo::GUILayer::do_destroy_widget(OOBase::uint32_t handle)
{
	OOBase::SharedPtr<Render::GUIWidget> widget = m_layer->lookup_widget(handle);
	if (!widget || !widget->on_destroy())
		return false;

	OOBase::SharedPtr<Render::GUIWidget> parent = widget->parent();
	if (parent)
		parent->remove_widget(widget);

	return m_layer->unregister_widget(handle);
}
