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

#ifndef INDIGO_LUARESOURCE_H_INCLUDED
#define INDIGO_LUARESOURCE_H_INCLUDED

#include "../src/Resource.h"
#include "Common.h"


namespace Indigo
{
	namespace Lua
	{
		class ResourceLoader
		{
		public:
			ResourceLoader(OOGL::ResourceBundle& res, const char* name);

			int lua_load(lua_State* L, const char* mode = NULL);

		private:
			OOGL::ResourceBundle& m_res;
			const char* m_name;
			size_t m_offset;

			static const char* reader(lua_State* L, void* data, size_t* size);
			const char* read(lua_State* L, size_t& size);
		};
	}
}

#endif // INDIGO_LUARESOURCE_H_INCLUDED
