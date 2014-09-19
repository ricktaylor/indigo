///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2013 Rick Taylor
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

#ifndef INDIGO_WINDOWS_H_INCLUDED
#define INDIGO_WINDOWS_H_INCLUDED

#include "Render.h"
#include "Framebuffer.h"
#include "State.h"
#include "StateFns.h"

namespace Indigo
{
	class Window
	{
		friend class State;
		friend class StateFns;
		friend bool start_render_thread(bool (*logic_thread)(const OOBase::Table<OOBase::String,OOBase::String>& args), const OOBase::Table<OOBase::String,OOBase::String>& config_args);

	public:
		enum Style
		{
			eWSvisible = 1,
			eWSresizable = 2,
			eWSdecorated = 4
		};

		Window(int width, int height, const char* title, unsigned int style = eWSdecorated, GLFWmonitor* monitor = NULL);
		~Window();

		bool is_valid() const;

		bool is_visible() const;
		void visible(bool show);

		bool is_iconified() const;
		void iconify(bool minimize);

		glm::ivec2 size() const;

		const OOBase::SharedPtr<Framebuffer>& get_default_frame_buffer() const;

	// Signals
	public:
		OOBase::Signal1<const Window&,OOBase::ThreadLocalAllocator> signal_close;
		OOBase::Signal2<const Window&,State&,OOBase::ThreadLocalAllocator> signal_draw;
		OOBase::Signal2<const Window&,const glm::ivec2&,OOBase::ThreadLocalAllocator> signal_sized;

	private:
		GLFWwindow* m_glfw_window;

		OOBase::SharedPtr<Framebuffer> m_default_fb;
		OOBase::SharedPtr<State>       m_state;
		OOBase::SharedPtr<StateFns>    m_state_fns;

		static void on_size(GLFWwindow* window, int width, int height);
		static void on_close(GLFWwindow* window);
		static void on_focus(GLFWwindow* window, int focused);
		static void on_iconify(GLFWwindow* window, int iconified);
		static void on_refresh(GLFWwindow* window);

		static bool draw_thread(const OOBase::Table<OOBase::String,OOBase::String>& config_args);
		bool draw();
		void swap();
	};
}

#endif // INDIGO_WINDOWS_H_INCLUDED
