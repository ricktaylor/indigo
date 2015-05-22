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

#include "GUIMenu.h"

namespace
{
	class GUIMenu : public Indigo::Render::GUIWidget
	{
	public:
		GUIMenu(const OOBase::SharedPtr<GUIWidget>& parent, const Indigo::GUIMenu::CreateParams* params);

		static OOBase::SharedPtr<Indigo::Render::GUIWidget> create(const OOBase::SharedPtr<Indigo::Render::GUIWidget>& parent, const Indigo::GUIWidget::CreateParams* params);

	private:
		//void on_draw(OOGL::State& glState);
	};
}

GUIMenu::GUIMenu(const OOBase::SharedPtr<GUIWidget>& parent, const Indigo::GUIMenu::CreateParams* params) : Indigo::Render::GUIWidget(parent,params)
{

}

OOBase::SharedPtr<Indigo::Render::GUIWidget> GUIMenu::create(const OOBase::SharedPtr<Indigo::Render::GUIWidget>& parent, const Indigo::GUIWidget::CreateParams* p)
{
	const Indigo::GUIMenu::CreateParams* params = static_cast<const Indigo::GUIMenu::CreateParams*>(p);

	OOBase::SharedPtr<GUIMenu> menu = OOBase::allocate_shared<GUIMenu,OOBase::ThreadLocalAllocator>(parent,params);
	if (!menu)
		LOG_ERROR_RETURN(("Failed to allocate GUIMenu: %s",OOBase::system_error_text()),menu);

	return menu;
}

OOBase::uint32_t Indigo::GUIMenu::create(GUILayer& layer, const Indigo::GUIMenu::CreateParams& params)
{
	return layer.create_widget(OOBase::make_delegate(&::GUIMenu::create),&params);
}
