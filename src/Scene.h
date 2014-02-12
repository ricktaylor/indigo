//////////////////////////////////////////////////////////////////////////////////
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

#ifndef INDIGO_SCENE_H_INCLUDED
#define INDIGO_SCENE_H_INCLUDED

#include "Common.h"

namespace Indigo
{
	class Camera;

	class Scene
	{
		friend class Camera;
	public:

	private:
		void cull(const glm::vec4 planes[6], const glm::vec4 abs_planes[6]);
		void draw(const glm::mat4& pv_matrix);
	};
}

#endif // INDIGO_SCENE_H_INCLUDED
