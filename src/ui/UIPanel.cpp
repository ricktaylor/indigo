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

#include "../../include/indigo/ui/UIPanel.h"

#include "../../include/indigo/Render.h"

#include "../Common.h"

Indigo::UIPanel::UIPanel(UIGroup* parent, const CreateParams& params) :
		UIGroup(parent,params),
		m_sizer(params.m_fixed,glm::max(params.m_background ? params.m_background->margins() : glm::uvec4(0),params.m_margins),params.m_padding),
		m_background(params.m_background),
		m_colour(params.m_colour),
		m_render_background(NULL)
{
	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
}

bool Indigo::UIPanel::on_render_create(Indigo::Render::UIGroup* group)
{
	if (m_background)
	{
		OOBase::SharedPtr<Render::UIDrawable> bk = m_background->make_drawable(m_colour,true,glm::ivec2(),size());
		if (!bk)
			return false;

		if (!group->add_drawable(bk))
			return false;

		m_render_background = bk.get();
	}

	OOBase::SharedPtr<Indigo::Render::UIGroup> subgroup = OOBase::allocate_shared<Indigo::Render::UIGroup,OOBase::ThreadLocalAllocator>(true,glm::ivec2(),size()) ;
	if (!subgroup)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	if (!group->add_drawable(subgroup))
		return false;

	m_render_parent = subgroup.get();

	return UIGroup::on_render_create(group);
}

glm::uvec2 Indigo::UIPanel::min_size() const
{
	return m_sizer.min_fit();
}

glm::uvec2 Indigo::UIPanel::ideal_size() const
{
	return m_sizer.ideal_fit();
}

void Indigo::UIPanel::on_size(glm::uvec2& sz)
{
	m_sizer.fit(sz);

	if (m_render_background)
		render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_background,&Render::UIDrawable::size),sz);
}
