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
		m_owner(owner),
		m_dirty(true)
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

bool Indigo::Render::UILayer::on_update()
{
	if (!visible())
		return false;

	bool dirty = m_dirty;
	m_dirty = false;
	return dirty;
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
	glm::ivec2 ipos = glm::clamp(glm::ivec2(glm::floor(pos)),glm::ivec2(0),glm::ivec2(sz.x-1,sz.y-1));

	OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator> hits;
	hit_test(hits,ipos);

	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator i=m_cursor_hits.begin();i;)
	{
		if (hits.find(*i))
			++i;
		else
		{
			OOBase::SharedPtr<UIDrawable> d = i->lock();

			i = m_cursor_hits.erase(i);

			if (d && d->on_cursorenter(false))
				break;
		}
	}

	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator i=hits.begin();i;++i)
	{
		if (m_cursor_hits.remove(*i) == 0)
		{
			OOBase::SharedPtr<UIDrawable> d = i->lock();
			if (d && d->on_cursorenter(true))
				break;
		}
	}

	m_cursor_hits.swap(hits);

	bool handled = false;
	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator i=m_cursor_hits.begin();!handled && i;)
	{
		OOBase::SharedPtr<UIDrawable> d = i->lock();
		if (!d)
			m_cursor_hits.erase(i);
		else
		{
			handled = d->on_cursormove();
			++i;
		}
	}

	return m_owner->m_modal || handled;
}

void Indigo::Render::UILayer::on_losecursor()
{
	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator i=m_cursor_hits.begin();i;++i)
	{
		OOBase::SharedPtr<UIDrawable> d = i->lock();
		if (d)
			d->on_cursorenter(false);
	}
	m_cursor_hits.clear();
}

void Indigo::Render::UILayer::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	bool handled = false;
	for (OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>::iterator i=m_cursor_hits.begin();!handled && i;++i)
	{
		OOBase::SharedPtr<UIDrawable> d = i->lock();
		if (d)
			handled = d->on_mousebutton(click);
	}
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
	if (visible() && m_on_close)
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

void Indigo::UILayer::make_dirty()
{
	if (m_render_parent)
		render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(static_cast<Render::UILayer*>(m_render_parent),&Render::UILayer::make_dirty));
}
