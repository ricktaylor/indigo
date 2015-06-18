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

#include "LuaAllocator.h"

lua_State* Indigo::Lua::Allocator::lua_newstate()
{
	return ::lua_newstate(&alloc,this);
}

void* Indigo::Lua::Allocator::alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	void* ret = NULL;
	Allocator* pThis = static_cast<Allocator*>(ud);
	if (!nsize)
		pThis->m_allocator.free(ptr);
	else
		ret = pThis->m_allocator.reallocate(ptr,nsize,16);
	return ret;
}

