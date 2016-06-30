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

#include "../include/indigo/Parser.h"
#include "../include/indigo/Resource.h"
#include "../include/indigo/Image.h"
#include "../include/indigo/Font.h"

#include "Common.h"

namespace
{
	bool is_whitespace(char c)
	{
		return (c == ' ' || c == '\r' || c == '\n' || c == '\t');
	}

	unsigned int hex(char c)
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		if (c >= 'A' && c <= 'F')
			return 10 + (c - 'A');
		else
			return 10 + (c - 'a');
	}
}

Indigo::Parser::~Parser()
{
	unload();
}

void Indigo::Parser::syntax_error(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);

	OOBase::ScopedString msg;
	int err = msg.vprintf(fmt,args);

	va_end(args);

	if (!err)
		OOBase::Logger::log(OOBase::Logger::Error,"Syntax error: %s, at line %u, column %u",msg.c_str(),m_error_pos.m_line,m_error_pos.m_col);
	else
		OOBase_CallCriticalFailure(err);
}

const char*& Indigo::Parser::inc_p(const char*& p, const char* pe)
{
	if (p != pe)
	{
		++p;
		++m_error_pos.m_col;
	}

	if (p != pe && *p == '\n')
	{
		++m_error_pos.m_line;
		m_error_pos.m_col = 1;
	}

	return p;
}

void Indigo::Parser::skip_whitespace(const char*& p, const char* pe)
{
	for (;p != pe;inc_p(p,pe))
	{
		if (*p == '/' && p != (pe-1))
		{
			if (p[1] == '/')
			{
				// Comment, skip to \n
				do
				{
					inc_p(p,pe);
				}
				while (p != pe && *p != '\n');
			}
			else if (p[1] == '*')
			{
				/* Comment, skip to */
				do
				{
					do
					{
						inc_p(p,pe);
					}
					while (p != pe && *p != '*');

					inc_p(p,pe);
				}
				while (p != pe && *p != '/');
			}
			else
				break;
		}
		else if (!is_whitespace(*p))
			break;
	}
}

bool Indigo::Parser::ident(const char*& p, const char* pe, OOBase::ScopedString& i)
{
	skip_whitespace(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p,pe))
	{
		if (*p < 'A' || (*p > 'Z' && *p < '_') || (*p > '_' && *p < 'a') || *p > 'z')
			break;
	}

	if (p == start)
		return false;

	if (!i.assign(start,p - start))
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	return true;
}

bool Indigo::Parser::type_name(const char*& p, const char* pe, OOBase::ScopedString& t)
{
	skip_whitespace(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p,pe))
	{
		if (*p < 'A' || (*p > 'Z' && *p != '_'))
			break;
	}

	if (p == start)
		return false;

	if (!t.assign(start,p - start))
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	return true;
}

bool Indigo::Parser::character(const char*& p, const char* pe, char c)
{
	skip_whitespace(p,pe);

	if (p == pe || *p != c)
		return false;

	inc_p(p,pe);
	return true;
}

bool Indigo::Parser::parse_uint(const char*& p, const char* pe, unsigned int& u)
{
	u = 0;

	skip_whitespace(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p,pe))
	{
		if (*p < '0' || *p > '9')
			break;

		u = (10 * u) + (*p - '0');
	}

	return (p != start);
}

bool Indigo::Parser::parse_uvec2(const char*& p, const char* pe, glm::uvec2& u)
{
	if (!character(p,pe,'('))
		SYNTAX_ERROR_RETURN(("'(' expected"),false);

	if (!parse_uint(p,pe,u.x))
		SYNTAX_ERROR_RETURN(("Unsigned integer expected"),false);

	if (!character(p,pe,','))
		SYNTAX_ERROR_RETURN(("',' expected"),false);

	if (!parse_uint(p,pe,u.y))
		SYNTAX_ERROR_RETURN(("Unsigned integer expected"),false);

	if (!character(p,pe,')'))
		SYNTAX_ERROR_RETURN(("')' expected"),false);

	return true;
}

