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

#ifndef INDIGO_TEXTURE_H_INCLUDED
#define INDIGO_TEXTURE_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class State;

	class Texture : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Texture>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class State;

	public:
		// Mutable create
		static OOBase::SharedPtr<Texture> create(GLenum target);

		// Immutable create
		static OOBase::SharedPtr<Texture> create(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		static OOBase::SharedPtr<Texture> create(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		static OOBase::SharedPtr<Texture> create(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		~Texture();

		// Mutable init
		void init(GLsizei levels, GLint internalFormat, GLsizei width);
		void init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height);
		void init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		void sub_image(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void sub_image(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void sub_image(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void sub_image(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

		GLenum target() const;

		bool is_valid() const;

	private:
		GLuint m_tex;
		GLenum m_target;

		Texture(GLenum target);

		void do_create(GLsizei levels, GLenum internalFormat, GLsizei width);
		void do_create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void do_create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		void bind(State& state, GLenum unit) const;
	};
}


#endif // INDIGO_TEXTURE_H_INCLUDED
