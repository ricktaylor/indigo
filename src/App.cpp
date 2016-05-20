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
#include "StartDlg.h"

#include "../core/Thread.h"
#include "../core/ZipResource.h"

#include "../ui/UIDialog.h"
#include "../ui/UIButton.h"
#include "../ui/UIImage.h"
#include "../ui/ImageLayer.h"
#include "../ui/UISizer.h"
#include "../ui/UILoader.h"

Indigo::Application::Application()
{
}

void Indigo::Application::run()
{
	ZipResource zip;
	if (!zip.open("test.zip"))
		return;

	if (create_window())
	{
		// Add the book layer
		OOBase::SharedPtr<ImageLayer> img_layer;
		OOBase::SharedPtr<Image> book_image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
		if (!book_image)
			LOG_ERROR(("Failed to allocate image: %s",OOBase::system_error_text()));
		else
		{
			if (book_image->load(zip,"main.png"))
			{
				img_layer = OOBase::allocate_shared<ImageLayer,OOBase::ThreadLocalAllocator>(book_image);
				if (!img_layer)
					LOG_ERROR(("Failed to allocate image layer: %s",OOBase::system_error_text()));
				else if (m_wnd->add_layer(img_layer,50))
					img_layer->show();
			}
		}

		UILoader loader(m_wnd);
		unsigned int zorder = 100;
		if (loader.load(zip,"ui.txt",zorder))
		{
			StartDlg dlg(loader);

			m_wnd->show();

			dlg.do_modal();
		}
	}

	m_wnd.reset();
}

bool Indigo::Application::create_window()
{
	OOBase::SharedPtr<Window> wnd = OOBase::allocate_shared<Window,OOBase::ThreadLocalAllocator>();
	if (!wnd)
		LOG_ERROR_RETURN(("Failed to allocate main window: %s",OOBase::system_error_text()),false);

	if (!wnd->create())
		return false;

	m_wnd.swap(wnd);

	return true;
}