bool Indigo::Parser::parse_uvec4(const char*& p, const char* pe, glm::uvec4& u)
{
	if (!character(p,pe,'('))
		SYNTAX_ERROR_RETURN(("'(' expected"),false);

	if (!parse_uint(p,pe,u.x))
		SYNTAX_ERROR_RETURN(("Unsigned integer expected"),false);

	if (!character(p,pe,','))
		SYNTAX_ERROR_RETURN(("',' expected"),false);

	if (!parse_uint(p,pe,u.y))
		SYNTAX_ERROR_RETURN(("Unsigned integer expected"),false);

	if (!character(p,pe,','))
		SYNTAX_ERROR_RETURN(("',' expected"),false);

	if (!parse_uint(p,pe,u.z))
		SYNTAX_ERROR_RETURN(("Unsigned integer expected"),false);

	if (!character(p,pe,','))
		SYNTAX_ERROR_RETURN(("',' expected"),false);

	if (!parse_uint(p,pe,u.w))
		SYNTAX_ERROR_RETURN(("Unsigned integer expected"),false);

	if (!character(p,pe,')'))
		SYNTAX_ERROR_RETURN(("')' expected"),false);

	return true;
}

bool Indigo::Parser::parse_int(const char*& p, const char* pe, int& i)
{
	i = 0;

	skip_whitespace(p,pe);

	bool neg = false;
	if (*p == '-')
	{
		neg = true;
		inc_p(p,pe);
	}
	else if (*p == '+')
		inc_p(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p,pe))
	{
		if (*p < '0' || *p > '9')
			break;

		i = (10 * i) + (*p - '0');
	}

	if (neg && p != start)
		i = -i;

	return (p != start);
}

bool Indigo::Parser::parse_ivec2(const char*& p, const char* pe, glm::ivec2& i)
{
	if (!character(p,pe,'('))
		SYNTAX_ERROR_RETURN(("'(' expected"),false);

	if (!parse_int(p,pe,i.x))
		SYNTAX_ERROR_RETURN(("Integer expected"),false);

	if (!character(p,pe,','))
		SYNTAX_ERROR_RETURN(("',' expected"),false);

	if (!parse_int(p,pe,i.y))
		SYNTAX_ERROR_RETURN(("Integer expected"),false);

	if (!character(p,pe,')'))
		SYNTAX_ERROR_RETURN(("')' expected"),false);

	return true;
}

bool Indigo::Parser::parse_string(const char*& p, const char* pe, OOBase::ScopedString& s)
{
	if (character(p,pe,'"'))
	{
		const char* start = p;
		for (;p != pe;inc_p(p,pe))
		{
			if (*p == '"')
				break;

			if (*p == '\\')
			{
				if (p == (pe-1) || (p[1] != '"' && p[1] != '\\'))
					SYNTAX_ERROR_RETURN(("Invalid control character \\%c",p[1]),false);

				inc_p(p,pe);
			}
		}

		if (p == pe || *p != '"')
			SYNTAX_ERROR_RETURN(("'\"' expected"),false);

		const char* end = p;
		inc_p(p,pe);

		const char* q = start;
		for (;q < end;++q)
		{
			if (*q == '\\')
			{
				if (q > start && !s.append(start,q - start))
					LOG_ERROR_RETURN(("Failed to append to string: %s",OOBase::system_error_text()),false);

				if (q[1] == '"' && !s.append('"'))
					LOG_ERROR_RETURN(("Failed to append to string: %s",OOBase::system_error_text()),false);
				else if (q[1] == '\\' && !s.append('\\'))
					LOG_ERROR_RETURN(("Failed to append to string: %s",OOBase::system_error_text()),false);

				start = q + 1;
			}
		}

		if (q > start && !s.append(start,q - start))
			LOG_ERROR_RETURN(("Failed to append to string: %s",OOBase::system_error_text()),false);
	}

	return true;
}

