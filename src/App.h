///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#ifndef INDIGO_APP_H_INCLUDED
#define INDIGO_APP_H_INCLUDED

#include "../core/Window.h"

namespace Indigo
{
	class UILoader;

	class Application
	{
	public:
		Application();

		OOBase::CmdArgs::options_t m_options;
		OOBase::CmdArgs::arguments_t m_args;

		void run();

	private:
		OOBase::SharedPtr<Window> m_wnd;

		bool create_window();
	};
}

#endif // INDIGO_APP_H_INCLUDED
