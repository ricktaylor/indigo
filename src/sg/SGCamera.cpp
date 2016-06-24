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

#include "../../include/indigo/sg/SGCamera.h"

#include "../../include/indigo/Render.h"
#include "../../include/indigo/Window.h"

#include "../Common.h"

#include <OOBase/Set.h>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace
{
	class RenderVisitor : public Indigo::Render::SGVisitor
	{
	public:
		RenderVisitor(const glm::vec3& source) :
			m_front_to_back(source),
			m_back_to_front(source),
			m_opaque_nodes(m_front_to_back),
			m_transparent_nodes(m_back_to_front)
		{}

		bool visit(const Indigo::Render::SGNode& node, OOBase::uint32_t& hint);
		void draw(OOGL::State& glState, const glm::mat4& mvp) const;

		struct node_info
		{
			Indigo::AABB m_bounds;
			glm::mat4 m_transform;
			OOBase::SharedPtr<Indigo::Render::SGDrawable> m_drawable;
		};

		struct FrontToBack
		{
			FrontToBack(const glm::vec3& source) : m_source(source)
			{}

			bool operator() (const node_info& lhs, const node_info& rhs) const
			{
				// Sort front to back
				return glm::distance(m_source,lhs.m_bounds.m_midpoint) < glm::distance(m_source,rhs.m_bounds.m_midpoint);
			}

			glm::vec3 m_source;
		} m_front_to_back;

		struct BackToFront
		{
			BackToFront(const glm::vec3& source) : m_source(source)
			{}

			bool operator() (const node_info& lhs, const node_info& rhs) const
			{
				// Sort front to back
				return glm::distance(m_source,lhs.m_bounds.m_midpoint) > glm::distance(m_source,rhs.m_bounds.m_midpoint);
			}

			glm::vec3 m_source;
		} m_back_to_front;

		OOBase::Set<node_info,FrontToBack,OOBase::ThreadLocalAllocator> m_opaque_nodes;
		OOBase::Set<node_info,BackToFront,OOBase::ThreadLocalAllocator> m_transparent_nodes;
	};
}

bool RenderVisitor::visit(const Indigo::Render::SGNode& node, OOBase::uint32_t& hint)
{
	if (!node.visible())
		return false;

	node_info i;
	i.m_drawable = node.drawable();
	if (i.m_drawable)
	{
		i.m_bounds = node.world_bounds();
		i.m_transform = node.world_transform();

		// TODO:  Clip test!!

		if (node.transparent())
			m_transparent_nodes.insert(i);
		else
			m_opaque_nodes.insert(i);
	}

	return true;
}

void RenderVisitor::draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	glState.disable(GL_BLEND);
	glState.enable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	for (OOBase::Set<node_info,FrontToBack,OOBase::ThreadLocalAllocator>::const_iterator i=m_opaque_nodes.begin();i;++i)
		i->m_drawable->on_draw(glState,mvp * i->m_transform);

	glState.enable(GL_BLEND);
	glDepthMask(GL_FALSE);

	for (OOBase::Set<node_info,BackToFront,OOBase::ThreadLocalAllocator>::const_iterator i=m_transparent_nodes.begin();i;++i)
		i->m_drawable->on_draw(glState,mvp * i->m_transform);
}

bool Indigo::Render::SGCamera::on_update()
{
	if (!m_visible || !m_scene || !m_scene->dirty())
		return false;

	m_scene->on_update(glm::mat4());
	return true;
}

void Indigo::Render::SGCamera::on_draw(OOGL::State& glState) const
{
	if (m_visible && m_scene && m_scene->visible())
	{
		RenderVisitor visitor(m_source);
		m_scene->visit(visitor);

		visitor.draw(glState,view_proj());
	}
}

void Indigo::Render::SGCamera::on_size(const glm::uvec2& sz)
{
	// Let the logic pipe handle it
	logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::SGCamera::size),sz);
}

