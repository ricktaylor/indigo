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

#if defined(_MSC_VER)
#pragma push_macro("near")
#pragma push_macro("far")
#undef near
#undef far
#endif

namespace Indigo
{
	class SGCamera;

	namespace Render
	{
		class SGCamera : public Layer
		{
			friend class Indigo::SGCamera;

		public:
			SGCamera(Indigo::SGCamera* owner, Window* window, const glm::vec3& source, const glm::mat4& vp, SGNode* scene, bool visible) :
				Layer(window), 
				m_owner(owner),
				m_source(source),
				m_view_proj(vp),
				m_scene(scene),
				m_visible(visible),
				m_cam_control(eCC_none)
			{ }

			const glm::mat4& view_proj() const { return m_view_proj; }
			const glm::vec3& source() const { return m_source; }
			SGNode* scene() const { return m_scene; }

		protected:
			virtual bool on_update();
			virtual void on_draw(OOGL::State& glState) const;
			virtual void on_size(const glm::uvec2& sz);

			virtual bool on_cursormove(const glm::dvec2& pos);
			virtual void on_mousebutton(const OOGL::Window::mouse_click_t& click);
			virtual void on_losecursor();
			virtual void on_scroll(const glm::dvec2& pos);
			//virtual void on_losefocus();

		private:
			Indigo::SGCamera* m_owner;
			glm::vec3 m_source;
			glm::mat4 m_view_proj;
			SGNode*   m_scene;
			bool      m_visible;
			
			enum eCamControl
			{
				eCC_none = 0,
				eCC_pan,
				eCC_rotate
			};
			enum eCamControl m_cam_control;
			glm::dvec2 m_cam_pos;

			void show(bool visible) { m_visible = visible; }
			void view_proj_source(const glm::mat4& vp, const glm::vec3& s) { m_view_proj = vp; m_source = s; }
			void view_proj(const glm::mat4& vp) { m_view_proj = vp; }
			void scene(SGNode* scene) { m_scene = scene; }
		};
	}

	class SGCamera : public Layer
	{
		friend class Render::SGCamera;

	public:
		struct CreateParams
		{
			CreateParams(
				const glm::vec3& position = glm::vec3(),
				const glm::vec3& target = glm::vec3(),
				const glm::vec3& up = glm::vec3(0.f,1.f,0.f),
				bool perspective = true,
				glm::mat4::value_type near = 0.1f,
				glm::mat4::value_type far = 100.f,
				glm::mat4::value_type fov = 0.785398f
			) : 
				m_position(position),
				m_target(target),
				m_up(up),
				m_perspective(perspective),
				m_near(near),
				m_far(far),
				m_fov(fov)
			{}

			glm::vec3                 m_position;
			glm::vec3                 m_target;
			glm::vec3                 m_up;
			bool                      m_perspective;
			glm::mat4::value_type     m_near;
			glm::mat4::value_type     m_far;
			glm::mat4::value_type     m_fov;
		};

		SGCamera(const OOBase::SharedPtr<SGNode>& scene, const CreateParams& params = CreateParams());
		virtual ~SGCamera();

		bool visible() const { return m_visible; }
		void show(bool visible = true);

		const glm::vec3& position() const { return m_position; }
		void position(const glm::vec3& pos);

		const glm::vec2& size() const { return m_size; }
		void size(const glm::uvec2& sz);

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

		const OOBase::SharedPtr<SGNode>& scene() const { return m_scene; }
		void scene(const OOBase::SharedPtr<SGNode>& s);

		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> on_close(const OOBase::Delegate0<void,OOBase::ThreadLocalAllocator>& delegate);

	private:
		OOBase::SharedPtr<Render::SGCamera> m_render_camera;
		OOBase::SharedPtr<SGNode> m_scene;
		glm::vec2 m_size;
		glm::vec3 m_position;
		glm::vec3 m_target;
		glm::vec3 m_up;
		glm::mat4::value_type m_near;
		glm::mat4::value_type m_far;
		glm::mat4::value_type m_fov;
		bool m_ortho;
		bool m_visible;

		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> m_on_close;

		OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window);
		void destroy_render_layer();

		bool on_close();
		void on_pan(const glm::dvec2& from, const glm::dvec2& to, const AABB& bounds);
		void on_rotate(const glm::dvec2& rot_v);
		void on_zoom(const glm::dvec2& zoom_v);
		
		glm::mat4 view() const;
		glm::mat4 proj() const;
		glm::mat4 view_proj() const { return proj() * view(); }
	};
}

#if defined(_MSC_VER)
#pragma pop_macro("near")
#pragma pop_macro("far")
#endif

#endif // INDIGO_SGCAMERA_H_INCLUDED
