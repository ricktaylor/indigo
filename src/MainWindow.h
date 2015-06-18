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

#ifndef INDIGO_MAINWINDOW_H_INCLUDED
#define INDIGO_MAINWINDOW_H_INCLUDED

#include "GUILayer.h"
#include "Layer.h"

namespace Indigo
{
	class Application;
	class MainWindow;
	
	namespace Render
	{
		class MainWindow
		{
			friend class Indigo::MainWindow;

		public:
			MainWindow(Indigo::MainWindow* owner);

			OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator>& layers() { return m_layers; }
			
			glm::u16vec2 size() const;

			const OOBase::SharedPtr<OOGL::Window>& window() const;

		private:
			Indigo::MainWindow* m_owner;
			OOBase::SharedPtr<OOGL::Window> m_wnd;
			OOBase::Vector<OOBase::SharedPtr<Layer>,OOBase::ThreadLocalAllocator> m_layers;

			bool create();
			
			void on_close(const OOGL::Window& win);
			void on_draw(const OOGL::Window& win, OOGL::State& glState);
			void on_move(const OOGL::Window& win, const glm::u16vec2& pos);
			void on_size(const OOGL::Window& win, const glm::u16vec2& sz);
		};
	}

	class MainWindow : public OOBase::NonCopyable
	{
		friend class Render::MainWindow;
		
	public:
		MainWindow();
		~MainWindow();

		bool create(Application* app);
		void destroy();

		bool show();

		GUI::Layer& top_layer();
		
		const OOBase::SharedPtr<OOGL::Window>& window() const;

	private:
		Application* m_app;
		OOBase::SharedPtr<Render::MainWindow> m_wnd;
		GUI::Layer m_top_layer;

		void do_create(bool* ret_val);
		void do_show();
		void do_destroy();
		void on_close();
	};
}

#endif // INDIGO_MAINWINDOW_H_INCLUDED
