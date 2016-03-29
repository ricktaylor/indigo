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

#include "../core/Common.h"
#include "../core/ShaderPool.h"
#include "../core/Render.h"

#include "UINinePatch.h"
#include "UIImage.h"

#include "../oogl/BufferObject.h"
#include "../oogl/Shader.h"
#include "../oogl/VertexArrayObject.h"

namespace
{
	class NinePatchFactory
	{
	public:
		NinePatchFactory();
		~NinePatchFactory();

		GLsizei alloc_patch(const glm::uvec2& size, const glm::uvec4& borders, const glm::uvec2& tex_size);
		void free_patch(GLsizei p);
		void layout_patch(GLsizei patch, const glm::uvec2& size, const glm::uvec4& borders, const glm::uvec2& tex_size);

		void draw(OOGL::State& state, const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::mat4& mvp, const glm::vec4& colour, const GLsizeiptr* firsts, const GLsizei* counts, GLsizei drawcount);

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

	const unsigned int vertices_per_patch = 16;
	const unsigned int elements_per_patch = 24;
}

NinePatchFactory::NinePatchFactory() : m_allocated(0)
{

}

NinePatchFactory::~NinePatchFactory()
{

}

bool NinePatchFactory::create_program()
{
	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = Indigo::ShaderPool::add_shader("2d_colour.vert",GL_VERTEX_SHADER,Indigo::static_resources());
	shaders[1] = Indigo::ShaderPool::add_shader("colour_blend.frag",GL_FRAGMENT_SHADER,Indigo::static_resources());
	if (shaders[0] && shaders[1])
		m_ptrProgram = Indigo::ShaderPool::add_program("2d_image",shaders,2);

	return m_ptrProgram;
}

GLsizei NinePatchFactory::alloc_patch(const glm::uvec2& size, const glm::uvec4& borders, const glm::uvec2& tex_size)
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

		OOBase::SharedPtr<OOGL::BufferObject> ptrNewVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_STATIC_DRAW,new_size * vertices_per_patch * sizeof(vertex_data));
		OOBase::SharedPtr<OOGL::BufferObject> ptrNewElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_STATIC_DRAW,new_size * elements_per_patch * sizeof(GLuint));
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

