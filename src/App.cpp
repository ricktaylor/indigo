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

Indigo::Application::Application() :
	m_state(eAS_None)
{
}

void Indigo::Application::run()
{
	if (create_window())
	{
		while (m_state != eAS_Quit)
			thread_pipe()->get();
	}

	m_wnd.reset();
}

bool Indigo::Application::raise_event(enum Events event)
{
	static const struct Transition
	{
		bool (Application::*m_fn)();
		enum States m_next;
	}
	s_transitions[eAS_Max][eAE_Max] =
	{
		{ // eAS_None
			{ &Application::start_screen,eAS_MainPage } // eAE_WndCreated
		},
		{ // eAS_MainPage
			{ NULL },  // eAE_WndCreated
			{ NULL,eAS_QuitPrompt } // eAE_WndClose
		},
		{ // eAS_QuitPrompt
			{ NULL },  // eAE_WndCreated
			{ NULL,eAS_Quit } // eAE_WndClose
		}
	};

	const struct Transition* trans = &s_transitions[m_state][event];
	if (trans->m_next != 0)
		m_state = trans->m_next;

	return !trans->m_fn || (this->*(trans->m_fn))();
}

void Indigo::Application::window_close(const Window& w)
{
	if (m_wnd.get() == &w)
		raise_event(eAE_WndClose);
}

bool Indigo::Application::create_window()
{
	OOBase::SharedPtr<Window> wnd = OOBase::allocate_shared<Window,OOBase::ThreadLocalAllocator>();
	if (!wnd || !wnd->create())
		return false;

	wnd->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Application::window_close));

	m_wnd.swap(wnd);

	if (!raise_event(eAE_WndCreated))
		m_wnd.swap(wnd);

	return m_wnd->show();
}

bool Indigo::Application::start_screen()
{
	ZipResource zip;
	if (!zip.open("test.zip"))
		return false;

	// Add the book layer
	OOBase::SharedPtr<Image> book_image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
	if (!book_image)
		return false;

	if (!book_image->load(zip,"book.png"))
		return false;

	OOBase::SharedPtr<ImageLayer> img_layer = OOBase::allocate_shared<ImageLayer,OOBase::ThreadLocalAllocator>(book_image);
	if (!img_layer || !m_wnd->add_layer(img_layer,50))
		return false;

	img_layer->show();

	unsigned int zorder = 100;
	return UILoader(m_wnd,zip).load("ui.txt",zorder);
}
