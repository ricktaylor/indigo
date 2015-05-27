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

#include "GUIPanel.h"
#include "Render.h"

namespace Indigo
{
	namespace Render
	{
		namespace GUI
		{
			class Sizer : public OOBase::NonCopyable
			{
			public:
				struct Item
				{
					OOBase::WeakPtr<Widget>        m_widget;
					OOBase::uint16_t                   m_width;
					OOBase::uint16_t                   m_height;
					Indigo::GUI::Sizer::ItemLayout m_layout;
				};

				unsigned int m_rows;
				unsigned int m_cols;

				typedef OOBase::Table<OOBase::Pair<unsigned int,unsigned int>,Item,OOBase::Less<OOBase::Pair<unsigned int,unsigned int> >,OOBase::ThreadLocalAllocator> items_t;
				items_t m_items;

				void fit(Panel& panel);
				bool layout(const Panel& panel);

				bool add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout);
				bool add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout);
				bool remove_item(unsigned int row, unsigned int column);
				bool remove_widget(Widget* widget);

				glm::u16vec2 get_best_size() const;
			};
		}
	}
}

glm::u16vec2 Indigo::Render::GUI::Sizer::get_best_size() const
{
	OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator> heights;
	OOBase::uint16_t max_width = 0;
	OOBase::uint16_t max_height = 0;

	for (items_t::const_iterator i=m_items.begin();i;++i)
	{
		glm::u16vec2 sz(0);
		glm::u16vec2 border(i->second.m_layout.m_border_left+i->second.m_layout.m_border_right,i->second.m_layout.m_border_top+i->second.m_layout.m_border_bottom);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<Widget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
				sz = widget->get_best_size() + border;
		}
		else
			sz = glm::u16vec2(i->second.m_width,i->second.m_height) + border;

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

		if (sz.x < widths[i->first.first])
			widths[i->first.first] = sz.x;

		if (sz.y < heights[i->first.second])
			heights[i->first.second] = sz.y;
	}

	for (OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
		max_width += *i;
	
	for (OOBase::Vector<OOBase::uint16_t,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		max_height += *i;
		
	return glm::u16vec2(max_width,max_height);
}

void Indigo::Render::GUI::Sizer::fit(Panel& panel)
{
	panel.size(get_best_size());
}

bool Indigo::Render::GUI::Sizer::layout(const Panel& panel)
{
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t> max_width(0,0);
	OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t> max_height(0,0);
	for (items_t::iterator i=m_items.begin();i;++i)
	{
		glm::u16vec2 sz(0);
		glm::u16vec2 border(i->second.m_layout.m_border_left+i->second.m_layout.m_border_right,i->second.m_layout.m_border_top+i->second.m_layout.m_border_bottom);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<Widget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
				sz = widget->get_best_size() + border;
		}
		else
			sz = glm::u16vec2(i->second.m_width,i->second.m_height) + border;

		while (widths.size() <= i->first.first)
		{
			if (!widths.push_back(OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>(0,0)))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}
		while (heights.size() <= i->first.second)
		{
			if (!heights.push_back(OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>(0,0)))
				LOG_ERROR_RETURN(("Failed to insert into vector: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);
		}

		if (sz.x < widths[i->first.first].first)
			widths[i->first.first].first = sz.x;

		if (sz.y < heights[i->first.second].first)
			heights[i->first.second].first = sz.y;

		widths[i->first.first].second += i->second.m_layout.m_proportion;
		heights[i->first.second].second += i->second.m_layout.m_proportion;
	}

	for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
	{
		max_width.first += i->first;
		max_width.second += i->second;
	}

	for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
	{
		max_height.first += i->first;
		max_height.second += i->second;
	}
	
	// Adjust the widths and heights
	glm::u16vec2 panel_size = panel.size();
	if (max_width.first < panel_size.x || max_height.first < panel_size.y)
		LOG_ERROR_RETURN(("Failed to fit children into panel"),false);

	float expand_x = static_cast<float>(panel_size.x - max_width.first) / max_width.second;
	for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>,OOBase::ThreadLocalAllocator>::iterator i=widths.begin();i;++i)
		i->first += static_cast<OOBase::uint16_t>(expand_x * i->second / max_width.second);

	float expand_y = static_cast<float>(panel_size.y - max_height.first) / max_height.second;
	for (OOBase::Vector<OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>,OOBase::ThreadLocalAllocator>::iterator i=heights.begin();i;++i)
		i->first += static_cast<OOBase::uint16_t>(expand_y * i->second / max_height.second);

	for (items_t::iterator i=m_items.begin();i;++i)
	{
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<Widget> widget(i->second.m_widget.lock());
			if (widget && widget->visible())
			{
				OOBase::uint16_t space_width = widths[i->first.first].first;
				OOBase::uint16_t space_height = heights[i->first.second].first;


			}
		}
	}
}

