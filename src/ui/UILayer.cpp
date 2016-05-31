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

#include "../../include/indigo/ui/UILayer.h"

namespace
{
	class UILayer : public Indigo::Render::UIGroup, public Indigo::Render::Layer
	{
	public:
		UILayer(Indigo::Render::Window* window, const glm::vec2& sz);

		void on_draw(OOGL::State& glState) const;
		void on_size(const glm::uvec2& sz);

		glm::mat4 m_mvp;
	};
}

::UILayer::UILayer(Indigo::Render::Window* window, const glm::vec2& sz) :
		Indigo::Render::UIGroup(true),
		Indigo::Render::Layer(window)
{
	m_mvp = glm::ortho(0.f,sz.x,0.f,sz.y);
}

void ::UILayer::on_draw(OOGL::State& glState) const
{
	glState.enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	Indigo::Render::UIGroup::on_draw(glState,m_mvp);
}

void ::UILayer::on_size(const glm::uvec2& sz)
{
	glm::vec2 sz2 = sz;
	m_mvp = glm::ortho(0.f,sz2.x,0.f,sz2.y);
}

Indigo::UILayer::UILayer(Window* wnd, const CreateParams& params) :
		UIGroup(NULL,params),
		m_wnd(wnd),
		m_sizer(params.m_fixed,params.m_margins,params.m_padding),
		m_modal(params.m_modal)
{
	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
}

void Indigo::UILayer::show(bool visible)
{
	Layer::show(visible);
	UIGroup::show(visible);
}

bool Indigo::UILayer::add_named_widget(const OOBase::SharedPtr<UIWidget>& widget, const char* name, size_t len)
{
	if (!m_names.insert(OOBase::Hash<const char*>::hash(name,len),widget))
		LOG_ERROR_RETURN(("Failed to insert widget name: %s",OOBase::system_error_text()),false);

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILayer::find_widget(const char* name, size_t len) const
{
	OOBase::HashTable<size_t,OOBase::WeakPtr<UIWidget>,OOBase::ThreadLocalAllocator>::const_iterator i = m_names.find(OOBase::Hash<const char*>::hash(name,len));
	if (i == m_names.end())
		return OOBase::SharedPtr<Indigo::UIWidget>();
	return i->second.lock();
}

void Indigo::UILayer::on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask)
{
	UIGroup::on_state_change(state,change_mask);

	if (change_mask & UIWidget::eWS_visible)
	{
		bool visible = (state & eWS_visible) == eWS_visible;
		if (visible)
			m_sizer.fit(m_size);

		Layer::show(visible);
	}
}

OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> Indigo::UILayer::on_close(const OOBase::Delegate0<void,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> prev = m_on_close;
	m_on_close = delegate;
	return prev;
}

bool Indigo::UILayer::on_close()
{
	if (m_on_close)
	{
		m_on_close.invoke();
		return true;
	}
	return false;
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

Indigo::Window* Indigo::UILayer::window() const
{
	return m_wnd;
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::UILayer::create_render_layer(Indigo::Render::Window* window)
{
	m_size = window->window()->size();

	OOBase::SharedPtr< ::UILayer> group = OOBase::allocate_shared< ::UILayer,OOBase::ThreadLocalAllocator>(window,m_size);
	if (!group)
		LOG_ERROR(("Failed to allocate group: %s",OOBase::system_error_text()));
	else
	{
		m_group = OOBase::static_pointer_cast<Render::UIGroup>(group);
		m_render_group = m_group.get();
	}

	return OOBase::static_pointer_cast<Indigo::Render::Layer>(group);
}

void Indigo::UILayer::destroy_render_layer()
{
	m_group.reset();

	Layer::destroy_render_layer();
}

bool Indigo::UILayer::on_mousemove(const double& screen_x, const double& screen_y)
{
	if (!m_size.x || !m_size.y)
		return false;

	bool ret = UIGroup::on_mousemove(glm::clamp(glm::ivec2(floor(screen_x),floor(screen_y)),glm::ivec2(0),glm::ivec2(m_size.x-1,m_size.y-1)));
	return m_modal || ret;
}

bool Indigo::UILayer::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	bool ret = UIGroup::on_mousebutton(click);
	return m_modal || ret;
}
