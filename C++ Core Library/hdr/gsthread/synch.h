/****************************************************************
**
**	SYNCH.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/synch.h,v 1.21 2011/05/16 20:35:42 khodod Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_SYNCH_H
#define IN_GSTHREAD_SYNCH_H

#include <gsthread/gsthread_base.h> // for EXPORT macros
#include <extension/GsNoCopy.h>   // for GsNoCopy<T>

#ifndef WIN32

#include <time.h>      // for timespec
#include <pthread.h>   // for pthread_* routines
#include <semaphore.h> // for sem_* routines
#include <errno.h> // should be <cerrno> - for errno

#else

#include <windows.h>
#include <process.h>

#endif

CC_BEGIN_NAMESPACE( Gs )

// ----------------------------------------------------------------------------- GsMutex

// simple mutex wrapper class
class GsMutex
	: private CC_NS(extension,GsNoCopy)<GsMutex>
{
public:
	GsMutex()  throw();
	~GsMutex() throw();

	// no copy constructor due to GsNoCopy
	// no assignment operator due to GsNoCopy

private:
	void m_lock()    throw();
	void m_unlock()  throw();
	bool m_trylock() throw();

#ifndef WIN32
	pthread_mutex_t m_ptmutex;
#else
	HANDLE m_win32mutex;
#endif

	friend class GsMutexLock;
	friend class GsMutexHierLock;
	friend class GsConditionVariable;
};

// ------------------------------------------------------------------------------ GsSemaphore

// simple semaphore wrapper class
class GsSemaphore
	: private CC_NS(extension,GsNoCopy)<GsSemaphore>
{
public:
	explicit GsSemaphore(unsigned int value = 0) throw();
	~GsSemaphore()                               throw();

	// no copy constructor due to GsNoCopy
	// no assignment operator due to GsNoCopy

    void wait()     throw();
	bool try_wait() throw(); // returns true on success
	void post()     throw();
 
	int getvalue() throw();

#ifndef WIN32
private:
	sem_t m_sem;
#else
	// under WIN32, you may actually need to get to this 
	// eg. MsgWaitForMultipleObjects(),
    // so here...
	HANDLE m_win32sem;
#endif
};

// exception-safe mutex locking
class GsMutexLock
	: private CC_NS(extension,GsNoCopy)<GsMutexLock>
{
public:
	explicit GsMutexLock(GsMutex& mut_) throw() : m_mutex(mut_) { m_mutex.m_lock();}
	~GsMutexLock()                      throw()                 { m_mutex.m_unlock();}

	// no copy constructor due to GsNoCopy
	// no assignment operator due to GsNoCopy

private:
	GsMutex& m_mutex;
};

class GsMutexHierLock
	: private CC_NS(extension,GsNoCopy)<GsMutexHierLock>
{
public:
	explicit GsMutexHierLock(GsMutex& mut_) throw() : m_mutex(mut_), m_have_lock(false) {}
	~GsMutexHierLock() throw() { if( m_have_lock ) m_mutex.m_unlock();}

	void lock()           throw() { m_mutex.m_lock(); m_have_lock = true; }
	void unlock()         throw() { m_mutex.m_unlock(); m_have_lock = false; }
	bool try_lock()       throw() { return m_mutex.m_trylock(); }
	operator bool() const throw() { return m_have_lock != false; }

	class ReLock;
	friend class ReLock;

private:
	GsMutex& m_mutex;
	bool m_have_lock;

public:
	class ReLock
	{
	public:
		explicit ReLock( GsMutexHierLock& lock_) throw() : m_lock(lock_) { m_lock.unlock();}
		~ReLock() throw() { m_lock.lock();}
	private:
		GsMutexHierLock& m_lock;
	};
};

// exception-safe semaphores
class GsSemaWait
    : private CC_NS(extension,GsNoCopy)<GsSemaWait>
{
public:
    explicit GsSemaWait( GsSemaphore& sem_, bool no_wait = false ) throw()
	    : m_sem(sem_),
		  m_unlock( no_wait ? m_sem.try_wait() : ( m_sem.wait(), true ) )
    {
	} // this can block for a long time

    ~GsSemaWait() throw()
    {
	    if( m_unlock )
		    m_sem.post();
	}

	// no copy constructor due to GsNoCopy
	// no assignment operator due to GsNoCopy

    void release() throw() { m_unlock = false;} // we don't want the semaphore posted when we are done.
    operator bool() throw() { return m_unlock == true; }

private:
    GsSemaphore& m_sem;
    bool m_unlock;
};

// condition variables
class GsConditionVariableRep;

class EXPORT_CLASS_GSTHREAD GsConditionVariable
	: private CC_NS(extension,GsNoCopy)<GsConditionVariable>
{
public:
	explicit GsConditionVariable( GsMutex& ) throw();
	~GsConditionVariable() throw();

	class TimeOut {};

	// these functions return no errors

	// note that you should own the mutex before calling these in order to avoid race conditions
    void signal    () throw();
    void broadcast () throw();

    void wait      () throw();

	// timed wait
	// usage:
	// {
    //     try
	//     {
    //         {
    //             GsMutexLock lock( cond.mutex() );
	//             GsConditionVariable::CondTimeout wait_time = cond.relative_wait( 3 ); // 3 seconds
	//             while( ! pred )
	//                 cond.timedwait( wait_time );
    //
    //             ... pred = false ...
    //         }
    //         ... use resource ...
    //     }
    //     catch( GsConditionVariable::TimeOut& )
    //     {
    //         ... we timed out, maybe do nothing
    //     }
    // }
#ifdef WIN32
	typedef double   CondTimeout;
#else
	typedef timespec CondTimeout;
#endif

	static CondTimeout relative_wait( double ) throw();
	void timedwait(CondTimeout const&) throw(TimeOut);

	// acquire a lock on the cond var mutex.
	// usage idiom:
	// {
	//     GsMutexLock lock( cond.mutex() );
	//     ...
	//     cond.broadcast();
	//     ...
	// }
	GsMutex& mutex() throw() { return m_gs_mutex; }
	
private:
	GsMutex&			    m_gs_mutex;
	GsConditionVariableRep* m_rep;
};

class EXPORT_CLASS_GSTHREAD GsThreadBarrier
{
public:
	explicit GsThreadBarrier( int count = 0 ) throw() : m_curr(0), m_value(count), m_count(count), m_mutex(), m_cond(m_mutex) {}
	~GsThreadBarrier() throw() {}

	void reset( int num, int value ) throw()
	{
		GsMutexLock l1( m_mutex );
		m_count = num;
		m_value = value;
		if( !m_count )
		{
			++m_curr;
			m_count = m_value;
			m_cond.broadcast();
		}
	}

	void post() throw()
	{
		GsMutexLock ll( m_mutex );
		this->m_post();
	}

	void wait() throw()
	{
		GsMutexLock l1( m_mutex );
		if( !this->m_post())
		{
			int curr = m_curr;
			while( m_curr == curr )
				m_cond.wait();
		}
	}

private:
	bool m_post() throw() // must be called with m_mutex locked
	{
		--m_count;
		if( !m_count )
		{
			++m_curr;
			m_count = m_value;
			m_cond.broadcast();
			return true;
		}
		return false;
	}

	int m_curr;
	int m_value;
	int m_count;
	GsMutex m_mutex;
	GsConditionVariable m_cond;
};

class EXPORT_CLASS_GSTHREAD GsReadersWriter
	: CC_NS(extension,GsNoCopy)<GsReadersWriter>
{
public:
	virtual ~GsReadersWriter() throw() {}

	class Read_Lock;
	class Write_Lock;
	friend class Read_Lock;
	friend class Write_Lock;

private:
	virtual void m_rdlock() throw() = 0;
	virtual void m_rdunlock() throw() = 0;
	virtual bool m_rdtrylock() throw() = 0;

	virtual void m_wrlock() throw() = 0;
	virtual void m_wrunlock() throw() = 0;
	virtual bool m_wrtrylock() throw() = 0;
};

typedef GsReadersWriter::Read_Lock GsReadLock;
typedef GsReadersWriter::Write_Lock GsWriteLock;

class GsReadersWriter::Read_Lock
	: CC_NS(extension,GsNoCopy)<GsReadersWriter::Read_Lock>
{
public:
	explicit Read_Lock( GsReadersWriter& lock_ ) throw() : m_lock(lock_) { m_lock.m_rdlock();}
	~Read_Lock()                                 throw()                 { m_lock.m_rdunlock();}

	// no copy constructor due to GsNoCopy
	// no assignment operator due to GsNoCopy

private:
	GsReadersWriter& m_lock;
};

class GsReadersWriter::Write_Lock
	: CC_NS(extension,GsNoCopy)<GsReadersWriter::Write_Lock>
{
public:
	explicit Write_Lock( GsReadersWriter& lock_ ) throw() : m_lock(lock_) { m_lock.m_wrlock();}
	~Write_Lock()                                 throw()                 { m_lock.m_wrunlock();}

	// no copy constructor due to GsNoCopy
	// no assignment operator due to GsNoCopy

private:
	GsReadersWriter& m_lock;
};


class GsWriterPreferenceReadersWriterImp;
class EXPORT_CLASS_GSTHREAD GsWriterPreferenceReadersWriter
	:  public GsReadersWriter
{
public:
	GsWriterPreferenceReadersWriter();
	virtual ~GsWriterPreferenceReadersWriter() throw();

private:
	virtual void m_rdlock() throw();
	virtual void m_rdunlock() throw();
	virtual bool m_rdtrylock() throw();

	virtual void m_wrlock() throw();
	virtual void m_wrunlock() throw();
	virtual bool m_wrtrylock() throw();

	GsWriterPreferenceReadersWriterImp *m_imp;
};




CC_END_NAMESPACE

// ------------------------------------- implementation ---------------------------------------------
// These functions are inline because they tend to be used in the most performance-critical sections of code
//

#ifdef GSTHREAD_DEBUG
#include <gsthread/thread_debug.h>
#define MUTEX_INIT GsMutexDebug::new_mutex( this )
#define MUTEX_DEAD GsMutexDebug::remove_mutex( this )
#define MUTEX_LOCKABLE(v) GsMutexDebug::lockable_mutex( this, v )
#define MUTEX_LOCKED(v) if( v ) GsMutexDebug::locked_mutex( this )
#define MUTEX_UNLOCK GsMutexDebug::unlock_mutex( this )

#define SEM_INIT GsSemaphoreDebug::new_semaphore( this )
#define SEM_DEAD GsSemaphoreDebug::remove_semaphore( this )
#define SEM_POST GsSemaphoreDebug::post_semaphore( this )
#define SEM_WAITABLE(v) GsSemaphoreDebug::waitable_semaphore( this, v )
#define SEM_WAITED(v) if( v ) GsSemaphoreDebug::waited_semaphore( this )

#else

#define MUTEX_INIT
#define MUTEX_DEAD
#define MUTEX_LOCKABLE(v)
#define MUTEX_LOCKED(v)
#define MUTEX_UNLOCK

#define SEM_INIT
#define SEM_DEAD
#define SEM_POST
#define SEM_WAITABLE(v)
#define SEM_WAITED(v)
#endif

CC_BEGIN_NAMESPACE( Gs )

#ifndef WIN32
// POSIX mutexes
inline GsMutex::GsMutex()  throw() { MUTEX_INIT; pthread_mutex_init(&m_ptmutex, 0);}
inline GsMutex::~GsMutex() throw() { MUTEX_DEAD; pthread_mutex_destroy(&m_ptmutex);}
inline void GsMutex::m_lock()    throw() { MUTEX_LOCKABLE(true); pthread_mutex_lock(&m_ptmutex); MUTEX_LOCKED(true); }
inline void GsMutex::m_unlock()  throw() { MUTEX_UNLOCK; pthread_mutex_unlock(&m_ptmutex);}
inline bool GsMutex::m_trylock() throw() { MUTEX_LOCKABLE(false); bool ret = (pthread_mutex_trylock(&m_ptmutex) == 0); MUTEX_LOCKED(ret); return ret;}

// POSIX semaphores
inline GsSemaphore::GsSemaphore(unsigned int value) throw() { SEM_INIT; sem_init(&m_sem, 0, value);}
inline GsSemaphore::~GsSemaphore()                  throw() { SEM_DEAD; sem_destroy(&m_sem);}
inline void GsSemaphore::wait()     throw() { SEM_WAITABLE(true); while( sem_wait(&m_sem) == -1 && errno == EINTR ) /* retry if we get interrupted by a signal */; SEM_WAITED(true);}
inline bool GsSemaphore::try_wait() throw() { SEM_WAITABLE(false); bool ret = (sem_trywait(&m_sem) == 0); SEM_WAITED(ret); return ret;}
inline void GsSemaphore::post()     throw() { SEM_POST; sem_post(&m_sem);}
inline int GsSemaphore::getvalue() throw() { int tmp; sem_getvalue(&m_sem, &tmp); return tmp;}

