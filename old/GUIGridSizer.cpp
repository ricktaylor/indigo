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

#include "../old/GUIGridSizer.h"

#include "../old/GUIPanel.h"
#include "Render.h"

namespace
{
	class GridSizer : public Indigo::Render::GUI::Sizer
	{
	public:
		struct Item
		{
			OOBase::WeakPtr<Indigo::Render::GUI::Widget> m_widget;
			OOBase::uint16_t               m_width;
			OOBase::uint16_t               m_height;
			Indigo::GUI::Sizer::ItemLayout m_layout;
		};

		typedef OOBase::Table<OOBase::Pair<unsigned int,unsigned int>,Item,OOBase::Less<OOBase::Pair<unsigned int,unsigned int> >,OOBase::ThreadLocalAllocator> items_t;
		items_t m_items;

		glm::u16vec2 m_padding;

		bool fit(Indigo::Render::GUI::Panel& panel);
		bool layout(const Indigo::Render::GUI::Panel& panel);

		bool add_widget(const OOBase::SharedPtr<Indigo::Render::GUI::Widget>& widget, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout);
		bool add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout);
		bool remove_item(unsigned int row, unsigned int column);
		bool remove_widget(const OOBase::SharedPtr<Indigo::Render::GUI::Widget>& widget);

		glm::u16vec2 ideal_size() const;

	private:
		bool get_extents(OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
					OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
					OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
					OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height);

		bool do_layout(const Indigo::Render::GUI::Panel& panel,
					OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
					OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
					OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
					OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height);
	};
}

glm::u16vec2 GridSizer::ideal_size() const
{
	OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator> heights;
	OOBase::uint16_t ideal_width = 0;
	OOBase::uint16_t ideal_height = 0;

	for (items_t::const_iterator i=m_items.begin();i;++i)
	{
		glm::u16vec2 sz(0);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<Indigo::Render::GUI::Widget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
				sz = widget->ideal_size() + m_padding;
		}
		else
			sz = glm::u16vec2(i->second.m_width,i->second.m_height) + m_padding;

		while (widths.size() <= i->first.first)
		{
			if (!widths.push_back(0))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),glm::u16vec2(0));
		}
		while (heights.size() <= i->first.second)
		{
			if (!heights.push_back(0))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),glm::u16vec2(0));
		}

		if (sz.x > widths[i->first.first])
			widths[i->first.first] = sz.x;

		if (sz.y > heights[i->first.second])
			heights[i->first.second] = sz.y;
	}

	for (OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
		ideal_width += *i;

	if (ideal_width)
		ideal_width += m_padding.x;

	for (OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		ideal_height += *i;

	if (ideal_height)
		ideal_height += m_padding.y;

	return glm::u16vec2(ideal_width,ideal_height);
}

bool GridSizer::get_extents(OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
			OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
			OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
			OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height)
{
	for (items_t::iterator i=m_items.begin();i;++i)
	{
		glm::u16vec2 sz(0);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<Indigo::Render::GUI::Widget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
				sz = widget->ideal_size() + m_padding;
		}
		else
			sz = glm::u16vec2(i->second.m_width,i->second.m_height) + m_padding;

		while (widths.size() <= i->first.first)
		{
			if (!widths.push_back(OOBase::Pair<OOBase::uint16_t,unsigned int>(0,0)))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}
		while (heights.size() <= i->first.second)
		{
			if (!heights.push_back(OOBase::Pair<OOBase::uint16_t,unsigned int>(0,0)))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}

		if (sz.x > widths[i->first.first].first)
			widths[i->first.first].first = sz.x;

		if (sz.y > heights[i->first.second].first)
			heights[i->first.second].first = sz.y;

		widths[i->first.first].second += i->second.m_layout.m_proportion;
		heights[i->first.second].second += i->second.m_layout.m_proportion;
	}

	for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
	{
		ideal_width.first += i->first;
		ideal_width.second += i->second;
	}
	if (ideal_width.first)
		ideal_width.first += m_padding.x;

	for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
	{
		ideal_height.first += i->first;
		ideal_height.second += i->second;
	}
	if (ideal_height.first)
		ideal_height.first += m_padding.y;

	return true;
}

