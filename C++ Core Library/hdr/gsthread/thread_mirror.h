/**************************************************************** -*-c++-*-
**
**	THREAD_PRIM.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/thread_mirror.h,v 1.3 2001/07/18 18:40:25 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_THREAD_MIRROR_H
#define IN_GSTHREAD_THREAD_MIRROR_H


/*
 *
 *
 *
 *
 *
 *
 */

#include <gsthread/gsthread_base.h>		// for EXPORT macros
#include <gsthread/thread_prim.h>		// for GsThreadRunnable
#include <gsthread/synch.h>				// for Mutex/Sema etc
#include <gsthread/thread_util.h>       // for GsThreadSafeCounter
#include <extension/GsRefCountedPtr.h>  // for GsRefCountedPtr<T>

CC_BEGIN_NAMESPACE( Gs )

// ----------------------------------------------------------------------------------------------------------------------------------------

/* Occasionally you do NOT want to pass ownership semantics to the job. In
** that case use a GsThreadRunnableMirror as your sacrifical wrapper, e.g.
** new GsSimpleThread( new GsThreadRunnableMirror( Runnable ) );
**
** See the file gsthread/mirror_example for correct use of this construct.
** It is deceptively tricky.
*/
class GsThreadRunnableMirror
    : public GsThreadRunnable
{
public:
    explicit GsThreadRunnableMirror( GsThreadRunnable* underlying ) : m_underlying( underlying ) { }
    // default destructor - does nothing, we don't own m_underlying
	// default copy/assignment operator

	void run() { m_underlying->run(); }

	void expert_startup(bool) throw() { m_underlying->expert_startup(false);}
	void expert_shutdown(bool) throw() { m_underlying->expert_shutdown(false);}

protected:
	GsThreadRunnable* m_underlying;
};

/* Occasionally you do NOT want to pass ownership semantics to the job. In
** that case use a GsThreadRunnableMirror as your sacrifical wrapper, e.g.
** new GsSimpleThread( new GsThreadRunnableMirror( Runnable ) );
**
** See the file gsthread/mirror_example for correct use of this construct.
** It is deceptively tricky.
*/
class GsThreadRunnableMirrorBarrier
    : public GsThreadRunnableMirror
{
public:
    explicit GsThreadRunnableMirrorBarrier( GsThreadRunnable* underlying, GsSemaphore& sem ) : GsThreadRunnableMirror( underlying ), m_sem( sem ) { }
    // default destructor - does nothing, we don't own m_underlying
	// default copy/assignment operator

	void expert_startup(bool) throw() { m_sem.wait(); GsThreadRunnableMirror::expert_startup(false); }

protected:
    GsSemaphore&      m_sem;
};

class GsThreadRefCountedRunnable
    : public GsThreadRunnable
{
public:
    explicit GsThreadRefCountedRunnable( GsThreadRunnable* underlying ) : m_underlying( underlying ) { }
    // default destructor - does nothing, we don't own m_underlying
	// default copy/assignment operator

	GsThreadRefCountedRunnable* clone() const { return new GsThreadRefCountedRunnable( *this );}

	void run() { m_underlying->run(); }

	void expert_startup(bool) throw() { m_underlying->expert_startup(false);}
	void expert_shutdown(bool) throw() { m_underlying->expert_shutdown(false);}

private:
	CC_NS(extension,GsRefCountedPtr)<GsThreadRunnable, GsThreadSafeCounter<int> > m_underlying;
};

CC_END_NAMESPACE

#endif
