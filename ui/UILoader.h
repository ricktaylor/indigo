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

#ifndef INDIGO_UILOADER_H_INCLUDED
#define INDIGO_UILOADER_H_INCLUDED

#include "UILayer.h"

namespace Indigo
{
	class ResourceBundle;

	class UILoader
	{
	public:
		UILoader(ResourceBundle& resource) : m_resource(resource)
		{}

		bool load(const char* name, UIGroup* parent);
		
		struct error_pos_t
		{
			size_t m_line;
			size_t m_col;
		};
		error_pos_t error_pos() const { return m_error_pos; }

	private:
		ResourceBundle& m_resource;
		error_pos_t m_error_pos;

		typedef OOBase::HashTable<const char*,OOBase::SharedPtr<UIWidget>,OOBase::ThreadLocalAllocator> widget_hash_t;
		widget_hash_t m_hashWidgets;

		void skip_whitespace(const char*& p, const char* pe);
		void skip_to(char to, const char*& p, const char* pe);
		const char*& inc_p(const char*& p);
		bool character(const char*& p, const char* pe, char c);
		bool ident(const char*& p, const char* pe, OOBase::ScopedString& i);
		bool type_name(const char*& p, const char* pe, OOBase::ScopedString& i);
		bool parse_uint(const char*& p, const char* pe, unsigned int& u);
		bool parse_uvec2(const char*& p, const char* pe, glm::uvec2& u);

		OOBase::SharedPtr<Indigo::UIWidget> load_top_level(const char*& p, const char* pe, UIGroup* parent);
		OOBase::SharedPtr<Indigo::UIWidget> load_layer(const char*& p, const char* pe);
		bool load_children(const char*& p, const char* pe, UIGroup* parent, const char* parent_name);
		OOBase::SharedPtr<Indigo::UIWidget> load_child(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent, const char* parent_name);

		bool load_grid_sizer(const char*& p, const char* pe, UIGroup* parent, const char* parent_name);
	};
}

#endif // INDIGO_UILOADER_H_INCLUDED
