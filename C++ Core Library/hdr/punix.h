/****************************************************************
**
**	PUNIX.H	- Machine/Compiler Portable definitions - UNIX
**
**	$Header: /home/cvs/src/portable/src/punix.h,v 1.48 2012/07/03 18:01:08 e19351 Exp $
**
****************************************************************/

#ifndef _PUNIX_H
#define _PUNIX_H

#include <stddef.h>

extern "C" int stricmp( const char *str1, const char *str2 );
extern "C" int strnicmp( const char *str1, const char *str2, size_t maxlen );
extern "C" char *strupr( char *s );
extern "C" char *strlwr( char *s );

#ifndef _REENTRANT
#define _REENTRANT
#endif
#ifndef NDEBUG
#define NDEBUG
#endif

#if defined( linux )
#	include <math.h>
#	include <limits.h> // for PATH_MAX
#	define isnand(x)	isnan(x)
#	define isnanf(x)	isnan(x)
#else
#	include <sys/param.h>
#	include <ieeefp.h>
#	if defined(CC_DEFINE_EXCEPTION_IN_MATH_H)
#		define exception __math_exception
#	endif
#	include <math.h>
#	if defined(CC_DEFINE_EXCEPTION_IN_MATH_H)
#		undef exception
#	endif
#endif

// unix HUGE is MAXFLOAT, we need it to be HUGE_VAL
#undef HUGE
#define HUGE HUGE_VAL

// If tolower is not a macro, but _tolower is, then change the TOLOWER
// macro to use _tolower.
#if defined( CC_HAVE_STD_CCTYPE )
#	include	<bits/std_cctype.h>
#else
#	include	<ctype.h>
#endif

#if !defined( tolower ) && defined( _tolower )
#	undef	TOLOWER
#	undef	TOUPPER
#	define TOLOWER( c )  (ISUPPER(c) ? _tolower( (unsigned char) (c) ) : (c))
#	define TOUPPER( c )  (ISLOWER(c) ? _toupper( (unsigned char) (c) ) : (c))
#endif

#if defined( linux )
// This is the Posix way of getting the maximum file name path.
#	define _MAX_PATH	PATH_MAX
#	define MAXPATHLEN	PATH_MAX		// remove this soon!
#else
#	define _MAX_PATH	MAXPATHLEN
#endif

#define _cdecl
#define	pascal 				
#define	PASCAL				
#define	huge
#define _huge
#define	far
#define _far
#define	FAR
#define _loadds
#define	halloc				calloc
#define  hfree(ptr)			free(ptr)
#define	SIGBREAK			SIGHUP
#define	SH_DENYRW			0x10
#define	SH_DENYWR			0x20
#define	SH_DENYRD			0x30
#define	SH_DENYNO			0x40
#define	O_BINARY			0

#define IS_NAN_DOUBLE(X)	isnand(X)
#define IS_NAN_FLOAT(X)		isnanf(X)

#if !defined( BYTE_ORDER )
#	if defined( __sparc ) || defined( __sparcv9 )
#		define	BYTE_ORDER BIG_ENDIAN
#	else
#		define  BYTE_ORDER LITTLE_ENDIAN
#	endif
#endif

#define	DOUBLE_ALIGNMENT		8
#define	ALIGNMENT			4
#define	MKDIR(dirname)		mkdir((dirname), 0777)
#define	SHUSH_UNREF_WARNING(varlst)

#define	DEBUGBREAK		

#if defined( INCL_FILEIO ) || defined( INCL_DIRECTORY )
	extern "C" int sopen( const char *filename, int oflag, int shflag, int pmode );
#	include	<unistd.h>
#	include <dirent.h>
#	include <sys/stat.h>
#	include <errno.h>
#	define FILENAME_PATH_SEPARATOR	':'
#	define FILENAME_SEPARATORS		"/"
#endif

#if defined( INCL_THREADS ) 
#	if defined( sun )
#		define HAVE_SOLARIS_THREADS
#		define HAVE_SOLARIS_SEMAPHORES
#		include <thread.h>
#	elif defined( linux ) || defined( __i386 )
#		define HAVE_POSIX_THREADS
#		define HAVE_POSIX_SEMAPHORES
#		include <pthread.h>
#	endif

#	define SEM_IMMEDIATE_RETURN		(0)
#	define SEM_INDEFINITE_WAIT		(-1)
#	define ThreadBegin( Func, StackSize, ArgList ) JaThreadBegin(Func,StackSize,ArgList)
#	define ThreadEnd() JaThreadEnd()
#endif

#if defined( INCL_ALLOCA )
#	include <alloca.h>
#endif

#endif /* _PUNIX_H */
