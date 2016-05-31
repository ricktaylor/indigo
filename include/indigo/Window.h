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
					unsigned int style = OOGL::Window::eWSresizable | OOGL::Window::eWSdecorated
			) :
				m_width(width),
				m_height(height),
				m_fullscreen(fullscreen),
				m_title(title),
				m_style(style)
			{}

			unsigned int m_width;
			unsigned int m_height;
			bool         m_fullscreen;
			const char*  m_title;
			unsigned int m_style; // From OOGL::Window::Style
		};

		bool create(const CreateParams& params = CreateParams());
		void destroy();

		bool show(bool visible = true);

		bool add_layer(const OOBase::SharedPtr<Layer>& layer, const char* name = NULL, size_t len = -1);
		bool remove_layer(const char* name, size_t len = -1);

		OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator> on_close(const OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator>& delegate);

	private:
		OOBase::SharedPtr<Indigo::Render::Window> m_render_wnd;
		OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator> m_layers;
		OOBase::HashTable<size_t,OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator> m_named_layers;

		OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator> m_on_close;

		void run();
		void on_create(const CreateParams* params, bool* ret);
		void on_destroy();
		void call_on_close();
		void on_move(glm::ivec2 pos);
		void on_size(glm::uvec2 sz);
		void on_mousemove(double screen_x, double screen_y);
		void on_mousebutton(OOGL::Window::mouse_click_t click);
	};

	namespace Render
	{
		class Window
		{
			friend class Indigo::Window;
			friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

		public:
			const OOBase::SharedPtr<OOGL::Window>& window() const { return m_wnd; }

		private:
			Window(Indigo::Window* owner);

			OOBase::SharedPtr<OOGL::Window> m_wnd;
			Indigo::Window* const m_owner;

			bool create_window(const Indigo::Window::CreateParams& params);

			OOBase::Vector<OOBase::WeakPtr<Layer>,OOBase::ThreadLocalAllocator> m_layers;

			void on_close(const OOGL::Window& win);
			void on_draw(const OOGL::Window& win, OOGL::State& glState);
			void on_move(const OOGL::Window& win, const glm::ivec2& pos);
			void on_size(const OOGL::Window& win, const glm::uvec2& sz);
			void on_mousemove(const OOGL::Window& win, double screen_x, double screen_y);
			void on_mousebutton(const OOGL::Window& win, const OOGL::Window::mouse_click_t& click);

			void add_render_layer(Indigo::Layer* layer, bool* ret);
		};
	}
}

#endif // INDIGO_WINDOW_H_INCLUDED
