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

#include "UILoader.h"

#include "../core/Resource.h"

#include "UISizer.h"

namespace
{
	bool is_whitespace(char c)
	{
		return (c == ' ' || c == '\r' || c == '\n' || c == '\t');
	}
}

const char*& Indigo::UILoader::inc_p(const char*& p)
{
	if (*++p == '\n')
	{
		++m_error_pos.m_line;
		m_error_pos.m_col = 1;
	}
	return p;
}

void Indigo::UILoader::skip_to(char to, const char*& p, const char* pe)
{
	while (p != pe && *p != to)
		inc_p(p);

	if (p != pe)
		inc_p(p);
}

void Indigo::UILoader::skip_whitespace(const char*& p, const char* pe)
{
	for (;p != pe;inc_p(p))
	{
		if (*p == '#')
		{
			// Comment, skip to \n
			skip_to('\n',inc_p(p),pe);
		}

		if (!is_whitespace(*p))
			break;
	}
}

bool Indigo::UILoader::ident(const char*& p, const char* pe, OOBase::ScopedString& i)
{
	skip_whitespace(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p))
	{
		if (!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_",*p))
			break;
	}

	if (p == start)
		return false;

	if (!i.assign(start,p - start))
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	return true;
}

bool Indigo::UILoader::type_name(const char*& p, const char* pe, OOBase::ScopedString& t)
{
	skip_whitespace(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p))
	{
		if (!strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ_",*p))
			break;
	}

	if (p == start)
		return false;

	if (!t.assign(start,p - start))
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	return true;
}

bool Indigo::UILoader::character(const char*& p, const char* pe, char c)
{
	skip_whitespace(p,pe);

	if (p == pe || *p != c)
		return false;

	inc_p(p);
	return true;
}

bool Indigo::UILoader::parse_uint(const char*& p, const char* pe, unsigned int& u)
{
	u = 0;

	skip_whitespace(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p))
	{
		if (!strchr("0123456789",*p))
			break;

		u = (10 * u) + ('0' - *p);
	}

	return (p != start);
}

bool Indigo::UILoader::parse_uvec2(const char*& p, const char* pe, glm::uvec2& u)
{
	if (!character(p,pe,'('))
		LOG_ERROR_RETURN(("Syntax error: '(' expected"),false);

	if (!parse_uint(p,pe,u.x))
		LOG_ERROR_RETURN(("Syntax error: unsigned integer expected"),false);

	if (!character(p,pe,','))
		LOG_ERROR_RETURN(("Syntax error: ',' expected"),false);

	if (!parse_uint(p,pe,u.y))
		LOG_ERROR_RETURN(("Syntax error: unsigned integer expected"),false);

	if (!character(p,pe,')'))
		LOG_ERROR_RETURN(("Syntax error: ')' expected"),false);

	return true;
}

bool Indigo::UILoader::load(const char* name, UIGroup* parent)
{
	OOBase::SharedPtr<const char> res = m_resource.load<const char>(name);
	if (!res)
		LOG_ERROR_RETURN(("UIResource '%s' does not exist in bundle",name),false);

	m_error_pos.m_line = 1;
	m_error_pos.m_col = 1;

	const char* p = res.get();
	const char* pe = p + m_resource.size(name);
	while (p != pe)
	{
		if (!load_top_level(p,pe,parent))
			return false;
	}

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_top_level(const char*& p, const char* pe, UIGroup* parent)
{
	OOBase::SharedPtr<Indigo::UIWidget> ret;

	OOBase::ScopedString type;
	if (!type_name(p,pe,type))
		LOG_ERROR_RETURN(("Syntax error: type name expected"),ret);

	if (type == "LAYER")
	{
		if (parent)
			LOG_WARNING(("Loading LAYER with parent?"));

		return load_layer(p,pe);
	}
	
	return load_child(p,pe,type,parent,NULL);
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_layer(const char*& p, const char* pe)
{
	OOBase::SharedPtr<Indigo::UIWidget> ret;

	OOBase::ScopedString name;
	if (!ident(p,pe,name))
		LOG_ERROR_RETURN(("Syntax error: identifier expected"),ret);

	if (m_hashWidgets.find(name.c_str()))
		LOG_ERROR_RETURN(("Duplicate identifier: %s",name),ret);

	OOBase::SharedPtr<UILayer> layer = OOBase::allocate_shared<UILayer,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),ret);

	if (!m_hashWidgets.insert(name.c_str(),layer))
		LOG_ERROR_RETURN(("Failed to insert layer into map: %s",OOBase::system_error_text()),ret);

	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "GRID_SIZER")
				{
					if (!load_grid_sizer(p,pe,layer.get(),name.c_str()))
						return ret;
				}
				else
					LOG_ERROR_RETURN(("Unexpected argument in LAYER: %s",arg.c_str()),ret);

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					LOG_ERROR_RETURN(("Missing argument in LAYER: %s",arg.c_str()),ret);
			}
		}

		if (!character(p,pe,')'))
			LOG_ERROR_RETURN(("Syntax error: ')' expected"),ret);
	}

	if (!load_children(p,pe,layer.get(),name.c_str()))
		return ret;
	
	return layer;
}

