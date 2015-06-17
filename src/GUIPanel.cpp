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

Indigo::Render::GUI::Panel::Panel() : m_style_flags(0)
{

}

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
		glm::u16vec4 borders = style()->borders();
		sz2.x += borders.x + borders.z;
		sz2.y += borders.y + borders.w;

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

	if (m_style_flags & Indigo::GUI::Panel::show_border)
	{
		if (m_style_flags & Indigo::GUI::Panel::colour_border)
			m_background.draw(glState,style()->background_colour(),style()->background_image(),child_mvp);
		else
			m_background.draw(glState,glm::vec4(1.f),style()->background_image(),child_mvp);

		glm::u16vec4 borders = style()->borders();
		if (borders.x || borders.w)
			child_mvp = glm::translate(child_mvp,glm::vec3(borders.x,borders.w,0));
	}

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
	if (old_sz != new_sz && (m_style_flags & Indigo::GUI::Panel::show_border))
	{
		window()->make_current();

		m_background.layout(new_sz,style()->borders(),style()->background_image_size());
	}

	return new_sz;
}

bool Indigo::Render::GUI::Panel::set_style_flags(unsigned int flags, bool refresh)
{
	m_style_flags = flags;
	if (refresh)
	{
		if (m_style_flags & Indigo::GUI::Panel::show_border)
		{
			window()->make_current();

			m_background.layout(Widget::size(),style()->borders(),style()->background_image_size());
		}
	}

	return true;
}

/*void Indigo::Render::GUI::Panel::borders(const glm::u16vec4& b)
{
	if (m_borders != b)
	{
		glm::u16vec2 sz(Widget::min_size());
		sz.x -= m_borders.x + m_borders.z;
		sz.y -= m_borders.y + m_borders.w;

		m_borders = b;

		min_size(sz);

		if (m_style_flags & Indigo::GUI::Panel::show_border)
		{
			window()->make_current();
			m_background.layout(Widget::size(),style()->borders(),style()->background_image_size());
		}
	}
}*/


/*bool Indigo::Render::GUI::Panel::texture(const OOBase::SharedPtr<OOGL::Texture>& tex, const glm::u16vec2& tex_size)
{
	m_texture = tex;
	if (!m_texture || !m_texture->valid())
		return false;

	if (tex_size != m_tex_size)
	{
		m_tex_size = tex_size;

		if (m_style_flags & Indigo::GUI::Panel::show_border)
		{
			window()->make_current();
			m_background.layout(Widget::size(),style()->borders(),style()->background_image_size());
		}
	}
	return true;
}*/

glm::u16vec2 Indigo::Render::GUI::Panel::min_size(const glm::u16vec2& sz)
{
	glm::u16vec4 borders = style()->borders();
	glm::u16vec2 new_sz(sz);
	if (new_sz.x != glm::u16vec2::value_type(-1) && new_sz.x < borders.x + borders.z)
		new_sz.x = borders.x + borders.z;

	if (new_sz.y != glm::u16vec2::value_type(-1) && new_sz.y < borders.y + borders.w)
		new_sz.y = borders.y + borders.w;

	return Widget::min_size(new_sz);
}

glm::u16vec2 Indigo::Render::GUI::Panel::client_size() const
{
	glm::u16vec4 borders = style()->borders();
	glm::u16vec2 client_sz(Widget::size());
	if (client_sz.x > borders.x + borders.z)
		client_sz.x -= borders.x + borders.z;
	else
		client_sz.x = 0;

	if (client_sz.y > borders.y + borders.w)
		client_sz.y -= borders.y + borders.w;
	else
		client_sz.y = 0;

	return client_sz;
}

glm::u16vec2 Indigo::Render::GUI::Panel::client_size(const glm::u16vec2& sz)
{
	glm::u16vec4 borders = style()->borders();
	glm::u16vec2 client_sz(sz.x + borders.x + borders.z, sz.y + borders.y + borders.w);
	client_sz = size(client_sz);

	if (client_sz.x > borders.x + borders.z)
		client_sz.x -= borders.x + borders.z;
	else
		client_sz.x = 0;

	if (client_sz.y > borders.y + borders.w)
		client_sz.y -= borders.y + borders.w;
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

bool Indigo::GUI::Panel::create(Widget* parent, unsigned int style_flags, const glm::u16vec2& min_size, const glm::i16vec2& pos)
{
	if (!Widget::create(parent,min_size,pos))
		return false;

	if (!common_create(style_flags))
	{
		destroy();
		return false;
	}

	return true;
}

bool Indigo::GUI::Panel::create(Widget* parent, const OOBase::SharedPtr<Style>& style, unsigned int style_flags, const glm::u16vec2& min_size, const glm::i16vec2& pos)
{
	if (!Widget::create(parent,style,min_size,pos))
		return false;

	if (!common_create(style_flags))
	{
		destroy();
		return false;
	}

	return true;
}

bool Indigo::GUI::Panel::common_create(unsigned int flags)
{
	bool ret = false;
	if (flags)
	{
		if (!render_call(OOBase::make_delegate(this,&Panel::set_style_flags),&ret,flags) || !ret)
			return false;
	}

	ret = false;
	return !m_sizer || (render_call(OOBase::make_delegate(this,&Panel::do_sizer),&ret,m_sizer.get()) && ret);
}

const OOBase::SharedPtr<Indigo::GUI::Sizer>& Indigo::GUI::Panel::sizer() const
{
	return m_sizer;
}

unsigned int Indigo::GUI::Panel::style_flags() const
{
	return render_widget<Render::GUI::Panel>()->m_style_flags;
}

bool Indigo::GUI::Panel::style_flags(unsigned int flags)
{
	bool ret = true;
	return render_call(OOBase::make_delegate(this,&Panel::set_style_flags),&ret,flags) && ret;
}

void Indigo::GUI::Panel::set_style_flags(bool* ret_val, unsigned int flags)
{
	*ret_val = render_widget<Render::GUI::Panel>()->set_style_flags(flags,*ret_val);
}

bool Indigo::GUI::Panel::sizer(const OOBase::SharedPtr<Sizer>& s)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Panel::do_sizer),&ret,s.get()) || !ret)
		return false;

	m_sizer = s;
	return true;
}

void Indigo::GUI::Panel::do_sizer(bool* ret_val, Sizer* s)
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
