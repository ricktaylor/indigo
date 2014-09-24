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
	class State;

	class StateFns : public OOBase::NonCopyable
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
		void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
		void glCompileShader(GLuint shader);
		void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
		void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);

		void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
		void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
		void glAttachShader(GLuint program, GLuint shader);
		void glDetachShader(GLuint program, GLuint shader);
		void glLinkProgram(GLuint program);
		void glUseProgram(GLuint program);

		void glActiveTexture(GLenum texture);
		void glBindMultiTexture(State& state, GLenum unit, GLenum target, GLuint texture);
		void glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
		void glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void glTextureSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void glTextureSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void glTextureSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

	private:
		StateFns();

		PFNGLGENFRAMEBUFFERSPROC m_fn_glGenFramebuffers;
		PFNGLDELETEFRAMEBUFFERSPROC m_fn_glDeleteFramebuffers;
		PFNGLBINDFRAMEBUFFERPROC m_fn_glBindFramebuffer;
		PFNGLCHECKFRAMEBUFFERSTATUSPROC m_fn_glCheckFramebufferStatus;

		PFNGLCREATESHADERPROC m_fn_glCreateShader;
		PFNGLDELETESHADERPROC m_fn_glDeleteShader;
		PFNGLSHADERSOURCEPROC m_fn_glShaderSource;
		PFNGLCOMPILESHADERPROC m_fn_glCompileShader;
		PFNGLGETSHADERIVPROC m_fn_glGetShaderiv;
		PFNGLGETSHADERINFOLOGPROC m_fn_glGetShaderInfoLog;
		PFNGLGETPROGRAMIVPROC m_fn_glGetProgramiv;
		PFNGLGETPROGRAMINFOLOGPROC m_fn_glGetProgramInfoLog;
		PFNGLATTACHSHADERPROC m_fn_glAttachShader;
		PFNGLDETACHSHADERPROC m_fn_glDetachShader;
		PFNGLLINKPROGRAMPROC m_fn_glLinkProgram;

		void (StateFns::*m_thunk_glUseProgram)(GLuint program);
		PFNGLUSEPROGRAMPROC m_fn_glUseProgram;
		void check_glUseProgram(GLuint program);
		void call_glUseProgram(GLuint program);

		void (StateFns::*m_thunk_glActiveTexture)(GLenum texture);
		PFNGLACTIVETEXTUREPROC m_fn_glActiveTexture;
		void check_glActiveTexture(GLenum texture);
		void call_glActiveTexture(GLenum texture);

		void (StateFns::*m_thunk_glBindMultiTexture)(State&,GLenum,GLenum,GLuint);
		PFNGLBINDMULTITEXTUREEXTPROC m_fn_glBindMultiTexture;
		void check_glBindMultiTexture(State& state, GLenum unit, GLenum target, GLuint texture);
		void emulate_glBindMultiTexture(State& state, GLenum unit, GLenum target, GLuint texture);
		void call_glBindMultiTexture(State& state, GLenum unit, GLenum target, GLuint texture);

		PFNGLTEXIMAGE3DPROC m_fn_glTexImage3D;

		void (StateFns::*m_thunk_glTextureStorage1D)(State&,GLuint,GLenum,GLsizei,GLenum,GLsizei);
		GLFWglproc m_fn_glTextureStorage1D;
		void check_glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void emulate_glTextureStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void call_glTextureStorage1DEXT(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void call_glTexStorage1D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);

		void (StateFns::*m_thunk_glTextureStorage2D)(State&,GLuint,GLenum,GLsizei,GLenum,GLsizei,GLsizei);
		GLFWglproc m_fn_glTextureStorage2D;
		void check_glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void emulate_glTextureStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void call_glTextureStorage2DEXT(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void call_glTexStorage2D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);

		void (StateFns::*m_thunk_glTextureStorage3D)(State&,GLuint,GLenum,GLsizei,GLenum,GLsizei,GLsizei,GLsizei);
		GLFWglproc m_fn_glTextureStorage3D;
		void check_glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void emulate_glTextureStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void call_glTextureStorage3DEXT(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void call_glTexStorage3D(State& state, GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		void (StateFns::*m_thunk_glTextureSubImage1D)(State&,GLuint,GLenum,GLint,GLint,GLsizei,GLenum,GLenum,const void*);
		GLFWglproc m_fn_glTextureSubImage1D;
		void check_glTextureSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage1DEXT(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage1D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);

		void (StateFns::*m_thunk_glTextureSubImage2D)(State&,GLuint,GLenum,GLint,GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,const void*);
		GLFWglproc m_fn_glTextureSubImage2D;
		void check_glTextureSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage2DEXT(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage2D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

		void (StateFns::*m_thunk_glTextureSubImage3D)(State&,GLuint,GLenum,GLint,GLint,GLint,GLint,GLsizei,GLsizei,GLsizei,GLenum,GLenum,const void*);
		GLFWglproc m_fn_glTextureSubImage3D;
		void check_glTextureSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage3DEXT(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage3D(State& state, GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
	};
}

#endif // INDIGO_STATE_H_INCLUDED
