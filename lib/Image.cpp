///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// OOGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OOGL.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#include "Image.h"

static unsigned int read_uint16_le(const unsigned char* data)
{
	unsigned int v = data[1];
	v = (v << 8) | data[0];
	return v;
}

OOBase::SharedPtr<OOGL::Texture> OOGL::load_targa(const unsigned char* data, size_t len)
{
	const unsigned char* start = data;
	OOBase::SharedPtr<Texture> tex;

	if (len < 18)
		LOG_ERROR_RETURN(("Bad length for TGA data"),tex);

	if (data[1] != 0)
		LOG_ERROR_RETURN(("Color-mapped TGA images unsupported"),tex);

	if ((data[2] & 0x7) != 2)
		LOG_ERROR_RETURN(("Only RGB TGA images supported"),tex);

	if (data[17] & 0x20)
		LOG_ERROR_RETURN(("Inverted TGA images not supported"),tex);

	bool rle = (data[2] & 0x8) != 0;

	GLsizei width = read_uint16_le(data+12);
	GLsizei height = read_uint16_le(data+14);
	unsigned int bpp = data[16] / 8;

	if (!rle && len < 18 + data[0] + (bpp * width * height))
		LOG_ERROR_RETURN(("Incorrect length for TGA data"),tex);

	if (data[0])
		LOG_DEBUG(("Loading TGA file %.*s",(int)data[0],data));

	// Skip image ID
	data += 18 + data[0];

	if (bpp == 2)
		tex = OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(GL_TEXTURE_2D,1,GL_RGB5_A1,width,height);
	else if (bpp == 3)
		tex = OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(GL_TEXTURE_2D,1,GL_RGB8,width,height);
	else if (bpp == 4)
		tex = OOBase::allocate_shared<Texture,OOBase::ThreadLocalAllocator>(GL_TEXTURE_2D,1,GL_RGBA8,width,height);
	else
		LOG_ERROR_RETURN(("Unexpected BPP for TGA data"),tex);

	OOBase::SharedPtr<unsigned char> ptrBuf;
	if (rle)
	{
		// Unpack RLE
		unsigned char* wp = static_cast<unsigned char*>(OOBase::ThreadLocalAllocator::allocate(bpp * width * height,16));
		if (wp)
		{
			ptrBuf = OOBase::make_shared<unsigned char,OOBase::ThreadLocalAllocator>(wp);
			if (!ptrBuf)
				OOBase::ThreadLocalAllocator::free(wp);
		}
		if (!ptrBuf)
			LOG_ERROR_RETURN(("Out of memory"),OOBase::SharedPtr<Texture>());

		while (data < start + len)
		{
			unsigned char header = *data++;
			unsigned char plen = (header & 0x7F) + 1;

			if (header & 0x80)
			{
				// RLE packet
				while (plen--)
				{
					memcpy(wp,data,bpp);
					wp += bpp;
				}

				data += bpp;
			}
			else
			{
				// Raw packet
				plen *= bpp;
				memcpy(wp,data,plen);
				wp += plen;
				data += plen;
			}
		}

		data = ptrBuf.get();
	}

	GLint old_align = 0;
	glGetIntegerv(GL_UNPACK_ALIGNMENT,&old_align);

	if ((width * bpp) % old_align == 0)
		old_align = 0;

	if (bpp == 2)
	{
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,2);
		tex->sub_image(0,0,0,width,height,GL_BGRA,GL_UNSIGNED_SHORT_1_5_5_5_REV,data);
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,old_align);
	}
	else if (bpp == 3)
	{
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		tex->sub_image(0,0,0,width,height,GL_BGR,GL_UNSIGNED_BYTE,data);
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,old_align);
	}
	else
	{
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		tex->sub_image(0,0,0,width,height,GL_BGRA,GL_UNSIGNED_BYTE,data);
		if (old_align)
			glPixelStorei(GL_UNPACK_ALIGNMENT,old_align);
	}

	return tex;
}
