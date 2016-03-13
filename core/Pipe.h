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

#ifndef INDIGO_PIPE_H_INCLUDED
#define INDIGO_PIPE_H_INCLUDED

namespace Indigo
{
	namespace detail
	{
		namespace IPC
		{
			template <typename Allocator>
			struct thunk0
			{
				thunk0(const OOBase::Delegate0<void,Allocator>& d) :
					m_delegate(d)
				{}

				OOBase::Delegate0<void,Allocator> m_delegate;

				static void call(void* p)
				{
					static_cast<thunk0*>(p)->m_delegate.invoke();
				}

				static void cleanup(void* p)
				{
					OOBase::ThreadLocalAllocator::delete_free(static_cast<thunk0*>(p));
				}
			};

			template <typename Allocator, typename P1>
			struct thunk1
			{
				thunk1(const OOBase::Delegate1<void,P1,Allocator>& d, P1 p1) :
					m_delegate(d),
					m_p1(p1)
				{}

				OOBase::Delegate1<void,P1,Allocator> m_delegate;
				P1 m_p1;

				static void call(void* p)
				{
					thunk1* t = static_cast<thunk1*>(p);
					t->m_delegate.invoke(t->m_p1);
				}

				static void cleanup(void* p)
				{
					OOBase::ThreadLocalAllocator::delete_free(static_cast<thunk1*>(p));
				}
			};

			template <typename Allocator, typename P1, typename P2>
			struct thunk2
			{
				thunk2(const OOBase::Delegate2<void,P1,P2,Allocator>& d, P1 p1, P2 p2) :
					m_delegate(d),
					m_p1(p1),
					m_p2(p2)
				{}

				OOBase::Delegate2<void,P1,P2,Allocator> m_delegate;
				P1 m_p1;
				P2 m_p2;

				static void call(void* p)
				{
					thunk2* t = static_cast<thunk2*>(p);
					t->m_delegate.invoke(t->m_p1,t->m_p2);
				}

				static void cleanup(void* p)
				{
					OOBase::ThreadLocalAllocator::delete_free(static_cast<thunk2*>(p));
				}
			};

			template <typename Allocator, typename P1, typename P2, typename P3>
			struct thunk3
			{
				thunk3(const OOBase::Delegate3<void,P1,P2,P3,Allocator>& d, P1 p1, P2 p2, P3 p3) :
					m_delegate(d),
					m_p1(p1),
					m_p2(p2),
					m_p3(p3)
				{}

				OOBase::Delegate3<void,P1,P2,P3,Allocator> m_delegate;
				P1 m_p1;
				P2 m_p2;
				P3 m_p3;

				static void call(void* p)
				{
					thunk3* t = static_cast<thunk3*>(p);
					t->m_delegate.invoke(t->m_p1,t->m_p2,t->m_p3);
				}

				static void cleanup(void* p)
				{
					OOBase::ThreadLocalAllocator::delete_free(static_cast<thunk3*>(p));
				}
			};
		}

		namespace IPC
		{
			class Queue
			{
			public:
				typedef bool (*callback_t)(void* p);

				bool enqueue(callback_t callback, void* param = NULL);

				bool dequeue(bool call_blocked, const OOBase::Timeout& timeout = OOBase::Timeout());

				bool drain();

				void swap(Queue& rhs)
				{
					OOBase::swap(m_lock,rhs.m_lock);
					OOBase::swap(m_cond,rhs.m_cond);
					OOBase::swap(m_queue,rhs.m_queue);
				}

			protected:
				OOBase::Condition::Mutex m_lock;
				OOBase::Condition        m_cond;

				struct Item
				{
					Item() : m_callback(NULL), m_param(NULL)
					{}

					Item(callback_t c, void* p) : m_callback(c), m_param(p)
					{}

					callback_t m_callback;
					void*      m_param;
				};

				OOBase::Queue<Item,OOBase::ThreadLocalAllocator> m_queue;
			};
		}
	}

	class Pipe : public OOBase::NonCopyable
	{
		friend class OOBase::AllocateNewStatic<OOBase::ThreadLocalAllocator>;

	public:
		Pipe(const char* local);

		OOBase::SharedPtr<Pipe> open(const char* remote);

		bool poll(const OOBase::Timeout& timeout = OOBase::Timeout());
		bool drain();

