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

Indigo::Render::UIDrawable::UIDrawable(bool visible, const glm::ivec2& position) :
		m_visible(visible),
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
	OOBase::SharedPtr<Render::UIGroup> group = OOBase::allocate_shared<Render::UIGroup,OOBase::ThreadLocalAllocator>((widget->state() & Indigo::UIWidget::eWS_visible) != 0,widget->position());
	if (!group)
		LOG_ERROR(("Failed to allocate group: %s",OOBase::system_error_text()));
	else if (!m_children.insert(zorder,group))
		LOG_ERROR(("Failed to insert group: %s",OOBase::system_error_text()));
	else if (!widget->on_render_create(group.get()))
		m_children.remove(zorder);
	else
	{
		widget->m_render_group.swap(group);
		*ret = true;
	}
}

Indigo::UIWidget::UIWidget(UIGroup* parent, const CreateParams& params) :
		m_parent(parent),
		m_state(params.m_state),
		m_position(params.m_position),
		m_size(params.m_size)
{
}

void Indigo::UIWidget::toggle_state(OOBase::uint32_t new_state, OOBase::uint32_t mask)
{
	OOBase::uint32_t change_mask = (m_state ^ new_state) & mask;
	if (change_mask)
	{
		m_state = (m_state & ~mask) | (new_state & mask);

		on_state_change(m_state,change_mask);
	}
}

void Indigo::UIWidget::on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask)
{
	if (change_mask & eWS_visible)
	{
		bool visible = (state & eWS_visible) == eWS_visible;
		if (m_render_group)
			render_pipe()->post(OOBase::make_delegate(static_cast<Render::UIDrawable*>(m_render_group.get()),&Render::UIDrawable::show),visible);
	}
}

void Indigo::UIWidget::show(bool visible)
{
	toggle_state(visible,eWS_visible);
}

void Indigo::UIWidget::enable(bool enable)
{
	toggle_state(enable,eWS_enabled);
}

void Indigo::UIWidget::position(const glm::ivec2& pos)
{
	if (m_position != pos)
	{
		m_position = pos;

		if (m_render_group)
			render_pipe()->post(OOBase::make_delegate(static_cast<Render::UIDrawable*>(m_render_group.get()),&Render::UIDrawable::position),pos);
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

Indigo::UIGroup::UIGroup(UIGroup* parent, const CreateParams& params) :
		UIWidget(parent,params)
{
}

bool Indigo::UIGroup::add_widget(const OOBase::SharedPtr<UIWidget>& widget, unsigned int zorder)
{
	if (!m_render_group)
		LOG_ERROR_RETURN(("Failed to insert widget: incomplete parent"),false);

	if (!m_render_parent)
		m_render_parent = m_render_group;

	if (!m_children.insert(zorder,widget))
		LOG_ERROR_RETURN(("Failed to insert widget: %s",OOBase::system_error_text()),false);

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate(m_render_parent.get(),&Render::UIGroup::add_subgroup),widget.get(),zorder,&ret) || !ret)
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

bool Indigo::UIGroup::on_mousemove(const glm::ivec2& pos)
{
	for (OOBase::Table<unsigned int,OOBase::SharedPtr<UIWidget>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=m_children.back();i;--i)
	{
		if (i->second->visible())
		{
			glm::ivec2 child_pos = i->second->position();
			if (pos.x >= child_pos.x && pos.y >= child_pos.y)
			{
				glm::ivec2 child_size = i->second->size();
				if (pos.x < child_pos.x + child_size.x && pos.y < child_pos.y + child_size.y)
				{
					OOBase::SharedPtr<UIWidget> mouse_child = m_mouse_child.lock();
					if (mouse_child != i->second)
					{
						if (mouse_child)
							mouse_child->on_mouseenter(false);

						m_mouse_child = i->second;
						i->second->on_mouseenter(true);
					}

					return i->second->on_mousemove(pos - child_pos);
				}
			}
		}
	}

	OOBase::SharedPtr<UIWidget> mouse_child = m_mouse_child.lock();
	if (mouse_child)
		mouse_child->on_mouseenter(false);

	m_mouse_child.reset();

	return false;
}

bool Indigo::UIGroup::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	OOBase::SharedPtr<UIWidget> mouse_child = m_mouse_child.lock();
	if (mouse_child && mouse_child->visible())
		return mouse_child->on_mousebutton(click);

	m_mouse_child.reset();

	return false;
}
