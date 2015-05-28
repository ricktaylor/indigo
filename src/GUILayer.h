///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
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

#ifndef INDIGO_GUILAYER_H_INCLUDED
#define INDIGO_GUILAYER_H_INCLUDED

#include "GUIPanel.h"

namespace Indigo
{
	namespace Render
	{
		class MainWindow;
	}

	namespace GUI
	{
		class Layer : public Panel
		{
		public:
			bool create(OOBase::SharedPtr<Render::MainWindow>& wnd);

		private:
			void do_create(bool* ret_val, OOBase::SharedPtr<Render::MainWindow>* wnd);
			OOBase::SharedPtr<Render::GUI::Widget> create_widget();

			glm::u16vec2 ideal_size() const;
		};
	}
}

#endif // INDIGO_GUILAYER_H_INCLUDED
