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

#include "State.h"
#include "Framebuffer.h"

Indigo::State::State(const OOBase::SharedPtr<Window>& window) :
		m_window(window),
		m_clear_depth(1.0f),
		m_clear_stencil(0),
		m_depth_range(0,1),
		m_scissor(false)
{
}

OOBase::SharedPtr<Indigo::Framebuffer> Indigo::State::bind(const OOBase::SharedPtr<Framebuffer>& fb)
{
	OOBase::SharedPtr<Framebuffer> prev = m_fb;

	if (m_fb != fb)
	{
		fb->bind();
		m_fb = fb;
	}

	return prev;
}

glm::vec4 Indigo::State::clear_colour(const glm::vec4& rgba)
{
	glm::vec4 prev = m_clear_colour;

	if (m_clear_colour != rgba)
	{
		m_clear_colour = rgba;
		glClearColor(m_clear_colour.r,m_clear_colour.g,m_clear_colour.b,m_clear_colour.a);
	}

	return prev;
}

GLfloat Indigo::State::clear_depth(GLfloat depth)
{
	GLfloat prev = m_clear_depth;

	if (m_clear_depth != depth)
	{
		m_clear_depth = depth;
		glClearDepth(m_clear_depth);
	}

	return prev;
}

GLint Indigo::State::clear_stencil(GLint s)
{
	GLint prev = m_clear_stencil;

	if (m_clear_stencil != s)
	{
		m_clear_stencil = s;
		glClearStencil(m_clear_stencil);
	}

	return prev;
}

glm::vec2 Indigo::State::depth_range(const glm::vec2& depth_range)
{
	glm::vec2 prev = m_depth_range;

	if (m_depth_range != depth_range)
	{
		m_depth_range = depth_range;
		glDepthRange(m_depth_range.x,m_depth_range.y);
	}

	return prev;
}

glm::ivec4 Indigo::State::scissor(const glm::ivec4& rect)
{
	glm::ivec4 prev = m_scissor_rect;
	bool scissor = (rect == glm::ivec4(0,0,0,0));

	if (scissor && rect != m_scissor_rect)
	{
		m_scissor_rect = rect;
		glScissor(m_scissor_rect.x,m_scissor_rect.y,m_scissor_rect.x,m_scissor_rect.y);
	}

	if (m_scissor != scissor)
	{
		m_scissor = scissor;
		if (m_scissor)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	return prev;
}
