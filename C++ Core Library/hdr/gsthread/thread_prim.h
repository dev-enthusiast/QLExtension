/**************************************************************** -*-c++-*-
**
**	THREAD_PRIM.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/thread_prim.h,v 1.24 2001/07/18 18:40:26 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_THREAD_PRIM_H
#define IN_GSTHREAD_THREAD_PRIM_H


/*
 *
 *
 *
 *
 *
 *
 */

#include <gsthread/gsthread_base.h>		// for EXPORT macros
#include <extension/GsRefCountedPtr.h>		// for GsRefCountedPtr<T>

CC_BEGIN_NAMESPACE( Gs )

// ------------------------------------------- GsThreadRunnable (Java: Runnable)

/****************************************************************
** Class	   : GsThreadRunnable 
** Description : A interface which allows you to be run
**               using the pattern
**               without giving ownership to the thread of the
**               runnable.
****************************************************************/

class EXPORT_CLASS_GSTHREAD GsThreadRunnable
{
public:
	virtual ~GsThreadRunnable() throw() {}
    virtual void run() = 0;

// optional overrides
	virtual void expert_startup( bool orig = true ) throw() {}
	virtual void expert_shutdown( bool orig = true ) throw() {}
};

// -------------------------------------------- GsSimpleThread (Java: Thread)

// spawns a thread to perform a job, waits for job to complete before being destroyed
// similar to Java's Thread class
//
// These are handles to the thread.
// The last handle that refer's to a thread join()'s the thread upon destruction.
// For now, these handles should not be passed between threads.
//

class EXPORT_CLASS_GSTHREAD GsSimpleThread
{
public:
	explicit GsSimpleThread( GsThreadRunnable* work = 0 ); // can throw if new fails, or if a thread couldn't be created

	// cannot allow default definitions for copy/assignment/destructor as they would instantial an incomplete template for m_guts,
	// although the default behavior is exactly what we want (with the exception of inline).
	GsSimpleThread( GsSimpleThread const& ) throw();
	~GsSimpleThread() throw();

	GsSimpleThread& operator=( GsSimpleThread const& ) throw();

	void join();   // can throw
    void detach(); // can throw

	bool is_running() const throw(); // returns true if the undelying thread is running, ie. not joined or detached.
	                                 // since GsSimpleThreads should not be passed between threads, this is thread-safe :)

private:
	struct Raw_Thread_Guts;

	CC_NS(extension,GsRefCountedPtr)<Raw_Thread_Guts, int> m_guts;
};

CC_END_NAMESPACE

#endif
