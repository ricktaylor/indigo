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

#include "../Parser.h"
#include "UIButton.h"
#include "UILayer.h"

namespace Indigo
{
	class ResourceBundle;
	class Image;
	class Font;

	class UILoader : public Parser
	{
	public:
		UILoader(OOBase::SharedPtr<Window> wnd) : m_wnd(wnd)
		{}

		virtual ~UILoader();

		void unload();

		OOBase::SharedPtr<UILayer> find_layer(const char* name, size_t len = -1) const;

		OOBase::SharedPtr<Window> window() const { return m_wnd; }

	private:
		OOBase::SharedPtr<Window> m_wnd;
		
		typedef OOBase::HashTable<size_t,OOBase::SharedPtr<UILayer>,OOBase::ThreadLocalAllocator> layer_hash_t;
		layer_hash_t m_hashLayers;

		typedef OOBase::HashTable<size_t,OOBase::SharedPtr<UIButton::Style>,OOBase::ThreadLocalAllocator> button_style_hash_t;
		button_style_hash_t m_hashButtonStyles;

		typedef OOBase::HashTable<size_t,OOBase::SharedPtr<NinePatch>,OOBase::ThreadLocalAllocator> ninepatch_hash_t;
		ninepatch_hash_t m_hash9Patches;

		bool parse_create_params(const OOBase::ScopedString& arg, const char*& p, const char* pe, UIWidget::CreateParams& params);

		bool load_top_level(const char*& p, const char* pe, const OOBase::ScopedString& type);
		bool load_layer(const char*& p, const char* pe);
		bool load_children(const char*& p, const char* pe, UIGroup* parent);
		OOBase::SharedPtr<UIWidget> load_child(const char*& p, const char* pe, const OOBase::ScopedString& type, UIGroup* parent);
		bool load_grid_sizer(const char*& p, const char* pe, UIGroup* parent, UIGridSizer& sizer, bool add_loose);
		bool load_button_style(const char*& p, const char* pe);
		OOBase::SharedPtr<NinePatch> load_9patch(const char*& p, const char* pe, const OOBase::ScopedString& patch_name);
		bool load_button_style_state(const char*& p, const char* pe, UIButton::StyleState& state, const OOBase::SharedPtr<UIButton::Style>& style);
		OOBase::SharedPtr<UIWidget> load_uiimage(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name);
		OOBase::SharedPtr<UIWidget> load_button(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name);
		OOBase::SharedPtr<UIWidget> load_label(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name);
		OOBase::SharedPtr<UIWidget> load_panel(const char*& p, const char* pe, UIGroup* parent, const OOBase::ScopedString& name);
	};
}

#endif // INDIGO_UILOADER_H_INCLUDED
