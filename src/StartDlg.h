///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Rick Taylor
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

#ifndef INDIGO_START_DLG_H_INCLUDED
#define INDIGO_START_DLG_H_INCLUDED

#include "../ui/UILoader.h"

namespace Indigo
{
	class UILoader;
	class Game;

	class StartDlg
	{
	public:
		enum Result
		{
			new_game,
			load_game,
			reinit,
			quit
		};

		StartDlg(UILoader& loader, Window::CreateParams& window_params, Game& game);

		enum Result do_modal();

	private:
		UILoader&                   m_loader;
		Game&                       m_game;
		Window::CreateParams&       m_window_params;
		bool                        m_live;
		Result                      m_result;

		void window_close(const Window& w);
		void on_quit();
		void on_config();
		void on_new_game();
	};
}

#endif // INDIGO_START_DLG_H_INCLUDED
