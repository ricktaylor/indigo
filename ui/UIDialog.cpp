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

#include "UIDialog.h"

namespace
{
	class UIDialog : public Indigo::Render::UIGroup, public Indigo::Render::Layer
	{
	public:
		UIDialog(Indigo::Render::Window* window);

		void on_draw(OOGL::State& glState) const;
		void on_size(const glm::uvec2& sz);

		glm::mat4 m_mvp;
	};
}

::UIDialog::UIDialog(Indigo::Render::Window* window) :
		Indigo::Render::UIGroup(true),
		Indigo::Render::Layer(window)
{
	glm::vec2 sz = window->window()->size();
	m_mvp = glm::ortho(0.f,sz.x,0.f,sz.y);
}

void ::UIDialog::on_draw(OOGL::State& glState) const
{
	glState.enable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
	Indigo::Render::UIGroup::on_draw(glState,m_mvp);
}

void ::UIDialog::on_size(const glm::uvec2& sz)
{
	glm::vec2 sz2 = sz;
	m_mvp = glm::ortho(0.f,sz2.x,0.f,sz2.y);
}

Indigo::UIDialog::UIDialog(const OOBase::SharedPtr<Window>& wnd, const CreateParams& params) :
		UIGroup(NULL,params),
		m_wnd(wnd),
		m_sizer(params.m_fixed,params.m_margins,params.m_padding),
		m_modal(params.m_modal)
{
	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());

}

void Indigo::UIDialog::show(bool visible)
{
	Layer::show(visible);
	UIGroup::show(visible);
}

bool Indigo::UIDialog::add_named_widget(const OOBase::SharedPtr<UIWidget>& widget, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& name)
{
	if (!m_names.insert(name,widget))
		LOG_ERROR_RETURN(("Failed to insert widget name: %s",OOBase::system_error_text()),false);

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UIDialog::find_widget(const char* name, size_t len) const
{
	OOBase::SharedString<OOBase::ThreadLocalAllocator> str;
	if (!str.assign(name,len))
		LOG_ERROR_RETURN(("Failed to assign string: %s",OOBase::system_error_text()),OOBase::SharedPtr<Indigo::UIWidget>());

	return find_widget(str);
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UIDialog::find_widget(const OOBase::SharedString<OOBase::ThreadLocalAllocator>& name) const
{
	OOBase::HashTable<OOBase::SharedString<OOBase::ThreadLocalAllocator>,OOBase::WeakPtr<UIWidget>,OOBase::ThreadLocalAllocator>::const_iterator i = m_names.find(name);
	if (i == m_names.end())
		return OOBase::SharedPtr<Indigo::UIWidget>();
	return i->second.lock();
}

void Indigo::UIDialog::on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask)
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

void Indigo::UIDialog::on_size(const glm::uvec2& sz)
{
	m_size = sz;
	m_sizer.fit(sz);
}

glm::uvec2 Indigo::UIDialog::min_size() const
{
	return m_sizer.min_fit();
}
glm::uvec2 Indigo::UIDialog::ideal_size() const
{
	return m_sizer.ideal_fit();
}

OOBase::SharedPtr<Indigo::Window> Indigo::UIDialog::window() const
{
	return m_wnd.lock();
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::UIDialog::create_render_layer(Indigo::Render::Window* window)
{
	OOBase::SharedPtr< ::UIDialog> group = OOBase::allocate_shared< ::UIDialog,OOBase::ThreadLocalAllocator>(window);
	if (!group)
		LOG_ERROR(("Failed to allocate group: %s",OOBase::system_error_text()));
	else
	{
		m_render_group = OOBase::static_pointer_cast<Render::UIGroup>(group);
		
		m_size = window->window()->size();
	}

	return OOBase::static_pointer_cast<Indigo::Render::Layer>(group);
}

bool Indigo::UIDialog::on_mousemove(const double& screen_x, const double& screen_y)
{
	if (!m_size.x || !m_size.y)
		return false;

	bool ret = UIGroup::on_mousemove(glm::clamp(glm::ivec2(floor(screen_x),floor(screen_y)),glm::ivec2(0),glm::ivec2(m_size.x-1,m_size.y-1)));
	return m_modal || ret;
}

bool Indigo::UIDialog::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	bool ret = UIGroup::on_mousebutton(click);
	return m_modal || ret;
}