bool Indigo::Parser::parse_float(const char*& p, const char* pe, float& i)
{
	skip_whitespace(p,pe);

	if (*p == '-' || *p == '+')
		inc_p(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p,pe))
	{
		if (*p < '0' || *p > '9')
			break;
	}

	if (*p == '.')
	{
		inc_p(p,pe);

		for (;p != pe;inc_p(p,pe))
		{
			if (*p < '0' || *p > '9')
				break;
		}
	}

	if (*p == 'E' || *p == 'e')
	{
		inc_p(p,pe);

		if (*p == '-' || *p == '+')
			inc_p(p,pe);

		for (;p != pe;inc_p(p,pe))
		{
			if (*p < '0' || *p > '9')
				break;
		}
	}

	if (p == start)
		return false;

	char* e = NULL;
	i = static_cast<float>(strtod(start,&e));

	return (p == e);
}

bool Indigo::Parser::parse_colour(const char*& p, const char* pe, glm::vec4& c)
{
	if (character(p,pe,'('))
	{
		if (!parse_float(p,pe,c.r))
			SYNTAX_ERROR_RETURN(("Float expected"),false);

		if (c.r < 0.f || c.r > 1.f)
			SYNTAX_ERROR_RETURN(("Value between 0.0 and 1.0 expected"),false);

		if (!character(p,pe,','))
			SYNTAX_ERROR_RETURN(("',' expected"),false);

		if (!parse_float(p,pe,c.g))
			SYNTAX_ERROR_RETURN(("Float expected"),false);

		if (c.g < 0.f || c.g > 1.f)
			SYNTAX_ERROR_RETURN(("Value between 0.0 and 1.0 expected"),false);

		if (!character(p,pe,','))
			SYNTAX_ERROR_RETURN(("',' expected"),false);

		if (c.b < 0.f || c.b > 1.f)
			SYNTAX_ERROR_RETURN(("Value between 0.0 and 1.0 expected"),false);

		if (!parse_float(p,pe,c.b))
			SYNTAX_ERROR_RETURN(("Float expected"),false);

		if (character(p,pe,','))
		{
			if (!parse_float(p,pe,c.a))
				SYNTAX_ERROR_RETURN(("Float expected"),false);

			if (c.a < 0.f || c.a > 1.f)
				SYNTAX_ERROR_RETURN(("Value between 0.0 and 1.0 expected"),false);
		}
		else
			c.a = 1.f;

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),false);

		return true;
	}
	else if (character(p,pe,'#'))
	{
		const char* start = p;
		for (;p != pe && (p - start) <= 8;inc_p(p,pe))
		{
			if (*p < '0' || (*p > '9' && *p < 'A') || (*p > 'F' && *p < 'a') || *p > 'f')
				break;
		}

		if (p - start >= 6)
		{
			c.r = (hex(start[0]) * 16 + hex(start[1])) / 255.f;
			c.g = (hex(start[2]) * 16 + hex(start[3])) / 255.f;
			c.b = (hex(start[4]) * 16 + hex(start[5])) / 255.f;
			c.a = 1.f;

			if (p - start == 8)
			{
				c.a = (hex(start[6]) * 16 + hex(start[7])) / 255.f;
				return true;
			}
			else if (p - start == 6)
				return true;
		}

		SYNTAX_ERROR_RETURN(("Invalid colour value"),false);
	}

	return false;
}

