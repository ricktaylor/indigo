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

#include "Font.h"
#include "../lib/Shader.h"

static const char s_font_vertex_shader[] =
		"";

static const char s_font_frag_shader[] =
		"";

namespace OOGL
{
	class FontManager
	{
	public:
		OOBase::SharedPtr<Program> m_program;
		GLint m_mvp;
		GLint m_colour;
	};
}

namespace
{
	bool init_manager(OOBase::SharedPtr<OOGL::FontManager>& manager)
	{
		manager = OOBase::allocate_shared<OOGL::FontManager,OOBase::ThreadLocalAllocator>();
		if (!manager)
			LOG_ERROR_RETURN(("Failed to allocate font manager"),false);

		manager->m_program = OOBase::allocate_shared<OOGL::Program,OOBase::ThreadLocalAllocator>();
		if (!manager->m_program)
			LOG_ERROR_RETURN(("Failed to allocate font program"),false);

		OOBase::SharedPtr<OOGL::Shader> shaders[2];

		shaders[0] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_VERTEX_SHADER);
		if (!shaders[0])
			LOG_ERROR_RETURN(("Failed to allocate font shader"),false);
		shaders[1] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_FRAGMENT_SHADER);
		if (!shaders[1])
			LOG_ERROR_RETURN(("Failed to allocate font shader"),false);

		shaders[0]->compile(s_font_vertex_shader);
		shaders[1]->compile(s_font_frag_shader);

		manager->m_program->link(shaders,2);

		//manager->m_mvp = static_cast<GLuint>(manager->m_program->uniform_location("uMVP"));
		//manager->m_colour = static_cast<GLuint>(manager->m_program->uniform_location("uColour"));

		return true;
	}
}


void OOGL::Font::draw(State& state, const glm::mat4& mvp, const glm::vec4& colour)
{
	//if (!state.m_font_manager && !init_manager(state.m_font_manager))
	//	return;

	// Use the state's font program
	//state.use(state.m_font_manager->m_program);

	// Set the uniforms for MVP and colour
	//state.m_font_manager->m_program->uniform(state.m_font_manager->m_mvp,mvp);
	//state.m_font_manager->m_program->uniform(state.m_font_manager->m_colour,colour);

	// Bind the Vertex Array Object
}

OOGL::Text::Text(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len) :
		m_font(font)
{

}

void OOGL::Text::draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, size_t start, size_t end)
{
	m_font->draw(state,mvp,colour);

}
