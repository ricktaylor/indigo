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
		UILayer(Indigo::Render::Window* const window) : Indigo::Render::Layer(window)
		{}

		void on_draw(OOGL::State& glState) const;

		void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
	};
}

void ::UILayer::on_draw(OOGL::State& glState) const
{
	glm::vec2 sz = m_window->window()->size();
	glm::mat4 proj = glm::ortho(0.f,sz.x,0.f,sz.y);

	Indigo::Render::UIGroup::on_draw(glState,proj);
}

void ::UILayer::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	Indigo::Render::UIGroup::on_draw(glState,mvp);
}

Indigo::Render::UIDrawable::UIDrawable(bool visible, const glm::i16vec2& position, const glm::u16vec2& size) :
		m_visible(visible),
		m_position(position),
		m_size(size)
{
}

void Indigo::Render::UIGroup::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	glm::mat4 child_mvp = glm::translate(mvp,glm::vec3(m_position.x,m_position.y,0));

	for (OOBase::Table<unsigned int,OOBase::SharedPtr<UIDrawable>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i=m_children.begin();i!=m_children.end();++i)
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
	else if (m_children.insert(zorder,widget->m_render_group) == m_children.end())
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
		m_render_group = OOBase::static_pointer_cast<Render::UIGroup>(group);

	return OOBase::static_pointer_cast<Indigo::Render::Layer>(group);
}
