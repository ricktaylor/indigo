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

namespace Indigo
{
	class UIWidget;
	class UIGroup;

	namespace Render
	{
		class UIGroup;

		class UIDrawable : public OOBase::NonCopyable
		{
			friend class UIGroup;

		public:
			virtual bool valid() const { return true; }

			void show(bool visible = true) { m_visible = visible; }
			void position(glm::ivec2 pos) { m_position = pos; }

			virtual void size(glm::uvec2 sz) {}

		protected:
			UIDrawable(const glm::ivec2& position = glm::ivec2(0,0));
			virtual ~UIDrawable() {}

			bool m_visible;
			glm::ivec2 m_position;

			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const = 0;
		};

		class UIGroup : public UIDrawable
		{
			friend class Indigo::UIGroup;

		public:
			UIGroup(const glm::ivec2& position = glm::ivec2(0,0)) : UIDrawable(position)
			{}

			bool add_drawable(const OOBase::SharedPtr<UIDrawable>& drawable, unsigned int zorder);
			bool remove_drawable(unsigned int zorder);

		protected:
			virtual void on_draw(OOGL::State& glState, const glm::mat4& mvp) const;

		private:
			OOBase::Table<unsigned int,OOBase::SharedPtr<UIDrawable>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_children;

			void add_subgroup(UIWidget* widget, unsigned int zorder, bool* ret);
		};
	}

	class UIWidget : public OOBase::NonCopyable
	{
		friend class UIGroup;
		friend class UILayer;
		friend class Render::UIGroup;

	public:
		enum State
		{
			eWS_visible = 0x1,
			eWS_enabled = 0x2,
			eWS_max = (eWS_visible | eWS_enabled)
		};

		virtual ~UIWidget()
		{}

		UIGroup* parent() const { return m_parent; }

		virtual bool valid() const { return m_parent != NULL && m_render_group; }

		bool visible() const { return valid() && (m_state & eWS_visible); }
		void show(bool visible = true);

		bool enabled() const { return valid() && (m_state & eWS_enabled); }
		void enable(bool enabled = true);

		OOBase::uint32_t state() const { return m_state; }
		void state(OOBase::uint32_t new_state);

		const glm::ivec2& position() const { return m_position; }
		void position(const glm::ivec2& pos);

		const glm::uvec2& size() const { return m_size; }
		glm::uvec2 size(const glm::uvec2& sz);

		virtual glm::uvec2 min_size() const { return glm::uvec2(0); }
		virtual glm::uvec2 ideal_size() const = 0;

	protected:
		UIWidget(UIGroup* parent, OOBase::uint32_t state = 0, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0));

		template <typename T>
		OOBase::SharedPtr<T> render_group() const
		{
			return OOBase::static_pointer_cast<T>(m_render_group);
		}

		virtual bool on_render_create(Indigo::Render::UIGroup* group) = 0;
		virtual void on_size(const glm::uvec2& sz) { }
		virtual void on_state(OOBase::uint32_t new_state) { m_state = (new_state & eWS_max); }
		virtual bool on_mousemove(const glm::ivec2& pos) { return false; }

	private:
		UIGroup* m_parent;
		OOBase::SharedPtr<Indigo::Render::UIGroup> m_render_group;
		OOBase::uint32_t m_state;
		glm::ivec2 m_position;
		glm::uvec2 m_size;
	};

	class UIGroup : public UIWidget
	{
	public:
		UIGroup(UIGroup* parent, OOBase::uint32_t state = 0, const glm::ivec2& position = glm::ivec2(0), const glm::uvec2& size = glm::uvec2(0));

		bool add_widget(const OOBase::SharedPtr<UIWidget>& widget, unsigned int zorder);
		bool remove_widget(unsigned int zorder);

		template <typename Widget>
		OOBase::SharedPtr<Widget> get_widget(unsigned int zorder) const
		{
			OOBase::Table<unsigned int,OOBase::SharedPtr<UIWidget>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator>::const_iterator i = m_children.find(zorder);
			if (i == m_children.end())
				return OOBase::SharedPtr<Widget>();
			return OOBase::static_pointer_cast<Widget>(i->second);
		}

	protected:
		virtual glm::uvec2 min_size() const;
		virtual glm::uvec2 ideal_size() const;

		virtual bool on_mousemove(const glm::ivec2& pos);

	private:
		OOBase::Table<unsigned int,OOBase::SharedPtr<UIWidget>,OOBase::Less<unsigned int>,OOBase::ThreadLocalAllocator> m_children;
	};
}

#endif // INDIGO_UIWIDGET_H_INCLUDED
