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

#include "Window.h"
#include "Texture.h"
#include "Shader.h"
#include "BufferObject.h"

Indigo::State::State(StateFns& fns) :
		m_state_fns(fns),
		m_active_texture_unit(GL_TEXTURE0)
{
}

OOBase::SharedPtr<Indigo::State> Indigo::State::get_current()
{
	GLFWwindow* win = glfwGetCurrentContext();
	if (!win)
	{
		LOG_ERROR(("No current context!"));
		return OOBase::SharedPtr<State>();
	}

	Window* window = static_cast<Window*>(glfwGetWindowUserPointer(win));
	if (!window)
	{
		LOG_ERROR(("No current window!"));
		return OOBase::SharedPtr<State>();
	}

	return window->m_state;
}

OOBase::SharedPtr<Indigo::Framebuffer> Indigo::State::bind(GLenum target, const OOBase::SharedPtr<Framebuffer>& fb)
{
	OOBase::SharedPtr<Framebuffer> prev;
	if (target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER)
	{
		prev = m_draw_fb;
		if (m_draw_fb != fb || m_read_fb != fb)
		{
			if (fb)
				fb->bind(GL_FRAMEBUFFER);

			m_draw_fb = fb;
			m_read_fb = fb;
		}
	}
	else if (target == GL_DRAW_FRAMEBUFFER)
	{
		prev = m_draw_fb;
		if (m_draw_fb != fb)
		{
			if (fb)
				fb->bind(GL_DRAW_FRAMEBUFFER);

			m_draw_fb = fb;
		}
	}
	else if (target == GL_READ_FRAMEBUFFER)
	{
		prev = m_read_fb;
		if (m_read_fb != fb)
		{
			if (fb)
				fb->bind(GL_READ_FRAMEBUFFER);

			m_read_fb = fb;
		}
	}

	return prev;
}

GLenum Indigo::State::activate_texture_unit(GLenum unit)
{
	GLenum prev = m_active_texture_unit;
	if (unit != m_active_texture_unit)
	{
		m_state_fns.glActiveTexture(unit);
		m_active_texture_unit = unit;
	}

	if (prev == GLenum(-1))
		prev = unit;

	return prev;
}

GLenum Indigo::State::active_texture_unit() const
{
	return m_active_texture_unit;
}

OOBase::SharedPtr<Indigo::Texture> Indigo::State::bind(GLenum unit, const OOBase::SharedPtr<Texture>& texture)
{
	OOBase::SharedPtr<Indigo::Texture> prev;

	tex_unit_t* tu = m_vecTexUnits.at(unit - GL_TEXTURE0);
	if (!tu)
	{
		if (!m_vecTexUnits.resize(unit - GL_TEXTURE0 + 1))
		{
			LOG_WARNING(("Failed to resize texture unit cache"));

			if (texture)
				texture->bind(*this,unit);

			return prev;
		}

		tu = m_vecTexUnits.at(unit - GL_TEXTURE0);
	}

	tex_unit_t::iterator i = tu->find(texture->target());
	if (i == tu->end())
	{
		if (texture)
		{
			texture->bind(*this,unit);

			if (!tu->insert(texture->target(),texture))
				LOG_WARNING(("Failed to add to texture unit cache"));
		}
	}
	else
	{
		prev = i->second;
		if (prev != texture)
		{
			if (texture)
				texture->bind(*this,unit);

			i->second = texture;
		}
	}

	return prev;
}

OOBase::SharedPtr<Indigo::BufferObject> Indigo::State::bind(const OOBase::SharedPtr<BufferObject>& buffer_object)
{
	return bind(buffer_object,buffer_object->m_target);
}

OOBase::SharedPtr<Indigo::BufferObject> Indigo::State::bind(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target)
{
	OOBase::SharedPtr<BufferObject> prev;
	OOBase::Table<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
	if (i == m_buffer_objects.end())
	{
		if (buffer_object)
			buffer_object->bind(target);
		else
			m_state_fns.glBindBuffer(target,0);

		if (!m_buffer_objects.insert(target,buffer_object))
			LOG_WARNING(("Failed to add to buffer object cache"));
	}
	else 
	{
		prev = i->second;
		if (i->second != buffer_object)
		{
			if (buffer_object)
				buffer_object->bind(target);
			else
				m_state_fns.glBindBuffer(target,0);

			i->second = buffer_object;
		}
	}

	return prev;
}

void Indigo::State::bind_multi_texture(GLenum unit, GLenum target, GLuint texture)
{
	m_state_fns.glBindMultiTexture(*this,unit,target,texture);
}

OOBase::SharedPtr<Indigo::Program> Indigo::State::use(const OOBase::SharedPtr<Program>& program)
{
	OOBase::SharedPtr<Program> prev = m_current_program;

	if (m_current_program != program)
	{
		if (program)
			program->use();
		else
			m_state_fns.glUseProgram(0);

		m_current_program = program;
	}

	return prev;
}

void Indigo::State::texture_storage(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width)
{
	m_state_fns.glTextureStorage1D(*this,texture,target,levels,internalFormat,width);
}

void Indigo::State::texture_storage(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height)
{
	m_state_fns.glTextureStorage2D(*this,texture,target,levels,internalFormat,width,height);
}

void Indigo::State::texture_storage(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
	m_state_fns.glTextureStorage3D(*this,texture,target,levels,internalFormat,width,height,depth);
}

void Indigo::State::texture_subimage(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	m_state_fns.glTextureSubImage1D(*this,texture,target,level,xoffset,width,format,type,pixels);
}

void Indigo::State::texture_subimage(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	m_state_fns.glTextureSubImage2D(*this,texture,target,level,xoffset,yoffset,width,height,format,type,pixels);
}

void Indigo::State::texture_subimage(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	m_state_fns.glTextureSubImage3D(*this,texture,target,level,xoffset,yoffset,zoffset,width,height,depth,format,type,pixels);
}

void Indigo::State::texture_parameter(GLuint texture, GLenum target, GLenum name, GLfloat val)
{
	m_state_fns.glTextureParameterf(*this,texture,target,name,val);
}

void Indigo::State::texture_parameter(GLuint texture, GLenum target, GLenum name, const GLfloat* pval)
{
	m_state_fns.glTextureParameterfv(*this,texture,target,name,pval);
}

void Indigo::State::texture_parameter(GLuint texture, GLenum target, GLenum name, GLint val)
{
	m_state_fns.glTextureParameteri(*this,texture,target,name,val);
}

void Indigo::State::texture_parameter(GLuint texture, GLenum target, GLenum name, const GLint* pval)
{
	m_state_fns.glTextureParameteriv(*this,texture,target,name,pval);
}

void Indigo::State::buffer_data(const OOBase::SharedPtr<BufferObject>& buffer, GLsizei size, const void *data, GLenum usage)
{
	m_state_fns.glBufferData(*this,buffer,size,data,usage);
}

void* Indigo::State::map_buffer_range(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizei orig_size, GLbitfield access)
{
	return m_state_fns.glMapBufferRange(*this,buffer,offset,length,orig_usage,orig_size,access);
}

bool Indigo::State::unmap_buffer(const OOBase::SharedPtr<BufferObject>& buffer)
{
	return m_state_fns.glUnmapBuffer(*this,buffer);
}

void Indigo::State::buffer_sub_data(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei size, const void* data)
{
	m_state_fns.glBufferSubData(*this,buffer,offset,size,data);
}

void Indigo::State::copy_buffer_data(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizei size)
{
	m_state_fns.glCopyBufferSubData(*this,write,writeoffset,read,readoffset,size);
}
