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

#ifndef INDIGO_STATE_FNS_H_INCLUDED
#define INDIGO_STATE_FNS_H_INCLUDED

#include <OOBase/SharedPtr.h>

#include "OOGL.h"

namespace OOGL
{
	class State;
	class BufferObject;
	class VertexArrayObject;
	class Texture;

	class StateFns : public OOBase::NonCopyable
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

	public:
		static OOBase::SharedPtr<StateFns> get_current();

		bool isGLversion(int major, int minor);

		void enable_logging();

		void glGenFramebuffers(GLsizei n, GLuint *framebuffers);
		void glDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
		void glBindFramebuffer(GLenum target, GLuint framebuffer);
		GLenum glCheckFramebufferStatus(GLenum target);

		GLuint glCreateShader(GLenum shaderType);
		void glDeleteShader(GLuint shader);
		void glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
		void glCompileShader(GLuint shader);
		void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
		void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);

		GLuint glCreateProgram();
		void glDeleteProgram(GLuint program);
		void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
		void glGetProgramInfoLog(GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
		void glAttachShader(GLuint program, GLuint shader);
		void glDetachShader(GLuint program, GLuint shader);
		void glLinkProgram(GLuint program);
		void glUseProgram(GLuint program);
		GLint glGetAttribLocation(GLuint program, const char* name);
		GLint glGetUniformLocation(GLuint program, const char* name);
		void glUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);
		void glUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);
		void glUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v);

		bool check_glTextureArray();
		bool check_glTexture3D();
		void glActiveTexture(GLuint unit);
		void glBindTextureUnit(State& state, GLuint unit, GLenum target, GLuint texture);
		void glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
		bool check_glTextureStorage();
		void glTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void glTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void glTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void glTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void glTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void glTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void glTextureSubImage1D(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void glTextureSubImage2D(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void glTextureSubImage3D(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void glTextureParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val);
		void glTextureParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* pval);
		void glTextureParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val);
		void glTextureParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* val);
		bool check_glGenerateMipmap();
		void glGenerateTextureMipmap(GLuint texture, GLenum target);

		void glGenBuffers(GLsizei n, GLuint* buffers);
		void glBindBuffer(GLenum target, GLuint buffer);
		void glDeleteBuffers(GLsizei n, const GLuint* buffers);
		void glBufferData(GLuint buffer, GLenum target, GLsizeiptr size, const void* data, GLenum usage);
		void* glMapBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		bool glUnmapBuffer(const OOBase::SharedPtr<BufferObject>& buffer);
		void glBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data);
		void glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size);

		void glGenVertexArrays(GLsizei n, GLuint* arrays);
		void glBindVertexArray(GLuint array);
		void glDeleteVertexArrays(GLsizei n, const GLuint* arrays);
		void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
		bool check_glVertexAttribIPointer();
		void glVertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
		void glEnableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);
		void glDisableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);

		void glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
		void glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount);
		void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex);
		bool check_glDrawElementsBaseVertex();
		void glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex);
		void glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset);
		bool check_glDrawRangeElementsBaseVertex();
		void glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex);
		bool check_glDrawInstanced();
		void glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instances);
		bool check_glDrawInstancedBaseInstance();
		void glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance);
		void glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances);
		bool check_glDrawElementsInstancedBaseVertex();
		void glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex);
		bool check_glDrawElementsInstancedBaseInstance();
		void glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLuint baseinstance);
		bool check_glDrawElementsInstancedBaseVertexBaseInstance();
		void glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex, GLuint baseinstance);

	private:
		StateFns();

		int m_gl_major;
		int m_gl_minor;
		bool m_logging;

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
		PFNGLCREATEPROGRAMPROC m_fn_glCreateProgram;
		PFNGLDELETEPROGRAMPROC m_fn_glDeleteProgram;
		PFNGLGETPROGRAMIVPROC m_fn_glGetProgramiv;
		PFNGLGETPROGRAMINFOLOGPROC m_fn_glGetProgramInfoLog;
		PFNGLATTACHSHADERPROC m_fn_glAttachShader;
		PFNGLDETACHSHADERPROC m_fn_glDetachShader;
		PFNGLLINKPROGRAMPROC m_fn_glLinkProgram;
		PFNGLUSEPROGRAMPROC m_fn_glUseProgram;
		PFNGLGETATTRIBLOCATIONPROC m_fn_glGetAttribLocation;
		PFNGLGETUNIFORMLOCATIONPROC m_fn_glGetUniformLocation;

		void (StateFns::*m_thunk_glUniform3fv)(const OOBase::SharedPtr<Program>&,GLint,GLsizei,const GLfloat*);
		GLFWglproc m_fn_glUniform3fv;
		void check_glUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);
		void call_glProgramUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);
		void call_glUniform3fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);

		void (StateFns::*m_thunk_glUniform4fv)(const OOBase::SharedPtr<Program>&,GLint,GLsizei,const GLfloat*);
		GLFWglproc m_fn_glUniform4fv;
		void check_glUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);
		void call_glProgramUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);
		void call_glUniform4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, const GLfloat* v);

		void (StateFns::*m_thunk_glUniformMatrix4fv)(const OOBase::SharedPtr<Program>&,GLint,GLsizei,GLboolean,const GLfloat*);
		GLFWglproc m_fn_glUniformMatrix4fv;
		void check_glUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v);
		void call_glProgramUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v);
		void call_glUniformMatrix4fv(const OOBase::SharedPtr<Program>& program, GLint location, GLsizei count, GLboolean transpose, const GLfloat* v);

		PFNGLACTIVETEXTUREPROC m_fn_glActiveTexture;

		void (StateFns::*m_thunk_glBindTextureUnit)(State&,GLuint,GLenum,GLuint);
		GLFWglproc m_fn_glBindTextureUnit;
		void check_glBindTextureUnit(State& state, GLuint unit, GLenum target, GLuint texture);
		void emulate_glBindTextureUnit(State& state, GLuint unit, GLenum target, GLuint texture);
		void call_glMultiBindTexture(State& state, GLuint unit, GLenum target, GLuint texture);
		void call_glBindTextureUnit(State& state, GLuint unit, GLenum target, GLuint texture);

		PFNGLTEXIMAGE3DPROC m_fn_glTexImage3D;

		void (StateFns::*m_thunk_glTextureStorage1D)(GLuint,GLenum,GLsizei,GLenum,GLsizei);
		GLFWglproc m_fn_glTextureStorage1D;
		void check_glTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void call_glTextureStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void call_glTextureStorage1DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);
		void call_glTexStorage1D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width);

		void (StateFns::*m_thunk_glTextureStorage2D)(GLuint,GLenum,GLsizei,GLenum,GLsizei,GLsizei);
		GLFWglproc m_fn_glTextureStorage2D;
		void check_glTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void call_glTextureStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void call_glTextureStorage2DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);
		void call_glTexStorage2D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height);

		void (StateFns::*m_thunk_glTextureStorage3D)(GLuint,GLenum,GLsizei,GLenum,GLsizei,GLsizei,GLsizei);
		GLFWglproc m_fn_glTextureStorage3D;
		void check_glTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void call_glTextureStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void call_glTextureStorage3DEXT(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);
		void call_glTexStorage3D(GLuint texture, GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth);

		void (StateFns::*m_thunk_glTextureSubImage1D)(GLuint,GLenum,GLint,GLint,GLsizei,GLenum,GLenum,const void*);
		void (StateFns::*m_thunk_glTextureSubImage1D_2)(const OOBase::SharedPtr<Texture>&,GLint,GLint,GLsizei,GLenum,GLenum,const void*);
		GLFWglproc m_fn_glTextureSubImage1D;
		void check_glTextureSubImage1D();
		void check_glTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void check_glTextureSubImage1D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage1D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage1DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage1DEXT_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage1D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage1D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels);

		void (StateFns::*m_thunk_glTextureSubImage2D)(GLuint,GLenum,GLint,GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,const void*);
		void (StateFns::*m_thunk_glTextureSubImage2D_2)(const OOBase::SharedPtr<Texture>&,GLint,GLint,GLint,GLsizei,GLsizei,GLenum,GLenum,const void*);
		GLFWglproc m_fn_glTextureSubImage2D;
		void check_glTextureSubImage2D();
		void check_glTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void check_glTextureSubImage2D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage2D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage2DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage2DEXT_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage2D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage2D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

		void (StateFns::*m_thunk_glTextureSubImage3D)(GLuint,GLenum,GLint,GLint,GLint,GLint,GLsizei,GLsizei,GLsizei,GLenum,GLenum,const void*);
		void (StateFns::*m_thunk_glTextureSubImage3D_2)(const OOBase::SharedPtr<Texture>&,GLint,GLint,GLint,GLint,GLsizei,GLsizei,GLsizei,GLenum,GLenum,const void*);
		GLFWglproc m_fn_glTextureSubImage3D;
		bool check_glTextureSubImage3D();
		void check_glTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void check_glTextureSubImage3D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage3D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage3DEXT(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTextureSubImage3DEXT_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage3D(GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
		void call_glTexSubImage3D_2(const OOBase::SharedPtr<Texture>& texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);

		void (StateFns::*m_thunk_glTextureParameterf)(const OOBase::SharedPtr<Texture>&,GLenum,GLfloat);
		GLFWglproc m_fn_glTextureParameterf;
		void check_glTextureParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val);
		void call_glTextureParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val);
		void call_glTextureParameterfEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val);
		void call_glTexParameterf(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLfloat val);

		void (StateFns::*m_thunk_glTextureParameterfv)(const OOBase::SharedPtr<Texture>&,GLenum,const GLfloat*);
		GLFWglproc m_fn_glTextureParameterfv;
		void check_glTextureParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* pval);
		void call_glTextureParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* pval);
		void call_glTextureParameterfvEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* pval);
		void call_glTexParameterfv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLfloat* pval);

		void (StateFns::*m_thunk_glTextureParameteri)(const OOBase::SharedPtr<Texture>&,GLenum,GLint);
		GLFWglproc m_fn_glTextureParameteri;
		void check_glTextureParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val);
		void call_glTextureParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val);
		void call_glTextureParameteriEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val);
		void call_glTexParameteri(const OOBase::SharedPtr<Texture>& texture, GLenum name, GLint val);

		void (StateFns::*m_thunk_glTextureParameteriv)(const OOBase::SharedPtr<Texture>&,GLenum,const GLint*);
		GLFWglproc m_fn_glTextureParameteriv;
		void check_glTextureParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* pval);
		void call_glTextureParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* pval);
		void call_glTextureParameterivEXT(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* pval);
		void call_glTexParameteriv(const OOBase::SharedPtr<Texture>& texture, GLenum name, const GLint* pval);

		void (StateFns::*m_thunk_glGenerateTextureMipmap)(GLuint,GLenum);
		GLFWglproc m_fn_glGenerateTextureMipmap;
		void check_glGenerateTextureMipmap(GLuint texture, GLenum target);
		void call_glGenerateTextureMipmap(GLuint texture, GLenum target);
		void call_glGenerateTextureMipmapEXT(GLuint texture, GLenum target);
		void call_glGenerateMipmap(GLuint texture, GLenum target);

		PFNGLGENBUFFERSPROC m_fn_glGenBuffers;
		PFNGLBINDBUFFERPROC m_fn_glBindBuffer;
		PFNGLDELETEBUFFERSPROC m_fn_glDeleteBuffers;

		void (StateFns::*m_thunk_glBufferData)(GLuint,GLenum,GLsizeiptr,const void*,GLenum);
		GLFWglproc m_fn_glBufferData;
		void check_glBufferData(GLuint buffer, GLenum target, GLsizeiptr size, const void *data, GLenum usage);
		void call_glNamedBufferData(GLuint buffer, GLenum target, GLsizeiptr size, const void *data, GLenum usage);
		void call_glBufferData(GLuint buffer, GLenum target, GLsizeiptr size, const void *data, GLenum usage);

		void* (StateFns::*m_thunk_glMapBufferRange)(const OOBase::SharedPtr<BufferObject>&,GLintptr,GLsizeiptr,GLenum,GLsizeiptr,GLbitfield);
		GLFWglproc m_fn_glMapBufferRange;
		void* check_glMapBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		void* call_glMapNamedBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		void* call_glMapBufferRange(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);
		void* call_glMapBuffer(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr length, GLenum orig_usage, GLsizeiptr orig_size, GLbitfield access);

		bool (StateFns::*m_thunk_glUnmapBuffer)(const OOBase::SharedPtr<BufferObject>&);
		GLFWglproc m_fn_glUnmapBuffer;
		bool check_glUnmapBuffer(const OOBase::SharedPtr<BufferObject>& buffer);
		bool call_glUnmapNamedBuffer(const OOBase::SharedPtr<BufferObject>& buffer);
		bool call_glUnmapBuffer(const OOBase::SharedPtr<BufferObject>& buffer);

		void (StateFns::*m_thunk_glBufferSubData)(const OOBase::SharedPtr<BufferObject>&,GLintptr,GLsizeiptr,const void*);
		GLFWglproc m_fn_glBufferSubData;
		void check_glBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data);
		void call_glNamedBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data);
		void call_glBufferSubData(const OOBase::SharedPtr<BufferObject>& buffer, GLintptr offset, GLsizeiptr size, const void* data);

		void (StateFns::*m_thunk_glCopyBufferSubData)(const OOBase::SharedPtr<BufferObject>&,GLintptr,const OOBase::SharedPtr<BufferObject>&,GLintptr,GLsizeiptr);
		GLFWglproc m_fn_glCopyBufferSubData;
		void check_glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size);
		void call_glCopyNamedBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size);
		void call_glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size);
		void emulate_glCopyBufferSubData(const OOBase::SharedPtr<BufferObject>& write, GLintptr writeoffset, const OOBase::SharedPtr<BufferObject>& read, GLintptr readoffset, GLsizeiptr size);

		PFNGLGENVERTEXARRAYSPROC m_fn_glGenVertexArrays;
		PFNGLDELETEVERTEXARRAYSPROC m_fn_glDeleteVertexArrays;
		PFNGLBINDVERTEXARRAYPROC m_fn_glBindVertexArray;

		PFNGLVERTEXATTRIBPOINTERPROC m_fn_glVertexAttribPointer;
		PFNGLVERTEXATTRIBIPOINTERPROC m_fn_glVertexAttribIPointer;

		void (StateFns::*m_thunk_glEnableVertexArrayAttrib)(const OOBase::SharedPtr<VertexArrayObject>&,GLuint);
		GLFWglproc m_fn_glEnableVertexArrayAttrib;
		void check_glEnableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);
		void call_glEnableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);
		void call_glEnableVertexAttribArray(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);

		void (StateFns::*m_thunk_glDisableVertexArrayAttrib)(const OOBase::SharedPtr<VertexArrayObject>&,GLuint);
		GLFWglproc m_fn_glDisableVertexArrayAttrib;
		void check_glDisableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);
		void call_glDisableVertexArrayAttrib(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);
		void call_glDisableVertexAttribArray(const OOBase::SharedPtr<VertexArrayObject>& vao, GLuint index);

		void (StateFns::*m_thunk_glMultiDrawArrays)(GLenum,const GLint*,const GLsizei*,GLsizei);
		GLFWglproc m_fn_glMultiDrawArrays;
		void check_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
		void call_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
		void emulate_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);

		void (StateFns::*m_thunk_glDrawArraysInstanced)(GLenum,GLint,GLsizei,GLsizei);
		GLFWglproc m_fn_glDrawArraysInstanced;
		void check_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instances);
		void call_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instances);

		void (StateFns::*m_thunk_glDrawArraysInstancedBaseInstance)(GLenum,GLint,GLsizei,GLsizei,GLuint);
		PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC m_fn_glDrawArraysInstancedBaseInstance;
		void check_glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance);
		void call_glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance);

		void (StateFns::*m_thunk_glDrawRangeElements)(GLenum,GLuint,GLuint,GLsizei,GLenum,GLsizeiptr);
		GLFWglproc m_fn_glDrawRangeElements;
		void check_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset);
		void call_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset);

		void (StateFns::*m_thunk_glDrawRangeElementsBaseVertex)(GLenum,GLuint,GLuint,GLsizei,GLenum,GLsizeiptr,GLint);
		PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC m_fn_glDrawRangeElementsBaseVertex;
		void check_glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertext);
		void call_glDrawRangeElementsBaseVertex(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex);
		
		void (StateFns::*m_thunk_glDrawElementsBaseVertex)(GLenum,GLsizei,GLenum,GLsizeiptr,GLint);
		PFNGLDRAWELEMENTSBASEVERTEXPROC m_fn_glDrawElementsBaseVertex;
		void check_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex);
		void call_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex);
		
		void (StateFns::*m_thunk_glMultiDrawElements)(GLenum,const GLsizei*,GLenum,const GLsizeiptr*,GLsizei);
		GLFWglproc m_fn_glMultiDrawElements;
		void check_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount);
		void call_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount);
		void emulate_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount);

		void (StateFns::*m_thunk_glMultiDrawElementsBaseVertex)(GLenum,const GLsizei*,GLenum,const GLsizeiptr*,GLsizei,const GLint*);
		PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC m_fn_glMultiDrawElementsBaseVertex;
		void check_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex);
		void call_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex);
		void emulate_glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei *count, GLenum type, const GLsizeiptr* offsets, GLsizei drawcount, const GLint *basevertex);

		void (StateFns::*m_thunk_glDrawElementsInstanced)(GLenum,GLsizei,GLenum,GLsizeiptr,GLsizei);
		GLFWglproc m_fn_glDrawElementsInstanced;
		void check_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances);
		void call_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances);

		void (StateFns::*m_thunk_glDrawElementsInstancedBaseVertex)(GLenum,GLsizei,GLenum,GLsizeiptr,GLsizei,GLint);
		PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC m_fn_glDrawElementsInstancedBaseVertex;
		void check_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex);
		void call_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex);

		void (StateFns::*m_thunk_glDrawElementsInstancedBaseInstance)(GLenum,GLsizei,GLenum,GLsizeiptr,GLsizei,GLuint);
		PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC m_fn_glDrawElementsInstancedBaseInstance;
		void check_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLuint baseinstance);
		void call_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLuint baseinstance);

		void (StateFns::*m_thunk_glDrawElementsInstancedBaseVertexBaseInstance)(GLenum,GLsizei,GLenum,GLsizeiptr,GLsizei,GLint,GLuint);
		PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC m_fn_glDrawElementsInstancedBaseVertexBaseInstance;
		void check_glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
		void call_glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
	};
}

#endif // INDIGO_STATE_H_INCLUDED
