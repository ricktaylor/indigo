///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2014 Rick Taylor
//
// This file is part of the Indigo boardgame engine.
//
// OOGL is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OOGL is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OOGL.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////////////

#ifndef OOGL_H_INCLUDED
#define OOGL_H_INCLUDED

//////////////////////////////////////////////

#include <OOBase/Base.h>

#if defined(_MSC_VER)
	//#include "Config_msvc.h"
#elif defined(HAVE_CONFIG_H)
	// Autoconf
//	#include "Config.h"
#else
#error Need some kind of configure scipt!
#endif

#define GL_GLEXT_LEGACY
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// Include our own glext.h
#include "./glext.h"

namespace OOGL
{
	void glPrimitiveRestartIndex(GLuint index);
}

#if !defined(NDEBUG)
#define OOGL_CHECK(fn) OOGL::glCheckError(fn,__FILE__,__LINE__)
#else
#define OOGL_CHECK(fn)
#endif

#endif // OOGL_H_INCLUDED
