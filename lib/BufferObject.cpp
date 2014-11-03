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

Indigo::BufferObject::BufferObject(GLenum target, GLenum usage, GLsizeiptr size, const void* data) : m_buffer(0), m_target(target), m_usage(usage), m_size(size)
{
	StateFns::get_current()->glGenBuffers(1,&m_buffer);
	StateFns::get_current()->glNamedBufferData(m_target,m_buffer,size,data,usage);
}

Indigo::BufferObject::~BufferObject()
{
	StateFns::get_current()->glDeleteBuffers(1,&m_buffer);
}

void Indigo::BufferObject::bind()
{
	StateFns::get_current()->glBindBuffer(m_target,m_buffer);
}

OOBase::SharedPtr<char> Indigo::BufferObject::map_i(GLenum access, GLintptr offset, GLsizeiptr length)
{
	OOBase::SharedPtr<char> ret;

	void* m = StateFns::get_current()->glMapBufferRange(m_target,m_buffer,offset,length,m_usage,m_size,access);
	if (m)
	{
		detail::BufferMapping* bm = NULL;
		OOBase::ThreadLocalAllocator::allocate_new(bm,shared_from_this(),m);
		if (bm)
			ret = OOBase::make_shared(reinterpret_cast<char*>(m),bm);

		if (!ret)
			StateFns::get_current()->glUnmapBuffer(m_target,m_buffer);
	}

	return ret;
}

void Indigo::BufferObject::unmap()
{
	StateFns::get_current()->glUnmapBuffer(m_target,m_buffer);
}
