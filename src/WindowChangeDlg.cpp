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

#include "../core/Common.h"

#include "WindowChangeDlg.h"

#include "../core/Thread.h"

Indigo::WindowChangeDlg::WindowChangeDlg(const OOBase::SharedPtr<UIDialog>& dialog) :
		m_dialog(dialog),
		m_live(false),
		m_result(false)
{
	if (!m_dialog)
		LOG_WARNING(("No dialog assigned to window change dialog"));
	else
	{
		OOBase::SharedPtr<UIButton> btn = OOBase::static_pointer_cast<UIButton>(m_dialog->find_widget("yes"));
		if (!btn)
			LOG_WARNING(("No yes button in window change dialog"));
		else
			btn->on_click(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&WindowChangeDlg::on_yes));

		btn = OOBase::static_pointer_cast<UIButton>(m_dialog->find_widget("no"));
		if (!btn)
			LOG_WARNING(("No no button in window change dialog"));
		else
			btn->on_click(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&WindowChangeDlg::on_no));
	}
}

bool Indigo::WindowChangeDlg::do_modal()
{
	if (m_dialog)
	{
		OOBase::Delegate1<void,const Indigo::Window&,OOBase::ThreadLocalAllocator> prev_close = m_dialog->window()->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&WindowChangeDlg::window_close));

		m_dialog->show();

		OOBase::Timeout timeout(10,0);

		for (m_live = true;m_live;)
		{
			thread_pipe()->get();

			if (timeout.has_expired())
				on_no();
		}

		m_dialog->window()->on_close(prev_close);

		m_dialog->show(false);
	}

	return m_result;
}

void Indigo::WindowChangeDlg::window_close(const Window& w)
{
	if (m_dialog->window().get() == &w)
		on_no();
}

void Indigo::WindowChangeDlg::on_no()
{
	if (m_live)
		m_result = false;
	m_live = false;
}

void Indigo::WindowChangeDlg::on_yes()
{
	if (m_live)
		m_result = true;
	m_live = false;
}
