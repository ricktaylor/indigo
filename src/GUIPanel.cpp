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

#include "GUIPanel.h"
#include "Render.h"

void Indigo::Render::GUI::Panel::layout()
{
	if (m_sizer)
		m_sizer->layout(*this);
}

glm::u16vec2 Indigo::Render::GUI::Panel::ideal_size() const
{
	glm::u16vec2 sz(Widget::ideal_size());
	if (m_sizer)
	{
		glm::u16vec2 sz2 = m_sizer->ideal_size();
		sz2.x += m_borders.x + m_borders.z;
		sz2.y += m_borders.y + m_borders.w;

		if (sz2.x > sz.x)
			sz.x = sz2.x;

		if (sz2.y > sz.y)
			sz.y = sz2.y;
	}
	return sz;
}

bool Indigo::Render::GUI::Panel::add_child(const OOBase::SharedPtr<Widget>& child)
{
	return m_children.push_back(child);
}

void Indigo::Render::GUI::Panel::remove_child(const OOBase::SharedPtr<Widget>& child)
{
	m_children.erase(child);
}

void Indigo::Render::GUI::Panel::draw(OOGL::State& glState, const glm::mat4& mvp)
{
	glm::i16vec2 p = position();
	glm::mat4 child_mvp = mvp * glm::translate(glm::mat4(1),glm::vec3(p.x,p.y,0));

	m_background.draw(glState,m_texture,child_mvp);

	child_mvp = glm::translate(child_mvp,glm::vec3(m_borders.x,m_borders.w,0));

	for (OOBase::Vector<OOBase::SharedPtr<Widget>,OOBase::ThreadLocalAllocator>::iterator i=m_children.begin();i;++i)
	{
		if ((*i)->visible())
			(*i)->draw(glState,child_mvp);
	}
}

glm::u16vec2 Indigo::Render::GUI::Panel::size(const glm::u16vec2& sz)
{
	glm::u16vec2 old_sz = Widget::size();
	glm::u16vec2 new_sz = Widget::size(sz);
	if (old_sz != new_sz)
	{
		window()->make_current();

		layout_background(new_sz);
	}

	return new_sz;
}

const glm::u16vec4& Indigo::Render::GUI::Panel::borders() const
{
	return m_borders;
}

void Indigo::Render::GUI::Panel::borders(const glm::u16vec4& b)
{
	if (m_borders != b)
	{
		glm::u16vec2 sz(Widget::min_size());
		sz.x -= m_borders.x + m_borders.z;
		sz.y -= m_borders.y + m_borders.w;

		m_borders = b;

		min_size(sz);

		window()->make_current();

		layout_background(Widget::size());
	}
}

const OOBase::SharedPtr<OOGL::Texture>& Indigo::Render::GUI::Panel::texture() const
{
	return m_texture;
}

bool Indigo::Render::GUI::Panel::texture(const OOBase::SharedPtr<OOGL::Texture>& tex, const glm::u16vec2& tex_size)
{
	m_texture = tex;
	if (!m_texture || !m_texture->valid())
		return false;

	window()->make_current();

	m_texture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	m_texture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	m_texture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	m_texture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

	if (tex_size != m_tex_size)
	{
		m_tex_size = tex_size;
		layout_background(Widget::size());
	}
	return true;
}

void Indigo::Render::GUI::Panel::layout_background(const glm::u16vec2& sz)
{
	if (m_texture && m_texture->valid() && sz.x && sz.y)
		m_background.layout(sz,m_borders,m_tex_size);
}

glm::u16vec2 Indigo::Render::GUI::Panel::min_size(const glm::u16vec2& sz)
{
	glm::u16vec2 new_sz(sz);
	if (new_sz.x != glm::u16vec2::value_type(-1) && new_sz.x < m_borders.x + m_borders.z)
		new_sz.x = m_borders.x + m_borders.z;

	if (new_sz.y != glm::u16vec2::value_type(-1) && new_sz.y < m_borders.y + m_borders.w)
		new_sz.y = m_borders.y + m_borders.w;

	return Widget::min_size(new_sz);
}

glm::u16vec2 Indigo::Render::GUI::Panel::client_size() const
{
	glm::u16vec2 client_sz(Widget::size());
	if (client_sz.x > m_borders.x + m_borders.z)
		client_sz.x -= m_borders.x + m_borders.z;
	else
		client_sz.x = 0;

	if (client_sz.y > m_borders.y + m_borders.w)
		client_sz.y -= m_borders.y + m_borders.w;
	else
		client_sz.y = 0;

	return client_sz;
}

