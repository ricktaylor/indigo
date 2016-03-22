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

#include "Common.h"
#include "Window.h"
#include "Render.h"

#include "../src/App.h"

void Indigo::Layer::show(bool visible)
{
	if (visible != m_visible)
	{
		m_visible = visible;

		if (m_render_layer)
			render_pipe()->post(OOBase::make_delegate(m_render_layer.get(),&Render::Layer::show),visible);
	}
}

Indigo::Render::Window::Window(Indigo::Window* owner) :
		m_owner(owner)
{
}

OOBase::WeakPtr<OOGL::Window> Indigo::Render::Window::create_window()
{
	unsigned int style = OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated;
	if (Indigo::is_debug())
		style |= OOGL::Window::eWSdebug_context;

	m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(800,600,"Indigo",style);
	if (!m_wnd)
		LOG_ERROR_RETURN(("Failed to create window: %s",OOBase::system_error_text()),OOBase::WeakPtr<OOGL::Window>());

	if (!m_wnd->valid())
		m_wnd.reset();
	else
	{
		m_wnd->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_close));
		m_wnd->on_moved(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_move));
		m_wnd->on_sized(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_size));
		m_wnd->on_draw(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_draw));

		if (Indigo::is_debug())
			OOGL::StateFns::get_current()->enable_logging();

		//glClearColor(0.f,0.f,0.f,0.f);
		//glEnable(GL_BLEND);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
	}

	return m_wnd;
}

void Indigo::Render::Window::on_close(const OOGL::Window& win)
{
	logic_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_close));
}

void Indigo::Render::Window::on_move(const OOGL::Window& win, const glm::ivec2& pos)
{
	logic_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_move),pos);
}

void Indigo::Render::Window::on_size(const OOGL::Window& win, const glm::uvec2& sz)
{
	logic_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_size),sz);
}

void Indigo::Render::Window::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
	{ 
		if (i->second->m_visible)
			i->second->on_draw(glState);
	}
}

void Indigo::Render::Window::add_render_layer(Indigo::Layer* layer, unsigned int zorder, bool* ret)
{
	*ret = false;
	layer->m_render_layer = layer->create_render_layer(this);
	if (layer->m_render_layer)
	{
		if (!m_layers.insert(zorder,layer->m_render_layer))
		{
			LOG_ERROR(("Failed to insert layer: %s",OOBase::system_error_text()));
			layer->m_render_layer.reset();
		}
		else
			*ret = true;
	}
}

Indigo::Window::Window()
{
}

Indigo::Window::~Window()
{
	m_layers.clear();

	render_pipe()->call(OOBase::make_delegate(this,&Window::on_destroy));
}

OOBase::WeakPtr<OOGL::Window> Indigo::Window::create()
{
	m_render_wnd = OOBase::allocate_shared<Indigo::Render::Window,OOBase::ThreadLocalAllocator>(this);
	if (!m_render_wnd)
		LOG_ERROR_RETURN(("Failed to create window: %s",OOBase::system_error_text()),OOBase::WeakPtr<OOGL::Window>());

	return m_render_wnd->create_window();
}

void Indigo::Window::on_destroy()
{
	m_render_wnd.reset();
}

bool Indigo::Window::show(bool visible)
{
	return render_pipe()->post(OOBase::make_delegate(m_render_wnd->m_wnd.get(),&OOGL::Window::show),visible);
}

bool Indigo::Window::add_layer(const OOBase::SharedPtr<Layer>& layer, unsigned int zorder)
{
	if (!m_layers.insert(zorder,layer))
		LOG_ERROR_RETURN(("Failed to insert layer: %s",OOBase::system_error_text()),false);

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate(m_render_wnd.get(),&Render::Window::add_render_layer),layer.get(),zorder,&ret) || !ret)
		m_layers.remove(zorder);
	return ret;
}

bool Indigo::Window::remove_layer(unsigned int zorder)
{
	return m_layers.remove(zorder);
}

unsigned int Indigo::Window::top_layer() const
{
	OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i = m_layers.back();
	return (i ? i->first : 0);
}

void Indigo::Window::on_close()
{
	bool handled = false;
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();!handled && i;--i)
		handled = i->second->on_quit();

	if (!handled)
		Application::on_quit();
}

void Indigo::Window::on_move(glm::ivec2 pos)
{
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
		i->second->on_move(pos);
}

void Indigo::Window::on_size(glm::uvec2 sz)
{
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
		i->second->on_size(sz);
}
