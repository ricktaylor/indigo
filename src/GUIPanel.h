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
	namespace GUI
	{
		class Panel;
	}

	namespace Render
	{
		namespace GUI
		{
			class Sizer;

			class Panel : public Widget
			{
				friend class Indigo::GUI::Panel;
			public:

			protected:
				virtual bool add_child(const OOBase::SharedPtr<Widget>& child) { return false; }
				virtual void remove_child(const OOBase::SharedPtr<Widget>& child) {}

			private:
				OOBase::SharedPtr<Sizer> m_sizer;
			};
		}
	}

	namespace GUI
	{
		class Sizer : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Sizer>
		{
			friend class Panel;
		public:
			bool create(unsigned int rows = 1, unsigned int columns = 1);
			bool destroy();

			bool fit(Panel& panel);

			bool add_row(unsigned int rows = 1);
			bool add_column(unsigned int columns = 1);

			bool add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column = 0);
			bool remove_widget(unsigned int row, unsigned int column = 0);
			bool remove_widget(const OOBase::SharedPtr<Widget>& widget);

		private:
			OOBase::SharedPtr<Indigo::Render::GUI::Sizer> m_sizer;

			void do_create(bool* ret_val, unsigned int rows, unsigned int columns);
			void do_destroy();
			void do_add_row(unsigned int rows);
			void do_add_column(unsigned int columns);
			void do_fit(bool* ret_val, Widget* panel);
			void do_add_widget(bool* ret_val, OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget, unsigned int row, unsigned int column);
			void do_remove_widget(bool* ret_val, Indigo::Render::GUI::Widget* widget, unsigned int row, unsigned int column);
		};

		class Panel : public Widget
		{
		public:
			bool create(Widget* parent, const glm::u16vec2& pos = glm::u16vec2(0), const glm::u16vec2& min_size = glm::u16vec2(-1));

			const OOBase::SharedPtr<Sizer>& sizer() const;
			bool sizer(const OOBase::SharedPtr<Sizer>& s, bool fit = true);

			bool layout();

		private:
			OOBase::SharedPtr<Sizer> m_sizer;

			void do_sizer(Sizer* s, bool* fit);
			OOBase::SharedPtr<Render::GUI::Widget> create_widget();
		};
	}
}

#endif // INDIGO_GUIPANEL_H_INCLUDED
