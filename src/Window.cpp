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

#include "Common.h"

#include <OOGL/StateFns.h>

Indigo::Render::Window::Window(Indigo::Window* owner) :
		m_owner(owner),
		m_have_cursor(false),
		m_dirty(true)
{
	ASSERT_RENDER_THREAD();
}

Indigo::Render::Window::~Window()
{
	ASSERT_RENDER_THREAD();

	m_layers.clear();
	m_wnd.reset();
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
		m_wnd->on_mousebutton(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_mousebutton));
		m_wnd->on_cursorenter(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_cursorenter));
		m_wnd->on_cursormove(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_cursormove));
		m_wnd->on_focus(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_focus));
		m_wnd->on_iconify(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_iconify));
				
		if (params.m_style & OOGL::Window::eWSdebug_context)
			OOGL::StateFns::get_current()->enable_logging();
	}

	return true;
}

void Indigo::Render::Window::on_close(const OOGL::Window&)
{
	logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::Window::on_close));
}

void Indigo::Render::Window::on_iconify(const OOGL::Window&, bool iconified)
{
	ASSERT_RENDER_THREAD();

	logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::Window::on_iconify),iconified);
}

void Indigo::Render::Window::on_move(const OOGL::Window& win, const glm::ivec2& pos)
{
	logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::Window::on_move),pos);
}

void Indigo::Render::Window::on_size(const OOGL::Window&, const glm::uvec2& sz)
{
	for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
		(*i)->on_size(sz);
}

void Indigo::Render::Window::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	bool hit_test = m_dirty;
	
	// Update all layers
	for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
	{
		if ((*i)->on_update())
			hit_test = true;
	}
	m_dirty = false;

	if (m_wnd->visible() && !m_wnd->iconified())
	{
		// If something changed, then hit test
		if (m_have_cursor && hit_test)
		{
			OOBase::SharedPtr<Layer> cursor_layer;
			for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();i;--i)
			{
				if ((*i)->on_cursormove(m_cursor_pos))
				{
					cursor_layer = *i;
					break;
				}
			}

			if (cursor_layer != m_cursor_layer)
			{
				OOBase::SharedPtr<Layer> prev_cursor_layer = m_cursor_layer.lock();
				if (prev_cursor_layer)
					prev_cursor_layer->on_losecursor();

				m_cursor_layer = cursor_layer;
			}
		}
	
		// Render all layers
		for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::const_iterator i=m_layers.cbegin();i;++i)
			(*i)->on_draw(glState);
	}
}

void Indigo::Render::Window::on_cursormove(const OOGL::Window& win, const glm::dvec2& pos)
{
	if (m_wnd->visible() && !m_wnd->iconified())
	{
		m_cursor_pos = pos;
		m_have_cursor = true;

		OOBase::SharedPtr<Layer> cursor_layer;
		for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();i;--i)
		{
			if ((*i)->on_cursormove(m_cursor_pos))
			{
				cursor_layer = *i;
				break;
			}
		}

		if (cursor_layer != m_cursor_layer)
		{
			OOBase::SharedPtr<Layer> prev_cursor_layer = m_cursor_layer.lock();
			if (prev_cursor_layer)
				prev_cursor_layer->on_losecursor();

			m_cursor_layer = cursor_layer;
		}
	}
}

void Indigo::Render::Window::on_mousebutton(const OOGL::Window&, const OOGL::Window::mouse_click_t& click)
{
	OOBase::SharedPtr<Layer> cursor_layer = m_cursor_layer.lock();
	if (cursor_layer)
	{
		if (cursor_layer->on_mousebutton(click))
		{
			// Grabbed focus!
			if (cursor_layer != m_focus_layer)
			{
				OOBase::SharedPtr<Layer> prev_focus_layer = m_focus_layer.lock();
				if (prev_focus_layer)
					prev_focus_layer->on_losefocus();

				m_focus_layer = cursor_layer;
			}
		}
	}
}

void Indigo::Render::Window::on_cursorenter(const OOGL::Window& win, bool enter)
{
	if (!enter)
	{
		OOBase::SharedPtr<Layer> prev_cursor_layer = m_cursor_layer.lock();
		if (prev_cursor_layer)
			prev_cursor_layer->on_losecursor();

		m_cursor_layer.reset();
	}

	m_have_cursor = enter;
}

