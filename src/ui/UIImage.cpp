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

#include "../Common.h"
#include "../../include/indigo/Render.h"
#include "../../include/indigo/Quad.h"

#include "../../include/indigo/ui/UIImage.h"

Indigo::Render::UIImage::UIImage(const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::uvec2& size, const glm::vec4& colour, bool visible, const glm::ivec2& position) :
		UIDrawable(visible,position),
		m_texture(texture),
		m_colour(colour),
		m_size(size.x,size.y,1.f)
{
}

void Indigo::Render::UIImage::on_draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	glm::mat4 mvp2 = glm::scale(mvp,m_size);
	Quad::draw(glState,m_texture,mvp2,m_colour);
}

Indigo::UIImage::UIImage(UIGroup* parent, const OOBase::SharedPtr<Image>& image, const CreateParams& params) :
		UIWidget(parent,params),
		m_image(image),
		m_render_image(NULL),
		m_colour(params.m_colour)
{
	if (!m_image || !m_image->valid())
		LOG_ERROR(("Invalid image passed to UINinePatch constructor"));

	if (params.m_size == glm::uvec2(0))
		this->size(ideal_size());
}

glm::uvec2 Indigo::UIImage::ideal_size() const
{
	return m_image->size();
}

void Indigo::UIImage::on_size(const glm::uvec2& sz)
{
	if (m_render_image)
		render_pipe()->post(OOBase::make_delegate(m_render_image,&Render::UIImage::size),sz);
}

bool Indigo::UIImage::on_render_create(Indigo::Render::UIGroup* group)
{
	bool cached = true;
	OOBase::SharedPtr<OOGL::Texture> texture = m_image->make_texture(GL_RGBA8,cached);
	if (!texture)
		return false;

	if (!cached)
	{
		texture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		texture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		texture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		texture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}

	OOBase::SharedPtr<Render::UIImage> render_image = OOBase::allocate_shared<Render::UIImage,OOBase::ThreadLocalAllocator>(texture,size(),m_colour,true);
	if (!render_image)
		LOG_ERROR_RETURN(("Failed to allocate button caption: %s",OOBase::system_error_text()),false);

	if (!group->add_drawable(render_image))
		return false;

	m_render_image = render_image.get();
	return true;
}
