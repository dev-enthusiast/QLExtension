/**************************************************************** -*-c++-*-
**
**	THREAD.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/thread.h,v 1.15 2001/08/07 16:04:07 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_THREAD_H
#define IN_GSTHREAD_THREAD_H

/*
 *  class Thread_Error;
 *
 *  template <typename FI>
 *  void make_threads( FI begin, FI end );
 *
 *  template <typename FI, typename OI>
 *  void make_threads( FI begin, FI end, OI out );
 *
 *  class Thread_Pool;
 *
 *  class GsThreadRunnableWrapper<typename T>;
 *
 *    GsThreadRunnableWrapper publicly inherits from GsThreadRunnable, and IS-A GsThreadRunnable.
 *    Type requirements: T is a model of Generator. (ie T::operator() must exist and be publicly accessible)
 *                       T may modify its state.    (ie T::operator() may be non-const)
 *                       T must be copy-constructable. (either T(T const&), or T(T&))
 *                       T must be destructable.
 *
 *
 *  template <typename T>
 *  GsThreadRunnable* make_runnable( T obj );
 *
 *    Returns a pointer to a new GsThreadRunnable that will invoke the function object obj when run.
 *    Type requirements: same as for GsThreadRunnableWrapper. (see above)
 *
 *    (NOTE: function objects are modeled on function pointers, and so are typically
 *           passed by value.  So actually, when the GsThreadRunnable is run, a copy of obj is invoked.)
 *    Always be careful about exceptions.  This function returns a raw pointer, very dangerous!
 *
 */

#include <gsthread/gsthread_base.h>  // for EXPORT macros for silly dll crap
#include <gsthread/resource_queue.h> // for GsResourceQueue<T>
#include <gsthread/thread_prim.h>    // for GsSimpleThread, GsThreadRunnable

#include <exception> // for std::exception
#include <iterator>  // for std::back_inserter
#include <vector>    // for std::vector
#include <ccstl.h>   // for CC_STL_ macros

CC_BEGIN_NAMESPACE( Gs )

// ------------------------------------------- Thread_Error (not used yet)

struct EXPORT_CLASS_GSTHREAD Thread_Error
	: public CC_NS(std,exception)
{
	Thread_Error(int) throw() {}
};

// ------------------------------------------- make_threads

// basic exception safety
template <class FI, class OI>
void make_threads( FI begin, FI end, OI out )
{
	while( begin != end )
	{
		*out = GsSimpleThread( *begin );
		*begin = 0; // if we successfully created a new thread, remove the job from the container
		++out;
		++begin;
	}
}

// basic exception safety, all the threads created are completed before we return,
// if there was an exception creating a thread, no more jobs are removed.
template <class FI>
void make_threads( FI begin, FI end )
{
	CC_USING( std::back_inserter );
	CC_STL_VECTOR( GsSimpleThread ) thread_vec;
	make_threads( begin, end, back_inserter( thread_vec ));
}

template <class FI, class RT>
void make_thread_clones( FI begin, FI end, RT const& orig )
{
	while( begin != end )
	{
		*begin = GsSimpleThread( new RT(orig));
		++begin;
	}
}

// ----------------------------------------------------------------------------- Thread_Pool

typedef GsResourceQueue<GsThreadRunnable*> GsThreadRunnable_Queue; // this /should/ not instantiate, but VC6 sucks

class EXPORT_CLASS_GSTHREAD Thread_Pool // cannot be copied because the m_run_queue member cannot be copied
{
public:
	explicit Thread_Pool( int num_threads );
	~Thread_Pool() throw();

	// no copy constructor as m_run_queue is not copyable
	// no assignment operator as m_run_queue is no assignable.

    void add_thread( GsThreadRunnable* job ) { m_run_queue.add_resource( job );}

	bool is_empty() { return m_run_queue.is_empty();}
	void until_empty() { m_run_queue.until_empty();}

	void until_done() { this->until_empty(); GsMutexLock ll( m_lock ); while( m_running ) m_no_runners.wait(); }

#if 0
	template <class II>
	void make_threads( II begin, II end )
    {
	    m_run_queue.add_resources( begin, end );
	}
#endif
  
private:
	GsThreadRunnable_Queue m_run_queue;
	GsMutex m_lock;
	int m_running;
	GsConditionVariable m_no_runners;

	// these must be BEFORE m_pooled_threads
	CC_STL_VECTOR( GsSimpleThread ) m_pooled_threads;

	class Pool_GsThreadRunnable;
	friend class Pool_GsThreadRunnable;
};

class EXPORT_CLASS_GSTHREAD Pool_Adder
{
public:
    explicit Pool_Adder( Thread_Pool& pool ) : m_pool( pool ) {}
    void operator()( GsThreadRunnable* job ) const { m_pool.add_thread( job );}
private:
    Thread_Pool& m_pool;
};

// --------------------------------------------------------------------------------
// template helpers

// -------------------------------------- make_job

template <class T_>
class GsThreadRunnableWrapper
  : public GsThreadRunnable
{
public:
	explicit GsThreadRunnableWrapper( T_ const& func_obj )
	: m_func( func_obj )
	{} // will throw iff T_'s copy constructor throws

	// use default destructor

	void run() { m_func();} // can throw whatever func() throws

	T_ m_func;
};

template <class T_>
GsThreadRunnable* make_runnable( T_ const& obj )
{
	return new GsThreadRunnableWrapper<T_>( obj );
}

CC_END_NAMESPACE

#endif
