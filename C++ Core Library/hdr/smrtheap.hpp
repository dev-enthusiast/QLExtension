// smrtheap.hpp -- SmartHeap public C++ header file
// Professional Memory Management Library
//
// Copyright (C) 1997-2005 Compuware Corporation.
// All Rights Reserved.
//
// No part of this source code may be copied, modified or reproduced
// in any form without retaining the above copyright notice.
// This source code, or source code derived from it, may not be redistributed
// without express written permission of the copyright owner.
//
// COMMENTS:
// - Include this header file to call the SmartHeap-specific versions of
//   operators new (i.e. with placement syntax), to:
//     o allocate from a specific memory pool;
//     o specify allocation flags, such as zero-initialization;
//     o resize an allocation.
//
// - If you include this header file, you must compile and link shnew.cpp, or
//   link with one of the SmartHeap static operator new libraries:
//   sh[l|d]XXXX.lib
//
// - Can be used in both EXEs and DLLs.
//
// - For 16-bit x86 platforms, use only in large or compact memory model.
//
// - If you do not want to use SmartHeap's global operator new but you do
//   want to use SmartHeap's other facilities in a C++ application, then 
//   include the smrtheap.h header file but do not include this header file,
//   and do not link with shnew.cpp.  The two ".Xpp" files are present
//   ONLY for the purpose of defining operator new and operator delete.
//
// - Use the MemDefaultPool global variable to refer to a memory pool to pass
//   to SmartHeap functions that accept a pool as a parameter,
//   e.g. MemPoolCount, MemPoolSize, MemPoolWalk, etc.
//

#if !defined(_SMARTHEAP_HPP)
#define _SMARTHEAP_HPP

#if defined(_MSC_VER) \
   && (defined(M_I86LM) || defined(M_I86CM) || defined(M_I86HM)) \
   && !defined(MEM_HUGE)
#define MEM_HUGE 0x8000u
#endif

#ifndef __BORLANDC__
/* Borland C++ does not treat extern "C++" correctly */
extern "C++"
{
#endif /* __BORLANDC__ */

#if defined(_MSC_VER) && _MSC_VER >= 900
#pragma warning(disable : 4507)
#endif

#if defined(MEM_DEBUG)
#if defined(new)
#undef new
#endif
#if defined(delete)
#undef delete
#endif
#endif /* MEM_DEBUG */

/* #undef new/delete before including new.h in case user previously
 * included heapagnt.h, which defines these macros when MEM_DEBUG is on
 */
// ANSI standard prototype for set_new_handler
#if (defined(__MWERKS__) \
	|| defined(__hpux) \
	|| (defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x500 && __SUNPRO_CC_COMPAT >= 5)\
	|| (defined(_MSC_VER) && _MSC_VER >= 1300) /* VC 7.0 */ \
	|| defined(_AIX43) \
	|| defined(__GNUC__) && __GNUC__ >= 3 \
	|| defined(__BORLANDC__) && __BORLANDC__ >= 0x560)

#define MEM_CPP_EXC 1

#if !defined(__hpux) || defined(_NAMESPACE_STD) || defined(_HP_NAMESPACE_STD) \
	|| defined(__ia64)
#define MEM_CPP_ANSI 1
#endif // hp

#endif // compiler-specific

#ifdef MEM_CPP_ANSI

#include <new>
#include "smrtheap.h"

#define MEM_STD_NAMESPACE_BEGIN namespace std {
#define MEM_STD_NAMESPACE_END }
#define MEM_STD_USING(x) using std::x
#define MEM_STD_(x) std::x

#ifdef __SUNPRO_CC
typedef void (MEM_ENTRY_ANSI * new_handler)();

#elif defined(_MSC_VER) && _MSC_VER == 1310 && defined(_M_IA64)
/* MS SDK (VC 7.1) Itanium has an old VC6 new header file
 * with no std::new_handler
 */
MEM_STD_NAMESPACE_BEGIN
typedef void (__cdecl *new_handler)();	// handler for operator new failures
MEM_STD_NAMESPACE_END
#endif


#else // MEM_CPP_ANSI

#include <new.h>
#include "smrtheap.h"

#define MEM_STD_NAMESPACE_BEGIN
#define MEM_STD_NAMESPACE_END
#define MEM_STD_USING(x)
#define MEM_STD_(x) x

#endif // MEM_CPP_ANSI


#ifdef MEM_CPP_EXC

#define MEM_CPP_THROW throw()
#ifdef __BORLANDC__
#define MEM_CPP_THROW2
#else
#define MEM_CPP_THROW2 MEM_CPP_THROW
#endif
#define MEM_CPP_THROW1(x) throw(MEM_STD_(x))
//#elif defined(_MSC_VER) && _MSC_VER >= 1100 && defined(_CPPUNWIND)
//#define MEM_CPP_THROW throw()

#else // MEM_CPP_EXC

