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

#include "../../include/indigo/Render.h"
#include "../../include/indigo/Quad.h"

#include "../../include/indigo/ui/UIImage.h"

#include "../Common.h"

Indigo::Render::UIImage::UIImage(const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::vec4& colour, bool visible, const glm::ivec2& position, const glm::uvec2& size) :
		UIDrawable(visible,position,size),
		m_texture(texture),
		m_colour(colour)
{
}

void Indigo::Render::UIImage::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	if (m_texture && m_colour.a > 0.f)
	{
		const glm::uvec2& sz = size();
		m_quad.draw(glState,m_texture,glm::scale(mvp,glm::vec3(sz.x,sz.y,0.f)),m_colour);
	}
}

Indigo::UIImage::UIImage(UIGroup* parent, const OOBase::SharedPtr<Image>& image, const CreateParams& params) :
		UIWidget(parent,params),
		m_image(image),
		m_render_image(NULL),
		m_colour(params.m_colour)
{
	if (m_image && !m_image->valid())
		LOG_ERROR(("Invalid image passed to UIImage constructor"));

	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
}

glm::uvec2 Indigo::UIImage::ideal_size() const
{
	return m_image ? m_image->size() : glm::uvec2();
}

void Indigo::UIImage::on_size(glm::uvec2& sz)
{
	if (m_render_image)
		render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(static_cast<Render::UIDrawable*>(m_render_image),&Render::UIDrawable::size),sz);
}

bool Indigo::UIImage::on_render_create(Indigo::Render::UIGroup* group)
{
	
	OOBase::SharedPtr<OOGL::Texture> texture;
	if (m_image)
	{
		bool cached = true;
		texture = m_image->make_texture(GL_RGBA8,cached);
		if (!texture)
			return false;

		if (!cached)
		{
			texture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			texture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			texture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			texture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		}
	}

	OOBase::SharedPtr<Render::UIImage> render_image = OOBase::allocate_shared<Render::UIImage,OOBase::ThreadLocalAllocator>(texture,m_colour,true,glm::ivec2(),size());
	if (!render_image)
		LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);

	if (!group->add_drawable(render_image))
		return false;

	m_render_image = render_image.get();
	return true;
}
