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

#include "UIImage.h"

#include "../oogl/BufferObject.h"
#include "../oogl/Shader.h"
#include "../oogl/VertexArrayObject.h"

namespace
{
	class UIImageFactory
	{
	public:
		UIImageFactory();
		~UIImageFactory();

		GLsizei alloc_quad();
		void free_quad(GLsizei p);

		void draw(OOGL::State& state, const glm::mat4& mvp, const glm::vec4& colour);

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

	typedef OOGL::ContextSingleton<UIImageFactory> UIImageFactory_t;

	static const unsigned int vertices_per_quad = 4;
	static const unsigned int elements_per_quad = 6;
}

UIImageFactory::UIImageFactory() : m_allocated(0)
{

}

UIImageFactory::~UIImageFactory()
{

}

bool UIImageFactory::create_program()
{
	OOBase::SharedPtr<OOGL::Shader> shaders[2];
	shaders[0] = Indigo::ShaderPool::add_shader("2d_colour.vert",GL_VERTEX_SHADER,Indigo::static_resources());
	shaders[1] = Indigo::ShaderPool::add_shader("colour_blend.frag",GL_FRAGMENT_SHADER,Indigo::static_resources());
	if (shaders[0] && shaders[1])
		m_ptrProgram = Indigo::ShaderPool::add_program("2d_image",shaders,2);

	return m_ptrProgram;
}

GLsizei UIImageFactory::alloc_quad()
{
	GLsizei quad = GLsizei(-1);

	for (free_list_t::iterator i=m_listFree.begin(); i; ++i)
	{
		if (i->second == 1)
		{
			quad = i->first;
			m_listFree.erase(i);
			break;
		}
		else if (i->second > 1)
		{
			quad = i->first;
			i->first += 1;
			i->second -= 1;
			break;
		}
	}

	if (quad == GLsizei(-1))
	{
		GLsizei new_size = 8;
		while (new_size < m_allocated + 1)
			new_size *= 2;

		OOBase::SharedPtr<OOGL::BufferObject> ptrNewVertices = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW,new_size * vertices_per_quad * sizeof(vertex_data));
		OOBase::SharedPtr<OOGL::BufferObject> ptrNewElements = OOBase::allocate_shared<OOGL::BufferObject,OOBase::ThreadLocalAllocator>(GL_ELEMENT_ARRAY_BUFFER,GL_DYNAMIC_DRAW,new_size * elements_per_quad * sizeof(GLuint));
		if (!ptrNewVertices || !ptrNewElements)
			LOG_ERROR_RETURN(("Failed to allocate VBO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),quad);

		if (m_ptrVertices)
			ptrNewVertices->copy(0,m_ptrVertices,0,m_allocated * vertices_per_quad * sizeof(vertex_data));

		if (m_ptrElements)
			ptrNewElements->copy(0,m_ptrElements,0,m_allocated * elements_per_quad * sizeof(GLuint));

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
				LOG_ERROR_RETURN(("Failed to allocate VAO: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),quad);
		}

		if (!m_ptrProgram && !create_program())
			return quad;

		quad = last->first;
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

	OOBase::SharedPtr<GLuint> ei = m_ptrElements->auto_map<GLuint>(GL_MAP_WRITE_BIT,quad * elements_per_quad * sizeof(GLuint),elements_per_quad * sizeof(GLuint));
	GLuint idx = quad * vertices_per_quad;
	GLuint* e = ei.get();
	e[0] = idx + 0;
	e[1] = idx + 1;
	e[2] = idx + 2;
	e[3] = idx + 2;
	e[4] = idx + 1;
	e[5] = idx + 3;

	OOBase::SharedPtr<vertex_data> attribs = m_ptrVertices->auto_map<vertex_data>(GL_MAP_WRITE_BIT,quad * vertices_per_quad * sizeof(vertex_data),vertices_per_quad * sizeof(vertex_data));
	vertex_data* a = attribs.get();
	a[0].x = 0.f;
	a[0].y = 1.f;
	a[1].x = 0.f;
	a[1].y = 0.f;
	a[2].x = 1.f;
	a[2].y = 1.f;
	a[3].x = 1.f;
	a[3].y = 0.f;

	a[0].u = 0;
	a[0].v = 0;
	a[1].u = 0;
	a[1].v = 0xFFFF;
	a[2].u = 0xFFFF;
	a[2].v = 0;
	a[3].u = 0xFFFF;
	a[3].v = 0xFFFF;

	return quad;
}

void UIImageFactory::free_quad(GLsizei p)
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

void UIImageFactory::draw(OOGL::State& glState, const glm::mat4& mvp, const glm::vec4& colour)
{
	if (m_ptrProgram)
	{
		glState.use(m_ptrProgram);

		m_ptrProgram->uniform("in_Colour",colour);
		m_ptrProgram->uniform("MVP",mvp);

		m_ptrVAO->draw_elements(GL_TRIANGLES,elements_per_quad,GL_UNSIGNED_INT,0);
	}
}

Indigo::Render::UIImage::UIImage(const glm::ivec2& position, const glm::u16vec2& size, bool visible, const glm::vec4& colour, const OOBase::SharedPtr<OOGL::Texture>& texture) :
		UIDrawable(position,visible),
		m_colour(colour),
		m_size(size.x,size.y,1.f),
		m_quad(-1),
		m_texture(texture)
{
	m_quad = UIImageFactory_t::instance().alloc_quad();
}

Indigo::Render::UIImage::~UIImage()
{
	if (m_quad != GLsizei(-1))
		UIImageFactory_t::instance().free_quad(m_quad);
}

bool Indigo::Render::UIImage::valid() const
{
	return m_quad != GLsizei(-1);
}

void Indigo::Render::UIImage::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_quad != GLsizei(-1) && m_texture && m_colour.a > 0.f)
	{
		glState.bind(0,m_texture);

		UIImageFactory_t::instance().draw(glState,glm::scale(mvp,m_size),m_colour);
	}
}

Indigo::UIImage::UIImage() : Image()
{
}

Indigo::UIImage::~UIImage()
{
	unload();
}

void Indigo::UIImage::do_unload()
{
	m_texture.reset();
}

void Indigo::UIImage::unload()
{
	Image::unload();

	if (m_texture)
	{
		if (m_texture.unique())
			render_pipe()->call(OOBase::make_delegate(this,&UIImage::do_unload));
		else
			m_texture.reset();
	}
}

OOBase::SharedPtr<Indigo::Render::UIImage> Indigo::UIImage::make_drawable(const glm::ivec2& position, const glm::u16vec2& size, bool visible, const glm::vec4& colour)
{
	if (!m_texture)
	{
		m_texture = make_texture(GL_RGBA8);
		if (!m_texture)
			return OOBase::SharedPtr<Indigo::Render::UIImage>();

		m_texture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		m_texture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		m_texture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		m_texture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}

	return OOBase::allocate_shared<Render::UIImage,OOBase::ThreadLocalAllocator>(position,size,visible,colour,m_texture);
}
