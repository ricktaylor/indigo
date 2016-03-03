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
	OOBase::SharedPtr<Indigo::UILayer> layer = OOBase::allocate_shared<Indigo::UILayer,OOBase::ThreadLocalAllocator>();
	m_wnd->add_layer(layer,100);

	OOBase::SharedPtr<Indigo::UIButton::Style> button_style = OOBase::allocate_shared<Indigo::UIButton::Style,OOBase::ThreadLocalAllocator>();
	button_style->m_background.Image::load(Indigo::static_resources(),"menu_border.png");
	button_style->m_font.load(Indigo::static_resources(),"Blokletters.fnt");
	button_style->m_colour = glm::vec4(1.f,1.f,1.f,1.f);
	button_style->m_shadow = glm::vec4(.0f,.0f,.0f,.75f);
	button_style->m_drop = glm::ivec2(0,-1);

	OOBase::SharedPtr<Indigo::UIButton> button = OOBase::allocate_shared<Indigo::UIButton,OOBase::ThreadLocalAllocator>(button_style,"Hello",-1,glm::ivec2(100,100));
	layer->add_widget(button,100);

	button_style->m_background.unload();
	button_style->m_font.unload();

	layer->show(true);
}

void Indigo::Application::start(OOBase::SharedPtr<Window> wnd, const OOBase::CmdArgs::options_t* options, const OOBase::CmdArgs::arguments_t* args)
{
	m_options = options;
	m_args = args;
	m_wnd = wnd;

	// Add a simple white blanking layer
	m_wnd->add_layer(OOBase::allocate_shared<Indigo::BlankingLayer,OOBase::ThreadLocalAllocator>(glm::vec4(1.f)),0);
	m_wnd->visible(true);

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