bool Indigo::UILoader::load_grid_sizer(const char*& p, const char* pe, UIGroup* parent, const char* parent_name)
{
	glm::uvec2 padding(0);
	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "PADDING")
				{
					if (!parse_uvec2(p,pe,padding))
						return false;
				}
				else
					LOG_ERROR_RETURN(("Unexpected argument in GRID_SIZER: %s",arg.c_str()),false);

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					LOG_ERROR_RETURN(("Missing argument in GRID_SIZER: %s",arg.c_str()),false);
			}
		}

		if (!character(p,pe,')'))
			LOG_ERROR_RETURN(("Syntax error: ')' expected"),false);
	}

	OOBase::SharedPtr<UIGridSizer> sizer = OOBase::allocate_shared<UIGridSizer,OOBase::ThreadLocalAllocator>();
	if (!sizer)
		LOG_ERROR_RETURN(("Faiedl to allocate sizer: %s",OOBase::system_error_text()),false);
	parent->sizer(sizer);
	
	if (character(p,pe,'{'))
	{
		for (;;)
		{
			unsigned int row,col,proportion = -1;

			if (!character(p,pe,'['))
				LOG_ERROR_RETURN(("Syntax error: '[' expected"),false);

			if (!parse_uint(p,pe,row))
				LOG_ERROR_RETURN(("Missing row argument in GRID_SIZER"),false);

			if (!character(p,pe,','))
				LOG_ERROR_RETURN(("Syntax error: ',' expected"),false);

			if (!parse_uint(p,pe,col))
				LOG_ERROR_RETURN(("Missing col argument in GRID_SIZER"),false);

			if (character(p,pe,'/') && !parse_uint(p,pe,proportion))
				LOG_ERROR_RETURN(("Missing proportion argument in GRID_SIZER"),false);
			
			if (!character(p,pe,']'))
				LOG_ERROR_RETURN(("Syntax error: ']' expected"),false);

			if (character(p,pe,'('))
			{
				unsigned int flags = UIGridSizer::align_centre | UIGridSizer::expand;
				do
				{
					OOBase::ScopedString arg;
					if (!type_name(p,pe,arg))
						LOG_ERROR_RETURN(("Missing layout argument"),false);

					if (arg == "ALIGN_LEFT")
						flags = (flags & 0x3c) | UIGridSizer::align_left;
					else if (arg == "ALIGN_RIGHT")
						flags = (flags & 0x3c) | UIGridSizer::align_right;
					else if (arg == "ALIGN_HCENTRE")
						flags = (flags & 0x3c) | UIGridSizer::align_hcentre;
					else if (arg == "ALIGN_BOTTOM")
						flags = (flags & 0x33) | UIGridSizer::align_bottom;
					else if (arg == "ALIGN_TOP")
						flags = (flags & 0x33) | UIGridSizer::align_top;
					else if (arg == "ALIGN_VCENTRE")
						flags = (flags & 0x33) | UIGridSizer::align_vcentre;
					else if (arg == "ALIGN_CENTRE")
						flags = (flags & 0x30) | UIGridSizer::align_centre;
					else if (arg == "EXPAND_HORIZ")
						flags = (flags & 0xF) | UIGridSizer::expand_horiz;
					else if (arg == "EXPAND_VERT")
						flags = (flags & 0xF) | UIGridSizer::expand_vert;
					else if (arg == "EXPAND")
						flags = (flags & 0xF) | UIGridSizer::expand;
					else
						LOG_ERROR_RETURN(("Unexpected layout argument: %s",arg.c_str()),false);
				}
				while (character(p,pe,'|'));

				if (!character(p,pe,')'))
					LOG_ERROR_RETURN(("Syntax error: ')' expected"),false);

				if (proportion == -1)
					proportion = 1;

				OOBase::ScopedString type;
				if (!type_name(p,pe,type))
					LOG_ERROR_RETURN(("Syntax error: type name expected"),false);

				OOBase::SharedPtr<Indigo::UIWidget> child = load_child(p,pe,type,parent,parent_name);
				if (!child)
					return false;

				if (!sizer->add_widget(row,col,child,flags,proportion))
					return false;
			}
			else
			{
				OOBase::ScopedString type;
				if (!type_name(p,pe,type))
					LOG_ERROR_RETURN(("Missing type name"),false);

				if (type == "SPACER")
				{
					if (proportion == -1)
						proportion = 0;

					glm::uvec2 size(0);
					if (!parse_uvec2(p,pe,size))
						LOG_ERROR_RETURN(("Syntax error: (width,height) expected"),false);
					
					if (!sizer->add_spacer(row,col,size,proportion))
						return false;
				}
				else
				{
					OOBase::SharedPtr<Indigo::UIWidget> child = load_child(p,pe,type,parent,parent_name);
					if (!child)
						return false;

					if (!sizer->add_widget(row,col,child))
						return false;
				}
			}
			
			if (!character(p,pe,','))
				break;
		}

		if (!character(p,pe,'}'))
			LOG_ERROR_RETURN(("Syntax error: '}' expected"),false);
	}
	
	return true;
}

