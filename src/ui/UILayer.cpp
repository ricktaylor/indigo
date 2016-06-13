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

#include "../../include/indigo/ui/UILayer.h"
#include "../../include/indigo/Window.h"
#include "../../include/indigo/Render.h"

#include "../Common.h"

Indigo::Render::UILayer::UILayer(Indigo::Render::Window* window, Indigo::UILayer* owner, bool visible, const glm::ivec2& pos, const glm::uvec2& sz) :
		Indigo::Render::UIGroup(visible,pos,sz),
		Indigo::Render::Layer(window),
		m_owner(owner)
{
	size(sz);
	glm::vec2 sz2(sz);
	m_mvp = glm::ortho(0.f,sz2.x,0.f,sz2.y);
}

void Indigo::Render::UILayer::on_draw(OOGL::State& glState) const
{
	if (visible())
	{
		glState.enable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glDepthMask(GL_FALSE);
		glState.disable(GL_DEPTH_TEST);

		Indigo::Render::UIGroup::on_draw(glState,m_mvp);
	}
}

bool Indigo::Render::UILayer::on_update(OOGL::State& glState)
{
	// TODO Enable dirty flag
	return true;
}

void Indigo::Render::UILayer::on_size(const glm::uvec2& sz)
{
	if (sz != size())
	{
		size(sz);
		glm::vec2 sz2(sz);
		m_mvp = glm::ortho(0.f,sz2.x,0.f,sz2.y);

		Indigo::logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::UILayer::on_layout),sz);
	}
}

bool Indigo::Render::UILayer::on_cursormove(const glm::dvec2& pos)
{
	if (!visible())
		return false;

	const glm::uvec2& sz = size();
	if (!sz.x || !sz.y)
		return false;

	OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator> hits;
	hit_test(hits,glm::clamp(glm::ivec2(glm::floor(pos)),glm::ivec2(0),glm::ivec2(sz.x-1,sz.y-1)));

	OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator i=m_cursor_hits.back();
	OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator j=hits.back();
	for (;i && j && *i == *j;--i,--j)
		;

	// Everything from i loses cursor
	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator k=m_cursor_hits.begin();k && k < i;++k)
	{
		OOBase::SharedPtr<UIDrawable> d = k->lock();
		if (d && d->on_cursorenter(false))
			break;
	}

	// Everything from j gains cursor
	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator k=hits.begin();k && k < j;++k)
	{
		OOBase::SharedPtr<UIDrawable> d = k->lock();
		if (d && d->on_cursorenter(true))
			break;
	}

	m_cursor_hits.swap(hits);
	hits.clear();

	bool handled = false;
	for (i=m_cursor_hits.begin();!handled && i;++i)
	{
		OOBase::SharedPtr<UIDrawable> d = i->lock();
		if (d)
			handled = d->on_cursormove();
	}

	return m_owner->m_modal || handled;
}

bool Indigo::Render::UILayer::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	bool grabbed_outer = false;
	bool handled = false;
	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator i=m_cursor_hits.begin();!handled && i;++i)
	{
		OOBase::SharedPtr<UIDrawable> d = i->lock();
		if (d)
		{
			bool grabbed = false;
			handled = d->on_mousebutton(click,grabbed);
			if (grabbed)
			{
				grabbed_outer = true;
				if (*i != m_focus_child)
				{
					// Grabbed focus!
					OOBase::SharedPtr<UIDrawable> prev_focus_child = m_focus_child.lock();
					if (prev_focus_child)
						prev_focus_child->on_losefocus();

					m_focus_child = *i;
				}
			}
		}
	}

	return grabbed_outer;
}


Indigo::UILayer::UILayer(const CreateParams& params) :
		UIGroup(NULL,params),
		m_sizer(params.m_fixed,params.m_margins,params.m_padding),
		m_modal(params.m_modal)
{
	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
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
	if (change_mask & UIWidget::eWS_visible)
	{
		bool visible = (state & eWS_visible) == eWS_visible;
		if (visible)
			m_sizer.fit(size());

		if (m_render_parent)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(static_cast<Render::UIDrawable*>(m_render_parent),&Render::UIDrawable::show),visible);
	}

	UIGroup::on_state_change(state,change_mask);
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

void Indigo::UILayer::on_layout(const glm::uvec2& sz)
{
	OOBase::Guard<Pipe> lock(*render_pipe());

	size(sz);
}

void Indigo::UILayer::on_size(glm::uvec2& sz)
{
	m_sizer.fit(sz);

	if (m_render_parent)
		render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(static_cast<Render::UIDrawable*>(m_render_parent),&Render::UIDrawable::size),sz);
}

glm::uvec2 Indigo::UILayer::min_size() const
{
	return m_sizer.min_fit();
}

glm::uvec2 Indigo::UILayer::ideal_size() const
{
	return m_sizer.ideal_fit();
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::UILayer::create_render_layer(Indigo::Render::Window* window)
{
	glm::uvec2 sz(window->window()->size());
	if (sz != size())
		Indigo::logic_pipe()->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&Indigo::UILayer::on_layout),sz);

	OOBase::SharedPtr<Indigo::Render::UILayer> group = OOBase::allocate_shared<Indigo::Render::UILayer,OOBase::ThreadLocalAllocator>(window,this,visible(),position(),size());
	if (!group)
		LOG_ERROR(("Failed to allocate group: %s",OOBase::system_error_text()));
	else if (!on_render_create(group.get()))
		group.reset();

	m_render_parent = group.get();

	return OOBase::static_pointer_cast<Indigo::Render::Layer>(group);
}

/*

bool Indigo::UILayer::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	if (!visible())
		return false;

	bool ret = UIGroup::on_mousebutton(click);
	return m_modal || ret;
}
*/
