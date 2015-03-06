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

Indigo::VertexArrayObject::VertexArrayObject() : m_array(0)
{
	StateFns::get_current()->glGenVertexArrays(1,&m_array);
}

Indigo::VertexArrayObject::~VertexArrayObject()
{
	StateFns::get_current()->glDeleteVertexArrays(1,&m_array);
}

void Indigo::VertexArrayObject::bind()
{
	StateFns::get_current()->glBindVertexArray(m_array);
}

// This all needs to move to state!!

void Indigo::VertexArrayObject::draw(GLenum mode, GLint first, GLsizei count)
{
	StateFns::get_current()->glDrawArrays(mode,first,count);
}

void Indigo::VertexArrayObject::draw(GLenum mode, GLint first, GLsizei count, GLsizei instances, GLuint baseinstance)
{
	if (baseinstance)
		StateFns::get_current()->glDrawArraysInstancedBaseInstance(mode,first,count,instances,baseinstance);
	else
		StateFns::get_current()->glDrawArraysInstanced(mode,first,count,instances);
}

void Indigo::VertexArrayObject::draw(GLenum mode, const GLint* firsts, const GLsizei* counts, GLsizei primcount)
{
	StateFns::get_current()->glMultiDrawArrays(mode,firsts,counts,primcount);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	if (basevertex)
		StateFns::get_current()->glDrawElementsBaseVertex(mode,count,type,offset,basevertex);
	else
		StateFns::get_current()->glDrawElements(mode,count,type,offset);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, GLsizei count, GLenum type, GLsizeiptr offset, GLsizei instances, GLint basevertex, GLuint baseinstance)
{
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

void Indigo::VertexArrayObject::draw_elements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLsizeiptr offset, GLint basevertex)
{
	if (!basevertex)
		StateFns::get_current()->glDrawRangeElements(mode,start,end,count,type,offset);
	else
		StateFns::get_current()->glDrawRangeElementsBaseVertex(mode,start,end,count,type,offset,basevertex);
}

void Indigo::VertexArrayObject::draw_elements(GLenum mode, const GLsizei* counts, GLenum type, const GLsizeiptr* offsets, GLsizei primcount, const GLint* basevertices)
{
	if (!basevertices)
		StateFns::get_current()->glMultiDrawElements(mode,counts,type,offsets,primcount);
	else
		StateFns::get_current()->glMultiDrawElementsBaseVertex(mode,counts,type,offsets,primcount,basevertices);
}
