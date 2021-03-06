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

#ifndef INDIGO_ZIPRESOURCE_H_INCLUDED
#define INDIGO_ZIPRESOURCE_H_INCLUDED

#include "Resource.h"

#include <OOBase/String.h>

namespace Indigo
{
	namespace detail
	{
		class ZipFile;
	}

	class ZipResource : public ResourceBundle
	{
	public:
		ZipResource();
		ZipResource(const OOBase::SharedPtr<detail::ZipFile>& zip, const OOBase::String& prefix);

		bool open(const char* filename);
		bool is_open() const;

		OOBase::SharedPtr<ResourceBundle> sub_dir(const char* name) const;

		OOBase::uint64_t size(const char* name) const;
		bool exists(const char* name) const;

	private:
		OOBase::SharedPtr<detail::ZipFile> m_zip;
		OOBase::String m_prefix;
		
		OOBase::SharedPtr<const char> load_i(const char* name) const;
	};
}


#endif // INDIGO_ZIPRESOURCE_H_INCLUDED
