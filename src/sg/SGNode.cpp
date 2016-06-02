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
#include "../../include/indigo/sg/SGNode.h"

#include "../../include/indigo/Render.h"

Indigo::Render::SGNode::SGNode(SGGroup* parent, bool visible, const glm::mat4& local_transform) :
		m_state((parent ? eRSG_dirty : 0) | (visible ? eRSG_visible : 0)),
		m_parent(parent),
		m_local_transform(local_transform),
		m_world_transform(local_transform)
{
}

void Indigo::Render::SGNode::show(bool visible)
{
	if (!visible)
		m_state &= ~eRSG_visible;
	else if (!(m_state & eRSG_visible))
	{
		m_state |= eRSG_visible;

		if (m_parent && dirty())
			m_parent->make_dirty();
	}
}

void Indigo::Render::SGNode::make_dirty()
{
	if (!(m_state & eRSG_dirty))
	{
		m_state |= eRSG_dirty;

		if (m_parent && visible())
			m_parent->make_dirty();
	}
}

void Indigo::Render::SGNode::on_update(const glm::mat4& parent_transform)
{
	m_world_transform = parent_transform * m_local_transform;

	m_state &= ~eRSG_dirty;
}

void Indigo::Render::SGNode::local_transform(glm::mat4 transform)
{
	if (m_local_transform != transform)
	{
		m_local_transform = transform;

		make_dirty();
	}
}

void Indigo::Render::SGGroup::attach_node(Indigo::SGNode* node, bool* ret)
{
	*ret = false;
	OOBase::SharedPtr<Render::SGNode> render_node = node->on_render_create(this);
	if (render_node)
	{
		*ret = add_node(render_node);
		if (*ret)
		{
			node->m_render_node = render_node.get();

			if (render_node->dirty())
				make_dirty();
		}
	}
}

Indigo::SGNode::SGNode(SGGroup* parent, const CreateParams& params) :
		m_parent(parent),
		m_render_node(NULL),
		m_state(params.m_state)
{
}

void Indigo::SGNode::toggle_state(OOBase::uint32_t new_state, OOBase::uint32_t mask)
{
	OOBase::uint32_t change_mask = (m_state ^ new_state) & mask;
	if (change_mask)
	{
		m_state = (m_state & ~mask) | (new_state & mask);

		on_state_change(m_state,change_mask);
	}
}

void Indigo::SGNode::on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask)
{
	if (change_mask & eNS_visible)
	{
		bool visible = (state & eNS_visible) == eNS_visible;
		if (m_render_node)
			render_pipe()->post(OOBase::make_delegate(m_render_node,&Render::SGNode::show),visible);
	}
}

void Indigo::SGNode::show(bool visible)
{
	toggle_state(visible,eNS_visible);
}

void Indigo::SGNode::enable(bool enable)
{
	toggle_state(enable,eNS_enabled);
}

glm::mat4 Indigo::SGNode::transform() const
{
	return glm::scale(glm::mat4_cast(m_rotation) * glm::translate(glm::mat4(),m_position),m_scaling);
}

void Indigo::SGNode::position(const glm::vec3& pos)
{
	if (m_position != pos)
	{
		m_position = pos;

		if (m_render_node)
			render_pipe()->post(OOBase::make_delegate(m_render_node,&Render::SGNode::local_transform),transform());
	}
}

void Indigo::SGNode::scaling(const glm::vec3& scale)
{
	if (m_scaling != scale)
	{
		m_scaling = scale;

		if (m_render_node)
			render_pipe()->post(OOBase::make_delegate(m_render_node,&Render::SGNode::local_transform),transform());
	}
}

void Indigo::SGNode::rotation(const glm::quat& rot)
{
	if (m_rotation != rot)
	{
		m_rotation = rot;

		if (m_render_node)
			render_pipe()->post(OOBase::make_delegate(m_render_node,&Render::SGNode::local_transform),transform());
	}
}

bool Indigo::SGGroup::attach_node(const OOBase::SharedPtr<SGNode>& node)
{
	if (!m_render_node)
		LOG_ERROR_RETURN(("Failed to insert node: incomplete group"),false);

	bool ret = false;
	render_pipe()->call(OOBase::make_delegate(static_cast<Indigo::Render::SGGroup*>(m_render_node),&Render::SGGroup::attach_node),node.get(),&ret);
	return ret;
}

namespace
{
	class SimpleGroup : public Indigo::Render::SGNode
	{
	public:
		SimpleGroup(Indigo::Render::SGGroup* parent) : SGNode(parent)
		{}

		OOBase::Vector<OOBase::SharedPtr<SGNode>,OOBase::ThreadLocalAllocator> m_children;

		void on_update(const glm::mat4& parent_transform);

		bool add_node(const OOBase::SharedPtr<SGNode>& node);
		bool remove_node(const OOBase::SharedPtr<SGNode>& node);
	};
}

void ::SimpleGroup::on_update(const glm::mat4& parent_transform)
{
	SGNode::on_update(parent_transform);

	for (OOBase::Vector<OOBase::SharedPtr<SGNode>,OOBase::ThreadLocalAllocator>::iterator i=m_children.begin();i;++i)
	{
		if ((*i)->dirty())
			(*i)->on_update(world_transform());
	}
}

bool ::SimpleGroup::add_node(const OOBase::SharedPtr<Indigo::Render::SGNode>& node)
{
	return m_children.push_back(node);
}

bool ::SimpleGroup::remove_node(const OOBase::SharedPtr<Indigo::Render::SGNode>& node)
{
	return m_children.remove(node) != 0;
}

bool Indigo::SGSimpleGroup::add_node(const OOBase::SharedPtr<SGNode>& node)
{
	if (!m_children.push_back(node))
		LOG_ERROR_RETURN(("Failed to insert node: %s",OOBase::system_error_text()),false);

	if (!attach_node(node))
	{
		m_children.pop_back();
		return false;
	}

	return true;
}

bool Indigo::SGSimpleGroup::remove_node(const OOBase::SharedPtr<SGNode>& node)
{
	return m_children.remove(node) != 0;
}

OOBase::SharedPtr<Indigo::Render::SGNode> Indigo::SGSimpleGroup::on_render_create(Render::SGGroup* parent)
{
	OOBase::SharedPtr<Indigo::Render::SGNode> node = OOBase::allocate_shared< ::SimpleGroup>(parent);
	if (!node)
		LOG_ERROR(("Failed to allocate: %s\n",OOBase::system_error_text()));
	return node;
}
