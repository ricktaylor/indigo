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

#include "../Common.h"

#include "../../include/indigo/ui/UILoader.h"

#include "../../include/indigo/Resource.h"

#include "../../include/indigo/ui/UISizer.h"
#include "../../include/indigo/ui/UIImage.h"
#include "../../include/indigo/ui/UIButton.h"
#include "../../include/indigo/ui/UIPanel.h"

bool Indigo::UILoader::parse_create_params(const OOBase::ScopedString& arg, const char*& p, const char* pe, UIWidget::CreateParams& params)
{
	if (arg == "POSITION")
		return parse_ivec2(p,pe,params.m_position);

	if (arg == "SIZE")
		return parse_uvec2(p,pe,params.m_size);

	if (arg == "VISIBLE")
	{
		params.m_state |= Indigo::UIWidget::eWS_visible;
		return true;
	}

	if (arg == "ENABLED")
	{
		params.m_state |= Indigo::UIWidget::eWS_enabled;
		return true;
	}

	if (arg == "DISABLED")
	{
		params.m_state &= ~Indigo::UIWidget::eWS_enabled;
		return true;
	}

	return false;
}

OOBase::SharedPtr<Indigo::UILayer> Indigo::UILoader::find_layer(const char* name, size_t len) const
{
	OOBase::SharedPtr<UILayer> ret;
	layer_hash_t::const_iterator i = m_hashLayers.find(OOBase::Hash<const char*>::hash(name,len));
	if (i)
		ret = i->second;
	return ret;
}

bool Indigo::UILoader::load_top_level(const char*& p, const char* pe, const OOBase::ScopedString& type)
{
	if (type == "DIALOG")
	{
		return load_layer(p,pe);
	}

	if (type == "BUTTON_STYLE")
	{
		return load_button_style(p,pe);
	}
	

	LOG_WARNING(("Invalid top level type '%s'",type.c_str()));
	return false;
}

bool Indigo::UILoader::load_layer(const char*& p, const char* pe)
{
	OOBase::ScopedString name;
	if (!ident(p,pe,name))
		SYNTAX_ERROR_RETURN(("Identifier expected"),false);

	size_t hash = OOBase::Hash<const char*>::hash(name);
	if (m_hashLayers.find(hash))
		SYNTAX_ERROR_RETURN(("Duplicate identifier '%s'",name.c_str()),false);

	UILayer::CreateParams params;

	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "FIXED")
				{
					params.m_fixed = true;
				}
				else if (arg == "MODAL")
				{
					params.m_modal = true;
				}
				else if (arg == "MARGINS")
				{
					if (!parse_uvec4(p,pe,params.m_margins))
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (arg == "PADDING")
				{
					if (!parse_uvec2(p,pe,params.m_padding))
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (!parse_create_params(arg,p,pe,params))
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in DIALOG",arg.c_str()),false);

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),false);
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),false);
	}

	OOBase::SharedPtr<UILayer> layer = OOBase::allocate_shared<UILayer,OOBase::ThreadLocalAllocator>(m_wnd.get(),params);
	if (!layer)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	if (!m_hashLayers.insert(hash,layer))
		LOG_ERROR_RETURN(("Failed to insert layer into map: %s",OOBase::system_error_text()),false);

	if (!m_wnd->add_layer(layer))
		return false;

	if (!load_grid_sizer(p,pe,layer.get(),layer->sizer(),true))
		return false;

	return true;
}

