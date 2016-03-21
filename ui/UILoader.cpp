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
#include "UIImage.h"

namespace
{
	bool is_whitespace(char c)
	{
		return (c == ' ' || c == '\r' || c == '\n' || c == '\t');
	}
}

void Indigo::UILoader::syntax_error(const char* fmt, ...)
{
	va_list args;
	va_start(args,fmt);

	OOBase::ScopedArrayPtr<char> msg;
	int err = OOBase::vprintf(msg,fmt,args);

	va_end(args);

	if (!err)
		OOBase::Logger::log(OOBase::Logger::Error,"Syntax error: %s, at line %u, column %u",msg.get(),m_error_pos.m_line,m_error_pos.m_col);
	else
		OOBase_CallCriticalFailure(err);
}

#define SYNTAX_ERROR_RETURN(expr,ret_val) return (syntax_error expr,ret_val)

const char*& Indigo::UILoader::inc_p(const char*& p, const char* pe)
{
	if (p != pe)
		++p;

	if (p != pe && *p == '\n')
	{
		++m_error_pos.m_line;
		m_error_pos.m_col = 1;
	}

	return p;
}

void Indigo::UILoader::skip_to(char to, const char*& p, const char* pe)
{
	while (p != pe && *p != to)
		inc_p(p,pe);

	inc_p(p,pe);
}

void Indigo::UILoader::skip_whitespace(const char*& p, const char* pe)
{
	for (;p != pe;inc_p(p,pe))
	{
		if (*p == '#')
		{
			// Comment, skip to \n
			skip_to('\n',inc_p(p,pe),pe);
		}

		if (!is_whitespace(*p))
			break;
	}
}

bool Indigo::UILoader::ident(const char*& p, const char* pe, OOBase::ScopedString& i)
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

bool Indigo::UILoader::type_name(const char*& p, const char* pe, OOBase::ScopedString& t)
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

bool Indigo::UILoader::character(const char*& p, const char* pe, char c)
{
	skip_whitespace(p,pe);

	if (p == pe || *p != c)
		return false;

	inc_p(p,pe);
	return true;
}

bool Indigo::UILoader::parse_uint(const char*& p, const char* pe, unsigned int& u)
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

bool Indigo::UILoader::parse_uvec2(const char*& p, const char* pe, glm::uvec2& u)
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

bool Indigo::UILoader::parse_int(const char*& p, const char* pe, int& i)
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

bool Indigo::UILoader::parse_ivec2(const char*& p, const char* pe, glm::ivec2& i)
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

bool Indigo::UILoader::parse_string(const char*& p, const char* pe, OOBase::ScopedString& s)
{
	if (!character(p,pe,'"'))
		SYNTAX_ERROR_RETURN(("'\"' expected"),false);

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

	return true;
}

