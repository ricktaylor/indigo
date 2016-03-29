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

#ifndef INDIGO_NinePatch_H_INCLUDED
#define INDIGO_NinePatch_H_INCLUDED

#include "../core/Image.h"

#include "UIWidget.h"

namespace Indigo
{
	namespace Render
	{
		class UINinePatch;
	}

	class NinePatch : public Image
	{
		friend class Render::UINinePatch;

	public:
		NinePatch();
		virtual ~NinePatch();
				
		virtual bool load(const unsigned char* buffer, size_t len, int components = 0);

		const glm::uvec4& margins() const { return m_margins; }
		
		glm::uvec2 ideal_size() const;
		glm::uvec2 min_size() const;

		OOBase::SharedPtr<Render::UIDrawable> make_drawable(const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0), const glm::vec4& colour = glm::vec4(1.f)) const;

	private:
		bool pixel_cmp(int x, int y, bool black);
		bool scan_line(int line, glm::uvec2& span);
		bool scan_column(int column, glm::uvec2& span);
		bool get_bounds();

		glm::uvec4 m_margins;
		
		struct Info
		{
			glm::uvec4 m_borders;
			glm::uvec2 m_tex_size;
		};
		OOBase::SharedPtr<Indigo::NinePatch::Info> m_info;
	};

	namespace Render
	{
		class UINinePatch : public UIDrawable
		{
			friend class Indigo::NinePatch;

		public:
			UINinePatch(const glm::ivec2& position, const glm::uvec2& size, const glm::vec4& colour, const OOBase::SharedPtr<OOGL::Texture>& texture, const OOBase::SharedPtr<Indigo::NinePatch::Info>& info);
			virtual ~UINinePatch();

			virtual bool valid() const;

			virtual void size(glm::uvec2 size);

		private:
			OOBase::SharedPtr<OOGL::Texture> m_texture;
			glm::vec4  m_colour;
			GLsizei    m_patch;
			GLsizeiptr m_firsts[3];
			GLsizei    m_counts[3];

			OOBase::SharedPtr<Indigo::NinePatch::Info> m_info;

			void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}

	class UINinePatch : public UIWidget
	{
	public:
		UINinePatch(UIGroup* parent, const OOBase::SharedPtr<NinePatch>& patch, const glm::vec4& colour = glm::vec4(1.f), OOBase::uint32_t state = 0, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0));

	protected:
		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Indigo::Render::UIGroup* group);

	private:
		OOBase::SharedPtr<NinePatch> m_9patch;
		OOBase::SharedPtr<Render::UIDrawable> m_render_9patch;

		glm::vec4 m_colour;

		void on_size(const glm::uvec2& sz);
	};
}

#endif // INDIGO_FONT_H_INCLUDED
