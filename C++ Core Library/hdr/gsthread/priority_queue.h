/**************************************************************** -*-c++-*-
**
**	RESOURCE_QUEUE.H	- a thread-safe resource queue implementation
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/priority_queue.h,v 1.3 2001/11/27 22:48:07 procmon Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_PRIOTITY_QUEUE_H
#define IN_GSTHREAD_PRIORITY_QUEUE_H

/*
 *  class GsPriorityQueue<typename T_>
 *
 *    A thread-safe priority queue implementation.
 *
 *    Methods:
 *
 *      void push(T_ const&);
 *        Operation       : Adds an element to the end of the queue.
 *        Exception Safety: Exception neutral/Strong.
 *                            T_'s copy constructor can throw anything
 *                            new can throw std::bad_alloc
 *        Thread Behavior : Can block for a short time.
 *                            Blocks only during a another write, or a read.
 *
 *      void pop(T_& arg);
 *        Operation       : Removes the first element from the queue and assigns the value to arg.
 *        Exception Safety: Exception neutral/Strong.
 *                            T_'s assignment operator can throw anything.
 *        Thread Behavior : Can block for a long time.
 *                            Blocks until the queue is non-empty.
 *
 *      T_ pop();
 *        Operation       : Removes the first element from the queue and returns a copy.
 *        Exception Safety: No throw.
 *        Thread Behavior : Can block for a long time.
 *                            Blocks until the queue is non-empty.
 *        NOTE: This method only exists only for types that have nothrow copy construction and assignment operators.
 *
 *      template <typename II>
 *      void push(II begin, II end);
 *        Operation       : Adds the elements defined by [begin, end) to the end of the queue.
 *                            The elements are added in order, and atomically.
 *        Exception Safety: Exception neutral/Basic.
 *                            If an exception is thrown on an attempt to add element X, the resulting state is that
 *                              the elements in [begin, X) have been added, but the elements [X, end) have not.
 *                            T_::T_(T_ const&) can throw anything.
 *                            std::queue::push can throw std::bad_alloc.
 *                            F_::operator* can throw anything.
 */

// ----------------------------------------------------------------------------- GsResourceQueue<T>

#include <gsthread/synch.h>      // for GsMutex/GsSemaphore
#include <extension/GsNoCopy.h>     // for GsNoCopy<T>

#include <queue>    // for std::priority_queque<T>
#include <ccstl.h>  // for CC_STL_ macros

CC_BEGIN_NAMESPACE( Gs )

template <class T_, class Comp_>
class GsPriorityQueue
	: private CC_NS(extension,GsNoCopy)< GsPriorityQueue<T_, Comp_> > 
      // redundant (because it contains a GsMutex and a Sepaphore, niether of which are copy-able), but just making it explicit
{
public:
	GsPriorityQueue() throw();
	~GsPriorityQueue() throw();

	// no copy constructor due to GsNoCopy
	// no assignement operator due to GsNoCopy
  
	void add_resource(T_ const& res); // this can block for a short time - can throw if copy constructor or new fails
	void take_resource(T_&);          // this can block for a long time, even indefinitely (deadlock) - can throw if operator= throws
    bool trytake_resource(T_&);       // this will not block, returns true if a resource was successfully acquired

protected: // make these protected in the event that someone wants to use private/protected inheritance to steal the implementation
	CC_STL_PRIORITY_QUEUE_WITH_COMP( T_, Comp_ ) m_queue;
	GsMutex              m_q_mutex; // guards m_queue
	GsSemaphore          m_sem;     // count of number of elements in the queue (= m_queue.size())

	void m_take_resource(T_&, GsSemaWait&);
};

// --------------------------------------------- implementation

template <class T_, class Comp_>
		GsPriorityQueue<T_, Comp_>::GsPriorityQueue() throw()
		: m_queue(),
		  m_q_mutex(),
		  m_sem(0) // start with an empty queue
{}

template <class T_, class Comp_>
		GsPriorityQueue<T_, Comp_>::~GsPriorityQueue() throw()
{
	GsMutexLock l( m_q_mutex );// make sure no other thread is currently messing with the queue
	while( !m_queue.empty())
		m_queue.pop();
}

template <class T_, class Comp_>
		void GsPriorityQueue<T_, Comp_>::add_resource( T_ const& res )
{
	{
		GsMutexLock l1(m_q_mutex);
		m_queue.push(res); // this push can throw
	}
	m_sem.post(); // let everyone know that we have added an element to the queue
	              // this does not need to be under the lock - potential performance penalty if it were
}

template <class T_, class Comp_>
		void GsPriorityQueue<T_, Comp_>::m_take_resource( T_& res, GsSemaWait& wt )
{
	GsMutexLock l(m_q_mutex); // we are going to modify the queue, so we need to grab the mutex
	res = m_queue.top();      // top() is nothrow, T_::operator= may throw anything
	m_queue.pop();            // pop() is nothrow
	wt.release();             // we succeeded - don't repost
}

template <class T_, class Comp_>
		void GsPriorityQueue<T_, Comp_>::take_resource( T_& res )
{
	GsSemaWait wt( m_sem ); // wait untill the queue is non-empty
	this->m_take_resource( res, wt );
}

template <class T_, class Comp_>
		bool GsPriorityQueue<T_, Comp_>::trytake_resource( T_& res )
{
    GsSemaWait wt( m_sem, true ); // check if the queue is non-empty
    if( !wt )
	    return false; // nothing available

	// not really a race condition as long as we are using semaphores
	// we would only suceed here if there was something to get

	this->m_take_resource( res, wt );
	return true;
}

CC_END_NAMESPACE

#endif
