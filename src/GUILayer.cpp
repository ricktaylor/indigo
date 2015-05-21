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

#include "GUILayer.h"

bool Indigo::Render::Widget::add_widget(const OOBase::SharedPtr<Widget>& widget)
{
	return m_children.push_back(widget) != m_children.end();
}

Indigo::Render::GUILayer::GUILayer() : m_next_handle(1)
{
}

OOBase::uint32_t Indigo::Render::GUILayer::add_widget(const OOBase::SharedPtr<Widget>& widget)
{
	OOBase::uint32_t h = 0;
	while (!h)
		h = m_next_handle++;

	if (m_handles.insert(h,widget) == m_handles.end())
		LOG_ERROR_RETURN(("Failed to add widget to GUILayer"),0);

	return h;
}

bool Indigo::Render::GUILayer::remove_widget(OOBase::uint32_t handle)
{
	return m_handles.remove(handle);
}

OOBase::SharedPtr<Indigo::Render::Widget> Indigo::Render::GUILayer::lookup_widget(OOBase::uint32_t handle)
{
	OOBase::HashTable<OOBase::uint32_t,OOBase::SharedPtr<Widget>,OOBase::ThreadLocalAllocator>::iterator i = m_handles.find(handle);
	if (i != m_handles.end())
		return i->value;

	return OOBase::SharedPtr<Widget>();
}

void Indigo::Render::GUILayer::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


}
