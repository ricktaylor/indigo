///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// OOGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OOGL.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef INDIGO_UIDIALOG_H_INCLUDED
#define INDIGO_UIDIALOG_H_INCLUDED

#include "UILayer.h"
#include "../Window.h"

namespace Indigo
{
	class UIDialog
	{
	public:
		UIDialog();

		void internal_do_modal(Window& wnd, const OOBase::SharedPtr<UILayer>& layer);

	protected:
		void end_dialog();

		virtual void on_window_close() { m_live = false; }

	private:
		bool m_live;
	};
}

#endif // INDIGO_UIDIALOG_H_INCLUDED
