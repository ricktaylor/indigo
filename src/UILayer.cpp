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
#include "UILayer.h"

namespace
{
	class UILayer : public Indigo::Render::UIGroup, public Indigo::Render::Layer
	{
	public:
		UILayer(Indigo::Render::Window* const window);

		void on_draw(OOGL::State& glState) const;

		void on_size(glm::uvec2 sz) { m_size = sz; }

	private:
		glm::uvec2 m_size;
	};
}

Indigo::Render::UIDrawable::UIDrawable(bool visible, const glm::ivec2& position) :
		m_visible(visible),
		m_position(position)
{
}

void Indigo::Render::UIGroup::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	glm::mat4 child_mvp = glm::translate(mvp,glm::vec3(m_position.x,m_position.y,0));

	for (OOBase::Table<unsigned int,OOBase::SharedPtr<UIDrawable>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i;++i)
	{
		if (i->second->m_visible)
			i->second->on_draw(glState,child_mvp);
	}
}

void Indigo::Render::UIGroup::add_widget_group(UIWidget* widget, unsigned int zorder, bool* ret)
{
	*ret = false;
	widget->m_render_group = OOBase::allocate_shared<Render::UIGroup,OOBase::ThreadLocalAllocator>();
	if (!widget->m_render_group)
		LOG_ERROR(("Failed to allocate group: %s",OOBase::system_error_text()));
	else if (!m_children.insert(zorder,widget->m_render_group))
	{
		LOG_ERROR(("Failed to insert group: %s",OOBase::system_error_text()));
		widget->m_render_group.reset();
	}
	else if (!widget->on_render_create(widget->m_render_group.get()))
	{
		m_children.remove(zorder);
		widget->m_render_group.reset();
	}
	else
		*ret = true;
}

::UILayer::UILayer(Indigo::Render::Window* const window) :
		Indigo::Render::Layer(window),
		m_size(window->window()->size())
{
}

void ::UILayer::on_draw(OOGL::State& glState) const
{
	if (m_visible)
	{
		glm::vec2 pos = m_position;
		glm::vec2 sz = m_size;

		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		Indigo::Render::UIGroup::on_draw(glState,glm::ortho(pos.x,sz.x,pos.y,sz.y));
	}
}

Indigo::UIWidget::UIWidget(const glm::ivec2& position, const glm::uvec2& size) :
		m_visible(false),
		m_enabled(true),
		m_focused(false),
		m_hilighted(false),
		m_position(position),
		m_min_size(size),
		m_max_size(-1)
{
	m_size = min_size();
}

bool Indigo::UIWidget::visible(bool show)
{
	if (show != m_visible)
	{
		if (!can_show(show))
			return false;

		m_visible = show;
	}
	return true;
}

bool Indigo::UIWidget::enable(bool enable)
{
	if (enable != m_enabled)
	{
		if (!can_enable(enable))
			return false;

		m_enabled = enable;
	}
	return true;
}

bool Indigo::UIWidget::focus(bool focused)
{
	if (focused != m_focused)
	{
		if (!enabled() || !can_focus(focused))
			return false;

		m_focused = focused;
	}
	return true;
}

bool Indigo::UIWidget::hilight(bool hilighted)
{
	if (hilighted != m_hilighted)
	{
		if (!enabled() || !can_hilight(hilighted))
			return false;

		m_hilighted = hilighted;
	}
	return true;
}

glm::uvec2 Indigo::UIWidget::size(const glm::uvec2& sz)
{
	if (m_size != sz)
	{
		m_size = sz;

		if (m_min_size.x != glm::uvec2::value_type(-1) && m_size.x < m_min_size.x)
			m_size.x = m_min_size.x;

		if (m_min_size.y != glm::uvec2::value_type(-1) && m_size.y < m_min_size.y)
			m_size.y = m_min_size.y;

		if (m_size.x > m_max_size.x)
			m_size.x = m_max_size.x;

		if (m_size.y > m_max_size.y)
			m_size.y = m_max_size.y;
	}

	return m_size;
}

glm::uvec2 Indigo::UIWidget::min_size() const
{
	glm::uvec2 min_size(m_min_size);
	if (min_size.x == glm::uvec2::value_type(-1) || min_size.y == glm::uvec2::value_type(-1))
	{
		glm::uvec2 ideal(ideal_size());
		if (min_size.x == glm::uvec2::value_type(-1))
			min_size.x = ideal.x;

		if (min_size.y == glm::uvec2::value_type(-1))
			min_size.y = ideal.y;
	}

	return min_size;
}

glm::uvec2 Indigo::UIWidget::min_size(const glm::uvec2& sz)
{
	if (m_min_size != sz)
	{
		m_min_size = sz;

		if (m_min_size.x != glm::uvec2::value_type(-1))
		{
			if (m_max_size.x < m_min_size.x)
				m_max_size.x = m_min_size.x;

			if (m_size.x < m_min_size.x)
				m_size.x = m_min_size.x;
		}

		if (m_min_size.y != glm::uvec2::value_type(-1))
		{
			if (m_max_size.y < m_min_size.y)
				m_max_size.y = m_min_size.y;

			if (m_size.y < m_min_size.y)
				m_size.y = m_min_size.y;
		}
	}

	return m_size;
}

glm::uvec2 Indigo::UIWidget::max_size(const glm::uvec2& sz)
{
	if (m_max_size != sz)
	{
		m_max_size = sz;

		if (m_min_size.x != glm::uvec2::value_type(-1) && m_max_size.x < m_min_size.x)
			m_max_size.x = m_min_size.x;

		if (m_min_size.y != glm::uvec2::value_type(-1) && m_max_size.y < m_min_size.y)
			m_max_size.y = m_min_size.y;

		if (m_size.x > m_max_size.x)
			m_size.x = m_max_size.x;

		if (m_size.y > m_max_size.y)
			m_size.y = m_max_size.y;
	}

	return m_size;
}

glm::uvec2 Indigo::UIWidget::ideal_size() const
{
	glm::uvec2 sz(0);
	if (m_min_size.x != glm::uvec2::value_type(-1))
		sz.x = m_min_size.x;

	if (m_min_size.y != glm::uvec2::value_type(-1))
		sz.y = m_min_size.y;

	return sz;
}

bool Indigo::UIGroup::add_widget(const OOBase::SharedPtr<UIWidget>& widget, unsigned int zorder)
{
	if (m_children.insert(zorder,widget) == m_children.end())
		LOG_ERROR_RETURN(("Failed to insert widget: %s",OOBase::system_error_text()),false);

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate(m_render_group.get(),&Render::UIGroup::add_widget_group),widget.get(),zorder,&ret) || !ret)
		m_children.remove(zorder);

	return ret;
}

bool Indigo::UIGroup::remove_widget(unsigned int zorder)
{
	return m_children.remove(zorder);
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::UILayer::create_render_layer(Indigo::Render::Window* const window)
{
	OOBase::SharedPtr< ::UILayer> group = OOBase::allocate_shared< ::UILayer,OOBase::ThreadLocalAllocator>(window);
	if (!group)
		LOG_ERROR(("Failed to allocate group: %s",OOBase::system_error_text()));
	else
	{
		m_render_group = OOBase::static_pointer_cast<Render::UIGroup>(group);
		m_size = window->window()->size();
	}

	return OOBase::static_pointer_cast<Indigo::Render::Layer>(group);
}

void Indigo::UILayer::on_size(const glm::uvec2& sz)
{
	render_pipe()->post(OOBase::make_delegate(render_group< ::UILayer>().get(),&::UILayer::on_size),sz);

	size(sz);
}