void Indigo::Render::Window::on_focus(const OOGL::Window& win, bool focused)
{
	if (!focused)
	{
		OOBase::SharedPtr<Layer> prev_focus_layer = m_focus_layer.lock();
		if (prev_focus_layer)
			prev_focus_layer->on_losefocus();

		m_focus_layer.reset();
	}
}

void Indigo::Render::Window::grab_focus(Layer* layer)
{
	OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.find(layer);
	if (i && m_focus_layer != *i)
	{
		OOBase::SharedPtr<Layer> prev_focus_layer = m_focus_layer.lock();
		if (prev_focus_layer)
			prev_focus_layer->on_losefocus();

		m_focus_layer = *i;
	}
}

void Indigo::Render::Window::add_render_layer(Indigo::Layer* layer, bool* ret)
{
	*ret = false;
	OOBase::SharedPtr<Render::Layer> render_layer = layer->m_render_layer;
	if (!render_layer)
	{
		render_layer = layer->create_render_layer(this);
		layer->m_render_layer = render_layer;
	}

	if (render_layer)
	{
		if (!m_layers.push_back(render_layer))
		{
			LOG_ERROR(("Failed to insert layer: %s",OOBase::system_error_text()));
			layer->m_render_layer.reset();
		}
		else
		{
			m_dirty = true;
			*ret = true;
		}
	}
}

void Indigo::Render::Window::remove_render_layer(Indigo::Layer* layer)
{
	if (m_layers.remove(layer->m_render_layer) != 0)
		m_dirty = true;
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
	if (!render_pipe()->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_create),params,&ret) || !ret)
		return false;

	return render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::run));
}

void Indigo::Window::on_create(const CreateParams& params, bool* ret)
{
	*ret = false;

	m_render_wnd = OOBase::allocate_shared<Indigo::Render::Window,OOBase::ThreadLocalAllocator>(this);
	if (!m_render_wnd)
		LOG_ERROR(("Failed to create window: %s",OOBase::system_error_text()));
	else
	{
		if (!m_render_wnd->create_window(params))
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

		OOBase::SharedPtr<OOGL::Window> wnd(weak_wnd.lock());
		if (!wnd)
			break;

		// Update animations

		// Draw window
		wnd->draw();
		wnd.reset();

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
	show(false);

	m_layers.clear();
	m_named_layers.clear();

	render_pipe()->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Window::on_destroy));
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

	return render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_wnd->m_wnd.get(),&OOGL::Window::show),visible);
}

bool Indigo::Window::add_layer(const OOBase::SharedPtr<Layer>& layer, const char* name, size_t len)
{
	if (!m_render_wnd)
		LOG_ERROR_RETURN(("Failed to add layer: incomplete window"),false);

	size_t hash;
	if (name && len)
	{
		hash = OOBase::Hash<const char*>::hash(name,len);

		if (!m_named_layers.insert(hash,layer))
			LOG_ERROR_RETURN(("Failed to insert layer name: %s",OOBase::system_error_text()),false);
	}

	if (!m_layers.push_back(layer))
		LOG_ERROR_RETURN(("Failed to insert layer: %s",OOBase::system_error_text()),false);

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_wnd.get(),&Render::Window::add_render_layer),layer.get(),&ret) || !ret)
	{
		if (name && len)
			m_named_layers.remove(hash);

		m_layers.pop_back();
	}

	return ret;
}

bool Indigo::Window::remove_layer(const char* name, size_t len)
{
	if (!name || !len)
		return false;

	OOBase::HashTable<size_t,OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i = m_named_layers.find(OOBase::Hash<const char*>::hash(name,len));
	if (!i)
		return false;

	OOBase::SharedPtr<Layer> l = i->second.lock();
	m_named_layers.erase(i);

	if (!l)
		return false;

	return remove_layer(l);
}

bool Indigo::Window::remove_layer(const OOBase::SharedPtr<Layer>& layer)
{
	OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i = m_layers.find(layer);
	if (!i)
		return false;

	if (m_render_wnd)
		render_pipe()->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_wnd.get(),&Render::Window::remove_render_layer),i->get());

	m_layers.erase(i);
	return true;
}

void Indigo::Window::on_close()
{
	bool handled = false;
	for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();!handled && i;--i)
		handled = (*i)->on_close();
}

void Indigo::Window::on_move(const glm::ivec2& pos)
{
	for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
		(*i)->on_move(pos);
}

void Indigo::Window::on_iconify(bool iconified)
{
	bool handled = false;
	for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.back();!handled && i;--i)
		handled = (*i)->on_iconify(iconified);
}
