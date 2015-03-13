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

#ifndef INDIGO_TEXTURE_H_INCLUDED
#define INDIGO_TEXTURE_H_INCLUDED

#include "OOGL.h"

namespace OOGL
{
	class State;

	class Texture : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Texture>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class State;

	public:
		// Mutable create
		Texture(GLenum target);

		// Immutable create
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		static OOBase::SharedPtr<Texture> none(GLenum target); 

		~Texture();

		// Mutable init
		void init(GLsizei levels, GLint internalFormat, GLsizei width);
		void init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height);
		void init(GLsizei levels, GLint internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		void image(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void image(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void image(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void image(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

		void parameter(GLenum name, GLfloat val);
		void parameter(GLenum name, const GLfloat* pval);
		void parameter(GLenum name, GLint val);
		void parameter(GLenum name, const GLint* pval);

		GLenum target() const;

	private:
		GLuint m_tex;
		GLenum m_target;

		Texture(GLuint tex, GLenum target);

		void create(GLsizei levels, GLenum internalFormat, GLsizei width);
		void create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void create(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		void bind(State& state, GLenum unit) const;
	};
}

#endif // INDIGO_TEXTURE_H_INCLUDED
