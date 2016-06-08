///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
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

#include "../include/indigo/Quad.h"

#include "../include/indigo/ShaderPool.h"

#include "Common.h"

namespace
{
	class QuadFactory
	{
	public:
		void draw(OOGL::State& state, const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::mat4& mvp, const glm::vec4& colour);

	private:
		struct vertex_data
		{
			GLfloat x;
			GLfloat y;
			GLushort u;
			GLushort v;
		};

		OOBase::SharedPtr<OOGL::Program> m_ptrProgram;
		OOBase::SharedPtr<OOGL::VertexArrayObject> m_ptrVAO;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements;

		bool alloc();
	};

	const unsigned int elements_per_quad = 6;
}

bool QuadFactory::alloc()
{
	GLubyte e[elements_per_quad] = { 0,1,2,2,1,3 };
	vertex_data a[4] =
	{
		{ 0.f, 1.f, 0, 0},
		{ 0.f, 0.f, 0, 0xFFFF},
		{ 1.f, 1.f, 0xFFFF, 0 },
		{ 1.f, 0.f, 0xFFFF, 0xFFFF }
	};

	m_ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();
	if (!m_ptrVAO)
		LOG_ERROR_RETURN(("Failed to allocate VAO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
	m_ptrVAO->bind();

	OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(a),a);
	OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(e),e);
	if (!m_ptrVertices || !m_ptrElements)
		LOG_ERROR_RETURN(("Failed to allocate VBO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = Indigo::ShaderPool::add_shader("2d_textured_colour.vert",GL_VERTEX_SHADER,Indigo::static_resources());
	shaders[1] = Indigo::ShaderPool::add_shader("colour_blend.frag",GL_FRAGMENT_SHADER,Indigo::static_resources());
	if (shaders[0] && shaders[1])
		m_ptrProgram = Indigo::ShaderPool::add_program("2d_image",shaders,2);

	if (!m_ptrProgram)
		return false;

	GLint pos = m_ptrProgram->attribute_location("in_Position");
	if (pos == -1)
		return false;

	m_ptrVAO->attribute(pos,m_ptrVertices,2,GL_FLOAT,false,sizeof(vertex_data),offsetof(vertex_data,x));
	m_ptrVAO->enable_attribute(pos);

	pos = m_ptrProgram->attribute_location("in_TexCoord");
	if (pos == -1)
		return false;

	m_ptrVAO->attribute(pos,m_ptrVertices,2,GL_UNSIGNED_SHORT,true,sizeof(vertex_data),offsetof(vertex_data,u));
	m_ptrVAO->enable_attribute(pos);

	m_ptrVAO->element_array(m_ptrElements);
	m_ptrVAO->unbind();

	return true;
}

void QuadFactory::draw(OOGL::State& glState, const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::mat4& mvp, const glm::vec4& colour)
{
	if (!m_ptrProgram)
		alloc();

	if (m_ptrProgram)
	{
		glState.use(m_ptrProgram);
		glState.bind(0,texture);

		m_ptrProgram->uniform("in_Colour",colour);
		m_ptrProgram->uniform("MVP",mvp);

		m_ptrVAO->draw_elements(GL_TRIANGLES,0,3,elements_per_quad,GL_UNSIGNED_BYTE,0);
	}
}

void Indigo::Render::Quad::draw(OOGL::State& state, const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::mat4& mvp, const glm::vec4& colour)
{
	if (texture && colour.a > 0.f)
		OOGL::ContextSingleton<QuadFactory>::instance().draw(state,texture,mvp,colour);
}
