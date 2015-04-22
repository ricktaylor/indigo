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
#include "../lib/Font.h"

#include <glm/gtc/matrix_transform.hpp>

#if defined(_WIN32)

#include "../resources/resource.h"

namespace
{
	void load_resource(const unsigned char*& p, size_t& len, LPCTSTR name, LPCTSTR type = MAKEINTRESOURCE(10))
	{
		if (!p)
		{
			HRSRC res = FindResource(NULL,name,type);
			if (res)
			{
				p = static_cast<const unsigned char*>(LockResource(LoadResource(NULL,res)));
				len = SizeofResource(NULL,res);
			}
		}
	}

	const unsigned char* s_Titillium_Regular = NULL;
	size_t s_Titillium_Regular_length = 0;
	const unsigned char* get_Titillium_Regular()
	{
		load_resource(s_Titillium_Regular,s_Titillium_Regular_length,MAKEINTRESOURCE(IDR_TITILLIUM));
		return s_Titillium_Regular;
	}
	size_t get_Titillium_Regular_length()
	{
		load_resource(s_Titillium_Regular,s_Titillium_Regular_length,MAKEINTRESOURCE(IDR_TITILLIUM));
		return s_Titillium_Regular_length;
	}

	const unsigned char* s_Titillium_Regular_0 = NULL;
	size_t s_Titillium_Regular_0_length = 0;
	const unsigned char* get_Titillium_Regular_0()
	{
		load_resource(s_Titillium_Regular_0,s_Titillium_Regular_0_length,MAKEINTRESOURCE(IDR_TITILLIUM_0));
		return s_Titillium_Regular_0;
	}
	size_t get_Titillium_Regular_0_length()
	{
		load_resource(s_Titillium_Regular_0,s_Titillium_Regular_0_length,MAKEINTRESOURCE(IDR_TITILLIUM_0));
		return s_Titillium_Regular_0_length;
	}
}

#define Titillium_Regular (get_Titillium_Regular())
#define Titillium_Regular_length (get_Titillium_Regular_length())
#define Titillium_Regular_0 (get_Titillium_Regular_0())
#define Titillium_Regular_0_length (get_Titillium_Regular_0_length())

#else
extern const unsigned char* Titillium_Regular;
extern size_t Titillium_Regular_length;
extern const unsigned char* Titillium_Regular_0;
extern size_t Titillium_Regular_0_length;
#endif

namespace
{
	class Triangle
	{
	public:
		void setup();
		void draw(OOGL::State& glState, const glm::mat4& VP);

	private:
		OOBase::SharedPtr<OOGL::VertexArrayObject> ptrVAO;
		OOBase::SharedPtr<OOGL::Program> ptrProgram;
	};
}

void Triangle::setup()
{
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
	OOBase::SharedString<OOBase::ThreadLocalAllocator> s = shaders[0]->info_log();
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
	OOBase::SharedPtr<OOGL::BufferObject> ptrVBO = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(points),points);
	GLint a = ptrProgram->attribute_location("in_Position");
	ptrVAO->attribute(a,ptrVBO,3,GL_FLOAT,false);
	ptrVAO->enable_attribute(a);

	float colours[] = {
			1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 0.f, 1.f
	};
	ptrVBO = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(colours),colours);
	a = ptrProgram->attribute_location("in_Colour");
	ptrVAO->attribute(a,ptrVBO,3,GL_FLOAT,false);
	ptrVAO->enable_attribute(a);
}

