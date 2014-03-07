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

#ifndef INDIGO_CAMERA_H_INCLUDED
#define INDIGO_CAMERA_H_INCLUDED

#include "State.h"

namespace Indigo
{
	class Viewport;
	class Scene;

	class Camera : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Camera>
	{
		friend class Viewport;

	public:
		Camera(const OOBase::SharedPtr<Viewport>& viewport, const glm::mat4& camera_matrix = glm::mat4(), const glm::mat4& projection_matrix = glm::mat4());
		Camera(const Camera& rhs);
		~Camera();

		Camera& operator = (const Camera& rhs);
		void swap(Camera& rhs);

		OOBase::SharedPtr<Viewport> viewport() const;

		const OOBase::SharedPtr<Scene>& scene() const;
		void scene(const OOBase::SharedPtr<Scene>& scene);

		typedef glm::mat4::value_type float_t;

		void camera_matrix(const glm::mat4& camera_matrix);
		const glm::mat4& camera_matrix() const;

		void look_at(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up);

		void projection_matrix(const glm::mat4& projection_matrix);
		const glm::mat4& projection_matrix() const;

		void frustum(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top, const float_t& zNear, const float_t& zFar);
	 	void ortho(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top);
	 	void ortho(const float_t& left, const float_t& right, const float_t& bottom, const float_t& top, const float_t& zNear, const float_t& zFar);
	 	void perspective(const float_t& fovy, const float_t& aspect, const float_t& zNear, const float_t& zFar);

	 	enum CullType
	 	{
	 		outside = 0,
	 		intersect,
	 		inside
	 	};
	 	enum CullType frustum_cull(const glm::vec3& centre, const glm::vec3& extent, unsigned int& hint) const;
	 	enum CullType frustum_cull(const glm::vec3& centre, const glm::vec3& extent) const;

	private:
		OOBase::WeakPtr<Viewport> m_viewport;
		OOBase::SharedPtr<Scene>  m_scene;
		glm::mat4                 m_camera_matrix;
		glm::mat4                 m_projection_matrix;

		bool                      m_dirty;
		glm::vec4                 m_clip_planes[6];
		glm::vec4                 m_clip_abs_planes[6];

		void refresh();
		void draw(State& gl_state);
	};
}

#endif // INDIGO_CAMERA_H_INCLUDED
