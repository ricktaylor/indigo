///////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015 Rick Taylor
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

#ifndef INDIGO_Label_H_INCLUDED
#define INDIGO_Label_H_INCLUDED

#include "../core/Font.h"

namespace Indigo
{
	namespace Render
	{
		class Label : public Text, public UIDrawable
		{
		public:
			Label(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len = -1, unsigned int size = 0, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), const glm::ivec2& position = glm::ivec2(0));

			void colour(const glm::vec4& colour) { m_colour = colour; }

		protected:
			glm::vec4 m_colour;
			float m_size;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};

		class ShadowLabel : public Label
		{
		public:
			ShadowLabel(const OOBase::SharedPtr<Font>& font, const char* sz, size_t len = -1, unsigned int size = 0, const glm::vec4& colour = glm::vec4(0.f,0.f,0.f,1.f), const glm::vec4& shadow = glm::vec4(.5f), const glm::ivec2& position = glm::ivec2(0), const glm::ivec2& drop = glm::ivec2(0,-1));

			void shadow(const glm::vec4& shadow) { m_shadow = shadow; }

		private:
			glm::vec4 m_shadow;
			glm::ivec2 m_drop;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}
}

#endif // INDIGO_Label_H_INCLUDED
