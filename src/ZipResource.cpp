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

namespace
{
	template <typename T>
	OOBase::uint32_t read_uint32(const T& buf, ptrdiff_t offset)
	{
		OOBase::uint32_t r = 0;
		memcpy(&r,&buf[offset],4);
		
#if (OOBASE_BYTE_ORDER == OOBASE_BIG_ENDIAN)
		OOBase::byte_swap(r);
#endif
		return r;
	}

	template <typename T>
	OOBase::uint16_t read_uint16(const T& buf, ptrdiff_t offset)
	{
		OOBase::uint16_t r = 0;
		memcpy(&r,&buf[offset],2);
		
#if (OOBASE_BYTE_ORDER == OOBASE_BIG_ENDIAN)
		OOBase::byte_swap(r);
#endif
		return r;
	}
}

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
			OOBase::Table<OOBase::String,OOBase::uint32_t> m_mapFiles;
		};
	}
}

int Indigo::detail::ZipFile::open(const char* filename)
{
	static const OOBase::uint8_t END_OF_CDR[4] = { 0x50, 0x4b, 0x05, 0x06 };
	static const OOBase::uint8_t CDR_HEADER[4] = { 0x50, 0x4b, 0x01, 0x02 };

	int err = m_file.open(filename,false);
	if (err)
		return err;

	OOBase::uint64_t len = m_file.length();
	if (len == OOBase::uint64_t(-1))
		return OOBase::system_error();

	// Step backwards looking for end of central directory record
	OOBase::uint32_t cdr_size = 0;

	OOBase::ScopedArrayPtr<OOBase::uint8_t,OOBase::ThreadLocalAllocator,256> buf;
	for (OOBase::int64_t offset = len - 256;!cdr_size;)
	{
		if (offset < 0)
			offset = 0;

		OOBase::uint64_t p = m_file.seek(offset,OOBase::File::seek_begin);
		if (p == OOBase::uint64_t(-1))
			return OOBase::system_error();

		size_t chunk_len = m_file.read(buf.get(),256);
		if (chunk_len == size_t(-1))
			return OOBase::system_error();

		// Scan forwards looking for signatures
		for (OOBase::uint8_t* c = buf.get(); c < buf.get() + chunk_len - 4; ++c)
		{
			if (*c == END_OF_CDR[0] && memcmp(c,END_OF_CDR,4) == 0)
			{
				OOBase::uint64_t eof_cdr = p + (c - buf.get());
				if (len - eof_cdr >= 22 && m_file.seek(eof_cdr,OOBase::File::seek_begin) == eof_cdr)
				{
					m_file.read(buf.get(),22);

					OOBase::uint16_t disk_no = read_uint16(buf,4);
					OOBase::uint16_t cdr_disk_no = read_uint16(buf,6);
					OOBase::uint16_t cdr_entries_disk = read_uint16(buf,8);
					OOBase::uint16_t cdr_entries = read_uint16(buf,10);
					OOBase::uint32_t cdr_size_i = read_uint32(buf,12);
					OOBase::uint32_t cdr_offset = read_uint32(buf,16);
					OOBase::uint16_t comments = read_uint16(buf,20);
					
					if (cdr_size_i >= 46 && (eof_cdr + 22 + comments == len) && 
						(disk_no == 0 && cdr_disk_no == 0 && cdr_entries_disk == cdr_entries) &&
						(cdr_offset + cdr_size_i <= eof_cdr))
					{
						if (m_file.seek(cdr_offset,OOBase::File::seek_begin) == cdr_offset)
						{
							m_file.read(buf.get(),46);
							if (memcmp(buf.get(),CDR_HEADER,4) == 0)
							{
								cdr_size = cdr_size_i;
								break;
							}
						}
					}
				}
			}
		}

		if (offset > 0)
			offset -= 253;
		else
			break;
	}

	if (!cdr_size)
		LOG_ERROR_RETURN(("Failed to find end of central dictionary in zip %s",filename),EINVAL);

	// Now loop reading file entries
	for (;;)
	{
		OOBase::uint16_t compression = read_uint16(buf,10);
		OOBase::uint16_t filename_len = read_uint16(buf,28);
		OOBase::uint16_t extra_len = read_uint16(buf,30);
		OOBase::uint16_t comments = read_uint16(buf,32);
		OOBase::uint16_t disk_no = read_uint16(buf,34);
		OOBase::uint32_t offset = read_uint32(buf,42);

		if (disk_no != 0)
			LOG_ERROR_RETURN(("Multi-disk zip file not supported: %s",filename),EINVAL);

		if (filename_len == 0)
			LOG_WARNING(("Ignoring empty filename in %s",filename));
		else
		{
			if (!buf.resize(filename_len))
				return ERROR_OUTOFMEMORY;

			if (m_file.read(buf.get(),filename_len) != filename_len)
				return OOBase::system_error();

			OOBase::String strFilename;
			if (!strFilename.assign(reinterpret_cast<char*>(buf.get()),filename_len))
				return ERROR_OUTOFMEMORY;

			if (m_mapFiles.insert(strFilename,offset) == m_mapFiles.end())
				return ERROR_OUTOFMEMORY;
		}

		if (cdr_size <= OOBase::uint32_t(filename_len) + extra_len + comments + 46)
			break;

		cdr_size -= 46 + filename_len + extra_len + comments;
		if (m_file.seek(extra_len + comments,OOBase::File::seek_current) == OOBase::uint64_t(-1))
			return OOBase::system_error();

		if (m_file.read(buf.get(),46) != 46)
			LOG_ERROR_RETURN(("Failed to read central dictionary header in zip %s",filename),EINVAL);

		if (memcmp(buf.get(),CDR_HEADER,4) != 0)
			LOG_ERROR_RETURN(("Invalid central dictionary header in zip %s",filename),EINVAL);
	}
	
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
