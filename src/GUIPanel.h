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

#ifndef INDIGO_GUIPANEL_H_INCLUDED
#define INDIGO_GUIPANEL_H_INCLUDED

#include "GUIWidget.h"

namespace Indigo
{
	namespace Render
	{
		namespace GUI
		{
			class Panel : public Indigo::Render::GUI::Widget
			{
			public:
				bool create();

			protected:
				virtual bool add_child(const OOBase::SharedPtr<Widget>& child) { return false; }
				virtual void remove_child(const OOBase::SharedPtr<Widget>& child) {}

			private:				
			};
		}
	}

	namespace GUI
	{
		class Panel : public Widget
		{
		public:
			bool create(Widget* parent);

		private:
			bool do_create();
			OOBase::SharedPtr<Render::GUI::Widget> create_widget();
		};
	}
}

#endif // INDIGO_GUIPANEL_H_INCLUDED
