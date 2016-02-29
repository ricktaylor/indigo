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

#include "Common.h"
#include "../lib/VertexArrayObject.h"
#include "../lib/BufferObject.h"
#include "../lib/Shader.h"

#include "Resource.h"
#include "UI9Patch.h"

namespace
{
	class UI9PatchFactory
	{
	public:
		UI9PatchFactory();
		~UI9PatchFactory();

		GLsizei alloc_patch(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size);
		void free_patch(GLsizei p);
		void layout_patch(GLsizei patch, const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size);

		void draw(OOGL::State& state, const glm::mat4& mvp, const glm::vec4& colour, const GLsizeiptr* firsts, const GLsizei* counts, GLsizei drawcount);

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

	typedef OOGL::ContextSingleton<UI9PatchFactory> UI9PatchFactory_t;

	static const unsigned int vertices_per_patch = 16;
	static const unsigned int elements_per_patch = 24;
}

UI9PatchFactory::UI9PatchFactory() : m_allocated(0)
{

}

UI9PatchFactory::~UI9PatchFactory()
{

}

bool UI9PatchFactory::create_program()
{
#if defined(_WIN32)
	static const char* s_UI9Patch_vert = static_cast<const char*>(Indigo::static_resources().load("UI9Patch.vert"));
	static const GLint s_len_UI9Patch_vert = static_cast<GLint>(Indigo::static_resources().size("UI9Patch.vert"));

	static const char* s_UI9Patch_frag = static_cast<const char*>(Indigo::static_resources().load("UI9Patch.frag"));
	static const GLint s_len_UI9Patch_frag = static_cast<GLint>(Indigo::static_resources().size("UI9Patch.frag"));
#else
	#include "UI9Patch.vert.h"
	#include "UI9Patch.frag.h"

	#define s_len_UI9Patch_vert static_cast<GLint>(sizeof(s_UI9Patch_vert))
	#define s_len_UI9Patch_frag static_cast<GLint>(sizeof(s_UI9Patch_frag))		
#endif

	if (!m_ptrProgram)
	{
		OOBase::SharedPtr<OOGL::Shader> shaders[2];
		shaders[0] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_VERTEX_SHADER);

		if (!shaders[0]->compile(s_UI9Patch_vert,s_len_UI9Patch_vert))
			LOG_ERROR_RETURN(("Failed to compile vertex shader: %s",shaders[0]->info_log().c_str()),false);
		
		shaders[1] = OOBase::allocate_shared<OOGL::Shader,OOBase::ThreadLocalAllocator>(GL_FRAGMENT_SHADER);
		if (!shaders[1]->compile(s_UI9Patch_frag,s_len_UI9Patch_frag))
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

GLsizei UI9PatchFactory::alloc_patch(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size)
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

		m_ptrVAO->unbind();
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

void UI9PatchFactory::layout_patch(GLsizei patch, const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size)
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

void UI9PatchFactory::free_patch(GLsizei p)
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

void UI9PatchFactory::draw(OOGL::State& glState, const glm::mat4& mvp, const glm::vec4& colour, const GLsizeiptr* firsts, const GLsizei* counts, GLsizei drawcount)
{
	if (m_ptrProgram)
	{
		glState.use(m_ptrProgram);

		m_ptrProgram->uniform("in_Colour",colour);
		m_ptrProgram->uniform("MVP",mvp);

		m_ptrVAO->multi_draw_elements(GL_TRIANGLE_STRIP,counts,GL_UNSIGNED_INT,firsts,drawcount);
	}
}

Indigo::Render::UI9Patch::UI9Patch(const glm::u16vec2& position, const glm::u16vec2& size, bool visible, const glm::vec4& colour, const OOBase::SharedPtr<Indigo::UI9Patch::Info>& info) :
		UIDrawable(position,visible),
		m_colour(colour),
		m_patch(-1),
		m_info(info)
{
	layout(size);
}

Indigo::Render::UI9Patch::~UI9Patch()
{
	if (m_patch != GLsizei(-1))
		UI9PatchFactory_t::instance().free_patch(m_patch);
}

bool Indigo::Render::UI9Patch::valid() const
{
	return m_patch != GLsizei(-1);
}

void Indigo::Render::UI9Patch::layout(const glm::u16vec2& size)
{
	if (size.x && size.y && m_info->m_tex_size.x && m_info->m_tex_size.y)
	{
		if (m_patch == GLsizei(-1))
			m_patch = UI9PatchFactory_t::instance().alloc_patch(size,m_info->m_borders,m_info->m_tex_size);
		else
			UI9PatchFactory_t::instance().layout_patch(m_patch,size,m_info->m_borders,m_info->m_tex_size);
	}
	else if (m_patch != GLsizei(-1))
	{
		UI9PatchFactory_t::instance().free_patch(m_patch);
		m_patch = GLsizei(-1);
	}

	if (m_patch != GLsizei(-1))
	{
		// Set up draw call
		GLsizei patch = m_patch * elements_per_patch;

		m_firsts[0] = 0;
		m_counts[0] = 0;

		if (m_info->m_borders.y)
		{
			if (!m_info->m_borders.x)
			{
				m_firsts[0] = 2;
				m_counts[0] = 6;
			}
			else
				m_counts[0] = 8;

			if (!m_info->m_borders.z)
				m_counts[0] -= 2;

			m_firsts[0] = (patch + m_firsts[0]) * sizeof(GLuint);
		}

		m_firsts[1] = 8;
		m_counts[1] = 8;
		if (!m_info->m_borders.x)
		{
			m_firsts[1] = 10;
			m_counts[1] = 6;
		}
		if (!m_info->m_borders.z)
			m_counts[1] -= 2;

		m_firsts[1] = (patch + m_firsts[1]) * sizeof(GLuint);

		m_firsts[2] = 16;
		m_counts[2] = 0;
		if (m_info->m_borders.w)
		{
			if (!m_info->m_borders.x)
			{
				m_firsts[2] = 18;
				m_counts[2] = 6;
			}
			else
				m_counts[2] = 8;

			if (!m_info->m_borders.z)
				m_counts[2] -= 2;

			m_firsts[2] = (patch + m_firsts[2]) * sizeof(GLuint);
		}
	}
}

void Indigo::Render::UI9Patch::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_patch != GLsizei(-1) && m_info->m_texture)
	{
		glState.bind(0,m_info->m_texture);

		if (m_counts[0])
		{
			if (m_counts[2])
				UI9PatchFactory_t::instance().draw(glState,mvp,m_colour,m_firsts,m_counts,3);
			else
				UI9PatchFactory_t::instance().draw(glState,mvp,m_colour,m_firsts,m_counts,2);
		}
		else
		{
			if (m_counts[2])
				UI9PatchFactory_t::instance().draw(glState,mvp,m_colour,&m_firsts[1],&m_counts[1],2);
			else
				UI9PatchFactory_t::instance().draw(glState,mvp,m_colour,&m_firsts[1],&m_counts[1],1);
		}
	}
}

