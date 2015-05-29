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

#ifndef OOGL_IMAGE_H_INCLUDED
#define OOGL_IMAGE_H_INCLUDED

#include "Texture.h"

namespace OOGL
{
	class ResourceBundle;

	class Image : public OOBase::NonCopyable
	{
	public:
		Image();
		~Image();

		bool load(const ResourceBundle& resource, const char* name, int components = 0);
		bool load(const unsigned char* buffer, int len, int components = 0);

		glm::uvec2 size() const
		{
			return glm::uvec2(m_width,m_height);
		}

		unsigned int components() const
		{
			return m_components;
		}

		const void* pixels() const
		{
			return m_pixels;
		}

		OOBase::SharedPtr<Texture> make_texture(GLenum internalFormat) const;
		
	private:
		int   m_width;
		int   m_height;
		int   m_components;
		void* m_pixels;
	};
}

#endif // OOGL_IMAGE_H_INCLUDED
