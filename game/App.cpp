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

#include "../src/Common.h"

#include "App.h"

#include "Game.h"
#include "StartDlg.h"
#include "WindowChangeDlg.h"

#include "../src/ZipResource.h"
#include "../src/ui/ImageLayer.h"

Indigo::Application::Application()
{
}

void Indigo::Application::run()
{
	m_wnd = OOBase::allocate_shared<Window,OOBase::ThreadLocalAllocator>();
	if (!m_wnd)
	{
		LOG_ERROR(("Failed to allocate main window: %s",OOBase::system_error_text()));
		OOBase_CallCriticalFailure("Failed to allocate main window");
		return;
	}

	OOBase::CmdArgs::options_t::const_iterator opt = m_options.find("window_caption");
	const Window::CreateParams default_window_params(1024,768,false,opt && !opt->second.empty() ? opt->second.c_str() : "The House");
	Window::CreateParams window_params(default_window_params);

	// TODO Load window params settings
	if (!m_wnd->create(window_params))
	{
		// Reset window params to sensible defaults
		window_params = default_window_params;
		if (!m_wnd->create(window_params))
		{
			OOBase_CallCriticalFailure("Failed to create main window with safe defaults");
			return;
		}
	}
	
	// Find resources
	OOBase::String resources;
	if (m_args.size() > 1)
		resources = m_args[0];
	else
	{
		opt = m_options.find("data_file");
		if (opt)
			resources = opt->second;
	}

	// Load resources
	ZipResource zip;
	if (resources.empty() || !zip.open(resources.c_str()))
	{
		// TODO Error Message
	}
	else
	{
		// This is where the credits movie goes - ;)

		for (;;)
		{
			Game game;
			if (!show_start_dlg(zip,window_params,game))
				break;

			if (!game.run())
				break;
		}
	}
	
	m_wnd.reset();
}

bool Indigo::Application::show_start_dlg(ResourceBundle& res, Window::CreateParams orig_params, Game& game)
{
	Window::CreateParams new_params;
	for (bool reinit = false;;)
	{
		bool reset = false;
		
		// Scope these locals
		{
			OOBase::SharedPtr<ImageLayer> img_layer;
			OOBase::SharedPtr<Image> image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
			if (!image)
				LOG_ERROR(("Failed to allocate image: %s",OOBase::system_error_text()));
			else
			{
				if (image->load(res,"start.png"))
				{
					img_layer = OOBase::allocate_shared<ImageLayer,OOBase::ThreadLocalAllocator>(image);
					if (!img_layer)
						LOG_ERROR(("Failed to allocate image layer 'start.png': %s",OOBase::system_error_text()));
					else if (!m_wnd->add_layer(img_layer))
						img_layer.reset();
				}
				image.reset();
			}
							
			UILoader loader(m_wnd);
			if (!loader.load(res,"start.ui"))
			{
				// TODO Error Message
				return false;
			}

			if (img_layer)
				img_layer->show();

			if (reinit)
			{
				WindowChangeDlg dlg(loader.find_dialog("window_change"));

				m_wnd->show();

				reinit = false;

				if (!dlg.do_modal())
					reset = true;
				else
				{
					// TODO: Save new_params

					orig_params = new_params;
				}
			}

			if (!reset)
			{
				new_params = orig_params;

				StartDlg dlg(loader,new_params,game);

				m_wnd->show();

				Indigo::StartDlg::Result ret = dlg.do_modal();

				if (ret == StartDlg::quit)
					return false;

				if (ret != StartDlg::reinit)
				{
					img_layer->show(false);
					return true;
				}

				reinit = true;
			}
		}

		if (reinit)
		{
			m_wnd->destroy();
			
			if (!m_wnd->create(new_params))
				reset = true;
		}
		
		if (reset)
		{
			m_wnd->destroy();
			
			if (!m_wnd->create(orig_params))
			{
				LOG_ERROR(("Failed to recreate main window with original parameters!"));
				break;
			}
		}
	}
	
	return false;
}
