/****************************************************************
**
**	ccconf.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/ccconf.h,v 1.81 2015/02/05 14:33:18 e19351 Exp $
**
**
**	NOTE: This header has a separate section for each compiler.
**		  If you need to add a new compiler, add a new section--
**		  don't add #ifs inside a compiler section.
**
****************************************************************/

#if !defined( IN_CCCONF_H )
#define IN_CCCONF_H


/*
**	What OS?  The only supported OSes are listed here.
**	Use these definitions like this:
**		#if CC_OS == CC_OS_UNIX
**		#if CC_UNIX_REV == CC_UNIX_SOL
*/

#define	CC_OS_OS2		0x100
#define	CC_OS_NT		0x200
#define CC_OS_UNIX		0x300

#define	CC_UNIX_SOL		0x301
#define	CC_UNIX_SUNOS	0x302
#define	CC_UNIX_AIX		0x303
#define	CC_UNIX_LINUX	0x304

// Standardize the many defines checked for in SecDb source..
// Ideal would be to stop using these and use the above defines,
// but that's an exercise for later.
#if defined( unix ) || defined( __unix )
#  ifndef unix
#    define unix
#  endif
#  ifndef __unix
#    define __unix
#  endif
#endif
#if defined( linux ) || defined( __linux ) || defined( __linux__ )
#  ifndef linux
#    define linux
#  endif
#  ifndef __linux
#    define __linux
#  endif
#  ifndef __linux__
#    define __linux__
#  endif
#endif
#if defined( sun ) || defined( __sun )
#  ifndef sun
#    define sun
#  endif
#  ifndef __sun
#    define __sun
#  endif
#endif
#if defined( sparc ) || defined( __sparc )
#  ifndef sparc
#    define sparc
#  endif
#  ifndef __sparc
#    define __sparc
#  endif
#endif

#if defined( OS2 )
#  define CC_OS CC_OS_OS2
#elif defined( WIN32 )
#  define CC_OS CC_OS_NT
#elif defined( __unix )
#  define CC_OS CC_OS_UNIX
#else
#	error Unknown operating system
#endif

#if defined( __unix )
#if   defined( __sun ) && defined( __SVR4 )
#  define CC_UNIX_REV		CC_UNIX_SOL
#elif defined( __linux__ )
#  define CC_UNIX_REV		CC_UNIX_LINUX
#else
#  error Unknown UNIX version
#endif
#endif



/*
**	What compiler?  The only supported compilers are listed here.
**	Use these definitions like this:
**		#if CC_COMPILER == CC_COMPILER_SW42
*/

#define	CC_COMPILER_MSOS2	0x1101	// Microsoft OS/2 compiler (6.0?)
#define	CC_COMPILER_IBMOS2	0x1201	// IBM C++
#define	CC_COMPILER_IBMOS2C	0x1202	// IBM C++ in C mode
#define	CC_COMPILER_IBMVA	0x1204	// IBM Visual Age

#define	CC_COMPILER_MSVC42	0x2101	// MSVC 4.2 (unsupported)
#define	CC_COMPILER_MSVC5	0x2102	// MSVC 5.0 (unsupported)
#define	CC_COMPILER_MSVC6	0x2104	// MSVC 6.0 (unsupported)
#define	CC_COMPILER_MSVC7	0x2107	// MSVC 7.0 (unsupported)
#define	CC_COMPILER_MSVC8	0x2108	// MSVC 8.0 (unsupported)
#define	CC_COMPILER_MSVC9	0x2109	// MSVC 9.0 (unsupported)
#define	CC_COMPILER_MSVC10	0x210A	// MSVC 10.0
#define	CC_COMPILER_MSVC11	0x210B	// MSVC 11.0 (unsupported)
#define	CC_COMPILER_MSVC12	0x210C	// MSVC 12.0

#define CC_COMPILER_SW6     0x3100  // SparcWorks 6.0u1
#define	CC_COMPILER_SW5		0x3101	// SparcWorks 5.0 in 5.0 mode
#define	CC_COMPILER_SW5_42	0x3102	// SparcWorks 5.0 in 4.2 mode
#define	CC_COMPILER_SW5_42_C	0x3103	// SparcWorks 5.0 in 4.2 C mode
#define	CC_COMPILER_SW42	0x3104	// SparcWorks 4.2
#define	CC_COMPILER_SW42_C	0x3108	// SparcWorks 4.2 in C mode
#define	CC_COMPILER_GCC295	0x3201	// GCC 2.95
#define	CC_COMPILER_GCC295C	0x3202	// GCC 2.95 in C mode
#define	CC_COMPILER_GCC27	0x3204	// GCC 2.7.2.1
#define	CC_COMPILER_GCC27C	0x3208	// GCC 2.7.2.1 in C mode
#define	CC_COMPILER_GCC291	0x3210	// GCC 2.91
#define	CC_COMPILER_GCC291C	0x3220	// GCC 2.91 in C mode

