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

#ifndef INDIGO_UILAYER_H_INCLUDED
#define INDIGO_UILAYER_H_INCLUDED

#include "UISizer.h"

#include "../core/Window.h"

namespace Indigo
{
	class UILayer : public UIGroup, public Layer
	{
	public:
		UILayer(bool fixed, const glm::uvec4& margins = glm::uvec4(0), const glm::uvec2& padding = glm::uvec2(0));

		void show(bool visible = true);

		UIGridSizer& sizer() { return m_sizer; }

		virtual void layout();

	protected:
		OOBase::SharedPtr<Render::Layer> create_render_layer(Indigo::Render::Window* window);

		virtual bool on_render_create(Indigo::Render::UIGroup* group) { return true; }
		virtual void on_size(const glm::uvec2& sz);

		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

	private:
		UIGridSizer m_sizer;
	};
}

#endif // INDIGO_UILAYER_H_INCLUDED
