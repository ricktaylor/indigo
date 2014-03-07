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

#ifndef INDIGO_VIEWPORT_H_INCLUDED
#define INDIGO_VIEWPORT_H_INCLUDED

#include "State.h"

namespace Indigo
{
	class Framebuffer;
	class Camera;

	class Viewport : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<Viewport>
	{
		friend class Framebuffer;

	public:
		Viewport(const OOBase::SharedPtr<Framebuffer>& fb, const glm::ivec2& lower_left, const glm::ivec2& size);
		Viewport(const Viewport& rhs);
		~Viewport();

		Viewport& operator = (const Viewport& rhs);
		void swap(Viewport& rhs);

		OOBase::SharedPtr<Framebuffer> framebuffer() const;

		void move(const glm::ivec2& lower_left, const glm::ivec2& size);

		void lower_left(const glm::ivec2& lower_left);
		const glm::ivec2& lower_left() const;

		void size(const glm::ivec2& size);
		const glm::ivec2& size() const;

		void scissor(bool scissor);
		bool scissor() const;

		typedef OOBase::Vector<OOBase::SharedPtr<Indigo::Camera>,OOBase::ThreadLocalAllocator> cameras_t;
		const cameras_t& cameras() const;

	private:
		OOBase::WeakPtr<Framebuffer> m_fb;
		glm::ivec2                   m_lower_left;
		glm::ivec2                   m_size;
		bool                         m_scissor;
		cameras_t                    m_cameras;

		void render(State& gl_state);
	};
}

#endif // INDIGO_VIEWPORT_H_INCLUDED