		void close();

		bool call(void (*fn)(void*), void* param);

		template <typename Allocator>
		bool call(const OOBase::Delegate0<void,Allocator>& delegate)
		{
			struct thunk
			{
				thunk(OOBase::Delegate0<void,Allocator> const* d) :
					m_delegate(d)
				{}

				OOBase::Delegate0<void,Allocator> const* m_delegate;

				static void call(void* p)
				{
					static_cast<thunk*>(p)->m_delegate->invoke();
				}
			};

			thunk t(&delegate);
			return call(&thunk::call,&t);
		}

		template <typename P1, typename Allocator>
		bool call(const OOBase::Delegate1<void,P1,Allocator>& delegate, P1 p1)
		{
			struct thunk
			{
				thunk(OOBase::Delegate1<void,P1,Allocator> const* d, P1 const* p1) :
					m_delegate(d),
					m_p1(p1)
				{}

				OOBase::Delegate1<void,P1,Allocator> const* m_delegate;
				P1 const* m_p1;

				static void call(void* p)
				{
					thunk* t = static_cast<thunk*>(p);
					t->m_delegate->invoke(*t->m_p1);
				}
			};

			thunk t(&delegate,&p1);
			return call(&thunk::call,&t);
		}

		template <typename P1, typename P2, typename Allocator>
		bool call(const OOBase::Delegate2<void,P1,P2,Allocator>& delegate, P1 p1, P2 p2)
		{
			struct thunk
			{
				thunk(OOBase::Delegate2<void,P1,P2,Allocator> const* d, P1 const* p1, P2 const* p2) :
					m_delegate(d),
					m_p1(p1),
					m_p2(p2)
				{}

				OOBase::Delegate2<void,P1,P2,Allocator> const* m_delegate;
				P1 const* m_p1;
				P2 const* m_p2;

				static void call(void* p)
				{
					thunk* t = static_cast<thunk*>(p);
					t->m_delegate->invoke(*t->m_p1,*t->m_p2);
				}
			};

			thunk t(&delegate,&p1,&p2);
			return call(&thunk::call,&t);
		}

