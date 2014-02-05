///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 Rick Taylor
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
#include "Window.h"
#include "Framebuffer.h"

static OOBase::SharedPtr<Indigo::Window> s_ptrSplash;

static void on_window_close()
{
	s_ptrSplash.reset();
}

static bool render_start(void*)
{
	// Set defaults
	glfwDefaultWindowHints();

	OOBase::SharedPtr<Indigo::Window> ptrSplash = OOBase::allocate_shared<Indigo::Window,OOBase::ThreadLocalAllocator>(320,200,"Test");
	if (!ptrSplash)
		return false;

	int err = ptrSplash->signal_close.connect(&on_window_close);
	if (err)
		LOG_ERROR_RETURN(("Failed to attach signal: %s",OOBase::system_error_text(err)),false);

	ptrSplash->visible(true);
	s_ptrSplash.swap(ptrSplash);
	return true;
}

bool logic_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	// Set up graphics first
	if (!Indigo::render_call(&render_start,NULL))
		return false;

	if (!Indigo::handle_events())
		return false;

	OOBase::Logger::log(OOBase::Logger::Information,"Quit");

	return true;
}
