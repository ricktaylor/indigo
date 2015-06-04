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

namespace Indigo
{
	OOGL::ResourceBundle& static_resources();
}

bool showSplash();

Indigo::Application::Application()
{
}

bool Indigo::Application::start(const OOBase::Table<OOBase::String,OOBase::String>& config_args)
{
	//if (!showSplash())
	//	return false;

	if (!m_main_wnd.create(this))
		return false;

	/*OOBase::String strZip;
	if (!config_args.find("$1",strZip))
		LOG_ERROR_RETURN(("No zip file"),false);

	ZipResource zip;
	if (!zip.open(strZip.c_str()))
		return false;*/

	OOBase::SharedPtr<GUI::Sizer> sizer = OOBase::allocate_shared<GUI::Sizer>();
	if (!sizer)
		LOG_ERROR_RETURN(("Failed to create sizer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	OOBase::SharedPtr<GUI::Panel> start_menu = OOBase::allocate_shared<GUI::Panel>();
	if (!start_menu)
		LOG_ERROR_RETURN(("Failed to create start_menu: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	GUI::Sizer::ItemLayout layout = {0};
	layout.m_flags = GUI::Sizer::ItemLayout::align_centre;
	layout.m_proportion = 1;

	return start_menu->create(&m_main_wnd.top_layer(),glm::u16vec2(48,48)) &&
			sizer->create(8,8) &&
			start_menu->sizer(sizer) &&
			start_menu->background(static_resources(),"menu_border.png") &&
			start_menu->borders(15,15,15,15) &&
			start_menu->visible(true) &&
			m_main_wnd.top_layer().sizer()->add_widget(start_menu,0,0,&layout) &&
			m_main_wnd.top_layer().visible(true) &&
			m_main_wnd.show();
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
	quit_loop();
}
