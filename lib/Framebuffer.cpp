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

#include "Framebuffer.h"
#include "Window.h"

Indigo::Framebuffer::Framebuffer() :
		m_id(GL_INVALID_VALUE),
		m_default(false)
{
	StateFns::get_current()->glGenFramebuffers(1,&m_id);
}

Indigo::Framebuffer::Framebuffer(GLuint id) :
		m_id(id),
		m_default(true)
{
}

OOBase::SharedPtr<Indigo::Framebuffer> Indigo::Framebuffer::get_default()
{
	OOBase::SharedPtr<Framebuffer> ret;
	GLint fb_id = GL_INVALID_VALUE;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING,&fb_id);
	if (fb_id != GL_INVALID_VALUE)
		ret = OOBase::allocate_shared<Framebuffer,OOBase::ThreadLocalAllocator>(fb_id);
	return ret;
}

Indigo::Framebuffer::~Framebuffer()
{
	if (!m_default && m_id != GL_INVALID_VALUE)
		StateFns::get_current()->glDeleteFramebuffers(1,&m_id);
}

GLenum Indigo::Framebuffer::check() const
{
	return StateFns::get_current()->glCheckFramebufferStatus(m_id);
}

void Indigo::Framebuffer::bind(GLenum target)
{
	StateFns::get_current()->glBindFramebuffer(target,m_id);
}
