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

#ifndef INDIGO_STATE_H_INCLUDED
#define INDIGO_STATE_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class Window;
	class Framebuffer;

	class State : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<State>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class Window;

	public:
		//void window(const OOBase::WeakPtr<Window>& w);
		OOBase::SharedPtr<Framebuffer> bind(const OOBase::SharedPtr<Framebuffer>& fb);

		glm::vec4 clear_colour(const glm::vec4& rgba);
		GLfloat clear_depth(GLfloat depth);
		GLint clear_stencil(GLint s);
		glm::vec2 depth_range(const glm::vec2& depth_range);
		glm::ivec4 scissor(const glm::ivec4& rect);

	private:
		OOBase::WeakPtr<Window>        m_window;
		OOBase::SharedPtr<Framebuffer> m_fb;
		glm::vec4                      m_clear_colour;
		GLfloat                        m_clear_depth;
		GLint                          m_clear_stencil;
		glm::vec2                      m_depth_range;
		bool                           m_scissor;
		glm::ivec4                     m_scissor_rect;

		State(const OOBase::SharedPtr<Window>& window);
	};
}

#endif // INDIGO_STATE_H_INCLUDED
