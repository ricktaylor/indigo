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

#include <OOBase/Logger.h>

#include "../include/OOGL/BufferObject.h"
#include "../include/OOGL/Shader.h"
#include "../include/OOGL/Texture.h"
#include "../include/OOGL/VertexArrayObject.h"
#include "../include/OOGL/Window.h"

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
	GLint params[4] = {0};
	glGetIntegerv(GL_VIEWPORT,params);
	OOGL_CHECK("glGetIntegerv");
	m_viewport.x = params[0];
	m_viewport.y = params[1];
	m_viewport.width = params[2];
	m_viewport.height = params[3];
}

OOGL::State::~State()
{
	// You probably want to use reset()...
}

void OOGL::State::reset()
{
	for (singleton_t val;m_listSingletonDestructors.pop_back(&val);)
	{
		OOBase::HashTable<const void*,void*,OOBase::ThreadLocalAllocator>::iterator i = m_mapSingletons.find(val.m_key);
		if (i)
			(*val.m_destructor)(i->second);
	}
	m_mapSingletons.clear();

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

OOGL::State::viewport_t OOGL::State::viewport(const viewport_t& vp)
{
	viewport_t prev = m_viewport;
	if (memcmp(&m_viewport,&vp,sizeof(vp)) != 0)
	{
		glViewport(vp.x,vp.y,vp.width,vp.height);
		OOGL_CHECK("glViewport");
		m_viewport = vp;
	}
	return prev;
}

const OOGL::State::viewport_t& OOGL::State::viewport() const
{
	return m_viewport;
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
				m_state_fns.glBindFramebuffer(GL_FRAMEBUFFER,fb->m_id);
		}
	}
	else if (target == GL_DRAW_FRAMEBUFFER)
	{
		prev = m_draw_fb;
		if (m_draw_fb != fb)
		{
			m_draw_fb = fb;

			if (fb)
				m_state_fns.glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fb->m_id);
		}
	}
	else if (target == GL_READ_FRAMEBUFFER)
	{
		prev = m_read_fb;
		if (m_read_fb != fb)
		{
			m_read_fb = fb;

			if (fb)
				m_state_fns.glBindFramebuffer(GL_READ_FRAMEBUFFER,fb->m_id);
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

OOBase::SharedPtr<OOGL::Texture> OOGL::State::bind_texture_active_unit(const OOBase::SharedPtr<Texture>& texture)
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

			m_state_fns.glBindTextureUnit(*this,unit,texture->m_target,texture->m_tex);
			
			return prev;
		}
		tu = m_vecTexUnits.at(unit);
	}

	tex_unit_t::iterator i = tu->find(texture->target());
	if (!i)
	{
		m_state_fns.glBindTextureUnit(*this,unit,texture->m_target,texture->m_tex);

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
			if (i->second.texture != texture->m_tex)
			{
				m_state_fns.glBindTextureUnit(*this,unit,texture->m_target,texture->m_tex);

				i->second.texture = texture->m_tex;
			}
			i->second.tex_ptr = texture;
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
			i->second.tex_ptr.reset();

			bind = true;
		}

	}

	if (bind)
	{
		m_state_fns.glBindTextureUnit(*this,m_active_texture_unit,target,texture);
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
	return bind_buffer_target(buffer_object,buffer_object ? buffer_object->m_target : 0);
}

OOBase::SharedPtr<OOGL::BufferObject> OOGL::State::bind_buffer_target(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target)
{
	OOBase::SharedPtr<BufferObject> prev;
	OOBase::HashTable<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
	if (!i)
	{
		if (buffer_object)
			m_state_fns.glBindBuffer(target,buffer_object->m_buffer);
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
				m_state_fns.glBindBuffer(target,buffer_object->m_buffer);
			else
				m_state_fns.glBindBuffer(target,0);
		}
	}

	return prev;
}

void OOGL::State::bind_buffer(GLuint buffer, GLenum target)
{
	bool bind = true;

	OOBase::HashTable<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
	if (i && i->second && i->second->m_buffer == buffer)
		bind = false;

	if (bind)
	{
		m_state_fns.glBindBuffer(target,buffer);

		if (i && i->second)
			m_buffer_objects.erase(i);
	}
}

void OOGL::State::update_bind(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target)
{
	OOBase::HashTable<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::ThreadLocalAllocator>::iterator i = m_buffer_objects.find(target);
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
		{
			m_state_fns.glBindVertexArray(vao->m_array);

			// VAO bind sets the GL_ELEMENT_ARRAY_BUFFER binding
			update_bind(vao->m_element_array,GL_ELEMENT_ARRAY_BUFFER);
		}
	}

	return prev;
}

OOBase::SharedPtr<OOGL::VertexArrayObject> OOGL::State::unbind_vao()
{
	OOBase::SharedPtr<VertexArrayObject> prev = m_current_vao;

	if (m_current_vao->m_array != 0)
	{
		m_current_vao.reset();

		m_state_fns.glBindVertexArray(0);

		// VAO bind sets the GL_ELEMENT_ARRAY_BUFFER binding
		m_buffer_objects.remove(GL_ELEMENT_ARRAY_BUFFER);
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

bool OOGL::State::get_singleton(const void* key, void** val)
{
	OOBase::HashTable<const void*,void*,OOBase::ThreadLocalAllocator>::iterator i = m_mapSingletons.find(key);
	if (!i)
		return false;

	if (val)
		*val = i->second;

	return true;
}

bool OOGL::State::set_singleton(const void* key, void* val, void (*destructor)(void*))
{
	OOBase::HashTable<const void*,void*,OOBase::ThreadLocalAllocator>::iterator i = m_mapSingletons.find(key);
	if (i)
	{
		if (destructor)
		{
			for (OOBase::List<singleton_t,OOBase::ThreadLocalAllocator>::iterator j=m_listSingletonDestructors.begin();j;++j)
			{
				if (j->m_key == key)
				{
					j->m_destructor = destructor;
					break;
				}
			}
		}

		i->second = val;
		return true;
	}

	if (destructor)
	{
		singleton_t v;
		v.m_key = key;
		v.m_destructor = destructor;

		if (!m_listSingletonDestructors.push_back(v))
			return false;
	}

	return m_mapSingletons.insert(key,val);
}

bool OOGL::State::enable(GLenum cap, bool enable)
{
	bool prev_state;
	OOBase::HashTable<GLenum,bool,OOBase::ThreadLocalAllocator>::iterator i = m_enables.find(cap);
	if (!i)
	{
		prev_state = (glIsEnabled(cap) == GL_TRUE);
		OOGL_CHECK("glIsEnabled");
		m_enables.insert(cap,enable);
	}
	else
	{
		prev_state = i->second;
		if (enable != i->second)
			i->second = enable;
	}

	if (prev_state != enable)
	{
		if (enable)
		{
			glEnable(cap);
			OOGL_CHECK("glEnable");
		}
		else
		{
			glDisable(cap);
			OOGL_CHECK("glDisable");
		}
	}

	return prev_state;
}
