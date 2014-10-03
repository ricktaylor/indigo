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
		friend class detail::BufferMapping;

	public:
		void data(GLintptr offset, GLsizeiptr size, const void* data);

		template <typename T>
		OOBase::SharedPtr<T> map(GLenum access)
		{
			return OOBase::reinterpret_pointer_cast<T,char>(map_i(access));
		}

		template <typename T>
		OOBase::SharedPtr<T> map(GLintptr offset, GLsizeiptr length, GLenum access)
		{
			return OOBase::reinterpret_pointer_cast<T,char>(map_i(offset,length,access));
		}

	protected:
		GLuint m_buffer;
		GLenum m_target;

		BufferObject(GLenum target, GLsizeiptr size, GLenum usage);
		~BufferObject();

	private:
		OOBase::SharedPtr<char> map_i(GLenum access);
		OOBase::SharedPtr<char> map_i(GLintptr offset, GLsizeiptr length, GLenum access);
		void unmap();
	};
}

#endif // INDIGO_BUFFEROBJECT_H_INCLUDED
