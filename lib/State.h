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

#ifndef INDIGO_STATE_H_INCLUDED
#define INDIGO_STATE_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class StateFns;
	class Framebuffer;
	class Texture;
	class Program;
	class FontManager;

	class State : public OOBase::NonCopyable
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class StateFns;
		friend class Window;
		friend class Texture;
		friend class Font;

	public:
		static OOBase::SharedPtr<State> get_current();

		OOBase::SharedPtr<Framebuffer> bind(const OOBase::SharedPtr<Framebuffer>& fb);
		OOBase::SharedPtr<Indigo::Texture> bind(GLenum unit, const OOBase::SharedPtr<Texture>& texture);

		OOBase::SharedPtr<Program> use(const OOBase::SharedPtr<Program>& program);

		GLenum active_texture_unit() const;

	private:
		StateFns&                      m_state_fns;
		OOBase::SharedPtr<Framebuffer> m_fb;
		GLenum                         m_active_texture_unit;
		OOBase::SharedPtr<Program>     m_current_program;
		OOBase::SharedPtr<FontManager> m_font_manager;

		typedef OOBase::Table<GLenum,OOBase::SharedPtr<Texture>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator> tex_unit_t;
		OOBase::Vector<tex_unit_t,OOBase::ThreadLocalAllocator> m_vecTexUnits;

		State(StateFns& fns);

		void bind_multi_texture(GLenum unit, GLenum target, GLuint texture);
		GLenum activate_texture_unit(GLenum unit);

		void texture_storage(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void texture_storage(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void texture_storage(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		void texture_subimage(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void texture_subimage(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void texture_subimage(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

		void texture_parameter(GLuint texture, GLenum target, GLenum name, GLfloat val);
		void texture_parameter(GLuint texture, GLenum target, GLenum name, const GLfloat* pval);
		void texture_parameter(GLuint texture, GLenum target, GLenum name, GLint val);
		void texture_parameter(GLuint texture, GLenum target, GLenum name, const GLint* pval);
	};
}

#endif // INDIGO_STATE_H_INCLUDED
