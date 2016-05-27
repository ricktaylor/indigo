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

#ifndef INDIGO_BUFFEROBJECT_H_INCLUDED
#define INDIGO_BUFFEROBJECT_H_INCLUDED

#include <OOBase/SharedPtr.h>

#include "OOGL.h"

namespace OOGL
{
	class State;

	class BufferObject : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<BufferObject>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class VertexArrayObject;
		friend class State;
		friend class StateFns;

	public:
		BufferObject(GLenum target, GLenum usage, GLsizeiptr size, const void* data = NULL);
		~BufferObject();

		bool valid() const;

		static OOBase::SharedPtr<BufferObject> none(GLenum target);
		
		OOBase::SharedPtr<OOGL::BufferObject> bind();

		template <typename T>
		OOBase::SharedPtr<T> auto_map(GLenum access, GLintptr offset = 0)
		{
			if (offset >= m_size)
				return OOBase::SharedPtr<T>();

			return OOBase::reinterpret_pointer_cast<T,char>(auto_map_i(access,offset,m_size - offset));
		}

		template <typename T>
		OOBase::SharedPtr<T> auto_map(GLenum access, GLintptr offset, GLsizeiptr length)
		{
			return OOBase::reinterpret_pointer_cast<T,char>(auto_map_i(access,offset,length));
		}

		void* map(GLenum access, GLintptr offset = 0)
		{
			if (offset >= m_size)
				return NULL;

			return map(access,offset,m_size - offset);
		}

		void* map(GLenum access, GLintptr offset, GLsizeiptr length);
		bool unmap();

		GLenum target() const
		{
			return m_target;
		}

		GLenum usage() const
		{
			return m_usage;
		}

		GLsizeiptr size() const
		{
			return m_size;
		}

		void write(GLintptr offset, GLsizeiptr size, const void* data);

		void copy(GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size);

	private:
		GLuint     m_buffer;
		GLenum     m_target;
		GLenum     m_usage;
		GLsizeiptr m_size;

		BufferObject(GLenum target);

		OOBase::SharedPtr<char> auto_map_i(GLenum access, GLintptr offset, GLsizeiptr length);
	};
}

#endif // INDIGO_BUFFEROBJECT_H_INCLUDED
