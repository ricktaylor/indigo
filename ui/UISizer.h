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

#include "UIWidget.h"

namespace Indigo
{
	class UISizer
	{
	public:
		virtual void fit(const glm::uvec2& size) = 0;
		virtual glm::uvec2 min_fit() const = 0;
		virtual glm::uvec2 ideal_fit() const = 0;

		const glm::uvec4& margins() const { return m_margins; }
		virtual void margins(const glm::uvec4& m) { m_margins = m; }

	protected:
		UISizer(const glm::uvec4& margins = glm::uvec4(0)) : m_margins(margins)
		{}

		glm::uvec4 m_margins;
	};

	class UIGridSizer : public UISizer
	{
	public:
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

		UIGridSizer(const glm::uvec4& margins = glm::uvec4(0), const glm::uvec2& padding = glm::uvec2(0));
		
		const glm::uvec2& padding() const { return m_padding; }
		void padding(const glm::uvec2& p);

		virtual void fit(const glm::uvec2& size);
		virtual glm::uvec2 min_fit() const;
		virtual glm::uvec2 ideal_fit() const;

		bool add_widget(unsigned int row, unsigned int col, const OOBase::SharedPtr<UIWidget>& widget, unsigned int layout_flags = (align_centre | expand), unsigned int proportion = 1);
		bool add_spacer(unsigned int row, unsigned int col, const glm::uvec2& size, unsigned int proportion = 0);

		bool remove_item(unsigned int row, unsigned int col);

	private:
		glm::uvec2 m_padding;

		struct Item
		{
			OOBase::WeakPtr<UIWidget> m_widget;
			glm::uvec2                m_size;
			unsigned int              m_flags;
			unsigned int              m_proportion;
		};

		typedef OOBase::Table<OOBase::Pair<unsigned int,unsigned int>,Item,OOBase::Less<OOBase::Pair<unsigned int,unsigned int> >,OOBase::ThreadLocalAllocator> items_t;
		items_t m_items;
	};
}

#endif // INDIGO_SIZER_H_INCLUDED
