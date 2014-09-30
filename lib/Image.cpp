///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#include "Image.h"

static unsigned int read_uint16_le(const unsigned char* data)
{
	unsigned int v = data[1];
	v = (v << 8) | data[0];
	return v;
}

OOBase::SharedPtr<Indigo::Texture> Indigo::load_targa(const unsigned char* data, size_t len, bool mipmap)
{
	const unsigned char* start = data;
	OOBase::SharedPtr<Texture> tex;

	if (len < 18)
		LOG_ERROR_RETURN(("Bad length for TGA data"),tex);

	unsigned int image_id_len = *data++;
	unsigned int color_map = *data++;
	if (color_map != 0)
		LOG_ERROR_RETURN(("Color-mapped TGA images unsupported"),tex);
	
	unsigned int image_type = *data++;

	if ((image_type & 0x7) != 2)
		LOG_ERROR_RETURN(("Only RGB TGA images supported"),tex);

	bool rle = (image_type & 0x8) != 0;

	// Ignore color map and origin info
	data += 9;

	unsigned int width = read_uint16_le(data); data += 2;
	unsigned int height = read_uint16_le(data); data += 2;
	unsigned int bpp = *data++;
	bool top_down = (*data++ & 0x20) == 0x20;

	if (data > start || len < 18 + image_id_len + ((bpp/8) * width * height))
		LOG_ERROR_RETURN(("Incorrect length for TGA data"),tex);
	
	if (image_id_len)
		LOG_DEBUG(("Loading TGA file %.*s",(int)image_id_len,data));

	// Skip image ID
	data += image_id_len;

	return tex;
}