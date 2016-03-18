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

bool Indigo::UILoader::load(ResourceBundle& resource, const char* name, UIGroup* parent)
{
	OOBase::SharedPtr<const char> res = resource.load<const char>(name);
	if (!res)
		LOG_ERROR_RETURN(("UIResource '%s' does not exist in bundle",name),false);

	return load(res.get(),resource.size(name),parent);
}

bool Indigo::UILoader::load(const char* data, size_t len, UIGroup* parent)
{
	m_error_pos.m_line = 1;
	m_error_pos.m_col = 1;

	const char* p = data;
	const char* pe = p + len;
	while (p != pe)
	{
		if (!load_top_level(p,pe,parent))
			return false;
	}

	return true;
}

bool Indigo::UILoader::load_top_level(const char*& p, const char* pe, UIGroup* parent)
{
	OOBase::ScopedString name;
	if (!ident(p,pe,name))
		LOG_ERROR_RETURN(("Syntax error: identifier expected"),false);

	OOBase::ScopedString type;
	if (!type_name(p,pe,type))
		LOG_ERROR_RETURN(("Syntax error: type name expected"),false);

	if (type == "LAYER")
	{
		if (parent)
			LOG_WARNING(("Loading LAYER with parent?"));

		return load_layer(p,pe,name.c_str());
	}

	LOG_ERROR(("Syntax error: unknown type name %s",type.c_str()));
	return false;
}

bool Indigo::UILoader::load_layer(const char*& p, const char* pe, const char* name)
{
	if (m_hashWidgets.find(name))
		LOG_ERROR_RETURN(("Duplicate identifier: %s",name),false);

	OOBase::SharedPtr<UILayer> layer = OOBase::allocate_shared<UILayer,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	if (!load_children(p,pe,layer.get()))
		return false;

	if (!m_hashWidgets.insert(name,layer))
		LOG_ERROR_RETURN(("Failed to insert layer into map: %s",OOBase::system_error_text()),false);

	return true;
}

bool Indigo::UILoader::parse_uvec2(const char*& p, const char* pe, glm::uvec2& u)
{
	if (!character(p,pe,'('))
		LOG_ERROR_RETURN(("Syntax error: '(' expected"),false);

	if (!parse_uint(p,pe,u.x))
		LOG_ERROR_RETURN(("Syntax error: integer expected"),false);

	if (!character(p,pe,','))
		LOG_ERROR_RETURN(("Syntax error: ',' expected"),false);

	if (!parse_uint(p,pe,u.y))
		LOG_ERROR_RETURN(("Syntax error: integer expected"),false);

	if (!character(p,pe,')'))
		LOG_ERROR_RETURN(("Syntax error: ')' expected"),false);

	return true;
}

bool Indigo::UILoader::load_grid_sizer(const char*& p, const char* pe, UIGroup* parent)
{
	glm::uvec2 padding(0);
	for (OOBase::ScopedString arg;p != pe && type_name(p,pe,arg);skip_whitespace(p,pe))
	{
		if (arg == "PADDING")
		{
			if (!parse_uvec2(p,pe,padding))
				return false;
		}
		else
			LOG_ERROR_RETURN(("Unexpected argument in GRID_SIZER: %s",arg.c_str()),false);
	}

	if (!character(p,pe,'['))
		LOG_ERROR_RETURN(("Syntax error: '[' expected"),false);

	while (!character(p,pe,']'))
	{
		// TODO;
	}

	return true;
}

bool Indigo::UILoader::load_children(const char*& p, const char* pe, UIGroup* parent)
{
	if (character(p,pe,'{'))
	{
		while (!character(p,pe,'}'))
		{
			// TODO;
		}
	}

	OOBase::ScopedString type;
	if (type_name(p,pe,type))
	{
		if (type == "GRID_SIZER")
		{
			if (!load_grid_sizer(p,pe,parent))
				return false;
		}

		LOG_ERROR(("Syntax error: unknown sizer type: %s",type.c_str()));
		return false;
	}

	return true;
}
