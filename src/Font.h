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

#ifndef INDIGO_FONT_H_INCLUDED
#define INDIGO_FONT_H_INCLUDED

#include "../lib/State.h"

namespace OOGL
{
	class Font : public OOBase::NonCopyable
	{
		friend class Text;

	public:
		Font(const char* fnt_data, ...);
		~Font();

	private:
		void draw(State& state, const glm::mat4& mvp, const glm::vec4& colour);
	};

	class Text : public OOBase::NonCopyable
	{
	public:
		Text(const OOBase::SharedPtr<Font>& font, const char* sz = NULL, size_t len = size_t(-1));
		~Text();

		void text(const char* sz, size_t len = -1);
		const char* text() const;

		void draw(State& state, const glm::mat4& mvp, const glm::vec4& colour, size_t start = 0, size_t end = size_t(-1));

	private:
		OOBase::SharedPtr<Font> m_font;

		char*  m_sz;
		size_t m_len;
	};
}

#endif // INDIGO_FONT_H_INCLUDED
