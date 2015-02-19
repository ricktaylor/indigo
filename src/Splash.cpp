///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#include "Render.h"

static OOBase::SharedPtr<Indigo::Window> s_ptrSplash;
static float ratio;

static void on_window_draw(const Indigo::Window& win, Indigo::State& glState)
{
	glState.bind(GL_FRAMEBUFFER,win.get_default_frame_buffer());

	// Always clear everything
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(-0.6f, -0.4f, 0.f);
	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.6f, -0.4f, 0.f);
	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.6f, 0.f);
	glEnd();
}

static void on_window_close(const Indigo::Window& win)
{
	s_ptrSplash.reset();
}

static bool create_splash(void*)
{
	OOBase::SharedPtr<Indigo::Window> ptrSplash = OOBase::allocate_shared<Indigo::Window,OOBase::ThreadLocalAllocator>(320,200,"Test");
	if (!ptrSplash || !ptrSplash->is_valid())
		return false;

	if (!ptrSplash->signal_close.connect(&on_window_close) ||
			!ptrSplash->signal_draw.connect(&on_window_draw))
		LOG_ERROR_RETURN(("Failed to attach signal"),false);

	glm::ivec2 sz = ptrSplash->size();
	ratio = sz.x / (float)sz.y;
	glViewport(0, 0, sz.x, sz.y);

	if (!Indigo::monitor_window(ptrSplash))
		LOG_ERROR_RETURN(("Failed to monitor window"),false);

	ptrSplash->visible(true);
	s_ptrSplash.swap(ptrSplash);

	return true;
}

static bool close_splash(void*)
{
	s_ptrSplash.reset();
	return true;
}

bool showSplash()
{
	return Indigo::render_call(&create_splash,NULL);
}

bool hideSplash()
{
	return Indigo::render_call(&close_splash,NULL);
}
