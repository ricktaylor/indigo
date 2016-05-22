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

#include "../core/Common.h"

#include "App.h"
#include "WindowChangeDlg.h"

//#include "../core/Thread.h"
#include "../core/ZipResource.h"

//#include "../ui/UIDialog.h"
//#include "../ui/UIButton.h"
//#include "../ui/UIImage.h"
#include "../ui/ImageLayer.h"
//#include "../ui/UISizer.h"
//#include "../ui/UILoader.h"

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

	// TODO Load window params settings
	Window::CreateParams window_params(1024,768);
	if (!m_wnd->create(window_params) &&!m_wnd->create())
	{
		OOBase_CallCriticalFailure("Failed to create main window with safe defaults");
		return;
	}
	
	ZipResource zip;
	if (!zip.open("test.zip"))
	{
		// TODO Error msg
	}
	else
	{
		// This is where the credits movie goes - ;)

		for (;;)
		{
			Indigo::StartDlg::Result res = show_start_dlg(zip,window_params);
			if (res == StartDlg::quit)
				break;

			if (res == StartDlg::new_game)
			{

			}

			if (res == StartDlg::load_game)
			{

			}
		}
	}
	
	m_wnd.reset();
}

Indigo::StartDlg::Result Indigo::Application::show_start_dlg(ResourceBundle& res, Window::CreateParams orig_params)
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
				if (image->load(res,"main.png"))
				{
					img_layer = OOBase::allocate_shared<ImageLayer,OOBase::ThreadLocalAllocator>(image);
					if (!img_layer)
						LOG_ERROR(("Failed to allocate image layer: %s",OOBase::system_error_text()));
					else if (!m_wnd->add_layer(img_layer,50))
						img_layer.reset();
				}
				image.reset();
			}
							
			UILoader loader(m_wnd);
			unsigned int zorder = 100;
			if (!loader.load(res,"ui.txt",zorder))
				return StartDlg::quit;

			if (img_layer)
				img_layer->show();

			if (reinit)
			{
				WindowChangeDlg dlg(OOBase::static_pointer_cast<UIDialog>(loader.find_dialog("window_change")));

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
				StartDlg dlg(loader);

				m_wnd->show();

				Indigo::StartDlg::Result ret = dlg.do_modal(orig_params);
				if (ret != StartDlg::reinit)
					return ret;

				reinit = true;
				new_params = dlg.m_window_params;
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
				LOG_ERROR_RETURN(("Failed to recreate main window with original parameters!"),StartDlg::quit);
		}
	}
	
	return StartDlg::quit;
}
