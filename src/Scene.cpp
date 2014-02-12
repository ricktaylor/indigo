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

#include "Scene.h"

void Indigo::Scene::cull(const glm::vec4 planes[6], const glm::vec4 abs_planes[6])
{
	// See http://fgiesen.wordpress.com/2010/10/17/view-frustum-culling/
			/* float d = dot(center, plane);
			  float r = dot(extent, absPlane);
			  if (d + r > 0) // partially inside
			  if (d - r >= 0) // fully inside */
}

void Indigo::Scene::draw(const glm::mat4& pv_matrix)
{

}
