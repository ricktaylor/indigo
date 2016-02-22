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

#ifndef INDIGO_UILAYER_H_INCLUDED
#define INDIGO_UILAYER_H_INCLUDED

#include "Window.h"

namespace Indigo
{
	class UIWidget;
	class UIGroup;

	namespace Render
	{
		class UIDrawable : public OOBase::NonCopyable
		{
		public:
			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const = 0;
		};

		class UIGroup : public UIDrawable
		{
			friend class Indigo::UIGroup;
			friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

		protected:
			UIGroup()
			{}

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;

		private:
			OOBase::Table<unsigned int,OOBase::SharedPtr<UIDrawable>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_children;

			void add_widget_group(UIWidget* widget, unsigned int zorder, bool* ret);
		};
	}

	class UIWidget : public OOBase::NonCopyable
	{
		friend class UIGroup;
		friend class UILayer;
		friend class Render::UIGroup;

	public:
		virtual bool valid()
		{
			return m_render_group;
		}

	protected:
		UIWidget()
		{}

		const OOBase::SharedPtr<Indigo::Render::UIGroup>& render_group()
		{
			return m_render_group;
		}

		virtual bool on_render_create(Indigo::Render::UIGroup* group)
		{
			return true;
		}

	private:
		OOBase::SharedPtr<Indigo::Render::UIGroup> m_render_group;
	};

	class UIGroup : public UIWidget
	{
	public:
		bool add_widget(const OOBase::SharedPtr<UIWidget>& widget, unsigned int zorder);
		bool remove_widget(unsigned int zorder);

		template <typename Widget>
		OOBase::SharedPtr<Widget> get_widget(unsigned int zorder) const
		{
			OOBase::Table<unsigned int,OOBase::SharedPtr<UIWidget>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i = m_children.find(zorder);
			if (i == m_children.end())
				return OOBase::SharedPtr<Widget>();
			return OOBase::static_pointer_cast<Widget>(i->second);
		}

	protected:
		OOBase::Table<unsigned int,OOBase::SharedPtr<UIWidget>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_children;
	};

	class UILayer : public UIGroup, public Layer
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

	public:

	protected:
		OOBase::SharedPtr<Render::Layer> create_render_layer(Indigo::Render::Window* const window);
	};
}

#endif // INDIGO_UILAYER_H_INCLUDED
