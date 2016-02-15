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

#include "Font.h"
#include "Render.h"
#include "../lib/BufferObject.h"
#include "../lib/VertexArrayObject.h"
#include "../lib/Shader.h"
#include "../lib/Resource.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Indigo
{
	OOGL::ResourceBundle& static_resources();
}

namespace
{
	class Triangle
	{
	public:
		void setup();
		void draw(OOGL::State& glState, const glm::mat4& V, const glm::mat4& P);

	private:
		OOBase::SharedPtr<OOGL::VertexArrayObject> m_ptrVAO;
		OOBase::SharedPtr<OOGL::Program> m_ptrProgram;

		struct vbo_vec3
		{
			float x;
			float y;
			float z;
		};
		struct vbo_data
		{
			vbo_vec3 vertex;
			vbo_vec3 normal;
			vbo_vec3 colour;
		};
	};
}

void Triangle::setup()
{
	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_VERTEX_SHADER);
	if (!shaders[0]->compile(
			"#version 120\n"
			"uniform mat4 MV;\n"
			"uniform mat4 P;\n"
			"uniform mat4 N;\n"
			"uniform vec3 light;\n"
			"attribute vec3 in_Position;\n"
			"attribute vec3 in_Normal;\n"
			"attribute vec3 in_Colour;\n"
			"varying vec3 pass_Colour;\n"
			"varying vec3 pass_Normal;\n"
			"varying vec3 pass_Pos;\n"
			"void main() {\n"
			"	pass_Colour = in_Colour;\n"
			"	gl_Position = P * MV * vec4(in_Position,1.0);\n"
			"	vec4 pos = MV * vec4(in_Position,1.0);\n"
			"	pass_Pos = vec3(pos) / pos.w;\n"
			"	pass_Normal = vec3(N * vec4(in_Normal,0.0));\n"
			"}\n"))
		LOG_ERROR(("Failed to compile vertex shader: %s",shaders[0]->info_log().c_str()));

	shaders[1] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_FRAGMENT_SHADER);
	if (!shaders[1]->compile(
			"#version 120\n"
			"varying vec3 pass_Colour;\n"
			"varying vec3 pass_Normal;\n"
			"varying vec3 pass_Pos;\n"
			"void main() {\n"
			"	vec3 normal = normalize(pass_Normal);\n"
			"	vec3 light_dir = normalize(vec3(2.0,2.0,2.0) - pass_Pos);\n"
			"	float lambert = max(dot(light_dir,pass_Normal),0.0);\n"
			"	float specular = 0.0;\n"
			"	if (lambert > 0.0) {\n"
			"		vec3 view_dir = normalize(-pass_Pos);\n"
			"		vec3 half_dir = normalize(light_dir + view_dir);\n"
			"		float specAngle = max(dot(half_dir,normal),0.0);\n"
			"		specular = pow(specAngle,16.0);\n"
			"	}\n"
			"   gl_FragColor = vec4(lambert*pass_Colour + vec3(1.0,1.0,1.0)*specular,1.0);\n"
			"}\n"))
		LOG_ERROR(("Failed to compile vertex shader: %s",shaders[1]->info_log().c_str()));
	
	m_ptrProgram = OOBase::allocate_shared<OOGL::Program,OOBase::ThreadLocalAllocator>();
	if (!m_ptrProgram->link(shaders,2))
		LOG_ERROR(("Failed to link shaders: %s",m_ptrProgram->info_log().c_str()));

	m_ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();

	vbo_data data[] =
	{
			// Front
			{ {-0.5f, 0.5f,1.0f}, {0.0f,0.0f,1.0f}, {1.0f,0.0f,0.0f} },
			{ {-0.5f,-0.5f,1.0f}, {0.0f,0.0f,1.0f}, {0.0f,1.0f,0.0f} },
			{ { 0.5f, 0.5f,1.0f}, {0.0f,0.0f,1.0f}, {0.0f,0.0f,1.0f} },
			{ { 0.5f,-0.5f,1.0f}, {0.0f,0.0f,1.0f}, {1.0f,1.0f,0.0f} },

			// Right
			{ { 0.5f, 0.5f,1.0f}, {1.0f,0.0f,0.0f}, {0.0f,0.0f,1.0f} },
			{ { 0.5f,-0.5f,1.0f}, {1.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f} },
			{ { 0.5f, 0.5f,0.0f}, {1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f} },
			{ { 0.5f,-0.5f,0.0f}, {1.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f} },

			// Top
			{ {-0.5f, 0.5f,0.0f}, {0.0f,1.0f,0.0f}, {1.0f,1.0f,0.0f} },
			{ {-0.5f, 0.5f,1.0f}, {0.0f,1.0f,0.0f}, {1.0f,0.0f,0.0f} },
			{ { 0.5f, 0.5f,0.0f}, {0.0f,1.0f,0.0f}, {0.0f,1.0f,0.0f} },
			{ { 0.5f, 0.5f,1.0f}, {0.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f} },

			// Back
			{ { 0.5f, 0.5f,0.0f}, {0.0f,0.0f,-1.0f}, {0.0f,1.0f,0.0f} },
			{ { 0.5f,-0.5f,0.0f}, {0.0f,0.0f,-1.0f}, {1.0f,0.0f,0.0f} },
			{ {-0.5f, 0.5f,0.0f}, {0.0f,0.0f,-1.0f}, {1.0f,1.0f,0.0f} },
			{ {-0.5f,-0.5f,0.0f}, {0.0f,0.0f,-1.0f}, {0.0f,0.0f,1.0f} },

			// Left
			{ {-0.5f, 0.5f,0.0f}, {-1.0f,0.0f,0.0f}, {1.0f,1.0f,0.0f} },
			{ {-0.5f,-0.5f,0.0f}, {-1.0f,0.0f,0.0f}, {0.0f,0.0f,1.0f} },
			{ {-0.5f, 0.5f,1.0f}, {-1.0f,0.0f,0.0f}, {1.0f,0.0f,0.0f} },
			{ {-0.5f,-0.5f,1.0f}, {-1.0f,0.0f,0.0f}, {0.0f,1.0f,0.0f} },

			// Bottom
			{ { 0.5f,-0.5f,0.0f}, {0.0f,-1.0f,0.0f}, {1.0f,0.0f,0.0f} },
			{ { 0.5f,-0.5f,1.0f}, {0.0f,-1.0f,0.0f}, {1.0f,1.0f,0.0f} },
			{ {-0.5f,-0.5f,0.0f}, {0.0f,-1.0f,0.0f}, {0.0f,0.0f,1.0f} },
			{ {-0.5f,-0.5f,1.0f}, {0.0f,-1.0f,0.0f}, {0.0f,1.0f,0.0f} },
	};

	OOBase::SharedPtr<OOGL::BufferObject> ptrVBO = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(data),data);
	GLint a = m_ptrProgram->attribute_location("in_Position");
	m_ptrVAO->attribute(a,ptrVBO,3,GL_FLOAT,false,sizeof(vbo_data),offsetof(vbo_data,vertex));
	m_ptrVAO->enable_attribute(a);

	a = m_ptrProgram->attribute_location("in_Normal");
	if (a != -1)
	{
		m_ptrVAO->attribute(a,ptrVBO,3,GL_FLOAT,false,sizeof(vbo_data),offsetof(vbo_data,normal));
		m_ptrVAO->enable_attribute(a);
	}

	a = m_ptrProgram->attribute_location("in_Colour");
	if (a != -1)
	{
		m_ptrVAO->attribute(a,ptrVBO,3,GL_FLOAT,false,sizeof(vbo_data),offsetof(vbo_data,colour));
		m_ptrVAO->enable_attribute(a);
	}

	GLuint indexes[] =
	{
		 0, 1, 2, 2, 1, 3,
		 4, 5, 6, 6, 5, 7,
		 8, 9,10,10, 9,11,
		12,13,14,14,13,15,
		16,17,18,18,17,19,
		20,21,22,22,21,23,
	};

	ptrVBO = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,sizeof(indexes),indexes);
	m_ptrVAO->element_array(ptrVBO);

}

