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

Indigo::UIGridSizer::UIGridSizer(const glm::uvec4& margins, const glm::uvec2& padding) :
	UISizer(margins),
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

glm::uvec2 Indigo::UIGridSizer::min_fit() const
{
	OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator> heights;
	unsigned int min_width = 0;
	unsigned int min_height = 0;

	for (items_t::const_iterator i=m_items.begin();i;++i)
	{
		glm::uvec2 sz(0);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<UIWidget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
				sz = widget->min_size() + m_padding;
		}
		else
			sz = i->second.m_size + m_padding;

		while (widths.size() <= i->first.first)
		{
			if (!widths.push_back(0))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text()),glm::uvec2(0));
		}
		while (heights.size() <= i->first.second)
		{
			if (!heights.push_back(0))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text()),glm::uvec2(0));
		}

		if (sz.x > widths[i->first.first])
			widths[i->first.first] = sz.x;

		if (sz.y > heights[i->first.second])
			heights[i->first.second] = sz.y;
	}

	for (OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
		min_width += *i;

	if (min_width)
		min_width -= m_padding.x;

	for (OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		min_height += *i;

	if (min_height)
		min_height -= m_padding.y;

	return glm::uvec2(min_width + m_margins.x + m_margins.z,min_height + m_margins.y + m_margins.w);
}

glm::uvec2 Indigo::UIGridSizer::ideal_fit() const
{
	OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator> heights;
	unsigned int ideal_width = 0;
	unsigned int ideal_height = 0;

	for (items_t::const_iterator i=m_items.begin();i;++i)
	{
		glm::uvec2 sz(0);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<UIWidget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
				sz = widget->ideal_size() + m_padding;
		}
		else
			sz = i->second.m_size + m_padding;

		while (widths.size() <= i->first.first)
		{
			if (!widths.push_back(0))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text()),glm::uvec2(0));
		}
		while (heights.size() <= i->first.second)
		{
			if (!heights.push_back(0))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text()),glm::uvec2(0));
		}

		if (sz.x > widths[i->first.first])
			widths[i->first.first] = sz.x;

		if (sz.y > heights[i->first.second])
			heights[i->first.second] = sz.y;
	}

	for (OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
		ideal_width += *i;

	if (ideal_width)
		ideal_width -= m_padding.x;

	for (OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		ideal_height += *i;

	if (ideal_height)
		ideal_height -= m_padding.y;

	return glm::uvec2(ideal_width + m_margins.x + m_margins.z,ideal_height + m_margins.y + m_margins.w);
}

void Indigo::UIGridSizer::fit(const glm::uvec2& outer_size)
{
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<unsigned int,unsigned int> ideal_width;
	OOBase::Pair<unsigned int,unsigned int> ideal_height;

	// Find the rows and columns sizes and proportions
	for (items_t::iterator i=m_items.begin();i;++i)
	{
		glm::uvec2 sz(0);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<UIWidget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
				sz = widget->ideal_size() + m_padding;
		}
		else
			sz = i->second.m_size + m_padding;

		while (widths.size() <= i->first.first)
			widths.push_back(OOBase::Pair<unsigned int,unsigned int>(0,0));
		
		while (heights.size() <= i->first.second)
			heights.push_back(OOBase::Pair<unsigned int,unsigned int>(0,0));
				
		if (sz.x > widths[i->first.first].first)
			widths[i->first.first].first = sz.x;

		if (sz.y > heights[i->first.second].first)
			heights[i->first.second].first = sz.y;

		widths[i->first.first].second += i->second.m_proportion;
		heights[i->first.second].second += i->second.m_proportion;
	}

	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
	{
		ideal_width.first += i->first;
		ideal_width.second += i->second;
	}
	if (ideal_width.first)
		ideal_width.first -= m_padding.x;

	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
	{
		ideal_height.first += i->first;
		ideal_height.second += i->second;
	}
	if (ideal_height.first)
		ideal_height.first -= m_padding.y;

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

	// Reset the widths and heights .second to the accumulated position
	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
	{
		if (i != widths.begin())
			i->second = (i-1)->first + (i-1)->second + m_padding.x;
		else
			i->second = m_margins.x;
	}
	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
	{
		if (i != heights.begin())
			i->second = (i-1)->first + (i-1)->second + m_padding.y;
		else
			i->second = m_margins.w;
	}

	// Loop through the grid resizing cells
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
				glm::uvec2 item_size;

				// Expand if required
				if (item_size != cell_size && (i->second.m_flags & expand))
				{
					if (i->second.m_flags & expand_horiz)
						item_size.x = cell_size.x;
					if (i->second.m_flags & expand_vert)
						item_size.y = cell_size.y;

					item_size = widget->size(item_size);
				}
				else
					item_size = widget->size();
				
				// Align if required
				if (i->second.m_flags & align_right)
					item_pos.x = cell_pos.x + static_cast<int>(cell_size.x) - static_cast<int>(item_size.x);
				else if (i->second.m_flags & align_hcentre)
					item_pos.x = cell_pos.x + (static_cast<int>(cell_size.x) - static_cast<int>(item_size.x))/2;

				if (i->second.m_flags & align_top)
					item_pos.y = cell_pos.y + static_cast<int>(cell_size.y) - static_cast<int>(item_size.y);
				else if (i->second.m_flags & align_vcentre)
					item_pos.y = cell_pos.y + (static_cast<int>(cell_size.y) - static_cast<int>(item_size.y))/2;

				widget->position(item_pos);
			}
		}
	}
}