bool Indigo::UILoader::load_grid_sizer(const char*& p, const char* pe, UIGroup* parent, UIGridSizer& sizer, bool add_loose)
{
	if (character(p,pe,'{'))
	{
		OOBase::ScopedString type;
		for (bool first = true;;first = false)
		{
			if (character(p,pe,'['))
			{
				unsigned int row = 0,col = 0,proportion = -1;

				if (!parse_uint(p,pe,row))
					SYNTAX_ERROR_RETURN(("Missing row argument in layout"),false);

				if (!character(p,pe,','))
					SYNTAX_ERROR_RETURN(("',' expected"),false);

				if (!parse_uint(p,pe,col))
					SYNTAX_ERROR_RETURN(("Missing column argument in layout"),false);

				if (character(p,pe,'/') && !parse_uint(p,pe,proportion))
					SYNTAX_ERROR_RETURN(("Missing proportion argument in layout"),false);

				if (!character(p,pe,']'))
					SYNTAX_ERROR_RETURN(("']' expected"),false);

				unsigned int flags = UIGridSizer::align_centre | UIGridSizer::expand;
				bool parens = character(p,pe,'(');
				if (parens)
				{
					if (type_name(p,pe,type))
					{
						flags = 0;
						for (;;)
						{
							if (type == "ALIGN_LEFT")
								flags = (flags & 0x3c) | UIGridSizer::align_left;
							else if (type == "ALIGN_RIGHT")
								flags = (flags & 0x3c) | UIGridSizer::align_right;
							else if (type == "ALIGN_HCENTRE")
								flags = (flags & 0x3c) | UIGridSizer::align_hcentre;
							else if (type == "ALIGN_BOTTOM")
								flags = (flags & 0x33) | UIGridSizer::align_bottom;
							else if (type == "ALIGN_TOP")
								flags = (flags & 0x33) | UIGridSizer::align_top;
							else if (type == "ALIGN_VCENTRE")
								flags = (flags & 0x33) | UIGridSizer::align_vcentre;
							else if (type == "ALIGN_CENTRE")
								flags = (flags & 0x30) | UIGridSizer::align_centre;
							else if (type == "EXPAND_HORIZ")
								flags = (flags & 0xF) | UIGridSizer::expand_horiz;
							else if (type == "EXPAND_VERT")
								flags = (flags & 0xF) | UIGridSizer::expand_vert;
							else if (type == "EXPAND")
								flags = (flags & 0xF) | UIGridSizer::expand;
							else
								SYNTAX_ERROR_RETURN(("Unexpected layout argument '%s'",type.c_str()),false);

							if (!character(p,pe,','))
								break;

							if (!type_name(p,pe,type))
								SYNTAX_ERROR_RETURN(("Argument expected"),false);
						}
					}

					if (!character(p,pe,')'))
						SYNTAX_ERROR_RETURN(("')' expected"),false);
				}

				if (!type_name(p,pe,type))
					SYNTAX_ERROR_RETURN(("Type name expected"),false);

				if (type == "GRID")
				{
					bool fixed = false;
					glm::uvec4 margins(0);
					glm::uvec2 padding(0);

					if (character(p,pe,'('))
					{
						OOBase::ScopedString arg;
						if (type_name(p,pe,arg))
						{
							for (;;)
							{
								if (arg == "FIXED")
								{
									fixed = true;
								}
								else if (arg == "MARGINS")
								{
									if (!parse_uvec4(p,pe,margins))
										return OOBase::SharedPtr<UIWidget>();
								}
								else if (arg == "PADDING")
								{
									if (!parse_uvec2(p,pe,padding))
										return OOBase::SharedPtr<UIWidget>();
								}
								else
									SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in GRID",arg.c_str()),OOBase::SharedPtr<UIWidget>());

								if (!character(p,pe,','))
									break;

								if (!type_name(p,pe,arg))
									SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
							}
						}

						if (!character(p,pe,')'))
							SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
					}

					OOBase::SharedPtr<UIGridSizer> s = OOBase::allocate_shared<UIGridSizer,OOBase::ThreadLocalAllocator>(fixed,margins,padding);
					if (!s)
						LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

					if (proportion == (unsigned int)-1)
						proportion = 1;

					if (!sizer.add_sizer(row,col,s,flags,proportion))
						return false;

					if (!load_grid_sizer(p,pe,parent,*s,false))
						return false;
				}
				else if (!parens && type == "SPACER")
				{
					glm::uvec2 size(0);
					if (!parse_uvec2(p,pe,size))
						SYNTAX_ERROR_RETURN(("(width,height) expected"),false);

					if (proportion == (unsigned int)-1)
						proportion = 0;

					if (!sizer.add_spacer(row,col,size,proportion))
						return false;
				}
				else
				{
					OOBase::SharedPtr<UIWidget> child = load_child(p,pe,type,parent);
					if (!child)
						return false;

					if (proportion == (unsigned int)-1)
						proportion = 1;

					if (!sizer.add_widget(row,col,child,flags,proportion))
						return false;
				}
			}
			else if (add_loose && type_name(p,pe,type))
			{
				OOBase::SharedPtr<UIWidget> child = load_child(p,pe,type,parent);
				if (!child)
					return false;
			}
			else if (!first)
			{
				if (add_loose)
					SYNTAX_ERROR_RETURN(("'[' or type name expected"),false);
				else
					SYNTAX_ERROR_RETURN(("'[' expected"),false);
			}

			if (!character(p,pe,','))
				break;
		}

		if (!character(p,pe,'}'))
			SYNTAX_ERROR_RETURN(("'}' expected"),false);
	}
	
	return true;
}

