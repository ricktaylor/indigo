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
#include "../core/Window.h"

#include "UIWidget.h"

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

Indigo::UIWidget::UIWidget(UIGroup* parent, const glm::ivec2& position, const glm::uvec2& size) :
		m_parent(parent),
		m_visible(false),
		/*m_enabled(true),
		m_active(false),
		m_hilighted(false),*/
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

/*bool Indigo::UIWidget::enable(bool enable)
{
	if (enable != m_enabled)
	{
		if (!can_enable(enable))
			return false;

		m_enabled = enable;
	}
	return true;
}

bool Indigo::UIWidget::activate(bool active)
{
	if (active != m_active)
	{
		if (!enabled() || !can_activate(active))
			return false;

		m_active = active;
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
}*/

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
		LOG_ERROR_RETURN(("Failed to insert widget: incomplete parent"),false);

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

glm::uvec2 Indigo::UIGroup::min_size() const
{
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
