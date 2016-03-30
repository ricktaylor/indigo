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

#include "../core/Thread.h"
#include "../core/ZipResource.h"

#include "../ui/UILayer.h"
#include "../ui/UIButton.h"
#include "../ui/UIImage.h"
#include "../ui/ImageLayer.h"
#include "../ui/UISizer.h"
#include "../ui/UILoader.h"

/*namespace Indigo
{
	namespace Application
	{
		const OOBase::CmdArgs::options_t* m_options;
		const OOBase::CmdArgs::arguments_t* m_args;
		OOBase::SharedPtr<Window> m_wnd;

		void splash();
	}
}

void Indigo::Application::splash()
{

}

void Indigo::Application::start(OOBase::SharedPtr<Window> wnd, const OOBase::CmdArgs::options_t* options, const OOBase::CmdArgs::arguments_t* args)
{
	m_options = options;
	m_args = args;
	m_wnd = wnd;

	splash();
}

void Indigo::Application::on_quit()
{
	m_wnd.reset();
}

void Indigo::Application::stop()
{
	thread_pipe()->close();
}
*/

void Indigo::Application::run()
{
	m_stop = false;

	OOBase::SharedPtr<Window> wnd = OOBase::allocate_shared<Window,OOBase::ThreadLocalAllocator>();
	if (wnd && wnd->create())
	{
		wnd->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Application::window_close));

		ZipResource zip;
		if (zip.open("test.zip"))
		{
			// Add the book layer
			OOBase::SharedPtr<Image> book_image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
			if (book_image)
			{
				book_image->load(zip,"book.png");

				OOBase::SharedPtr<ImageLayer> img_layer = OOBase::allocate_shared<ImageLayer,OOBase::ThreadLocalAllocator>(book_image);
				if (img_layer && wnd->add_layer(img_layer,50))
					img_layer->show();
			}

			UILoader loader(wnd,zip);

			unsigned int zorder = 100;
			if (loader.load("ui.txt",zorder))
				wnd->show();
		}
	}

	while (!m_stop)
		thread_pipe()->get();

	wnd->destroy();
}

void Indigo::Application::window_close(const Window& w)
{
	m_stop = true;
}
