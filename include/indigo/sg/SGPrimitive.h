///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2016 Rick Taylor
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

#ifndef INDIGO_SGPRIMITIVE_H_INCLUDED
#define INDIGO_SGPRIMITIVE_H_INCLUDED

#include "SGNode.h"

namespace Indigo
{
	class SGCube : public SGNode
	{
	public:
		struct CreateParams : public SGNode::CreateParams
		{
			CreateParams(OOBase::uint32_t state = 0,
					const glm::vec3& position = glm::vec3(),
					const glm::vec3& scaling = glm::vec3(),
					const glm::quat& rotation = glm::quat(),
					const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f)
			) :
				SGNode::CreateParams(state,position,scaling,rotation),
				m_colour(colour)
			{}

			glm::vec4               m_colour;
		};

		SGCube(SGGroup* parent, const CreateParams& params = CreateParams()) : 
				SGNode(parent,params),
				m_colour(params.m_colour)
		{}

	private:
		virtual OOBase::SharedPtr<Render::SGNode> on_render_create(Render::SGGroup* parent);

		glm::vec4 m_colour;
	};
}

#endif // INDIGO_SGPRIMITIVE_H_INCLUDED