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
	class Framebuffer :
			public OOBase::NonCopyable,
			public OOBase::EnableSharedFromThis<Framebuffer>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class Window;
		friend class State;

	public:
		Framebuffer();
		~Framebuffer();

		GLenum check() const;

	private:
		GLuint       m_id;
		bool         m_default;

		Framebuffer(GLuint id);
		static OOBase::SharedPtr<Framebuffer> get_default();

		void bind();
	};
}

#endif // INDIGO_FRAMEBUFFER_H_INCLUDED
