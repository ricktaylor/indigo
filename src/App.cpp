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
#include "Font.h"
#include "GUILabel.h"

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

	/*OOBase::String strZip;
	if (!config_args.find("$1",strZip))
		LOG_ERROR_RETURN(("No zip file"),false);

	ZipResource zip;
	if (!zip.open(strZip.c_str()))
		return false;*/

	return create_mainwnd() && show_menu();
}

bool Indigo::Application::create_mainwnd()
{
	if (!m_main_wnd.create(this))
		return false;

	m_start_menu = OOBase::allocate_shared<GUI::Panel>();
	if (!m_start_menu)
		LOG_ERROR_RETURN(("Failed to create start_menu: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!m_start_menu->create(&m_main_wnd.top_layer()))
		return false;

	OOBase::SharedPtr<GUI::Sizer> sizer = OOBase::allocate_shared<GUI::Sizer>();
	if (!sizer)
		LOG_ERROR_RETURN(("Failed to create sizer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!sizer->create(8,8) ||
			!m_start_menu->sizer(sizer) ||
			!m_start_menu->background(static_resources(),"menu_border.png") ||
			!m_start_menu->borders(9,9,9,9))
	{
		return false;
	}

	OOBase::SharedPtr<Font> font = OOBase::allocate_shared<Font>(m_main_wnd.window());
	if (!font)
		LOG_ERROR_RETURN(("Failed to create font: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!font->load(static_resources(),"Titillium-Regular.fnt"))
		return false;

	GUI::Sizer::ItemLayout layout = {0};
	layout.m_flags = GUI::Sizer::ItemLayout::expand;
	layout.m_proportion = 1;

	for (int i=0;i<10;++i)
	{
		OOBase::SharedPtr<GUI::Label> label = OOBase::allocate_shared<GUI::Label>();
		if (!label)
			LOG_ERROR_RETURN(("Failed to create label: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		OOBase::String str;
		str.printf("Item %d",i);

		if (!label->create(m_start_menu.get(),str,font) ||
			!label->visible(true) ||
			!sizer->add_widget(label,0,i,&layout))
		{
			return false;
		}
	}

	if (!m_start_menu->fit() || !m_start_menu->visible(true))
		return false;

	layout.m_flags = GUI::Sizer::ItemLayout::align_centre;

	if (!m_main_wnd.top_layer().sizer()->add_widget(m_start_menu,0,0,&layout) ||
			!m_main_wnd.top_layer().layout())
		return false;

	return m_main_wnd.show();
}

bool Indigo::Application::show_menu()
{
	return m_main_wnd.top_layer().visible(true);
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
