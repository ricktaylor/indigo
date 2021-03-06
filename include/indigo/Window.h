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

#ifndef INDIGO_WINDOW_H_INCLUDED
#define INDIGO_WINDOW_H_INCLUDED

#include <OOGL/Window.h>

#include <OOBase/Vector.h>
#include <OOBase/HashTable.h>

#include "Layer.h"

namespace Indigo
{
	class Window : public OOBase::NonCopyable
	{
		friend class Render::Window;
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		
	public:
		Window();
		~Window();

		struct CreateParams
		{
			CreateParams(
					unsigned int width = 0, 
					unsigned int height = 0,
					bool fullscreen = false,
					const char* title = NULL,
					unsigned int style = OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated,
					unsigned int msaa_samples = 0
			) :
				m_width(width),
				m_height(height),
				m_fullscreen(fullscreen),
				m_title(title),
				m_style(style),
				m_msaa_samples(msaa_samples)
			{}

			unsigned int m_width;
			unsigned int m_height;
			bool         m_fullscreen;
			const char*  m_title;
			unsigned int m_style; // From OOGL::Window::Style
			unsigned int m_msaa_samples;
		};

		bool create(const CreateParams& params = CreateParams());
		void destroy();

		bool show(bool visible = true);

		bool add_layer(const OOBase::SharedPtr<Layer>& layer, const char* name = NULL, size_t len = -1);
		bool remove_layer(const char* name, size_t len = -1);
		bool remove_layer(const OOBase::SharedPtr<Layer>& layer);

	private:
		OOBase::SharedPtr<Indigo::Render::Window> m_render_wnd;
		OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator> m_layers;
		OOBase::HashTable<size_t,OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator> m_named_layers;

		void run();
		void on_create(const CreateParams& params, bool* ret);
		void on_destroy();
		void on_close();
		void on_move(const glm::ivec2& pos);
		void on_iconify(bool iconified);
	};

	namespace Render
	{
		class Window
		{
			friend class Indigo::Window;
			friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

		public:
			const OOBase::SharedPtr<OOGL::Window>& window() const { return m_wnd; }

			void grab_focus(Layer* layer);
			const glm::dvec2& cursor_pos() const { return m_cursor_pos; }

		private:
			Window(Indigo::Window* owner);
			~Window();

			OOBase::SharedPtr<OOGL::Window> m_wnd;
			Indigo::Window* const m_owner;

			bool create_window(const Indigo::Window::CreateParams& params);

			OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator> m_layers;

			void on_close(const OOGL::Window& win);
			void on_iconify(const OOGL::Window&, bool iconified);
			void on_draw(const OOGL::Window& win, OOGL::State& glState);
			void on_move(const OOGL::Window& win, const glm::ivec2& pos);
			void on_size(const OOGL::Window& win, const glm::uvec2& sz);
			void on_mousebutton(const OOGL::Window& win, const OOGL::Window::mouse_click_t& click);
			void on_cursorenter(const OOGL::Window& win, bool enter);
			void on_cursormove(const OOGL::Window& win, const glm::dvec2& pos);
			void on_focus(const OOGL::Window& win, bool focused);
			void on_scroll(const OOGL::Window& win, const glm::dvec2& pos);

			void add_render_layer(Indigo::Layer* layer, bool* ret);
			void remove_render_layer(Indigo::Layer* layer);

			bool m_have_cursor;
			bool m_dirty;
			glm::dvec2 m_cursor_pos;
			OOBase::WeakPtr<Layer> m_cursor_layer;
			OOBase::WeakPtr<Layer> m_focus_layer;
		};
	}
}

#endif // INDIGO_WINDOW_H_INCLUDED
