///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#include "BufferObject.h"
#include "State.h"
#include "StateFns.h"

void Indigo::detail::BufferMapping::dispose()
{
	// Unmap the buffer
	m_buffer->unmap();
}

void Indigo::detail::BufferMapping::destroy()
{
	OOBase::ThreadLocalAllocator::delete_free(this);
}

OOBase::SharedPtr<Indigo::BufferObject> Indigo::BufferObject::create(GLenum target, GLenum usage, GLsizeiptr size, const void* data)
{
	return OOBase::allocate_shared<BufferObject,OOBase::ThreadLocalAllocator>(target,usage,size,data);
}

Indigo::BufferObject::BufferObject(GLenum target, GLenum usage, GLsizeiptr size, const void* data) : m_buffer(0), m_target(target), m_usage(usage), m_size(size)
{
	StateFns::get_current()->glGenBuffers(1,&m_buffer);
	State::get_current()->buffer_data(shared_from_this(),size,data,usage);
}

Indigo::BufferObject::~BufferObject()
{
	StateFns::get_current()->glDeleteBuffers(1,&m_buffer);
}

void Indigo::BufferObject::bind(GLenum target)
{
	StateFns::get_current()->glBindBuffer(target,m_buffer);
}

OOBase::SharedPtr<char> Indigo::BufferObject::map_i(GLenum access, GLintptr offset, GLsizeiptr length)
{
	OOBase::SharedPtr<char> ret;
	OOBase::SharedPtr<BufferObject> self(shared_from_this());

	void* m = State::get_current()->map_buffer_range(self,offset,length,m_usage,m_size,access);
	if (m)
	{
		detail::BufferMapping* bm = NULL;
		OOBase::ThreadLocalAllocator::allocate_new(bm,self,m);
		if (bm)
			ret = OOBase::make_shared(reinterpret_cast<char*>(m),bm);

		if (!ret)
			State::get_current()->unmap_buffer(self);
	}

	return ret;
}

void Indigo::BufferObject::unmap()
{
	State::get_current()->unmap_buffer(shared_from_this());
}

void Indigo::BufferObject::copy(GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size)
{
	State::get_current()->copy_buffer_data(shared_from_this(),writeoffset,read,readoffset,size);
}
