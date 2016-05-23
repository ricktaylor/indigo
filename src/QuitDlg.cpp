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

#include "QuitDlg.h"

#include "../core/Thread.h"

Indigo::QuitDlg::QuitDlg(const OOBase::SharedPtr<UIDialog>& dialog) :
		m_dialog(dialog),
		m_live(false),
		m_result(true)
{
	if (!m_dialog)
		LOG_WARNING(("No dialog assigned to quit dialog"));
	else
	{
		OOBase::SharedPtr<UIButton> btn = OOBase::static_pointer_cast<UIButton>(m_dialog->find_widget("quit"));
		if (!btn)
			LOG_WARNING(("No quit button in quit dialog"));
		else
			btn->on_click(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&QuitDlg::on_quit));

		btn = OOBase::static_pointer_cast<UIButton>(m_dialog->find_widget("cancel"));
		if (!btn)
			LOG_WARNING(("No cancel button in quit dialog"));
		else
			btn->on_click(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&QuitDlg::on_cancel));
	}
}

bool Indigo::QuitDlg::do_modal()
{
	if (m_dialog)
	{
		OOBase::Delegate1<void,const Indigo::Window&,OOBase::ThreadLocalAllocator> prev_close = m_dialog->window()->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&QuitDlg::window_close));

		m_dialog->show();

		for (m_live = true;m_live;)
			thread_pipe()->get();

		m_dialog->window()->on_close(prev_close);

		m_dialog->show(false);
	}

	return m_result;
}

void Indigo::QuitDlg::window_close(const Window& w)
{
	if (m_dialog->window().get() == &w)
		on_quit();
}

void Indigo::QuitDlg::on_quit()
{
	if (m_live)
		m_result = true;
	m_live = false;
}

void Indigo::QuitDlg::on_cancel()
{
	if (m_live)
		m_result = false;
	m_live = false;
}
