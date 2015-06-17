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

#include "GUIStyle.h"
#include "Render.h"

Indigo::Render::GUI::Style::Style() : m_foreground_colour(1.f), m_background_colour(1.f), m_borders(0)
{

}

Indigo::GUI::Style::Style() : m_foreground_colour(255), m_background_colour(255), m_borders(0)
{

}

Indigo::GUI::Style::~Style()
{
	destroy();
}

bool Indigo::GUI::Style::create(const OOBase::SharedPtr<OOGL::Window>& wnd)
{
	if (!wnd || !wnd->valid())
		LOG_ERROR_RETURN(("Widget::Create bad Window"),false);

	if (m_render_style)
		LOG_ERROR_RETURN(("Widget::Create called twice"),false);

	m_wnd = wnd;

	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Style::do_create),&ret) && ret;
}

void Indigo::GUI::Style::do_create(bool* ret_val)
{
	OOBase::SharedPtr<Render::GUI::Style> style = OOBase::allocate_shared<Render::GUI::Style,OOBase::ThreadLocalAllocator>();
	if (!style)
	{
		LOG_ERROR(("Failed to allocate Style: %s",OOBase::system_error_text()));
		*ret_val = false;
	}
	else
	{
		style.swap(m_render_style);
		*ret_val = true;
	}
}

bool Indigo::GUI::Style::destroy()
{
	return !m_render_style || render_call(OOBase::make_delegate(this,&Style::do_destroy));
}

void Indigo::GUI::Style::do_destroy()
{
	m_render_style.reset();
}

bool Indigo::GUI::Style::clone(const OOBase::SharedPtr<Style>& orig)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Style::do_clone),&ret,orig->m_render_style.get()) || !ret)
		return false;

	m_wnd = orig->m_wnd;
	m_font = orig->m_font;
	m_foreground_colour = orig->m_foreground_colour;
	m_background_colour = orig->m_background_colour;
	m_borders = orig->m_borders;
	m_background_image = orig->m_background_image;

	return true;
}

void Indigo::GUI::Style::do_clone(bool* ret_val, Render::GUI::Style* orig)
{
	OOBase::SharedPtr<Render::GUI::Style> style = OOBase::allocate_shared<Render::GUI::Style,OOBase::ThreadLocalAllocator>();
	if (!style)
	{
		LOG_ERROR(("Failed to allocate Style: %s",OOBase::system_error_text()));
		*ret_val = false;
	}
	else
	{
		style->m_font = orig->m_font;
		style->m_foreground_colour = orig->m_foreground_colour;
		style->m_background_colour = orig->m_background_colour;
		style->m_background_image = orig->m_background_image;
		style->m_background_image_size = orig->m_background_image_size;
		style->m_borders = orig->m_borders;

		m_render_style.swap(style);
		*ret_val = true;
	}
}

bool Indigo::GUI::Style::font(const OOBase::SharedPtr<Font>& font)
{
	if (!m_render_style)
		LOG_ERROR_RETURN(("Style::create not called!"),false);

	if (!font)
		LOG_ERROR_RETURN(("Style font NULL"),false);

	if (m_font != font)
	{
		bool ret = false;
		if (!render_call(OOBase::make_delegate(this,&Style::set_font),&ret,font.get()) || !ret)
			return false;

		m_font = font;
	}
	return true;
}

bool Indigo::GUI::Style::font(const OOGL::ResourceBundle& resource, const char* name)
{
	OOBase::SharedPtr<Font> f = OOBase::allocate_shared<Font>(m_wnd);
	if (!f)
		LOG_ERROR_RETURN(("Failed to allocate font: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!f->load(resource,name))
		return false;

	return font(f);
}

void Indigo::GUI::Style::set_font(bool* ret_val, Font* font)
{
	m_render_style->m_font = font->render_font();
	*ret_val = true;
}

bool Indigo::GUI::Style::foreground_colour(const glm::u8vec4& col)
{
	if (!m_render_style)
		LOG_ERROR_RETURN(("Style::create not called!"),false);

	if (m_foreground_colour != col)
	{
		glm::vec4 fcol(col.r/255.f,col.g/255.f,col.b/255.f,col.a/255.f);
		bool ret = false;
		if (!render_call(OOBase::make_delegate(this,&Style::set_foreground_colour),&ret,&fcol) || !ret)
			return false;

		m_foreground_colour = col;
	}
	return true;
}

void Indigo::GUI::Style::set_foreground_colour(bool* ret_val, glm::vec4* c)
{
	m_render_style->m_foreground_colour = *c;
	*ret_val = true;
}

bool Indigo::GUI::Style::background_colour(const glm::u8vec4& col)
{
	if (!m_render_style)
		LOG_ERROR_RETURN(("Style::create not called!"),false);

	if (m_background_colour != col)
	{
		glm::vec4 fcol(col.r/255.f,col.g/255.f,col.b/255.f,col.a/255.f);
		bool ret = false;
		if (!render_call(OOBase::make_delegate(this,&Style::set_background_colour),&ret,&fcol) || !ret)
			return false;

		m_background_colour = col;
	}
	return true;
}

void Indigo::GUI::Style::set_background_colour(bool* ret_val, glm::vec4* c)
{
	m_render_style->m_background_colour = *c;
	*ret_val = true;
}

bool Indigo::GUI::Style::background_image(const OOBase::SharedPtr<Image>& image)
{
	bool ret = false;
	if (!render_call(OOBase::make_delegate(this,&Style::set_background_image),&ret,image.get()) || !ret)
		return false;

	m_background_image = image;
	return true;
}

bool Indigo::GUI::Style::background_image(const OOGL::ResourceBundle& resource, const char* name)
{
	OOBase::SharedPtr<Image> image = OOBase::allocate_shared<Image>();
	if (!image)
		LOG_ERROR_RETURN(("Failed to allocate image: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!image->load(resource,name))
		return false;

	return background_image(image);
}

void Indigo::GUI::Style::set_background_image(bool* ret_val, Image* image)
{
	*ret_val = false;
	OOBase::SharedPtr<OOGL::Image> bg = image->render_image();
	if (bg && bg->valid())
	{
		m_wnd->make_current();

		OOBase::SharedPtr<OOGL::Texture> t = bg->make_texture(GL_RGBA8);
		if (t && t->valid())
		{
			t->parameter(GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			t->parameter(GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			t->parameter(GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			t->parameter(GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

			m_render_style->m_background_image.swap(t);
			m_render_style->m_background_image_size = bg->size();
			*ret_val = true;
		}
	}
}

bool Indigo::GUI::Style::borders(OOBase::uint16_t left, OOBase::uint16_t top, OOBase::uint16_t right, OOBase::uint16_t bottom)
{
	return borders(glm::u16vec4(left,top,right,bottom));
}

bool Indigo::GUI::Style::borders(const glm::u16vec4& b)
{
	bool ret = false;
	return render_call(OOBase::make_delegate(this,&Style::set_borders),&ret,&b) && ret;
}

void Indigo::GUI::Style::set_borders(bool* ret_val, const glm::u16vec4* borders)
{
	m_render_style->m_borders = *borders;
	*ret_val = true;
}
