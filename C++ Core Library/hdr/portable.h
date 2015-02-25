/****************************************************************
**
**	PORTABLE.H	- Machine/Compiler Portable definitions
**
**	$Header: /home/cvs/src/portable/src/portable.h,v 1.119 2012/07/03 18:01:08 e19351 Exp $
**
****************************************************************/

#ifndef IN_PORTABLE_H
#define IN_PORTABLE_H

/*
**	Conditional definitions:
**		INCL_DIRECTORY	- Include directory operations
**		INCL_ENDIAN		- Include big/little endian operators
**		INCL_FILEIO		- Include low level file IO
**		INCL_THREADS	- Include multithread library functions
**		USE_GS_STATUS	- Use the new definition of GsStatus as an enum.
**						  Without this setting, GsStatus is just an int.
**
**      INCL_NO_GS_TIME   - Turn off the #defines that redirect all ANSI time
**                          calls to the gs_xxx routines in the time project.	
**      INCL_NO_GS_GETENV - Turn off the #defines that redirect getenv/putenv.
**      INCL_NO_SDB_MATH  - Turn off the #defines that redirect math functions.
*/


/*
**	Special EXPORT macros. DEPRECATED!!!!! use the below LIBNAME_API scheme.
**
**	The following three macros are used to declare exportable/importable symbols
**	in WIN32. They should not be used directly in the code. In one header
**	file each library should include the following (where LIBNAME is replaced by
**	the name of the library):
**
**	#ifdef BUILDING_LIBNAME
**	#define EXPORT_C_LIBNAME  EXPORT_C_EXPORT
**	...
**	#endif
**	#define EXPORT_C_LIBNAME  EXPORT_C_IMPORT
**	...
**	#endif
**
**
**	EXPORT_C_EXPORT		- Function/Variable is exported from a DLL with "C" linkage.
**	EXPORT_CPP_EXPORT	- Function/Variable is exported from a DLL with "C++" linkage.
**						  (The external symbol name will be mangled.)
**	EXPORT_CLASS_EXPORT	- Class and all member functions/static vars/... are exported.
**
**	EXPORT_C_IMPORT		- Function/Variable is imported from a DLL with "C" linkage.
**	EXPORT_CPP_IMPORT	- Function/Variable is imported from a DLL with "C++" linkage.
**						  (The external symbol name will be mangled.)
**	EXPORT_CLASS_IMPORT	- Class and all member functions/static vars/... are imported.
**
**
**	All of these macros have been made obsolete by the above macros.  New code
**	should use EXPORT_xxx_libname
**
**	  EXPORT  		- Obsolete?
**
**	  DLLEXPORT		- Used by WIN32 to identify C symbols which are 
**					  exportable from the DLL. 
**					  On Unix, all 'extern' symbols are exported.
**
**	  CLASS_EXPORT	- Used by WIN32 to identify C++ classes to be
**					  exported by the DLL.  
**
**	  CPP_EXPORT	- A suffix! used by WIN32 to declare a C++ function 
**					  exportable. i.e. "int CPP_EXPORT function( ... );
**
*/

#define EXPORT_CPP_EXPORT SECDB_DLL_EXPORT
#define EXPORT_CPP_IMPORT SECDB_DLL_IMPORT
#define EXPORT_CLASS_EXPORT SECDB_DLL_EXPORT
#define EXPORT_CLASS_IMPORT SECDB_DLL_IMPORT
#define EXPORT_C_EXPORT extern "C" SECDB_DLL_EXPORT
#define EXPORT_C_IMPORT extern "C" SECDB_DLL_IMPORT
#define DLLEXPORT extern "C" SECDB_DLL_IMPORT



// DLL API visibility macros. Use them! Even on *nix.
// In each project a common header should define the following for every DLL:
//
// #ifdef BUILDING_{LIBRARY}
// #	define {LIBRARY}_API SECDB_DLL_EXPORT
// #else
// #	define {LIBRARY}_API SECDB_DLL_IMPORT
// #endif
//
// These should be used for declarations:
//
// extern "C" {LIBRARY}_API void myCFunction();
// {LIBRARY}_API void myCPPFunction();
// class {LIBRARY}_API MyClass {..};

