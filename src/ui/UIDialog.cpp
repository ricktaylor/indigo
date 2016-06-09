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

#include "../../include/indigo/ui/UIDialog.h"

#include "../../include/indigo/Render.h"

#include "../../include/indigo/ui/UILayer.h"

#include "../Common.h"

Indigo::UIDialog::UIDialog() :
		m_live(false)
{
}

void Indigo::UIDialog::internal_do_modal(Window& wnd, const OOBase::SharedPtr<UILayer>& layer)
{
	if (!layer)
		LOG_WARNING(("No layer assigned to dialog!"));
	else
	{
		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> prev_close = layer->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&UIDialog::on_window_close));

		if (wnd.add_layer(layer))
		{
			layer->show(true);
			wnd.show();

			for (m_live = true;m_live;)
				thread_pipe()->get();

			layer->show(false);
			wnd.remove_layer(layer);
		}

		layer->on_close(prev_close);
	}
}

void Indigo::UIDialog::end_dialog()
{
	m_live = false;
}

