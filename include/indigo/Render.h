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

#ifndef INDIGO_RENDER_H_INCLUDED
#define INDIGO_RENDER_H_INCLUDED

#include "Pipe.h"

namespace Indigo
{
	const OOBase::SharedPtr<Pipe>& render_pipe();
	const OOBase::SharedPtr<Pipe>& logic_pipe();

#if !defined(NDEBUG)
	void assert_render_thread();
#endif
}

#if !defined(NDEBUG)
#define ASSERT_RENDER_THREAD() (Indigo::assert_render_thread())
#else
#define ASSERT_RENDER_THREAD()
#endif

#endif // INDIGO_RENDER_H_INCLUDED