OOBase::SharedPtr<Indigo::ResourceBundle> Indigo::Parser::cd_resource(const char* res_name, OOBase::ScopedString& filename)
{
	OOBase::SharedPtr<ResourceBundle> prev_resource = m_resource;

	if (!filename.assign(res_name))
		LOG_ERROR_RETURN(("Failed to assign string: %s",OOBase::system_error_text()),prev_resource);

	for (size_t start = 0;;)
	{
		size_t slash = filename.find('/',start);
		if (slash == OOBase::ScopedString::npos)
		{
			if (!filename.assign(res_name + start))
				LOG_ERROR(("Failed to assign string: %s",OOBase::system_error_text()));
			break;
		}

		if (slash > 0)
		{
			OOBase::ScopedString sub_dir;
			if (!sub_dir.assign(res_name + start,slash - start))
				LOG_ERROR_RETURN(("Failed to assign string: %s",OOBase::system_error_text()),prev_resource);

			OOBase::SharedPtr<ResourceBundle> res = m_resource->sub_dir(sub_dir.c_str());
			if (!res)
				break;

			m_resource = res;
		}

		start = slash + 1;
	}

	return prev_resource;
}

OOBase::SharedPtr<Indigo::Image> Indigo::Parser::load_image(const OOBase::ScopedString& image_name)
{
	size_t hash = OOBase::Hash<const char*>::hash(image_name);
	image_hash_t::iterator i = m_hashImages.find(hash);
	if (i)
		return i->second;

	if (!m_resource->exists(image_name.c_str()))
		SYNTAX_ERROR_RETURN(("Missing image resource '%s'",image_name.c_str()),OOBase::SharedPtr<Image>());

	OOBase::SharedPtr<Image> image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
	if (!image)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),image);

	if (!image->load(*m_resource,image_name.c_str(),4))
		return OOBase::SharedPtr<Image>();

	if (!m_hashImages.insert(hash,image))
		LOG_WARNING(("Failed to cache image: %s",OOBase::system_error_text()));

	return image;
}

OOBase::SharedPtr<Indigo::Font> Indigo::Parser::load_font(const OOBase::ScopedString& font_name)
{
	size_t hash = OOBase::Hash<const char*>::hash(font_name);
	font_hash_t::iterator i = m_hashFonts.find(hash);
	if (i)
		return i->second;

	OOBase::ScopedString file_name;
	OOBase::SharedPtr<ResourceBundle> prev_res = cd_resource(font_name.c_str(),file_name);
	if (!prev_res)
		return OOBase::SharedPtr<Font>();

	if (!m_resource->exists(file_name.c_str()))
		SYNTAX_ERROR_RETURN(("Missing font resource '%s'",font_name.c_str()),OOBase::SharedPtr<Font>());

	OOBase::SharedPtr<Font> font = OOBase::allocate_shared<Font,OOBase::ThreadLocalAllocator>();
	if (!font)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),font);

	if (!font->load(*m_resource,file_name.c_str()))
		return OOBase::SharedPtr<Font>();

	m_resource = prev_res;

	if (!m_hashFonts.insert(hash,font))
		LOG_WARNING(("Failed to cache font: %s",OOBase::system_error_text()));

	return font;
}

bool Indigo::Parser::load(const OOBase::SharedPtr<ResourceBundle>& resource, const char* res_name)
{
	m_resource = resource;

	OOBase::ScopedString filename;
	if (!cd_resource(res_name,filename))
		return false;

	OOBase::SharedPtr<const char> res = resource->load<const char>(filename.c_str());
	if (!res)
		LOG_ERROR_RETURN(("Resource '%s' does not exist in bundle",res_name),false);
		
	m_error_pos.m_line = 1;
	m_error_pos.m_col = 1;

	const char* p = res.get();
	const char* pe = p + m_resource->size(filename.c_str());
	bool ok = true;

	for (OOBase::ScopedString type;ok && p != pe;)
	{
		if (type_name(p,pe,type))
		{
			ok = load_top_level(p,pe,type);
		}
		else if (p != pe)
		{
			syntax_error("Type name expected");
			ok = false;
		}
	}

	m_resource.reset();
	return ok;
}

void Indigo::Parser::unload()
{
	for (image_hash_t::iterator i=m_hashImages.begin();i;++i)
		i->second->unload();
}
