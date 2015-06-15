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

#include "NinePatch.h"

#include "../lib/VertexArrayObject.h"
#include "../lib/BufferObject.h"
#include "../lib/Shader.h"

#include <OOBase/TLSSingleton.h>

namespace Indigo
{
	OOGL::ResourceBundle& static_resources();
}

namespace
{
	class NinePatchFactory
	{
	public:
		NinePatchFactory();
		~NinePatchFactory();

		GLsizei alloc_patch(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size);
		void free_patch(GLsizei p);
		void layout_patch(GLsizei patch, const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size);

		void draw(OOGL::State& state, const glm::mat4& mvp, const GLsizeiptr* firsts, const GLsizei* counts, GLsizei drawcount);

	private:
		typedef OOBase::Table<GLsizei,GLsizei,OOBase::Less<GLsizei>,OOBase::ThreadLocalAllocator> free_list_t;
		free_list_t m_listFree;
		GLsizei m_allocated;

		struct vertex_data
		{
			float x;
			float y;
			GLushort u;
			GLushort v;
		};

		OOBase::SharedPtr<OOGL::Program> m_ptrProgram;
		OOBase::SharedPtr<OOGL::VertexArrayObject> m_ptrVAO;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrVertices;
		OOBase::SharedPtr<OOGL::BufferObject> m_ptrElements;

		bool create_program();
	};

	typedef OOGL::ContextSingleton<NinePatchFactory> NinePatchFactory_t;

	static const unsigned int vertices_per_patch = 16;
	static const unsigned int elements_per_patch = 24;
}

NinePatchFactory::NinePatchFactory() : m_allocated(0)
{

}

NinePatchFactory::~NinePatchFactory()
{

}

bool NinePatchFactory::create_program()
{
	if (!m_ptrProgram)
	{
		OOBase::SharedPtr<OOGL::Shader> shaders[2];
		shaders[0] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_VERTEX_SHADER);
		if (!shaders[0]->compile(Indigo::static_resources(),"NinePatch.vert"))
			LOG_ERROR_RETURN(("Failed to compile vertex shader: %s",shaders[0]->info_log().c_str()),false);
		
		shaders[1] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_FRAGMENT_SHADER);
		if (!shaders[1]->compile(Indigo::static_resources(),"NinePatch.frag"))
			LOG_ERROR_RETURN(("Failed to compile vertex shader: %s",shaders[1]->info_log().c_str()),false);
		
		OOBase::SharedPtr<OOGL::Program> program = OOBase::allocate_shared<OOGL::Program,OOBase::ThreadLocalAllocator>();
		if (!program)
			LOG_ERROR_RETURN(("Failed to allocate shader program: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		if (!program->link(shaders,2))
			LOG_ERROR_RETURN(("Failed to link shaders: %s",program->info_log().c_str()),false);

		m_ptrProgram = program;
	}
	return true;
}

