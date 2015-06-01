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
					OOBase::uint16_t               m_width;
					OOBase::uint16_t               m_height;
					Indigo::GUI::Sizer::ItemLayout m_layout;
				};

				typedef OOBase::Table<OOBase::Pair<unsigned int,unsigned int>,Item,OOBase::Less<OOBase::Pair<unsigned int,unsigned int> >,OOBase::ThreadLocalAllocator> items_t;
				items_t m_items;

				glm::u16vec2 m_padding;

				bool fit(Panel& panel);
				bool layout(const Panel& panel);

				bool add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout);
				bool add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout);
				bool remove_item(unsigned int row, unsigned int column);
				bool remove_widget(Widget* widget);

				glm::u16vec2 ideal_size() const;

			private:
				bool get_extents(OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
							OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
							OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
							OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height);

				bool do_layout(const Panel& panel,
							OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
							OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
							OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
							OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height);
			};
		}
	}
}

glm::u16vec2 Indigo::Render::GUI::Sizer::ideal_size() const
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
			OOBase::SharedPtr<Widget> widget(i->second.m_widget.lock());
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

		if (sz.x < widths[i->first.first])
			widths[i->first.first] = sz.x;

		if (sz.y < heights[i->first.second])
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

bool Indigo::Render::GUI::Sizer::get_extents(OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
			OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
			OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
			OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height)
{
	for (items_t::iterator i=m_items.begin();i;++i)
	{
		glm::u16vec2 sz(0);
		if (i->second.m_widget)
		{
			OOBase::SharedPtr<Widget> widget(i->second.m_widget.lock());
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

		if (sz.x < widths[i->first.first].first)
			widths[i->first.first].first = sz.x;

		if (sz.y < heights[i->first.second].first)
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

bool Indigo::Render::GUI::Sizer::fit(Panel& panel)
{
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_width(0,0);
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_height(0,0);
	if (!get_extents(widths,heights,ideal_width,ideal_height))
		return false;

	glm::u16vec2 size = panel.size();
	if (panel.client_size(ideal_size()) == size)
		return true;

	return do_layout(panel,widths,heights,ideal_width,ideal_height);
}

bool Indigo::Render::GUI::Sizer::layout(const Panel& panel)
{
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> widths;
	OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator> heights;
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_width(0,0);
	OOBase::Pair<OOBase::uint16_t,unsigned int> ideal_height(0,0);
	if (!get_extents(widths,heights,ideal_width,ideal_height))
		return false;

	return do_layout(panel,widths,heights,ideal_width,ideal_height);
}

bool Indigo::Render::GUI::Sizer::do_layout(const Panel& panel,
		OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& widths,
		OOBase::Vector<OOBase::Pair<OOBase::uint16_t,unsigned int>,OOBase::ThreadLocalAllocator>& heights,
		OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_width,
		OOBase::Pair<OOBase::uint16_t,unsigned int>& ideal_height)
{
	// Adjust the widths and heights
	glm::u16vec2 panel_size = panel.client_size();
	if (panel_size.x != ideal_width.first)
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
	if (panel_size.y != ideal_height.first)
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
						OOBase::SharedPtr<Widget> widget(i->second.m_widget.lock());
						if (widget && widget->visible())
						{
							glm::i16vec2 item_pos(cell_pos);
							glm::u16vec2 item_size(widget->ideal_size());

							// Expand if required
							if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::expand)
							{
								if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::expand_horiz)
									item_size.x = cell_size.x;
								if (i->second.m_layout.m_flags & Indigo::GUI::Sizer::ItemLayout::expand_vert)
									item_size.y = cell_size.y;

								item_size = widget->size(item_size);
							}

							// Align if required
							if (item_size != cell_size)
							{
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

bool Indigo::Render::GUI::Sizer::add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout)
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

bool Indigo::Render::GUI::Sizer::add_spacer(OOBase::uint16_t width, OOBase::uint16_t height, unsigned int row, unsigned int column, const Indigo::GUI::Sizer::ItemLayout& layout)
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

bool Indigo::GUI::Sizer::create(OOBase::uint16_t hspace, OOBase::uint16_t vspace)
{
	ItemLayout def_layout;
	def_layout.m_flags = ItemLayout::expand | ItemLayout::align_left | ItemLayout::align_top;
	def_layout.m_proportion = 0;
	return create(hspace,vspace,def_layout);
}

bool Indigo::GUI::Sizer::create(OOBase::uint16_t hspace, OOBase::uint16_t vspace, const ItemLayout& layout)
{
	const glm::u16vec2 space(hspace,vspace);
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Sizer::do_create),&ret,&space) || !ret)
		return false;

	m_default_layout = layout;
	return true;
}

void Indigo::GUI::Sizer::do_create(bool* ret_val, const glm::u16vec2* space)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Sizer> sizer = OOBase::allocate_shared<Indigo::Render::GUI::Sizer,OOBase::ThreadLocalAllocator>();
	if (!sizer)
	{
		LOG_ERROR(("Failed to allocate sizer: %s",OOBase::system_error_text()));
		*ret_val = false;
		return;
	}

	sizer->m_padding = *space;

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
		*ret_val = m_sizer->fit(*render_panel.get());
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

