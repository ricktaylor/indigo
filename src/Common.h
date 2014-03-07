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

#ifndef INDIGO_COMMON_H_INCLUDED
#define INDIGO_COMMON_H_INCLUDED

// Mingw can include pid_t etc, which we don't want
#if defined(__MINGW32__)
#define _NO_OLDNAMES
#endif

//////////////////////////////////////////////

#include <OOBase/SharedPtr.h>
#include <OOBase/Vector.h>
#include <OOBase/CDRStream.h>
#include <OOBase/TLSSingleton.h>
#include <OOBase/Environment.h>
#include <OOBase/Posix.h>
#include <OOBase/BoundedQueue.h>
#include <OOBase/StackAllocator.h>
#include <OOBase/HandleTable.h>
#include <OOBase/Thread.h>
#include <OOBase/Socket.h>
#include <OOBase/Set.h>
#include <OOBase/String.h>
#include <OOBase/CmdArgs.h>
#include <OOBase/Win32Security.h>
#include <OOBase/Logger.h>
#include <OOBase/ConfigFile.h>
#include <OOBase/Server.h>
#include <OOBase/File.h>
#include <OOBase/Singleton.h>
#include <OOBase/SignalSlot.h>

#if defined(_MSC_VER)
	//#include "Config_msvc.h"
#elif defined(HAVE_CONFIG_H)
	// Autoconf
	#include <Config.h>
#else
#error Need some kind of configure scipt!
#endif

namespace Indigo
{
	bool is_debug();
}

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#if defined(HAVE_GL_GLEXT_H)
#include <GL/glext.h>
#else
// Include our own glext.h
#include "glext.h"
#endif

#endif // INDIGO_COMMON_H_INCLUDED
