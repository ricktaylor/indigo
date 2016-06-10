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

#include "../include/indigo/Layer.h"
#include "../include/indigo/Render.h"

#include "Common.h"

Indigo::Render::Layer::Layer(Window* window) : m_window(window)
{
	ASSERT_RENDER_THREAD();
}

Indigo::Render::Layer::~Layer()
{
	ASSERT_RENDER_THREAD();
}

Indigo::Layer::~Layer()
{
	render_pipe()->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Layer::reset_render_layer));
}

void Indigo::Layer::reset_render_layer()
{
	m_render_layer.reset();
}
