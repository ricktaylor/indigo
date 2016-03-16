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

#include "../oogl/OOGL.h"

namespace Indigo
{
	class ResourceBundle
	{
	public:
		virtual ~ResourceBundle()
		{}

		template <typename T, typename Allocator>
		OOBase::SharedPtr<T> load(const char* name, size_t length = 0) const
		{
			return load<T,Allocator>(name,0,length);
		}

		template <typename T, typename Allocator>
		OOBase::SharedPtr<T> load(const char* name, size_t start, size_t length = 0) const
		{
			if (!length)
				length = size(name);

			OOBase::SharedPtr<T> ret;
			if (length)
			{
				T* p = static_cast<T*>(Allocator::allocate(length,16));
				if (!p)
					LOG_ERROR_RETURN(("Failed to allocate: %s\n",OOBase::system_error_text()),OOBase::SharedPtr<T>());

				ret = OOBase::make_shared<T,Allocator>(p);
				if (!ret)
				{
					LOG_ERROR(("Failed to allocate: %s\n",OOBase::system_error_text()));
					Allocator::free(p);
				}
				else if (!load(p,name,start,length))
					ret.reset();
			}
			return ret;
		}

		bool load(void* dest, const char* name, size_t length = 0) const
		{
			return load(dest,name,0,length);
		}

		virtual bool load(void* dest, const char* name, size_t start, size_t length = 0) const = 0;
		virtual OOBase::uint64_t size(const char* name) const = 0;
		virtual bool exists(const char* name) const = 0;
	};

	ResourceBundle& static_resources();
}

#endif // OOGL_RESOURCE_H_INCLUDED
