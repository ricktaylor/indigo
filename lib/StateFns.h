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
#include "BufferObject.h"

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
		void glTextureParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val);
		void glTextureParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* pval);
		void glTextureParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val);
		void glTextureParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* val);

		void glGenBuffers(GLsizei n, GLuint* buffers);
		void glBindBuffer(GLenum target, GLuint buffer);
		void glDeleteBuffers(GLsizei n, GLuint* buffers);
		void glNamedBufferData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLsizeiptr size, const void* data, GLenum usage);
		void* glMapBufferRange(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		void glUnmapBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer);

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
		PFNGLUSEPROGRAMPROC m_fn_glUseProgram;
		PFNGLACTIVETEXTUREPROC m_fn_glActiveTexture;

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

		void (StateFns::*m_thunk_glTextureParameterf)(State&,GLuint,GLenum,GLenum,GLfloat);
		GLFWglproc m_fn_glTextureParameterf;
		void check_glTextureParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val);
		void call_glTextureParameterfEXT(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val);
		void call_glTexParameterf(State& state, GLuint texture, GLenum target, GLenum name, GLfloat val);

		void (StateFns::*m_thunk_glTextureParameterfv)(State&,GLuint,GLenum,GLenum,const GLfloat*);
		GLFWglproc m_fn_glTextureParameterfv;
		void check_glTextureParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* pval);
		void call_glTextureParameterfvEXT(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* pval);
		void call_glTexParameterfv(State& state, GLuint texture, GLenum target, GLenum name, const GLfloat* pval);

		void (StateFns::*m_thunk_glTextureParameteri)(State&,GLuint,GLenum,GLenum,GLint);
		GLFWglproc m_fn_glTextureParameteri;
		void check_glTextureParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val);
		void call_glTextureParameteriEXT(State& state, GLuint texture, GLenum target, GLenum name, GLint val);
		void call_glTexParameteri(State& state, GLuint texture, GLenum target, GLenum name, GLint val);

		void (StateFns::*m_thunk_glTextureParameteriv)(State&,GLuint,GLenum,GLenum,const GLint*);
		GLFWglproc m_fn_glTextureParameteriv;
		void check_glTextureParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* pval);
		void call_glTextureParameterivEXT(State& state, GLuint texture, GLenum target, GLenum name, const GLint* pval);
		void call_glTexParameteriv(State& state, GLuint texture, GLenum target, GLenum name, const GLint* pval);

		PFNGLGENBUFFERSPROC m_fn_glGenBuffers;
		PFNGLBINDBUFFERPROC m_fn_glBindBuffer;
		PFNGLDELETEBUFFERSPROC m_fn_glDeleteBuffers;

		void (StateFns::*m_thunk_glNamedBufferData)(State&,const OOBase::SharedPtr<BufferObject>&,GLsizeiptr,const void*,GLenum);
		GLFWglproc m_fn_glNamedBufferData;
		void check_glNamedBufferData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLsizeiptr size, const void *data, GLenum usage);
		void call_glNamedBufferDataEXT(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLsizeiptr size, const void *data, GLenum usage);
		void call_glBufferData(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLsizeiptr size, const void *data, GLenum usage);

		void* (StateFns::*m_thunk_glMapNamedBufferRange)(State&,const OOBase::SharedPtr<BufferObject>&,GLintptr,GLsizei,GLenum,GLsizeiptr,GLbitfield);
		GLFWglproc m_fn_glMapNamedBufferRange;
		void* check_glMapNamedBufferRange(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		void* call_glMapNamedBufferRangeEXT(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		void* call_glMapBufferRange(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		void* call_glMapBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizei length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);

		void (StateFns::*m_thunk_glUnmapNamedBuffer)(State&,const OOBase::SharedPtr<BufferObject>&);
		GLFWglproc m_fn_glUnmapNamedBuffer;
		void check_glUnmapNamedBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer);
		void call_glUnmapNamedBufferEXT(State& state, const OOBase::SharedPtr<BufferObject>& buffer);
		void call_glUnmapBuffer(State& state, const OOBase::SharedPtr<BufferObject>& buffer);
	};
}

#endif // INDIGO_STATE_H_INCLUDED