bool Indigo::UILoader::load_children(const char*& p, const char* pe, UIGroup* parent)
{
	if (character(p,pe,'{'))
	{
		OOBase::ScopedString type;
		if (type_name(p,pe,type))
		{
			for (;;)
			{
				OOBase::SharedPtr<UIWidget> child = load_child(p,pe,type,parent);
				if (!child)
					return false;

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,type))
					SYNTAX_ERROR_RETURN(("Type name expected"),false);
			}
		}

		if (!character(p,pe,'}'))
			SYNTAX_ERROR_RETURN(("'}' expected"),false);
	}

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_child(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent)
{
	OOBase::SharedPtr<UIWidget> ret;

	OOBase::ScopedString name;
	ident(p,pe,name);

	if (type == "BUTTON")
	{
		ret = load_button(p,pe,parent,name);
	}
	else if (type == "IMAGE")
	{
		ret = load_uiimage(p,pe,parent,name);
	}
	else if (type == "LABEL")
	{
		ret = load_label(p,pe,parent,name);
	}
	else if (type == "PANEL")
	{
		ret = load_panel(p,pe,parent,name);
	}
	else
		SYNTAX_ERROR_RETURN(("Unknown type name '%s'",type.c_str()),ret);

	return ret;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_uiimage(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name)
{
	UIImage::CreateParams params;

	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "COLOUR")
				{
					if (!parse_colour(p,pe,params.m_colour))
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (!parse_create_params(arg,p,pe,params))
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in IMAGE",arg.c_str()),OOBase::SharedPtr<UIWidget>());

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
	}

	OOBase::ScopedString image_name;
	if (!parse_string(p,pe,image_name))
		return OOBase::SharedPtr<UIWidget>();

	if (image_name.empty())
		SYNTAX_ERROR_RETURN(("Image name expected"),OOBase::SharedPtr<UIWidget>());

	OOBase::SharedPtr<Image> image = load_image(p,pe,image_name);
	if (!image)
		return OOBase::SharedPtr<UIWidget>();

	OOBase::SharedPtr<UIImage> uiimage = OOBase::allocate_shared<UIImage,OOBase::ThreadLocalAllocator>(parent,image,params);
	if (!uiimage)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

	if (!parent->add_widget(uiimage,name.c_str(),name.length()))
		return OOBase::SharedPtr<UIWidget>();

	return uiimage;
}

OOBase::SharedPtr<Indigo::NinePatch> Indigo::UILoader::load_9patch(const char*& p, const char* pe, const OOBase::ScopedString& patch_name)
{
	size_t hash = OOBase::Hash<const char*>::hash(patch_name);
	ninepatch_hash_t::iterator i = m_hash9Patches.find(hash);
	if (i)
		return i->second;

	if (!m_resource->exists(patch_name.c_str()))
		SYNTAX_ERROR_RETURN(("Missing 9 patch resource '%s'",patch_name.c_str()),OOBase::SharedPtr<NinePatch>());

	OOBase::SharedPtr<NinePatch> patch = OOBase::allocate_shared<NinePatch,OOBase::ThreadLocalAllocator>();
	if (!patch)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),patch);

	if (!patch->load(*m_resource,patch_name.c_str(),4))
		return OOBase::SharedPtr<NinePatch>();

	if (!m_hash9Patches.insert(hash,patch))
		LOG_WARNING(("Failed to cache 9 patch: %s",OOBase::system_error_text()));

	return patch;
}

