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

#include "../Image.h"

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

		virtual bool valid() const;
				
		bool load(const ResourceBundle& resource, const char* name, int components = 0);
		virtual bool load(const unsigned char* buffer, size_t len, int components = 0);

		const glm::uvec4& margins() const { return m_margins; }
		
		glm::uvec2 ideal_size() const;
		glm::uvec2 min_size() const;

		OOBase::SharedPtr<Render::UIDrawable> make_drawable(const glm::vec4& colour = glm::vec4(1.f), bool visible = true, const glm::ivec2& position = glm::ivec2(), const glm::uvec2& size = glm::uvec2()) const;

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
		OOBase::SharedPtr<NinePatch::Info> m_info;
	};

	namespace Render
	{
		class UINinePatch : public UIDrawable
		{
			friend class Indigo::NinePatch;

		public:
			UINinePatch(const OOBase::SharedPtr<OOGL::Texture>& texture, const glm::vec4& colour, const OOBase::SharedPtr<NinePatch::Info>& info, bool visible = true, const glm::ivec2& position = glm::ivec2(), const glm::uvec2& size = glm::uvec2());
			virtual ~UINinePatch();

			virtual void size(const glm::uvec2& size);

		private:
			void* const m_factory;
			OOBase::SharedPtr<OOGL::Texture> m_texture;
			glm::vec4  m_colour;
			GLsizei    m_patch;
			GLsizeiptr m_firsts[3];
			GLsizei    m_counts[3];

			OOBase::SharedPtr<NinePatch::Info> m_info;

			void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;
		};
	}

	class UINinePatch : public UIWidget
	{
	public:
		struct CreateParams : public UIWidget::CreateParams
		{
			CreateParams(OOBase::uint32_t state = eWS_visible,
					const glm::ivec2& position = glm::ivec2(),
					const glm::uvec2& size = glm::uvec2(),
					const OOBase::SharedPtr<NinePatch>& patch = OOBase::SharedPtr<NinePatch>(),
					const glm::vec4& colour = glm::vec4(1.f)
			) :
				UIWidget::CreateParams(state,position,size),
				m_patch(patch),
				m_colour(colour)
			{}

			OOBase::SharedPtr<NinePatch> m_patch;
			glm::vec4                    m_colour;
		};

		UINinePatch(UIGroup* parent, const CreateParams& params = CreateParams());

	protected:
		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_render_create(Render::UIGroup* group);

	private:
		OOBase::SharedPtr<NinePatch> m_9patch;
		Render::UIDrawable* m_render_9patch;

		glm::vec4 m_colour;

		void on_size(glm::uvec2& sz);
	};
}

#endif // INDIGO_FONT_H_INCLUDED
