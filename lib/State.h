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
	class Texture;

	class State : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<State>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class Window;
		friend class Texture;

	public:
		static OOBase::SharedPtr<State> get_current();

		OOBase::SharedPtr<Framebuffer> bind(const OOBase::SharedPtr<Framebuffer>& fb);

		GLenum activate_texture_unit(GLenum unit);
		void bind(GLenum unit, const OOBase::SharedPtr<Texture>& texture);

	private:
		OOBase::WeakPtr<Window>        m_window;
		OOBase::SharedPtr<Framebuffer> m_fb;
		GLenum                         m_active_texture_unit;

		State(const OOBase::SharedPtr<Window>& window);

		void bind_multi_texture(GLenum unit, GLenum target, GLuint texture);
	};
}

#endif // INDIGO_STATE_H_INCLUDED
