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

#ifndef INDIGO_MAINWINDOW_H_INCLUDED
#define INDIGO_MAINWINDOW_H_INCLUDED

#include "Common.h"

#include <OOBase/SignalSlot.h>

namespace Indigo
{
	namespace detail
	{
		class MainWindowImpl;
	}

	class MainWindow : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<MainWindow>
	{
		friend class detail::MainWindowImpl;

	public:
		MainWindow();
		~MainWindow();

		bool create();
		void destroy();

	private:
		OOBase::SharedPtr<detail::MainWindowImpl> m_wnd;

		bool do_create();
		bool do_destroy();
		void on_close();
	};
}

#endif // INDIGO_MAINWINDOW_H_INCLUDED