bool Indigo::UILoader::load_children(const char*& p, const char* pe, UIGroup* parent, const char* parent_name)
{
	if (character(p,pe,'{'))
	{
		for (;;)
		{
			OOBase::ScopedString type;
			if (!type_name(p,pe,type))
				LOG_ERROR_RETURN(("Syntax error: type name expected"),false);

			OOBase::SharedPtr<Indigo::UIWidget> child = load_child(p,pe,type,parent,parent_name);
			if (!child)
				return false;

			if (!character(p,pe,','))
				break;
		}

		if (!character(p,pe,'}'))
			LOG_ERROR_RETURN(("Syntax error: '}' expected"),false);
	}

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_child(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent, const char* parent_name)
{
	OOBase::SharedPtr<Indigo::UIWidget> ret;
	OOBase::ScopedString fq_name;

	OOBase::ScopedString name;
	if (ident(p,pe,name))
	{
		if (parent_name && (!fq_name.assign(parent_name) || !!fq_name.append('.')))
			LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),ret);
		
		if (!fq_name.append(name.c_str(),name.length()))
			LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),ret);

		if (m_hashWidgets.find(fq_name.c_str()))
			LOG_ERROR_RETURN(("Duplicate identifier: %s",fq_name.c_str()),ret);
	}

	if (type == "BUTTON")
	{
		// TODO
	}
	else if (type == "IMAGE")
	{
		// TODO
	}
	else
		LOG_ERROR_RETURN(("Syntax error: unknown type name %s",type.c_str()),ret);

	if (ret && !fq_name.empty())
	{
		if (!m_hashWidgets.insert(fq_name.c_str(),ret))
			LOG_ERROR_RETURN(("Failed to insert widget into map: %s",OOBase::system_error_text()),ret);
	}

	return ret;
}
