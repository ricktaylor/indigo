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

Indigo::Render::TopLayer::TopLayer(Indigo::TopLayer* owner) : m_owner(owner)
{
}

bool Indigo::Render::TopLayer::create(const OOBase::SharedPtr<Render::MainWindow>& wnd)
{
	return wnd->add_layer(shared_from_this());
}

Indigo::TopLayer::TopLayer()
{
}

bool Indigo::TopLayer::create(const OOBase::SharedPtr<Render::MainWindow>& wnd)
{
	OOBase::SharedPtr<Render::TopLayer> layer = OOBase::allocate_shared<Render::TopLayer,OOBase::ThreadLocalAllocator>(this);
	if (!layer)
		LOG_ERROR_RETURN(("Failed to allocate TopLayer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!layer->create(wnd))
		return false;
	
	layer.swap(m_layer);
	return true;
}

void Indigo::TopLayer::destroy()
{
	m_layer.reset();
}
