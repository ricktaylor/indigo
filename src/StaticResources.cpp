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

#include "Common.h"

#include "../lib/Resource.h"

#include <OOBase/Singleton.h>

namespace
{
	class StaticResources : public OOGL::ResourceBundle
	{
	public:
		const unsigned char* load(const char* name, size_t start, size_t length = size_t(-1));
		size_t size(const char* name);
		bool exists(const char* name);
	};
}

#if defined(_WIN32)

#include "../resources/resource.h"

namespace
{
	struct RES
	{
		const char* name;
		LPCTSTR res_name;
		LPCTSTR type;
		HRSRC res
		const unsigned char* data;
		size_t length;
	};
	RES s_resources[] =
	{
		{ "Titillium-Regular.fnt", MAKEINTRESOURCE(IDR_TITILLIUM), MAKEINTRESOURCE(10), NULL, NULL, 0 },
		{ "Titillium-Regular_0.png", MAKEINTRESOURCE(IDR_TITILLIUM_0), MAKEINTRESOURCE(10), NULL, NULL, 0 },
	};

	const RES* find_resource(const char* name)
	{
		for (size_t i=0;i<sizeof(s_resources)/sizeof(s_resources[0]);++i)
		{
			if (strcmp(name,s_resources[i].name) == 0)
			{
				if (!s_resources[i].data)
				{
					if ((s_resources[i].res = FindResource(NULL,s_resources[i].name,s_resources[i].type))
					{
						s_resources[i].data = static_cast<const unsigned char*>(LockResource(LoadResource(NULL,s_resources[i].res)));
						s_resources[i].length = SizeofResource(NULL,s_resources[i].res);
					}
				}
				return &s_resources[i];
			}
		}
		return NULL;
	}
}

const unsigned char* StaticResources::load(const char* name, size_t start, size_t length)
{
	const RES* r = find_resource(name);
	if (!r || !r->data)
		return NULL;

	if (start > r->length)
		start = r->length;

	return r->data + start;
}

size_t StaticResources::size(const char* name)
{
	const RES* r = find_resource(name);
	return (r ? r->length : 0);
}

bool StaticResources::exists(const char* name)
{
	const RES* r = find_resource(name);
	return (r && r->data);
}

#else

namespace
{
	// Include the bin2c files here
	#include "Titillium-Regular_0.c"
	#include "Titillium-Regular.c"

	struct RES
	{
		const char* name;
		const unsigned char* data;
		size_t length;
	};
	const RES s_resources[] =
	{
		{ "Titillium-Regular.fnt", s_Titillium_Regular, sizeof(s_Titillium_Regular) },
		{ "Titillium-Regular.bin_0.png", s_Titillium_Regular_0, sizeof(s_Titillium_Regular_0) },
	};

	const RES* find_resource(const char* name)
	{
		for (size_t i=0;i<sizeof(s_resources)/sizeof(s_resources[0]);++i)
		{
			if (strcmp(name,s_resources[i].name) == 0)
				return &s_resources[i];
		}
		return NULL;
	}
}

const unsigned char* StaticResources::load(const char* name, size_t start, size_t length)
{
	const RES* r = find_resource(name);
	if (!r)
		return NULL;

	if (start > r->length)
		start = r->length;

	return r->data + start;
}

size_t StaticResources::size(const char* name)
{
	const RES* r = find_resource(name);
	return (r ? r->length : 0);
}

bool StaticResources::exists(const char* name)
{
	return find_resource(name) != NULL;
}

#endif

namespace Indigo
{
	OOGL::ResourceBundle& static_resources()
	{
		return OOBase::Singleton<StaticResources>::instance();
	}
}
