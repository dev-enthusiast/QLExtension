/**************************************************************** -*-c++-*-
**
**	THREAD_ITERATOR.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/thread_iterator.h,v 1.7 2001/07/20 18:31:11 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_THREAD_ITERATOR_H
#define IN_GSTHREAD_THREAD_ITERATOR_H

#include <gsthread/thread_prim.h>

#include <iterator>
#include <vector>
#include <ccstl.h>

CC_BEGIN_NAMESPACE( Gs )

// this is a private class, but we need it outside.
template <class U_>
		class Thread_Creator_Proxy
		{
		public:
			Thread_Creator_Proxy(U_& thread_container) throw()
			: m_container(thread_container)
			{}

			// use default copy constructor
			// use default destructor
			// no assignment operator due to a member that is a reference

			Thread_Creator_Proxy& operator=(GsThreadRunnable* t)
			{
				m_container.push_back(GsSimpleThread(t)); // if the push_back fails, the thread is run to completion before the exception is propagated
				return *this;
			}

		private:
			U_& m_container;
		};

template <class T_>
		class Thread_Creator
		{
		public:
			// p 185 - need to do this the hard way because gcc 2.95.2 doesn't have an iterator class
			typedef CC_NS(std,output_iterator_tag) iterator_category;
			typedef void                           value_type;
			typedef void                           difference_type;
			typedef void                           pointer;
			typedef void                           reference;

			// p 98-99
			// default constructor - leaves the iterator in a singular state as allowed (ie. no operations may be called)
			Thread_Creator() throw()
			: m_container( 0 )
			{}

			// destructor - use default
			// copy constructor - use default
			// assignment operator - use default

			Thread_Creator( T_& cont ) throw()
			: m_container( &cont )
			{
			}

			// dereference assignment
			Thread_Creator_Proxy<T_> operator*() throw()
			{
				return *m_container; // uses Thread_Creator_Proxy<T_>::Thread_Creator_Proxy(T_&)
			}
  
			// pre and post increment - dummy
			Thread_Creator& operator++() throw() { return *this;}
			void operator++(int) throw() {}

		private:
			T_* m_container;
		};

template <class T_>
		class Thread_Creator_With_Helper
			:	private Thread_Creator< CC_STL_VECTOR( T_ ) >
{
private:
	typedef CC_STL_VECTOR( T_ ) C_; // work-around for lack of default template arguments
	typedef Thread_Creator<C_> Super;

public:
	typedef Super::iterator_category iterator_category;
	typedef Super::value_type        value_type;
	typedef Super::difference_type   difference_type;
	typedef Super::pointer           pointer;
	typedef Super::reference         reference;

	Thread_Creator_With_Helper() // throws iff C_'s constructor throws
	: Thread_Creator<C_>( m_container )
	{}

	// use default copy constructor
	// use default assignment operator
	// destructor - use default

	/*using*/ Super::operator*;
	/*using*/ Super::operator++;

private:
	C_ m_container;
};

template <class T_>
		inline Thread_Creator<T_> thread_creator( T_& cont )
{
	return Thread_Creator<T_>( cont );
}

inline Thread_Creator_With_Helper<GsSimpleThread> thread_creator()
{
	return Thread_Creator_With_Helper<GsSimpleThread>();
}

CC_END_NAMESPACE

#endif
