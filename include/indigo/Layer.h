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

#ifndef INDIGO_LAYER_H_INCLUDED
#define INDIGO_LAYER_H_INCLUDED

#include <OOGL/Window.h>

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

		virtual ~Layer();

		OOBase::SharedPtr<Render::Layer> render_layer() const { return m_render_layer; }

		virtual OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window) = 0;
		virtual void destroy_render_layer();

		virtual void on_move(const glm::ivec2& sz) {}
		virtual void on_size(const glm::uvec2& sz) {}
		virtual bool on_mousemove(const double& screen_x, const double& screen_y) { return false; }
		virtual bool on_mousebutton(const OOGL::Window::mouse_click_t& click) { return false; }

	private:
		OOBase::SharedPtr<Render::Layer> m_render_layer;
		bool m_visible;
	};
}

#endif // INDIGO_LAYER_H_INCLUDED
