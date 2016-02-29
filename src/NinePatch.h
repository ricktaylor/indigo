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

#include "UILayer.h"
#include "Image.h"

namespace Indigo
{
	namespace Render
	{
		class NinePatch;
	}

	class NinePatch : public Image
	{
		friend class Render::NinePatch;

	public:
		NinePatch();
		virtual ~NinePatch();
				
		virtual bool load(const unsigned char* buffer, int len, int components = 0);
		virtual void unload();

		const glm::u16vec4& margins() const { return m_margins; }
		
		OOBase::SharedPtr<Render::NinePatch> make_drawable(const glm::u16vec2& position = glm::u16vec2(0), const glm::u16vec2& size = glm::u16vec2(0), bool visible = true, const glm::vec4& colour = glm::vec4(1.f)) const;

	private:
		bool pixel_cmp(int x, int y, bool black);
		bool scan_line(int line, glm::u16vec2& span);
		bool scan_column(int column, glm::u16vec2& span);
		bool get_bounds();

		glm::u16vec4 m_margins;
		
		struct Info
		{
			glm::u16vec4 m_borders;
			glm::u16vec2 m_tex_size;
			OOBase::SharedPtr<OOGL::Texture> m_texture;
		};
		OOBase::SharedPtr<Indigo::NinePatch::Info> m_info;

		 void do_unload();
	};

	namespace Render
	{
		class NinePatch : public UIDrawable
		{
			friend class Indigo::NinePatch;

		public:
			NinePatch(const glm::u16vec2& position, const glm::u16vec2& size, bool visible, const glm::vec4& colour, const OOBase::SharedPtr<Indigo::NinePatch::Info>& info);
			virtual ~NinePatch();

			bool valid() const;

			void layout(const glm::u16vec2& size);

		private:
			glm::vec4  m_colour;
			GLsizei    m_patch;
			GLsizeiptr m_firsts[3];
			GLsizei    m_counts[3];

			OOBase::SharedPtr<Indigo::NinePatch::Info> m_info;

			void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}
}

#endif // INDIGO_FONT_H_INCLUDED