bool Indigo::Render::GUI::Sizer::add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout)
{
	if (row >= m_rows)
		LOG_ERROR_RETURN(("Sizer: Adding item beyond max row"),false);
	if (column >= m_cols)
		LOG_ERROR_RETURN(("Sizer: Adding item beyond max column"),false);

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

bool Indigo::Render::GUI::Sizer::add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout)
{
	if (row >= m_rows)
		LOG_ERROR_RETURN(("Sizer: Adding item beyond max row"),false);
	if (column >= m_cols)
		LOG_ERROR_RETURN(("Sizer: Adding item beyond max column"),false);

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

bool Indigo::Render::GUI::Sizer::remove_widget(Widget* widget)
{
	for (items_t::iterator i=m_items.begin();i;++i)
	{
		if (i->second.m_widget.lock().get() == widget)
			return m_items.erase(i);
	}
	return false;
}

bool Indigo::Render::GUI::Sizer::remove_item(unsigned int row, unsigned int column)
{
	return m_items.remove(OOBase::Pair<unsigned int,unsigned int>(row,column));
}

bool Indigo::GUI::Sizer::create(unsigned int rows, unsigned int columns)
{
	ItemLayout def_layout;
	def_layout.m_flags = ItemLayout::align_left | ItemLayout::align_top;
	def_layout.m_border_left = 3;
	def_layout.m_border_right = 3;
	def_layout.m_border_top = 3;
	def_layout.m_border_bottom = 3;
	def_layout.m_proportion = 0;

	return create(def_layout,rows,columns);
}

bool Indigo::GUI::Sizer::create(const ItemLayout& layout, unsigned int rows, unsigned int columns)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Sizer::do_create),&ret,rows,columns) || !ret)
		return false;

	m_default_layout = layout;
	return true;
}

void Indigo::GUI::Sizer::do_create(bool* ret_val, unsigned int rows, unsigned int columns)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Sizer> sizer = OOBase::allocate_shared<Indigo::Render::GUI::Sizer,OOBase::ThreadLocalAllocator>();
	if (!sizer)
	{
		LOG_ERROR(("Failed to allocate sizer: %s",OOBase::system_error_text()));
		*ret_val = false;
		return;
	}

	sizer->m_rows = rows;
	sizer->m_cols = columns;

	sizer.swap(m_sizer);
	*ret_val = true;
}

bool Indigo::GUI::Sizer::destroy()
{
	return !m_sizer || render_call(OOBase::make_delegate(this,&Sizer::do_destroy));
}

void Indigo::GUI::Sizer::do_destroy()
{
	m_sizer.reset();
}

bool Indigo::GUI::Sizer::add_row(unsigned int rows)
{
	return m_sizer && rows > 0 && render_call(OOBase::make_delegate(this,&Sizer::do_add_row),rows);
}

void Indigo::GUI::Sizer::do_add_row(unsigned int rows)
{
	m_sizer->m_rows += rows;
}

bool Indigo::GUI::Sizer::add_column(unsigned int columns)
{
	return m_sizer && columns > 0 && render_call(OOBase::make_delegate(this,&Sizer::do_add_column),columns);
}

void Indigo::GUI::Sizer::do_add_column(unsigned int columns)
{
	m_sizer->m_cols += columns;
}

bool Indigo::GUI::Sizer::fit(Panel& panel)
{
	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_fit),&ret,static_cast<Widget*>(&panel)) && ret;
}

void Indigo::GUI::Sizer::do_fit(bool* ret_val, Widget* panel)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> render_panel(panel->render_widget<Indigo::Render::GUI::Panel>());
	if (!render_panel)
		*ret_val = false;
	else
	{
		m_sizer->fit(*render_panel.get());
		*ret_val = true;
	}
}

bool Indigo::GUI::Sizer::layout(const Panel& panel)
{
	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_layout),&ret,static_cast<const Widget*>(&panel)) && ret;
}

