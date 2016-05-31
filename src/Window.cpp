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

#include "../include/indigo/Window.h"
#include "../include/indigo/Render.h"
#include "../include/indigo/Thread.h"

#include <OOGL/StateFns.h>

#include "Common.h"

Indigo::Render::Window::Window(Indigo::Window* owner) :
		m_owner(owner)
{
}

bool Indigo::Render::Window::create_window(const Indigo::Window::CreateParams& params)
{
	GLFWmonitor* monitor = NULL;
	if (params.m_fullscreen || !params.m_height || !params.m_width)
		monitor = glfwGetPrimaryMonitor();

	const char* title = params.m_title;
	if (!title)
		title = "Indigo";

	m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(params.m_width,params.m_height,title,params.m_style,monitor);
	if (!m_wnd)
		LOG_ERROR_RETURN(("Failed to create window: %s",OOBase::system_error_text()),false);

	if (!m_wnd->valid())
		m_wnd.reset();
	else
	{
		m_wnd->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_close));
		m_wnd->on_moved(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_move));
		m_wnd->on_sized(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_size));
		m_wnd->on_draw(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_draw));
		m_wnd->on_mousemove(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_mousemove));
		m_wnd->on_mousebutton(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_mousebutton));

		if (params.m_style & OOGL::Window::eWSdebug_context)
			OOGL::StateFns::get_current()->enable_logging();
	}

	return true;
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
	for (OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;)
	{
		OOBase::SharedPtr<Layer> layer = i->lock();
		if (layer)
		{
			layer->on_size(sz);
			++i;
		}
		else
			i = m_layers.erase(i);
	}


	logic_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_size),sz);
}

void Indigo::Render::Window::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	for (OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;)
	{ 
		OOBase::SharedPtr<Layer> layer = i->lock();
		if (layer)
		{
			if (layer->m_visible)
				layer->on_draw(glState);
			++i;
		}
		else
			i = m_layers.erase(i);
	}
}

void Indigo::Render::Window::on_mousemove(const OOGL::Window& win, double screen_x, double screen_y)
{
	logic_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_mousemove),screen_x,screen_y);
}

void Indigo::Render::Window::on_mousebutton(const OOGL::Window& win, const OOGL::Window::mouse_click_t& click)
{
	logic_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_mousebutton),click);
}

void Indigo::Render::Window::add_render_layer(Indigo::Layer* layer, bool* ret)
{
	*ret = false;
	layer->m_render_layer = layer->create_render_layer(this);
	if (layer->m_render_layer)
	{
		if (!m_layers.push_back(layer->m_render_layer))
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
	destroy();
}

bool Indigo::Window::create(const CreateParams& params)
{
	if (m_render_wnd)
		LOG_WARNING_RETURN(("Window already created"),true);

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate(this,&Window::on_create),&params,&ret) || !ret)
		return false;

	return render_pipe()->post(OOBase::make_delegate(this,&Window::run));
}

void Indigo::Window::on_create(const CreateParams* params, bool* ret)
{
	*ret = false;

	m_render_wnd = OOBase::allocate_shared<Indigo::Render::Window,OOBase::ThreadLocalAllocator>(this);
	if (!m_render_wnd)
		LOG_ERROR(("Failed to create window: %s",OOBase::system_error_text()));
	else
	{
		if (!m_render_wnd->create_window(*params))
			m_render_wnd.reset();
		else
			*ret = true;
	}
}