#define MEM_CPP_THROW
#define MEM_CPP_THROW2
#define MEM_CPP_THROW1(x)
typedef void (MEM_ENTRY_ANSI * new_handler)();

#endif // MEM_CPP_EXC



#ifndef _CRTIMP
#define _CRTIMP
#endif // _CRTIMP
#ifndef _CRTIMP2
#define _CRTIMP2 _CRTIMP
#endif



#ifdef _MSC_VER_xxx
_CRTIMP _PNH MEM_ENTRY_ANSI _set_new_handler(_PNH);
#if UINT_MAX == 0xFFFFu
_PNH MEM_ENTRY_ANSI _set_fnew_handler(_PNH);
_PNHH MEM_ENTRY_ANSI _set_hnew_handler(_PNHH);
#endif // UINT_MAX
#endif // _MSC_VER

MEM_STD_NAMESPACE_BEGIN
_CRTIMP2 new_handler MEM_ENTRY_ANSI set_new_handler(new_handler) MEM_CPP_THROW2;
MEM_STD_NAMESPACE_END

#if !(defined(__SUNPRO_CC) && __SUNPRO_CC >= 0x500)
#define SHI_NEWDEFARGS 1
#endif

/* also #undef new/delete in case compiler has defined these in new.h */
#if defined(MEM_DEBUG)
#if defined(new)
#undef new
#endif
#if defined(delete)
#undef delete
#endif
#endif /* MEM_DEBUG */

void MEM_FAR * MEM_ENTRY_ANSI shi_New(MEM_SIZET sz, unsigned flags=0, MEM_POOL pool=0);

// operator new variants:


// version of new that passes memory allocation flags
// (e.g. MEM_ZEROINIT to zero-initialize memory)
// call with syntax 'ptr = new (flags) <type>'
inline void MEM_FAR *operator new(size_t sz, unsigned flags)
   { return shi_New(sz, flags); }
#if defined(_MSC_VER) && _MSC_VER >= 1200
inline void MEM_FAR operator delete(void *p, unsigned)
   { ::operator delete(p); }
#endif // _MSC_VER

// version of new that allocates from a specified memory pool with alloc flags
// call with the syntax 'ptr = new (pool, [flags=0]) <type>'
inline void MEM_FAR *operator new(size_t sz, MEM_POOL pool, unsigned flags
#ifdef SHI_NEWDEFARGS
											 =0
#endif
											  )
   { return shi_New(sz, flags, pool); }
#if defined(_MSC_VER) && _MSC_VER >= 1200
inline void MEM_FAR operator delete(void *p, MEM_POOL, unsigned)
   { ::operator delete(p); }
#endif // _MSC_VER

#ifdef SHI_ARRAY_NEW
inline void MEM_FAR *operator new[](size_t sz, MEM_POOL pool, unsigned flags
#ifdef SHI_NEWDEFARGS
												=0
#endif
												 )
   { return shi_New(sz, flags, pool); }
#endif

// version of new that changes the size of a memory block previously allocated
// from an SmartHeap memory pool
// call with the syntax 'ptr = new (ptr, flags) <type>'
#if !defined(__BORLANDC__) && !defined(__HIGHC__)
/* bug in BC++, MetaWare High C++ parsers confuse this with new(file,line) */
inline void MEM_FAR *operator new(size_t new_sz, void MEM_FAR *lpMem,
                                  unsigned flags)
   { return MemReAllocPtr(lpMem, new_sz, flags); }
#if defined(_MSC_VER) && _MSC_VER >= 1200
inline void MEM_FAR operator delete(void *p, void MEM_FAR *, unsigned)
   { ::operator delete(p); }
#endif // _MSC_VER

#ifdef SHI_ARRAY_NEW
inline void MEM_FAR *operator new[](size_t new_sz, void MEM_FAR *lpMem,
                                  unsigned flags)
   { return MemReAllocPtr(lpMem, new_sz, flags); }
#endif // SHI_ARRAY_NEW
#endif


#ifndef DBG_FORMAL
#define DBG_FORMAL
#define DBG_ACTUAL
#ifndef DEBUG_NEW
#define DEBUG_NEW new
#endif // DEBUG_NEW
#define DEBUG_NEW1(x_) new(x_)
#define DEBUG_NEW2(x_, y_) new(x_, y_)
#define DEBUG_NEW3(x_, y_, z_) new(x_, y_, z_)
#ifndef DEBUG_DELETE
#define DEBUG_DELETE delete
#endif
#endif

#ifdef DEFINE_NEW_MACRO
#define new DEBUG_NEW
#define delete DEBUG_DELETE
#endif

#if defined(_MSC_VER) && _MSC_VER >= 900
#pragma warning(default : 4507)
#endif

#ifndef __BORLANDC__
}
#endif /* __BORLANDC__ */

#endif /* !defined(_SMARTHEAP_HPP) */
