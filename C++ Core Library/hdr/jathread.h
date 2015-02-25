/* $Header: /home/cvs/src/kool_ade/src/jathread.h,v 1.20 2012/05/31 17:48:27 e19351 Exp $ */
/****************************************************************
**
**	JATHREAD.H	- Portable interface to OS thread APIs
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.20 $
**
**	Brian A. Weston
**
**	This API is essentially Solaris's thread API.
**
****************************************************************/

#if !defined( IN_JATHREAD_H )
#define IN_JATHREAD_H

#if !defined( INCL_THREADS )
#error INCL_THREADS needs to be defined prior to include portable.h
#endif

#if !defined( aix )

/*
** Types
*/

#if defined( WIN32 )
typedef long 			JATHREAD_HANDLE;
typedef DWORD   		JATHREAD_ID;
#endif

#ifdef HAVE_SUNOS_THREADS
typedef long			JATHREAD_HANDLE;
typedef long			JATHREAD_ID;
#endif

#ifdef HAVE_SOLARIS_THREADS
typedef thread_key_t 	JATHREAD_HANDLE;
typedef thread_t		JATHREAD_ID;
#endif

#ifdef HAVE_POSIX_THREADS
typedef pthread_key_t 	JATHREAD_HANDLE;
typedef pthread_t		JATHREAD_ID;
#endif


typedef void			(* JATHREAD_BEGIN_FUNC)( void * Ptr );
typedef void			(* JATHREAD_EXIT_FUNC)(  void * Ptr );

typedef enum JaThreadPriorityEnum
{
	JATHREAD_IDLE,
	JATHREAD_LOW,
	JATHREAD_NORMAL,
	JATHREAD_HIGH

} JATHREAD_PRIORITY;


/*
** Functions
*/

DLLEXPORT JATHREAD_ID
		JaThreadBegin( 			JATHREAD_BEGIN_FUNC Func, unsigned int StackSize, void *ArgList );

DLLEXPORT int
		JaThreadPrioritySet(	JATHREAD_PRIORITY );

DLLEXPORT int
		JaThreadHandleCreate(	JATHREAD_HANDLE *Handle, JATHREAD_EXIT_FUNC Func ),
		JaThreadPointerGet( 	JATHREAD_HANDLE Handle, void **Ptr ),
		JaThreadPointerSet( 	JATHREAD_HANDLE Handle, void *Ptr );

DLLEXPORT void
		JaThreadEnd( 			void );


/*
** Macros
*/

#if defined( WIN32 )
#define	JaThreadId	 		(GetCurrentThreadId())
#endif

#ifdef HAVE_SUNOS_THREADS
#define JaThreadId			0
#endif

#ifdef HAVE_SOLARIS_THREADS
#define	JaThreadId	 		(thr_self())
#endif

#ifdef HAVE_POSIX_THREADS
#define JaThreadId	 		(pthread_self())
#endif

#endif

#endif
