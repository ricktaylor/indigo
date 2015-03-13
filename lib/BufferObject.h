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

#include "OOGL.h"

namespace OOGL
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

	class BufferObject : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<BufferObject>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class State;
		friend class StateFns;

	public:
		BufferObject(GLenum target, GLenum usage, GLsizei size, const void* data = NULL);
		~BufferObject();

		static OOBase::SharedPtr<BufferObject> none(GLenum target);
		
		template <typename T>
		OOBase::SharedPtr<T> auto_map(GLenum access, GLintptr offset = 0)
		{
			if (offset >= m_size)
				return OOBase::SharedPtr<T>();

			return OOBase::reinterpret_pointer_cast<T,char>(auto_map_i(access,offset,static_cast<GLsizei>(m_size - offset)));
		}

		template <typename T>
		OOBase::SharedPtr<T> auto_map(GLenum access, GLintptr offset, GLsizei length)
		{
			return OOBase::reinterpret_pointer_cast<T,char>(auto_map_i(access,offset,length));
		}

		void* map(GLenum access, GLintptr offset = 0)
		{
			if (offset >= m_size)
				return NULL;

			return map(access,offset,static_cast<GLsizei>(m_size - offset));
		}

		void* map(GLenum access, GLintptr offset, GLsizei length);
		bool unmap();

		GLenum target() const
		{
			return m_target;
		}

		GLenum usage() const
		{
			return m_usage;
		}

		GLsizei size() const
		{
			return m_size;
		}

		void write(GLintptr offset, GLsizei size, const void* data);

		void copy(GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size);

	private:
		GLuint     m_buffer;
		GLenum     m_target;
		GLenum     m_usage;
		GLsizei    m_size;

		BufferObject(GLenum target);

		OOBase::SharedPtr<char> auto_map_i(GLenum access, GLintptr offset, GLsizei length);
		void bind(GLenum target);
	};
}

#endif // INDIGO_BUFFEROBJECT_H_INCLUDED
