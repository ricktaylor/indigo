///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 Rick Taylor
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

#ifndef INDIGO_SQUIRREL_H_INCLUDED
#define INDIGO_SQUIRREL_H_INCLUDED

#include "../3rdparty/SQUIRREL3/include/squirrel.h"

namespace Indigo
{
	namespace Squirrel
	{
		class Script : public OOBase::RefCounted
		{
			friend class Environment;
			friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

		public:
			bool call(const char* func, const char* format, ...);

		private:
			Script(HSQUIRRELVM vm, HSQOBJECT handle) : m_refcount(1), m_vm(vm), m_handle(handle)
			{}

			~Script()
			{
				sq_release(m_vm,&m_handle);
			}

			virtual void destroy()
			{
				OOBase::ThreadLocalAllocator::delete_free(this);
			}

			size_t      m_refcount;
			HSQUIRRELVM m_vm;
			HSQOBJECT   m_handle;
		};

		class Environment : public OOBase::NonCopyable
		{
		public:
			Environment(unsigned int stack = 1024);
			~Environment();

			bool load_library(const char* filename);
			OOBase::RefPtr<Script> load_script(const char* filename, unsigned int stack = 1024);

			bool add_class(const char* name, size_t size, const char* base = NULL);

		private:
			HSQUIRRELVM m_vm;
		};
	}
}

#endif // INDIGO_SQUIRREL_H_INCLUDED
