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

#ifndef INDIGO_GUILAYER_H_INCLUDED
#define INDIGO_GUILAYER_H_INCLUDED

#include "Layer.h"

namespace Indigo
{
	namespace Render
	{
		class GUILayer;

		class Widget
		{
		public:
			OOBase::SharedPtr<Widget> parent() const;
			virtual GUILayer* layer() const;

			virtual bool add_widget(const OOBase::SharedPtr<Widget>& widget);

		protected:
			OOBase::Vector<OOBase::SharedPtr<Widget>,OOBase::ThreadLocalAllocator> m_children;
		};

		class GUILayer : public Layer
		{
		public:
			GUILayer();
			
			OOBase::uint32_t add_widget(const OOBase::SharedPtr<Widget>& widget);
			bool remove_widget(OOBase::uint32_t handle);
			OOBase::SharedPtr<Widget> lookup_widget(OOBase::uint32_t handle);

		protected:
			virtual void on_draw(const OOGL::Window& win, OOGL::State& glState);

		private:
			OOBase::uint32_t m_next_handle;
			OOBase::HashTable<OOBase::uint32_t,OOBase::SharedPtr<Widget>,OOBase::ThreadLocalAllocator> m_handles;
		};
	}
}

#endif // INDIGO_GUILAYER_H_INCLUDED