void Indigo::Window::run()
{
	static const OOBase::uint64_t monitor_refresh = 1000000 / 60;

	OOBase::WeakPtr<OOGL::Window> weak_wnd(m_render_wnd->m_wnd);

	for (;;)
	{
		OOBase::Clock draw_clock;

		// Scope wnd
		{
			OOBase::SharedPtr<OOGL::Window> wnd(weak_wnd.lock());
			if (!wnd)
				break;

			if (wnd->visible() && !wnd->iconified())
			{
				// Update animations


				// Draw window
				wnd->draw();
			}
		}

		// Poll for UI events
		glfwPollEvents();

		// Drain render commands
		if (!thread_pipe()->drain())
			return;

		// If we have cycles spare, wait a bit
		if (draw_clock.microseconds() < monitor_refresh - 5000)
		{
			OOBase::Timeout wait(0,static_cast<unsigned int>(monitor_refresh - draw_clock.microseconds()));
			while (!wait.has_expired())
			{
				glfwPollEvents();

				OOBase::Timeout wait2(0,1000);
				if (wait < wait2)
					wait2 = wait;

				if (!thread_pipe()->poll(wait2))
					return;
			}
		}
	}
}

void Indigo::Window::destroy()
{
	m_layers.clear();
	m_named_layers.clear();

	render_pipe()->call(OOBase::make_delegate(this,&Window::on_destroy));
}

void Indigo::Window::on_destroy()
{
	m_render_wnd.reset();
}

bool Indigo::Window::show(bool visible)
{
	if (!m_render_wnd)
	{
		if (visible)
			LOG_ERROR_RETURN(("Failed to show: incomplete window"),false);
		else
			LOG_WARNING_RETURN(("Incomplete window is not visible"),true);
	}

	return render_pipe()->post(OOBase::make_delegate(m_render_wnd->m_wnd.get(),&OOGL::Window::show),visible);
}

bool Indigo::Window::add_layer(const OOBase::SharedPtr<Layer>& layer, const char* name, size_t len)
{
	if (!m_render_wnd)
		LOG_ERROR_RETURN(("Failed to add layer: incomplete window"),false);

	if (!m_layers.push_back(layer))
		LOG_ERROR_RETURN(("Failed to insert layer: %s",OOBase::system_error_text()),false);

	if (name && len && !m_named_layers.insert(OOBase::Hash<const char*>::hash(name,len),layer))
		LOG_ERROR_RETURN(("Failed to insert layer name: %s",OOBase::system_error_text()),false);

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate(m_render_wnd.get(),&Render::Window::add_render_layer),layer.get(),&ret) || !ret)
		remove_layer(name,len);

	return ret;
}

bool Indigo::Window::remove_layer(const char* name, size_t len)
{
	if (!name || !len)
		return false;

	return m_named_layers.remove(OOBase::Hash<const char*>::hash(name,len)) != 0;
}

void Indigo::Window::on_close()
{
	bool handled = false;
	for (OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();!handled && i;)
	{
		OOBase::SharedPtr<Layer> layer = i->lock();
		if (!layer)
			i = m_layers.erase(i);
		else
		{
			if (layer->visible())
				handled = layer->on_close();

			--i;
		}
	}
}

void Indigo::Window::on_move(glm::ivec2 pos)
{
	for (OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;)
	{
		OOBase::SharedPtr<Layer> layer = i->lock();
		if (!layer)
			i = m_layers.erase(i);
		else
		{
			layer->on_move(pos);
			++i;
		}
	}
}

void Indigo::Window::on_size(glm::uvec2 sz)
{
	for (OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;)
	{
		OOBase::SharedPtr<Layer> layer = i->lock();
		if (!layer)
			i = m_layers.erase(i);
		else
		{
			layer->on_size(sz);
			++i;
		}
	}
}

void Indigo::Window::on_mousemove(double screen_x, double screen_y)
{
	bool handled = false;
	for (OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();!handled && i;)
	{
		OOBase::SharedPtr<Layer> layer = i->lock();
		if (!layer)
			i = m_layers.erase(i);
		else
		{
			if (layer->visible())
				handled = layer->on_mousemove(screen_x,screen_y);

			--i;
		}
	}
}

void Indigo::Window::on_mousebutton(OOGL::Window::mouse_click_t click)
{
	bool handled = false;
	for (OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();!handled && i;)
	{
		OOBase::SharedPtr<Layer> layer = i->lock();
		if (!layer)
			i = m_layers.erase(i);
		else
		{
			if (layer->visible())
				handled = layer->on_mousebutton(click);

			--i;
		}
	}
}