#define CC_COMPILER_GCC44   0x4100  // GCC 4.4.x

#if CC_OS == CC_OS_UNIX
#	if defined( __SUNPRO_C ) && !defined( __cplusplus )
#		if __SUNPRO_C < 0x500
#			define CC_COMPILER CC_COMPILER_SW42_C
#		else
#			define CC_COMPILER CC_COMPILER_SW6
#		endif
#	elif defined( __SUNPRO_CC )
#		if	!defined( __cplusplus )
#			error Unknown UNIX/SUNPRO_CC compiler: expected to be in C++ mode
#		endif
#		if __SUNPRO_CC < 0x500
#			define CC_COMPILER CC_COMPILER_SW42
#		else
#			define CC_COMPILER CC_COMPILER_SW6
#		endif
#	elif __GNUG__ == 2 && __GNUC_MINOR__ == 95
#		define CC_COMPILER CC_COMPILER_GCC295
#	elif __GNUC__ == 2 && __GNUC_MINOR__ == 95
#		define CC_COMPILER CC_COMPILER_GCC295C
#	elif __GNUG__ == 2 && __GNUC_MINOR__ == 91
#		define CC_COMPILER CC_COMPILER_GCC291
#	elif __GNUC__ == 2 && __GNUC_MINOR__ == 91
#		define CC_COMPILER CC_COMPILER_GCC291C
#	elif __GNUG__ == 2 && __GNUC_MINOR__ == 7
#		define CC_COMPILER CC_COMPILER_GCC27
#	elif __GNUC__ == 2 && __GNUC_MINOR__ == 7
#		define CC_COMPILER CC_COMPILER_GCC27C
#	elif __GNUC__ == 4 && __GNUC_MINOR__ == 4 /* && __GNUC_PATCHLEVEL__ == 5 */
#		define CC_COMPILER CC_COMPILER_GCC44
#	else
#		error Unknown UNIX compiler
#	endif
#elif CC_OS == CC_OS_NT
#	if _MSC_VER == 1100	       // VC++ 5.0
#		define CC_COMPILER CC_COMPILER_MSVC5
#	elif _MSC_VER == 1200	   // VC++ 6.0
#		define CC_COMPILER CC_COMPILER_MSVC6
#	elif _MSC_VER == 1600	   // VS2010
#		define CC_COMPILER CC_COMPILER_MSVC10
#	elif _MSC_VER == 1800	   // VS2013
#		define CC_COMPILER CC_COMPILER_MSVC12
#	else
#		error Unknown NT compiler
#	endif
#else
#	error Unknown OS--can not select compiler
#endif


/*
**	Don't try to do anything if not C++.
*/

#if !defined( __cplusplus )
#error SecDb is a C++ application
#endif

/*
**	Microsoft Visual C++
*/

#ifdef _MSC_VER
#	if _MSC_VER < 1600
#		error MSVC version must be >= VC10
#	endif

#	define CC_STDINT_HAVE
// A misnomer, but recent versions define socklen_t.
#	define CC_SOCKET_USES_SIZE_T 1

#	define CC_USE_MS_EXTENSIONS

// Disable this warning: warning C4290:
//        warning C4290: C++ Exception Specification ignored
#	pragma warning( disable: 4290 )

// Disable this warning: warning C4786:
//        warning C4786: 'symbol name' : identifier was truncated to '255' characters...
// Since this happens so many times when templates are used
#	pragma warning( disable: 4786 )

#	define CC_HAVE_IOSFWD
#	define CC_DELETE_PLACEMENT

#endif

/*
**	gcc 4.4.x in C++ mode
*/

#if CC_COMPILER == CC_COMPILER_GCC44

#define CC_GSCVSID_VIA_IDENT
//#define CC_NO_STD_STREAMS
#define CC_SOCKET_USES_SIZE_T	1
#define CC_HAVE_IOSFWD
#define CC_HAS_ALLOCA_H

// Max & Min are a pain.  Deal with them this way
#define CC_NEED_MIN_MAX

//#	define CC_HAVE_STD_CCTYPE

#if CC_UNIX_REV == CC_UNIX_SOL
#	define CC_STDINT_USE_INTTYPES
#	define CC_BROKEN_RECV		1
#elif CC_UNIX_REV == CC_UNIX_LINUX
#	define CC_STDINT_HAVE			// Red Hat Linux 6.0 and greater
#endif

#endif

#include <cccomp.h>

#endif
