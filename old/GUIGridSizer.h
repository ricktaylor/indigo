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

#ifndef INDIGO_GUIGRIDSIZER_H_INCLUDED
#define INDIGO_GUIGRIDSIZER_H_INCLUDED

#include "../old/GUISizer.h"

namespace Indigo
{
	namespace GUI
	{
		class GridSizer : public Sizer
		{
			friend class Panel;

		public:
			GridSizer();
			~GridSizer();

			bool create(OOBase::uint16_t hspace, OOBase::uint16_t vspace);
			bool create(OOBase::uint16_t hspace, OOBase::uint16_t vspace, const Sizer::ItemLayout& default_layout);

			bool add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column = 0, const Sizer::ItemLayout* layout = NULL);
			bool add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column = 0, const Sizer::ItemLayout* layout = NULL);

			bool remove_item(unsigned int row, unsigned int column);
			bool remove_widget(const OOBase::SharedPtr<Widget>& widget);

		private:
			virtual OOBase::SharedPtr<Indigo::Render::GUI::Sizer> create_render_sizer();

			void do_add_widget(bool* ret_val, const OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget, unsigned int row, unsigned int column, const Sizer::ItemLayout* layout);
			void do_add_spacer(bool* ret_val, OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>* pos, OOBase::Pair<unsigned int,unsigned int>* idx, const Sizer::ItemLayout* layout);
			void do_remove_widget(bool* ret_val, const OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget);
			void do_remove_item(bool* ret_val, unsigned int row, unsigned int column);
		};
	}
}

#endif // INDIGO_GUIGRIDSIZER_H_INCLUDED
