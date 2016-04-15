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

#ifndef INDIGO_UILOADER_H_INCLUDED
#define INDIGO_UILOADER_H_INCLUDED

#include "UILayer.h"
#include "UIButton.h"

namespace Indigo
{
	class ResourceBundle;
	class Image;
	class Font;

	class UILoader
	{
	public:
		UILoader(OOBase::SharedPtr<Window> wnd) : m_wnd(wnd), m_resource(NULL)
		{}

		bool load(ResourceBundle& resource, const char* resource_name, unsigned int& zorder, UIGroup* parent = NULL);
		
	private:
		OOBase::SharedPtr<Window> m_wnd;
		ResourceBundle* m_resource;

		struct error_pos_t
		{
			unsigned int m_line;
			unsigned int m_col;
		} m_error_pos;

		typedef OOBase::HashTable<OOBase::SharedString<OOBase::ThreadLocalAllocator>,OOBase::SharedPtr<UIWidget>,OOBase::ThreadLocalAllocator> widget_hash_t;
		widget_hash_t m_hashWidgets;

		typedef OOBase::HashTable<OOBase::SharedString<OOBase::ThreadLocalAllocator>,OOBase::SharedPtr<Image>,OOBase::ThreadLocalAllocator> image_hash_t;
		image_hash_t m_hashImages;

		typedef OOBase::HashTable<OOBase::SharedString<OOBase::ThreadLocalAllocator>,OOBase::SharedPtr<NinePatch>,OOBase::ThreadLocalAllocator> ninepatch_hash_t;
		ninepatch_hash_t m_hash9Patches;

		typedef OOBase::HashTable<OOBase::SharedString<OOBase::ThreadLocalAllocator>,OOBase::SharedPtr<Font>,OOBase::ThreadLocalAllocator> font_hash_t;
		font_hash_t m_hashFonts;

		typedef OOBase::HashTable<OOBase::SharedString<OOBase::ThreadLocalAllocator>,OOBase::SharedPtr<UIButton::Style>,OOBase::ThreadLocalAllocator> button_style_hash_t;
		button_style_hash_t m_hashButtonStyles;

		void syntax_error(const char* fmt, ...) OOBASE_FORMAT(printf,2,3);

		void skip_whitespace(const char*& p, const char* pe);
		const char*& inc_p(const char*& p, const char* pe);
		bool character(const char*& p, const char* pe, char c);
		bool ident(const char*& p, const char* pe, OOBase::ScopedString& i);
		bool type_name(const char*& p, const char* pe, OOBase::ScopedString& i);
		bool parse_uint(const char*& p, const char* pe, unsigned int& u);
		bool parse_uvec2(const char*& p, const char* pe, glm::uvec2& u);
		bool parse_uvec4(const char*& p, const char* pe, glm::uvec4& u);
		bool parse_int(const char*& p, const char* pe, int& i);
		bool parse_ivec2(const char*& p, const char* pe, glm::ivec2& i);
		bool parse_string(const char*& p, const char* pe, OOBase::ScopedString& s);
		bool parse_float(const char*& p, const char* pe, float& i);
		bool parse_colour(const char*& p, const char* pe, glm::vec4& c);

		bool load_top_level(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent, unsigned int zorder);
		bool load_layer(const char*& p, const char* pe, unsigned int zorder);
		bool load_children(const char*& p, const char* pe, UIGroup* parent, const char* parent_name, unsigned int& zorder);
		OOBase::SharedPtr<UIWidget> load_child(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent, const char* parent_name, unsigned int zorder);
		bool load_grid_sizer(const char*& p, const char* pe, UIGroup* parent, const char* parent_name, UIGridSizer& sizer, unsigned int& zorder, bool add_loose);
		OOBase::SharedPtr<Image> load_image(const char*& p, const char* pe, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& image_name);
		OOBase::SharedPtr<Font> load_font(const char*& p, const char* pe, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& font_name);
		OOBase::SharedPtr<NinePatch> load_9patch(const char*& p, const char* pe, const OOBase::SharedString<OOBase::ThreadLocalAllocator>& patch_name);
		bool load_button_style(const char*& p, const char* pe);
		bool load_button_style_state(const char*& p, const char* pe, UIButton::StyleState& state, const OOBase::SharedPtr<UIButton::Style>& style);
		OOBase::SharedPtr<UIWidget> load_uiimage(const char*& p, const char* pe, UIGroup* parent, unsigned int zorder);
		OOBase::SharedPtr<UIWidget> load_button(const char*& p, const char* pe, UIGroup* parent, unsigned int zorder);
		OOBase::SharedPtr<UIWidget> load_label(const char*& p, const char* pe, UIGroup* parent, unsigned int zorder);
		OOBase::SharedPtr<UIWidget> load_panel(const char*& p, const char* pe, UIGroup* parent, const char* name, unsigned int zorder);
	};
}

#endif // INDIGO_UILOADER_H_INCLUDED
