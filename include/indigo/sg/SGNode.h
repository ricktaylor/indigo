///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
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

#ifndef INDIGO_SGNODE_H_INCLUDED
#define INDIGO_SGNODE_H_INCLUDED

#include <OOBase/SharedPtr.h>

#include <glm/gtc/quaternion.hpp>

namespace Indigo
{
	class SGNode;
	class SGGroup;

	namespace Render
	{
		class AABB
		{
		public:
			AABB(const glm::vec3& midpoint = glm::vec3(), const glm::vec3& extents = glm::vec3()) :
				m_midpoint(midpoint),
				m_extents(extents)
			{}

			AABB(const AABB& rhs) :
				m_midpoint(rhs.m_midpoint),
				m_extents(rhs.m_extents)
			{}

			AABB& operator = (const AABB& rhs)
			{
				m_midpoint = rhs.m_midpoint;
				m_extents = rhs.m_extents;
				return *this;
			}

			glm::vec3 m_midpoint;
			glm::vec3 m_extents;
		};

		class SGGroup;

		class SGNode : public OOBase::NonCopyable
		{
		public:
			virtual ~SGNode() {}

			virtual bool valid() const { return true; }

			SGGroup* parent() const { return m_parent; }

			bool visible() const { return m_state & eRSG_visible; }
			void show(bool visible = true);

			bool dirty() const { return (m_state & (eRSG_visible | eRSG_dirty)) == (eRSG_visible | eRSG_dirty); }
			void make_dirty();

			const glm::mat4& local_transform() const { return m_local_transform; }
			void local_transform(glm::mat4 transform);

			const glm::mat4& world_transform() const { return m_world_transform; }
			virtual AABB world_AABB() const { return AABB(); }

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const {}
			virtual void on_update(const glm::mat4& parent_transform);


		protected:
			SGNode(SGGroup* parent, bool visible = false, const glm::mat4& local_transform = glm::mat4());

		private:
			enum State
			{
				eRSG_dirty = 1,
				eRSG_visible = 2,
			};
			unsigned int m_state;
			SGGroup*     m_parent;
			glm::mat4    m_local_transform;
			glm::mat4    m_world_transform;
		};

		class SGGroup : public SGNode
		{
			friend class Indigo::SGGroup;

		protected:
			virtual void on_update(const glm::mat4& parent_transform) = 0;

			virtual bool add_node(const OOBase::SharedPtr<SGNode>& node) = 0;
			virtual bool remove_node(const OOBase::SharedPtr<SGNode>& node) = 0;

		private:
			void attach_node(Indigo::SGNode* node, bool* ret);
		};
	}

	class SGNode : public OOBase::NonCopyable
	{
		friend class SGGroup;
		friend class Render::SGGroup;

	public:
		enum State
		{
			eNS_visible = 0x1,
			eNS_enabled = 0x2
		};

		struct CreateParams
		{
			CreateParams(OOBase::uint32_t state = 0
			) :
				m_state(state)
			{}

			OOBase::uint32_t m_state;
		};

		virtual ~SGNode() {}

		SGGroup* parent() const { return m_parent; }

		virtual bool valid() const { return m_render_node; }

		bool visible() const { return valid() && (m_state & eNS_visible); }
		virtual void show(bool visible = true);

		bool enabled() const { return valid() && (m_state & eNS_enabled); }
		void enable(bool enabled = true);

		OOBase::uint32_t state() const { return m_state; }
		void toggle_state(OOBase::uint32_t new_state, OOBase::uint32_t mask);
		void toggle_state(bool on, OOBase::uint32_t mask) { toggle_state(on ? mask : 0,mask); }

		const glm::vec3& position() const { return m_position; }
		void position(const glm::vec3& pos);

		const glm::vec3& scaling() const { return m_scaling; }
		void scaling(const glm::vec3& scale);

		const glm::quat& rotation() const { return m_rotation; }
		void rotation(const glm::quat& rot);

	protected:
		SGNode(SGGroup* parent, const CreateParams& params = CreateParams());

		Render::SGNode* render_node() const { return m_render_node; }

		virtual OOBase::SharedPtr<Render::SGNode> on_render_create(Render::SGGroup* parent) = 0;
		virtual void on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask);

	private:
		SGGroup*         m_parent;
		Render::SGNode*  m_render_node;
		OOBase::uint32_t m_state;
		glm::vec3        m_position;
		glm::vec3        m_scaling;
		glm::quat        m_rotation;

		glm::mat4 transform() const;
	};

	class SGGroup : public SGNode
	{
	protected:
		SGGroup(SGGroup* parent, const CreateParams& params = CreateParams()) : SGNode(parent,params)
		{}

		bool attach_node(const OOBase::SharedPtr<SGNode>& node);
	};

	class SGSimpleGroup : public SGGroup
	{
	public:
		SGSimpleGroup(SGGroup* parent = NULL, const CreateParams& params = CreateParams()) : SGGroup(parent,params)
		{}

		bool add_node(const OOBase::SharedPtr<SGNode>& node);
		bool remove_node(const OOBase::SharedPtr<SGNode>& node);

	private:
		OOBase::Vector<OOBase::SharedPtr<SGNode>,OOBase::ThreadLocalAllocator> m_children;

		OOBase::SharedPtr<Render::SGNode> on_render_create(Render::SGGroup* parent);
	};
}

#endif // INDIGO_SGNODE_H_INCLUDED
