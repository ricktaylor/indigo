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

#include "../oogl/include/OOGL/OOGL.h"

namespace Indigo
{
	class ResourceBundle
	{
	public:
		virtual ~ResourceBundle()
		{}

		template <typename T>
		OOBase::SharedPtr<const T> load(const char* name) const
		{
			return OOBase::reinterpret_pointer_cast<const T,const char>(load_i(name));
		}

		virtual OOBase::uint64_t size(const char* name) const = 0;
		virtual bool exists(const char* name) const = 0;

	protected:
		virtual OOBase::SharedPtr<const char> load_i(const char* name) const = 0;
	};

	ResourceBundle& static_resources();
}

#endif // OOGL_RESOURCE_H_INCLUDED
