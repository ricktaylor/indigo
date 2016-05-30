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

namespace Indigo
{
	class SGGroup;

	namespace Render
	{
		class SGGroup;

		class SGNode : public OOBase::NonCopyable
		{
		public:
			virtual bool valid() const { return true; }

			SGGroup* parent() const { return m_parent; }

			bool is_dirty() const { return m_dirty; }
			virtual void update();

		protected:
			SGNode(SGGroup* parent);

			virtual ~SGNode() {}

			void make_dirty();

		private:
			bool     m_dirty;
			SGGroup* m_parent;
		};

		class SGGroup : public SGNode
		{
			friend class Indigo::SGGroup;

		public:
			
		};
	}

	class SGNode : public OOBase::NonCopyable
	{
		friend class SGGroup;
		friend class Render::SGGroup;

	public:
		virtual ~SGNode();

		SGGroup* parent() const { return m_parent; }

		virtual bool valid() const { return true; }

	protected:
		SGNode(SGNode* parent);

		virtual bool on_render_create(Render::SGGroup* parent) = 0;

	private:
		SGGroup* m_parent;		
	};

	class SGGroup : public SGNode
	{
	public:

	protected:
		Render::SGGroup* m_render_group;

		bool attach_node(const OOBase::SharedPtr<SGNode>& node);
		
	private:		
	};

	class SGSimpleGroup : public SGGroup
	{

	};
}

#endif // INDIGO_SGNODE_H_INCLUDED
