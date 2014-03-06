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

void Indigo::State::bind(const OOBase::SharedPtr<Framebuffer>& fb)
{
	if (m_fb != fb)
	{
		fb->bind();
		m_fb = fb;
	}
}

void Indigo::State::clear_colour(const glm::vec4& rgba)
{
	if (m_clear_colour != rgba)
	{
		m_clear_colour = rgba;
		glClearColor(m_clear_colour.r,m_clear_colour.g,m_clear_colour.b,m_clear_colour.a);
	}
}

void Indigo::State::clear_depth(GLfloat depth)
{
	if (m_clear_depth != depth)
	{
		m_clear_depth = depth;
		glClearDepth(m_clear_depth);
	}
}

void Indigo::State::clear_stencil(GLint s)
{
	if (m_clear_stencil != s)
	{
		m_clear_stencil = s;
		glClearStencil(m_clear_stencil);
	}
}

void Indigo::State::depth_range(const glm::vec2& depth_range)
{
	if (m_depth_range != depth_range)
	{
		m_depth_range = depth_range;
		glDepthRange(m_depth_range.x,m_depth_range.y);
	}
}

void Indigo::State::scissor_off()
{
	if (m_scissor)
	{
		m_scissor = false;
		glDisable(GL_SCISSOR_TEST);
	}
}

void Indigo::State::scissor(const glm::ivec2& lower_left, const glm::ivec2& size)
{
	glm::ivec4 sc(lower_left.x,lower_left.y,size.x,size.y);
	if (!m_scissor)
	{
		m_scissor = true;
		glEnable(GL_SCISSOR_TEST);
		m_scissor_rect = sc;
		glScissor(m_scissor_rect.x,m_scissor_rect.y,m_scissor_rect.x,m_scissor_rect.y);
	}
	else if (m_scissor_rect != sc)
	{
		m_scissor_rect = sc;
		glScissor(m_scissor_rect.x,m_scissor_rect.y,m_scissor_rect.x,m_scissor_rect.y);
	}
}