void NinePatchFactory::layout_patch(GLsizei patch, const glm::uvec2& size, const glm::uvec4& borders, const glm::uvec2& tex_size)
{
	static const unsigned int ushort_max = 0xFFFF;

	OOBase::SharedPtr<vertex_data> attribs = m_ptrVertices->auto_map<vertex_data>(GL_MAP_WRITE_BIT,patch * vertices_per_patch * sizeof(vertex_data),vertices_per_patch * sizeof(vertex_data));
	vertex_data* a = attribs.get();

	a[0].x = 0.f;
	a[1].x = static_cast<float>(borders.x);
	a[3].x = static_cast<float>(size.x);
	a[2].x = a[3].x - borders.z;

	a[0].u = 0;
	a[1].u = static_cast<GLushort>(static_cast<float>(borders.x) / tex_size.x * ushort_max);
	a[2].u = static_cast<GLushort>(static_cast<float>(tex_size.x - borders.z) / tex_size.x * ushort_max);
	a[3].u = ushort_max;

	for (size_t i=0;i<4;++i)
	{
		a[i+12].x = a[i+8].x = a[i+4].x = a[i].x;

		a[i].y = static_cast<float>(size.y);
		a[i+4].y = a[i].y - borders.y;
		a[i+8].y = static_cast<float>(borders.w);
		a[i+12].y = 0.f;

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

void NinePatchFactory::draw(OOGL::State& glState, const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::mat4& mvp, const glm::vec4& colour, const GLsizeiptr* firsts, const GLsizei* counts, GLsizei drawcount)
{
	if (m_ptrProgram)
	{
		glState.use(m_ptrProgram);
		glState.bind(0,texture);

		m_ptrProgram->uniform("in_Colour",colour);
		m_ptrProgram->uniform("MVP",mvp);

		m_ptrVAO->multi_draw_elements(GL_TRIANGLE_STRIP,counts,GL_UNSIGNED_INT,firsts,drawcount);
	}
}

Indigo::Render::UINinePatch::UINinePatch(const glm::ivec2& position, const glm::uvec2& size, const glm::vec4& colour, const OOBase::SharedPtr<OOGL::Texture>& texture, const OOBase::SharedPtr<Indigo::NinePatch::Info>& info) :
		UIDrawable(position),
		m_texture(texture),
		m_colour(colour),
		m_patch(-1),
		m_info(info)
{
	this->size(size);
}

Indigo::Render::UINinePatch::~UINinePatch()
{
	if (m_patch != GLsizei(-1))
		NinePatchFactory_t::instance().free_patch(m_patch);
}

bool Indigo::Render::UINinePatch::valid() const
{
	return UIDrawable::valid() && m_texture->valid() && m_patch != GLsizei(-1);
}

void Indigo::Render::UINinePatch::size(glm::uvec2 size)
{
	if (size.x && size.y && m_info->m_tex_size.x && m_info->m_tex_size.y)
	{
		if (m_patch == GLsizei(-1))
			m_patch = NinePatchFactory_t::instance().alloc_patch(size,m_info->m_borders,m_info->m_tex_size);
		else
			NinePatchFactory_t::instance().layout_patch(m_patch,size,m_info->m_borders,m_info->m_tex_size);
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

void Indigo::Render::UINinePatch::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_patch != GLsizei(-1) && m_texture && m_colour.a > 0.f)
	{
		if (m_counts[0])
		{
			if (m_counts[2])
				NinePatchFactory_t::instance().draw(glState,m_texture,mvp,m_colour,m_firsts,m_counts,3);
			else
				NinePatchFactory_t::instance().draw(glState,m_texture,mvp,m_colour,m_firsts,m_counts,2);
		}
		else
		{
			if (m_counts[2])
				NinePatchFactory_t::instance().draw(glState,m_texture,mvp,m_colour,&m_firsts[1],&m_counts[1],2);
			else
				NinePatchFactory_t::instance().draw(glState,m_texture,mvp,m_colour,&m_firsts[1],&m_counts[1],1);
		}
	}
}

Indigo::NinePatch::NinePatch() : Image(),
		m_margins(0)
{
}

Indigo::NinePatch::~NinePatch()
{
	unload();
}

bool Indigo::NinePatch::load(const unsigned char* buffer, size_t len, int components)
{
	if (!Image::load(buffer,len,components))
		return false;

	bool ret = false;
	m_info = OOBase::allocate_shared<Indigo::NinePatch::Info>();
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

bool Indigo::NinePatch::pixel_cmp(int x, int y, bool black)
{
	static const char black_rgba[4] = { 0, 0, 0, char(0xFF) };
	int comp_bits = 0;

	const char* p = static_cast<char*>(m_pixels) + ((y * m_width) + x) * m_components;

	for (unsigned int c=0;c<m_valid_components;++c)
	{
		if (p[c] == black_rgba[c])
			comp_bits |= (1 << c);
	}

	return (black == (comp_bits == ((1 << m_valid_components)-1)));
}

bool Indigo::NinePatch::scan_line(int line, glm::uvec2& span)
{
	for (span.x = 0;span.x < m_width && pixel_cmp(span.x,line,false);++span.x)
		;

	if (span.x == 0)
		return false;

	for (span.y = span.x;span.y < m_width && pixel_cmp(span.y,line,true);++span.y)
		;

	if (span.x == span.y || span.y == m_width)
		return false;

	unsigned int eol = span.y;
	for (;eol < m_width && pixel_cmp(eol,line,false);++eol)
		;

	return (eol == m_width);
}

bool Indigo::NinePatch::scan_column(int column, glm::uvec2& span)
{
	for (span.x = 0;span.x < m_height && pixel_cmp(column,span.x,false);++span.x)
		;

	if (span.x == 0)
		return false;

	for (span.y = span.x;span.y < m_height && pixel_cmp(column,span.y,true);++span.y)
		;

	if (span.x == span.y || span.y == m_height)
		return false;

	unsigned int eoc = span.y;
	for (;eoc < m_height && pixel_cmp(column,eoc,false);++eoc)
		;

	return (eoc == m_height);
}

bool Indigo::NinePatch::get_bounds()
{
	bool has_border = false;
	bool has_margins = false;

	glm::uvec2 span;
	if (scan_line(0,span))
	{
		m_info->m_borders.x = span.x;
		m_info->m_borders.z = m_width - span.y;

		if (scan_column(0,span))
		{
			has_border = true;

			m_info->m_borders.y = m_height - span.y;
			m_info->m_borders.w = span.x;

			if (scan_line(m_height-1,span))
			{
				m_margins.x = span.x;
				m_margins.z = m_width  - span.y;

				if (scan_column(m_width-1,span))
				{
					has_margins = true;

					m_margins.y = m_height - span.y;
					m_margins.w = span.x;
				}
			}
		}
	}

	if (has_margins)
	{
		// Now swap out the pixels for a sub-image...
		char* new_pixels = static_cast<char*>(OOBase::ThreadLocalAllocator::allocate((m_width-2)*(m_height-2)*m_components));
		if (!new_pixels)
			LOG_ERROR_RETURN(("Failed to allocate 9-patch pixel data!"),false);

		char* dest = new_pixels;
		const char* src = static_cast<const char*>(m_pixels) + (m_width+1)*m_components;
		for (unsigned int y=1;y<m_height-1;++y)
		{
			memcpy(dest,src,(m_width-2)*m_components);

			src += m_width*m_components;
			dest += (m_width-2)*m_components;
		}

		OOBase::ThreadLocalAllocator::free(m_pixels);
		m_pixels = new_pixels;
		m_height -= 2;
		m_width -= 2;
	}
	else if (has_border)
	{
		// Now swap out the pixels for a sub-image...
		char* new_pixels = static_cast<char*>(OOBase::ThreadLocalAllocator::allocate((m_width-1)*(m_height-1)*m_components));
		if (!new_pixels)
			LOG_ERROR_RETURN(("Failed to allocate 9-patch pixel data!"),false);

		char* dest = new_pixels;
		const char* src = static_cast<const char*>(m_pixels) + (m_width+1)*m_components;
		for (unsigned int y=1;y<m_height;++y)
		{
			memcpy(dest,src,(m_width-1)*m_components);

			src += m_width*m_components;
			dest += (m_width-1)*m_components;
		}

		OOBase::ThreadLocalAllocator::free(m_pixels);
		m_pixels = new_pixels;
		m_height -= 1;
		m_width -= 1;
	}

	m_info->m_tex_size = size();

	return true;
}

OOBase::SharedPtr<Indigo::Render::UIDrawable> Indigo::NinePatch::make_drawable(const glm::ivec2& position, const glm::uvec2& size, const glm::vec4& colour) const
{
	if (!m_info)
		LOG_ERROR_RETURN(("NinePatch::make_drawable called with no info!"),OOBase::SharedPtr<Indigo::Render::UIDrawable>());

	bool is_9 = (m_info->m_borders != glm::uvec4(0));

	OOBase::SharedPtr<OOGL::Texture> texture = make_texture(GL_RGBA8,is_9 ? 1 : 0);
	if (!texture)
		return OOBase::SharedPtr<Indigo::Render::UIDrawable>();

	texture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	texture->parameter(GL_TEXTURE_MIN_FILTER,is_9 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
	texture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	texture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	if (is_9)
		return OOBase::allocate_shared<Render::UINinePatch,OOBase::ThreadLocalAllocator>(position,size,colour,texture,m_info);
	else
		return OOBase::allocate_shared<Render::UIImage,OOBase::ThreadLocalAllocator>(texture,size,colour);
}

glm::uvec2 Indigo::NinePatch::min_size() const
{
	glm::uvec2 margins(m_margins.x + m_margins.z,m_margins.y + m_margins.w);
	glm::uvec2 borders(0);
	if (m_info)
	{
		borders.x = m_info->m_borders.x + m_info->m_borders.z;
		borders.y = m_info->m_borders.y + m_info->m_borders.w;
	}

	return glm::max(margins,borders);
}

glm::uvec2 Indigo::NinePatch::ideal_size() const
{
	if (m_info)
		return m_info->m_tex_size;

	return min_size();
}

Indigo::UINinePatch::UINinePatch(UIGroup* parent, const OOBase::SharedPtr<NinePatch>& patch, const glm::vec4& colour, OOBase::uint32_t state, const glm::ivec2& position, const glm::uvec2& size) :
		UIWidget(parent,state,position,size),
		m_9patch(patch),
		m_colour(colour)
{
	if (size == glm::uvec2(0))
		this->size(m_9patch->size());
}

glm::uvec2 Indigo::UINinePatch::min_size() const
{
	return m_9patch->min_size();
}

glm::uvec2 Indigo::UINinePatch::ideal_size() const
{
	return m_9patch->ideal_size();
}

void Indigo::UINinePatch::on_size(const glm::uvec2& sz)
{
	if (m_render_9patch)
		render_pipe()->post(OOBase::make_delegate(m_render_9patch.get(),&Render::UIDrawable::size),sz);
}

bool Indigo::UINinePatch::on_render_create(Indigo::Render::UIGroup* group)
{
	if (!m_9patch)
		return false;

	m_render_9patch = m_9patch->make_drawable(glm::ivec2(0),size(),m_colour);
	if (!m_render_9patch)
		return false;

	if (!group->add_drawable(m_render_9patch,0))
		return false;

	m_render_9patch->show();

	return true;
}
