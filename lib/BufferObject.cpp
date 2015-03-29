///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#include "BufferObject.h"
#include "State.h"
#include "StateFns.h"

void OOGL::detail::BufferMapping::dispose()
{
	// Unmap the buffer
	m_buffer->unmap();
}

void OOGL::detail::BufferMapping::destroy()
{
	OOBase::ThreadLocalAllocator::delete_free(this);
}

OOGL::BufferObject::BufferObject(GLenum target, GLenum usage, GLsizeiptr size, const void* data) : 
		m_buffer(0), 
		m_target(target),
		m_usage(usage),
		m_size(size)
{
	OOBase::SharedPtr<StateFns> fns = StateFns::get_current();
	fns->glGenBuffers(1,&m_buffer);
	State::get_current()->bind_buffer(m_buffer,target);
	fns->glBufferData(*State::get_current(),m_buffer,m_target,size,data,usage);
}

OOGL::BufferObject::BufferObject(GLenum target) : 
		m_buffer(0), 
		m_target(target),
		m_usage(0),
		m_size(0)
{
}

OOGL::BufferObject::~BufferObject()
{
	if (m_buffer)
		StateFns::get_current()->glDeleteBuffers(1,&m_buffer);
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::BufferObject::none(GLenum target)
{
	return OOBase::allocate_shared<BufferObject,OOBase::ThreadLocalAllocator>(target);
}

void OOGL::BufferObject::bind(GLenum target)
{
	StateFns::get_current()->glBindBuffer(target,m_buffer);
}

void* OOGL::BufferObject::map(GLenum access, GLintptr offset, GLsizeiptr length)
{
	return StateFns::get_current()->glMapBufferRange(*State::get_current(),shared_from_this(),offset,length,m_usage,m_size,access);
}

bool OOGL::BufferObject::unmap()
{
	return StateFns::get_current()->glUnmapBuffer(*State::get_current(),shared_from_this());
}

OOBase::SharedPtr<char> OOGL::BufferObject::auto_map_i(GLenum access, GLintptr offset, GLsizeiptr length)
{
	OOBase::SharedPtr<char> ret;
	OOBase::SharedPtr<BufferObject> self(shared_from_this());

	void* m = StateFns::get_current()->glMapBufferRange(*State::get_current(),self,offset,length,m_usage,m_size,access);
	if (m)
	{
		detail::BufferMapping* bm = NULL;
		OOBase::ThreadLocalAllocator::allocate_new(bm,self,m);
		if (bm)
			ret = OOBase::make_shared(reinterpret_cast<char*>(m),bm);

		if (!ret)
			StateFns::get_current()->glUnmapBuffer(*State::get_current(),self);
	}

	return ret;
}

void OOGL::BufferObject::write(GLintptr offset, GLsizeiptr size, const void* data)
{
	StateFns::get_current()->glBufferSubData(*State::get_current(),shared_from_this(),offset,size,data);
}

void OOGL::BufferObject::copy(GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size)
{
	StateFns::get_current()->glCopyBufferSubData(*State::get_current(),shared_from_this(),writeoffset,read,readoffset,size);
}
