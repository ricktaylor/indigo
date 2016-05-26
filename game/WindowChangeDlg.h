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

#ifndef INDIGO_WND_CHANGE_DLG_H_INCLUDED
#define INDIGO_WND_CHANGE_DLG_H_INCLUDED

#include "../src/ui/UILoader.h"

namespace Indigo
{
	class WindowChangeDlg
	{
	public:
		WindowChangeDlg(const OOBase::SharedPtr<UILayer>& dialog);

		bool do_modal();

	private:
		OOBase::SharedPtr<UILayer> m_dialog;
		bool                        m_live;
		bool                        m_result;

		void window_close(const Window& w);
		void on_yes();
		void on_no();
	};
}

#endif // INDIGO_WND_CHANGE_DLG_H_INCLUDED