void Triangle::draw(OOGL::State& glState, const glm::mat4& V, const glm::mat4& P)
{
	glm::mat4 model = glm::rotate(glm::mat4(1.0f),-glm::radians((float)glfwGetTime() * 150.f),glm::vec3(0,1,0));
	model = glm::rotate(model,glm::radians((float)glfwGetTime() * 50.f),glm::vec3(1,0,0));
	m_ptrProgram->uniform("MV",V * model);
	m_ptrProgram->uniform("P",P);
	m_ptrProgram->uniform("N",glm::transpose(glm::inverse(model)));

	glState.use(m_ptrProgram);
	m_ptrVAO->draw_elements(GL_TRIANGLES,36,GL_UNSIGNED_INT);
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
		void on_move(const OOGL::Window& win, const glm::u16vec2& pos);
		void on_size(const OOGL::Window& win, const glm::u16vec2& sz);
		void on_character(const OOGL::Window& win, unsigned int codepoint, int mods);
		void on_keystroke(const OOGL::Window& win, const OOGL::Window::key_stroke_t& keystroke);

		OOBase::SharedPtr<Splash> self;

		glm::vec2 m_dpmm;
		Triangle m_tri;

		double m_start;
		OOBase::SharedString<OOBase::ThreadLocalAllocator> m_str;
		OOBase::SharedPtr<Indigo::Render::Text> m_text;
		OOBase::SharedPtr<Indigo::Render::Text> m_fps;
	};
}

Splash::Splash() : m_ratio(0), m_start(0.0)
{
}

bool Splash::init()
{
	return Indigo::render_call(&create,this);
}

