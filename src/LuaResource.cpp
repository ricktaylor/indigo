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

#include "LuaResource.h"

Indigo::Lua::ResourceLoader::ResourceLoader(OOGL::ResourceBundle& res, const char* name) :
		m_res(res),
		m_name(name),
		m_offset(0)
{
}

int Indigo::Lua::ResourceLoader::lua_load(lua_State* L, const char* mode)
{
	return ::lua_load(L,&reader,this,m_name,mode);
}

const char* Indigo::Lua::ResourceLoader::reader(lua_State* L, void* data, size_t* size)
{
	return static_cast<ResourceLoader*>(data)->read(L,*size);
}

const char* Indigo::Lua::ResourceLoader::read(lua_State* L, size_t& size)
{
	const char* ret = NULL;
	size_t len = m_res.size(m_name);
	if (len)
	{
		size = len - m_offset;
		if (size)
			ret = static_cast<const char*>(m_res.load(m_name,m_offset,len - m_offset));

		m_offset += size;
	}
	return ret;
}
