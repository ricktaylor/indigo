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

#include "Render.h"
#include "../lib/Window.h"

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

		protected:
			Layer(Window* window) : m_window(window)
			{}

			virtual void on_draw(OOGL::State& glState) const = 0;

			Window* const m_window;
		};
	}

	class Layer : public OOBase::NonCopyable
	{
		friend class Window;
		friend class Render::Window;

	public:
		virtual bool valid()
		{
			return m_render_layer;
		}

	protected:
		Layer()
		{}

		template <typename RenderLayer>
		OOBase::SharedPtr<RenderLayer> render_layer() const
		{
			return OOBase::static_pointer_cast<RenderLayer>(m_render_layer);
		}

		virtual OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window) = 0;

		virtual bool on_quit() { return false; }
		virtual void on_move(const glm::ivec2& sz) {}
		virtual void on_size(const glm::uvec2& sz) {}

	private:
		OOBase::SharedPtr<Render::Layer> m_render_layer;
	};

	class BlankingLayer : public Layer
	{
	public:
		BlankingLayer(const glm::vec4& colour);

		glm::vec4 colour(const glm::vec4& colour);

	private:
		glm::vec4 m_colour;

		OOBase::SharedPtr<Render::Layer> create_render_layer(Render::Window* window);
	};

	class Window;

	namespace Render
	{
		class Window
		{
			friend class Indigo::Window;
			friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

		public:
			const OOBase::SharedPtr<OOGL::Window>& window() const
			{
				return m_wnd;
			}

		private:
			Window(Indigo::Window* owner);

			OOBase::SharedPtr<OOGL::Window> m_wnd;
			Indigo::Window* const m_owner;

			OOBase::WeakPtr<OOGL::Window> create_window();

			OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_layers;

			void on_close(const OOGL::Window& win);
			void on_draw(const OOGL::Window& win, OOGL::State& glState);
			void on_move(const OOGL::Window& win, const glm::ivec2& pos);
			void on_size(const OOGL::Window& win, const glm::uvec2& sz);

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

		OOBase::WeakPtr<OOGL::Window> create();

		bool visible(bool show);

		bool add_layer(const OOBase::SharedPtr<Layer>& layer, unsigned int zorder);
		bool remove_layer(unsigned int zorder);

		template <typename Layer>
		OOBase::SharedPtr<Layer> get_layer(unsigned int zorder) const
		{
			OOBase::Table<unsigned int,OOBase::SharedPtr<Indigo::Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i = m_layers.find(zorder);
			if (i == m_layers.end())
				return OOBase::SharedPtr<Layer>();
			return OOBase::static_pointer_cast<Layer>(i->second);
		}

		unsigned int top_layer() const;

	private:
		OOBase::SharedPtr<Indigo::Render::Window> m_render_wnd;
		OOBase::Table<unsigned int,OOBase::SharedPtr<Layer>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_layers;

		void on_destroy();
		void on_close();
		void on_move(glm::ivec2 pos);
		void on_size(glm::uvec2 sz);
	};
}

#endif // INDIGO_Window_H_INCLUDED
