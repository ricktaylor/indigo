///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#include "Camera.h"
#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

Indigo::Camera::Camera(const OOBase::SharedPtr<Viewport>& viewport, const glm::mat4& camera_matrix, const glm::mat4& projection_matrix) :
		m_viewport(viewport),
		m_camera_matrix(camera_matrix),
		m_projection_matrix(projection_matrix),
		m_dirty(true)
{
}

Indigo::Camera::Camera(const Camera& rhs) :
		m_viewport(rhs.m_viewport),
		m_scene(rhs.m_scene),
		m_camera_matrix(rhs.m_camera_matrix),
		m_projection_matrix(rhs.m_projection_matrix),
		m_dirty(true)
{
}

Indigo::Camera::~Camera()
{
}

Indigo::Camera& Indigo::Camera::operator = (const Camera& rhs)
{
	Camera(rhs).swap(*this);
	return *this;
}

void Indigo::Camera::swap(Camera& rhs)
{
	OOBase::swap(m_viewport,rhs.m_viewport);
	OOBase::swap(m_scene,rhs.m_scene);
	OOBase::swap(m_camera_matrix,rhs.m_camera_matrix);
	OOBase::swap(m_projection_matrix,rhs.m_projection_matrix);

	m_dirty = rhs.m_dirty = true;
}

OOBase::SharedPtr<Indigo::Viewport> Indigo::Camera::viewport() const
{
	return m_viewport.lock();
}

const OOBase::SharedPtr<Indigo::Scene>& Indigo::Camera::scene() const
{
	return m_scene;
}

void Indigo::Camera::scene(const OOBase::SharedPtr<Scene>& scene)
{
	m_scene = scene;
}

void Indigo::Camera::camera_matrix(const glm::mat4& camera_matrix)
{
	m_camera_matrix = camera_matrix;
	m_dirty = true;
}

const glm::mat4& Indigo::Camera::camera_matrix() const
{
	return m_camera_matrix;
}

void Indigo::Camera::look_at(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
{
	m_camera_matrix = glm::lookAt(eye,target,up);
	m_dirty = true;
}

void Indigo::Camera::projection_matrix(const glm::mat4& projection_matrix)
{
	m_projection_matrix = projection_matrix;
	m_dirty = true;
}

const glm::mat4& Indigo::Camera::projection_matrix() const
{
	return m_projection_matrix;
}

void Indigo::Camera::frustum(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top, const float_t& zNear, const float_t& zFar)
{
	m_projection_matrix = glm::frustum(left,right,bottom,top,zNear,zFar);
	m_dirty = true;
}

void Indigo::Camera::ortho(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top)
{
	m_projection_matrix = glm::ortho(left,right,bottom,top);
	m_dirty = true;
}

void Indigo::Camera::ortho(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top, const float_t& zNear, const float_t& zFar)
{
	m_projection_matrix = glm::ortho(left,right,bottom,top,zNear,zFar);
	m_dirty = true;
}

void Indigo::Camera::perspective(const float_t& fovy, const float_t& aspect, const float_t& zNear, const float_t& zFar)
{
	m_projection_matrix = glm::perspective(fovy,aspect,zNear,zFar);
	m_dirty = true;
}

void Indigo::Camera::draw(State& gl_state)
{
	// Draw
	if (m_scene)
	{
		glm::mat4 view_matrix = m_projection_matrix * m_camera_matrix;

		m_scene->draw(gl_state,view_matrix);
	}
}

void Indigo::Camera::refresh()
{
	glm::mat4 view_matrix = m_projection_matrix * m_camera_matrix;

	// Create non-normalized clipping planes
	glm::vec4 col0(glm::column(view_matrix,0));
	glm::vec4 col1(glm::column(view_matrix,1));
	glm::vec4 col2(glm::column(view_matrix,2));
	glm::vec4 col3(glm::column(view_matrix,3));

	m_clip_planes[0] = col3 + col0; // left
	m_clip_planes[1] = col3 - col0; // right
	m_clip_planes[2] = col3 - col1; // top
	m_clip_planes[3] = col3 + col1; // bottom
	m_clip_planes[4] = col3 + col2; // near
	m_clip_planes[5] = col3 - col2; // far

	m_clip_abs_planes[0] = glm::abs(m_clip_planes[0]);
	m_clip_abs_planes[1] = glm::abs(m_clip_planes[1]);
	m_clip_abs_planes[2] = glm::abs(m_clip_planes[2]);
	m_clip_abs_planes[3] = glm::abs(m_clip_planes[3]);
	m_clip_abs_planes[4] = glm::abs(m_clip_planes[4]);
	m_clip_abs_planes[5] = glm::abs(m_clip_planes[5]);

	m_dirty = false;
}

enum Indigo::Camera::CullType Indigo::Camera::frustum_cull(const glm::vec3& centre, const glm::vec3& extent) const
{
	unsigned int hint = 0;
	return frustum_cull(centre,extent,hint);
}

enum Indigo::Camera::CullType Indigo::Camera::frustum_cull(const glm::vec3& centre, const glm::vec3& extent, unsigned int& hint) const
{
	// See http://fgiesen.wordpress.com/2010/10/17/view-frustum-culling/
	enum CullType cull = outside;

	if (m_dirty)
		const_cast<Camera*>(this)->refresh();

	glm::vec4 centre4(centre,1.0f);
	glm::vec4 extent4(extent,1.0f);

	unsigned int i = hint;
	do
	{
		glm::vec3::value_type d = glm::dot(centre4,m_clip_planes[i]);
		glm::vec3::value_type r = glm::dot(extent4,m_clip_abs_planes[i]);

		if (d + r > 0) // partially inside
			cull = intersect;
		else if (d - r >= 0) // fully inside
		{
			if (cull != intersect)
				cull = inside;
		}
		else
		{
			hint = i;
			return outside;
		}

		i = (i + 1) % 6;
	}
	while (i != hint);

	return cull;
}