bool GridSizer::fit(Indigo::Render::GUI::Panel& panel)
{
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_width(0,0);
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_height(0,0);
	if (!get_extents(widths,heights,ideal_width,ideal_height))
		return false;

	glm::u16vec2 size = panel.Widget::size();
	if (panel.client_size(glm::u16vec2(ideal_width.first,ideal_height.first)) == size)
		return true;

	return do_layout(panel,widths,heights,ideal_width,ideal_height);
}

bool GridSizer::layout(const Indigo::Render::GUI::Panel& panel)
{
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_width(0,0);
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_height(0,0);
	if (!get_extents(widths,heights,ideal_width,ideal_height))
		return false;

	return do_layout(panel,widths,heights,ideal_width,ideal_height);
}

bool GridSizer::do_layout(const Indigo::Render::GUI::Panel& panel,
		OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
		OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
		OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
		OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height)
{
	// Adjust the widths and heights
	glm::u16vec2 panel_size = panel.client_size();
	if (panel_size.x != ideal_width.first && ideal_width.second)
	{
		float expand_x = static_cast<float>(panel_size.x - ideal_width.first) / ideal_width.second;
		for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
		{
			int w = i->first + static_cast<int>(expand_x * i->second / ideal_width.second);
			if (w < 0)
				i->first = 0;
			else
				i->first = w;
		}
	}
	if (panel_size.y != ideal_height.first && ideal_height.second)
	{
		float expand_y = static_cast<float>(panel_size.y - ideal_height.first) / ideal_height.second;
		for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		{
			int h = i->first + static_cast<int>(expand_y * i->second / ideal_height.second);
			if (h < 0)
				i->first = 0;
			else
				i->first = h;
		}
	}

	// Loop through the grid
	glm::i16vec2 cell_pos;
	glm::u16vec2 cell_size;
	unsigned int y = 0;
	for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>::iterator k=heights.begin();k;++k)
	{
		cell_pos.x = 0;
		if (k->first)
		{
			cell_pos.y += m_padding.y;
			cell_size.y = k->first - m_padding.y;

			unsigned int x = 0;
			for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>::iterator j=widths.begin();j;++j)
			{
				if (j->first)
				{
					cell_pos.x += m_padding.x;
					cell_size.x = j->first - m_padding.x;

					items_t::iterator i = m_items.find(OOBase::Pair<unsigned int,unsigned int>(x,y));
					if (i && i->second.m_widget)
					{
						OOBase::SharedPtr<Indigo::Render::GUI::Widget> widget(i->second.m_widget.lock());
						if (widget && widget->visible())
						{
							glm::i16vec2 item_pos(cell_pos);
							glm::u16vec2 item_size(widget->size());

							// Expand if required
							if (item_size != cell_size && (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::expand))
							{
								if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::expand_horiz)
									item_size.x = cell_size.x;
								if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::expand_vert)
									item_size.y = cell_size.y;
							}
							else
								item_size = widget->ideal_size();

							item_size = widget->size(item_size);

							// Align if required
							if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::align_right)
								item_pos.x = cell_pos.x + static_cast<int>(cell_size.x) - item_size.x;
							else if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::align_hcentre)
								item_pos.x = cell_pos.x + (static_cast<int>(cell_size.x) - item_size.x)/2;

							if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::align_top)
								item_pos.y = cell_pos.y + static_cast<int>(cell_size.y) - item_size.y;
							else if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::align_vcentre)
								item_pos.y = cell_pos.y + (static_cast<int>(cell_size.y) - item_size.y)/2;

							widget->position(item_pos);
						}
					}
					cell_pos.x += j->first;
				}
				++x;
			}
			cell_pos.y += k->first;
		}
		++y;
	}

	return true;
}

bool GridSizer::add_widget(const OOBase::SharedPtr<Indigo::Render::GUI::Widget>& widget, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout)
{
	Item item;
	item.m_widget = widget;
	item.m_layout = layout;

	OOBase::Pair<unsigned int,unsigned int> pos(row,column);
	items_t::iterator i=m_items.find(pos);
	if (i)
	{
		i->second = item;
		return true;
	}
	return m_items.insert(pos,item);
}

bool GridSizer::add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout)
{
	Item item;
	item.m_width = width;
	item.m_height = height;
	item.m_layout = layout;

	OOBase::Pair<unsigned int,unsigned int> pos(row,column);
	items_t::iterator i=m_items.find(pos);
	if (i)
	{
		i->second = item;
		return true;
	}
	return m_items.insert(pos,item);
}

