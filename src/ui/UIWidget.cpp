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

#include "../Common.h"

#include "../../include/indigo/Render.h"
#include "../../include/indigo/Window.h"

#include "../../include/indigo/ui/UIWidget.h"

Indigo::Render::UIDrawable::UIDrawable(bool visible, const glm::ivec2& position) :
		m_visible(visible),
		m_position(position)
{
}

void Indigo::Render::UIGroup::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	for (OOBase::Vector<OOBase::SharedPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i;++i)
	{
		if ((*i)->m_visible)
			(*i)->on_draw(glState,glm::translate(mvp,glm::vec3((*i)->m_position.x,(*i)->m_position.y,0)));
	}
}

bool Indigo::Render::UIGroup::add_drawable(const OOBase::SharedPtr<UIDrawable>& drawable)
{
	if (!m_children.push_back(drawable))
		LOG_ERROR_RETURN(("Failed to insert drawable: %s",OOBase::system_error_text()),false);
	return true;
}

bool Indigo::Render::UIGroup::remove_drawable(const OOBase::SharedPtr<UIDrawable>& drawable)
{
	return m_children.remove(drawable) != 0;
}

void Indigo::Render::UIGroup::add_subgroup(UIWidget* widget, bool* ret)
{
	*ret = false;
	OOBase::SharedPtr<Render::UIGroup> group = OOBase::allocate_shared<Render::UIGroup,OOBase::ThreadLocalAllocator>((widget->state() & Indigo::UIWidget::eWS_visible) != 0,widget->position());
	if (!group)
		LOG_ERROR(("Failed to allocate group: %s",OOBase::system_error_text()));
	else if (!m_children.push_back(group))
		LOG_ERROR(("Failed to insert group: %s",OOBase::system_error_text()));
	else if (!widget->on_render_create(group.get()))
		m_children.pop_back();
	else
	{
		widget->m_render_group = group.get();
		*ret = true;
	}
}

Indigo::UIWidget::UIWidget(UIGroup* parent, const CreateParams& params) :
		m_parent(parent),
		m_render_group(NULL),
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
			render_pipe()->post(OOBase::make_delegate(static_cast<Render::UIDrawable*>(m_render_group),&Render::UIDrawable::show),visible);
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
			render_pipe()->post(OOBase::make_delegate(static_cast<Render::UIDrawable*>(m_render_group),&Render::UIDrawable::position),pos);
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
		UIWidget(parent,params),
		m_render_parent(NULL)
{
}

bool Indigo::UIGroup::add_widget(const OOBase::SharedPtr<UIWidget>& widget, const char* name, size_t len)
{
	if (!m_render_group)
		LOG_ERROR_RETURN(("Failed to insert widget: incomplete parent"),false);

	if (!m_render_parent)
		m_render_parent = m_render_group;

	if (!m_children.push_back(widget))
		LOG_ERROR_RETURN(("Failed to insert widget: %s",OOBase::system_error_text()),false);

	if (name && len && !add_named_widget(widget,name,len))
	{
		m_children.pop_back();
		return false;
	}

	bool ret = false;
	if (!render_pipe()->call(OOBase::make_delegate(m_render_parent,&Render::UIGroup::add_subgroup),widget.get(),&ret) || !ret)
		m_children.pop_back();

	return ret;
}

bool Indigo::UIGroup::add_named_widget(const OOBase::SharedPtr<UIWidget>& widget, const char* name, size_t len)
{
	return m_parent->add_named_widget(widget,name,len);
}

bool Indigo::UIGroup::remove_widget(const OOBase::SharedPtr<UIWidget>& widget)
{
	return m_children.remove(widget) != 0;
}

glm::uvec2 Indigo::UIGroup::min_size() const
{
	glm::uvec2 min(0);
	for (OOBase::Vector<OOBase::SharedPtr<UIWidget>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i;++i)
	{
		if ((*i)->visible())
		{
			glm::uvec2 min1((*i)->min_size());
			min1 += (*i)->position();
			min = glm::max(min1,min);
		}
	}

	return min;
}

glm::uvec2 Indigo::UIGroup::ideal_size() const
{
	glm::uvec2 ideal(0);
	for (OOBase::Vector<OOBase::SharedPtr<UIWidget>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i;++i)
	{
		if ((*i)->visible())
		{
			glm::uvec2 ideal1((*i)->ideal_size());
			ideal1 += (*i)->position();
			ideal = glm::max(ideal1,ideal);
		}
	}

	return ideal;
}

bool Indigo::UIGroup::on_mousemove(const glm::ivec2& pos)
{
	for (OOBase::Vector<OOBase::SharedPtr<UIWidget>,OOBase::ThreadLocalAllocator>::iterator i=m_children.back();i;--i)
	{
		if ((*i)->visible())
		{
			glm::ivec2 child_pos = (*i)->position();
			if (pos.x >= child_pos.x && pos.y >= child_pos.y)
			{
				glm::ivec2 child_size = (*i)->size();
				if (pos.x < child_pos.x + child_size.x && pos.y < child_pos.y + child_size.y)
				{
					OOBase::SharedPtr<UIWidget> mouse_child = m_mouse_child.lock();
					if (mouse_child != (*i))
					{
						if (mouse_child)
							mouse_child->on_mouseenter(false);

						m_mouse_child = (*i);
						(*i)->on_mouseenter(true);
					}

					return (*i)->on_mousemove(pos - child_pos);
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
	if (mouse_child && mouse_child->visible() && mouse_child->enabled())
		return mouse_child->on_mousebutton(click);

	m_mouse_child.reset();

	return false;
}
