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

#ifndef INDIGO_SGMESHLOADER_H_INCLUDED
#define INDIGO_SGMESHLOADER_H_INCLUDED

#include <OOBase/String.h>

#include "SGNode.h"

namespace Indigo
{
	class Image;
	class ResourceBundle;

	class SGMeshLoader
	{
	public:
		SGMeshLoader()
		{}

		~SGMeshLoader();

		bool load(const OOBase::SharedPtr<ResourceBundle>& resource, const char* resource_name);
		void unload();

		OOBase::SharedPtr<SGNode> find_mesh(const char* name, size_t len = -1) const;

	private:
		typedef OOBase::HashTable<size_t,OOBase::SharedPtr<SGNode>,OOBase::ThreadLocalAllocator> mesh_hash_t;
		mesh_hash_t m_hashMeshes;

		typedef OOBase::HashTable<size_t,OOBase::SharedPtr<Image>,OOBase::ThreadLocalAllocator> image_hash_t;
		image_hash_t m_hashImages;

		OOBase::SharedPtr<ResourceBundle> m_resource;

		struct error_pos_t
		{
			unsigned int m_line;
			unsigned int m_col;
		} m_error_pos;

		void syntax_error(const char* fmt, ...) OOBASE_FORMAT(printf,2,3);
		void skip_whitespace(const char*& p, const char* pe);
		const char*& inc_p(const char*& p, const char* pe);
		bool character(const char*& p, const char* pe, char c);
		bool parse_string(const char*& p, const char* pe, OOBase::ScopedString& s);
		bool parse_float(const char*& p, const char* pe, float& i);
		bool command(const char*& p, const char* pe, OOBase::ScopedString& i);

		bool load_command(const char*& p, const char* pe, const OOBase::ScopedString& cmd);

		OOBase::SharedPtr<ResourceBundle> cd_resource(const char* res_name, OOBase::ScopedString& filename);
	};
}

#endif // INDIGO_SGMESHLOADER_H_INCLUDED