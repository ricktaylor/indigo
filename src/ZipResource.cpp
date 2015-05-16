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

#include "ZipResource.h"

namespace Indigo
{
	namespace detail
	{
		class ZipFile
		{
		public:
			int open(const char* filename);
			const void* load(const OOBase::String& prefix, const char* name, size_t start, size_t length = size_t(-1));
			OOBase::uint64_t size(const OOBase::String& prefix, const char* name);
			bool exists(const OOBase::String& prefix, const char* name);

		private:
			OOBase::File m_file;
		};
	}
}

int Indigo::detail::ZipFile::open(const char* filename)
{
	int err = m_file.open(filename,false);
	if (err)
		return err;

	OOBase::uint64_t len = m_file.length();
	if (len == OOBase::uint64_t(-1))
		return OOBase::system_error();

	// Find end of central directory record

	OOBase::uint64_t eof_cdr = OOBase::uint64_t(-1);

	// Step backwards looking for signatures...
	OOBase::ScopedArrayPtr<OOBase::uint8_t,OOBase::ThreadLocalAllocator,256> buf;
	for (OOBase::int64_t offset = len - 256;eof_cdr == OOBase::uint64_t(-1);)
	{
		if (offset < 0)
			offset = 0;

		OOBase::uint64_t p = m_file.seek(0,OOBase::File::seek_begin);
		if (p == OOBase::uint64_t(-1))
			return OOBase::system_error();

		size_t len = m_file.read(buf.get(),256);
		if (len == size_t(-1))
			return OOBase::system_error();

		// Scan forwards looking for signatures
		for (OOBase::uint8_t* c = buf.get(); c < buf.get() + len - 4; ++c)
		{
			static const OOBase::uint8_t END_OF_CDR[4] = { 0x50, 0x4b, 0x05, 0x06 };
			if (*c == END_OF_CDR[0] && memcmp(c,END_OF_CDR,4) == 0)
			{
				eof_cdr = p + (c - buf.get());
				break;
			}
		}

		if (offset > 0)
			offset -= 253;
		else
			break;
	}

	if (eof_cdr == OOBase::uint64_t(-1))
		LOG_ERROR_RETURN(("Failed to find end of central dictionary in zip %s",filename),EINVAL);

	m_file.seek(eof_cdr,OOBase::File::seek_begin);
	m_file.read(buf.get(),20);

	void* TODO; // TODO: Now check the central dictionary

	return 0;
}

const void* Indigo::detail::ZipFile::load(const OOBase::String& prefix, const char* name, size_t start, size_t length)
{
	return NULL;
}

OOBase::uint64_t Indigo::detail::ZipFile::size(const OOBase::String& prefix, const char* name)
{
	return 0;
}

bool Indigo::detail::ZipFile::exists(const OOBase::String& prefix, const char* name)
{
	return false;
}

Indigo::ZipResource::ZipResource()
{
}

Indigo::ZipResource::ZipResource(const OOBase::SharedPtr<detail::ZipFile>& zip, const OOBase::String& prefix) : m_zip(zip), m_prefix(prefix)
{
}

int Indigo::ZipResource::open(const char* filename)
{
	OOBase::SharedPtr<detail::ZipFile> zip = OOBase::allocate_shared<detail::ZipFile>();
	if (!zip)
		return ERROR_OUTOFMEMORY;

	int err = zip->open(filename);
	if (!err)
		zip.swap(m_zip);

	return err;
}

bool Indigo::ZipResource::is_open() const
{
	return m_zip;
}

Indigo::ZipResource Indigo::ZipResource::sub_dir(const char* prefix)
{
	OOBase::String new_prefix(m_prefix);
	if (!new_prefix.append(prefix))
		return ZipResource();

	return ZipResource(m_zip,new_prefix);
}

const void* Indigo::ZipResource::load(const char* name, size_t start, size_t length)
{
	if (!m_zip)
		return NULL;

	return m_zip->load(m_prefix,name,start,length);
}

OOBase::uint64_t Indigo::ZipResource::size(const char* name)
{
	if (!m_zip)
		return 0;

	return m_zip->size(m_prefix,name);
}

bool Indigo::ZipResource::exists(const char* name)
{
	if (!m_zip)
		return false;

	return m_zip->exists(m_prefix,name);;
}
