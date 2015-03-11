/****************************************************************
**
**	PWIN32.H	- Machine/Compiler Portable definitions - Win NT
**
**	$Header: /home/cvs/src/portable/src/pwin32.h,v 1.43 2015/03/03 13:38:48 e19351 Exp $
**
****************************************************************/

#ifndef _PWIN32_H
#define _PWIN32_H

// turn off annoying warnings
// 4121: alignment of a member was sensitive to packing
// 4127: conditional expression is constant
// 4201: nonstandard extension used : nameless struct/union
// 4231: nonstandard extension used : 'extern' before template explicit instantiation
//		This is necessary to use the EXPORT_TEMPLATE_XXX macros without
//		generating bogus warnings
// 4251: 'm_x' : class 'X' needs to have dll-interface to be used by clients of class 'Y'
// 4273: 'SockStrError': inconsistent dll linkage.  dllexport assumed.
//		We can reenable this warning after everything is converted to use
//		the EXPORT_xxx_libname approach.
// 4275: non dll-interface class 'X' used as base for dll-interface class 'Y'
// 4310: cast truncates constant value
// 4514: unreferenced inline function removed
// 4706: assignment within conditional expression
// 4710: function not expanded
// 4711: function selected for inline expansion
// 4800: forcing bool variable to one or zero, possible performance loss
// Promote warnings to errors
// 4002: too many actual parameters for macro
// 4003: not enough actual parameters for macro
// 4700: local variable 'name' used without having been initialized
//			This is almost always a real error, not a warning.

#pragma warning( disable : 4121 4127 4201 4231 4251 4273 4275 4310 4514 4706 4710 4711 4800; error : 4002 4003 4700 )

#define WIN32_LEAN_AND_MEAN

#define NOMINMAX
// Get rid of windows macro versions, in case some file included windows.h first.
// Looking at you, gsutil\gsxml.cpp!
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#ifdef SECDB_ALT_ALLOCATORS

// Alternative memory allocator hooks.
DLLEXPORT void *HappyMalloc(unsigned int size);
DLLEXPORT void *HappyCalloc(unsigned int n, unsigned int size);
DLLEXPORT void *HappyRealloc(void *p, unsigned int size);
DLLEXPORT void HappyFree(void *p);
DLLEXPORT char *HappyStrdup(const char *);
DLLEXPORT int HappyAllocCount;
DLLEXPORT int HappyAllocSize;

#ifndef NO_HAPPY_HEAP
// MSVC's STL (rightfully) complains when free/malloc are redefined. This
// header wraps those functions for other STL headers, so include it first.
#	if defined( _MSC_VER ) && _MSC_VER >= 1600
#		include <xdebug>
#	endif

#	define calloc		HappyCalloc
#	define malloc		HappyMalloc
#	define free		HappyFree
#	define realloc		HappyRealloc
#	define strdup		HappyStrdup
#endif

#endif

// These should go away - haven't been relevant since DOS.
#define	halloc				calloc
#define hfree(ptr)			free(ptr)

#define  HUGE				HUGE_VAL
#define _huge
#define IS_NAN_DOUBLE(X)	((((short *) &X )[ 3 ] & 0x7ff0 ) == 0x7ff0 )
#define IS_NAN_FLOAT(X)	((((short *) &X )[ 1 ] & 0x7f80 ) == 0x7f80 )

#define	BYTE_ORDER		LITTLE_ENDIAN

#define	DOUBLE_ALIGNMENT		1
#define	ALIGNMENT			4
#define	MKDIR(dirname)		mkdir((dirname))
#define	SHUSH_UNREF_WARNING(varlst)	varlst;

#if defined( _DEBUG )
#	define	DEBUGBREAK		__asm { int 3 }
#else
#	define	DEBUGBREAK		
#endif

#if defined( INCL_FILEIO )
#	include	<io.h>
#	include <sys/stat.h> 
	// Access codes
#	if !defined( F_OK )
#		define F_OK	0x00
#		define X_OK	0x01
#		define W_OK	0x02
#		define R_OK	0x04
#	endif 
#	define FILENAME_PATH_SEPARATOR	';'
#	define FILENAME_SEPARATORS		"/\\:"
#endif

#if defined( INCL_DIRECTORY )
#	include <dos.h>
#	include <dirent.h>
#endif

#if defined( INCL_THREADS )
#	include	<process.h>
#	include	<windows.h>
#	define 	SEM_INDEFINITE_WAIT INFINITE
#	define	ThreadBegin(Func,StackSize,ArgList)	JaThreadBegin(Func,StackSize,ArgList)
#	define	ThreadEnd()						   	JaThreadEnd()
#endif

/*********************************************************************
** Compensate for the lack of a C99-compliant snprintf implementation 
** in the Visual Studio 2010 compiler. This solution is taken from the
** following post courtesy of Valentin Milea:
** http://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
** This version will NULL-terminate the string in case of overflow,
** and the return value is the number of characters (excluding the 
** terminating '\0' that would have been written.
*********************************************************************/

#include <cstdarg>
#include <cstdio>

#define snprintf 	sdb_snprintf
#define vsnprintf 	sdb_vsnprintf


inline int sdb_vsnprintf( char *str, size_t size, const char *format, va_list cp )
{
	int count = -1;

	if( size != 0 )
		count = _vsnprintf_s( str, size, _TRUNCATE, format, cp );

	if( count == -1 )
		count = _vscprintf( format, cp );

	return count;
}

inline int sdb_snprintf( char *str, size_t size, const char *format, ... )
{
	int count;
	va_list cp;

	va_start( cp, format );
	count = sdb_vsnprintf( str, size, format, cp );
	va_end( cp );

	return count;
}

#if defined( INCL_ALLOCA )
#	include <malloc.h>
#	ifndef alloca
#		define alloca _alloca
#	endif
#endif

// Replace the slow C library getenv and putenv functions with code that
// handles the TZ environment variable specially.  This hack dramatically
// speeds up POSIX time zone routines, such as mktime, that are required to
// read TZ each time they are called.

DLLEXPORT char *gs_getenv(const char *);
DLLEXPORT int gs_putenv(const char *);

#ifndef INCL_NO_GS_GETENV
#	define getenv gs_getenv
#	define putenv gs_putenv
#endif

#endif /* _PWIN32_H */
