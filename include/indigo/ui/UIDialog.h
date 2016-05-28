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

namespace Indigo
{
	class UIDialog
	{
	protected:
		UIDialog(const OOBase::SharedPtr<UILayer>& layer);

		void internal_do_modal();

		void end_dialog();

		virtual void on_window_close() = 0;

	private:
		OOBase::SharedPtr<UILayer> m_layer;
		bool                       m_live;

		void window_close(const Window& w);
	};
}

#endif // INDIGO_UIDIALOG_H_INCLUDED
