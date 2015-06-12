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

#include <OOBase/Logger.h>

void OOGL::glCheckError(const char* fn, const char* file, unsigned int line)
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
		OOBase::Logger::filenum_t(OOBase::Logger::Error,file,line).log("glGetError() returned code %X after call to %s",err,fn);
	/*else
		OOBase::Logger::log(OOBase::Logger::Information,"%s",fn);*/
}

OOGL::State::State(StateFns& fns) :
		m_state_fns(fns),
		m_active_texture_unit(0)
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
			m_draw_fb = fb;
			m_read_fb = fb;

			if (fb)
				fb->internal_bind(GL_FRAMEBUFFER);
		}
	}
	else if (target == GL_DRAW_FRAMEBUFFER)
	{
		prev = m_draw_fb;
		if (m_draw_fb != fb)
		{
			m_draw_fb = fb;

			if (fb)
				fb->internal_bind(GL_DRAW_FRAMEBUFFER);
		}
	}
	else if (target == GL_READ_FRAMEBUFFER)
	{
		prev = m_read_fb;
		if (m_read_fb != fb)
		{
			m_read_fb = fb;

			if (fb)
				fb->internal_bind(GL_READ_FRAMEBUFFER);
		}
	}

	return prev;
}

GLuint OOGL::State::activate_texture_unit(GLuint unit)
{
	GLuint prev = m_active_texture_unit;
	if (unit != m_active_texture_unit)
	{
		m_state_fns.glActiveTexture(unit);
		m_active_texture_unit = unit;
	}

	if (prev == GLuint(-1))
		prev = unit;

	return prev;
}

GLuint OOGL::State::active_texture_unit() const
{
	return m_active_texture_unit;
}

OOBase::SharedPtr<OOGL::Texture> OOGL::State::internal_bind(const OOBase::SharedPtr<Texture>& texture)
{
	return bind(m_active_texture_unit,texture);
}

OOBase::SharedPtr<OOGL::Texture> OOGL::State::bind(GLuint unit, const OOBase::SharedPtr<Texture>& texture)
{
	assert(texture && texture->valid());

	OOBase::SharedPtr<OOGL::Texture> prev;

	tex_unit_t* tu = m_vecTexUnits.at(unit);
	if (!tu)
	{
		if (!m_vecTexUnits.resize(unit + 1))
		{
			LOG_WARNING(("Failed to resize texture unit cache"));

			texture->internal_bind(*this,unit);

			return prev;
		}
		tu = m_vecTexUnits.at(unit);
	}

	tex_unit_t::iterator i = tu->find(texture->target());
	if (!i)
	{
		texture->internal_bind(*this,unit);

		tex_pair p;
		p.texture = texture->m_tex;
		p.tex_ptr = texture;

		if (!tu->insert(texture->target(),p))
			LOG_WARNING(("Failed to add to texture unit cache"));
	}
	else
	{
		prev = i->second.tex_ptr;
		if (prev != texture)
		{
			i->second.tex_ptr = texture;
			if (i->second.texture != texture->m_tex)
			{
				texture->internal_bind(*this,unit);
				i->second.texture = texture->m_tex;
			}
		}
	}

	return prev;
}

void OOGL::State::bind_texture(GLuint texture, GLenum target)
{
	bool bind = false;

	tex_unit_t* tu = m_vecTexUnits.at(m_active_texture_unit);
	if (!tu)
	{
		if (!m_vecTexUnits.resize(m_active_texture_unit + 1))
		{
			LOG_WARNING(("Failed to resize texture unit cache"));

			bind = true;
		}
		else
			tu = m_vecTexUnits.at(m_active_texture_unit);
	}
	if (tu)
	{
		tex_unit_t::iterator i = tu->find(target);
		if (!i)
		{
			tex_pair p;
			p.texture = texture;

			if (!tu->insert(target,p))
				LOG_WARNING(("Failed to add to texture unit cache"));

			bind = true;
		}
		else if (i->second.texture != texture)
		{
			i->second.texture = texture;

			bind = true;
		}

	}

	if (bind)
	{
		glBindTexture(target,texture);

		OOGL_CHECK("glBindTexture");
	}
}

void OOGL::State::update_texture_binding(GLuint texture, GLenum target)
{
	tex_unit_t* tu = m_vecTexUnits.at(m_active_texture_unit);
	if (tu)
	{
		tex_unit_t::iterator i = tu->find(target);
		if (i && i->second.texture != texture)
		{
			i->second.tex_ptr.reset();
			i->second.texture = texture;
		}
	}
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::State::bind(const OOBase::SharedPtr<BufferObject>& buffer_object)
{
	return internal_bind(buffer_object,buffer_object ? buffer_object->m_target : 0);
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::State::internal_bind(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target)
{
	OOBase::SharedPtr<BufferObject> prev;
	OOBase::Table<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
	if (!i)
	{
		if (buffer_object)
			buffer_object->internal_bind(target);
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
			i->second = buffer_object;

			if (buffer_object)
				buffer_object->internal_bind(target);
			else
				m_state_fns.glBindBuffer(target,0);
		}
	}

	return prev;
}

void OOGL::State::bind_buffer(GLuint buffer, GLenum target)
{
	bool bind = true;

	OOBase::Table<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
	if (i && i->second && i->second->m_buffer == buffer)
		bind = false;

	if (bind)
		m_state_fns.glBindBuffer(target,buffer);
}

void OOGL::State::update_bind(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target)
{
	OOBase::Table<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::Less<GLenum>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
	if (!i)
	{
		if (!m_buffer_objects.insert(target,buffer_object))
			LOG_WARNING(("Failed to add to buffer object cache"));
	}
	else if (i->second != buffer_object)
		i->second = buffer_object;
}

OOBase::SharedPtr<OOGL::VertexArrayObject> OOGL::State::bind(const OOBase::SharedPtr<VertexArrayObject>& vao)
{
	OOBase::SharedPtr<VertexArrayObject> prev = m_current_vao;

	if (m_current_vao != vao)
	{
		m_current_vao = vao;

		if (vao)
			vao->internal_bind();
	}

	return prev;
}

OOBase::SharedPtr<OOGL::Program> OOGL::State::use(const OOBase::SharedPtr<Program>& program)
{
	OOBase::SharedPtr<Program> prev = m_current_program;

	if (m_current_program != program)
	{
		if (program)
			program->internal_use();
		else
			m_state_fns.glUseProgram(0);

		m_current_program = program;
	}

	return prev;
}
