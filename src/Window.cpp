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
#include "App.h"

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
	render_pipe()->post(OOBase::make_delegate(Indigo::App::instance_ptr(),&Indigo::Application::on_close));
}

void Indigo::Render::Window::on_move(const OOGL::Window& win, const glm::ivec2& pos)
{
	render_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_move),pos);
}

void Indigo::Render::Window::on_size(const OOGL::Window& win, const glm::uvec2& sz)
{
	//glViewport(0, 0, sz.x, sz.y);

	render_pipe()->post(OOBase::make_delegate(m_owner,&Indigo::Window::on_size),sz);
}

void Indigo::Render::Window::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	//glState.bind(GL_DRAW_FRAMEBUFFER,win.default_frame_buffer());

	//glClear(GL_COLOR_BUFFER_BIT);

	for (OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i!=m_layers.end();++i)
		i->second->on_draw(glState);
}

void Indigo::Render::Window::add_render_layer(Indigo::Layer* layer, unsigned int zorder, bool* ret)
{
	*ret = false;
	layer->m_render_layer = layer->create_render_layer();
	if (layer->m_render_layer)
	{
		if (m_layers.insert(zorder,layer->m_render_layer) == m_layers.end())
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

OOBase::WeakPtr<OOGL::Window> Indigo::Window::create()
{
	m_render_wnd = OOBase::allocate_shared<Indigo::Render::Window,OOBase::ThreadLocalAllocator>(this);
	if (!m_render_wnd)
		LOG_ERROR_RETURN(("Failed to create window: %s",OOBase::system_error_text()),OOBase::WeakPtr<OOGL::Window>());

	return m_render_wnd->create_window();
}

bool Indigo::Window::add_layer(const OOBase::SharedPtr<Layer>& layer, unsigned int zorder)
{
	if (m_layers.insert(zorder,layer) == m_layers.end())
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

void Indigo::Window::on_move(glm::ivec2 pos)
{
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i!=m_layers.end();++i)
		i->second->on_move(pos);
}

void Indigo::Window::on_size(glm::uvec2 sz)
{
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i!=m_layers.end();++i)
		i->second->on_size(sz);
}

/*bool Indigo::Window::create()
{
	if (m_wnd)
		LOG_ERROR_RETURN(("Window already created"),false);

	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Window::do_create),&ret) || !ret)
		return false;

	// Set up top layer
	OOBase::SharedPtr<GUI::Style> style = OOBase::allocate_shared<GUI::Style>();
	if (!style || !style->create(m_wnd->m_wnd))
		LOG_ERROR_RETURN(("Failed to create style: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!style->font(static_resources(),"Titillium-Regular.fnt") ||
			!style->foreground_colour(glm::u8vec4(192,224,225,255)) ||
			!style->border_image(static_resources(),"menu_border.png") ||
			!style->borders(9,9,9,9))
	{
		return false;
	}

	if (!m_top_layer.create(m_wnd,style,0))
		return false;

	OOBase::SharedPtr<GUI::GridSizer> sizer = OOBase::allocate_shared<GUI::GridSizer>();
	if (!sizer)
		LOG_ERROR_RETURN(("Failed to create sizer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!sizer->create(0,0) || !m_top_layer.sizer(sizer))
		return false;

	m_app = app;
	return (m_wnd != NULL);
}
*/