bool Indigo::Render::SGCamera::on_cursormove(const glm::dvec2& pos)
{
	if (!m_visible)
		return false;
	
	// First we must hit test the scene

	// If nothing hits, then check to see if it's a camera move
	switch (m_cam_control)
	{
	case eCC_pan:
		if (pos != m_cam_pos)
		{
			logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::SGCamera::on_pan),m_cam_pos - pos,m_scene->world_bounds());
			m_cam_pos = pos;
		}
		break;

	case eCC_rotate:
		if (pos != m_cam_pos)
		{
			logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::SGCamera::on_rotate),pos - m_cam_pos);
			m_cam_pos = pos;
		}
		break;

	default:
		break;
	}

	// Let the logic pipe handle it
	
	return true;
}

void Indigo::Render::SGCamera::on_mousebutton(const OOGL::Window::mouse_click_t& click)
{
	// If nothing else is hit

	if (click.down)
	{
		if (click.button == GLFW_MOUSE_BUTTON_LEFT)
		{
			m_cam_control = eCC_pan;
			m_cam_pos = m_window->cursor_pos();
		}
		else if (click.button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			m_cam_control = eCC_rotate;
			m_cam_pos = m_window->cursor_pos();
		}
	}
	else
		m_cam_control = eCC_none;
}

void Indigo::Render::SGCamera::on_scroll(const glm::dvec2& pos)
{
	// If nothing else is hit

	logic_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_owner,&Indigo::SGCamera::on_zoom),pos);
}

void Indigo::Render::SGCamera::on_losecursor()
{
	m_cam_control = eCC_none;
}

Indigo::SGCamera::SGCamera(const OOBase::SharedPtr<SGNode>& scene, const CreateParams& params) :
		m_scene(scene),
		m_position(params.m_position),
		m_target(params.m_target),
		m_up(params.m_up),
		m_near(params.m_near),
		m_far(params.m_far),
		m_fov(params.m_fov),
		m_ortho(!params.m_perspective),
		m_visible(false)
{
	if (!scene)
		LOG_WARNING(("SGCamera created with no scene!"));
}

Indigo::SGCamera::~SGCamera()
{
	render_pipe()->call(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(this,&SGCamera::destroy_render_layer));
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::SGCamera::create_render_layer(Render::Window* window)
{
	m_size = window->window()->size();

	Render::SGNode* render_scene = NULL;
	if (m_scene)
	{
		render_scene = m_scene->render_node();
		if (render_scene)
		{
			// Clamp the target to within the scene AABB
			AABB bounds = render_scene->world_bounds();
			
			glm::clamp(m_target,bounds.min(),bounds.max());
		}
	}

	OOBase::SharedPtr<Render::SGCamera> render_cam = OOBase::allocate_shared<Render::SGCamera,OOBase::ThreadLocalAllocator>(this,window,m_position,view_proj(),render_scene,m_visible);
	if (!render_cam)
		LOG_ERROR(("Failed to allocate render camera: %s",OOBase::system_error_text()));
	else
		m_render_camera = render_cam;

	return render_cam;
}

void Indigo::SGCamera::destroy_render_layer()
{
	m_render_camera.reset();
}

void Indigo::SGCamera::show(bool visible)
{
	if (visible != m_visible)
	{
		m_visible = visible;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::show),m_visible);
	}
}

void Indigo::SGCamera::on_pan(const glm::dvec2& pan_v, const AABB& bounds)
{
	glm::vec3 cam_dir = m_position - m_target;
	glm::vec3 dirx = glm::normalize(glm::cross(cam_dir,m_up));
	glm::vec3 diry = glm::normalize(glm::cross(m_up,dirx));
		
	glm::vec3 move = dirx * static_cast<float>(-pan_v.x);
	move += diry * static_cast<float>(-pan_v.y);

	float length = glm::length(cam_dir) * glm::tan(m_fov);
	move *= length / m_size.x;

	m_position += move;

	glm::vec3 new_target = m_target + move; //glm::clamp(m_target + move,bounds.min(),bounds.max());
	m_target = new_target;
			
	if (m_render_camera)
		render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj_source),view_proj(),m_position);
}

