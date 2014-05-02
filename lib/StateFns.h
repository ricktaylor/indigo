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

#ifndef INDIGO_STATE_FNS_H_INCLUDED
#define INDIGO_STATE_FNS_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class StateFns : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<StateFns>
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		
	public:
		static OOBase::SharedPtr<StateFns> get_current();

		void glGenFramebuffers(GLsizei n, GLuint *framebuffers);
		void glDeleteFramebuffers(GLsizei n, GLuint *framebuffers);
		void glBindFramebuffer(GLenum target, GLuint framebuffer);
		GLenum glCheckFramebufferStatus(GLenum target);

		GLuint glCreateShader(GLenum shaderType);
		void glDeleteShader(GLuint shader);
		void glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);

	private:
		StateFns();

		PFNGLGENFRAMEBUFFERSPROC m_fn_glGenFramebuffers;
		PFNGLDELETEFRAMEBUFFERSPROC m_fn_glDeleteFramebuffers;
		PFNGLBINDFRAMEBUFFERPROC m_fn_glBindFramebuffer;
		PFNGLCHECKFRAMEBUFFERSTATUSPROC m_fn_glCheckFramebufferStatus;

		PFNGLCREATESHADERPROC m_fn_glCreateShader;
		PFNGLDELETESHADERPROC m_fn_glDeleteShader;
		PFNGLSHADERSOURCEPROC m_fn_glShaderSource;
	};
}

#endif // INDIGO_STATE_H_INCLUDED
