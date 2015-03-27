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

#include "Window.h"
#include "Texture.h"
#include "Shader.h"
#include "BufferObject.h"
#include "VertexArrayObject.h"

void OOGL::glCheckError(const char* fn, const char* file, unsigned int line)
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
		OOBase::Logger::filenum_t(OOBase::Logger::Error,file,line).log("glGetError() returned code %X after call to %s",err,fn);
}

OOGL::State::State(StateFns& fns) :
		m_state_fns(fns),
		m_active_texture_unit(GL_TEXTURE0)
{
}

void OOGL::State::reset()
{
	// Make sure we unbind everything
	m_read_fb.reset();

	m_buffer_objects.clear();
	m_vecTexUnits.clear();
	m_current_vao.reset();
	m_current_program.reset();
	m_draw_fb.reset();
}

OOBase::SharedPtr<OOGL::State> OOGL::State::get_current()
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

OOBase::SharedPtr<OOGL::Framebuffer> OOGL::State::bind(GLenum target, const OOBase::SharedPtr<Framebuffer>& fb)
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

GLenum OOGL::State::activate_texture_unit(GLenum unit)
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

GLenum OOGL::State::active_texture_unit() const
{
	return m_active_texture_unit;
}

OOBase::SharedPtr<OOGL::Texture> OOGL::State::bind(GLenum unit, const OOBase::SharedPtr<Texture>& texture)
{
	OOBase::SharedPtr<OOGL::Texture> prev;

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

OOBase::SharedPtr<OOGL::Texture> OOGL::State::bind_texture(GLuint texture, GLenum target)
{
	OOBase::SharedPtr<OOGL::Texture> prev;

	tex_unit_t* tu = m_vecTexUnits.at(m_active_texture_unit - GL_TEXTURE0);
	if (!tu)
		glBindTexture(target,texture);
	else
	{
		tex_unit_t::iterator i = tu->find(target);
		if (i == tu->end())
			glBindTexture(target,texture);
		else
		{
			prev = i->second;
			if (prev->m_tex != texture)
			{
				glBindTexture(target,texture);

				tu->erase(i);
			}
		}
	}

	return prev;
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::State::bind(const OOBase::SharedPtr<BufferObject>& buffer_object)
{
	return bind(buffer_object,buffer_object ? buffer_object->m_target : 0);
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::State::bind(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target)
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

OOBase::SharedPtr<OOGL::BufferObject> OOGL::State::bind_buffer(GLuint buffer, GLenum target)
{
	OOBase::SharedPtr<BufferObject> prev;
	OOBase::Table<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
	if (i != m_buffer_objects.end())
	{
		prev = i->second;
		if (i->second->m_buffer != buffer)
		{
			m_state_fns.glBindBuffer(target,buffer);

			m_buffer_objects.erase(i);
		}
	}

	m_state_fns.glBindBuffer(target,buffer);

	return prev;
}

OOBase::SharedPtr<OOGL::VertexArrayObject> OOGL::State::bind(const OOBase::SharedPtr<VertexArrayObject>& vao)
{
	OOBase::SharedPtr<VertexArrayObject> prev = m_current_vao;

	if (m_current_vao != vao)
	{
		if (vao)
			vao->bind();
		
		m_current_vao = vao;
	}

	return prev;
}

OOBase::SharedPtr<OOGL::Program> OOGL::State::use(const OOBase::SharedPtr<Program>& program)
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