bool Indigo::UILoader::load(const char* name, unsigned int& zorder, UIGroup* parent)
{
	OOBase::SharedPtr<const char> res = m_resource.load<const char>(name);
	if (!res)
		LOG_ERROR_RETURN(("UIResource '%s' does not exist in bundle",name),false);

	m_error_pos.m_line = 1;
	m_error_pos.m_col = 1;

	const char* p = res.get();
	const char* pe = p + m_resource.size(name);

	for (OOBase::ScopedString type;p != pe;)
	{
		if (type_name(p,pe,type))
		{
			if (!load_top_level(p,pe,type,parent,zorder++))
				return false;
		}
		else if (p != pe)
			SYNTAX_ERROR_RETURN(("Type name expected"),false);
	}

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_top_level(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent, unsigned int zorder)
{
	if (type == "LAYER")
	{
		if (parent)
			LOG_WARNING(("Loading LAYER with parent?"));

		return load_layer(p,pe,zorder);
	}
	
	return load_child(p,pe,type,parent,NULL,zorder);
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_layer(const char*& p, const char* pe, unsigned int zorder)
{
	OOBase::ScopedString name;
	if (!ident(p,pe,name))
		SYNTAX_ERROR_RETURN(("Identifier expected"),OOBase::SharedPtr<UIWidget>());

	if (m_hashWidgets.find(name.c_str()))
		SYNTAX_ERROR_RETURN(("Duplicate identifier '%s'",name.c_str()),OOBase::SharedPtr<UIWidget>());

	OOBase::SharedPtr<UILayer> layer = OOBase::allocate_shared<UILayer,OOBase::ThreadLocalAllocator>();
	if (!layer)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

	if (!m_hashWidgets.insert(name.c_str(),layer))
		LOG_ERROR_RETURN(("Failed to insert layer into map: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

	if (!m_wnd->add_layer(layer,zorder))
		return OOBase::SharedPtr<UIWidget>();

	bool visible = true;
	bool sizer = false;
	zorder = 0;
	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "GRID_SIZER")
				{
					if (!load_grid_sizer(p,pe,layer.get(),name.c_str(),zorder))
						return OOBase::SharedPtr<UIWidget>();

					sizer = true;
				}
				else if (arg == "VISIBLE")
				{

				}
				else
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in LAYER",arg.c_str()),OOBase::SharedPtr<UIWidget>());

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
	}

	if (!load_children(p,pe,layer.get(),name.c_str(),zorder))
		return OOBase::SharedPtr<UIWidget>();
	
	if (sizer)
		layer->sizer()->size(layer->size());

	layer->show(visible);

	return layer;
}

bool Indigo::UILoader::load_grid_sizer(const char*& p, const char* pe, UIGroup* parent, const char* parent_name, unsigned int& zorder)
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
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in GRID_SIZER",arg.c_str()),false);

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),false);
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),false);
	}

	OOBase::SharedPtr<UIGridSizer> sizer = OOBase::allocate_shared<UIGridSizer,OOBase::ThreadLocalAllocator>();
	if (!sizer)
		LOG_ERROR_RETURN(("Failed to allocate sizer: %s",OOBase::system_error_text()),false);
	parent->sizer(sizer);
	
	if (character(p,pe,'{'))
	{
		if (character(p,pe,'['))
		{
			for (OOBase::ScopedString type;;)
			{
				unsigned int row,col,proportion = -1;

				if (!parse_uint(p,pe,row))
					SYNTAX_ERROR_RETURN(("Missing row argument in GRID_SIZER"),false);

				if (!character(p,pe,','))
					SYNTAX_ERROR_RETURN(("',' expected"),false);

				if (!parse_uint(p,pe,col))
					SYNTAX_ERROR_RETURN(("Missing column argument in GRID_SIZER"),false);

				if (character(p,pe,'/') && !parse_uint(p,pe,proportion))
					SYNTAX_ERROR_RETURN(("Missing proportion argument in GRID_SIZER"),false);

				if (!character(p,pe,']'))
					SYNTAX_ERROR_RETURN(("']' expected"),false);

				if (character(p,pe,'('))
				{
					unsigned int flags = UIGridSizer::align_centre | UIGridSizer::expand;
					if (type_name(p,pe,type))
					{
						flags = 0;
						for (;;)
						{
							if (type == "ALIGN_LEFT")
								flags = (flags & 0x3c) | UIGridSizer::align_left;
							else if (type == "ALIGN_RIGHT")
								flags = (flags & 0x3c) | UIGridSizer::align_right;
							else if (type == "ALIGN_HCENTRE")
								flags = (flags & 0x3c) | UIGridSizer::align_hcentre;
							else if (type == "ALIGN_BOTTOM")
								flags = (flags & 0x33) | UIGridSizer::align_bottom;
							else if (type == "ALIGN_TOP")
								flags = (flags & 0x33) | UIGridSizer::align_top;
							else if (type == "ALIGN_VCENTRE")
								flags = (flags & 0x33) | UIGridSizer::align_vcentre;
							else if (type == "ALIGN_CENTRE")
								flags = (flags & 0x30) | UIGridSizer::align_centre;
							else if (type == "EXPAND_HORIZ")
								flags = (flags & 0xF) | UIGridSizer::expand_horiz;
							else if (type == "EXPAND_VERT")
								flags = (flags & 0xF) | UIGridSizer::expand_vert;
							else if (type == "EXPAND")
								flags = (flags & 0xF) | UIGridSizer::expand;
							else
								SYNTAX_ERROR_RETURN(("Unexpected layout argument '%s'",type.c_str()),false);

							if (!character(p,pe,'|'))
								break;

							if (!type_name(p,pe,type))
								SYNTAX_ERROR_RETURN(("Argument expected"),false);
						}
					}

					if (!character(p,pe,')'))
						SYNTAX_ERROR_RETURN(("')' expected"),false);

					if (proportion == (unsigned int)-1)
						proportion = 1;

					if (!type_name(p,pe,type))
						SYNTAX_ERROR_RETURN(("Type name expected"),false);

					OOBase::SharedPtr<UIWidget> child = load_child(p,pe,type,parent,parent_name,zorder++);
					if (!child)
						return false;

					if (!sizer->add_widget(row,col,child,flags,proportion))
						return false;
				}
				else
				{
					if (!type_name(p,pe,type))
						SYNTAX_ERROR_RETURN(("Type name expected"),false);

					if (type == "SPACER")
					{
						if (proportion == (unsigned int)-1)
							proportion = 0;

						glm::uvec2 size(0);
						if (!parse_uvec2(p,pe,size))
							SYNTAX_ERROR_RETURN(("(width,height) expected"),false);

						if (!sizer->add_spacer(row,col,size,proportion))
							return false;
					}
					else
					{
						OOBase::SharedPtr<UIWidget> child = load_child(p,pe,type,parent,parent_name,zorder++);
						if (!child)
							return false;

						if (!sizer->add_widget(row,col,child))
							return false;
					}
				}

				if (!character(p,pe,','))
					break;

				if (!character(p,pe,'['))
					SYNTAX_ERROR_RETURN(("'[' expected"),false);
			}
		}

		if (!character(p,pe,'}'))
			SYNTAX_ERROR_RETURN(("'}' expected"),false);
	}
	
	return true;
}

