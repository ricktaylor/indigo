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

#include "../lib/Texture.h"
#include "Resource.h"

namespace Indigo
{
	class ResourceBundle;

	class Image : public OOBase::NonCopyable
	{
	public:
		Image();
		virtual ~Image();

		bool valid() const;

		bool load(const ResourceBundle& resource, const char* name, int components = 0);
		virtual bool load(const unsigned char* buffer, int len, int components = 0);

		void unload();

		glm::uvec2 size() const
		{
			return glm::uvec2(m_width,m_height);
		}

		unsigned int components() const
		{
			return m_components;
		}

		OOBase::SharedPtr<OOGL::Texture> make_texture(GLenum internalFormat) const;
		
	protected:
		int   m_width;
		int   m_height;
		int   m_components;
		void* m_pixels;
	};
}

#endif // INDIGO_IMAGE_H_INCLUDED
