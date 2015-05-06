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
	OOGL::ResourceBundle& static_resources();
}

namespace
{
	class MainWindowImpl : public OOBase::EnableSharedFromThis<MainWindowImpl>
	{
	public:
		MainWindowImpl();

		bool create(Indigo::MainWindow* parent, void (*close_callback)(Indigo::MainWindow*));

	private:
		Indigo::MainWindow*             m_parent;
		void (*m_close_callback)(Indigo::MainWindow*);

		OOBase::SharedPtr<OOGL::Window> m_wnd;

		float     m_ratio;
		glm::vec2 m_dpmm;

		void on_close(const OOGL::Window& win);
		void on_draw(const OOGL::Window& win, OOGL::State& glState);
		void on_move(const OOGL::Window& win, const glm::ivec2& pos);
		void on_size(const OOGL::Window& win, const glm::ivec2& sz);
	};

	OOBase::SharedPtr<MainWindowImpl>& main_window()
	{
		return OOBase::TLSSingleton<OOBase::SharedPtr<MainWindowImpl> >::instance();
	}

	struct create_info
	{
		Indigo::MainWindow* parent;
		void (*close_callback)(Indigo::MainWindow*);
	};

	bool create_main_window(void*);
	bool destroy_main_window(void*);
}

MainWindowImpl::MainWindowImpl() : m_ratio(0)
{
}

bool MainWindowImpl::create(Indigo::MainWindow* parent, void (*close_callback)(Indigo::MainWindow*))
{
	m_parent = parent;
	m_close_callback = close_callback;

	unsigned int style = OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated;
	if (Indigo::is_debug())
		style |= OOGL::Window::eWSdebug_context;

	m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(800,600,"Indigo",style);
	if (!m_wnd || !m_wnd->is_valid())
		return false;

	if (!m_wnd->signal_close.connect(OOBase::WeakPtr<MainWindowImpl>(shared_from_this()),&MainWindowImpl::on_close) ||
		!m_wnd->signal_moved.connect(OOBase::WeakPtr<MainWindowImpl>(shared_from_this()),&MainWindowImpl::on_move) ||
		!m_wnd->signal_sized.connect(OOBase::WeakPtr<MainWindowImpl>(shared_from_this()),&MainWindowImpl::on_size) ||
		!m_wnd->signal_draw.connect(OOBase::WeakPtr<MainWindowImpl>(shared_from_this()),&MainWindowImpl::on_draw))
	LOG_ERROR_RETURN(("Failed to attach signal"),false);

	if (Indigo::is_debug())
		OOGL::StateFns::get_current()->enable_logging();

	glClearColor(0.f,0.f,0.f,0.f);

	if (!Indigo::monitor_window(m_wnd))
		LOG_ERROR_RETURN(("Failed to monitor window"),false);

	on_size(*m_wnd,m_wnd->size());
	m_wnd->visible(true);

	return true;
}

void MainWindowImpl::on_close(const OOGL::Window& win)
{
	Indigo::raise_event(m_close_callback,m_parent);
}

void MainWindowImpl::on_move(const OOGL::Window& win, const glm::ivec2& pos)
{
	on_size(win,win.size());
}

void MainWindowImpl::on_size(const OOGL::Window& win, const glm::ivec2& sz)
{
	m_dpmm = win.dots_per_mm();
	m_ratio = (sz.x * m_dpmm.x) / (sz.y * m_dpmm.y);
	glViewport(0, 0, sz.x, sz.y);
}

void MainWindowImpl::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	glState.bind(GL_DRAW_FRAMEBUFFER,win.get_default_frame_buffer());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

bool create_main_window(void* p)
{
	OOBase::SharedPtr<MainWindowImpl> wnd = OOBase::allocate_shared<MainWindowImpl,OOBase::ThreadLocalAllocator>();
	if (!wnd)
		LOG_ERROR_RETURN(("Failed to allocate MainWindow: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	struct create_info* ci = static_cast<struct create_info*>(p);
	if (!wnd->create(ci->parent,ci->close_callback))
		return false;

	main_window() = wnd;
	return true;
}

bool destroy_main_window(void*)
{
	main_window().reset();
	return true;
}

Indigo::MainWindow::MainWindow() : m_live(false)
{
}

Indigo::MainWindow::~MainWindow()
{
	destroy();
}

bool Indigo::MainWindow::create()
{
	if (m_live)
		LOG_ERROR_RETURN(("MainWindow already created"),false);

	struct create_info ci = { this, &MainWindow::on_close };
	return (m_live = Indigo::render_call(&::create_main_window,&ci));
}

void Indigo::MainWindow::destroy()
{
	if (m_live)
	{
		if (Indigo::render_call(&::destroy_main_window,NULL))
			m_live = false;
	}
}

void Indigo::MainWindow::on_close(MainWindow* pThis)
{
	pThis->signal_close.fire(*pThis);
}
