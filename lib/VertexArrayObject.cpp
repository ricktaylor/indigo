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

#include "VertexArrayObject.h"
#include "State.h"
#include "StateFns.h"

Indigo::VertexArrayObject::VertexArrayObject() : m_array(0)
{
	StateFns::get_current()->glGenVertexArrays(1,&m_array);
}

Indigo::VertexArrayObject::VertexArrayObject(GLuint array) : m_array(array)
{
}

OOBase::SharedPtr<Indigo::VertexArrayObject> Indigo::VertexArrayObject::none()
{
	return OOBase::allocate_shared<VertexArrayObject,OOBase::ThreadLocalAllocator>(0);
}

Indigo::VertexArrayObject::~VertexArrayObject()
{
	if (m_array)
		StateFns::get_current()->glDeleteVertexArrays(1,&m_array);
}

void Indigo::VertexArrayObject::bind()
{
	StateFns::get_current()->glBindVertexArray(m_array);
}

void Indigo::VertexArrayObject::draw(GLenum mode, GLint first, GLsizei count)
{
	State::get_current()->bind(shared_from_this());
	glDrawArrays(mode,first,count);
}

void Indigo::VertexArrayObject::draw_instanced(GLenum mode, GLint first, GLsizei count, GLsizei instances)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glDrawArraysInstanced(mode,first,count,instances);
}

void Indigo::VertexArrayObject::draw_instanced(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance)
{
	State::get_current()->bind(shared_from_this());
	if (baseinstance)
		StateFns::get_current()->glDrawArraysInstancedBaseInstance(mode,first,count,instances,baseinstance);
	else
		StateFns::get_current()->glDrawArraysInstanced(mode,first,count,instances);
}

void Indigo::VertexArrayObject::draw(GLenum mode, const GLint* firsts, const GLsizei* counts, GLsizei primcount)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glMultiDrawArrays(mode,firsts,counts,primcount);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset)
{
	State::get_current()->bind(shared_from_this());
	glDrawElements(mode,count,type,(const void*)offset);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	State::get_current()->bind(shared_from_this());
	if (basevertex)
		StateFns::get_current()->glDrawElementsBaseVertex(mode,count,type,offset,basevertex);
	else
		glDrawElements(mode,count,type,(const void*)offset);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glDrawRangeElements(mode,start,end,count,type,offset);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	State::get_current()->bind(shared_from_this());
	if (!basevertex)
		StateFns::get_current()->glDrawRangeElements(mode,start,end,count,type,offset);
	else
		StateFns::get_current()->glDrawRangeElementsBaseVertex(mode,start,end,count,type,offset,basevertex);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glMultiDrawElements(mode,counts,type,offsets,primcount);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint* basevertices)
{
	State::get_current()->bind(shared_from_this());
	if (!basevertices)
		StateFns::get_current()->glMultiDrawElements(mode,counts,type,offsets,primcount);
	else
		StateFns::get_current()->glMultiDrawElementsBaseVertex(mode,counts,type,offsets,primcount,basevertices);
}

void Indigo::VertexArrayObject::draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances)
{
	State::get_current()->bind(shared_from_this());
	StateFns::get_current()->glDrawElementsInstanced(mode,count,type,offset,instances);
}

void Indigo::VertexArrayObject::draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex)
{
	State::get_current()->bind(shared_from_this());
	if (!basevertex)
		StateFns::get_current()->glDrawElementsInstanced(mode,count,type,offset,instances);
	else
		StateFns::get_current()->glDrawElementsInstancedBaseVertex(mode,count,type,offset,instances,basevertex);
}

void Indigo::VertexArrayObject::draw_elements_instanced(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex, GLuint baseinstance)
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