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

#ifndef INDIGO_GUILAYER_H_INCLUDED
#define INDIGO_GUILAYER_H_INCLUDED

#include "Layer.h"

namespace Indigo
{
	namespace GUIWidget
	{
		struct CreateParams
		{
			OOBase::uint32_t m_parent;
			bool             m_visible;
		};
	}

	namespace Render
	{
		class GUILayer;

		class GUIWidget
		{
		public:
			GUIWidget(const OOBase::SharedPtr<GUIWidget>& parent, const Indigo::GUIWidget::CreateParams* params);

			OOBase::SharedPtr<GUIWidget> parent() const;
			virtual GUILayer* layer() const;

			bool visible() const;
			bool visible(bool bShow = true);

			virtual bool add_widget(const OOBase::SharedPtr<GUIWidget>& widget);
			virtual bool remove_widget(const OOBase::SharedPtr<GUIWidget>& widget);

			virtual bool on_destroy() { return true; }
			virtual void on_draw(OOGL::State& glState);

		protected:
			OOBase::SharedPtr<GUIWidget> m_parent;
			bool m_visible;
			OOBase::Vector<OOBase::SharedPtr<GUIWidget>,OOBase::ThreadLocalAllocator> m_children;
		};

		class GUILayer : public Layer, public GUIWidget, public OOBase::EnableSharedFromThis<GUILayer>
		{
		public:
			GUILayer();
			
			GUILayer* layer() const;

			OOBase::uint32_t register_widget(const OOBase::SharedPtr<GUIWidget>& widget);
			bool unregister_widget(OOBase::uint32_t handle);
			OOBase::SharedPtr<GUIWidget> lookup_widget(OOBase::uint32_t handle) const;

		protected:
			void on_draw(const OOGL::Window& win, OOGL::State& glState);

		private:
			OOBase::uint32_t m_next_handle;
			OOBase::HashTable<OOBase::uint32_t,OOBase::SharedPtr<GUIWidget>,OOBase::ThreadLocalAllocator> m_handles;
		};

		class MainWindow;
	}

	class GUILayer
	{
	public:
		bool create(const OOBase::SharedPtr<Render::MainWindow>& wnd);
		void destroy();

		OOBase::uint32_t create_widget(const OOBase::Delegate2<OOBase::SharedPtr<Render::GUIWidget>,const OOBase::SharedPtr<Render::GUIWidget>&,const GUIWidget::CreateParams*>& delegate, const GUIWidget::CreateParams* p);
		bool destroy_widget(OOBase::uint32_t handle);

		bool show_widget(OOBase::uint32_t handle, bool visible = true);

	private:
		struct WidgetCreateParams
		{
			OOBase::uint32_t m_handle;
			const GUIWidget::CreateParams* m_params;
			const OOBase::Delegate2<OOBase::SharedPtr<Render::GUIWidget>,const OOBase::SharedPtr<Render::GUIWidget>&,const GUIWidget::CreateParams*>* m_delegate;
		};
		OOBase::SharedPtr<Render::GUILayer> m_layer;

		bool do_create_widget(WidgetCreateParams* p);
		bool do_destroy_widget(OOBase::uint32_t handle);
		bool do_show_widget(OOBase::uint32_t handle, bool* visible);
	};
}

#endif // INDIGO_GUILAYER_H_INCLUDED
