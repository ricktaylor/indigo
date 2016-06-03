///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Rick Taylor
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
#include "../../include/indigo/sg/SGCamera.h"

#include "../../include/indigo/Render.h"
#include "../../include/indigo/Window.h"

void Indigo::Render::SGCamera::on_draw(OOGL::State& glState) const
{

}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::SGCamera::create_render_layer(Render::Window* window)
{
	m_size = window->window()->size();

	OOBase::SharedPtr<Render::SGCamera> render_cam = OOBase::allocate_shared<Render::SGCamera,OOBase::ThreadLocalAllocator>(window);
	if (!render_cam)
		LOG_ERROR(("Failed to allocate render camera: %s",OOBase::system_error_text()));
	else
		m_render_camera = render_cam;

	return render_cam;
}

void Indigo::SGCamera::destroy_render_layer()
{
	m_render_camera.reset();

	Layer::destroy_render_layer();
}

glm::mat4 Indigo::SGCamera::view_proj() const
{
	if (m_ortho)
		return glm::ortho(0.f,m_size.x,0.f,m_size.y,m_near,m_far) * glm::lookAt(m_position,m_target,m_up);

	return glm::perspectiveFov(m_fov,m_size.x,m_size.y,m_near,m_far) * glm::lookAt(m_position,m_target,m_up);
}

void Indigo::SGCamera::position(const glm::vec3& pos)
{
	if (m_position != pos)
	{
		m_position = pos;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::target(const glm::vec3& t)
{
	if (m_target != t)
	{
		m_target = t;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::up(const glm::vec3& u)
{
	if (m_up != u)
	{
		m_up = u;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::ortho(bool ortho)
{
	if (m_ortho != ortho)
	{
		m_ortho = ortho;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::near(glm::mat4::value_type n)
{
	if (m_near != n)
	{
		m_near = n;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::far(glm::mat4::value_type f)
{
	if (m_far != f)
	{
		m_far = f;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::fov(glm::mat4::value_type f)
{
	if (m_fov != f)
	{
		m_fov = f;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::on_size(const glm::uvec2& sz)
{
	if (m_size.x != sz.x || m_size.y != sz.y)
	{
		m_size = sz;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}
