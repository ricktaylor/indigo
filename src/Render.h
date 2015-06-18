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

#ifndef INDIGO_RENDER_H_INCLUDED
#define INDIGO_RENDER_H_INCLUDED

#include "Common.h"
#include "../lib/Window.h"

namespace Indigo
{
	bool start_render_thread(bool (*logic_thread)(const OOBase::CmdArgs::options_t&,const OOBase::CmdArgs::arguments_t&), const OOBase::CmdArgs::options_t& options, const OOBase::CmdArgs::arguments_t& args);

	bool render_call(bool (*fn)(void*), void* param);

	template <typename Allocator>
	bool render_call(const OOBase::Delegate0<void,Allocator>& delegate)
	{
		struct thunk
		{
			OOBase::Delegate0<void,Allocator> const* m_delegate;

			static bool call(void* p)
			{
				thunk* t = static_cast<thunk*>(p);
				t->m_delegate->invoke();
				return true;
			}
		};

		thunk t;
		t.m_delegate = &delegate;
		return render_call(&thunk::call,&t);
	}

	template <typename P1, typename Allocator>
	bool render_call(const OOBase::Delegate1<void,P1,Allocator>& delegate, P1 p1)
	{
		struct thunk
		{
			OOBase::Delegate1<void,P1,Allocator> const* m_delegate;
			P1 const* m_p1;

			static bool call(void* p)
			{
				thunk* t = static_cast<thunk*>(p);
				t->m_delegate->invoke(*t->m_p1);
				return true;
			}
		};

		thunk t;
		t.m_delegate = &delegate;
		t.m_p1 = &p1;
		return render_call(&thunk::call,&t);
	}

	template <typename P1, typename P2, typename Allocator>
	bool render_call(const OOBase::Delegate2<void,P1,P2,Allocator>& delegate, P1 p1, P2 p2)
	{
		struct thunk
		{
			OOBase::Delegate2<void,P1,P2,Allocator> const* m_delegate;
			P1 const* m_p1;
			P2 const* m_p2;

			static bool call(void* p)
			{
				thunk* t = static_cast<thunk*>(p);
				t->m_delegate->invoke(*t->m_p1,*t->m_p2);
				return true;
			}
		};

		thunk t;
		t.m_delegate = &delegate;
		t.m_p1 = &p1;
		t.m_p2 = &p2;
		return render_call(&thunk::call,&t);
	}

	template <typename P1, typename P2, typename P3, typename Allocator>
	bool render_call(const OOBase::Delegate3<void,P1,P2,P3,Allocator>& delegate, P1 p1, P2 p2, P3 p3)
	{
		struct thunk
		{
			OOBase::Delegate3<void,P1,P2,P3,Allocator> const* m_delegate;
			P1 const* m_p1;
			P2 const* m_p2;
			P3 const* m_p3;

			static bool call(void* p)
			{
				thunk* t = static_cast<thunk*>(p);
				t->m_delegate->invoke(*t->m_p1,*t->m_p2,*t->m_p3);
				return true;
			}
		};

		thunk t;
		t.m_delegate = &delegate;
		t.m_p1 = &p1;
		t.m_p2 = &p2;
		t.m_p3 = &p3;
		return render_call(&thunk::call,&t);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename Allocator>
	bool render_call(const OOBase::Delegate4<void,P1,P2,P3,P4,Allocator>& delegate, P1 p1, P2 p2, P3 p3, P4 p4)
	{
		struct thunk
		{
			OOBase::Delegate4<void,P1,P2,P3,P4,Allocator> const* m_delegate;
			P1 const* m_p1;
			P2 const* m_p2;
			P3 const* m_p3;
			P4 const* m_p4;

			static bool call(void* p)
			{
				thunk* t = static_cast<thunk*>(p);
				t->m_delegate->invoke(*t->m_p1,*t->m_p2,*t->m_p3,*t->m_p4);
				return true;
			}
		};

		thunk t;
		t.m_delegate = &delegate;
		t.m_p1 = &p1;
		t.m_p2 = &p2;
		t.m_p3 = &p3;
		t.m_p4 = &p4;
		return render_call(&thunk::call,&t);
	}

	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename Allocator>
	bool render_call(const OOBase::Delegate5<void,P1,P2,P3,P4,P5,Allocator>& delegate, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
	{
		struct thunk
		{
			OOBase::Delegate5<void,P1,P2,P3,P4,P5,Allocator> const* m_delegate;
			P1 const* m_p1;
			P2 const* m_p2;
			P3 const* m_p3;
			P4 const* m_p4;
			P5 const* m_p5;

			static bool call(void* p)
			{
				thunk* t = static_cast<thunk*>(p);
				t->m_delegate->invoke(*t->m_p1,*t->m_p2,*t->m_p3,*t->m_p4,*t->m_p5);
				return true;
			}
		};

		thunk t;
		t.m_delegate = &delegate;
		t.m_p1 = &p1;
		t.m_p2 = &p2;
		t.m_p3 = &p3;
		t.m_p4 = &p4;
		t.m_p5 = &p5;
		return render_call(&thunk::call,&t);
	}

	bool raise_event(void (*fn)(OOBase::CDRStream&), OOBase::CDRStream& stream);

	inline bool raise_event(const OOBase::Delegate0<void,OOBase::CrtAllocator>& delegate)
	{
		struct thunk
		{
			static void event(OOBase::CDRStream& stream)
			{
				OOBase::Delegate0<void,OOBase::CrtAllocator> delegate;
				if (!stream.read(delegate))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else if (delegate)
					delegate.invoke();
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(delegate))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::event,stream);
	}

	template <typename P1>
	bool raise_event(const OOBase::Delegate1<void,P1,OOBase::CrtAllocator>& delegate, P1 p1)
	{
		struct thunk
		{
			static void member_event(OOBase::CDRStream& stream)
			{
				OOBase::Delegate1<void,P1,OOBase::CrtAllocator> delegate;
				P1 p1;
				if (!stream.read(delegate) || !stream.read(p1))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else
					delegate.invoke(p1);
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(delegate) || !stream.write(p1))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::member_event,stream);
	}

	template <typename P1, typename P2>
	bool raise_event(const OOBase::Delegate2<void,P1,P2,OOBase::CrtAllocator>& delegate, P1 p1, P2 p2)
	{
		struct thunk
		{
			static void member_event(OOBase::CDRStream& stream)
			{
				OOBase::Delegate2<void,P1,P2,OOBase::CrtAllocator> delegate;
				P1 p1; P2 p2;
				if (!stream.read(delegate) || !stream.read(p1) || !stream.read(p2))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else
					delegate.invoke(p1,p2);
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text(ERROR_OUTOFMEMORY)),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(delegate) || !stream.write(p1) || !stream.write(p2))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::member_event,stream);
	}

	bool quit_loop();
	bool handle_events();

	bool monitor_window(const OOBase::WeakPtr<OOGL::Window>& win);
}

#endif // INDIGO_RENDER_H_INCLUDED