bool GridSizer::remove_widget(const OOBase::SharedPtr<Indigo::Render::GUI::Widget>& widget)
{
	for (items_t::iterator i=m_items.begin();i;++i)
	{
		if (i->second.m_widget.lock() == widget)
			return m_items.erase(i);
	}
	return false;
}

bool GridSizer::remove_item(unsigned int row, unsigned int column)
{
	return m_items.remove(OOBase::Pair<unsigned int,unsigned int>(row,column));
}

Indigo::GUI::GridSizer::GridSizer() : Sizer()
{

}

Indigo::GUI::GridSizer::~GridSizer()
{

}

OOBase::SharedPtr<Indigo::Render::GUI::Sizer> Indigo::GUI::GridSizer::create_render_sizer()
{
	OOBase::SharedPtr< ::GridSizer> sizer = OOBase::allocate_shared< ::GridSizer,OOBase::ThreadLocalAllocator>();
	if (!sizer)
		LOG_ERROR(("Failed to allocate grid sizer: %s",OOBase::system_error_text()));

	return sizer;
}

bool Indigo::GUI::GridSizer::create(OOBase::uint16_t hspace, OOBase::uint16_t vspace)
{
	return Sizer::create(hspace,vspace);
}

bool Indigo::GUI::GridSizer::create(OOBase::uint16_t hspace, OOBase::uint16_t vspace, const ItemLayout& layout)
{
	return Sizer::create(hspace,vspace,layout);
}

bool Indigo::GUI::GridSizer::add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column, const ItemLayout* layout)
{
	if (!widget)
		return false;

	bool ret = false;
	const OOBase::SharedPtr<Indigo::Render::GUI::Widget> render_widget(Sizer::render_widget(widget));
	return render_widget && render_call(OOBase::make_delegate(this,&GridSizer::do_add_widget),&ret,&render_widget,row,column,layout ? layout : default_layout()) && ret;
}

void Indigo::GUI::GridSizer::do_add_widget(bool* ret_val, const OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget, unsigned int row, unsigned int column, const ItemLayout* layout)
{
	OOBase::SharedPtr< ::GridSizer> sizer = render_sizer< ::GridSizer>();
	if (!sizer)
		*ret_val = false;
	else
		*ret_val = sizer->add_widget(*widget,row,column,*layout);
}

bool Indigo::GUI::GridSizer::add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const ItemLayout* layout)
{
	OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t> pos(width,height);
	OOBase::Pair<unsigned int,unsigned int> idx(row,column);

	bool ret = false;
	return render_call(OOBase::make_delegate(this,&GridSizer::do_add_spacer),&ret,&pos,&idx,layout ? layout : default_layout()) && ret;
}

void Indigo::GUI::GridSizer::do_add_spacer(bool* ret_val, OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>* pos, OOBase::Pair<unsigned int,unsigned int>* idx, const ItemLayout* layout)
{
	OOBase::SharedPtr< ::GridSizer> sizer = render_sizer< ::GridSizer>();
	if (!sizer)
		*ret_val = false;
	else
		*ret_val = sizer->add_spacer(pos->first,pos->second,idx->first,idx->second,*layout);
}

bool Indigo::GUI::GridSizer::remove_item(unsigned int row, unsigned int column)
{
	bool ret = false;
	return render_call(OOBase::make_delegate(this,&GridSizer::do_remove_item),&ret,row,column) && ret;
}

void Indigo::GUI::GridSizer::do_remove_item(bool* ret_val, unsigned int row, unsigned int column)
{
	OOBase::SharedPtr< ::GridSizer> sizer = render_sizer< ::GridSizer>();
	if (!sizer)
		*ret_val = false;
	else
		*ret_val = sizer->remove_item(row,column);
}

bool Indigo::GUI::GridSizer::remove_widget(const OOBase::SharedPtr<Widget>& widget)
{
	if (!widget)
		return false;

	bool ret = false;
	const OOBase::SharedPtr<Indigo::Render::GUI::Widget> render_widget(Sizer::render_widget(widget));
	return render_widget && render_call(OOBase::make_delegate(this,&GridSizer::do_remove_widget),&ret,&render_widget) && ret;
}

void Indigo::GUI::GridSizer::do_remove_widget(bool* ret_val, const OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget)
{
	OOBase::SharedPtr< ::GridSizer> sizer = render_sizer< ::GridSizer>();
	if (!sizer)
		*ret_val = false;
	else
		*ret_val = sizer->remove_widget(*widget);
}
