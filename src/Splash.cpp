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

#include "Render.h"
#include "../lib/BufferObject.h"
#include "../lib/VertexArrayObject.h"
#include "../lib/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace
{
	class Splash : public OOBase::EnableSharedFromThis<Splash>
	{
	public:
		Splash();

		bool init();

	private:
		// Render thread local members
		OOBase::SharedPtr<OOGL::Window> m_wnd;
		float m_ratio;

		static bool create(void* p);

		void on_create();
		void on_draw(const OOGL::Window& win, OOGL::State& glState);
		void on_close(const OOGL::Window& win);

		OOBase::SharedPtr<Splash> self;

		OOBase::SharedPtr<OOGL::BufferObject> ptrVertices;
		OOBase::SharedPtr<OOGL::BufferObject> ptrColours;
		OOBase::SharedPtr<OOGL::VertexArrayObject> ptrVAO;
		OOBase::SharedPtr<OOGL::Program> ptrProgram;
	};
}

Splash::Splash() : m_ratio(0)
{
}

bool Splash::init()
{
	return Indigo::render_call(&create,this);
}

bool Splash::create(void* p)
{
	Splash* pThis = static_cast<Splash*>(p);

	pThis->m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(320,200,"Test");
	if (!pThis->m_wnd || !pThis->m_wnd->is_valid())
		return false;

	if (!pThis->m_wnd->signal_close.connect(OOBase::WeakPtr<Splash>(pThis->shared_from_this()),&Splash::on_close) ||
			!pThis->m_wnd->signal_draw.connect(OOBase::WeakPtr<Splash>(pThis->shared_from_this()),&Splash::on_draw))
		LOG_ERROR_RETURN(("Failed to attach signal"),false);

	pThis->on_create();

	if (!Indigo::monitor_window(pThis->m_wnd))
		LOG_ERROR_RETURN(("Failed to monitor window"),false);

	pThis->m_wnd->visible(true);
	pThis->self = pThis->shared_from_this();

	return true;
}

void Splash::on_create()
{
	glm::ivec2 sz = m_wnd->size();
	m_ratio = sz.x / (float)sz.y;
	glViewport(0, 0, sz.x, sz.y);

	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_VERTEX_SHADER);
	shaders[0]->compile(
			"#version 120\n"
			"attribute vec3 in_Position;\n"
			"attribute vec3 in_Colour;\n"
			"uniform mat4 MVP;\n"
			"varying vec3 pass_Colour;\n"
			"void main() {\n"
			"	pass_Colour = in_Colour;\n"
			"	vec4 v = vec4(in_Position,1.0);\n"
			"	gl_Position = MVP * v;\n"
			"}\n");
	OOBase::String s = shaders[0]->info_log();
	if (!s.empty())
		LOG_DEBUG(("%s",s.c_str()));

	shaders[1] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_FRAGMENT_SHADER);
	shaders[1]->compile(
			"#version 120\n"
			"varying vec3 pass_Colour;\n"
			"void main() {\n"
			"    gl_FragColor = vec4(pass_Colour,1.0);\n"
			"}\n");
	s = shaders[1]->info_log();
	if (!s.empty())
		LOG_DEBUG(("%s",s.c_str()));

	ptrProgram = OOBase::allocate_shared<OOGL::Program,OOBase::ThreadLocalAllocator>();
	ptrProgram->link(shaders,2);
	s = ptrProgram->info_log();
	if (!s.empty())
		LOG_DEBUG(("%s",s.c_str()));

	ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();

	float points[] = {
			-0.6f, -0.4f, 0.f,
			0.6f, -0.4f, 0.f,
			0.f, 0.6f, 0.f
	};
	ptrVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(points),points);

	float colours[] = {
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 0.f, 1.f
	};
	ptrColours = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(colours),colours);

	GLint a = ptrProgram->attribute_location("in_Position");
	ptrVAO->attribute(a,ptrVertices,3,GL_FLOAT,false);
	ptrVAO->enable_attribute(a);

	a = ptrProgram->attribute_location("in_Colour");
	ptrVAO->attribute(a,ptrColours,3,GL_FLOAT,false);
	ptrVAO->enable_attribute(a);
}

void Splash::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	glState.bind(GL_DRAW_FRAMEBUFFER,win.get_default_frame_buffer());

	// Always clear everything
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glm::mat4 proj = glm::ortho(-m_ratio, m_ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),m_ratio,0.1f,100.0f);
	//glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(3,0,0),glm::vec3(0,0,0),glm::vec3(0,1,0));
	view = glm::rotate(view,-glm::radians((float)glfwGetTime() * 50.f),glm::vec3(0,1,0));

	glm::mat4 model = glm::rotate(glm::mat4(1.0f),-glm::radians((float)glfwGetTime() * 50.f),glm::vec3(0,0,1));

	glm::mat4 MVP = proj * view * model;

	glState.use(ptrProgram);
	ptrProgram->uniform("MVP",MVP);

	ptrVAO->draw(GL_TRIANGLES,0,3);
}

void Splash::on_close(const OOGL::Window& win)
{
	self.reset();
}

bool showSplash()
{
	OOBase::SharedPtr<Splash> ptrSplash = OOBase::allocate_shared<Splash,OOBase::CrtAllocator>();
	if (!ptrSplash)
		return false;

	return ptrSplash->init();
}