bool Splash::create(void* p)
{
	Splash* pThis = static_cast<Splash*>(p);

	unsigned int style = OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated;
	if (Indigo::is_debug())
		style |= OOGL::Window::eWSdebug_context;

	pThis->m_wnd = OOBase::allocate_shared<OOGL::Window,OOBase::ThreadLocalAllocator>(320,200,"Test",style);
	if (!pThis->m_wnd || !pThis->m_wnd->valid())
		return false;

	pThis->m_wnd->make_current();

	if (Indigo::is_debug())
		OOGL::StateFns::get_current()->enable_logging();

	pThis->m_wnd->on_close(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(pThis,&Splash::on_close));
	pThis->m_wnd->on_moved(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(pThis,&Splash::on_move));
	pThis->m_wnd->on_sized(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(pThis,&Splash::on_size));
	pThis->m_wnd->on_draw(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(pThis,&Splash::on_draw));
	pThis->m_wnd->on_character(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(pThis,&Splash::on_character));
	pThis->m_wnd->on_keystroke(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(pThis,&Splash::on_keystroke));

	pThis->m_tri.setup();

	OOBase::SharedPtr<Indigo::Render::Font> fnt = OOBase::allocate_shared<Indigo::Render::Font,OOBase::ThreadLocalAllocator>();
	if (!fnt)
		LOG_ERROR_RETURN(("Failed to load font"),false);

	if (!fnt->load(Indigo::static_resources(),"Titillium-Regular.fnt"))
		return false;

	pThis->m_text = OOBase::allocate_shared<Indigo::Render::Text,OOBase::ThreadLocalAllocator>(fnt,"Now try typing!");
	pThis->m_fps = OOBase::allocate_shared<Indigo::Render::Text,OOBase::ThreadLocalAllocator>(fnt);

	glClearColor(0.f,0.f,0.f,0.f);
	glEnable(GL_BLEND);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	if (!Indigo::monitor_window(pThis->m_wnd))
		LOG_ERROR_RETURN(("Failed to monitor window"),false);

	pThis->self = pThis->shared_from_this();
	pThis->on_size(*pThis->m_wnd,pThis->m_wnd->size());
	pThis->m_wnd->visible(true);

	return true;
}

void Splash::on_move(const OOGL::Window& win, const glm::u16vec2& pos)
{
	glm::u16vec2 sz = win.size();
	m_dpmm = win.dots_per_mm();
	m_ratio = (sz.x * m_dpmm.x) / (sz.y * m_dpmm.y);
	glViewport(0, 0, sz.x, sz.y);
}

void Splash::on_size(const OOGL::Window& win, const glm::u16vec2& sz)
{
	m_dpmm = win.dots_per_mm();
	m_ratio = (sz.x * m_dpmm.x) / (sz.y * m_dpmm.y);
	glViewport(0, 0, sz.x, sz.y);
}

void Splash::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	double now = glfwGetTime();
	glState.bind(GL_DRAW_FRAMEBUFFER,win.default_frame_buffer());

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);
	
	glm::mat4 proj = glm::perspective(glm::radians(45.0f),m_ratio,0.1f,100.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(2,3,4),glm::vec3(0,0,0),glm::vec3(0,1,0));
	//view = glm::rotate(view,-glm::radians((float)now * 50.f),glm::vec3(1,0,0));

	m_tri.draw(glState,view,proj);

	if (m_start != 0.0)
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		OOBase::ScopedString fps;
		fps.printf("%.4f ms = %.4f fps",(now - m_start) * 1000.0,1.0/(now - m_start));
		m_fps->text(fps.c_str());

		glm::vec2 sz = win.size();
		proj = glm::ortho(0.f, (float)sz.x, 0.f, (float)sz.y, 1.f, -1.f);
		glm::mat4 model = glm::scale(glm::mat4(1),glm::vec3(32.f,32.f,0.f));

		m_fps->draw(glState,proj * model,glm::vec4(1.f));
	}

	m_start = now;

	if (m_text->length())
	{
		proj = glm::ortho(-m_ratio, m_ratio, -1.f, 1.f, 1.f, -1.f);
		glm::mat4 model = glm::scale(glm::mat4(1),glm::vec3(.25f,.25f,0.f));
		model = glm::translate(model,glm::vec3(-m_text->length()/2,-.5f,0.f));

		m_text->draw(glState,proj * model,glm::vec4(fmod(now / 2,1),fmod(now / 3,1),fmod(now / 7,1),fmod(now,1)));
	}
}

void Splash::on_character(const OOGL::Window& win, unsigned int codepoint, int mods)
{
	m_str.append(static_cast<char>(codepoint));

	win.make_current();

	m_text->text(m_str.c_str());
}

void Splash::on_keystroke(const OOGL::Window& win, const OOGL::Window::key_stroke_t& keystroke)
{
	if (keystroke.key == GLFW_KEY_BACKSPACE && keystroke.action == GLFW_PRESS)
	{
		if (!m_str.empty())
		{
			win.make_current();

			m_str.assign(m_str.c_str(),m_str.length()-1);
			m_text->text(m_str.c_str());
		}
	}
}

void Splash::on_close(const OOGL::Window& win)
{
	self.reset();
}

bool showSplash()
{
	OOBase::SharedPtr<Splash> ptrSplash = OOBase::allocate_shared<Splash>();
	if (!ptrSplash)
		return false;

	return ptrSplash->init();
}
