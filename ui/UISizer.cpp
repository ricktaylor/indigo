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

namespace
{
	static const Indigo::UIGridSizer::Layout s_default_layout(Indigo::UIGridSizer::Layout::align_centre | Indigo::UIGridSizer::Layout::expand, 1);
}

Indigo::UIGridSizer::UIGridSizer(const glm::uvec2& padding) :
	m_default_layout(s_default_layout),
	m_padding(padding)
{
}

bool Indigo::UIGridSizer::add_widget(unsigned int row, unsigned int col, const OOBase::SharedPtr<UIWidget>& widget)
{
	return add_widget(row,col,widget,m_default_layout);
}

bool Indigo::UIGridSizer::add_widget(unsigned int row, unsigned int col, const OOBase::SharedPtr<UIWidget>& widget, const Layout& layout)
{
	struct Item item;
	item.m_widget = widget;
	item.m_layout = layout;
	return m_items.insert(OOBase::Pair<unsigned int,unsigned int>(row,col),item);
}

bool Indigo::UIGridSizer::add_spacer(unsigned int row, unsigned int col, const glm::uvec2& size)
{
	return add_spacer(row,col,size,m_default_layout);
}

bool Indigo::UIGridSizer::add_spacer(unsigned int row, unsigned int col, const glm::uvec2& size, const Layout& layout)
{
	struct Item item;
	item.m_size = size;
	item.m_layout = layout;
	return m_items.insert(OOBase::Pair<unsigned int,unsigned int>(row,col),item);
}

bool Indigo::UIGridSizer::remove_item(unsigned int row, unsigned int col)
{
	return m_items.remove(OOBase::Pair<unsigned int,unsigned int>(row,col));
}

glm::uvec2 Indigo::UIGridSizer::min_size() const
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
		min_width += m_padding.x;

	for (OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		min_height += *i;

	if (min_height)
		min_height += m_padding.y;

	return glm::uvec2(min_width,min_height);
}

glm::uvec2 Indigo::UIGridSizer::ideal_size() const
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
		ideal_width += m_padding.x;

	for (OOBase::Vector<unsigned int,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		ideal_height += *i;

	if (ideal_height)
		ideal_height += m_padding.y;

	return glm::uvec2(ideal_width,ideal_height);
}

void Indigo::UIGridSizer::size(const glm::uvec2& size)
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

		widths[i->first.first].second += i->second.m_layout.m_proportion;
		heights[i->first.second].second += i->second.m_layout.m_proportion;
	}

	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
	{
		ideal_width.first += i->first;
		ideal_width.second += i->second;
	}
	if (ideal_width.first)
		ideal_width.first += m_padding.x;

	for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
	{
		ideal_height.first += i->first;
		ideal_height.second += i->second;
	}
	if (ideal_height.first)
		ideal_height.first += m_padding.y;

	// Adjust the widths and heights
	if (size.x != ideal_width.first && ideal_width.second)
	{
		float expand_x = static_cast<float>(size.x - ideal_width.first) / ideal_width.second;
		for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
		{
			int w = i->first + static_cast<int>(expand_x * i->second / ideal_width.second);
			if (w < 0)
				i->first = 0;
			else
				i->first = w;
		}
	}
	if (size.y != ideal_height.first && ideal_height.second)
	{
		float expand_y = static_cast<float>(size.y - ideal_height.first) / ideal_height.second;
		for (OOBase::Vector<OOBase::Pair<unsigned int,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		{
			int h = i->first + static_cast<int>(expand_y * i->second / ideal_height.second);
			if (h < 0)
				i->first = 0;
			else
				i->first = h;
		}
	}

	// Loop through the grid resizing cells
	for (items_t::iterator i = m_items.begin();i;++i)
	{
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<UIWidget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
			{
				glm::ivec2 cell_pos(m_padding);
				glm::uvec2 cell_size(0);

				for (size_t x=0;x<i->first.first;++x)
					cell_pos.x += widths[x].first;
			
				for (size_t y=0;y<i->first.second;++y)
					cell_pos.y += heights[y].first;

				cell_pos.x += m_padding.x * i->first.first;
				cell_pos.y += m_padding.y * i->first.second;

				cell_size.x = widths[i->first.first].first;
				cell_size.y = heights[i->first.second].first;

				glm::ivec2 item_pos(cell_pos);
				glm::uvec2 item_size;

				// Expand if required
				if (item_size != cell_size && (i->second.m_layout.m_flags & Layout::expand))
				{
					if (i->second.m_layout.m_flags & Layout::expand_horiz)
						item_size.x = cell_size.x;
					if (i->second.m_layout.m_flags & Layout::expand_vert)
						item_size.y = cell_size.y;

					item_size = widget->size(item_size);
				}
				else
					item_size = widget->size();
				
				// Align if required
				if (i->second.m_layout.m_flags & Layout::align_right)
					item_pos.x = cell_pos.x + static_cast<int>(cell_size.x) - static_cast<int>(item_size.x);
				else if (i->second.m_layout.m_flags & Layout::align_hcentre)
					item_pos.x = cell_pos.x + (static_cast<int>(cell_size.x) - static_cast<int>(item_size.x))/2;

				if (i->second.m_layout.m_flags & Layout::align_top)
					item_pos.y = cell_pos.y + static_cast<int>(cell_size.y) - static_cast<int>(item_size.y);
				else if (i->second.m_layout.m_flags & Layout::align_vcentre)
					item_pos.y = cell_pos.y + (static_cast<int>(cell_size.y) - static_cast<int>(item_size.y))/2;

				widget->position(item_pos);

				LOG_DEBUG(("[%d,%d] {%u,%u}",item_pos.x,item_pos.y,item_size.x,item_size.y));
			}
		}
	}
}
