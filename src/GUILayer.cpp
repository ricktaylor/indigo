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

#include "MainWindow.h"
#include "Render.h"

namespace 
{
	class Layer : public Indigo::Render::GUI::Panel, public Indigo::Render::Layer
	{
	public:
		bool create(const OOBase::SharedPtr<Indigo::Render::MainWindow>& wnd);

	private:
		OOBase::SharedPtr<Indigo::Render::MainWindow> m_wnd;

		glm::u16vec2 ideal_size() const;
		const OOBase::SharedPtr<OOGL::Window>& window() const;

		void on_draw(const OOGL::Window& win, OOGL::State& glState);
		void on_size(const OOGL::Window& win, const glm::u16vec2& sz);
	};
}

bool Layer::create(const OOBase::SharedPtr<Indigo::Render::MainWindow>& wnd)
{
	if (!wnd->add_layer(OOBase::static_pointer_cast<Layer>(shared_from_this())))
		return false;

	m_wnd = wnd;
	size(wnd->size());

	return true;
}

const OOBase::SharedPtr<OOGL::Window>& Layer::window() const
{
	return m_wnd->window();
}

glm::u16vec2 Layer::ideal_size() const
{
	return Widget::size();
}

void Layer::on_draw(const OOGL::Window& win, OOGL::State& glState)
{
	if (shown())
	{
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glm::vec2 sz = win.size();
		glm::mat4 proj = glm::ortho(0.f,sz.x,0.f,sz.y);

		draw(glState,proj);
	}
}

void Layer::on_size(const OOGL::Window& win, const glm::u16vec2& sz)
{
	size(sz);
	layout();
}

OOBase::SharedPtr<Indigo::Render::GUI::Widget> Indigo::GUI::Layer::create_widget()
{
	OOBase::SharedPtr< ::Layer> layer = OOBase::allocate_shared< ::Layer,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR(("Failed to allocate layer: %s",OOBase::system_error_text()));

	return layer;
}

bool Indigo::GUI::Layer::create(OOBase::SharedPtr<Render::MainWindow>& wnd)
{
	if (!Panel::create(NULL))
		return false;

	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Layer::do_create),&ret,&wnd) || !ret)
	{
		destroy();
		return false;
	}

	return true;
}

void Indigo::GUI::Layer::do_create(bool* ret_val, OOBase::SharedPtr<Render::MainWindow>* wnd)
{
	*ret_val = render_widget< ::Layer>()->create(*wnd);
}
