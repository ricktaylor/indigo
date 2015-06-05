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

#ifndef INDIGO_FONT_H_INCLUDED
#define INDIGO_FONT_H_INCLUDED

#include "Common.h"

#include "../lib/Font.h"
#include "../lib/Window.h"

namespace Indigo
{
	class Font : public OOBase::NonCopyable
	{
	public:
		Font(const OOBase::SharedPtr<OOGL::Window>& wnd);
		~Font();

		bool load(const OOGL::ResourceBundle& resource, const char* name);
		bool destroy();

		const OOBase::SharedPtr<OOGL::Font>& render_font() const;

	private:
		OOBase::SharedPtr<OOGL::Window> m_wnd;
		OOBase::SharedPtr<OOGL::Font>   m_font;

		void do_load(bool* ret_val, const OOGL::ResourceBundle* resource, const char* name);
		void do_destroy();
	};
}

#endif // INDIGO_IMAGE_H_INCLUDED
