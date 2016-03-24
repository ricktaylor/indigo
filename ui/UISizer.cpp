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

#include "../core/Common.h"

#include "UISizer.h"

Indigo::UIGridSizer::UIGridSizer(bool fixed, const glm::uvec4& margins, const glm::uvec2& padding) :
	UISizer(margins),
	m_fixed(fixed),
	m_padding(padding)
{
}

bool Indigo::UIGridSizer::add_widget(unsigned int row, unsigned int col, const OOBase::SharedPtr<UIWidget>& widget, unsigned int layout_flags, unsigned int proportion)
{
	struct Item item;
	item.m_widget = widget;
	item.m_flags = layout_flags;
	item.m_proportion = proportion;
	return m_items.insert(OOBase::Pair<unsigned int,unsigned int>(row,col),item);
}

bool Indigo::UIGridSizer::add_sizer(unsigned int row, unsigned int col, const OOBase::SharedPtr<UISizer>& sizer, unsigned int layout_flags, unsigned int proportion)
{
	struct Item item;
	item.m_sizer = sizer;
	item.m_flags = layout_flags;
	item.m_proportion = proportion;
	return m_items.insert(OOBase::Pair<unsigned int,unsigned int>(row,col),item);
}

bool Indigo::UIGridSizer::add_spacer(unsigned int row, unsigned int col, const glm::uvec2& size, unsigned int proportion)
{
	struct Item item;
	item.m_size = size;
	item.m_flags = 0;
	item.m_proportion = proportion;
	return m_items.insert(OOBase::Pair<unsigned int,unsigned int>(row,col),item);
}

bool Indigo::UIGridSizer::remove_item(unsigned int row, unsigned int col)
{
	return m_items.remove(OOBase::Pair<unsigned int,unsigned int>(row,col));
}

bool Indigo::UIGridSizer::measure(OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
		OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
		OOBase::Pair<unsigned int,unsigned int>& cumulative_width,
		OOBase::Pair<unsigned int,unsigned int>& cumulative_height,
		bool min_size) const
{
	// Find the rows and columns sizes and proportions
	unsigned int max_var_width = 0;
	unsigned int max_var_height = 0;

	for (items_t::const_iterator i=m_items.begin();i;++i)
	{
		glm::uvec2 sz(0);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<UIWidget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
			{
				if (min_size)
					sz = widget->min_size() + m_padding;
				else
					sz = widget->ideal_size() + m_padding;
			}
		}
		else if (i->second.m_sizer)
		{
			if (min_size)
				sz = i->second.m_sizer->min_fit() + m_padding;
			else
				sz = i->second.m_sizer->ideal_fit() + m_padding;
		}
		else
			sz = i->second.m_size + m_padding;

		while (widths.size() <= i->first.first)
		{
			if (!widths.push_back(OOBase::Pair<unsigned int,unsigned int>(0,0)))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text()),false);
		}

		while (heights.size() <= i->first.second)
		{
			if (!heights.push_back(OOBase::Pair<unsigned int,unsigned int>(0,0)))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text()),false);
		}

		OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator p = widths.position(i->first.first);
		if (sz.x > p->first)
			p->first = sz.x;

		p->second += i->second.m_proportion;

		if (m_fixed && p->second && p->first > max_var_width)
			max_var_width = p->first;

		p = heights.position(i->first.second);
		if (sz.y > p->first)
			p->first = sz.y;

		p->second += i->second.m_proportion;

		if (m_fixed && p->second && p->first > max_var_height)
			max_var_height = p->first;
	}

	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
	{
		if (m_fixed && i->second)
		{
			i->first = max_var_width;
			cumulative_width.first += i->first;
		}
		else
			cumulative_width.first += i->first;

		cumulative_width.second += i->second;
	}
	if (cumulative_width.first)
		cumulative_width.first -= m_padding.x;

	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
	{
		if (m_fixed && i->second)
		{
			i->first = max_var_height;
			cumulative_height.first += i->first;
		}
		else
			cumulative_height.first += i->first;

		cumulative_height.second += i->second;
	}
	if (cumulative_height.first)
		cumulative_height.first -= m_padding.y;

	return true;
}

glm::uvec2 Indigo::UIGridSizer::min_fit() const
{
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<unsigned int,unsigned int> min_width(0,0);
	OOBase::Pair<unsigned int,unsigned int> min_height(0,0);

	if (!measure(widths,heights,min_width,min_height,true))
		return glm::uvec2(0);

	return glm::uvec2(min_width.first + m_margins.x + m_margins.z,min_height.first + m_margins.y + m_margins.w);
}

glm::uvec2 Indigo::UIGridSizer::ideal_fit() const
{
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<unsigned int,unsigned int> ideal_width(0,0);
	OOBase::Pair<unsigned int,unsigned int> ideal_height(0,0);

	if (!measure(widths,heights,ideal_width,ideal_height,false))
		return glm::uvec2(0);

	return glm::uvec2(ideal_width.first + m_margins.x + m_margins.z,ideal_height.first + m_margins.y + m_margins.w);
}

