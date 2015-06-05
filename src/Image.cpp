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

#include "Image.h"
#include "Render.h"

Indigo::Image::~Image()
{
	destroy();
}

bool Indigo::Image::load(const OOGL::ResourceBundle& resource, const char* name)
{
	if (m_image)
		return false;

	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Indigo::Image::do_load),&ret,&resource,name) && ret;
}

void Indigo::Image::do_load(bool* ret_val, const OOGL::ResourceBundle* resource, const char* name)
{
	OOBase::SharedPtr<OOGL::Image> image = OOBase::allocate_shared<OOGL::Image>();
	if (!image)
	{
		LOG_ERROR(("Failed to allocate render image: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)));
		*ret_val = false;
	}
	else
		*ret_val = image->load(*resource,name);

	if (*ret_val)
		image.swap(m_image);
}

bool Indigo::Image::destroy()
{
	return !m_image || render_call(OOBase::make_delegate(this,&Indigo::Image::do_destroy));
}

void Indigo::Image::do_destroy()
{
	m_image.reset();
}

glm::uvec2 Indigo::Image::size() const
{
	glm::uvec2 sz(0);
	render_call(OOBase::make_delegate(const_cast<Image*>(this),&Image::get_size),&sz);
	return sz;
}

void Indigo::Image::get_size(glm::uvec2* sz)
{
	if (m_image)
		*sz = m_image->size();
}

unsigned int Indigo::Image::components() const
{
	unsigned int comps = 0;
	render_call(OOBase::make_delegate(const_cast<Image*>(this),&Image::get_components),&comps);
	return comps;
}

void Indigo::Image::get_components(unsigned int* comp)
{
	if (m_image)
		*comp = m_image->components();
}

const OOBase::SharedPtr<OOGL::Image>& Indigo::Image::render_image() const
{
	return m_image;
}
