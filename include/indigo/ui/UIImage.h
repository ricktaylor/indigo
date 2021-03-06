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

#include "UIWidget.h"

#include "../Quad.h"
#include "../Image.h"

namespace Indigo
{
	namespace Render
	{
		class UIImage : public UIDrawable
		{
		public:
			UIImage(const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::vec4& colour = glm::vec4(1.f), bool visible = true, const glm::ivec2& position = glm::ivec2(), const glm::uvec2& size = glm::uvec2());

		protected:
			Quad m_quad;
			OOBase::SharedPtr<OOGL::Texture> m_texture;
			glm::vec4 m_colour;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}

	class UIImage : public UIWidget
	{
	public:
		struct CreateParams : public UIWidget::CreateParams
		{
			CreateParams(OOBase::uint32_t state = eWS_visible,
					const glm::ivec2& position = glm::ivec2(),
					const glm::uvec2& size = glm::uvec2(),
					const glm::vec4& colour = glm::vec4(1.f)
			) :
				UIWidget::CreateParams(state,position,size),
				m_colour(colour)
			{}

			glm::vec4                m_colour;
		};

		UIImage(UIGroup* parent, const OOBase::SharedPtr<Image>& image, const CreateParams& params = CreateParams());

	protected:
		virtual glm::uvec2 min_size() const { return glm::uvec2(); }
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Render::UIGroup* group);

	private:
		OOBase::SharedPtr<Image> m_image;
		Render::UIImage* m_render_image;
		glm::vec4 m_colour;

		void on_size(glm::uvec2& sz);
	};
}

#endif // INDIGO_FONT_H_INCLUDED