#ifdef WIN32
#	ifdef _MSC_VER
#		define SECDB_DLL_EXPORT __declspec(dllexport)
#		define SECDB_DLL_IMPORT __declspec(dllimport)
#	elif defined( __GNUC__ )
#		define SECDB_DLL_EXPORT __attribute__ ((dllexport))
#		define SECDB_DLL_IMPORT __attribute__ ((dllimport))
#	endif
#elif defined( __GNUC__ )
#	define SECDB_DLL_EXPORT __attribute__ ((visibility ("default")))
#	define SECDB_DLL_IMPORT __attribute__ ((visibility ("default")))
#else
#	define SECDB_DLL_EXPORT
#	define SECDB_DLL_IMPORT
#endif



// Mark functions as deprecated. Takes advantage of built-in support for
// function deprecation warnings in some compilers. Example:
//
// DEPRECATED void oldFunction();
// void newFunction();

#ifdef __GNUC__
#define DEPRECATED __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#define DEPRECATED
#endif



/*
** Support for language features across compilers
*/

#include <ccconf.h>
						
/*
**	The GNU C compiler allows us to indicate that some functions are invoked
**	with a printf style format followed by a variable number of values.  The
**	value of x below indicates which positional argument is the printf style
**	format string.
*/

#ifdef __GNUC__
#define ARGS_LIKE_PRINTF( x ) __attribute__(( format (printf, (x), (x) + 1) ))
#else
#define ARGS_LIKE_PRINTF( x )
#endif


/****************************************************************
**
**	Redefine certain functions
**
****************************************************************/

#define tempnam Do_Not_Use_non_ANSI_tempnam
DLLEXPORT char
		*TempFileName( const char *Dir, const char *Prefix );


/****************************************************************
**
**	COMMON DEFINITIONS
**
****************************************************************/

#if !defined( TRUE )
#	define	TRUE				1
#endif

#if !defined( FALSE )
#	define	FALSE				0
#endif

//	GsBool is used to indicate TRUE/FALSE when the Err stack has
// not been started.
#define GsBool bool

//	GsStatus is the value returned from a function to indicate
//	success or failure with the Err stack started.
typedef enum GsStatus
{
	GS_ERR = FALSE,
	GS_OK  = TRUE

} GsStatus;


/*
**	The global list of unchecked DLLs
*/

typedef struct DllVersionStructure DLL_VERSION;

DLLEXPORT DLL_VERSION
		*DllVersionList;


/*
**	Big/Little endian defines - BYTE_ORDER is one of these values
*/

#ifndef linux
#define	LITTLE_ENDIAN	1234
#define	BIG_ENDIAN	4321
#endif


/*
**	Prototype functions
*/

DLLEXPORT void
		SleepDouble(	double Seconds ),
		TimeZoneSetup(	void );


/*******************************************************************
**	Fix the <ctype.h> macros
**	
**	Define uppercase versions of the functions in <ctype.h> which
**	solve two problems:
**		1)	ctype.h functions are defined to accept values from
**			the set EOF and unsigned char.  For historical reasons,
**			we compile using signed chars.  These macros are
**			intended to be used for signed or unsigned chars, but
**			not for EOF.
**		2)	Some compilers implement the ctype.h functions as
**			macros and some implement them as functions.  The
**			uppercase versions are intended to always be macros
**			or in-line functions.
**	
**	By default, use the normal macros, but cast to unsigned char,
**	since most operating systems define macros for these functions.
**	If necessary, they are redefined in compiler-specific sections.
*******************************************************************/

#define ISALNUM( c )  isalnum(  (unsigned char) (c) )
#define ISALPHA( c )  isalpha(  (unsigned char) (c) )
#define ISCNTRL( c )  iscntrl(  (unsigned char) (c) )
#define ISDIGIT( c )  isdigit(  (unsigned char) (c) )
#define ISGRAPH( c )  isgraph(  (unsigned char) (c) )
#define ISLOWER( c )  islower(  (unsigned char) (c) )
#define ISPRINT( c )  isprint(  (unsigned char) (c) )
#define ISPUNCT( c )  ispunct(  (unsigned char) (c) )
#define ISSPACE( c )  isspace(  (unsigned char) (c) )
#define ISUPPER( c )  isupper(  (unsigned char) (c) )
#define ISXDIGIT( c ) isxdigit( (unsigned char) (c) )
#define TOLOWER( c )  tolower(  (unsigned char) (c) )
#define TOUPPER( c )  toupper(  (unsigned char) (c) )

