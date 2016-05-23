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

#include "StartDlg.h"
#include "QuitDlg.h"

#include "../core/Thread.h"

Indigo::StartDlg::StartDlg(UILoader& loader) :
		m_loader(loader),
		m_live(false),
		m_result(StartDlg::quit)
{
}

Indigo::StartDlg::Result Indigo::StartDlg::do_modal(const Window::CreateParams& window_params)
{
	m_window_params = window_params;

	OOBase::SharedPtr<UIDialog> dialog = m_loader.find_dialog("start");
	if (!dialog)
		LOG_WARNING_RETURN(("No dialog assigned to quit dialog"),StartDlg::quit);

	OOBase::Delegate1<void,const Indigo::Window&,OOBase::ThreadLocalAllocator> prev_close = dialog->window()->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&StartDlg::window_close));

	OOBase::SharedPtr<UIButton> btn = OOBase::static_pointer_cast<UIButton>(dialog->find_widget("quit"));
	if (!btn)
		LOG_WARNING(("No quit button in quit dialog"));
	else
		btn->on_click(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&StartDlg::on_quit));
	
	btn = OOBase::static_pointer_cast<UIButton>(dialog->find_widget("config"));
	if (btn)
		btn->on_click(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&StartDlg::on_config));

	dialog->show();

	for (m_live = true;m_live;)
		thread_pipe()->get();

	dialog->window()->on_close(prev_close);

	dialog->show(false);

	return m_result;
}

void Indigo::StartDlg::window_close(const Window& w)
{
	if (m_loader.find_dialog("start")->window().get() == &w)
		on_quit();
}

void Indigo::StartDlg::on_quit()
{
	if (m_live)
	{
		QuitDlg dlg(OOBase::static_pointer_cast<UIDialog>(m_loader.find_dialog("quit")));

		if (dlg.do_modal())
		{
			m_result = StartDlg::quit;
			m_live = false;
		}
	}
}

void Indigo::StartDlg::on_config()
{
	if (m_live)
	{
		m_window_params = Window::CreateParams();

		m_result = StartDlg::reinit;
		m_live = false;
	}
}
