/**************************************************************** -*-c++-*-
**
**	RESOURCE_QUEUE.H	- a thread-safe resource queue implementation
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/resource_queue.h,v 1.23 2011/05/16 20:35:42 khodod Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_RESOURCE_QUEUE_H
#define IN_GSTHREAD_RESOURCE_QUEUE_H

/*
 *  class GsResourceQueue<typename T_>
 *
 *    A thread-safe queue implementation.
 *
 *    Methods:
 *
 *      void add_resource(T_ const&);
 *        Operation       : Adds an element to the end of the queue.
 *        Exception Safety: Exception neutral/Strong.
 *                            T_'s copy constructor can throw anything
 *                            new can throw std::bad_alloc
 *        Thread Behavior : Can block for a short time.
 *                            Blocks only during a another write, or a read.
 *
 *      void take_resource(T_& arg);
 *        Operation       : Removes the first element from the queue and assigns the value to arg.
 *        Exception Safety: Exception neutral/Strong.
 *                            T_'s assignment operator can throw anything.
 *        Thread Behavior : Can block for a long time.
 *                            Blocks until the queue is non-empty.
 *
 *      T_ take_resource();
 *        Operation       : Removes the first element from the queue and returns a copy.
 *        Exception Safety: No throw.
 *        Thread Behavior : Can block for a long time.
 *                            Blocks until the queue is non-empty.
 *        NOTE: This method only exists only for types that have nothrow copy construction and assignment operators.
 *
 *      template <typename II>
 *      void add_resources(II begin, II end);
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

#include <deque>    // for std::deque<T>
#include <ccstl.h>  // for CC_STL_ macros

CC_BEGIN_NAMESPACE( Gs )

// see below
template <class T_>
	class visit_hack;

template <class T_>
class GsResourceQueue
	: private CC_NS(extension,GsNoCopy)< GsResourceQueue<T_> > 
      // redundant (because it contains a GsMutex and a Sepaphore, niether of which are copy-able), but just making it explicit
{
public:
	typedef typename CC_STL_DEQUE( T_ )::size_type size_type;

	GsResourceQueue() throw();
	~GsResourceQueue() throw();

	// no copy constructor due to GsNoCopy
	// no assignement operator due to GsNoCopy
  
	void add_resource(T_ const& res); // this can block for a short time - can throw if copy constructor or new fails
	void take_resource(T_&);          // this can block for a long time, even indefinitely (deadlock) - can throw if operator= throws
    bool trytake_resource(T_&);       // this will not block, returns true if a resource was successfully acquired

    void putback_resource(T_ const& res); // this can block for a short time - can throw if copy constructor or new fails

	bool is_empty();
	void until_empty();
	size_type  size( );

#if 0
	template <typename II_>
	    void add_resources( II_ begin, II_ end ); // can throw, offers only basic exception guarantee
#endif

	// safe iff copy constructor and assignment operator if T_ are nothrow.
	T_ take_resource() throw();

	friend class visit_hack<T_>;

protected:  // make these protected in the event that someone
	        // wants to use private/protected inheritance to steal 
        	// the implementation

	CC_STL_DEQUE( T_ ) m_queue;
	GsMutex              m_q_mutex; // guards m_queue
	GsConditionVariable  m_sig_more; // signalled when more items are added to the queue
	GsConditionVariable  m_sig_empty; // signalled when the queue is empty

	void m_take_resource( T_& );
};

// --------------------------------------------- implementation

template <class T_>
		GsResourceQueue<T_>::GsResourceQueue() throw()
		: m_queue(),
		  m_q_mutex(),
		  m_sig_more(m_q_mutex),
		  m_sig_empty(m_q_mutex)
{}

template <class T_>
		GsResourceQueue<T_>::~GsResourceQueue() throw()
{
	GsMutexLock l( m_q_mutex );// make sure no other thread is currently messing with the queue
	m_queue.clear();
}

template <class T_>
		void GsResourceQueue<T_>::add_resource( T_ const& res )
{
	{
		GsMutexLock l1(m_q_mutex);
		m_queue.push_back(res); // this push can throw
	}
	m_sig_more.signal(); // let at most one thread know that we have added an element to the queue
	                     // this does not need to be under the lock - potential performance penalty if it were
}

template <class T_>
		void GsResourceQueue<T_>::putback_resource( T_ const& res )
{
	{
		GsMutexLock l1(m_q_mutex);
		m_queue.push_front(res); // this push can throw
	}
	m_sig_more.signal(); // let at most one thread know that we have added an element to the queue
	                     // this does not need to be under the lock - potential performance penalty if it were
}

#if 0
template <typename T_>
template <typename II_>
void GsResourceQueue<T_>::add_resources( II_ begin, II_ const end )
{
    GsMutexLock l( m_q_mutex );
    while( begin != end )
    {
	    m_queue.push_back( *begin ); // II_::operator* can throw,
		                             // std::deque::push_back can throw if allocation fails,
		                             // T_::T_(T_ const&) can throw
		m_sig_more.signal();
		++begin;
    }
}
#endif

template <class T_>
		void GsResourceQueue<T_>::m_take_resource( T_& res )
{
	res = m_queue.front();    // front() is nothrow, T_::operator= may throw anything
	m_queue.pop_front();      // pop() is nothrow
	if( m_queue.empty())
		m_sig_empty.broadcast(); // let everyone know that we are empty
}

template <class T_>
		void GsResourceQueue<T_>::take_resource( T_& res )
{
	GsMutexLock l(m_sig_more.mutex());
	while( m_queue.empty()) m_sig_more.wait(); // wait until the queue is non-empty
	this->m_take_resource( res );
}

template <class T_>
		bool GsResourceQueue<T_>::trytake_resource( T_& res )
{
	GsMutexLock l(m_sig_more.mutex());
	if( m_queue.empty())
		return false; // nothing available

	this->m_take_resource( res );
	return true;
}

template <class T_>
		T_ GsResourceQueue<T_>::take_resource() throw()
{
	GsMutexLock l(m_sig_more.mutex());
	while( m_queue.empty()) m_sig_more.wait(); // wait until the queue is non-empty
	T_ res(m_queue.front()); // front() is nothrow, T_'s copy constructor may throw anything
	m_queue.pop_front();     // pop_front() is nothrow
	if( m_queue.empty())
		m_sig_empty.broadcast(); // let everyone know we are empty
	return res;              // potential leak- if T_'s copy constructor throws here when
	                         // initializing the return value, we lose the value. - use iff T_'s copy constructor is nothrow
}

template <class T_>
		bool GsResourceQueue<T_>::is_empty()
{
	GsMutexLock l(m_sig_empty.mutex());
	return m_queue.empty();
}

template <class T_>
		typename GsResourceQueue<T_>::size_type GsResourceQueue<T_>::size()
{
	GsMutexLock l(m_q_mutex);	
	size_type res = m_queue.size( );
	return res;
}

template <class T_>
		void GsResourceQueue<T_>::until_empty()
{
	GsMutexLock l(m_sig_empty.mutex());
	while( !m_queue.empty()) m_sig_empty.wait(); // wait until the queue is empty
}

template<class T_>
class visit_hack
{
public:
	visit_hack( GsResourceQueue<T_>& visitMe )
	:
	m_visitMe( visitMe )
	{
	}
protected:
	void visit( )
	{
		GsMutexLock l( m_visitMe.m_q_mutex );
		typename CC_STL_DEQUE( T_ )::iterator curr = m_visitMe.m_queue.begin( );
		typename CC_STL_DEQUE( T_ )::iterator end  = m_visitMe.m_queue.end( );
		for( ; curr != end ; ++curr )
		{
			operate( *curr );
		}
	}
	
	virtual void operate( T_& ref ) = 0;

private:
	GsResourceQueue<T_>& m_visitMe;
};

template<class T_, class BinOp>
class visit_hack_op : private visit_hack<T_>
{
public:
	visit_hack_op( GsResourceQueue<T_>& visitMe, BinOp op )
	:
	visit_hack<T_>( visitMe ),
	m_op( op )
	{
		this->visit( );
	}
	void operate( T_& ref )
	{
		m_op( ref );
	}
private:
	BinOp m_op;
};

template<class T_, class BinOp>
void resource_visit( GsResourceQueue<T_>& visitMe, BinOp op )
{
	visit_hack_op<T_, BinOp>( visitMe, op );
}

CC_END_NAMESPACE

#endif