void Indigo::UIGridSizer::fit(const glm::uvec2& outer_size)
{
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<unsigned int,unsigned int> ideal_width(0,0);
	OOBase::Pair<unsigned int,unsigned int> ideal_height(0,0);

	if (!measure(widths,heights,ideal_width,ideal_height,false))
		return;

	// Adjust the widths and heights
	glm::uvec2 size(outer_size.x - (m_margins.x + m_margins.z),outer_size.y - (m_margins.y + m_margins.w));
	if (size.x != ideal_width.first && ideal_width.second)
	{
		float expand_x = (static_cast<float>(size.x) - ideal_width.first) / ideal_width.second;

		for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
			i->first += static_cast<unsigned int>(expand_x * i->second);
	}
	if (size.y != ideal_height.first && ideal_height.second)
	{
		float expand_y = (static_cast<float>(size.y) - ideal_height.first) / ideal_height.second;

		for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
			i->first += static_cast<unsigned int>(expand_y * i->second);
	}

	// Reset the widths and heights .second to the accumulated final position
	unsigned int prev = m_position.x + m_margins.x;
	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
	{
		i->second = prev;
		prev += i->first + m_padding.x;
	}
	prev = m_position.y + m_margins.w;
	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
	{
		i->second = prev;
		prev += i->first + m_padding.y;
	}

	// Loop through the grid moving and sizing cells
	for (items_t::iterator i = m_items.begin();i;++i)
	{
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<UIWidget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
			{
				glm::uvec2 cell_size(widths[i->first.first].first,heights[i->first.second].first);
				glm::ivec2 cell_pos(widths[i->first.first].second,heights[i->first.second].second);

				glm::ivec2 item_pos(cell_pos);
				glm::uvec2 item_size(widget->size());

				if (cell_size.x < item_size.x || (i->second.m_flags & expand_horiz))
					item_size.x = cell_size.x;

				if (cell_size.y < item_size.y || (i->second.m_flags & expand_vert))
					item_size.y = cell_size.y;

				item_size = widget->size(item_size);
				
				// Align if required
				if ((i->second.m_flags & (align_right | expand_horiz)) == align_right)
					item_pos.x = cell_pos.x + static_cast<int>(cell_size.x) - static_cast<int>(item_size.x);
				else if ((i->second.m_flags & (align_hcentre | expand_horiz)) == align_hcentre)
					item_pos.x = cell_pos.x + (static_cast<int>(cell_size.x) - static_cast<int>(item_size.x))/2;

				if ((i->second.m_flags & (align_top | expand_vert)) == align_top)
					item_pos.y = cell_pos.y + static_cast<int>(cell_size.y) - static_cast<int>(item_size.y);
				else if ((i->second.m_flags & (align_vcentre | expand_vert)) == align_vcentre)
					item_pos.y = cell_pos.y + (static_cast<int>(cell_size.y) - static_cast<int>(item_size.y))/2;

				widget->position(item_pos);
			}
		}
		else if (i->second.m_sizer)
		{
			glm::uvec2 cell_size(widths[i->first.first].first,heights[i->first.second].first);
			glm::ivec2 cell_pos(widths[i->first.first].second,heights[i->first.second].second);

			glm::ivec2 item_pos(cell_pos);
			glm::uvec2 item_size(i->second.m_sizer->ideal_fit());

			if (cell_size.x < item_size.x || (i->second.m_flags & expand_horiz))
				item_size.x = cell_size.x;

			if (cell_size.y < item_size.y || (i->second.m_flags & expand_vert))
				item_size.y = cell_size.y;

			item_size = glm::max(item_size,i->second.m_sizer->min_fit());

			i->second.m_sizer->fit(item_size);

			// Align if required
			if ((i->second.m_flags & (align_right | expand_horiz)) == align_right)
				item_pos.x = cell_pos.x + static_cast<int>(cell_size.x) - static_cast<int>(item_size.x);
			else if ((i->second.m_flags & (align_hcentre | expand_horiz)) == align_hcentre)
				item_pos.x = cell_pos.x + (static_cast<int>(cell_size.x) - static_cast<int>(item_size.x))/2;

			if ((i->second.m_flags & (align_top | expand_vert)) == align_top)
				item_pos.y = cell_pos.y + static_cast<int>(cell_size.y) - static_cast<int>(item_size.y);
			else if ((i->second.m_flags & (align_vcentre | expand_vert)) == align_vcentre)
				item_pos.y = cell_pos.y + (static_cast<int>(cell_size.y) - static_cast<int>(item_size.y))/2;

			i->second.m_sizer->position(item_pos);
		}
	}
}

void Indigo::UIGridSizer::position(const glm::ivec2& p)
{
	if (p != m_position)
	{
		glm::ivec2 offset = p - m_position;

		for (items_t::iterator i = m_items.begin();i;++i)
		{
			if (i->second.m_widget)
			{
				OOBase::SharedPtr<UIWidget> widget(i->second.m_widget.lock());
				if (widget && widget->visible())
					widget->position(widget->position() + offset);
			}
			else if (i->second.m_sizer)
				i->second.m_sizer->position(i->second.m_sizer->position() + offset);
		}

		m_position = p;
	}
}
