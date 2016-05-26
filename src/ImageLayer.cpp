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

#include "Common.h"
#include "Image.h"
#include "ImageLayer.h"
#include "Quad.h"

namespace
{
	class ImageLayer : public Indigo::Render::Layer
	{
	public:
		ImageLayer(const OOBase::SharedPtr<OOGL::Texture>& texture, Indigo::Render::Window* window, const glm::vec4& colour);

		void on_draw(OOGL::State& glState) const;

		void colour(glm::vec4 colour) { m_colour = colour; }

		OOBase::SharedPtr<OOGL::Texture> m_texture;
		glm::vec4 m_colour;

		glm::mat4 m_mvp;
	};
}

::ImageLayer::ImageLayer(const OOBase::SharedPtr<OOGL::Texture>& texture, Indigo::Render::Window* window, const glm::vec4& colour) :
		Indigo::Render::Layer(window),
		m_texture(texture),
		m_colour(colour),
		m_mvp(2.f,0.f,0.f,0.f,0.f,2.f,0.f,0.f,0.f,0.f,-1.f,0.f,-1.f,-1.f,0.f,1.f)
{
}

void ::ImageLayer::on_draw(OOGL::State& glState) const
{
	if (m_texture)
	{
		glState.enable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		Indigo::Render::Quad::draw(glState,m_texture,m_mvp,m_colour);
	}
}

Indigo::ImageLayer::ImageLayer(const OOBase::SharedPtr<Image>& image, const glm::vec4& colour) :
		m_image(image),
		m_colour(colour)
{
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::ImageLayer::create_render_layer(Render::Window* window)
{
	OOBase::SharedPtr< ::ImageLayer> layer;
	bool cached = true;
	OOBase::SharedPtr<OOGL::Texture> texture = m_image->make_texture(GL_RGBA8,cached,1);
	if (!texture)
		return layer;

	if (!cached)
	{
		texture->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		texture->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		texture->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		texture->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}
	
	layer = OOBase::allocate_shared< ::ImageLayer,OOBase::ThreadLocalAllocator>(texture,window,m_colour);
	if (!layer)
		LOG_ERROR(("Failed to allocate layer: %s",OOBase::system_error_text()));

	return layer;
}

glm::vec4 Indigo::ImageLayer::colour(const glm::vec4& colour)
{
	glm::vec4 prev_colour = m_colour;
	if (colour != prev_colour)
	{
		OOBase::SharedPtr< ::ImageLayer> layer = OOBase::static_pointer_cast< ::ImageLayer>(render_layer());
		if (layer)
			render_pipe()->post(OOBase::make_delegate(layer.get(),&::ImageLayer::colour),colour);
	}

	return prev_colour;
}