void Indigo::Render::GUI::Panel::layout()
{
	if (m_sizer)
		m_sizer->layout(*this);
}

glm::u16vec2 Indigo::Render::GUI::Panel::ideal_size() const
{
	glm::u16vec2 sz(Widget::ideal_size());
	if (m_sizer)
	{
		glm::u16vec2 sz2 = m_sizer->ideal_size();
		if (sz2.x > sz.x)
			sz.x = sz2.x;

		if (sz2.y > sz.y)
			sz.y = sz2.y;
	}

	return sz;
}

bool Indigo::Render::GUI::Panel::add_child(const OOBase::SharedPtr<Widget>& child)
{
	return m_children.push_back(child);
}

void Indigo::Render::GUI::Panel::remove_child(const OOBase::SharedPtr<Widget>& child)
{
	m_children.erase(child);
}

void Indigo::Render::GUI::Panel::draw(OOGL::State& glState, const glm::mat4& mvp)
{
	if (m_background)
		;

	for (OOBase::Vector<OOBase::SharedPtr<Widget>,OOBase::ThreadLocalAllocator>::iterator i=m_children.begin();i;++i)
	{
		if ((*i)->visible())
		{
			glm::i16vec2 p = (*i)->position();
			glm::mat4 view = glm::translate(glm::mat4(1),glm::vec3(p.x,p.y,0));
			(*i)->draw(glState,mvp * view);
		}
	}
}

glm::u16vec4 Indigo::Render::GUI::Panel::borders() const
{
	return m_borders;
}

glm::u16vec2 Indigo::Render::GUI::Panel::client_size() const
{
	glm::u16vec2 client_sz(size());
	if (client_sz.x > m_borders.x + m_borders.z)
		client_sz.x -= m_borders.x + m_borders.z;
	else
		client_sz.x = 0;

	if (client_sz.y > m_borders.y + m_borders.w)
		client_sz.y -= m_borders.y + m_borders.w;
	else
		client_sz.y = 0;

	return client_sz;
}

glm::u16vec2 Indigo::Render::GUI::Panel::client_size(const glm::u16vec2& sz)
{
	glm::u16vec2 client_sz(sz.x + m_borders.x + m_borders.z, sz.y + m_borders.y + m_borders.w);
	client_sz = size(client_sz);

	if (client_sz.x > m_borders.x + m_borders.z)
		client_sz.x -= m_borders.x + m_borders.z;
	else
		client_sz.x = 0;

	if (client_sz.y > m_borders.y + m_borders.w)
		client_sz.y -= m_borders.y + m_borders.w;
	else
		client_sz.y = 0;

	return client_sz;
}

OOBase::SharedPtr<Indigo::Render::GUI::Widget> Indigo::GUI::Panel::create_widget()
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> layer = OOBase::allocate_shared<Indigo::Render::GUI::Panel,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR(("Failed to allocate layer: %s",OOBase::system_error_text()));

	return layer;
}

bool Indigo::GUI::Panel::create(Widget* parent, const glm::i16vec2& pos, const glm::u16vec2& min_size)
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

bool Indigo::GUI::Panel::sizer(const OOBase::SharedPtr<Sizer>& s)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Panel::do_sizer),s.get(),&ret) || !ret)
		return false;

	m_sizer = s;
	return true;
}

void Indigo::GUI::Panel::do_sizer(Sizer* s, bool* ret_val)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (!panel)
		*ret_val = false;
	else
	{
		panel->m_sizer = s->m_sizer;
		*ret_val = true;
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

const OOBase::SharedPtr<Indigo::Image>& Indigo::GUI::Panel::background() const
{
	return m_background;
}

bool Indigo::GUI::Panel::background(const OOBase::SharedPtr<Image>& image)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Panel::do_background),image.get(),&ret) || !ret)
		return false;

	m_background = image;
	return true;
}

void Indigo::GUI::Panel::do_background(Image* image, bool* ret_val)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (!panel)
		*ret_val = false;
	else
	{
		OOBase::SharedPtr<OOGL::Image> bg = image->render_image();
		if (bg && bg->valid())
			bg.swap(panel->m_background);

		*ret_val = true;
	}
}

glm::u16vec4 Indigo::GUI::Panel::borders() const
{
	glm::u16vec4 borders(0);
	render_call(OOBase::make_delegate(const_cast<Panel*>(this),&Panel::do_get_borders),&borders);
	return borders;
}

void Indigo::GUI::Panel::do_get_borders(glm::u16vec4* borders)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (panel)
		*borders = panel->m_borders;
}

bool Indigo::GUI::Panel::borders(OOBase::uint16_t left, OOBase::uint16_t top, OOBase::uint16_t right, OOBase::uint16_t bottom)
{
	glm::u16vec4 borders(left,top,right,bottom);
	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Panel::do_set_borders),&borders,&ret) && ret;
}

void Indigo::GUI::Panel::do_set_borders(glm::u16vec4* borders, bool* ret_val)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (!panel)
		*ret_val = false;
	else
	{
		panel->m_borders = *borders;
		*ret_val = true;
	}
}