bool Indigo::UILoader::load_button_style(const char*& p, const char* pe)
{
	OOBase::ScopedString name;
	if (!ident(p,pe,name))
		SYNTAX_ERROR_RETURN(("Identifier expected"),false);

	size_t hash = OOBase::Hash<const char*>::hash(name);
	if (m_hashButtonStyles.find(hash))
		SYNTAX_ERROR_RETURN(("Duplicate identifier '%s'",name.c_str()),false);

	OOBase::SharedPtr<UIButton::Style> style = OOBase::allocate_shared<UIButton::Style,OOBase::ThreadLocalAllocator>();
	if (!style)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

	UIButton::StyleState default_state;
	default_state.m_font_size = 0;
	default_state.m_background_colour = glm::vec4(1.f);
	default_state.m_text_colour = glm::vec4(0.f,0.f,0.f,1.f);
	default_state.m_shadow = glm::vec4(.5f);
	default_state.m_drop = glm::ivec2(0);
	default_state.m_style_flags = UIButton::align_centre;

	if (!load_button_style_state(p,pe,default_state,style))
		return false;

	if (!m_hashButtonStyles.insert(hash,style))
		LOG_ERROR_RETURN(("Failed to cache button style: %s",OOBase::system_error_text()),false);

	return true;
}

bool Indigo::UILoader::load_button_style_state(const char*& p, const char* pe, UIButton::StyleState& state, const OOBase::SharedPtr<UIButton::Style>& style)
{
	if (character(p,pe,'{'))
	{
		OOBase::ScopedString type;
		if (type_name(p,pe,type))
		{
			for (;;)
			{
				if (type == "FONT")
				{
					if (character(p,pe,'('))
					{
						if (type_name(p,pe,type))
						{
							for (;;)
							{
								if (type == "SIZE")
								{
									if (!parse_uint(p,pe,state.m_font_size))
										return false;
								}
								else if (type == "COLOUR")
								{
									if (!parse_colour(p,pe,state.m_text_colour))
										return false;
								}
								else if (type == "SHADOW")
								{
									if (!parse_colour(p,pe,state.m_shadow))
										return false;
								}
								else if (type == "DROP")
								{
									if (!parse_ivec2(p,pe,state.m_drop))
										return false;
								}
								else
									SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in FONT",type.c_str()),false);

								if (!character(p,pe,','))
									break;

								if (!type_name(p,pe,type))
									SYNTAX_ERROR_RETURN(("Argument expected"),false);
							}
						}

						if (!character(p,pe,')'))
							SYNTAX_ERROR_RETURN(("')' expected"),false);
					}

					OOBase::ScopedString font_name;
					if (!parse_string(p,pe,font_name))
						return false;

					if (font_name.empty())
					{
						if (style)
							SYNTAX_ERROR_RETURN(("Font name required"),false);
					}
					else
					{
						state.m_font = load_font(p,pe,font_name);
						if (!state.m_font)
							return false;
					}
				}
				else if (type == "BACKGROUND")
				{
					if (character(p,pe,'('))
					{
						if (type_name(p,pe,type))
						{
							for (;;)
							{
								if (type == "COLOUR")
								{
									if (!parse_colour(p,pe,state.m_background_colour))
										return false;
								}
								else
									SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in FONT",type.c_str()),false);

								if (!character(p,pe,','))
									break;

								if (!type_name(p,pe,type))
									SYNTAX_ERROR_RETURN(("Argument expected"),false);
							}
						}

						if (!character(p,pe,')'))
							SYNTAX_ERROR_RETURN(("')' expected"),false);
					}

					OOBase::ScopedString patch_name;
					if (!parse_string(p,pe,patch_name))
						return false;

					if (patch_name.empty())
					{
						if (style)
							SYNTAX_ERROR_RETURN(("9 patch name expected"),OOBase::SharedPtr<NinePatch>());
					}
					else
					{
						state.m_background = load_9patch(p,pe,patch_name);
						if (!state.m_background)
							return false;
					}
				}
				else if (type == "ALIGN_LEFT")
					state.m_style_flags = (state.m_style_flags & 0xc) | UIButton::align_left;
				else if (type == "ALIGN_RIGHT")
					state.m_style_flags = (state.m_style_flags & 0xc) | UIButton::align_right;
				else if (type == "ALIGN_HCENTRE")
					state.m_style_flags = (state.m_style_flags & 0xc) | UIButton::align_hcentre;
				else if (type == "ALIGN_BOTTOM")
					state.m_style_flags = (state.m_style_flags & 0x3) | UIButton::align_bottom;
				else if (type == "ALIGN_TOP")
					state.m_style_flags = (state.m_style_flags & 0x3) | UIButton::align_top;
				else if (type == "ALIGN_VCENTRE")
					state.m_style_flags = (state.m_style_flags & 0x3) | UIButton::align_vcentre;
				else if (type == "ALIGN_CENTRE")
					state.m_style_flags = UILabel::align_centre;
				else if (style && type == "STATE")
				{
					OOBase::ScopedString name;
					if (!ident(p,pe,name))
						SYNTAX_ERROR_RETURN(("Identifier expected"),false);

					if (name == "Normal")
					{
						style->m_normal = state;

						if (!load_button_style_state(p,pe,style->m_normal,OOBase::SharedPtr<UIButton::Style>()))
							return false;
					}
					else if (name == "Active")
					{
						style->m_active = state;

						if (!load_button_style_state(p,pe,style->m_active,OOBase::SharedPtr<UIButton::Style>()))
							return false;
					}
					else if (name == "Pressed")
					{
						style->m_pressed = state;

						if (!load_button_style_state(p,pe,style->m_pressed,OOBase::SharedPtr<UIButton::Style>()))
							return false;
					}
					else if (name == "Disabled")
					{
						style->m_disabled = state;

						if (!load_button_style_state(p,pe,style->m_disabled,OOBase::SharedPtr<UIButton::Style>()))
							return false;
					}
					else
						SYNTAX_ERROR_RETURN(("Unknown BUTTON_STYLE state '%s'",name.c_str()),false);
				}

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,type))
					SYNTAX_ERROR_RETURN(("Type name expected"),false);
			}
		}

		if (!character(p,pe,'}'))
			SYNTAX_ERROR_RETURN(("'}' expected"),false);
	}

	return true;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_button(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name)
{
	UIButton::CreateParams params;
	OOBase::ScopedString style_name;

	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "STYLE")
				{
					if (!ident(p,pe,style_name))
						SYNTAX_ERROR_RETURN(("Expected style name"),OOBase::SharedPtr<UIWidget>());
				}
				else if (!parse_create_params(arg,p,pe,params))
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in BUTTON",arg.c_str()),OOBase::SharedPtr<UIWidget>());

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
	}

	if (style_name.empty())
		SYNTAX_ERROR_RETURN(("No style for BUTTON"),OOBase::SharedPtr<UIWidget>());

	button_style_hash_t::iterator i = m_hashButtonStyles.find(OOBase::Hash<const char*>::hash(style_name));
	if (!i)
		SYNTAX_ERROR_RETURN(("Undefined style '%s' for BUTTON",style_name.c_str()),OOBase::SharedPtr<UIWidget>());

	params.m_style = i->second;

	OOBase::SharedString<OOBase::ThreadLocalAllocator> caption;
	if (!parse_string(p,pe,caption))
		return OOBase::SharedPtr<UIWidget>();

	OOBase::SharedPtr<UIButton> button = OOBase::allocate_shared<UIButton,OOBase::ThreadLocalAllocator>(parent,caption,params);
	if (!button)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

	if (!parent->add_widget(button,name.c_str(),name.length()))
		return OOBase::SharedPtr<UIWidget>();

	return button;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_label(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name)
{
	UILabel::CreateParams params;

	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "FONT")
				{
					if (character(p,pe,'('))
					{
						if (type_name(p,pe,arg))
						{
							for (;;)
							{
								if (arg == "SIZE")
								{
									if (!parse_uint(p,pe,params.m_font_size))
										return OOBase::SharedPtr<UIWidget>();
								}
								else if (arg == "COLOUR")
								{
									if (!parse_colour(p,pe,params.m_colour))
										return OOBase::SharedPtr<UIWidget>();
								}
								else
									SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in FONT",arg.c_str()),OOBase::SharedPtr<UIWidget>());

								if (!character(p,pe,','))
									break;

								if (!type_name(p,pe,arg))
									SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
							}
						}

						if (!character(p,pe,')'))
							SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
					}

					OOBase::ScopedString font_name;
					if (!parse_string(p,pe,font_name))
						return OOBase::SharedPtr<UIWidget>();

					if (font_name.empty())
						SYNTAX_ERROR_RETURN(("Font name required"),OOBase::SharedPtr<UIWidget>());

					params.m_font = load_font(p,pe,font_name);
					if (!params.m_font)
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (arg == "ALIGN_LEFT")
					params.m_style = (params.m_style & 0xc) | UILabel::align_left;
				else if (arg == "ALIGN_RIGHT")
					params.m_style = (params.m_style & 0xc) | UILabel::align_right;
				else if (arg == "ALIGN_HCENTRE")
					params.m_style = (params.m_style & 0xc) | UILabel::align_hcentre;
				else if (arg == "ALIGN_BOTTOM")
					params.m_style = (params.m_style & 0x3) | UILabel::align_bottom;
				else if (arg == "ALIGN_TOP")
					params.m_style = (params.m_style & 0x3) | UILabel::align_top;
				else if (arg == "ALIGN_VCENTRE")
					params.m_style = (params.m_style & 0x3) | UILabel::align_vcentre;
				else if (arg == "ALIGN_CENTRE")
					params.m_style = UILabel::align_centre;
				else if (!parse_create_params(arg,p,pe,params))
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in LABEL",arg.c_str()),OOBase::SharedPtr<UIWidget>());

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
	}

	if (!params.m_font)
		SYNTAX_ERROR_RETURN(("No font for LABEL"),OOBase::SharedPtr<UIWidget>());

	OOBase::SharedString<OOBase::ThreadLocalAllocator> caption;
	if (!parse_string(p,pe,caption))
		return OOBase::SharedPtr<UIWidget>();

	OOBase::SharedPtr<UILabel> label = OOBase::allocate_shared<UILabel,OOBase::ThreadLocalAllocator>(parent,caption,params);
	if (!label)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

	if (!parent->add_widget(label,name.c_str(),name.length()))
		return OOBase::SharedPtr<UIWidget>();

	return label;
}

