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

#include "GUISizer.h"
#include "GUIPanel.h"
#include "Render.h"

Indigo::Render::GUI::Sizer::Sizer()
{

}

Indigo::Render::GUI::Sizer::~Sizer()
{

}

Indigo::GUI::Sizer::Sizer()
{

}

Indigo::GUI::Sizer::~Sizer()
{
	destroy();
}

bool Indigo::GUI::Sizer::create(OOBase::uint16_t hspace, OOBase::uint16_t vspace)
{
	ItemLayout def_layout;
	def_layout.m_flags = ItemLayout::expand | ItemLayout::align_left | ItemLayout::align_top;
	def_layout.m_proportion = 0;
	return create(hspace,vspace,def_layout);
}

bool Indigo::GUI::Sizer::create(OOBase::uint16_t hspace, OOBase::uint16_t vspace, const ItemLayout& layout)
{
	const glm::u16vec2 space(hspace,vspace);
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Sizer::do_create),&ret,&space) || !ret)
		return false;

	m_default_layout = layout;
	return true;
}

void Indigo::GUI::Sizer::do_create(bool* ret_val, const glm::u16vec2* space)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Sizer> sizer = create_render_sizer();
	if (!sizer)
	{
		*ret_val = false;
		return;
	}

	sizer->m_padding = *space;

	sizer.swap(m_sizer);
	*ret_val = true;
}

bool Indigo::GUI::Sizer::destroy()
{
	return !m_sizer || render_call(OOBase::make_delegate(this,&Sizer::do_destroy));
}

void Indigo::GUI::Sizer::do_destroy()
{
	m_sizer.reset();
}

bool Indigo::GUI::Sizer::fit(Panel& panel)
{
	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_fit),&ret,static_cast<Widget*>(&panel)) && ret;
}

void Indigo::GUI::Sizer::do_fit(bool* ret_val, Widget* panel)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> render_panel(panel->render_widget<Indigo::Render::GUI::Panel>());
	if (!render_panel)
		*ret_val = false;
	else
		*ret_val = m_sizer->fit(*render_panel.get());
}

bool Indigo::GUI::Sizer::layout(const Panel& panel)
{
	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_layout),&ret,static_cast<const Widget*>(&panel)) && ret;
}

void Indigo::GUI::Sizer::do_layout(bool* ret_val, const Widget* panel)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> render_panel(panel->render_widget<Indigo::Render::GUI::Panel>());
	if (!render_panel)
		*ret_val = false;
	else
		*ret_val = m_sizer->layout(*render_panel.get());
}
