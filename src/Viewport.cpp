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

#include "Viewport.h"
#include "Camera.h"

Indigo::Viewport::Viewport(const OOBase::SharedPtr<Framebuffer>& fb, const glm::ivec2& lower_left, const glm::ivec2& size) :
		m_fb(fb),
		m_lower_left(lower_left),
		m_size(size),
		m_scissor(false)
{
}

Indigo::Viewport::Viewport(const Viewport& rhs) :
		m_fb(rhs.m_fb),
		m_lower_left(rhs.m_lower_left),
		m_size(rhs.m_size),
		m_scissor(rhs.m_scissor),
		m_cameras(rhs.m_cameras)
{
}

Indigo::Viewport::~Viewport()
{
}

Indigo::Viewport& Indigo::Viewport::operator = (const Viewport& rhs)
{
	Viewport(rhs).swap(*this);
	return *this;
}

void Indigo::Viewport::swap(Viewport& rhs)
{
	OOBase::swap(m_fb,rhs.m_fb);
	OOBase::swap(m_lower_left,rhs.m_lower_left);
	OOBase::swap(m_size,rhs.m_size);
	OOBase::swap(m_cameras,rhs.m_cameras);
}

OOBase::SharedPtr<Indigo::Framebuffer> Indigo::Viewport::framebuffer() const
{
	return m_fb.lock();
}

void Indigo::Viewport::move(const glm::ivec2& lower_left, const glm::ivec2& size)
{
	if (lower_left != m_lower_left || size != m_size)
	{
		m_lower_left = lower_left;
		m_size = size;

		for (cameras_t::iterator c = m_cameras.begin(); c != m_cameras.end(); ++c)
			(*c)->cull();
	}
}

const glm::ivec2& Indigo::Viewport::lower_left() const
{
	return m_lower_left;
}

void Indigo::Viewport::lower_left(const glm::ivec2& lower_left)
{
	move(lower_left,m_size);
}

const glm::ivec2& Indigo::Viewport::size() const
{
	return m_size;
}

void Indigo::Viewport::size(const glm::ivec2& size)
{
	move(m_lower_left,size);
}

void Indigo::Viewport::scissor(bool scissor)
{
	m_scissor = scissor;
}

bool Indigo::Viewport::scissor() const
{
	return m_scissor;
}

const Indigo::Viewport::cameras_t& Indigo::Viewport::cameras() const
{
	return m_cameras;
}

void Indigo::Viewport::render(State& gl_state)
{
	glViewport(m_lower_left.x,m_lower_left.y,m_size.x,m_size.y);

	if (!m_scissor)
		gl_state.scissor_off();
	else
		gl_state.scissor(m_lower_left,m_size);

	for (cameras_t::iterator c = m_cameras.begin(); c != m_cameras.end(); ++c)
		(*c)->draw(gl_state);
}
