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

#ifndef INDIGO_IMAGELAYER_H_INCLUDED
#define INDIGO_IMAGELAYER_H_INCLUDED

#include "Window.h"

namespace Indigo
{
	class Image;

	class ImageLayer : public Layer
	{
	public:
		ImageLayer(const OOBase::SharedPtr<Image>& image, const glm::vec4& colour = glm::vec4(1.f));

		glm::vec4 colour(const glm::vec4& colour);

	private:
		OOBase::SharedPtr<Image> m_image;
		glm::vec4 m_colour;

		OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window);
	};
}

#endif // INDIGO_IMAGELAYER_H_INCLUDED
