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
#include "../ui/UILayer.h"
#include "../ui/UIButton.h"
#include "../ui/UIImage.h"
#include "../ui/ImageLayer.h"

namespace Indigo
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
	// Add the book layer
	OOBase::SharedPtr<Image> book_image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
	book_image->load(static_resources(),"ui/book.png");
		
	OOBase::SharedPtr<ImageLayer> img_layer = OOBase::allocate_shared<ImageLayer,OOBase::ThreadLocalAllocator>(book_image);
	m_wnd->add_layer(img_layer,50);
	
	OOBase::SharedPtr<UILayer> layer = OOBase::allocate_shared<UILayer,OOBase::ThreadLocalAllocator>();
	m_wnd->add_layer(layer,100);

#if 1
	OOBase::SharedPtr<UIButton::Style> button_style = OOBase::allocate_shared<UIButton::Style,OOBase::ThreadLocalAllocator>();
	button_style->m_background.Image::load(static_resources(),"menu_border.png",4);
	button_style->m_font.load(static_resources(),"Bilbo.fnt");
	button_style->m_colour = glm::vec4(0.f,0.f,0.f,.95f);
	button_style->m_shadow = glm::vec4(.0f,.0f,.0f,.75f);
	button_style->m_drop = glm::ivec2(0,0);

	OOBase::SharedPtr<UIButton> button = OOBase::allocate_shared<UIButton,OOBase::ThreadLocalAllocator>(layer.get(),button_style,"Hello there, very long text.  This is fun!  ",-1,glm::ivec2(100,100));
	layer->add_widget(button,100);

	button_style->m_background.unload();
	button_style->m_font.unload();

	button->show();
#elif 1
	OOBase::SharedPtr<Image> image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
	image->load(static_resources(),"menu_border.png");

	OOBase::SharedPtr<UIImage> piccy = OOBase::allocate_shared<UIImage,OOBase::ThreadLocalAllocator>(image,glm::ivec2(10,100));
	layer->add_widget(piccy,100);

	image->unload();
	piccy->show();
#endif

	img_layer->show();
	layer->show();

	m_wnd->show();
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
