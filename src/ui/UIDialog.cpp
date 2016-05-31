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

#include "../Common.h"

#include "../../include/indigo/Render.h"

#include "../../include/indigo/ui/UIDialog.h"
#include "../../include/indigo/ui/UILayer.h"

Indigo::UIDialog::UIDialog(const OOBase::SharedPtr<UILayer>& layer) :
		m_layer(layer),
		m_live(false)
{
	if (!m_layer)
		LOG_WARNING(("No layer assigned to quit dialog"));


}

void Indigo::UIDialog::internal_do_modal()
{
	if (m_layer)
	{
		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> prev_close = m_layer->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&UIDialog::on_window_close));

		m_layer->show();

		for (m_live = true;m_live;)
			thread_pipe()->get();

		m_layer->on_close(prev_close);

		m_layer->show(false);
	}
}

void Indigo::UIDialog::end_dialog()
{
	m_live = false;
}

