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

////////////////////////////////////////
// Try to work out what's going on with MS Windows
#if defined(HAVE_WINDOWS_H)
	#if !defined(_WIN32)
	#error No _WIN32 ?!?
	#endif

	// Prevent inclusion of old winsock
	#define _WINSOCKAPI_

	// Reduce the amount of windows we include
	#define WIN32_LEAN_AND_MEAN
	#if !defined(STRICT)
	#define STRICT
	#endif

	// We support Vista API's
	#if !defined(_WIN32_WINNT)
	#define _WIN32_WINNT 0x0600
	#elif _WIN32_WINNT < 0x0500
	#error Indigo requires _WIN32_WINNT >= 0x0500!
	#endif

	// We require IE 5 or later
	#if !defined(_WIN32_IE)
	#define _WIN32_IE 0x0500
	#elif _WIN32_IE < 0x0500
	#error Indigo requires _WIN32_IE >= 0x0500!
	#endif

	#if !defined(WINVER)
	#define WINVER _WIN32_WINNT
	#endif

	#include <windows.h>

	// Check for obsolete windows versions
	#if defined(_WIN32_WINDOWS)
	#error You cannot build Indigo for Windows 95/98/Me!
	#endif

	// Remove the unistd include - we are windows
	#if defined(HAVE_UNISTD_H)
	#undef HAVE_UNISTD_H
	#endif
#endif

namespace Indigo
{
	bool is_debug();


}

#endif // INDIGO_COMMON_H_INCLUDED
