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

#include "BufferObject.h"
#include "State.h"
#include "StateFns.h"

Indigo::BufferObject::BufferObject(GLenum target, GLsizeiptr size, GLenum usage) : m_buffer(0), m_target(target)
{
	StateFns::get_current()->glGenBuffers(1,&m_buffer);
	State::get_current()->buffer_init(m_target,m_buffer,size,usage);
}

Indigo::BufferObject::~BufferObject()
{
	StateFns::get_current()->glDeleteBuffers(1,&m_buffer);
}
