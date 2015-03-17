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

#ifndef INDIGO_IMAGE_H_INCLUDED
#define INDIGO_IMAGE_H_INCLUDED

#include "Texture.h"

namespace OOGL
{
	class Image : public OOBase::NonCopyable
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

	public:
		static OOBase::SharedPtr<Image> load(const char* filename, int components = 0);
		static OOBase::SharedPtr<Image> load(const unsigned char* buffer, int len, int components = 0);

		~Image();

		const int   m_width;
		const int   m_height;
		const int   m_components;
		const void* m_pixels;

	private:
		Image(int width, int height, int components, void* pixels);
	};
}

#endif // INDIGO_IMAGE_H_INCLUDED
