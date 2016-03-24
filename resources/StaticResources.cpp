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
#include "../core/Resource.h"

namespace
{
	class StaticResources : public Indigo::ResourceBundle
	{
	public:
		OOBase::SharedPtr<const char> load_i(const char* name) const;
		OOBase::uint64_t size(const char* name) const;
		bool exists(const char* name) const;
	};

	class NullShare : public OOBase::detail::SharedCountBase
	{
	public:
		NullShare() : OOBase::detail::SharedCountBase() {}
		void dispose() {}
		void destroy() {}
	};
}

#if defined(_WIN32)

#include "./resource.h"

namespace
{
	struct RES
	{
		const char* name;
		const DWORD id;
		LPCTSTR type;
		HRSRC res;
		const void* data;
		OOBase::uint64_t length;
	};
	RES s_resources[] =
	{
		{ "2d_colour.vert", IDR_2D_COLOUR_VS, RT_RCDATA, NULL, NULL, 0 },
		{ "colour_blend.frag", IDR_COLOUR_BLEND_FS, RT_RCDATA, NULL, NULL, 0 },
		{ "alpha_blend.frag", IDR_ALPHA_BLEND_FS, RT_RCDATA, NULL, NULL, 0 }
	};

	const RES* find_resource(const char* name)
	{
		for (size_t i=0;i<sizeof(s_resources)/sizeof(s_resources[0]);++i)
		{
			if (strcmp(name,s_resources[i].name) == 0)
			{
				if (!s_resources[i].res && (s_resources[i].res = FindResource(NULL,MAKEINTRESOURCE(s_resources[i].id),s_resources[i].type)))
				{
					s_resources[i].data = LockResource(LoadResource(NULL,s_resources[i].res));
					s_resources[i].length = SizeofResource(NULL,s_resources[i].res);
				}
				return &s_resources[i];
			}
		}
		LOG_ERROR(("Failed to find static resource %s",name));
		return NULL;
	}
}

#else

namespace
{
	// Include the bin2c files here
	#include "./resources/2d_colour.vert.h"
	#include "./resources/alpha_blend.frag.h"
	#include "./resources/colour_blend.frag.h"

	struct RES
	{
		const char* name;
		const void* data;
		OOBase::uint64_t length;
	};
	const RES s_resources[] =
	{
		{ "2d_colour.vert", s_2d_colour_vert, sizeof(s_2d_colour_vert) },
		{ "alpha_blend.frag", s_alpha_blend_frag, sizeof(s_alpha_blend_frag) },
		{ "colour_blend.frag", s_colour_blend_frag, sizeof(s_colour_blend_frag) }
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

#endif

OOBase::SharedPtr<const char> StaticResources::load_i(const char* name) const
{
	OOBase::SharedPtr<const char> ret;
	const RES* r = find_resource(name);
	if (r && r->data)
	{
		NullShare* ns = NULL;
		if (!OOBase::CrtAllocator::allocate_new(ns))
			LOG_ERROR(("Failed to find allocate: %s",OOBase::system_error_text()));
		else
		{
			ret = OOBase::make_shared(static_cast<const char*>(r->data),ns);
			if (!ret)
			{
				LOG_ERROR(("Failed to find allocate: %s",OOBase::system_error_text()));
				OOBase::CrtAllocator::delete_free(ns);
			}
		}
	}

	return ret;
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

namespace Indigo
{
	ResourceBundle& static_resources()
	{
		return OOBase::Singleton<StaticResources>::instance();
	}
}