/****************************************************************
**
**	Microsoft Windows
**
****************************************************************/

#if defined( WIN32 )

#include <pwin32.h>

/****************************************************************
**
**	UNIX
**
****************************************************************/

#elif defined( __unix )

#include <punix.h>

/****************************************************************
**
**	UNKNOWN PLATFORM
**
****************************************************************/

#else
#	error PORTABLE.H - Unknown platform being used
#endif


/****************************************************************
**
**	BYTE ORDERING
**
****************************************************************/

#if defined( INCL_ENDIAN )

#	if BYTE_ORDER == LITTLE_ENDIAN
#		define LITTLE_ENDIAN_2( x )		(x)
#		define LITTLE_ENDIAN_4( x )		(x)
#	elif BYTE_ORDER == BIG_ENDIAN
#		define LITTLE_ENDIAN_2( x )									\
			( (((( unsigned short )( x )) & 0x00ff ) << 8 )			\
			 |(((( unsigned short )( x )) & 0xff00 ) >> 8 )
#		define LITTLE_ENDIAN_4( x )	\
			( (((( unsigned long )( x )) & 0x000000ff ) << 24 ) 	\
			 |(((( unsigned long )( x )) & 0x0000ff00 ) <<  8 )		\
			 |(((( unsigned long )( x )) & 0x00ff0000 ) >>  8 )		\
			 |(((( unsigned long )( x )) & 0xff000000 ) >> 24 ))
#	else
#		error PORTABLE.H - Unknown operating system being used (INCL_ENDIAN)
#	endif

#endif

#ifndef INCL_NO_GS_TIME

// Temporary hack to switch the entire build area over to using the gs_xxxx
// time zone routines.  Once all instances of these symbols in the code are
// changed, this hack can be removed.

#include <timezone.h>

#define TIMEZONE_SETUP()

// POSIX time API.

#define asctime gs_asctime
#define asctime_r gs_asctime_r
#define ctime gs_ctime
#define ctime_r gs_ctime_r
#define difftime gs_difftime
#undef gmtime
#define gmtime gs_gmtime
#undef gmtime_r
#define gmtime_r gs_gmtime_r
#undef localtime
#define localtime gs_localtime
#undef localtime_r
#define localtime_r gs_localtime_r
#define mktime gs_mktime
#define tzname gs_tzname
#define tzset gs_tzset
#define strftime gs_strftime

// Non-POSIX functions used in our code.

#define timegm gs_timegm

#endif // not INCL_NO_GS_TIME

/*
**	Replaces the old min/max macros on all platforms. This should be moved
**	to the usage site and removed, using std::min( a, b ) as needed.
*/

#include <algorithm>

using std::max;
using std::min;
#	define DEFMAX(T,T1,T2) inline T max( T1 a, T2 b ) { return a > b ? a : b; }
#	define DEFMIN(T,T1,T2) inline T min( T1 a, T2 b ) { return a < b ? a : b; }
#	define DEFMM(T,T1,T2)	\
				DEFMAX(T,T1,T2) DEFMAX(T,T2,T1) \
				DEFMIN(T,T1,T2) DEFMIN(T,T2,T1)
// When the args are different types, the return type is ambiguous. These
// specify it for common types, imitating the relational operators (< >),
// for compatibility with the old macros.
DEFMM(long,long,int)
DEFMM(double,double,float)
DEFMM(double,double,int)
DEFMM(double,double,long)
#	undef	DEFMAX
#	undef	DEFMIN
#	undef	DEFMM

// Replace calls to certain math functions from libm with calls
// to our custom version that gives consistent results across 
// platforms. This behavior may be turned off by setting 
// INCL_NO_SDB_MATH.

DLLEXPORT double sdb_exp( double );
DLLEXPORT double sdb_log( double );
DLLEXPORT double sdb_pow( double, double );
DLLEXPORT double sdb_scalbn( double, int );

#ifndef INCL_NO_SDB_MATH

#define exp sdb_exp
#define log sdb_log
#define pow sdb_pow
#define scalbn sdb_scalbn

#endif

#endif
