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
	class Layer : public Indigo::Render::GUI::Widget, public Indigo::Render::Layer
	{
	public:
		bool create(const OOBase::SharedPtr<Indigo::Render::MainWindow>& wnd);

	private:
		void on_draw(const OOGL::Window& win, OOGL::State& glState) {}
		void on_size(const OOGL::Window& win, const glm::ivec2& sz) {}
	};
}

bool Layer::create(const OOBase::SharedPtr<Indigo::Render::MainWindow>& wnd)
{
	return wnd->add_layer(OOBase::static_pointer_cast<Layer>(shared_from_this()));
}

OOBase::SharedPtr<Indigo::Render::GUI::Widget> Indigo::GUI::Layer::create_widget()
{
	// Move this to a derived class
	OOBase::SharedPtr<::Layer> layer = OOBase::allocate_shared<::Layer,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR(("Failed to allocate layer: %s",OOBase::system_error_text()));

	return layer;
}

bool Indigo::GUI::Layer::create(OOBase::SharedPtr<Render::MainWindow>& wnd)
{
	if (!Widget::create(OOBase::SharedPtr<Widget>()))
		return false;

	if (!render_call(OOBase::make_delegate(this,&Layer::do_create),&wnd))
	{
		destroy();
		return false;
	}

	return true;
}

bool Indigo::GUI::Layer::do_create(OOBase::SharedPtr<Render::MainWindow>* wnd)
{
	OOBase::SharedPtr<::Layer> layer(widget<::Layer>());
	if (!layer)
		return false;

	return layer->create(*wnd);
}
