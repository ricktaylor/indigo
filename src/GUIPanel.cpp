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
				Sizer();

				struct Item
				{
					OOBase::WeakPtr<Widget> m_widget;
				};

				bool create(unsigned int rows, unsigned int columns);

				bool fit(const OOBase::SharedPtr<Panel>& panel);

				void add_rows(unsigned int rows);
				void add_columns(unsigned int columns);

				bool add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column);
				bool remove_widget(Widget* widget, unsigned int row, unsigned int column);

			private:
				unsigned int m_rows;
				unsigned int m_cols;

				typedef OOBase::Table<OOBase::Pair<unsigned int,unsigned int>,Item,OOBase::Less<OOBase::Pair<unsigned int,unsigned int> >,OOBase::ThreadLocalAllocator> items_t;
				items_t m_items;
			};
		}
	}
}

Indigo::Render::GUI::Sizer::Sizer() : m_rows(0), m_cols(0)
{
}

bool Indigo::Render::GUI::Sizer::create(unsigned int rows, unsigned int columns)
{
	m_rows = rows;
	m_cols = columns;

	return true;
}

void Indigo::Render::GUI::Sizer::add_rows(unsigned int rows)
{
	m_rows += rows;
}

void Indigo::Render::GUI::Sizer::add_columns(unsigned int columns)
{
	m_cols += columns;
}

bool Indigo::Render::GUI::Sizer::fit(const OOBase::SharedPtr<Panel>& panel)
{

}

bool Indigo::Render::GUI::Sizer::add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column)
{
	if (row >= m_rows)
		LOG_ERROR_RETURN(("Sizer: Adding item beyond max row"),false);
	if (column >= m_cols)
		LOG_ERROR_RETURN(("Sizer: Adding item beyond max column"),false);

	OOBase::Pair<unsigned int,unsigned int> pos(row,column);
	items_t::iterator i=m_items.find(pos);
	if (i)
	{
		i->second.m_widget = widget;
		return true;
	}

	Item item;
	item.m_widget = widget;
	return m_items.insert(pos,item);
}

bool Indigo::Render::GUI::Sizer::remove_widget(Widget* widget, unsigned int row, unsigned int column)
{
	if (widget)
	{
		for (items_t::iterator i=m_items.begin();i;++i)
		{
			if (i->second.m_widget.lock().get() == widget)
			{
				row = i->first.first;
				column = i->first.second;
				widget = NULL;
				break;
			}
		}
		if (widget)
			return false;
	}

	// Remove from row/column
}

bool Indigo::GUI::Sizer::create(unsigned int rows, unsigned int columns)
{
	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Sizer::do_create),&ret,rows,columns);
}

void Indigo::GUI::Sizer::do_create(bool* ret_val, unsigned int rows, unsigned int columns)
{
	OOBase::SharedPtr<Indigo::Render::GUI::Sizer> sizer = OOBase::allocate_shared<Indigo::Render::GUI::Sizer,OOBase::ThreadLocalAllocator>();
	if (!sizer)
	{
		LOG_ERROR(("Failed to allocate sizer: %s",OOBase::system_error_text()));
		*ret_val = false;
	}
	else if (!sizer->create(rows,columns))
		*ret_val = false;
	else
	{
		sizer.swap(m_sizer);
		*ret_val = true;
	}
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
	m_sizer->add_rows(rows);
}

bool Indigo::GUI::Sizer::add_column(unsigned int columns)
{
	return m_sizer && columns > 0 && render_call(OOBase::make_delegate(this,&Sizer::do_add_column),columns);
}

void Indigo::GUI::Sizer::do_add_column(unsigned int columns)
{
	m_sizer->add_columns(columns);
}

bool Indigo::GUI::Sizer::fit(Panel& panel)
{
	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_fit),&ret,static_cast<Widget*>(&panel));
}

void Indigo::GUI::Sizer::do_fit(bool* ret_val, Widget* panel)
{
	*ret_val = m_sizer->fit(panel->render_widget<Indigo::Render::GUI::Panel>());
}

bool Indigo::GUI::Sizer::add_widget(const OOBase::SharedPtr<Widget>& widget, unsigned int row, unsigned int column)
{
	if (!m_sizer)
		return false;

	bool ret = false;
	OOBase::SharedPtr<Render::GUI::Widget> render_widget(widget->render_widget<Render::GUI::Widget>());
	return render_call(OOBase::make_delegate(this,&Sizer::do_add_widget),&ret,&render_widget,row,column) && ret;
}

void Indigo::GUI::Sizer::do_add_widget(bool* ret_val, OOBase::SharedPtr<Indigo::Render::GUI::Widget>* widget, unsigned int row, unsigned int column)
{
	*ret_val = m_sizer->add_widget(*widget,row,column);
}

bool Indigo::GUI::Sizer::remove_widget(unsigned int row, unsigned int column)
{
	bool ret = false;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_remove_widget),&ret,static_cast<Indigo::Render::GUI::Widget*>(NULL),row,column) && ret;
}

bool Indigo::GUI::Sizer::remove_widget(const OOBase::SharedPtr<Widget>& widget)
{
	bool ret = false;
	unsigned int n = -1;
	return m_sizer && render_call(OOBase::make_delegate(this,&Sizer::do_remove_widget),&ret,widget->render_widget<Indigo::Render::GUI::Widget>().get(),n,n) && ret;
}

void Indigo::GUI::Sizer::do_remove_widget(bool* ret_val, Indigo::Render::GUI::Widget* widget, unsigned int row, unsigned int column)
{
	*ret_val = m_sizer->remove_widget(widget,row,column);
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
	OOBase::SharedPtr<Indigo::Render::GUI::Panel> panel = render_widget<Indigo::Render::GUI::Panel>();
	if (!panel)
		*fit = true;
	else if (!*fit || s->m_sizer->fit(panel))
	{
		*fit = true;
		panel->m_sizer = s->m_sizer;
	}
	else
		*fit = false;
}

bool Indigo::GUI::Panel::layout()
{
	return !m_sizer || m_sizer->fit(*this);
}
