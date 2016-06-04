///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Rick Taylor
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

#include "../../include/indigo/sg/SGPrimitive.h"

#include "../../include/indigo/ShaderPool.h"

#include "../Common.h"

namespace
{
	class Cube : public Indigo::Render::SGNode
	{
	public:
		Cube(Indigo::Render::SGGroup* parent, const glm::vec4& colour, bool visible = false, const glm::mat4& local_transform = glm::mat4());

		void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;

	private:
		glm::vec4 m_colour;
	};

	class CubeFactory
	{
	public:
		void draw(OOGL::State& state, const glm::mat4& mvp, const glm::vec4& colour);

	private:
		struct vertex_data
		{
			GLfloat x;
			GLfloat y;
			GLfloat z;
		};

		OOBase::SharedPtr<OOGL::Program> m_ptrProgram;
		OOBase::SharedPtr<OOGL::VertexArrayObject> m_ptrVAO;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements;

		bool alloc();
	};

	const unsigned int elements_per_cube = 36;
}

bool CubeFactory::alloc()
{
	// cube ///////////////////////////////////////////////////////////////////////
	//    v6----- v5
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | |v7---|-|v4
	//  |/      |/
	//  v2------v3

	vertex_data vertices[] =
	{
		{ 1.f, 1.f, 1.f},  {-1.f, 1.f, 1.f},  {-1.f,-1.f, 1.f},  { 1.f,-1.f, 1.f},   // v0,v1,v2,v3 (front)
        { 1.f, 1.f, 1.f},  { 1.f,-1.f, 1.f},  { 1.f,-1.f,-1.f},  { 1.f, 1.f,-1.f},   // v0,v3,v4,v5 (right)
        { 1.f, 1.f, 1.f},  { 1.f, 1.f,-1.f},  {-1.f, 1.f,-1.f},  {-1.f, 1.f, 1.f},   // v0,v5,v6,v1 (top)
        {-1.f, 1.f, 1.f},  {-1.f, 1.f,-1.f},  {-1.f,-1.f,-1.f},  {-1.f,-1.f, 1.f},   // v1,v6,v7,v2 (left)
        {-1.f,-1.f,-1.f},  { 1.f,-1.f,-1.f},  { 1.f,-1.f, 1.f},  {-1.f,-1.f, 1.f},   // v7,v4,v3,v2 (bottom)
        { 1.f,-1.f,-1.f},  {-1.f,-1.f,-1.f},  {-1.f, 1.f,-1.f},  { 1.f, 1.f,-1.f}    // v4,v7,v6,v5 (back)
	};

	GLubyte indices[elements_per_cube] =
	{
		 0, 1, 2,   2, 3, 0,      // front
		 4, 5, 6,   6, 7, 4,      // right
		 8, 9,10,  10,11, 8,      // top
		12,13,14,  14,15,12,      // left
		16,17,18,  18,19,16,      // bottom
		20,21,22,  22,23,20       // back
	};

	OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(vertices),vertices);
	OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(indices),indices);
	if (!m_ptrVertices || !m_ptrElements)
		LOG_ERROR_RETURN(("Failed to allocate VBO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	m_ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();
	if (!m_ptrVAO)
		LOG_ERROR_RETURN(("Failed to allocate VAO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = Indigo::ShaderPool::add_shader("3d_colour.vert",GL_VERTEX_SHADER,Indigo::static_resources());
	shaders[1] = Indigo::ShaderPool::add_shader("colour.frag",GL_FRAGMENT_SHADER,Indigo::static_resources());
	if (shaders[0] && shaders[1])
		m_ptrProgram = Indigo::ShaderPool::add_program("3d_cube",shaders,2);

	if (!m_ptrProgram)
		return false;

	GLint pos = m_ptrProgram->attribute_location("in_Position");
	if (pos == -1)
		return false;

	m_ptrVAO->attribute(pos,m_ptrVertices,3,GL_FLOAT,false,sizeof(vertex_data),offsetof(vertex_data,x));
	m_ptrVAO->enable_attribute(pos);

	m_ptrVAO->element_array(m_ptrElements);
	m_ptrVAO->unbind();

	return true;
}

void CubeFactory::draw(OOGL::State& glState, const glm::mat4& mvp, const glm::vec4& colour)
{
	if (!m_ptrProgram)
		alloc();

	if (m_ptrProgram)
	{
		glState.use(m_ptrProgram);
		
		m_ptrProgram->uniform("in_Colour",colour);
		m_ptrProgram->uniform("MVP",mvp);

		m_ptrVAO->draw_elements(GL_TRIANGLES,elements_per_cube,GL_UNSIGNED_BYTE,0);
	}
}

Cube::Cube(Indigo::Render::SGGroup* parent, const glm::vec4& colour, bool visible, const glm::mat4& local_transform) :
		Indigo::Render::SGNode(parent,visible,local_transform),
		m_colour(colour)
{
}

void Cube::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_colour.a > 0.f)
		OOGL::ContextSingleton<CubeFactory>::instance().draw(glState,mvp,m_colour);
}

OOBase::SharedPtr<Indigo::Render::SGNode> Indigo::SGCube::on_render_create(Render::SGGroup* parent)
{
	OOBase::SharedPtr<Indigo::Render::SGNode> node = OOBase::allocate_shared< ::Cube>(parent,m_colour,visible(),transform());
	if (!node)
		LOG_ERROR(("Failed to allocate: %s\n",OOBase::system_error_text()));
	return node;
}
