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
#include "../core/Render.h"

#include "UIPanel.h"

Indigo::UIPanel::UIPanel(UIGroup* parent, const CreateParams& params) :
		UIGroup(parent,params),
		m_sizer(params.m_fixed,params.m_background ? params.m_background->margins() : glm::uvec4(0),params.m_padding),
		m_background(params.m_background),
		m_colour(params.m_colour)
{
}

bool Indigo::UIPanel::on_render_create(Indigo::Render::UIGroup* group)
{
	unsigned int zorder = 0;
	if (m_background)
	{
		m_render_background = m_background->make_drawable(true,glm::ivec2(0),size(),m_colour);
		if (!m_render_background)
			return false;

		if (!group->add_drawable(m_render_background,zorder++))
			return false;
	}

	OOBase::SharedPtr<Indigo::Render::UIGroup> subgroup = OOBase::allocate_shared<Indigo::Render::UIGroup,OOBase::ThreadLocalAllocator>() ;
	if (!subgroup)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	if (!group->add_drawable(subgroup,zorder++))
		return false;

	m_render_parent.swap(subgroup);
	return true;
}

glm::uvec2 Indigo::UIPanel::min_size() const
{
	return m_sizer.min_fit();
}
glm::uvec2 Indigo::UIPanel::ideal_size() const
{
	return m_sizer.ideal_fit();
}

void Indigo::UIPanel::on_size(const glm::uvec2& sz)
{
	m_sizer.fit(sz);

	if (m_render_background)
		render_pipe()->post(OOBase::make_delegate(m_render_background.get(),&Render::UIDrawable::size),sz);
}

bool Indigo::UIPanel::on_mousemove(const glm::ivec2& pos)
{
	UIGroup::on_mousemove(pos);

	// Always eat the message
	return true;
}
