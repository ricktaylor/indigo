///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// OOGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OOGL.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OOGL_RESOURCE_H_INCLUDED
#define OOGL_RESOURCE_H_INCLUDED

#include "OOGL.h"

namespace OOGL
{
	class ResourceBundle
	{
	public:
		virtual ~ResourceBundle()
		{}

		const void* load(const char* name, size_t length = size_t(-1))
		{
			return load(name,0,length);
		}

		virtual const void* load(const char* name, size_t start, size_t length = size_t(-1)) = 0;
		virtual OOBase::uint64_t size(const char* name) = 0;
		virtual bool exists(const char* name) = 0;
	};
}

#endif // OOGL_RESOURCE_H_INCLUDED
