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

#ifndef INDIGO_IMAGE_H_INCLUDED
#define INDIGO_IMAGE_H_INCLUDED

#include "Common.h"

#include "../lib/Image.h"

namespace Indigo
{
	class Image
	{
	public:
		~Image();

		bool load(const OOGL::ResourceBundle& resource, const char* name);
		bool destroy();

		glm::uvec2 size() const;
		unsigned int components() const;

		OOBase::SharedPtr<OOGL::Image> render_image() const;

	private:
		OOBase::SharedPtr<OOGL::Image> m_image;

		void do_load(bool* ret_val, const OOGL::ResourceBundle* resource, const char* name);
		void do_destroy();
		void get_size(glm::uvec2* sz);
		void get_components(unsigned int* comp);
	};
}

#endif // INDIGO_IMAGE_H_INCLUDED
