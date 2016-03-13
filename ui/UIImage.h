///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// OOGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OOGL.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef INDIGO_UIImage_H_INCLUDED
#define INDIGO_UIImage_H_INCLUDED

#include "../core/Image.h"
#include "UILayer.h"
#include "Quad.h"

namespace Indigo
{
	namespace Render
	{
		class UIImage : public UIDrawable
		{
		public:
			UIImage(const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::uvec2& size, const glm::vec4& colour = glm::vec4(1.f), const glm::ivec2& position = glm::ivec2(0));

			void colour(const glm::vec4& colour) { m_colour = colour; }
			void size(const glm::uvec2& size) { m_size = glm::vec3(size.x,size.y,1.f); }

		protected:
			OOBase::SharedPtr<OOGL::Texture> m_texture;
			glm::vec4 m_colour;
			glm::vec3 m_size;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}

	class UIImage : public UIWidget
	{
	public:
		UIImage(UIWidget* parent, const OOBase::SharedPtr<Image>& image, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0), const glm::vec4& colour = glm::vec4(1.f));

	protected:
		virtual glm::uvec2 min_size() const { return glm::uvec2(0); }
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Indigo::Render::UIGroup* group);

	private:
		OOBase::SharedPtr<Image> m_image;
		OOBase::SharedPtr<Render::UIImage> m_render_image;

		glm::vec4 m_colour;
	};
}

#endif // INDIGO_FONT_H_INCLUDED
