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

#include "Common.h"

namespace Indigo
{
	class Window;
	class FramebufferStack;

	namespace detail
	{
		struct FramebufferFunctions
		{
			friend class Indigo::Window;

			FramebufferFunctions() :
				m_fn_genFramebuffers(NULL),
				m_fn_delFramebuffers(NULL),
				m_fn_bindFramebuffer(NULL),
				m_fn_checkFramebufferStatus(NULL)
			{}

			PFNGLGENFRAMEBUFFERSPROC        m_fn_genFramebuffers;
			PFNGLDELETEFRAMEBUFFERSPROC     m_fn_delFramebuffers;
			PFNGLBINDFRAMEBUFFERPROC        m_fn_bindFramebuffer;
			PFNGLCHECKFRAMEBUFFERSTATUSPROC m_fn_checkFramebufferStatus;

		private:
			void init(GLFWwindow* win);
		};
	}

	class Framebuffer : public OOBase::SafeBoolean, public OOBase::NonCopyable
	{
	public:
		Framebuffer(const OOBase::SharedPtr<Window>& window, GLuint id = GL_INVALID_VALUE);
		~Framebuffer();

		operator bool_type() const;

		bool bind(GLenum target = GL_FRAMEBUFFER) const;
		GLenum check() const;

	private:
		OOBase::SharedPtr<detail::FramebufferFunctions> m_fns;
		GLuint m_id;
		bool   m_destroy;
	};
}

#endif // INDIGO_FRAMEBUFFER_H_INCLUDED
