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

#include <OOBase/SharedPtr.h>

#include "OOGL.h"

namespace OOGL
{
	class State;

	class Texture : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Texture>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class State;
		friend class StateFns;

	public:
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* pixels);
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		Texture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

		static OOBase::SharedPtr<Texture> none(GLenum target); 

		~Texture();

		bool valid() const;

		void sub_image(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void sub_image(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void sub_image(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

		void cubemap_sub_image(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

		void parameter(GLenum name, GLfloat val);
		void parameter(GLenum name, const GLfloat* pval);
		void parameter(GLenum name, GLint val);
		void parameter(GLenum name, const GLint* pval);

		GLenum target() const;

		void invalidate(GLint level);
		void invalidate(GLint level, GLint xoffset, GLsizei width);
		void invalidate(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height);
		void invalidate(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);

		void clear(GLint level, GLenum format, GLenum type, const void* pixels);
		void clear(GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void clear(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void clear(GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

	protected:
		GLuint m_tex;
		GLenum m_target;

		Texture(GLenum target);

		void init_mutable(GLsizei levels, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void init_mutable(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void init_mutable(GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

	private:
		Texture(GLuint tex, GLenum target);

		void init();
	};

	class MutableTexture : public Texture
	{
	public:
		MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const void* pixels);
		MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		MutableTexture(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
	};
}

#endif // INDIGO_TEXTURE_H_INCLUDED