		template <typename P1, typename P2, typename P3, typename Allocator>
		bool call(const OOBase::Delegate3<void,P1,P2,P3,Allocator>& delegate, P1 p1, P2 p2, P3 p3)
		{
			struct thunk
			{
				thunk(OOBase::Delegate3<void,P1,P2,P3,Allocator> const* d, P1 const* p1, P2 const* p2, P3 const* p3) :
					m_delegate(d),
					m_p1(p1),
					m_p2(p2),
					m_p3(p3)
				{}

				OOBase::Delegate3<void,P1,P2,P3,Allocator> const* m_delegate;
				P1 const* m_p1;
				P2 const* m_p2;
				P3 const* m_p3;

				static void call(void* p)
				{
					thunk* t = static_cast<thunk*>(p);
					t->m_delegate->invoke(*t->m_p1,*t->m_p2,*t->m_p3);
				}
			};

			thunk t(&delegate,&p1,&p2,&p3);
			return call(&thunk::call,&t);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename Allocator>
		bool call(const OOBase::Delegate4<void,P1,P2,P3,P4,Allocator>& delegate, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			struct thunk
			{
				thunk(OOBase::Delegate4<void,P1,P2,P3,P4,Allocator> const* d, P1 const* p1, P2 const* p2, P3 const* p3, P4 const* p4) :
					m_delegate(d),
					m_p1(p1),
					m_p2(p2),
					m_p3(p3),
					m_p4(p4)
				{}

				OOBase::Delegate4<void,P1,P2,P3,P4,Allocator> const* m_delegate;
				P1 const* m_p1;
				P2 const* m_p2;
				P3 const* m_p3;
				P4 const* m_p4;

				static void call(void* p)
				{
					thunk* t = static_cast<thunk*>(p);
					t->m_delegate->invoke(*t->m_p1,*t->m_p2,*t->m_p3,*t->m_p4);
				}
			};

			thunk t(&delegate,&p1,&p2,&p3,&p4);
			return call(&thunk::call,&t);
		}

		template <typename P1, typename P2, typename P3, typename P4, typename P5, typename Allocator>
		bool call(const OOBase::Delegate5<void,P1,P2,P3,P4,P5,Allocator>& delegate, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			struct thunk
			{
				thunk(OOBase::Delegate5<void,P1,P2,P3,P4,P5,Allocator> const* d, P1 const* p1, P2 const* p2, P3 const* p3, P4 const* p4, P5 const* p5) :
					m_delegate(d),
					m_p1(p1),
					m_p2(p2),
					m_p3(p3),
					m_p4(p4),
					m_p5(p5)
				{}

				OOBase::Delegate5<void,P1,P2,P3,P4,P5,Allocator> const* m_delegate;
				P1 const* m_p1;
				P2 const* m_p2;
				P3 const* m_p3;
				P4 const* m_p4;
				P5 const* m_p5;

				static void call(void* p)
				{
					thunk* t = static_cast<thunk*>(p);
					t->m_delegate->invoke(*t->m_p1,*t->m_p2,*t->m_p3,*t->m_p4,*t->m_p5);
				}
			};

			thunk t(&delegate,&p1,&p2,&p3,&p4,&p5);
			return call(&thunk::call,&t);
		}

		bool post(void (*fn)(void*), void* param, void (*fn_cleanup)(void*) = NULL);

		template <typename Allocator>
		bool post(const OOBase::Delegate0<void,Allocator>& delegate)
		{
			typedef detail::IPC::thunk0<Allocator> thunk;

			thunk* t;
			if (!OOBase::ThreadLocalAllocator::allocate_new(t,delegate))
				LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

			if (!post(&thunk::call,t,&thunk::cleanup))
			{
				OOBase::ThreadLocalAllocator::delete_free(t);
				return false;
			}
			return true;
		}

		template <typename P1, typename Allocator>
		bool post(const OOBase::Delegate1<void,P1,Allocator>& delegate, P1 p1)
		{
			typedef detail::IPC::thunk1<Allocator,P1> thunk;

			thunk* t;
			if (!OOBase::ThreadLocalAllocator::allocate_new(t,delegate,p1))
				LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

			if (!post(&thunk::call,t,&thunk::cleanup))
			{
				OOBase::ThreadLocalAllocator::delete_free(t);
				return false;
			}
			return true;
		}

		template <typename P1, typename P2, typename Allocator>
		bool post(const OOBase::Delegate2<void,P1,P2,Allocator>& delegate, P1 p1, P2 p2)
		{
			typedef detail::IPC::thunk2<Allocator,P1,P2> thunk;

			thunk* t;
			if (!OOBase::ThreadLocalAllocator::allocate_new(t,delegate,p1,p2))
				LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

			if (!post(&thunk::call,t,&thunk::cleanup))
			{
				OOBase::ThreadLocalAllocator::delete_free(t);
				return false;
			}
			return true;
		}

		template <typename P1, typename P2, typename P3, typename Allocator>
		bool post(const OOBase::Delegate3<void,P1,P2,P3,Allocator>& delegate, P1 p1, P2 p2, P3 p3)
		{
			typedef detail::IPC::thunk3<Allocator,P1,P2,P3> thunk;

			thunk* t;
			if (!OOBase::ThreadLocalAllocator::allocate_new(t,delegate,p1,p2,p3))
				LOG_ERROR_RETURN(("Failed to allocate: %s",OOBase::system_error_text()),false);

			if (!post(&thunk::call,t,&thunk::cleanup))
			{
				OOBase::ThreadLocalAllocator::delete_free(t);
				return false;
			}
			return true;
		}

	private:
		Pipe(const OOBase::SharedPtr<detail::IPC::Queue>& send_pipe, const OOBase::SharedPtr<detail::IPC::Queue>& recv_pipe);

		OOBase::SharedPtr<detail::IPC::Queue> m_recv_queue;
		OOBase::SharedPtr<detail::IPC::Queue> m_send_queue;

		struct CallInfo
		{
			OOBase::SharedPtr<detail::IPC::Queue> m_reply;
			void (*m_fn)(void*);
			void (*m_fn_cleanup)(void*);
			void* m_param;
		};

		static bool make_call(void* param);
		static bool do_post(void* param);
		static bool post_cleanup(void* param);
	};
}

#endif // INDIGO_PIPE_H_INCLUDED
