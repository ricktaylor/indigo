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

#include "Common.h"

namespace Indigo
{
	namespace Render
	{
		namespace GUI
		{
			class Widget : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Widget>
			{
			public:
				Widget();
				virtual ~Widget();

				bool create(const OOBase::SharedPtr<Widget>& parent);

				bool visible() const;
				bool visible(bool show = true);

				bool enabled() const;
				bool enable(bool enabled = true);

				bool focused() const;
				bool focus(bool focused = true);

				bool hilighted() const;
				bool hilight(bool hilighted = true);
				
			protected:
				OOBase::WeakPtr<Widget> m_parent;

				virtual bool add_child(const OOBase::SharedPtr<Widget>& child) { return false; }
				virtual void remove_child(const OOBase::SharedPtr<Widget>& child) {}

				virtual bool on_show(bool show) { return true; }
				virtual bool on_enable(bool enabled) { return true; }
				virtual bool on_focus(bool focused) { return false; }
				virtual bool on_hilight(bool hilighted) { return true; }

				virtual void on_refresh_layout() {}

			private:
				bool m_visible;
				bool m_enabled;
				bool m_focused;
				bool m_hilighted;
			};
		}
	}

	namespace GUI
	{
		class Widget : public OOBase::NonCopyable
		{
		public:
			Widget();
			virtual ~Widget();

			bool destroy();

			bool visible() const;
			bool visible(bool show = true);

			bool enabled() const;
			bool enable(bool enabled = true);
			bool disable() { return enable(false); }

			bool focused() const;
			bool focus(bool focused = true);

			bool hilighted() const;
			bool hilight(bool hilighted = true);

		protected:
			bool create(const OOBase::SharedPtr<Widget>& parent);

			template <typename T>
			OOBase::SharedPtr<T> widget() const
			{
				return OOBase::static_pointer_cast<T>(m_ptrWidget);
			}

			virtual OOBase::SharedPtr<Render::GUI::Widget> create_widget() = 0;

		private:
			OOBase::SharedPtr<Render::GUI::Widget> m_ptrWidget;

			bool do_create(const OOBase::SharedPtr<Widget>* parent);
			bool do_destroy();
			bool get_visible(bool* visible);
			bool set_visible(bool visible);
			bool get_enabled(bool* enabled);
			bool set_enable(bool enabled);
			bool get_focused(bool* focused);
			bool set_focus(bool focused);
			bool get_hilighted(bool* hilighted);
			bool set_hilight(bool hilighted);
		};
	}
}

#endif // INDIGO_GUIWIDGET_H_INCLUDED