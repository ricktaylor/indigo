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

void Indigo::Render::Layer::show(bool visible)
{
	if (m_visible != visible)
	{
		m_visible = visible;

		if (visible)
		{
			// TODO: Fake a mouse move to init any cursor stuff here
		}
	}
}

Indigo::Layer::~Layer()
{
	render_pipe()->call(OOBase::make_delegate(this,&Layer::destroy_render_layer));
}

void Indigo::Layer::destroy_render_layer()
{
	m_render_layer.reset();
}

void Indigo::Layer::show(bool visible)
{
	if (visible != m_visible)
	{
		m_visible = visible;

		if (m_render_layer)
			render_pipe()->post(OOBase::make_delegate(m_render_layer.get(),&Render::Layer::show),visible);
	}
}