bool Indigo::UILoader::load_children(const char*& p, const char* pe, UIGroup* parent, const char* parent_name, unsigned int& zorder)
{
	if (character(p,pe,'{'))
	{
		OOBase::ScopedString type;
		if (type_name(p,pe,type))
		{
			for (;;)
			{
				OOBase::SharedPtr<UIWidget> child = load_child(p,pe,type,parent,parent_name,zorder++);
				if (!child)
					return false;

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,type))
					SYNTAX_ERROR_RETURN(("Type name expected"),false);
			}
		}

		if (!character(p,pe,'}'))
			SYNTAX_ERROR_RETURN(("'}' expected"),false);
	}

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_child(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent, const char* parent_name, unsigned int zorder)
{
	OOBase::SharedPtr<UIWidget> ret;
	OOBase::ScopedString fq_name;

	OOBase::ScopedString name;
	if (ident(p,pe,name))
	{
		if (parent_name && (!fq_name.assign(parent_name) || !fq_name.append('.')))
			LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),ret);
		
		if (!fq_name.append(name.c_str(),name.length()))
			LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),ret);

		if (m_hashWidgets.find(fq_name.c_str()))
			SYNTAX_ERROR_RETURN(("Duplicate identifier '%s'",fq_name.c_str()),ret);
	}

	if (type == "BUTTON")
	{
		// TODO
	}
	else if (type == "IMAGE")
	{
		ret = load_uiimage(p,pe,parent);
	}
	else
		SYNTAX_ERROR_RETURN(("Unknown type name '%s'",type.c_str()),ret);

	if (ret)
	{
		if (!fq_name.empty())
		{
			if (!m_hashWidgets.insert(fq_name.c_str(),ret))
				LOG_ERROR_RETURN(("Failed to insert widget into map: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());
		}

		if (parent && !parent->add_widget(ret,zorder))
		{
			if (!fq_name.empty())
				m_hashWidgets.remove(fq_name.c_str());

			return OOBase::SharedPtr<UIWidget>();
		}
	}

	return ret;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_uiimage(const char*& p, const char* pe, UIGroup* parent)
{
	glm::ivec2 position(0);
	glm::uvec2 size(0);
	glm::vec4 colour(1.f);
	bool visible = false;

	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "VISIBLE")
				{
					visible = true;
				}
				else if (arg == "POSITION")
				{
					if (!parse_ivec2(p,pe,position))
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (arg == "SIZE")
				{
					if (!parse_uvec2(p,pe,size))
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (arg == "COLOUR")
				{

				}
				else
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in GRID_SIZER",arg.c_str()),OOBase::SharedPtr<UIWidget>());

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
	}

	OOBase::SharedPtr<Image> image = load_image(p,pe);
	if (!image)
		return OOBase::SharedPtr<UIWidget>();

	OOBase::SharedPtr<UIImage> uiimage = OOBase::allocate_shared<UIImage,OOBase::ThreadLocalAllocator>(parent,image,position,size,colour);
	if (!uiimage)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

	uiimage->show(visible);

	return uiimage;
}

OOBase::SharedPtr<Indigo::Image> Indigo::UILoader::load_image(const char*& p, const char* pe)
{
	OOBase::ScopedString image_name;
	if (!parse_string(p,pe,image_name))
		return OOBase::SharedPtr<Image>();

	if (image_name.empty())
		SYNTAX_ERROR_RETURN(("Image name expected"),OOBase::SharedPtr<Image>());

	image_hash_t::iterator i = m_hashImages.find(image_name.c_str());
	if (i)
		return i->second;

	OOBase::SharedPtr<Image> image = OOBase::allocate_shared<Image,OOBase::ThreadLocalAllocator>();
	if (!image)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),image);

	if (!image->load(m_resource,image_name.c_str(),4))
		return OOBase::SharedPtr<Image>();

	if (!m_hashImages.insert(image_name.c_str(),image))
		LOG_WARNING(("Failed to cache image: %s",OOBase::system_error_text()));

	return image;
}
