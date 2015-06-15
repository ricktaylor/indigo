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
		const void* load(const char* name, size_t start, size_t length) const;
		OOBase::uint64_t size(const char* name) const;
		bool exists(const char* name) const;
	};
}

#if defined(_WIN32)

#include "../resources/resource.h"

namespace
{
	struct RES
	{
		const char* name;
		const DWORD id;
		LPCTSTR type;
		HRSRC res;
		const unsigned char* data;
		OOBase::uint64_t length;
	};
	RES s_resources[] =
	{
		{ "Titillium-Regular.fnt", IDR_TITILLIUM, RT_RCDATA, NULL, NULL, 0 },
		{ "Titillium-Regular_0.png", IDB_TITILLIUM, RT_RCDATA, NULL, NULL, 0 },
		{ "menu_border.png", IDB_MENU_BORDER, RT_RCDATA, NULL, NULL, 0 },
		{ "NinePatch.vert", IDR_NINEPATCH_VS, RT_RCDATA, NULL, NULL, 0 },
		{ "NinePatch.frag", IDR_NINEPATCH_FS, RT_RCDATA, NULL, NULL, 0 },
		{ "Font_8bit.vert", IDR_FONT_8BIT_VS, RT_RCDATA, NULL, NULL, 0 },
		{ "Font_8bit.frag", IDR_FONT_8BIT_FS, RT_RCDATA, NULL, NULL, 0 },
	};

	const RES* find_resource(const char* name)
	{
		for (size_t i=0;i<sizeof(s_resources)/sizeof(s_resources[0]);++i)
		{
			if (strcmp(name,s_resources[i].name) == 0)
			{
				if (!s_resources[i].res && (s_resources[i].res = FindResource(NULL,MAKEINTRESOURCE(s_resources[i].id),s_resources[i].type)))
				{
					s_resources[i].data = static_cast<const unsigned char*>(LockResource(LoadResource(NULL,s_resources[i].res)));
					s_resources[i].length = SizeofResource(NULL,s_resources[i].res);
				}
				return &s_resources[i];
			}
		}
		LOG_ERROR(("Failed to find static resource %s",name));
		return NULL;
	}
}

const void* StaticResources::load(const char* name, size_t start, size_t length) const
{
	const RES* r = find_resource(name);
	if (!r || !r->data)
		return NULL;

	if (start > r->length)
		start = r->length;

	return r->data + start;
}

OOBase::uint64_t StaticResources::size(const char* name) const
{
	const RES* r = find_resource(name);
	return (r ? r->length : 0);
}

bool StaticResources::exists(const char* name) const
{
	const RES* r = find_resource(name);
	return (r && r->data);
}

#else

namespace
{
	// Include the bin2c files here
	#include "Titillium-Regular_0.png.h"
	#include "Titillium-Regular.fnt.h"
	#include "menu_border.png.h"
	#include "NinePatch.vert.h"
	#include "NinePatch.frag.h"
	#include "Font_8bit.vert.h"
	#include "Font_8bit.frag.h"

	struct RES
	{
		const char* name;
		const unsigned char* data;
		OOBase::uint64_t length;
	};
	const RES s_resources[] =
	{
		{ "Titillium-Regular.fnt", s_Titillium_Regular, sizeof(s_Titillium_Regular) },
		{ "Titillium-Regular_0.png", s_Titillium_Regular_0, sizeof(s_Titillium_Regular_0) },
		{ "menu_border.png", s_menu_border, sizeof(s_menu_border) },
		{ "NinePatch.vert", s_NinePatch_vert, sizeof(s_NinePatch_vert) },
		{ "NinePatch.frag", s_NinePatch_frag, sizeof(s_NinePatch_frag) },
		{ "Font_8bit.vert", s_Font_8bit_vert, sizeof(s_Font_8bit_vert) },
		{ "Font_8bit.frag", s_Font_8bit_frag, sizeof(s_Font_8bit_frag) }
	};

	const RES* find_resource(const char* name)
	{
		for (size_t i=0;i<sizeof(s_resources)/sizeof(s_resources[0]);++i)
		{
			if (strcmp(name,s_resources[i].name) == 0)
				return &s_resources[i];
		}
		LOG_ERROR(("Failed to find static resource %s",name));
		return NULL;
	}
}

const void* StaticResources::load(const char* name, size_t start, size_t length) const
{
	const RES* r = find_resource(name);
	if (!r)
		return NULL;

	if (start > r->length)
		start = r->length;

	return r->data + start;
}

OOBase::uint64_t StaticResources::size(const char* name) const
{
	const RES* r = find_resource(name);
	return (r ? r->length : 0);
}

bool StaticResources::exists(const char* name) const
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
