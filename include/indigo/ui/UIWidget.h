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

#ifndef INDIGO_UIWIDGET_H_INCLUDED
#define INDIGO_UIWIDGET_H_INCLUDED

#include <OOBase/Vector.h>

#include <OOGL/Window.h>

namespace Indigo
{
	class UIWidget;
	class UIGroup;

	namespace Render
	{
		class UIGroup;

		class UIEventHandler
		{
		public:
			virtual bool on_mousebutton(const OOGL::Window::mouse_click_t& click) { return false; }
			virtual bool on_cursorenter(bool enter) { return false; }
			virtual bool on_cursormove() { return false; }
			virtual void on_losefocus() {}
		};

		class UIDrawable : public OOBase::NonCopyable, public OOBase::EnableSharedFromThis<UIDrawable>
		{
			friend class UIGroup;
			friend class UILayer;
			
		public:
			void show(bool visible = true) { m_visible = visible; }
			bool visible() const { return m_visible; }

			void position(const glm::ivec2& pos) { m_position = pos; }
			const glm::ivec2& position() const { return m_position; }

			virtual void size(const glm::uvec2& size) { m_size = size; }
			const glm::uvec2& size() const { return m_size; }

			OOBase::SharedPtr<UIEventHandler> event_handler(const OOBase::SharedPtr<UIEventHandler>& handler);
			const OOBase::SharedPtr<UIEventHandler>& event_handler() const { return m_event_handler; }

		protected:
			UIDrawable(bool visible, const glm::ivec2& position, const glm::uvec2& size);
			virtual ~UIDrawable();

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const = 0;

		private:
			bool       m_visible;
			glm::ivec2 m_position;
			glm::uvec2 m_size;

			OOBase::SharedPtr<UIEventHandler> m_event_handler;

			virtual void hit_test(OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>& hits, const glm::ivec2& pos);

			bool on_mousebutton(const OOGL::Window::mouse_click_t& click);
			bool on_cursorenter(bool enter);
			bool on_cursormove();
			void on_losefocus();
		};

		class UIGroup : public UIDrawable
		{
			friend class UILayer;
			friend class Indigo::UIGroup;

		public:
			UIGroup(bool visible = true, const glm::ivec2& position = glm::ivec2(), const glm::uvec2& size = glm::uvec2()) : UIDrawable(visible,position,size)
			{}

			bool add_drawable(const OOBase::SharedPtr<UIDrawable>& drawable);
			bool remove_drawable(const OOBase::SharedPtr<UIDrawable>& drawable);

		protected:
			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;

		private:
			OOBase::Vector<OOBase::SharedPtr<UIDrawable>,OOBase::ThreadLocalAllocator> m_children;

			void add_subgroup(UIWidget* widget, bool* ret);
			void hit_test(OOBase::Vector<OOBase::WeakPtr<UIDrawable>,OOBase::ThreadLocalAllocator>& hits, const glm::ivec2& pos);
		};
	}

	class UIWidget : public OOBase::NonCopyable
	{
		friend class UIGroup;
		friend class Render::UIGroup;

	public:
		enum State
		{
			eWS_visible = 0x1,
			eWS_enabled = 0x2
		};

		struct CreateParams
		{
			CreateParams(OOBase::uint32_t state = 0,
					const glm::ivec2& position = glm::ivec2(),
					const glm::uvec2& size = glm::uvec2()
			) :
				m_state(state),
				m_position(position),
				m_size(size)
			{}

			OOBase::uint32_t m_state;
			glm::ivec2       m_position;
			glm::uvec2       m_size;
		};

		virtual ~UIWidget() {}

		UIGroup* parent() const { return m_parent; }

		bool visible() const { return (m_state & eWS_visible) == eWS_visible; }
		virtual void show(bool visible = true);

		bool enabled() const { return (m_state & eWS_enabled) == eWS_enabled; }
		void enable(bool enabled = true);

		OOBase::uint32_t state() const { return m_state; }
		void toggle_state(OOBase::uint32_t new_state, OOBase::uint32_t mask);
		void toggle_state(bool on, OOBase::uint32_t mask) { toggle_state(on ? mask : 0,mask); }

		const glm::ivec2& position() const { return m_position; }
		void position(const glm::ivec2& pos);

		const glm::uvec2& size() const { return m_size; }
		const glm::uvec2& size(const glm::uvec2& sz);

		virtual glm::uvec2 min_size() const { return glm::uvec2(); }
		virtual glm::uvec2 ideal_size() const = 0;

	protected:
		UIWidget(UIGroup* parent, const CreateParams& params = CreateParams());

		virtual bool on_render_create(Render::UIGroup* group) = 0;
		virtual void on_size(glm::uvec2& sz) { }
		virtual void on_state_change(OOBase::uint32_t state, OOBase::uint32_t change_mask);

		virtual void make_dirty();

	private:
		UIGroup*         m_parent;
		Render::UIGroup* m_render_group;
		OOBase::uint32_t m_state;
		glm::ivec2       m_position;
		glm::uvec2       m_size;
	};

	class UIGroup : public UIWidget
	{
	public:
		UIGroup(UIGroup* parent, const CreateParams& params = CreateParams());

		bool add_widget(const OOBase::SharedPtr<UIWidget>& widget, const char* name = NULL, size_t len = -1);
		bool remove_widget(const OOBase::SharedPtr<UIWidget>& widget);

	protected:
		Render::UIGroup* m_render_parent;

		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

		virtual bool add_named_widget(const OOBase::SharedPtr<UIWidget>& widget, const char* name, size_t len = -1);

		virtual bool on_render_create(Render::UIGroup* group);

	private:
		OOBase::Vector<OOBase::SharedPtr<UIWidget>,OOBase::ThreadLocalAllocator> m_children;
	};
}

#endif // INDIGO_UIWIDGET_H_INCLUDED