#else
// WIN32 mutexes
inline GsMutex::GsMutex()  throw() : m_win32mutex( CreateMutex( 0, false, 0 ) ) { MUTEX_INIT; }
inline GsMutex::~GsMutex() throw() { MUTEX_DEAD; if( m_win32mutex ) CloseHandle( m_win32mutex ); }
inline void GsMutex::m_lock()    throw() { MUTEX_LOCKABLE(true); WaitForSingleObject( m_win32mutex, INFINITE ); MUTEX_LOCKED(true); }
inline void GsMutex::m_unlock()  throw() { MUTEX_UNLOCK; ReleaseMutex( m_win32mutex );}
inline bool GsMutex::m_trylock() throw() { MUTEX_LOCKABLE(false); bool ret = (WaitForSingleObject( m_win32mutex, 0 ) == WAIT_OBJECT_0); MUTEX_LOCKED(ret); return ret;}

// WIN32 semaphores
inline GsSemaphore::GsSemaphore(unsigned int value) throw() : m_win32sem( CreateSemaphore( 0, value, 0x7fffffff, 0 )) { SEM_INIT; }
inline GsSemaphore::~GsSemaphore()                  throw() { SEM_DEAD; CloseHandle( m_win32sem );}
inline void GsSemaphore::wait()     throw() { SEM_WAITABLE(true); WaitForSingleObject( m_win32sem, INFINITE ); SEM_WAITED(true); }
inline bool GsSemaphore::try_wait() throw() { SEM_WAITABLE(false); bool ret = (WaitForSingleObject( m_win32sem, 0 ) == WAIT_OBJECT_0); SEM_WAITABLE(ret); return ret;}
inline void GsSemaphore::post()     throw() { SEM_POST; ReleaseSemaphore( m_win32sem, 1, 0 );}
//int getvalue() throw(); // ???

#endif

CC_END_NAMESPACE

#undef MUTEX_INIT
#undef MUTEX_DEAD
#undef MUTEX_LOCKABLE
#undef MUTEX_LOCKED
#undef MUTEX_UNLOCK

#undef SEM_INIT
#undef SEM_DEAD
#undef SEM_POST
#undef SEM_WAITABLE
#undef SEM_WAITED

#endif // ifndef IN_GSTHREAD_SYNCH_H
