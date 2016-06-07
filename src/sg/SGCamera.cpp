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

namespace
{


	class ClipVisitor : public Indigo::Render::SGVisitor
	{
	public:
		ClipVisitor(const glm::vec3& source) :
			m_front_to_back(source),
			m_nodes(m_front_to_back)
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

		/*struct BackToFront
		{
			BackToFront(const glm::vec3& source) : m_source(source)
			{}

			bool operator() (const node_info& lhs, const node_info& rhs) const
			{
				// Sort front to back
				return glm::distance(m_source,lhs.m_bounds.m_midpoint) > glm::distance(m_source,rhs.m_bounds.m_midpoint);
			}

			glm::vec3 m_source;
		} m_back_to_front;*/

		OOBase::Set<node_info,FrontToBack,OOBase::ThreadLocalAllocator> m_nodes;
	};
}

bool ClipVisitor::visit(const Indigo::Render::SGNode& node, OOBase::uint32_t& hint)
{
	if (!node.visible())
		return false;

	node_info i;
	i.m_drawable = node.drawable();
	if (i.m_drawable)
	{
		i.m_bounds = node.world_bounds();
		i.m_transform = node.world_transform();
		m_nodes.insert(i);
	}

	return true;
}

void ClipVisitor::draw(OOGL::State& glState, const glm::mat4& mvp) const
{
	glState.disable(GL_BLEND);

	for (OOBase::Set<node_info,FrontToBack,OOBase::ThreadLocalAllocator>::const_iterator i=m_nodes.begin();i;++i)
		i->m_drawable->on_draw(glState,mvp * i->m_transform);
}

void Indigo::Render::SGCamera::on_draw(OOGL::State& glState) const
{
	if (m_scene)
	{
		m_scene->on_update(glm::mat4());

		ClipVisitor visitor(m_source);
		m_scene->visit(visitor);

		visitor.draw(glState,view_proj());
	}
}

Indigo::SGCamera::SGCamera(const CreateParams& params) :
		m_scene(params.m_scene),
		m_position(params.m_position),
		m_target(params.m_target),
		m_up(params.m_up),
		m_near(params.m_near),
		m_far(params.m_far),
		m_fov(params.m_fov),
		m_ortho(!params.m_perspective)
{
}

OOBase::SharedPtr<Indigo::Render::Layer> Indigo::SGCamera::create_render_layer(Render::Window* window)
{
	m_size = window->window()->size();

	Render::SGNode* render_scene = NULL;
	if (m_scene)
		render_scene = m_scene->render_node();

	OOBase::SharedPtr<Render::SGCamera> render_cam = OOBase::allocate_shared<Render::SGCamera,OOBase::ThreadLocalAllocator>(window,m_position,view_proj(),render_scene);
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

void Indigo::SGCamera::position(const glm::vec3& pos)
{
	if (m_position != pos)
	{
		m_position = pos;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj_source),view_proj(),m_position);
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

void Indigo::SGCamera::on_size(const glm::uvec2& sz)
{
	if (m_size.x != sz.x || m_size.y != sz.y)
	{
		m_size = sz;

		if (m_render_camera)
			render_pipe()->post(OOBase::make_delegate<OOBase::ThreadLocalAllocator>(m_render_camera.get(),&Render::SGCamera::view_proj),view_proj());
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
	if (m_on_close)
	{
		m_on_close.invoke();
		return true;
	}
	return false;
}
