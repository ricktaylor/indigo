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

#ifndef INDIGO_UIPANEL_H_INCLUDED
#define INDIGO_UIPANEL_H_INCLUDED

#include "UINinePatch.h"
#include "UISizer.h"

namespace Indigo
{
	class UIPanel : public UIGroup
	{
	public:
		UIPanel(UIGroup* parent, const OOBase::SharedPtr<NinePatch>& background, const glm::vec4& colour = glm::vec4(1.f), bool fixed = false, const glm::uvec2& padding = glm::uvec2(0), OOBase::uint32_t state = 0, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0));

		UIGridSizer& sizer() { return m_sizer; }

		virtual void layout();

	protected:
		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Indigo::Render::UIGroup* group);
		virtual void on_size(const glm::uvec2& sz);

	private:
		UIGridSizer m_sizer;
		OOBase::SharedPtr<NinePatch> m_background;
		glm::vec4 m_colour;

		OOBase::SharedPtr<Render::UIDrawable> m_render_background;
	};
}

#endif /* INDIGO_UIPANEL_H_INCLUDED */
