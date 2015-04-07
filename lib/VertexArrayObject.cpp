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

#include "VertexArrayObject.h"
#include "State.h"
#include "StateFns.h"
#include "BufferObject.h"

OOGL::VertexArrayObject::VertexArrayObject() : m_array(0)
{
	StateFns::get_current()->glGenVertexArrays(1,&m_array);
}

OOGL::VertexArrayObject::VertexArrayObject(GLuint array) : m_array(array)
{
}

OOBase::SharedPtr<OOGL::VertexArrayObject> OOGL::VertexArrayObject::none()
{
	return OOBase::allocate_shared<VertexArrayObject,OOBase::ThreadLocalAllocator>(0);
}

OOGL::VertexArrayObject::~VertexArrayObject()
{
	if (m_array)
		StateFns::get_current()->glDeleteVertexArrays(1,&m_array);
}

void OOGL::VertexArrayObject::bind()
{
	StateFns::get_current()->glBindVertexArray(m_array);
}

void OOGL::VertexArrayObject::attribute(GLuint index, const OOBase::SharedPtr<BufferObject>& buffer, GLint components, GLenum type, GLsizei stride, GLsizeiptr offset)
{
	assert(buffer->target() == GL_ARRAY_BUFFER);
	State::get_current()->bind(buffer);

	StateFns::get_current()->glVertexAttribIPointer(index,components,type,stride,reinterpret_cast<const GLvoid*>(offset));
}

void OOGL::VertexArrayObject::attribute(GLuint index, const OOBase::SharedPtr<BufferObject>& buffer, GLint components, GLenum type, bool normalized, GLsizei stride, GLsizeiptr offset)
{
	assert(buffer->target() == GL_ARRAY_BUFFER);
	State::get_current()->bind(buffer);

	StateFns::get_current()->glVertexAttribPointer(index,components,type,normalized ? GL_TRUE : GL_FALSE,stride,reinterpret_cast<const GLvoid*>(offset));
}

void OOGL::VertexArrayObject::enable_attribute(GLuint index, bool enable)
{
	if (enable)
		StateFns::get_current()->glEnableVertexArrayAttrib(*State::get_current(),shared_from_this(),index);
	else
		StateFns::get_current()->glDisableVertexArrayAttrib(*State::get_current(),shared_from_this(),index);
}

void OOGL::VertexArrayObject::draw(GLenum mode, GLint first, GLsizei count)
{
	State::get_current()->bind(shared_from_this());
	glDrawArrays(mode,first,count);

	OOGL_CHECK("glDrawArrays");
}

void OOGL::VertexArrayObject::draw_instanced(GLenum mode, GLint first, GLsizei count, GLsizei instances)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glDrawArraysInstanced(mode,first,count,instances);
}

void OOGL::VertexArrayObject::draw_instanced(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance)
{
	State::get_current()->bind(shared_from_this());
	if (baseinstance)
		StateFns::get_current()->glDrawArraysInstancedBaseInstance(mode,first,count,instances,baseinstance);
	else
		StateFns::get_current()->glDrawArraysInstanced(mode,first,count,instances);
}

void OOGL::VertexArrayObject::draw(GLenum mode, const GLint* firsts, const GLsizei* counts, GLsizei primcount)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glMultiDrawArrays(mode,firsts,counts,primcount);
}

void OOGL::VertexArrayObject::draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset)
{
	State::get_current()->bind(shared_from_this());
	glDrawElements(mode,count,type,(const void*)offset);

	OOGL_CHECK("glDrawElements");
}

void OOGL::VertexArrayObject::draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	State::get_current()->bind(shared_from_this());
	if (basevertex)
		StateFns::get_current()->glDrawElementsBaseVertex(mode,count,type,offset,basevertex);
	else
	{
		glDrawElements(mode,count,type,(const void*)offset);
		OOGL_CHECK("glDrawElements");
	}
}

void OOGL::VertexArrayObject::draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glDrawRangeElements(mode,start,end,count,type,offset);
}

void OOGL::VertexArrayObject::draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	State::get_current()->bind(shared_from_this());
	if (!basevertex)
		StateFns::get_current()->glDrawRangeElements(mode,start,end,count,type,offset);
	else
		StateFns::get_current()->glDrawRangeElementsBaseVertex(mode,start,end,count,type,offset,basevertex);
}

void OOGL::VertexArrayObject::draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glMultiDrawElements(mode,counts,type,offsets,primcount);
}

void OOGL::VertexArrayObject::draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint* basevertices)
{
	State::get_current()->bind(shared_from_this());
	if (!basevertices)
		StateFns::get_current()->glMultiDrawElements(mode,counts,type,offsets,primcount);
	else
		StateFns::get_current()->glMultiDrawElementsBaseVertex(mode,counts,type,offsets,primcount,basevertices);
}

void OOGL::VertexArrayObject::draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glDrawElementsInstanced(mode,count,type,offset,instances);
}

void OOGL::VertexArrayObject::draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex)
{
	State::get_current()->bind(shared_from_this());
	if (!basevertex)
		StateFns::get_current()->glDrawElementsInstanced(mode,count,type,offset,instances);
	else
		StateFns::get_current()->glDrawElementsInstancedBaseVertex(mode,count,type,offset,instances,basevertex);
}

void OOGL::VertexArrayObject::draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex, GLuint baseinstance)
{
	State::get_current()->bind(shared_from_this());
	if (!basevertex)
	{
		if (!baseinstance)
			StateFns::get_current()->glDrawElementsInstanced(mode,count,type,offset,instances);
		else
			StateFns::get_current()->glDrawElementsInstancedBaseInstance(mode,count,type,offset,instances,baseinstance);
	}
	else
	{
		if (!baseinstance)
			StateFns::get_current()->glDrawElementsInstancedBaseVertex(mode,count,type,offset,instances,basevertex);
		else
			StateFns::get_current()->glDrawElementsInstancedBaseVertexBaseInstance(mode,count,type,offset,instances,basevertex,baseinstance);
	}
}
