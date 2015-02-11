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

#include "../lib/Window.h"

namespace Indigo
{
	bool start_render_thread(bool (*logic_thread)(const OOBase::Table<OOBase::String,OOBase::String>& args), const OOBase::Table<OOBase::String,OOBase::String>& config_args);

	bool render_call(bool (*fn)(void*), void* param);

	bool raise_event(void (*fn)(OOBase::CDRStream&), OOBase::CDRStream& stream);

	template <typename T>
	bool raise_event(T* pThis, void (T::*member_fn)())
	{
		struct thunk
		{
			static void member_event(OOBase::CDRStream& stream)
			{
				T* pThis;
				void (T::*fn)();
				if (!stream.read(pThis) || !stream.read(fn))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else
					(pThis->*fn)();
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text()),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(pThis) || !stream.write(member_fn))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::member_event,stream);
	}

	inline bool raise_event(void (*fn)())
	{
		struct thunk
		{
			static void event(OOBase::CDRStream& stream)
			{
				void (*fn)();
				if (!stream.read(fn))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else
					(*fn)();
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text()),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(fn))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::event,stream);
	}

	template <typename T, typename P1>
	bool raise_event(T* pThis, void (T::*member_fn)(P1), P1 p1)
	{
		struct thunk
		{
			static void member_event(OOBase::CDRStream& stream)
			{
				T* pThis;
				void (T::*fn)(P1 p1);
				P1 p1;
				if (!stream.read(pThis) || !stream.read(fn) || !stream.read(p1))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else
					(pThis->*fn)(p1);
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text()),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(pThis) || !stream.write(member_fn) || !stream.write(p1))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::member_event,stream);
	}

	template <typename P1>
	bool raise_event(void (*fn)(P1), P1 p1)
	{
		struct thunk
		{
			static void event(OOBase::CDRStream& stream)
			{
				void (*fn)();
				P1 p1;
				if (!stream.read(fn) | !stream.read(p1))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else
					(*fn)(p1);
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text()),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(fn) || !stream.write(p1))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::event,stream);
	}

	template <typename T, typename P1, typename P2>
	bool raise_event(T* pThis, void (T::*member_fn)(P1,P2), P1 p1, P2 p2)
	{
		struct thunk
		{
			static void member_event(OOBase::CDRStream& stream)
			{
				T* pThis;
				void (T::*fn)(P1 p1, P2 p2);
				P1 p1; P2 p2;
				if (!stream.read(pThis) || !stream.read(fn) || !stream.read(p1) || !stream.read(p2))
					LOG_ERROR(("Failed to unmarshal event parameters: %s",OOBase::system_error_text(stream.last_error())));
				else
					(pThis->*fn)(p1,p2);
			}
		};

		OOBase::RefPtr<OOBase::Buffer> buffer = OOBase::Buffer::create<OOBase::ThreadLocalAllocator>();
		if (!buffer)
			LOG_ERROR_RETURN(("Failed to allocate buffer: %s",OOBase::system_error_text()),false);

		OOBase::CDRStream stream(buffer);
		if (!stream.write(pThis) || !stream.write(member_fn) || !stream.write(p1) || !stream.write(p2))
			LOG_ERROR_RETURN(("Failed to marshal event parameters: %s",OOBase::system_error_text(stream.last_error())),false);

		return raise_event(&thunk::member_event,stream);
	}

	bool handle_events();

	int add_window(const OOBase::WeakPtr<Indigo::Window>& win);
}

#endif // INDIGO_RENDER_H_INCLUDED
