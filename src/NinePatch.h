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

#ifndef INDIGO_NINEPATCH_H_INCLUDED
#define INDIGO_NINEPATCH_H_INCLUDED

#include "Common.h"

#include "../lib/State.h"
#include "../lib/Image.h"

namespace Indigo
{
	namespace Render
	{
		class NinePatch : public OOBase::NonCopyable
		{
		public:
			NinePatch();
			NinePatch(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size);
			~NinePatch();

			bool valid() const;

			void layout(const glm::u16vec2& size, const glm::u16vec4& borders, const glm::u16vec2& tex_size);

			void draw(OOGL::State& state, const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::mat4& mvp) const;

		private:
			GLsizei    m_patch;
			GLsizeiptr m_firsts[3];
			GLsizei    m_counts[3];
		};
	}
}

#endif // INDIGO_FONT_H_INCLUDED