Indigo::UI9Patch::UI9Patch() : Image(),
		m_margins(0)
{
}

Indigo::UI9Patch::~UI9Patch()
{
	unload();
}

bool Indigo::UI9Patch::load(const unsigned char* buffer, int len, int components)
{
	if (!Image::load(buffer,len,components))
		return false;

	bool ret = false;
	m_info = OOBase::allocate_shared<Indigo::UI9Patch::Info>();
	if (!m_info)
		LOG_ERROR(("Failed to allocate 9-patch info block: %s",OOBase::system_error_text()));
	else
	{
		if (m_width <= 2 || m_height <= 2)
			LOG_ERROR(("Image too small for a 9-patch!"));

		ret = get_bounds();
	}

	if (!ret)
		unload();

	return ret;
}

void Indigo::UI9Patch::do_unload()
{
	m_info.reset();
}

void Indigo::UI9Patch::unload()
{
	Image::unload();

	if (m_info)
	{
		if (m_info.unique())
			render_pipe()->call(OOBase::make_delegate(this,&UI9Patch::do_unload));
		else
			m_info.reset();
	}
}

bool Indigo::UI9Patch::pixel_cmp(int x, int y, bool black)
{
	static const char black_rgba[4] = { 0, 0, 0, char(0xFF) };
	int comp_bits = 0;

	const char* p = static_cast<char*>(m_pixels) + ((y * m_width) + x) * m_components;

	for (int c=0;c<m_components;++c)
	{
		if (p[c] == black_rgba[c])
			comp_bits |= (1 << c);
	}

	return (black == (comp_bits == ((1 << m_components)-1)));
}

