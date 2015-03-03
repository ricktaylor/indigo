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
	class BufferObject;
	
	class State : public OOBase::NonCopyable
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class StateFns;
		friend class Window;
		friend class Texture;
		friend class BufferObject;
		
	public:
		static OOBase::SharedPtr<State> get_current();

		OOBase::SharedPtr<Framebuffer> bind(GLenum target, const OOBase::SharedPtr<Framebuffer>& fb);
		OOBase::SharedPtr<Texture> bind(GLenum unit, const OOBase::SharedPtr<Texture>& texture);
		OOBase::SharedPtr<BufferObject> bind(const OOBase::SharedPtr<BufferObject>& buffer_object);

		OOBase::SharedPtr<Program> use(const OOBase::SharedPtr<Program>& program);

		GLenum active_texture_unit() const;

	private:
		StateFns&                      m_state_fns;
		OOBase::SharedPtr<Framebuffer> m_draw_fb;
		OOBase::SharedPtr<Framebuffer> m_read_fb;
		GLenum                         m_active_texture_unit;
		OOBase::SharedPtr<Program>     m_current_program;
		
		typedef OOBase::Table<GLenum,OOBase::SharedPtr<Texture>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator> tex_unit_t;
		OOBase::Vector<tex_unit_t,OOBase::ThreadLocalAllocator> m_vecTexUnits;

		OOBase::Table<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator> m_buffer_objects;

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

		OOBase::SharedPtr<BufferObject> bind(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target);
		void buffer_data(const OOBase::SharedPtr<BufferObject>& buffer, GLsizei size, const void *data, GLenum usage);
		void* map_buffer_range(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizei orig_size, GLbitfield access);
		bool unmap_buffer(const OOBase::SharedPtr<BufferObject>& buffer);
		void buffer_sub_data(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei size, const void* data);
		void copy_buffer_data(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size);
	};
}

#endif // INDIGO_STATE_H_INCLUDED
