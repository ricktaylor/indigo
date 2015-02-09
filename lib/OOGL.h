///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
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

#ifndef OOGL_H_INCLUDED
#define OOGL_H_INCLUDED

//////////////////////////////////////////////

#include <OOBase/CDRStream.h>
#include <OOBase/Queue.h>
#include <OOBase/Condition.h>
#include <OOBase/Thread.h>
#include <OOBase/Logger.h>
#include <OOBase/Table.h>
#include <OOBase/SignalSlot.h>

#if defined(_MSC_VER)
	//#include "Config_msvc.h"
#elif defined(HAVE_CONFIG_H)
	// Autoconf
	#include <Config.h>
#else
#error Need some kind of configure scipt!
#endif

#define GL_GLEXT_LEGACY
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// Include our own glext.h
#include "./glext.h"

#endif // OOGL_H_INCLUDED
