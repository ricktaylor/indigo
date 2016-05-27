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

#ifndef INDIGO_STATE_H_INCLUDED
#define INDIGO_STATE_H_INCLUDED

#include <OOBase/SharedPtr.h>
#include <OOBase/Table.h>
#include <OOBase/HashTable.h>
#include <OOBase/List.h>

#include "OOGL.h"

namespace OOGL
{
	class StateFns;
	class Framebuffer;
	class Texture;
	class Program;
	class BufferObject;
	class VertexArrayObject;
	
	class State : public OOBase::NonCopyable
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;
		friend class StateFns;
		friend class Window;
		friend class Texture;
		friend class ImmutableTexture;
		friend class BufferObject;
		friend class VertexArrayObject;
		
	public:
		static OOBase::SharedPtr<State> get_current();

		OOBase::SharedPtr<Framebuffer> bind(GLenum target, const OOBase::SharedPtr<Framebuffer>& fb);
		OOBase::SharedPtr<Texture> bind(GLenum unit, const OOBase::SharedPtr<Texture>& texture);
		OOBase::SharedPtr<BufferObject> bind(const OOBase::SharedPtr<BufferObject>& buffer_object);
		OOBase::SharedPtr<VertexArrayObject> bind(const OOBase::SharedPtr<VertexArrayObject>& vao);

		OOBase::SharedPtr<Program> use(const OOBase::SharedPtr<Program>& program);

		GLuint active_texture_unit() const;

		bool get_singleton(const void* key, void** val);
		bool set_singleton(const void* key, void* val, void (*destructor)(void*) = NULL);

		bool enable(GLenum cap, bool enable = true);
		bool disable(GLenum cap) { return enable(cap,false); }

		struct viewport_t
		{
			GLint x;
			GLint y;
			GLsizei width;
			GLsizei height;
		};
		viewport_t viewport(const viewport_t& vp);
		const viewport_t& viewport() const;


	private:
		StateFns&                            m_state_fns;
		OOBase::SharedPtr<Framebuffer>       m_draw_fb;
		OOBase::SharedPtr<Framebuffer>       m_read_fb;
		GLuint                               m_active_texture_unit;
		OOBase::SharedPtr<Program>           m_current_program;
		OOBase::SharedPtr<VertexArrayObject> m_current_vao;
		viewport_t                           m_viewport;
		
		struct tex_pair
		{
			GLuint texture;
			OOBase::SharedPtr<Texture> tex_ptr;
		};
		typedef OOBase::HashTable<GLuint,tex_pair,OOBase::ThreadLocalAllocator> tex_unit_t;
		OOBase::Vector<tex_unit_t,OOBase::ThreadLocalAllocator> m_vecTexUnits;
		void bind_texture(GLuint texture, GLenum target);
		void update_texture_binding(GLuint texture, GLenum target);

		void bind_buffer(GLuint buffer, GLenum target);
		OOBase::HashTable<GLenum,OOBase::SharedPtr<BufferObject>,OOBase::ThreadLocalAllocator> m_buffer_objects;

		OOBase::HashTable<GLenum,bool,OOBase::ThreadLocalAllocator> m_enables;

		State(StateFns& fns);
		~State();
		void reset();

		GLuint activate_texture_unit(GLuint unit);
		OOBase::SharedPtr<OOGL::VertexArrayObject> unbind_vao();

		void update_bind(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target);
		OOBase::SharedPtr<BufferObject> bind_buffer_target(const OOBase::SharedPtr<BufferObject>& buffer_object, GLenum target);
		OOBase::SharedPtr<Texture> bind_texture_active_unit(const OOBase::SharedPtr<Texture>& texture);

		// Singleton support
		OOBase::HashTable<const void*,void*,OOBase::ThreadLocalAllocator> m_mapSingletons;
		struct singleton_t
		{
			const void* m_key;
			void (*m_destructor)(void*);
		};
		OOBase::List<singleton_t,OOBase::ThreadLocalAllocator> m_listSingletonDestructors;
	};

	template <typename T>
	class ContextSingleton : public OOBase::NonCopyable
	{
	public:
		static T* instance_ptr()
		{
			void* inst = NULL;
			if (!State::get_current()->get_singleton(reinterpret_cast<void*>(&ContextSingleton<T>::init),&inst))
				inst = init();

			return static_cast<T*>(inst);
		}

		static T& instance()
		{
			T* i = instance_ptr();
			if (!i)
				OOBase_CallCriticalFailure("Null instance pointer");

			return *i;
		}

	private:
		static void* init()
		{
			// We do this long-hand so T can friend us
			void* t = OOBase::ThreadLocalAllocator::allocate(sizeof(T),OOBase::alignment_of<T>::value);
			if (!t)
				OOBase_CallCriticalFailure(ERROR_OUTOFMEMORY);

			// Add destructor before calling constructor
			if (!State::get_current()->set_singleton(reinterpret_cast<void*>(&ContextSingleton<T>::init),t,&destroy))
			{
				OOBase::ThreadLocalAllocator::free(t);
				OOBase_CallCriticalFailure(ERROR_OUTOFMEMORY);
			}

#if defined(OOBASE_HAVE_EXCEPTIONS)
			try
			{
#endif
				::new (t) T();
#if defined(OOBASE_HAVE_EXCEPTIONS)
			}
			catch (...)
			{
				OOBase::ThreadLocalAllocator::free(t);
				throw;
			}
#endif
			return t;
		}

		static void destroy(void* p)
		{
			if (p)
				OOBase::ThreadLocalAllocator::delete_free(static_cast<T*>(p));
		}
	};

	void glCheckError(const char* fn, const char* file, unsigned int line);
}

#endif // INDIGO_STATE_H_INCLUDED
