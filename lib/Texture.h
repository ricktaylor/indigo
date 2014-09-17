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

#ifndef INDIGO_TEXTURE_H_INCLUDED
#define INDIGO_TEXTURE_H_INCLUDED

#include "OOGL.h"

namespace Indigo
{
	class State;

	class Texture : public OOBase::NonCopyable
	{
	public:
		Texture(GLenum type);
		~Texture();

		GLenum type() const;

		void bind(State* state, GLenum unit) const;

		bool is_valid() const;

	private:
		GLuint m_tex;
		GLenum m_type;
	};
}


#endif // INDIGO_TEXTURE_H_INCLUDED
