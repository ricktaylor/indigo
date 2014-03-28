
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

#include "../lib/Window.h"

static OOBase::SharedPtr<Indigo::Window> s_ptrSplash;

static void on_window_close()
{
	s_ptrSplash.reset();
}

static bool create_splash(void*)
{
	OOBase::SharedPtr<Indigo::Window> ptrSplash = OOBase::allocate_shared<Indigo::Window,OOBase::ThreadLocalAllocator>(320,200,"Test");
	if (!ptrSplash || !ptrSplash->is_valid())
		return false;

	int err = ptrSplash->signal_close.connect(&on_window_close);
	if (err)
		LOG_ERROR_RETURN(("Failed to attach signal: %s",OOBase::system_error_text(err)),false);

	ptrSplash->visible(true);
	s_ptrSplash.swap(ptrSplash);

	return true;
}

static bool close_splash(void*)
{
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