GLsizei NinePatchFactory::alloc_patch(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size)
{
	GLsizei patch = GLsizei(-1);

	for (free_list_t::iterator i=m_listFree.begin(); i; ++i)
	{
		if (i->second == 1)
		{
			patch = i->first;
			m_listFree.erase(i);
			break;
		}
		else if (i->second > 1)
		{
			patch = i->first;
			i->first += 1;
			i->second -= 1;
			break;
		}
	}

	if (patch == GLsizei(-1))
	{
		GLsizei new_size = 8;
		while (new_size < m_allocated + 1)
			new_size *= 2;

		OOBase::SharedPtr<OOGL::BufferObject> ptrNewVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW,new_size * vertices_per_patch * sizeof(vertex_data));
		OOBase::SharedPtr<OOGL::BufferObject> ptrNewElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_DYNAMIC_DRAW,new_size * elements_per_patch * sizeof(GLuint));
		if (!ptrNewVertices || !ptrNewElements)
			LOG_ERROR_RETURN(("Failed to allocate VBO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),patch);

		if (m_ptrVertices)
			ptrNewVertices->copy(0,m_ptrVertices,0,m_allocated * vertices_per_patch * sizeof(vertex_data));

		if (m_ptrElements)
			ptrNewElements->copy(0,m_ptrElements,0,m_allocated * elements_per_patch * sizeof(GLuint));

		m_ptrVertices.swap(ptrNewVertices);
		m_ptrElements.swap(ptrNewElements);

		free_list_t::iterator last = m_listFree.back();
		if (last)
			last->second += new_size - m_allocated;
		else
			last = m_listFree.insert(m_allocated,new_size - m_allocated);
		m_allocated = new_size;

		if (!m_ptrVAO)
		{
			m_ptrVAO = OOBase::allocate_shared<OOGL::VertexArrayObject,OOBase::ThreadLocalAllocator>();
			if (!m_ptrVAO)
				LOG_ERROR_RETURN(("Failed to allocate VAO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),patch);
		}

		if (!m_ptrProgram && !create_program())
			return patch;

		patch = last->first;
		last->first += 1;
		last->second -= 1;

		GLint a = m_ptrProgram->attribute_location("in_Position");
		m_ptrVAO->attribute(a,m_ptrVertices,2,GL_FLOAT,false,sizeof(vertex_data),offsetof(vertex_data,x));
		m_ptrVAO->enable_attribute(a);

		a = m_ptrProgram->attribute_location("in_TexCoord");
		if (a != -1)
		{
			m_ptrVAO->attribute(a,m_ptrVertices,2,GL_UNSIGNED_SHORT,true,sizeof(vertex_data),offsetof(vertex_data,u));
			m_ptrVAO->enable_attribute(a);
		}

		m_ptrVAO->element_array(m_ptrElements);
		OOGL::State::get_current()->bind(OOGL::VertexArrayObject::none());
	}

	OOBase::SharedPtr<GLuint> ei = m_ptrElements->auto_map<GLuint>(GL_MAP_WRITE_BIT,patch * elements_per_patch * sizeof(GLuint),elements_per_patch * sizeof(GLuint));
	GLuint* e = ei.get();
	GLuint idx = patch * vertices_per_patch;
	for (size_t i=0;i<12;++i)
	{
		e[i*2] = static_cast<GLuint>(idx + i);
		e[i*2 + 1] = static_cast<GLuint>(idx + i + 4);
	}

	layout_patch(patch,size,borders,tex_size);

	return patch;
}

void NinePatchFactory::layout_patch(GLsizei patch, const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size)
{
	unsigned int ushort_max = 0xFFFF;

	OOBase::SharedPtr<vertex_data> attribs = m_ptrVertices->auto_map<vertex_data>(GL_MAP_WRITE_BIT,patch * vertices_per_patch * sizeof(vertex_data),vertices_per_patch * sizeof(vertex_data));
	vertex_data* a = attribs.get();

	a[0].x = 0;
	a[1].x = borders.x;
	a[3].x = size.x;
	a[2].x = a[3].x - borders.z;

	a[0].u = 0;
	a[1].u = static_cast<GLushort>(static_cast<float>(borders.x) / tex_size.x * ushort_max);
	a[2].u = static_cast<GLushort>(static_cast<float>(tex_size.x - borders.z) / tex_size.x * ushort_max);
	a[3].u = ushort_max;

	for (size_t i=0;i<4;++i)
	{
		a[i+12].x = a[i+8].x = a[i+4].x = a[i].x;

		a[i].y = size.y;
		a[i+4].y = a[i].y - borders.y;
		a[i+8].y = borders.w;
		a[i+12].y = 0;

		a[i+12].u = a[i+8].u = a[i+4].u = a[i].u;

		a[i].v = 0;
		a[i+4].v = static_cast<GLushort>(static_cast<float>(borders.w) / tex_size.y * ushort_max);
		a[i+8].v = static_cast<GLushort>(static_cast<float>(tex_size.y - borders.y) / tex_size.y * ushort_max);
		a[i+12].v = ushort_max;
	}
}

void NinePatchFactory::free_patch(GLsizei p)
{
	free_list_t::iterator i = m_listFree.insert(p,1);
	if (i)
	{
		while (i != m_listFree.begin())
		{
			free_list_t::iterator j = i+1;
			if (j && j->first == i->first + i->second)
			{
				// Merge i with j
				i->second += j->second;
				m_listFree.erase(j);
			}
			else
			{
				--i;
			}
		}
	}
}

void NinePatchFactory::draw(OOGL::State& state, const glm::mat4& mvp, const GLsizeiptr* firsts, const GLsizei* counts, GLsizei drawcount)
{
	if (m_ptrProgram)
	{
		state.use(m_ptrProgram);

		m_ptrProgram->uniform("MVP",mvp);

		m_ptrVAO->multi_draw_elements(GL_TRIANGLE_STRIP,counts,GL_UNSIGNED_INT,firsts,drawcount);
	}
}

Indigo::Render::NinePatch::NinePatch() : m_patch(-1)
{
	memset(m_counts,0,sizeof(m_counts));
}

Indigo::Render::NinePatch::NinePatch(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size) : m_patch(-1)
{
	layout(size,borders,tex_size);
}

Indigo::Render::NinePatch::~NinePatch()
{
	if (m_patch != GLsizei(-1))
		NinePatchFactory_t::instance().free_patch(m_patch);
}

bool Indigo::Render::NinePatch::valid() const
{
	return m_patch != GLsizei(-1);
}

void Indigo::Render::NinePatch::layout(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size)
{
	if (size.x && size.y && tex_size.x && tex_size.y)
	{
		if (m_patch == GLsizei(-1))
			m_patch = NinePatchFactory_t::instance().alloc_patch(size,borders,tex_size);
		else
			NinePatchFactory_t::instance().layout_patch(m_patch,size,borders,tex_size);
	}
	else if (m_patch != GLsizei(-1))
	{
		NinePatchFactory_t::instance().free_patch(m_patch);
		m_patch = GLsizei(-1);
	}

	if (m_patch != GLsizei(-1))
	{
		// Set up draw call
		GLsizei patch = m_patch * elements_per_patch;

		m_firsts[0] = 0;
		m_counts[0] = 0;

		if (borders.y)
		{
			if (!borders.x)
			{
				m_firsts[0] = 2;
				m_counts[0] = 6;
			}
			else
				m_counts[0] = 8;

			if (!borders.z)
				m_counts[0] -= 2;

			m_firsts[0] = (patch + m_firsts[0]) * sizeof(GLuint);
		}

		m_firsts[1] = 8;
		m_counts[1] = 8;
		if (!borders.x)
		{
			m_firsts[1] = 10;
			m_counts[1] = 6;
		}
		if (!borders.z)
			m_counts[1] -= 2;

		m_firsts[1] = (patch + m_firsts[1]) * sizeof(GLuint);

		m_firsts[2] = 16;
		m_counts[2] = 0;
		if (borders.w)
		{
			if (!borders.x)
			{
				m_firsts[2] = 18;
				m_counts[2] = 6;
			}
			else
				m_counts[2] = 8;

			if (!borders.z)
				m_counts[2] -= 2;

			m_firsts[2] = (patch + m_firsts[2]) * sizeof(GLuint);
		}
	}
}

void Indigo::Render::NinePatch::draw(OOGL::State& state, const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::mat4& mvp) const
{
	if (m_patch != GLsizei(-1))
	{
		state.bind(0,texture);

		if (m_counts[0])
		{
			if (m_counts[2])
				NinePatchFactory_t::instance().draw(state,mvp,m_firsts,m_counts,3);
			else
				NinePatchFactory_t::instance().draw(state,mvp,m_firsts,m_counts,2);
		}
		else
		{
			if (m_counts[2])
				NinePatchFactory_t::instance().draw(state,mvp,&m_firsts[1],&m_counts[1],2);
			else
				NinePatchFactory_t::instance().draw(state,mvp,&m_firsts[1],&m_counts[1],1);
		}
	}
}
