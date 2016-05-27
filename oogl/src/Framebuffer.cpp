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

#include "../include/OOGL/Framebuffer.h"
#include "../include/OOGL/Window.h"

OOGL::Framebuffer::Framebuffer() :
		m_id(GL_INVALID_VALUE),
		m_default(false)
{
	StateFns::get_current()->glGenFramebuffers(1,&m_id);
}

OOGL::Framebuffer::Framebuffer(GLuint id) :
		m_id(id),
		m_default(true)
{
}

bool OOGL::Framebuffer::valid() const
{
	return m_id != GL_INVALID_VALUE;
}

OOBase::SharedPtr<OOGL::Framebuffer> OOGL::Framebuffer::get_default()
{
	OOBase::SharedPtr<Framebuffer> ret;
	GLint fb_id = GL_INVALID_VALUE;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING,&fb_id);
	if (fb_id != GL_INVALID_VALUE)
		ret = OOBase::allocate_shared<Framebuffer,OOBase::ThreadLocalAllocator>(fb_id);
	return ret;
}

OOGL::Framebuffer::~Framebuffer()
{
	if (!m_default && m_id != GL_INVALID_VALUE)
		StateFns::get_current()->glDeleteFramebuffers(1,&m_id);
}

GLenum OOGL::Framebuffer::check() const
{
	return StateFns::get_current()->glCheckFramebufferStatus(m_id);
}

