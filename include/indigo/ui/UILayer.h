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

#ifndef INDIGO_UILAYER_H_INCLUDED
#define INDIGO_UILAYER_H_INCLUDED

#include <OOBase/HashTable.h>

#include "../Layer.h"

#include "UISizer.h"

namespace Indigo
{
	class UILayer;

	namespace Render
	{
		class UILayer : public UIGroup, public Layer
		{
			friend class Indigo::UILayer;

		public:
			UILayer(Window* window, Indigo::UILayer* owner, bool visible = false, const glm::ivec2& pos = glm::ivec2(), const glm::uvec2& size = glm::uvec2());

		private:
			glm::mat4        m_mvp;
			Indigo::UILayer* m_owner;
			bool             m_dirty;

			OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator> m_cursor_hits;
			OOBase::WeakPtr<UIDrawable> m_focus_child;

			bool on_update();
			void on_draw(OOGL::State& glState) const;
			bool on_mousebutton(const OOGL::Window::mouse_click_t& click);
			void on_size(const glm::uvec2& sz);
			bool on_cursormove(const glm::dvec2& pos);

			void make_dirty() { m_dirty = true; }
		};
	}

	class UILayer : public Layer, public UIGroup
	{
		friend class Render::UILayer;

	public:
		struct CreateParams : public UIWidget::CreateParams
		{
			CreateParams(OOBase::uint32_t state = eWS_enabled,
					bool fixed = false,
					bool modal = true,
					const glm::uvec4& margins = glm::uvec4(),
					const glm::uvec2& padding = glm::uvec2()
			) :
				UIWidget::CreateParams(state,glm::ivec2(),glm::uvec2()),
				m_fixed(fixed),
				m_modal(modal),
				m_padding(padding)
			{}

			bool          m_fixed;
			bool          m_modal;
			glm::uvec4    m_margins;
			glm::uvec2    m_padding;
		};

		UILayer(const CreateParams& params = CreateParams());

		UIGridSizer& sizer() { return m_sizer; }

		OOBase::SharedPtr<UIWidget> find_widget(const char* name, size_t len = -1) const;

		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> on_close(const OOBase::Delegate0<void,OOBase::ThreadLocalAllocator>& delegate);

	protected:
		bool add_named_widget(const OOBase::SharedPtr<UIWidget>& widget, const char* name, size_t len = -1);

	private:
		UIGridSizer      m_sizer;
		bool             m_modal;

		OOBase::Delegate0<void,OOBase::ThreadLocalAllocator> m_on_close;
		OOBase::HashTable<size_t,OOBase::WeakPtr<UIWidget>,OOBase::ThreadLocalAllocator> m_names;

		OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window);
		glm::uvec2 min_size() const;
		glm::uvec2 ideal_size() const;

		bool on_close();
		void on_size(glm::uvec2& sz);
		void on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask);
		void on_layout(const glm::uvec2& sz);
		void make_dirty();
	};
}

#endif // INDIGO_UILAYER_H_INCLUDED
