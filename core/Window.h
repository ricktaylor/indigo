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

#ifndef INDIGO_Window_H_INCLUDED
#define INDIGO_Window_H_INCLUDED

#include "../oogl/Window.h"

#include "Render.h"

namespace Indigo
{
	class Window;

	namespace Render
	{
		class Window;

		class Layer : public OOBase::NonCopyable
		{
			friend class Window;

		public:
			void show(bool visible);

		protected:
			Layer(Window* window) : m_window(window), m_visible(false)
			{}

			virtual void on_draw(OOGL::State& glState) const = 0;
			virtual void on_size(const glm::uvec2& sz) {};

			Window* const m_window;
			bool m_visible;
		};
	}

	class Layer : public OOBase::NonCopyable
	{
		friend class Window;
		friend class Render::Window;

	public:
		virtual bool valid() const { return m_render_layer; }
		
		virtual void show(bool visible = true);
		bool visible() const { return m_visible; }

	protected:
		Layer() : m_visible(false)
		{}

		OOBase::SharedPtr<Render::Layer> render_layer() const { return m_render_layer; }

		virtual OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window) = 0;

		virtual void on_move(const glm::ivec2& sz) {}
		virtual void on_size(const glm::uvec2& sz) {}
		virtual bool on_mousemove(const double& screen_x, const double& screen_y) { return false; }
		virtual bool on_mousebutton(const OOGL::Window::mouse_click_t& click) { return false; }

	private:
		OOBase::SharedPtr<Render::Layer> m_render_layer;
		bool m_visible;
	};

	class Window;

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

			bool create_window();

			OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_layers;

			void on_close(const OOGL::Window& win);
			void on_draw(const OOGL::Window& win, OOGL::State& glState);
			void on_move(const OOGL::Window& win, const glm::ivec2& pos);
			void on_size(const OOGL::Window& win, const glm::uvec2& sz);
			void on_mousemove(const OOGL::Window& win, double screen_x, double screen_y);
			void on_mousebutton(const OOGL::Window& win, const OOGL::Window::mouse_click_t& click);

			void add_render_layer(Indigo::Layer* layer, unsigned int zorder, bool* ret);
		};
	}

	class Window : public OOBase::NonCopyable
	{
		friend class Render::Window;
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		
	public:
		Window();
		~Window();

		bool create();
		void destroy();

		bool show(bool visible = true);

		bool add_layer(const OOBase::SharedPtr<Layer>& layer, unsigned int zorder);
		OOBase::SharedPtr<Layer> get_layer(unsigned int zorder) const;
		bool remove_layer(unsigned int zorder);

		unsigned int top_layer() const;

		OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator> on_close(const OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator>& delegate);

	private:
		OOBase::SharedPtr<Indigo::Render::Window> m_render_wnd;
		OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_layers;

		OOBase::Delegate1<void,const Window&,OOBase::ThreadLocalAllocator> m_on_close;

		void run();
		void on_create(bool* ret);
		void on_destroy();
		void call_on_close();
		void on_move(glm::ivec2 pos);
		void on_size(glm::uvec2 sz);
		void on_mousemove(double screen_x, double screen_y);
		void on_mousebutton(OOGL::Window::mouse_click_t click);
	};
}

#endif // INDIGO_Window_H_INCLUDED
