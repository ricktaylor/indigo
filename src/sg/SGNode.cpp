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

Indigo::Render::SGNode::SGNode(SGGroup* parent) :
		m_state(eRSG_dirty),
		m_parent(parent)
{
	make_dirty();
}

void Indigo::Render::SGNode::show(bool visible)
{
	if (visible)
		m_state |= eRSG_visible;
	else
		m_state &= ~eRSG_visible;
}

void Indigo::Render::SGNode::make_dirty()
{
	if (!(m_state & eRSG_dirty))
	{
		m_state |= eRSG_dirty;

		if (m_parent)
			m_parent->make_dirty();
	}
}

void Indigo::Render::SGNode::update(const glm::mat4& parent_transform)
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
	node->m_render_node = node->on_render_create(this);
	*ret = (node->m_render_node != NULL);
	if (*ret)
		make_dirty();
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
