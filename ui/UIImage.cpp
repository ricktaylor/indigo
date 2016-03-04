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
#include "UIImage.h"

#include "Quad.h"

Indigo::Render::UIImage::UIImage(const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::uvec2& size, const glm::vec4& colour, const glm::ivec2& position) :
		UIDrawable(position),
		m_texture(texture),
		m_colour(colour),
		m_size(size.x,size.y,1.f)
{
}

void Indigo::Render::UIImage::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_texture)
	{
		glState.bind(0,m_texture);

		Quad::draw(glState,glm::scale(mvp,m_size),m_colour);
	}
}

Indigo::UIImage::UIImage(const OOBase::SharedPtr<Image>& image, const glm::ivec2& position, const glm::uvec2& size, const glm::vec4& colour) :
		UIWidget(position,size),
		m_image(image),
		m_colour(colour)
{
	if (size == glm::uvec2(0))
		this->size(m_image->size());
}

glm::uvec2 Indigo::UIImage::ideal_size() const
{
	return m_image->size();
}

bool Indigo::UIImage::on_render_create(Indigo::Render::UIGroup* group)
{
	OOBase::SharedPtr<OOGL::Texture> texture = m_image->make_texture(GL_RGBA8);
	if (!texture)
		return false;

	m_render_image = OOBase::allocate_shared<Render::UIImage,OOBase::ThreadLocalAllocator>(texture,size(),m_colour);
	if (!m_render_image)
		LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);

	if (!group->add_drawable(m_render_image,0))
		return false;

	m_render_image->visible(true);

	return true;
}
