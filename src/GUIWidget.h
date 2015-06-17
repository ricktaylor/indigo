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

#ifndef INDIGO_GUIWIDGET_H_INCLUDED
#define INDIGO_GUIWIDGET_H_INCLUDED

#include "GUIStyle.h"

namespace Indigo
{
	namespace GUI
	{
		class Widget;
		class Style;
	}

	namespace Render
	{
		namespace GUI
		{
			class Widget : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Widget>
			{
				friend class Indigo::GUI::Widget;
				friend class Panel;

			public:
				bool shown() const;

				bool visible() const;
				bool visible(bool show = true);

				bool enabled() const;
				bool enable(bool enabled = true);

				bool focused() const;
				bool focus(bool focused = true);

				bool hilighted() const;
				bool hilight(bool hilighted = true);

				const glm::i16vec2& position() const { return m_position; }
				void position(const glm::i16vec2& pos);

				const glm::u16vec2& size() const { return m_size; }
				virtual glm::u16vec2 size(const glm::u16vec2& sz);

				glm::u16vec2 min_size() const;
				virtual glm::u16vec2 min_size(const glm::u16vec2& sz);

				glm::u16vec2 max_size() const { return m_max_size; }
				glm::u16vec2 max_size(const glm::u16vec2& sz);
				
				virtual glm::u16vec2 client_size() const { return m_size; }
				virtual glm::u16vec2 client_size(const glm::u16vec2& sz) { return size(sz); }

				virtual glm::u16vec2 ideal_size() const;

			protected:
				Widget();
				virtual ~Widget();

				OOBase::WeakPtr<Widget> m_parent;

				virtual bool add_child(const OOBase::SharedPtr<Widget>& child) { return false; }
				virtual void remove_child(const OOBase::SharedPtr<Widget>& child) {}

				virtual bool can_show(bool show) { return true; }
				virtual bool can_enable(bool enabled) { return false; }
				virtual bool can_focus(bool focused) { return false; }
				virtual bool can_hilight(bool hilighted) { return false; }

				virtual const OOBase::SharedPtr<OOGL::Window>& window() const;

				virtual void draw(OOGL::State& glState, const glm::mat4& mvp) = 0;

				const OOBase::SharedPtr<Style>& style() const { return m_style; }

			private:
				bool m_visible;
				bool m_enabled;
				bool m_focused;
				bool m_hilighted;
				glm::i16vec2 m_position;
				glm::u16vec2 m_min_size;
				glm::u16vec2 m_max_size;
				glm::u16vec2 m_size;
				OOBase::SharedPtr<Style> m_style;

				bool create(const OOBase::SharedPtr<Widget>& parent, const OOBase::SharedPtr<Style>& style, const glm::i16vec2& pos, const glm::u16vec2& min_size);
			};
		}
	}

	namespace GUI
	{
		class Widget : public OOBase::NonCopyable
		{
			friend class Sizer;
			friend class Style;

		public:
			Widget();
			virtual ~Widget();

			bool destroy();

			bool shown() const;
			bool visible() const;
			bool visible(bool show = true);

			bool enabled() const;
			bool enable(bool enabled = true);
			bool disable() { return enable(false); }

			bool focused() const;
			bool focus(bool focused = true);

			bool hilighted() const;
			bool hilight(bool hilighted = true);

			const OOBase::SharedPtr<Style>& style() const { return m_style; }
			bool style(const OOBase::SharedPtr<Style>& s);

		protected:
			bool create(Widget* parent, const glm::u16vec2& min_size = glm::u16vec2(-1), const glm::i16vec2& pos = glm::i16vec2(0));
			bool create(Widget* parent, const OOBase::SharedPtr<Style>& style, const glm::u16vec2& min_size = glm::u16vec2(-1), const glm::i16vec2& pos = glm::i16vec2(0));

			template <typename T>
			OOBase::SharedPtr<T> render_widget() const
			{
				return OOBase::static_pointer_cast<T>(m_render_widget);
			}

			virtual OOBase::SharedPtr<Render::GUI::Widget> create_render_widget() = 0;

		private:
			OOBase::SharedPtr<Render::GUI::Widget> m_render_widget;
			OOBase::SharedPtr<Style> m_style;

			void do_create(bool* ret_val, Widget* parent, const OOBase::SharedPtr<Style>* style, const glm::i16vec2* pos, const glm::u16vec2* min_size);
			void do_destroy();
			void get_shown(bool* shown);
			void get_visible(bool* visible);
			void set_visible(bool* visible);
			void get_enabled(bool* enabled);
			void set_enable(bool* enabled);
			void get_focused(bool* focused);
			void set_focus(bool* focused);
			void get_hilighted(bool* hilighted);
			void set_hilight(bool* hilighted);
			void set_style(OOBase::SharedPtr<Render::GUI::Style>* style);
		};
	}
}

#endif // INDIGO_GUIWIDGET_H_INCLUDED
