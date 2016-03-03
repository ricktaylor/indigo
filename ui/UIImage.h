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

namespace Indigo
{
	namespace Render
	{
		class UIImage;
	}

	class UIImage : public Image
	{
		friend class Render::UIImage;

	public:
		UIImage();
		virtual ~UIImage();

		virtual void unload();

		OOBase::SharedPtr<Render::UIImage> make_drawable(const glm::ivec2& position = glm::ivec2(0), const glm::u16vec2& size = glm::u16vec2(0), bool visible = true, const glm::vec4& colour = glm::vec4(1.f));

	private:
		OOBase::SharedPtr<OOGL::Texture> m_texture;

		void do_unload();
	};

	namespace Render
	{
		class UIImage : public UIDrawable
		{
			friend class Indigo::UIImage;

		public:
			UIImage(const glm::ivec2& position, const glm::u16vec2& size, bool visible, const glm::vec4& colour, const OOBase::SharedPtr<OOGL::Texture>& texture);
			virtual ~UIImage();

			bool valid() const;

			void size(const glm::u16vec2& size) { m_size = glm::vec3(size.x,size.y,1.f); }

			void colour(const glm::vec4& colour) { m_colour = colour; }

		private:
			glm::vec4  m_colour;
			glm::vec3  m_size;
			GLsizei    m_quad;

			OOBase::SharedPtr<OOGL::Texture> m_texture;

			void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}
}

#endif // INDIGO_FONT_H_INCLUDED
