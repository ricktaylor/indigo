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

#include "../include/OOGL/BufferObject.h"
#include "../include/OOGL/State.h"
#include "../include/OOGL/StateFns.h"

namespace
{
	class BufferMapping : public OOBase::detail::SharedCountBase
	{
	public:
		BufferMapping(const OOBase::SharedPtr<OOGL::BufferObject>& buffer, void* map) :
			SharedCountBase(), m_buffer(buffer), m_map(map)
		{
		}

		void dispose();
		void destroy();

	private:
		OOBase::SharedPtr<OOGL::BufferObject> m_buffer;
		void* m_map;
	};
}

void BufferMapping::dispose()
{
	// Unmap the buffer
	m_buffer->unmap();
}

void BufferMapping::destroy()
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
	fns->glBufferData(m_buffer,m_target,size,data,usage);
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

bool OOGL::BufferObject::valid() const
{
	return m_buffer != 0;
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::BufferObject::none(GLenum target)
{
	return OOBase::allocate_shared<BufferObject,OOBase::ThreadLocalAllocator>(target);
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::BufferObject::bind()
{
	// Use VAO element_array() instead
	assert(m_target != GL_ELEMENT_ARRAY_BUFFER);

	return State::get_current()->bind(shared_from_this());
}

void* OOGL::BufferObject::map(GLenum access, GLintptr offset, GLsizeiptr length)
{
	return StateFns::get_current()->glMapBufferRange(shared_from_this(),offset,length,m_usage,m_size,access);
}

bool OOGL::BufferObject::unmap()
{
	return StateFns::get_current()->glUnmapBuffer(shared_from_this());
}

OOBase::SharedPtr<char> OOGL::BufferObject::auto_map_i(GLenum access, GLintptr offset, GLsizeiptr length)
{
	OOBase::SharedPtr<OOGL::BufferObject> self(shared_from_this());
	OOBase::SharedPtr<char> ret;
	void* m = StateFns::get_current()->glMapBufferRange(self,offset,length,m_usage,m_size,access);
	if (m)
	{
		BufferMapping* bm = NULL;
		if (OOBase::ThreadLocalAllocator::allocate_new(bm,self,m))
		{
			ret = OOBase::make_shared(reinterpret_cast<char*>(m),bm);
			if (!ret)
				OOBase::ThreadLocalAllocator::delete_free(bm);
		}

		if (!ret)
			StateFns::get_current()->glUnmapBuffer(self);
	}

	return ret;
}

void OOGL::BufferObject::write(GLintptr offset, GLsizeiptr size, const void* data)
{
	StateFns::get_current()->glBufferSubData(shared_from_this(),offset,size,data);
}

void OOGL::BufferObject::copy(GLintptr dst_offset, const OOBase::SharedPtr<BufferObject>& src, GLintptr src_offset, GLsizeiptr length)
{
	StateFns::get_current()->glCopyBufferSubData(shared_from_this(),dst_offset,src,src_offset,length);
}