void Triangle::draw(OOGL::State& glState, const glm::mat4& VP)
{
	glm::mat4 model = glm::rotate(glm::mat4(1.0f),-glm::radians((float)glfwGetTime() * 150.f),glm::vec3(0,0,1));
	ptrProgram->uniform("MVP",VP * model);

	glState.use(ptrProgram);
	ptrVAO->draw(GL_TRIANGLES,0,3);
}

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

		void on_draw(const OOGL::Window& win, OOGL::State& glState);
		void on_close(const OOGL::Window& win);
		void on_move(const OOGL::Window& win, const glm::ivec2& pos);
		void on_size(const OOGL::Window& win, const glm::ivec2& sz);

		OOBase::SharedPtr<Splash> self;

		glm::vec2 m_dpmm;
		Triangle m_tri;

		OOBase::SharedPtr<OOGL::Text> m_text;
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

	pThis->m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(320,200,"Test",OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated);
	if (!pThis->m_wnd || !pThis->m_wnd->is_valid())
		return false;

	if (!pThis->m_wnd->signal_close.connect(OOBase::WeakPtr<Splash>(pThis->shared_from_this()),&Splash::on_close) ||
			!pThis->m_wnd->signal_moved.connect(OOBase::WeakPtr<Splash>(pThis->shared_from_this()),&Splash::on_move) ||
			!pThis->m_wnd->signal_sized.connect(OOBase::WeakPtr<Splash>(pThis->shared_from_this()),&Splash::on_size) ||
			!pThis->m_wnd->signal_draw.connect(OOBase::WeakPtr<Splash>(pThis->shared_from_this()),&Splash::on_draw))
		LOG_ERROR_RETURN(("Failed to attach signal"),false);

	pThis->m_tri.setup();

	OOBase::SharedPtr<OOGL::Font> fnt = OOBase::allocate_shared<OOGL::Font,OOBase::ThreadLocalAllocator>();
	if (!fnt)
		LOG_ERROR_RETURN(("Failed to load font"),false);

	if (!fnt->load(Titillium_Regular,Titillium_Regular_length,Titillium_Regular_0,Titillium_Regular_0_length))
		return false;

	OOBase::SharedString<OOBase::ThreadLocalAllocator> s;
	s.assign("This is some text!");
	pThis->m_text = OOBase::allocate_shared<OOGL::Text,OOBase::ThreadLocalAllocator>(fnt,s);

	glClearColor(0.f,0.f,0.f,0.f);
	glEnable(GL_BLEND);

	if (!Indigo::monitor_window(pThis->m_wnd))
		LOG_ERROR_RETURN(("Failed to monitor window"),false);

	pThis->on_size(*pThis->m_wnd,pThis->m_wnd->size());
	pThis->m_wnd->visible(true);
	pThis->self = pThis->shared_from_this();

	return true;
}

void Splash::on_move(const OOGL::Window& win, const glm::ivec2& pos)
{
	glm::ivec2 sz = win.size();
	m_dpmm = win.dots_per_mm();
	m_ratio = (sz.x * m_dpmm.x) / (sz.y * m_dpmm.y);
	glViewport(0, 0, sz.x, sz.y);
}

void Splash::on_size(const OOGL::Window& win, const glm::ivec2& sz)
{
	m_dpmm = win.dots_per_mm();
	m_ratio = (sz.x * m_dpmm.x) / (sz.y * m_dpmm.y);
	glViewport(0, 0, sz.x, sz.y);
}

void Splash::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	glState.bind(GL_DRAW_FRAMEBUFFER,win.get_default_frame_buffer());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);
	
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),m_ratio,0.1f,100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(3,0,0),glm::vec3(0,0,0),glm::vec3(0,1,0));
	view = glm::rotate(view,-glm::radians((float)glfwGetTime() * 50.f),glm::vec3(0,1,0));

	m_tri.draw(glState,proj * view);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	proj = glm::ortho(-m_ratio, m_ratio, -1.f, 1.f, 1.f, -1.f);
	glm::mat4 model(1);
	model = glm::scale(model,glm::vec3(.5f,.5f,0.f));
	model = glm::translate(model,glm::vec3(-m_text->length()/2,-.5f,0.f));

	m_text->draw(glState,proj * model,glm::vec4(fmod(glfwGetTime() / 2,1),fmod(glfwGetTime() / 3,1),fmod(glfwGetTime() / 7,1),fmod(glfwGetTime(),1)));
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
