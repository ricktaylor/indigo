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

#ifndef INDIGO_SGCAMERA_H_INCLUDED
#define INDIGO_SGCAMERA_H_INCLUDED

#include "SGNode.h"

#include "../Layer.h"

namespace Indigo
{
	class SGCamera;

	namespace Render
	{
		class SGCamera : public Layer
		{
			friend class Indigo::SGCamera;

		public:
			SGCamera(Window* window) : Layer(window)
			{}

			const glm::mat4& view_proj() const { return m_view_proj; }
			void view_proj(glm::mat4 vp);

		protected:
			virtual void on_draw(OOGL::State& glState) const;

		private:
			glm::mat4 m_view_proj;
		};
	}

	class SGCamera : public Layer
	{
	public:
		SGCamera();

		const glm::vec3& position() const { return m_position; }
		void position(const glm::vec3& pos);

		const glm::vec3& target() const { return m_target; }
		void target(const glm::vec3& t);

		const glm::vec3& up() const { return m_up; }
		void up(const glm::vec3& u);

		bool ortho() const { return m_ortho; }
		void ortho(bool ortho = true);

		bool perspective() const { return !m_ortho; }
		void perspective(bool pers) { ortho(!pers); }

		glm::mat4::value_type near() const { return m_near; }
		void near(glm::mat4::value_type n);

		glm::mat4::value_type far() const { return m_far; }
		void far(glm::mat4::value_type f);

		glm::mat4::value_type fov() const { return m_fov; }
		void fov(glm::mat4::value_type f);

	private:
		OOBase::SharedPtr<Render::SGCamera> m_render_camera;
		glm::vec2 m_size;
		glm::vec3 m_position;
		glm::vec3 m_target;
		glm::vec3 m_up;
		glm::mat4::value_type m_near;
		glm::mat4::value_type m_far;
		glm::mat4::value_type m_fov;
		bool m_ortho;

		OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window);
		void destroy_render_layer();

		void on_size(const glm::uvec2& sz);

		glm::mat4 view_proj() const;
	};
}

#endif // INDIGO_SGCAMERA_H_INCLUDED
