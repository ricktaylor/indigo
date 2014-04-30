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

Indigo::Framebuffer::Framebuffer(const OOBase::SharedPtr<Window>& window) :
		m_window(window),
		m_id(GL_INVALID_VALUE),
		m_default(false)
{
	OOBase::SharedPtr<Window> win = m_window.lock();
	if (win)
		win->m_state_fns->glGenFramebuffers(1,&m_id);
}

Indigo::Framebuffer::Framebuffer(const OOBase::SharedPtr<Window>& window, GLuint id) :
		m_window(window),
		m_id(id),
		m_default(true)
{
}

OOBase::SharedPtr<Indigo::Framebuffer> Indigo::Framebuffer::get_default(const OOBase::SharedPtr<Window>& window)
{
	OOBase::SharedPtr<Framebuffer> ret;
	GLint fb_id = GL_INVALID_VALUE;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING,&fb_id);
	if (fb_id != GL_INVALID_VALUE)
		ret = OOBase::allocate_shared<Framebuffer,OOBase::ThreadLocalAllocator>(window,fb_id);
	return ret;
}

Indigo::Framebuffer::~Framebuffer()
{
	if (!m_default && m_id != GL_INVALID_VALUE)
	{
		OOBase::SharedPtr<Window> win = m_window.lock();
		if (win)
			win->m_state_fns->glDeleteFramebuffers(1,&m_id);
	}
}

OOBase::SharedPtr<Indigo::Window> Indigo::Framebuffer::window() const
{
	return m_window.lock();
}

GLenum Indigo::Framebuffer::check() const
{
	OOBase::SharedPtr<Window> win = m_window.lock();
	if (!win)
		return GL_FRAMEBUFFER_UNSUPPORTED;

	return win->m_state_fns->glCheckFramebufferStatus(m_id);
}

void Indigo::Framebuffer::bind()
{
	OOBase::SharedPtr<Window> win = m_window.lock();
	if (win)
		win->m_state_fns->glBindFramebuffer(GL_FRAMEBUFFER,m_id);
}
