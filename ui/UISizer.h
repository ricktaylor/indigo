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

#ifndef INDIGO_SIZER_H_INCLUDED
#define INDIGO_SIZER_H_INCLUDED

#include "UILayer.h"

namespace Indigo
{
	class UIGridSizer : public UISizer
	{
	public:
		struct Layout
		{
			enum eLayoutFlags
			{
				align_left = 0,
				align_right = 1,
				align_hcentre = 2,
				align_bottom = 0,
				align_top = 1 << 2,
				align_vcentre = 2 << 2,
				align_centre = align_hcentre | align_vcentre,
				expand_horiz = 1 << 4,
				expand_vert = 2 << 4,
				expand = expand_horiz | expand_vert
			};
			OOBase::uint16_t m_flags;
			OOBase::uint16_t m_proportion;
		};

		UIGridSizer(const glm::uvec2& padding = glm::uvec2(0));
		
		Layout& default_layout() { return m_default_layout; }

		const glm::uvec2& padding() const { return m_padding; }
		void padding(const glm::uvec2& p);

		virtual void size(const glm::uvec2& size);
		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

		bool add_widget(const glm::uvec2& pos, const OOBase::SharedPtr<UIWidget>& widget);
		bool add_widget(const glm::uvec2& pos, const OOBase::SharedPtr<UIWidget>& widget, const Layout& layout);
		bool add_spacer(const glm::uvec2& pos, const glm::uvec2& size);
		bool add_spacer(const glm::uvec2& pos, const glm::uvec2& size, const Layout& layout);

		bool remove_item(const glm::uvec2& pos);

	private:
		Layout m_default_layout;
		glm::uvec2 m_padding;

		struct Item
		{
			OOBase::WeakPtr<UIWidget> m_widget;
			glm::uvec2                m_size;
			Layout                    m_layout;
		};

		typedef OOBase::Table<OOBase::Pair<unsigned int,unsigned int>,Item,OOBase::Less<OOBase::Pair<unsigned int,unsigned int> >,OOBase::ThreadLocalAllocator> items_t;
		items_t m_items;
	};
}

#endif // INDIGO_SIZER_H_INCLUDED
