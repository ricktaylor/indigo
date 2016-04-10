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

::UILayer::UILayer(Indigo::Render::Window* window) :
		Indigo::Render::UIGroup(true),
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

Indigo::UILayer::UILayer(bool fixed, const glm::uvec4& margins, const glm::uvec2& padding) :
		UIGroup(NULL,eWS_visible),
		m_sizer(fixed,margins,padding)
{
}

void Indigo::UILayer::show(bool visible)
{
	Layer::show(visible);
}

void Indigo::UILayer::on_size(const glm::uvec2& sz)
{
	m_size = sz;
	m_sizer.fit(sz);
}

glm::uvec2 Indigo::UILayer::min_size() const
{
	return m_sizer.min_fit();
}
glm::uvec2 Indigo::UILayer::ideal_size() const
{
	return m_sizer.ideal_fit();
}

void Indigo::UILayer::layout()
{
	m_sizer.fit(m_size);
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

bool Indigo::UILayer::on_mousemove(const double& screen_x, const double& screen_y)
{
	if (!m_size.x || !m_size.y)
		return false;

	return UIGroup::on_mousemove(glm::clamp(glm::ivec2(floor(screen_x),floor(screen_y)),glm::ivec2(0),glm::ivec2(m_size.x-1,m_size.y-1)));
}

bool Indigo::UILayer::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	return UIGroup::on_mousebutton(click);
}
