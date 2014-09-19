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

Indigo::State::State(StateFns& fns) :
		m_state_fns(fns),
		m_active_texture_unit(-1)
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

OOBase::SharedPtr<Indigo::Framebuffer> Indigo::State::bind(const OOBase::SharedPtr<Framebuffer>& fb)
{
	OOBase::SharedPtr<Framebuffer> prev = m_fb;

	if (m_fb != fb)
	{
		if (fb)
			fb->bind();

		m_fb = fb;
	}

	return prev;
}

GLenum Indigo::State::activate_texture_unit(GLenum unit)
{
	GLenum prev = m_active_texture_unit;
	if (unit != m_active_texture_unit)
	{
		glActiveTexture(unit);
		m_active_texture_unit = unit;
	}

	if (prev == GLenum(-1))
		prev = unit;

	return prev;
}

OOBase::SharedPtr<Indigo::Texture> Indigo::State::bind(GLenum unit, const OOBase::SharedPtr<Texture>& texture)
{
	OOBase::SharedPtr<Indigo::Texture> prev;

	tex_unit_t* tu = m_vecTexUnits.at(unit - GL_TEXTURE0);
	if (!tu)
	{
		int err = m_vecTexUnits.resize(unit - GL_TEXTURE0 + 1);
		if (err)
		{
			LOG_WARNING(("Failed to resize texture unit cache: %s",OOBase::system_error_text(err)));

			if (texture)
				texture->bind(*this,unit);

			return prev;
		}

		tu = m_vecTexUnits.at(unit - GL_TEXTURE0);
	}

	tex_unit_t::iterator i = tu->find(texture->type());
	if (i == tu->end())
	{
		if (texture)
		{
			texture->bind(*this,unit);

			int err = tu->insert(texture->type(),texture);
			if (err)
				LOG_WARNING(("Failed to add to texture unit cache: %s",OOBase::system_error_text(err)));
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

void Indigo::State::bind_multi_texture(GLenum unit, GLenum target, GLuint texture)
{
	m_state_fns.glBindMultiTexture(this,unit,target,texture);
}

OOBase::SharedPtr<Indigo::Program> Indigo::State::use(const OOBase::SharedPtr<Program>& program)
{
	OOBase::SharedPtr<Program> prev = m_program;

	if (m_program != program)
	{
		if (program)
			program->use();
		else
			m_state_fns.glUseProgram(0);

		m_program = program;
	}

	return prev;
}
