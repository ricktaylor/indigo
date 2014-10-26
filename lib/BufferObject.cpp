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

Indigo::BufferObject::BufferObject(GLenum target, GLsizeiptr size, GLenum usage) : m_buffer(0), m_target(target)
{
	StateFns::get_current()->glGenBuffers(1,&m_buffer);
	State::get_current()->buffer_init(m_target,m_buffer,size,usage);
}

Indigo::BufferObject::~BufferObject()
{
	StateFns::get_current()->glDeleteBuffers(1,&m_buffer);
}

OOBase::SharedPtr<char> Indigo::BufferObject::map_i(GLenum access, GLintptr offset, GLsizeiptr length)
{
	OOBase::SharedPtr<char> ret;

	// Do the mapping!
	void* m = NULL;
	if (offset == 0 && length == -1)
		m = glMapBuffer(m_target,access);
	else
		m = glMapBufferRange(m_target,offset,length,access);

	detail::BufferMapping* bm = NULL;
	OOBase::ThreadLocalAllocator::allocate_new(bm,shared_from_this(),m);
	if (bm)
		ret = OOBase::make_shared(reinterpret_cast<char*>(m),bm);

	return ret;
}
