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

#include "Common.h"
#include "Squirrel.h"

void* sq_vm_malloc(SQUnsignedInteger size)
{
	void* p = OOBase::ThreadLocalAllocator::allocate(size, 16);
	if (!p)
		LOG_ERROR(("Failed to allocate squirrel memory"));
	return p;
}

void* sq_vm_realloc(void* p, SQUnsignedInteger oldsize,	SQUnsignedInteger size)
{
	p = OOBase::ThreadLocalAllocator::reallocate(p, size, 16);
	if (!p)
		LOG_ERROR(("Failed to reallocate squirrel memory"));
	return p;
}

void sq_vm_free(void* p, SQUnsignedInteger size)
{
	OOBase::ThreadLocalAllocator::free(p);
}

static void on_compiler_error(HSQUIRRELVM, const SQChar* desc,const SQChar* source, SQInteger line, SQInteger column)
{
	OOBase::Logger::log(OOBase::Logger::Error,"Script compilation error: %s in %s at line "_PRINT_INT_FMT", column "_PRINT_INT_FMT,desc,source,line,column);
}

static void on_error(HSQUIRRELVM, const SQChar* msg, ...)
{
	va_list args;
	va_start(args,msg);

	OOBase::StackAllocator<512> allocator;
	OOBase::TempPtr<char> ptr(allocator);
	if (OOBase::temp_vprintf(ptr,msg,args) == 0)
		OOBase::Logger::log(OOBase::Logger::Error,"Script runtime error: %s",static_cast<char*>(ptr));
	else
		OOBase::Logger::log(OOBase::Logger::Error,"Script runtime error: %s",msg);

	va_end(args);
}

static void on_print(HSQUIRRELVM, const SQChar* msg, ...)
{
	va_list args;
	va_start(args,msg);

	OOBase::Logger::log(OOBase::Logger::Information,msg,args);

	va_end(args);
}

static SQInteger on_file_read(SQUserPointer p, SQUserPointer dest, SQInteger size)
{
	SQInteger r = static_cast<OOBase::File*>(p)->read(dest,size);
	if (r != 0)
		return r;
	return -1;
}

static bool load_file(HSQUIRRELVM vm, const char* filename)
{
	OOBase::File file;
	int err = file.open(filename,false);
	if (err)
		LOG_ERROR_RETURN(("Failed to open file %s: %s",filename,OOBase::system_error_text(err)),false);

	return SQ_SUCCEEDED(sq_readclosure(vm,&on_file_read,&file));
}

Indigo::Squirrel::Environment::Environment(unsigned int stack)
		: m_vm(NULL)
{
	m_vm = sq_open(stack);
	if (!m_vm)
		LOG_ERROR(("Failed to create squirrel environment"));
	else
	{
		sq_setforeignptr(m_vm,this);
		sq_setcompilererrorhandler(m_vm,&on_compiler_error);
		sq_setprintfunc(m_vm,&on_print,&on_error);
	}
}

Indigo::Squirrel::Environment::~Environment()
{
	if (m_vm)
		sq_close(m_vm);
}

bool Indigo::Squirrel::Environment::load_library(const char* filename)
{
	if (!m_vm)
		LOG_ERROR_RETURN(("Attempted to load library into invalid environment"),false);

	bool ret = false;
	SQInteger top = sq_gettop(m_vm);
	sq_pushroottable(m_vm);

	if (load_file(m_vm,filename))
	{
		sq_push(m_vm,-2);

		ret = SQ_SUCCEEDED(sq_call(m_vm,1,SQFalse,SQTrue));
	}

	sq_settop(m_vm,top);

	return ret;
}

OOBase::RefPtr<Indigo::Squirrel::Script> Indigo::Squirrel::Environment::load_script(const char* filename, unsigned int stack)
{
	OOBase::RefPtr<Script> ptrScript;

	if (!m_vm)
		LOG_ERROR_RETURN(("Attempted to load script into invalid environment"),ptrScript);

	HSQUIRRELVM new_vm = sq_newthread(m_vm,stack);
	if (!new_vm)
		LOG_ERROR_RETURN(("Failed to create new squirrel script"),ptrScript);

	sq_newtable(new_vm);

	if (load_file(new_vm,filename))
	{
		sq_push(new_vm,-2);

		if (SQ_SUCCEEDED(sq_call(new_vm,1,SQFalse,SQTrue)))
		{
			sq_pop(new_vm,1);

			HSQOBJECT handle;
			sq_resetobject(&handle);
			sq_getstackobj(m_vm,-1,&handle);
			sq_addref(new_vm,&handle);

			Script* pScript = NULL;
			if (OOBase::ThreadLocalAllocator::allocate_new(pScript,new_vm,handle))
				ptrScript = pScript;
			else
			{
				LOG_ERROR(("Failed to allocate script object"));
				sq_release(new_vm,&handle);
			}
		}
	}

	sq_pop(m_vm,1);

	return ptrScript;
}

bool Indigo::Squirrel::Environment::add_class(const char* name, size_t size, const char* base)
{
	SQInteger top = sq_gettop(m_vm);

	// Get the base class 'base'
	if (base)
	{
		sq_pushstring(m_vm,base,-1);
		if (!SQ_SUCCEEDED(sq_get(m_vm,-1)) || sq_gettype(m_vm,-1) != OT_CLASS)
		{
			sq_settop(m_vm,top);
			LOG_ERROR_RETURN(("'%s' is not a squirrel class",base),false);
		}
	}

	// Create the class
	if (!SQ_SUCCEEDED(sq_newclass(m_vm,base ? 1 : 0)))
	{
		sq_settop(m_vm,top);
		LOG_ERROR_RETURN(("Failed to create class '%s'",name),false);
	}

	// Set userdata size
	if (!SQ_SUCCEEDED(sq_setclassudsize(m_vm,-1,size)))
	{
		sq_settop(m_vm,top);
		LOG_ERROR_RETURN(("sq_setclassudsize() failed"),false);
	}



	return true;
}
