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
#include "../lib/BufferObject.h"
#include "../lib/VertexArrayObject.h"
#include "../lib/Shader.h"
#include "../lib/Font.h"
#include "../lib/Resource.h"

#include <OOBase/TLSSingleton.h>

#include <glm/gtc/matrix_transform.hpp>

namespace Indigo
{
	namespace detail
	{
		class MainWindowImpl
		{
		public:
			MainWindowImpl(Indigo::MainWindow* parent);

			bool create();

		private:
			Indigo::MainWindow* m_parent;

			OOBase::SharedPtr<OOGL::Window> m_wnd;

			float     m_ratio;
			glm::vec2 m_dpmm;

			void on_close(const OOGL::Window& win);
			void on_draw(const OOGL::Window& win, OOGL::State& glState);
			void on_move(const OOGL::Window& win, const glm::ivec2& pos);
			void on_size(const OOGL::Window& win, const glm::ivec2& sz);
		};
	}
}

Indigo::detail::MainWindowImpl::MainWindowImpl(Indigo::MainWindow* parent) : m_parent(parent), m_ratio(0)
{
}

bool Indigo::detail::MainWindowImpl::create()
{
	unsigned int style = OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated;
	if (Indigo::is_debug())
		style |= OOGL::Window::eWSdebug_context;

	m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(800,600,"Indigo",style);
	if (!m_wnd || !m_wnd->is_valid())
		return false;

	m_wnd->on_close(OOBase::Delegate1<const OOGL::Window&,OOBase::ThreadLocalAllocator>(this,&MainWindowImpl::on_close));
	m_wnd->on_moved(OOBase::Delegate2<const OOGL::Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator>(this,&MainWindowImpl::on_move));
	m_wnd->on_sized(OOBase::Delegate2<const OOGL::Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator>(this,&MainWindowImpl::on_size));
	m_wnd->on_draw(OOBase::Delegate2<const OOGL::Window&,OOGL::State&,OOBase::ThreadLocalAllocator>(this,&MainWindowImpl::on_draw));

	if (Indigo::is_debug())
		OOGL::StateFns::get_current()->enable_logging();

	glClearColor(0.f,0.f,0.f,0.f);

	if (!Indigo::monitor_window(m_wnd))
		LOG_ERROR_RETURN(("Failed to monitor window"),false);

	on_size(*m_wnd,m_wnd->size());
	m_wnd->visible(true);

	return true;
}

void Indigo::detail::MainWindowImpl::on_close(const OOGL::Window& win)
{
	raise_event(OOBase::Delegate0<OOBase::CrtAllocator>(m_parent,&MainWindow::on_close));
}

void Indigo::detail::MainWindowImpl::on_move(const OOGL::Window& win, const glm::ivec2& pos)
{
	on_size(win,win.size());
}

void Indigo::detail::MainWindowImpl::on_size(const OOGL::Window& win, const glm::ivec2& sz)
{
	m_dpmm = win.dots_per_mm();
	m_ratio = (sz.x * m_dpmm.x) / (sz.y * m_dpmm.y);
	glViewport(0, 0, sz.x, sz.y);
}

void Indigo::detail::MainWindowImpl::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	glState.bind(GL_DRAW_FRAMEBUFFER,win.get_default_frame_buffer());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

Indigo::MainWindow::MainWindow()
{
}

Indigo::MainWindow::~MainWindow()
{
	destroy();
}

bool Indigo::MainWindow::create()
{
	if (m_wnd)
		LOG_ERROR_RETURN(("MainWindow already created"),false);

	if (!render_call(&MainWindow::do_create,this))
		return false;

	return (m_wnd != NULL);
}

void Indigo::MainWindow::destroy()
{
	if (m_wnd)
		render_call(&MainWindow::do_destroy,this);
}

bool Indigo::MainWindow::do_create()
{
	OOBase::SharedPtr<detail::MainWindowImpl> wnd = OOBase::allocate_shared<detail::MainWindowImpl,OOBase::ThreadLocalAllocator>(this);
	if (!wnd)
		LOG_ERROR_RETURN(("Failed to allocate MainWindow: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!wnd->create())
		return false;

	m_wnd = wnd;
	return true;
}

bool Indigo::MainWindow::do_destroy()
{
	m_wnd.reset();
	return true;
}

OOBase::Delegate1<const Indigo::MainWindow&,OOBase::ThreadLocalAllocator> Indigo::MainWindow::on_close(const OOBase::Delegate1<const MainWindow&,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate1<const MainWindow&,OOBase::ThreadLocalAllocator> prev = m_on_close;
	m_on_close = delegate;
	return prev;
}

void Indigo::MainWindow::on_close()
{
	m_on_close.invoke(*this);
}
