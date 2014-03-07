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

#ifndef INDIGO_FRAMEBUFFER_H_INCLUDED
#define INDIGO_FRAMEBUFFER_H_INCLUDED

#include "State.h"

namespace Indigo
{
	class Window;
	class Viewport;

	class Framebuffer :
			public OOBase::SafeBoolean,
			public OOBase::NonCopyable,
			public OOBase::EnableSharedFromThis<Framebuffer>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class Window;
		friend class State;

	public:
		Framebuffer(const OOBase::SharedPtr<Window>& window);
		~Framebuffer();

		operator bool_type() const;

		OOBase::SharedPtr<Window> window() const;

		GLenum check() const;

		typedef OOBase::Vector<OOBase::SharedPtr<Viewport>,OOBase::ThreadLocalAllocator> viewports_t;
		const viewports_t& viewports() const;
		OOBase::SharedPtr<Viewport> add_viewport(const glm::ivec2& lower_left, const glm::ivec2& size);

		void render(State& gl_state);

	// Signals
	public:
		OOBase::Signal1<glm::ivec2,OOBase::ThreadLocalAllocator> signal_sized;

	private:
		OOBase::WeakPtr<Window>  m_window;
		GLuint       m_id;
		bool         m_default;
		viewports_t  m_viewports;

		Framebuffer(const OOBase::SharedPtr<Window>& window, GLuint id);
		static OOBase::SharedPtr<Framebuffer> get_default(const OOBase::SharedPtr<Window>& window);

		void bind();
	};
}

#endif // INDIGO_FRAMEBUFFER_H_INCLUDED
