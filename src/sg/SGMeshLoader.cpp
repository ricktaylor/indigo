///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Rick Taylor
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

#include "../../include/indigo/sg/SGMeshLoader.h"

#include "../../include/indigo/Resource.h"
#include "../../include/indigo/Image.h"

#include "../Common.h"

#define SYNTAX_ERROR_RETURN(expr,ret_val) return (syntax_error expr,ret_val)

namespace
{
	bool is_whitespace(char c)
	{
		return (c == ' ' || c == '\r' || c == '\n' || c == '\t');
	}
}

Indigo::SGMeshLoader::~SGMeshLoader()
{
	unload();
}

void Indigo::SGMeshLoader::unload()
{
}

void Indigo::SGMeshLoader::syntax_error(const char* fmt, ...)
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

OOBase::SharedPtr<Indigo::ResourceBundle> Indigo::SGMeshLoader::cd_resource(const char* res_name, OOBase::ScopedString& filename)
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
			if (!sub_dir.assign(res_name + start,slash - start - 1))
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

bool Indigo::SGMeshLoader::load(const OOBase::SharedPtr<ResourceBundle>& resource, const char* resource_name)
{
	m_resource = resource;

	OOBase::ScopedString filename;
	if (!cd_resource(resource_name,filename))
		return false;

	OOBase::SharedPtr<const char> res = resource->load<const char>(filename.c_str());
	if (!res)
		LOG_ERROR_RETURN(("Resource '%s' does not exist in bundle",resource_name),false);
		
	m_error_pos.m_line = 1;
	m_error_pos.m_col = 1;

	const char* p = res.get();
	const char* pe = p + m_resource->size(filename.c_str());
	bool ok = false;

	// Read header
	if (pe < p + 20 || strncmp(p,"# Inter-Quake Export",20) != 0)
		syntax_error("Invalid IQE file");
	else
	{
		p = p + 20;
		m_error_pos.m_line += 20;

		ok = true;
		OOBase::ScopedString cmd;

		while (ok && p != pe)
		{
			if (command(p,pe,cmd))
			{
				ok = load_command(p,pe,cmd);
			}
			else if (p != pe)
			{
				syntax_error("Command expected");
				ok = false;
			}
		}		
	}

	m_resource.reset();
	return ok;
}

const char*& Indigo::SGMeshLoader::inc_p(const char*& p, const char* pe)
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

void Indigo::SGMeshLoader::skip_whitespace(const char*& p, const char* pe)
{
	for (;p != pe;inc_p(p,pe))
	{
		if (*p == '\n' && p != (pe-1))
		{
			if (p[1] == '#')
			{
				// Comment, skip to \n
				do
				{
					inc_p(p,pe);
				}
				while (p != pe && *p != '\n');
			}
		}
		else if (!is_whitespace(*p))
			break;
	}
}

bool Indigo::SGMeshLoader::character(const char*& p, const char* pe, char c)
{
	skip_whitespace(p,pe);

	if (p == pe || *p != c)
		return false;

	inc_p(p,pe);
	return true;
}

bool Indigo::SGMeshLoader::parse_string(const char*& p, const char* pe, OOBase::ScopedString& s)
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

bool Indigo::SGMeshLoader::parse_float(const char*& p, const char* pe, float& i)
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


bool Indigo::SGMeshLoader::command(const char*& p, const char* pe, OOBase::ScopedString& i)
{
	skip_whitespace(p,pe);

	const char* start = p;
	for (;p != pe;inc_p(p,pe))
	{
		if (is_whitespace(*p))
			break;
	}

	if (p == start)
		return false;

	if (!i.assign(start,p - start))
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	return true;
}

bool Indigo::SGMeshLoader::load_command(const char*& p, const char* pe, const OOBase::ScopedString& cmd)
{
	if (cmd == "mesh")
	{
		OOBase::ScopedString mesh_name;
		if (!parse_string(p,pe,mesh_name))
			SYNTAX_ERROR_RETURN(("Expected mesh NAME"),false);
	}
	else if (cmd == "material")
	{
		OOBase::ScopedString material_name;
		if (!parse_string(p,pe,material_name))
			SYNTAX_ERROR_RETURN(("Expected material NAME"),false);
	}

	
	SYNTAX_ERROR_RETURN(("Unknown IQE command '%s'",cmd.c_str()),false);
}