bool Indigo::UI9Patch::scan_line(int line, glm::u16vec2& span)
{
	for (span.x = 0;span.x < m_width && pixel_cmp(span.x,line,false);++span.x)
		;

	if (span.x == 0)
		return false;

	for (span.y = span.x;span.y < m_width && pixel_cmp(span.y,line,true);++span.y)
		;

	if (span.x == span.y || span.y == m_width)
		return false;

	int eol = span.y;
	for (;eol < m_width && pixel_cmp(eol,line,false);++eol)
		;

	return (eol == m_width);
}

bool Indigo::UI9Patch::scan_column(int column, glm::u16vec2& span)
{
	for (span.x = 0;span.x < m_height && pixel_cmp(column,span.x,false);++span.x)
		;

	if (span.x == 0)
		return false;

	for (span.y = span.x;span.y < m_height && pixel_cmp(column,span.y,true);++span.y)
		;

	if (span.x == span.y || span.y == m_height)
		return false;

	int eoc = span.y;
	for (;eoc < m_height && pixel_cmp(column,eoc,false);++eoc)
		;

	return (eoc == m_height);
}

bool Indigo::UI9Patch::get_bounds()
{
	glm::u16vec2 span;
	if (!scan_line(0,span))
		LOG_ERROR_RETURN(("Bad top row in 9-patch!"),false);

	m_info->m_borders.x = span.x;
	m_info->m_borders.z = m_width - span.y;

	if (!scan_column(0,span))
		LOG_ERROR_RETURN(("Bad left column in 9-patch!"),false);

	m_info->m_borders.y = m_height - span.y;
	m_info->m_borders.w = span.x;

	if (!scan_line(m_height-1,span))
		LOG_ERROR_RETURN(("Bad bottom row in 9-patch!"),false);

	m_margins.x = span.x;
	m_margins.z = m_width  - span.y;

	if (!scan_column(m_width-1,span))
		LOG_ERROR_RETURN(("Bad right column in 9-patch!"),false);

	m_margins.y = m_height - span.y;
	m_margins.w = span.x;

	// Now swap out the pixels for a sub-image...
	char* new_pixels = static_cast<char*>(OOBase::ThreadLocalAllocator::allocate((m_width-2)*(m_height-2)*m_components));
	if (!new_pixels)
		LOG_ERROR_RETURN(("Failed to allocate 9-patch pixel data!"),false);

	char* dest = new_pixels;
	const char* src = static_cast<const char*>(m_pixels) + (m_width+1)*m_components;
	for (int y=1;y<m_height-1;++y)
	{
		memcpy(dest,src,(m_width-2)*m_components);

		src += m_width*m_components;
		dest += (m_width-2)*m_components;
	}

	OOBase::ThreadLocalAllocator::free(m_pixels);
	m_pixels = new_pixels;
	m_height -= 2;
	m_width -= 2;

	m_info->m_tex_size = size();

	return true;
}

OOBase::SharedPtr<Indigo::Render::UI9Patch> Indigo::UI9Patch::make_drawable(const glm::u16vec2& position, const glm::u16vec2& size, bool visible, const glm::vec4& colour) const
{
	if (!m_info->m_texture)
	{
		m_info->m_texture = make_texture(GL_RGBA8);
		if (!m_info->m_texture)
			return OOBase::SharedPtr<Indigo::Render::UI9Patch>();

		m_info->m_texture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		m_info->m_texture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		m_info->m_texture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		m_info->m_texture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}

	return OOBase::allocate_shared<Render::UI9Patch,OOBase::ThreadLocalAllocator>(position,size,visible,colour,m_info);
}
