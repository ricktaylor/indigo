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

#include "Common.h"
#include "App.h"
#include "Thread.h"
#include "UILayer.h"

void Indigo::Application::start(OOBase::SharedPtr<Window> wnd, const OOBase::CmdArgs::options_t* options, const OOBase::CmdArgs::arguments_t* args)
{
	m_options = options;
	m_args = args;
	m_wnd = wnd;
}

void Indigo::Application::stop()
{

	thread_pipe()->close();
}

void Indigo::Application::on_close()
{
	m_wnd.reset();
}


/*#include "ZipResource.h"
#include "Font.h"
#include "GUILabel.h"
#include "GUIGridSizer.h"
#include "LuaAllocator.h"
#include "LuaResource.h"
#include "Pipe.h"

#include <setjmp.h>

namespace Indigo
{
	namespace Application
	{
		bool run(const OOBase::SharedPtr<Pipe>& ptrPipe, void* ctx);
	}

	OOGL::ResourceBundle& static_resources();
}

namespace
{
	jmp_buf panic_jmp_buf;

	int at_panic(lua_State *L)
	{
		LOG_ERROR(("Lua error: %s",lua_tostring(L,-1)));
		longjmp(panic_jmp_buf,1);
		return 0;
	}

	class Application
	{
	public:
		static bool run(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args);

		void on_main_wnd_close();

	private:
		void* m_wnd;
		Indigo::Lua::Allocator m_lua_allocator;
		lua_State* m_lua_state;

		Application();
		~Application();

		bool start(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args);
		bool start_lua(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args);

		bool create_mainwnd();

		bool show_menu();
	};
}

Indigo::Application::Application() : m_lua_state(NULL)
{
}

Indigo::Application::~Application()
{
	if (m_lua_state)
		lua_close(m_lua_state);
}

bool Indigo::Application::start(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args)
{
	//if (!showSplash())
	//	return false;

	if (!create_mainwnd())
		return false;

*/	/*OOBase::String strZip;
	if (!config_args.find("$1",strZip))
		LOG_ERROR_RETURN(("No zip file"),false);

	ZipResource zip;
	if (!zip.open(strZip.c_str()))
		return false;*/

/*	if (!start_lua(options,args))
		return false;

	show_menu();

	return  m_main_wnd.show();
}

bool Indigo::Application::start_lua(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args)
{
	m_lua_state = m_lua_allocator.lua_newstate();
	if (!m_lua_state)
		LOG_ERROR_RETURN(("Failed to create Lua state: %s", OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	bool ret = true;
	Lua::ResourceLoader loader(static_resources(),"init.lua");

	lua_CFunction old_panic = lua_atpanic(m_lua_state,&at_panic);
	if (!setjmp(panic_jmp_buf))
	{
		luaL_openlibs(m_lua_state);

		int r = loader.lua_load(m_lua_state);
		if (r == LUA_OK)
			r = lua_pcall(m_lua_state,0,LUA_MULTRET,0);

		if (r != LUA_OK)
		{
			LOG_ERROR(("Failed to load init.lua"));
			ret = false;
		}
	}
	else
		ret = false;

	lua_atpanic(m_lua_state,old_panic);

	return ret;
}

bool Indigo::Application::create_mainwnd()
{
	if (!m_main_wnd.create(this))
		return false;

	m_start_menu = OOBase::allocate_shared<GUI::Panel>();
	if (!m_start_menu)
		LOG_ERROR_RETURN(("Failed to create start_menu: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!m_start_menu->create(&m_main_wnd.top_layer()))
		return false;

	OOBase::SharedPtr<GUI::GridSizer> sizer = OOBase::allocate_shared<GUI::GridSizer>();
	if (!sizer)
		LOG_ERROR_RETURN(("Failed to create sizer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	if (!sizer->create(0,0) || !m_start_menu->sizer(sizer))
	{
		return false;
	}

	GUI::Sizer::ItemLayout layout = {0};
	layout.m_flags = GUI::Sizer::ItemLayout::align_centre;
	layout.m_proportion = 1;

	const char* items[] = 
	{
		" Quit ",
		" Credits ",
		" Load Game ",
		" New Game "
	};

	for (size_t i=0;i<sizeof(items)/sizeof(items[0]);++i)
	{
		OOBase::SharedPtr<GUI::Label> label = OOBase::allocate_shared<GUI::Label>();
		if (!label)
			LOG_ERROR_RETURN(("Failed to create label: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		OOBase::String str;
		str.assign(items[i]);

		if (!label->create(m_start_menu.get(),str) ||
			!label->visible(true) ||
			!sizer->add_widget(label,0,i,&layout))
		{
			return false;
		}
	}

	if (!m_start_menu->fit() || !m_start_menu->visible(true))
		return false;

	layout.m_flags = GUI::Sizer::ItemLayout::align_centre;

	if (!OOBase::static_pointer_cast<GUI::GridSizer>(m_main_wnd.top_layer().sizer())->add_widget(m_start_menu,0,0,&layout) ||
			!m_main_wnd.top_layer().layout())
		return false;

	return true;
}

bool Indigo::Application::show_menu()
{
	return m_main_wnd.top_layer().visible(true);
}

bool Indigo::Application::run(const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args)
{
	Application app;
	if (!app.start(options,args))
		return false;

	if (!Indigo::handle_events())
		return false;

	app.m_main_wnd.destroy();
	return true;
}

void Indigo::Application::on_main_wnd_close()
{
	OOBase::Logger::log(OOBase::Logger::Information,"Quit");
	quit_loop();
}


namespace
{
	const luaL_Reg indigolibs[] =
	{
//		{"window", &Indigo::Window::luaopen},
		{ NULL, NULL }
	};
}

bool Indigo::Application::run(const OOBase::SharedPtr<Pipe>& ptrPipe, void* ctx)
{
	Lua::Allocator lua_allocator;
	lua_State* lua_state = lua_allocator.lua_newstate();
	if (!lua_state)
		LOG_ERROR_RETURN(("Failed to create Lua state: %s", OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

	bool ret = true;
	Lua::ResourceLoader loader(static_resources(),"init.lua");

	lua_CFunction old_panic = lua_atpanic(lua_state,&at_panic);
	if (!setjmp(panic_jmp_buf))
	{
		luaL_openlibs(lua_state);

		// "require" functions from 'indigolibs' and set results to global table
		for (const luaL_Reg *lib = indigolibs; lib->func; lib++)
		{
			luaL_requiref(lua_state, lib->name, lib->func, 1);
			lua_pop(lua_state, 1); // remove lib
		}

		int r = loader.lua_load(lua_state);
		if (r == LUA_OK)
			r = lua_pcall(lua_state,0,LUA_MULTRET,0);

		if (r != LUA_OK)
		{
			LOG_ERROR(("Failed to load init.lua"));
			ret = false;
		}
		else
		{
			// Wait for quit
			ret = ptrPipe->poll();
		}
	}
	else
		ret = false;

	lua_atpanic(lua_state,old_panic);

	lua_close(lua_state);
	lua_state = NULL;

	return ret;
}

*/