void Indigo::SGCamera::on_rotate(const glm::dvec2& rot_v)
{
	glm::vec3 eye = m_position - m_target;
	glm::vec3 dirx = glm::normalize(glm::cross(eye,m_up));
	glm::vec3 diry = glm::normalize(glm::cross(m_up,dirx));
		
	eye = glm::rotate(eye,static_cast<float>(glm::pi<double>() * rot_v.x / m_size.x),diry);
	eye = glm::rotate(eye,static_cast<float>(glm::pi<double>() * rot_v.y / m_size.y),dirx);

	glm::vec3 new_pos = m_target + eye;
	float angle = glm::angle(glm::normalize(m_target - new_pos),m_up);
	if (angle > 1.7 && angle < 3.0)
	{
		m_position = new_pos;
		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj_source),view_proj(),m_position);
	}
}

void Indigo::SGCamera::on_zoom(const glm::dvec2& pan_v)
{
	float zoom = (float)(pan_v.y - m_size.y);

	glm::vec3 cam_dir = m_position - m_target;
	float l = glm::length(cam_dir);
	if ((l > m_near * 1.9f || zoom > 0) && (l < m_far * 0.9f || zoom < 0))
	{
		m_position = m_target + (cam_dir * (1 + zoom / 10.f));
		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj_source),view_proj(),m_position);
	}
}

void Indigo::SGCamera::position(const glm::vec3& pos)
{
	if (m_position != pos)
	{
		m_position = pos;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj_source),view_proj(),m_position);
	}
}

void Indigo::SGCamera::size(const glm::uvec2& sz)
{
	if (m_size.x != sz.x || m_size.y != sz.y)
	{
		m_size = sz;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::target(const glm::vec3& t)
{
	if (m_target != t)
	{
		m_target = t;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::up(const glm::vec3& u)
{
	if (m_up != u)
	{
		m_up = u;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::ortho(bool ortho)
{
	if (m_ortho != ortho)
	{
		m_ortho = ortho;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

#if defined(_MSC_VER)
#pragma push_macro("near")
#pragma push_macro("far")
#undef near
#undef far
#endif

glm::mat4 Indigo::SGCamera::view_proj() const
{
	if (m_ortho)
		return glm::ortho(0.f,m_size.x,0.f,m_size.y,m_near,m_far) * glm::lookAt(m_position,m_target,m_up);

	return glm::perspectiveFov(m_fov,m_size.x,m_size.y,m_near,m_far) * glm::lookAt(m_position,m_target,m_up);
}


void Indigo::SGCamera::near(glm::mat4::value_type n)
{
	if (m_near != n)
	{
		m_near = n;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::far(glm::mat4::value_type f)
{
	if (m_far != f)
	{
		m_far = f;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

#if defined(_MSC_VER)
#pragma pop_macro("near")
#pragma pop_macro("far")
#endif

void Indigo::SGCamera::fov(glm::mat4::value_type f)
{
	if (m_fov != f)
	{
		m_fov = f;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
	}
}

void Indigo::SGCamera::scene(const OOBase::SharedPtr<SGNode>& s) 
{
	if (m_scene != s)
	{
		m_scene = s;

		Render::SGNode* render_scene = NULL;
		if (m_scene)
			render_scene = m_scene->render_node();

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::scene),render_scene);
	}
}

OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> Indigo::SGCamera::on_close(const OOBase::Delegate0<void,OOBase::ThreadLocalAllocator>& delegate)
{
	OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> prev = m_on_close;
	m_on_close = delegate;
	return prev;
}

bool Indigo::SGCamera::on_close()
{
	if (m_visible && m_on_close)
	{
		m_on_close.invoke();
		return true;
	}
	return false;
}
