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

#include "UILayer.h"

namespace
{
	class UILayer : public Indigo::Render::UIGroup, public Indigo::Render::Layer
	{
	public:
		UILayer(Indigo::Render::Window* window);

		void on_draw(OOGL::State& glState) const;
	};
}

Indigo::Render::UIDrawable::UIDrawable(const glm::ivec2& position) :
		m_visible(false),
		m_position(position)
{
}

void Indigo::Render::UIGroup::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<UIDrawable>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i;++i)
	{
		if (i->second->m_visible)
			i->second->on_draw(glState,glm::translate(mvp,glm::vec3(i->second->m_position.x,i->second->m_position.y,0)));
	}
}

bool Indigo::Render::UIGroup::add_drawable(const OOBase::SharedPtr<UIDrawable>& drawable, unsigned int zorder)
{
	if (!m_children.insert(zorder,drawable))
		LOG_ERROR_RETURN(("Failed to insert drawable: %s",OOBase::system_error_text()),false);
	return true;
}

bool Indigo::Render::UIGroup::remove_drawable(unsigned int zorder)
{
	return m_children.remove(zorder);
}

void Indigo::Render::UIGroup::add_subgroup(UIWidget* widget, unsigned int zorder, bool* ret)
{
	*ret = false;
	widget->m_render_group = OOBase::allocate_shared<Render::UIGroup,OOBase::ThreadLocalAllocator>(widget->position());
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
	{
		if (widget->visible())
			widget->m_render_group->show(true);

		*ret = true;
	}
}

::UILayer::UILayer(Indigo::Render::Window* window) :
		Indigo::Render::Layer(window)
{
}

void ::UILayer::on_draw(OOGL::State& glState) const
{
	glState.enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glm::vec2 sz = m_window->window()->size();
	Indigo::Render::UIGroup::on_draw(glState,glm::ortho(0.f,sz.x,0.f,sz.y));
}

Indigo::UIWidget::UIWidget(UIGroup* parent, const glm::ivec2& position, const glm::uvec2& size) :
		m_parent(parent),
		m_visible(false),
		m_enabled(true),
		m_focused(false),
		m_hilighted(false),
		m_position(position),
		m_size(size)
{
}

void Indigo::UIWidget::show(bool visible)
{
	if (visible != m_visible)
	{
		m_visible = visible;

		if (m_render_group)
			render_pipe()->post(OOBase::make_delegate(render_group<Render::UIDrawable>().get(),&Render::UIDrawable::show),visible);
	}
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

void Indigo::UIWidget::position(const glm::ivec2& pos)
{
	if (m_position != pos)
	{
		m_position = pos;

		if (m_render_group)
			render_pipe()->post(OOBase::make_delegate(render_group<Render::UIDrawable>().get(),&Render::UIDrawable::position),pos);
	}
}

glm::uvec2 Indigo::UIWidget::size(const glm::uvec2& sz)
{
	if (m_size != sz)
	{
		glm::uvec2 prev_size = m_size;

		m_size = glm::max(sz,this->min_size());

		if (prev_size != m_size)
			on_size(m_size);
	}

	return m_size;
}

Indigo::UIGroup::UIGroup(UIGroup* parent, const glm::ivec2& position, const glm::uvec2& size) : 
		UIWidget(parent,position,size)
{
}

bool Indigo::UIGroup::add_widget(const OOBase::SharedPtr<UIWidget>& widget, unsigned int zorder)
{
	if (!m_render_group)
		return false;

	if (!m_children.insert(zorder,widget))
		LOG_ERROR_RETURN(("Failed to insert widget: %s",OOBase::system_error_text()),false);

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate(m_render_group.get(),&Render::UIGroup::add_subgroup),widget.get(),zorder,&ret) || !ret)
		m_children.remove(zorder);

	return ret;
}

bool Indigo::UIGroup::remove_widget(unsigned int zorder)
{
	return m_children.remove(zorder);
}

void Indigo::UIGroup::sizer(const OOBase::SharedPtr<UISizer>& s)
{
	if (s != m_sizer)
	{
		m_sizer = s;
		if (m_sizer)
			m_sizer->size(m_size);
	}
}

glm::uvec2 Indigo::UIGroup::min_size() const
{
	if (m_sizer)
		return m_sizer->min_size();

	glm::uvec2 min(0);
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<UIWidget>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i;++i)
	{
		if (i->second->visible())
		{
			glm::uvec2 min1(i->second->min_size());
			min1 += i->second->position();
			min = glm::max(min1,min);
		}
	}

	return min;
}

glm::uvec2 Indigo::UIGroup::ideal_size() const
{
	if (m_sizer)
		return m_sizer->ideal_size();

	glm::uvec2 ideal(0);
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<UIWidget>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i;++i)
	{
		if (i->second->visible())
		{
			glm::uvec2 ideal1(i->second->ideal_size());
			ideal1 += i->second->position();
			ideal = glm::max(ideal1,ideal);
		}
	}

	return ideal;
}

void Indigo::UIGroup::on_size(const glm::uvec2& sz)
{
	if (m_sizer)
		m_sizer->size(sz);
}

Indigo::UILayer::UILayer() : 
		UIGroup(NULL)
{
}

void Indigo::UILayer::show(bool visible)
{
	UIGroup::show(visible);
	Layer::show(visible);
}

void Indigo::UILayer::on_size(const glm::uvec2& sz)
{
	UIGroup::on_size(sz);
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::UILayer::create_render_layer(Indigo::Render::Window* window)
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
