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

#ifndef INDIGO_BUFFEROBJECT_H_INCLUDED
#define INDIGO_BUFFEROBJECT_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class State;
	class BufferObject;

	namespace detail
	{
		class BufferMapping : public OOBase::detail::SharedCountBase
		{
		public:
			BufferMapping(const OOBase::SharedPtr<BufferObject>& buffer, void* map) :
				SharedCountBase(), m_buffer(buffer), m_map(map)
			{
			}

			void dispose();
			void destroy();

		private:
			OOBase::SharedPtr<BufferObject> m_buffer;
			void* m_map;
		};
	}

	class BufferObject : public OOBase::EnableSharedFromThis<BufferObject>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class State;
		friend class StateFns;

	public:
		static OOBase::SharedPtr<BufferObject> create(GLenum target, GLenum usage, GLsizeiptr size, const void* data = NULL);

		template <typename T>
		OOBase::SharedPtr<T> auto_map(GLenum access, GLintptr offset = 0)
		{
			return OOBase::reinterpret_pointer_cast<T,char>(auto_map_i(access,offset,m_size - offset));
		}

		template <typename T>
		OOBase::SharedPtr<T> auto_map(GLenum access, GLintptr offset, GLsizeiptr length)
		{
			return OOBase::reinterpret_pointer_cast<T,char>(auto_map_i(access,offset,length));
		}

		void* map(GLenum access, GLintptr offset = 0)
		{
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

		void copy(GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size);

	private:
		GLuint     m_buffer;
		GLenum     m_target;
		GLenum     m_usage;
		GLsizeiptr m_size;

		BufferObject(GLenum target, GLenum usage, GLsizeiptr size, const void* data = NULL);
		~BufferObject();

		OOBase::SharedPtr<char> auto_map_i(GLenum access, GLintptr offset, GLsizeiptr length);
		void bind(GLenum target);
	};
}

#endif // INDIGO_BUFFEROBJECT_H_INCLUDED