glm::u16vec2 Indigo::Render::GUI::Panel::client_size(const glm::u16vec2& sz)
{
	glm::u16vec2 client_sz(sz.x + m_borders.x + m_borders.z, sz.y + m_borders.y + m_borders.w);
	client_sz = size(client_sz);

	if (client_sz.x > m_borders.x + m_borders.z)
		client_sz.x -= m_borders.x + m_borders.z;
	else
		client_sz.x = 0;

	if (client_sz.y > m_borders.y + m_borders.w)
		client_sz.y -= m_borders.y + m_borders.w;
	else
		client_sz.y = 0;

	return client_sz;
}

OOBase::SharedPtr<Indigo::Render::GUI::Widget> Indigo::GUI::Panel::create_render_widget()
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> layer = OOBase::allocate_shared<Indigo::Render::GUI::Panel,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR(("Failed to allocate layer: %s",OOBase::system_error_text()));

	return layer;
}

bool Indigo::GUI::Panel::create(Widget* parent, const glm::u16vec2& min_size, const glm::i16vec2& pos)
{
	if (!Widget::create(parent,min_size,pos))
		return false;

	bool fit = true;
	if (m_sizer && (!render_call(OOBase::make_delegate(this,&Panel::do_sizer),m_sizer.get(),&fit) || !fit))
	{
		destroy();
		return false;
	}

	return true;
}

const OOBase::SharedPtr<Indigo::GUI::Sizer>& Indigo::GUI::Panel::sizer() const
{
	return m_sizer;
}

bool Indigo::GUI::Panel::sizer(const OOBase::SharedPtr<Sizer>& s)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Panel::do_sizer),s.get(),&ret) || !ret)
		return false;

	m_sizer = s;
	return true;
}

void Indigo::GUI::Panel::do_sizer(Sizer* s, bool* ret_val)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (!panel)
		*ret_val = false;
	else
	{
		panel->m_sizer = s->m_sizer;
		*ret_val = true;
	}
}

bool Indigo::GUI::Panel::fit()
{
	return !m_sizer || m_sizer->fit(*this);
}

bool Indigo::GUI::Panel::layout()
{
	return !m_sizer || m_sizer->layout(*this);
}

const OOBase::SharedPtr<Indigo::Image>& Indigo::GUI::Panel::background() const
{
	return m_background;
}

bool Indigo::GUI::Panel::background(const OOBase::SharedPtr<Image>& image)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Panel::do_background),image.get(),&ret) || !ret)
		return false;

	m_background = image;
	return true;
}

bool Indigo::GUI::Panel::background(const OOGL::ResourceBundle& resource, const char* name)
{
	OOBase::SharedPtr<Image> image = OOBase::allocate_shared<Image>();
	if (!image)
		LOG_ERROR_RETURN(("Failed to allocate image: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!image->load(resource,name))
		return false;

	return background(image);
}

void Indigo::GUI::Panel::do_background(Image* image, bool* ret_val)
{
	*ret_val = false;
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (panel)
	{
		OOBase::SharedPtr<OOGL::Image> bg = image->render_image();
		if (bg && bg->valid())
			*ret_val = panel->texture(bg->make_texture(GL_RGBA8),bg->size());
	}
}

glm::u16vec4 Indigo::GUI::Panel::borders() const
{
	glm::u16vec4 borders(0);
	render_call(OOBase::make_delegate(const_cast<Panel*>(this),&Panel::do_get_borders),&borders);
	return borders;
}

void Indigo::GUI::Panel::do_get_borders(glm::u16vec4* borders)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (panel)
		*borders = panel->borders();
}

bool Indigo::GUI::Panel::borders(OOBase::uint16_t left, OOBase::uint16_t top, OOBase::uint16_t right, OOBase::uint16_t bottom)
{
	glm::u16vec4 borders(left,top,right,bottom);
	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Panel::do_set_borders),&borders,&ret) && ret;
}

void Indigo::GUI::Panel::do_set_borders(glm::u16vec4* borders, bool* ret_val)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (!panel)
		*ret_val = false;
	else
	{
		panel->borders(*borders);
		*ret_val = true;
	}
}
