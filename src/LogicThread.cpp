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
#include "MainWindow.h"

bool showSplash();

static void fire_close(const Indigo::MainWindow& wnd)
{
	LOG_DEBUG(("Quit"));
}

bool logic_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	bool ret = false;

	//if (!showSplash())
	//	return false;

	Indigo::MainWindow wnd;
	if (wnd.create())
	{
		wnd.on_close(OOBase::Delegate1<const Indigo::MainWindow&,OOBase::ThreadLocalAllocator>(&fire_close));

		ret = Indigo::handle_events();

		OOBase::Logger::log(OOBase::Logger::Information,"Quit");
	}

	return ret;
}
