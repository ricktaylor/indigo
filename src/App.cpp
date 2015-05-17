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

#include "App.h"
#include "Render.h"
#include "ZipResource.h"

bool showSplash();

Indigo::Application::Application() : m_main_wnd(this)
{
}

bool Indigo::Application::start(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	//if (!showSplash())
	//	return false;

	if (!m_main_wnd.create())
		return false;

	OOBase::String strZip;
	if (!config_args.find("$1",strZip))
		LOG_ERROR_RETURN(("No zip file"),false);

	ZipResource zip;
	if (!zip.open(strZip.c_str()))
		return false;
		
	return true;
}

bool Indigo::Application::run(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	Application app;
	if (!app.start(config_args))
		return false;

	if (!Indigo::handle_events())
		return false;

	OOBase::Logger::log(OOBase::Logger::Information,"Quit");
	return true;
}

void Indigo::Application::on_main_wnd_close()
{
	m_main_wnd.destroy();
}
