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
#include "App.h"
#include "Render.h"
#include "Font.h"
#include "GUIGridSizer.h"

#include "../lib/Shader.h"

#include <OOBase/TLSSingleton.h>

#include <glm/gtc/matrix_transform.hpp>

Indigo::Render::MainWindow::MainWindow(Indigo::MainWindow* owner) : m_owner(owner)
{
}

bool Indigo::Render::MainWindow::create()
{
	unsigned int style = OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated;
	if (Indigo::is_debug())
		style |= OOGL::Window::eWSdebug_context;

	m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(800,600,"Indigo",style);
	if (!m_wnd || !m_wnd->valid())
		return false;

	m_wnd->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&MainWindow::on_close));
	m_wnd->on_moved(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&MainWindow::on_move));
	m_wnd->on_sized(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&MainWindow::on_size));
	m_wnd->on_draw(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&MainWindow::on_draw));

	if (Indigo::is_debug())
		OOGL::StateFns::get_current()->enable_logging();

	glClearColor(0.f,0.f,0.f,0.f);
	glEnable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if (!Indigo::monitor_window(m_wnd))
		LOG_ERROR_RETURN(("Failed to monitor window"),false);

	return true;
}

glm::u16vec2 Indigo::Render::MainWindow::size() const
{
	glm::u16vec2 sz(0);
	if (m_wnd)
		sz = m_wnd->size();
	return sz;
}

const OOBase::SharedPtr<OOGL::Window>& Indigo::Render::MainWindow::window() const
{
	return m_wnd;
}

void Indigo::Render::MainWindow::on_close(const OOGL::Window& win)
{
	raise_event(OOBase::make_delegate(m_owner,&Indigo::MainWindow::on_close));
}

void Indigo::Render::MainWindow::on_move(const OOGL::Window& win, const glm::u16vec2& pos)
{
	//on_size(win,win.size());
}

void Indigo::Render::MainWindow::on_size(const OOGL::Window& win, const glm::u16vec2& sz)
{
	glViewport(0, 0, sz.x, sz.y);

	for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
		(*i)->on_size(win,sz);
}

void Indigo::Render::MainWindow::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	glState.bind(GL_DRAW_FRAMEBUFFER,win.default_frame_buffer());

	glClear(GL_COLOR_BUFFER_BIT);

	for (OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>::iterator i=m_layers.begin();i;++i)
		(*i)->on_draw(win,glState);
}

Indigo::MainWindow::MainWindow() : m_app(NULL)
{
}

Indigo::MainWindow::~MainWindow()
{
	destroy();
}

bool Indigo::MainWindow::create(Application* app)
{
	if (m_wnd)
		LOG_ERROR_RETURN(("MainWindow already created"),false);

	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&MainWindow::do_create),&ret) || !ret)
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

void Indigo::MainWindow::do_create(bool* ret_val)
{
	*ret_val = false;

	OOBase::SharedPtr<Render::MainWindow> wnd = OOBase::allocate_shared<Render::MainWindow,OOBase::ThreadLocalAllocator>(this);
	if (!wnd)
		LOG_ERROR(("Failed to allocate MainWindow: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));
	else if (wnd->create())
	{
		wnd.swap(m_wnd);
		*ret_val = true;
	}
}

void Indigo::MainWindow::destroy()
{
	m_top_layer.destroy();

	if (m_wnd)
		render_call(OOBase::make_delegate(this,&MainWindow::do_destroy));

	m_app = NULL;
}

void Indigo::MainWindow::do_destroy()
{
	m_wnd.reset();
}

void Indigo::MainWindow::on_close()
{
	if (m_app)
		m_app->on_main_wnd_close();
}

Indigo::GUI::Layer& Indigo::MainWindow::top_layer()
{
	return m_top_layer;
}

bool Indigo::MainWindow::show()
{
	return render_call(OOBase::make_delegate(this,&MainWindow::do_show));
}

void Indigo::MainWindow::do_show()
{
	m_wnd->on_size(*m_wnd->window(),m_wnd->window()->size());
	m_wnd->window()->visible(true);
}

const OOBase::SharedPtr<OOGL::Window>& Indigo::MainWindow::window() const
{
	return m_wnd->window();
}