OOBase::SharedPtr<Indigo::UIWidget> Indigo::UILoader::load_panel(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name)
{
	UIPanel::CreateParams params;

	if (character(p,pe,'('))
	{
		OOBase::ScopedString arg;
		if (type_name(p,pe,arg))
		{
			for (;;)
			{
				if (arg == "FIXED")
				{
					params.m_fixed = true;
				}
				else if (arg == "COLOUR")
				{
					if (!parse_colour(p,pe,params.m_colour))
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (arg == "PADDING")
				{
					if (!parse_uvec2(p,pe,params.m_padding))
						return OOBase::SharedPtr<UIWidget>();
				}
				else if (!parse_create_params(arg,p,pe,params))
					SYNTAX_ERROR_RETURN(("Unexpected argument '%s' in PANEL",arg.c_str()),OOBase::SharedPtr<UIWidget>());

				if (!character(p,pe,','))
					break;

				if (!type_name(p,pe,arg))
					SYNTAX_ERROR_RETURN(("Argument expected"),OOBase::SharedPtr<UIWidget>());
			}
		}

		if (!character(p,pe,')'))
			SYNTAX_ERROR_RETURN(("')' expected"),OOBase::SharedPtr<UIWidget>());
	}

	OOBase::ScopedString patch_name;
	if (!parse_string(p,pe,patch_name))
		return OOBase::SharedPtr<UIWidget>();

	if (patch_name.empty())
		SYNTAX_ERROR_RETURN(("9 patch name expected"),OOBase::SharedPtr<UIWidget>());

	params.m_background = load_9patch(p,pe,patch_name);
	if (!params.m_background)
		return OOBase::SharedPtr<UIWidget>();

	OOBase::SharedPtr<UIPanel> panel = OOBase::allocate_shared<UIPanel,OOBase::ThreadLocalAllocator>(parent,params);
	if (!panel)
		LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),OOBase::SharedPtr<UIWidget>());

	if (!parent->add_widget(panel,name.c_str(),name.length()))
		return OOBase::SharedPtr<UIWidget>();

	if (!load_grid_sizer(p,pe,panel.get(),panel->sizer(),true))
		return OOBase::SharedPtr<UIWidget>();

	return panel;
}