void Indigo::GUI::Sizer::do_layout(bool* ret_val, const Widget* panel)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> render_panel(panel->render_widget<Indigo::Render::GUI::Panel>());
	if (!render_panel)
		*ret_val = false;
	else
		*ret_val = m_sizer->layout(*render_panel.get());
}

bool Indigo::GUI::Sizer::add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column, const ItemLayout* layout)
{
	if (!m_sizer || !widget)
		return false;

	bool ret = false;
	OOBase::SharedPtr<Render::GUI::Widget> render_widget(widget->render_widget<Render::GUI::Widget>());
	return render_widget && render_call(OOBase::make_delegate(this,&Sizer::do_add_widget),&ret,&render_widget,row,column,layout ? layout : &m_default_layout) && ret;
}

void Indigo::GUI::Sizer::do_add_widget(bool* ret_val, OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget, unsigned int row, unsigned int column, const ItemLayout* layout)
{
	*ret_val = m_sizer->add_widget(*widget,row,column,*layout);
}

bool Indigo::GUI::Sizer::add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const ItemLayout* layout)
{
	OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t> pos(width,height);
	OOBase::Pair<unsigned int,unsigned int> idx(row,column);

	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_add_spacer),&ret,&pos,&idx,layout ? layout : &m_default_layout) && ret;
}

void Indigo::GUI::Sizer::do_add_spacer(bool* ret_val, OOBase::Pair<OOBase::uint16_t,OOBase::uint16_t>* pos, OOBase::Pair<unsigned int,unsigned int>* idx, const ItemLayout* layout)
{
	*ret_val = m_sizer->add_spacer(pos->first,pos->second,idx->first,idx->second,*layout);
}

bool Indigo::GUI::Sizer::remove_item(unsigned int row, unsigned int column)
{
	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_remove_item),&ret,row,column) && ret;
}

void Indigo::GUI::Sizer::do_remove_item(bool* ret_val, unsigned int row, unsigned int column)
{
	*ret_val = m_sizer->remove_item(row,column);
}

bool Indigo::GUI::Sizer::remove_widget(const OOBase::SharedPtr<Widget>& widget)
{
	bool ret = false;
	return m_sizer && widget && render_call(OOBase::make_delegate(this,&Sizer::do_remove_widget),&ret,widget->render_widget<Indigo::Render::GUI::Widget>().get()) && ret;
}

void Indigo::GUI::Sizer::do_remove_widget(bool* ret_val, Indigo::Render::GUI::Widget* widget)
{
	*ret_val = m_sizer->remove_widget(widget);
}

void Indigo::Render::GUI::Panel::refresh_layout()
{
	if (m_sizer)
		m_sizer->layout(*this);
}

glm::u16vec2 Indigo::Render::GUI::Panel::get_best_size() const
{
	if (!m_sizer)
		return glm::u16vec2(0);

	return m_sizer->get_best_size();
}

OOBase::SharedPtr<Indigo::Render::GUI::Widget> Indigo::GUI::Panel::create_widget()
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> layer = OOBase::allocate_shared<Indigo::Render::GUI::Panel,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR(("Failed to allocate layer: %s",OOBase::system_error_text()));

	return layer;
}

bool Indigo::GUI::Panel::create(Widget* parent, const glm::u16vec2& pos, const glm::u16vec2& min_size)
{
	if (!Widget::create(parent,pos,min_size))
		return false;

	bool fit = true;
	if (m_sizer && (!render_call(OOBase::make_delegate(this,&Panel::do_sizer),m_sizer.get(),&fit) || !fit))
	{
		destroy();
		return false;
	}

	return true;
}

const OOBase::SharedPtr<Indigo::GUI::Sizer>& Indigo::GUI::Panel::sizer() const
{
	return m_sizer;
}

bool Indigo::GUI::Panel::sizer(const OOBase::SharedPtr<Sizer>& s, bool fit)
{
	if (!render_call(OOBase::make_delegate(this,&Panel::do_sizer),s.get(),&fit) || !fit)
		return false;

	m_sizer = s;
	return true;
}

void Indigo::GUI::Panel::do_sizer(Sizer* s, bool* fit)
{
	*fit = true;
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (panel)
	{
		if (*fit)
			s->m_sizer->fit(*panel.get());

		panel->m_sizer = s->m_sizer;
	}
}

bool Indigo::GUI::Panel::fit()
{
	return !m_sizer || m_sizer->fit(*this);
}

bool Indigo::GUI::Panel::layout()
{
	return !m_sizer || m_sizer->layout(*this);
}
