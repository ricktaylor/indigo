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

Indigo::UIPanel::UIPanel(UIGroup* parent, const OOBase::SharedPtr<NinePatch>& background, const glm::vec4& colour, bool fixed, const glm::uvec2& padding, const glm::ivec2& position, const glm::uvec2& size) :
		UIGroup(parent,position,size),
		m_sizer(fixed,background ? background->margins() : glm::uvec4(0),padding),
		m_background(background),
		m_colour(colour)
{
}

bool Indigo::UIPanel::on_render_create(Indigo::Render::UIGroup* group)
{
	if (m_background)
	{
		m_render_background = m_background->make_drawable(glm::ivec2(0),size(),m_colour);
		if (!m_render_background)
			return false;

		if (!group->add_drawable(m_render_background,0))
			return false;

		m_render_background->show();
	}

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

void Indigo::UIPanel::layout()
{
	this->size(m_sizer.ideal_fit());
}

void Indigo::UIPanel::do_size(glm::uvec2 sz)
{
	if (m_render_background)
		m_render_background->size(sz);
}

void Indigo::UIPanel::on_size(const glm::uvec2& sz)
{
	m_sizer.fit(sz);

	if (valid() && m_render_background)
		render_pipe()->post(OOBase::make_delegate(this,&UIPanel::do_size),sz);
}
