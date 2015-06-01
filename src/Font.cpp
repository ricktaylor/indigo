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

#include "Font.h"
#include "Render.h"

Indigo::Font::~Font()
{
	destroy();
}

bool Indigo::Font::load(const OOGL::ResourceBundle& resource, const char* name)
{
	if (m_font)
		return false;

	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Indigo::Font::do_load),&ret,&resource,name) && ret;
}

void Indigo::Font::do_load(bool* ret_val, const OOGL::ResourceBundle* resource, const char* name)
{
	OOBase::SharedPtr<OOGL::Font> font = OOBase::allocate_shared<OOGL::Font>();
	if (!font)
	{
		LOG_ERROR(("Failed to allocate render font: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));
		*ret_val = false;
	}
	else
		*ret_val = font->load(*resource,name);

	if (*ret_val)
		font.swap(m_font);
}

bool Indigo::Font::destroy()
{
	return !m_font || render_call(OOBase::make_delegate(this,&Indigo::Font::do_destroy));
}

void Indigo::Font::do_destroy()
{
	m_font.reset();
}

OOBase::SharedPtr<OOGL::Font> Indigo::Font::render_font() const
{
	return m_font;
}
