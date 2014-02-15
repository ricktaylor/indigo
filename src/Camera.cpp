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
		m_projection_matrix(projection_matrix)
{
}

Indigo::Camera::Camera(const Camera& rhs) :
		m_viewport(rhs.m_viewport),
		m_scene(rhs.m_scene),
		m_camera_matrix(rhs.m_camera_matrix),
		m_projection_matrix(rhs.m_projection_matrix)
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
	if (scene != m_scene)
	{
		m_scene = scene;

		cull();
	}
}

void Indigo::Camera::camera_matrix(const glm::mat4& camera_matrix)
{
	m_camera_matrix = camera_matrix;
}

const glm::mat4& Indigo::Camera::camera_matrix() const
{
	return m_camera_matrix;
}

void Indigo::Camera::look_at(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
	m_camera_matrix = glm::lookAt(eye,center,up);
}

void Indigo::Camera::projection_matrix(const glm::mat4& projection_matrix)
{
	m_projection_matrix = projection_matrix;
}

const glm::mat4& Indigo::Camera::projection_matrix() const
{
	return m_projection_matrix;
}

void Indigo::Camera::frustum(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top, const float_t& zNear, const float_t& zFar)
{
	m_projection_matrix = glm::frustum(left,right,bottom,top,zNear,zFar);
}

void Indigo::Camera::ortho(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top)
{
	m_projection_matrix = glm::ortho(left,right,bottom,top);
}

void Indigo::Camera::ortho(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top, const float_t& zNear, const float_t& zFar)
{
	m_projection_matrix = glm::ortho(left,right,bottom,top,zNear,zFar);
}

void Indigo::Camera::perspective(const float_t& fovy, const float_t& aspect, const float_t& zNear, const float_t& zFar)
{
	m_projection_matrix = glm::perspective(fovy,aspect,zNear,zFar);
}

void Indigo::Camera::draw()
{
	// Draw
	if (m_scene)
	{
		glm::mat4 view_matrix = m_projection_matrix * m_camera_matrix;

		m_scene->draw(view_matrix);
	}
}

void Indigo::Camera::cull()
{
	if (m_scene)
	{
		glm::mat4 view_matrix = m_projection_matrix * m_camera_matrix;

		// Frustrum Cull

		// create non-normalized clipping planes
		glm::vec4 col0(glm::column(view_matrix,0));
		glm::vec4 col1(glm::column(view_matrix,1));
		glm::vec4 col2(glm::column(view_matrix,2));
		glm::vec4 col3(glm::column(view_matrix,3));

		glm::vec4 planes[6];
		planes[0] = col3 + col0; // left
		planes[1] = col3 - col0; // right
		planes[2] = col3 - col1; // top
		planes[3] = col3 + col1; // bottom
		planes[4] = col3 + col2; // near
		planes[5] = col3 - col2; // far

		glm::vec4 abs_planes[6];
		abs_planes[0] = glm::abs(planes[0]);
		abs_planes[1] = glm::abs(planes[1]);
		abs_planes[2] = glm::abs(planes[2]);
		abs_planes[3] = glm::abs(planes[3]);
		abs_planes[4] = glm::abs(planes[4]);
		abs_planes[5] = glm::abs(planes[5]);

		m_scene->cull(planes,abs_planes);
	}
}
