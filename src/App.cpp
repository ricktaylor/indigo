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

#include "Common.h"
#include "App.h"
#include "Thread.h"
#include "UILayer.h"

static void splash()
{
	OOBase::SharedPtr<Indigo::UILayer> layer = OOBase::allocate_shared<Indigo::UILayer,OOBase::ThreadLocalAllocator>();

	Indigo::APP::instance().m_wnd->add_layer(layer,100);

	Indigo::APP::instance().m_wnd->visible(true);
}

void Indigo::Application::start(OOBase::SharedPtr<Window> wnd, const OOBase::CmdArgs::options_t* options, const OOBase::CmdArgs::arguments_t* args)
{
	m_options = options;
	m_args = args;
	m_wnd = wnd;

	splash();
}

void Indigo::Application::stop()
{

	thread_pipe()->close();
}

