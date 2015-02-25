/**************************************************************** -*-c++-*-
**
**	THREAD_UTIL.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/thread_util.h,v 1.9 2011/05/26 03:03:50 khodod Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_THREAD_UTIL_H
#define IN_GSTHREAD_THREAD_UTIL_H

#include <gsthread/synch.h> // for GsMutex/GsMutexLock

CC_BEGIN_NAMESPACE( Gs )

// T_ is expected to be a type that acts like a simple counter
// T_ must be destructable
// T_ must support pre increment/decrement
// T_ must be copy constructable

template <class T_>
class GsThreadSafeCounter // not copyable because GsMutex's aren't copyable
{
public:
    GsThreadSafeCounter() : m_mut(), m_obj() {}
    explicit GsThreadSafeCounter(T_ const& arg) : m_mut(), m_obj(arg) {}
    // use default destructor

    // predecrement/preincrement
    T_ operator--() { GsMutexLock L1(m_mut); --m_obj; return m_obj;}
    T_ operator++() { GsMutexLock L1(m_mut); ++m_obj; return m_obj;}

    // postdecrement/postincrement
    T_ operator--(int) { GsMutexLock L1(m_mut); T_ ret = m_obj; --m_obj; return ret; }
    T_ operator++(int) { GsMutexLock L1(m_mut); T_ ret = m_obj; ++m_obj; return ret; }

    // accessors
    void reset(T_ const& val) { GsMutexLock L1(m_mut); m_obj = val; }
    T_ val() const { GsMutexLock L1(m_mut); T_ ret = m_obj; return ret; }

private:
    GsMutex m_mut;
    T_ m_obj;
};

#ifdef __linux 
#ifdef USE_ASSEMBLER_VERSION

template<>
class GsThreadSafeCounter<int>
{
public:
    GsThreadSafeCounter() : m_count(0) { }
    explicit GsThreadSafeCounter(int const& arg) : m_count(arg) { }
    // use default destructor

	// NOTE
	//   the values returned by these operators do not have much meaning
	//   because by the time you use them, another thread could have changed
	//   them.
	// The intended use is for a refcounted pointer where you only ever
	// test for 0 which is always safe because once you get to 0 you know
	// no other thread can change it

    // predecrement/preincrement
	
    int operator--()
	{
		int old_val = -1;
		asm( "lock; xaddl %0, %1"
			 :"=r" (old_val), "=m" (m_count) /* outputs */
			 :"0" (old_val), "1" (m_count), "m" (&m_count)); /* inputs */
        return old_val != 1; // if the old value was 1, then the new value must be 0
	}

    int operator++()
	{
		asm( "lock; incl %0" :"=m" (m_count) :"0" (m_count));
		return 1; // this return value should really never be used, so no need to take the penalty of doing a read
	}

    // postdecrement/postincrement
    int operator--(int) { asm( "lock; decl %0" :"=m" (m_count) :"0" (m_count)); return m_count; }
    int operator++(int) { asm( "lock; incl %0" :"=m" (m_count) :"0" (m_count)); return m_count; }

    // accessors
    void reset(int const& val) { m_count = val; }
    int val() const { return m_count; }

private:
	volatile int m_count;
};
#endif
#endif

#ifdef WIN32
#include <winbase.h>

template<>
class GsThreadSafeCounter<int>
{
public:
    GsThreadSafeCounter() : m_count(0) {}
    explicit GsThreadSafeCounter(int const& arg) : m_count( (LONG) arg ) {}
    // use default destructor

	// NOTE
	//   the values returned by these operators do not have much meaning
	//   because by the time you use them, another thread could have changed
	//   them.
	// The intended use is for a refcounted pointer where you only ever
	// test for 0 which is always safe because once you get to 0 you know
	// no other thread can change it

    // predecrement/preincrement
    int operator--() { return InterlockedDecrement( &m_count ); }
    int operator++() { return InterlockedIncrement( &m_count ); }

    // postdecrement/postincrement
    int operator--(int) { int ret = (int) m_count; InterlockedDecrement( &m_count ); return ret; }
    int operator++(int) { int ret = (int) m_count; InterlockedIncrement( &m_count ); return ret; }

    // accessors
    void reset(int const& val) { m_count = (LONG) val; }
    int val() const { return (int) m_count; }

private:
	LONG m_count;
};
#endif

CC_END_NAMESPACE

#endif // IN_GSTHREAD_THREAD_UTIL_H
