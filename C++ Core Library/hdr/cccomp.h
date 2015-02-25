/****************************************************************
**
**	cccomp.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/cccomp.h,v 1.45 2012/07/03 13:47:03 e19351 Exp $
**
****************************************************************/

#if !defined( IN_CCCOMP_H )
#define IN_CCCOMP_H

/*
  Conditional Language Support Macros 
 
  macro                    Explanation
  ------------------------ -----------------------------------------------------
  CC_NO_POSTFIX            Some especially old compilers do not support
                           postfix operators properly

  CC_NO_BOOL               This replaces uses of the 'bool' keyword with int 
                           for compilers which do not support it.

  CC_NO_TYPENAME           This removes references to the 'typename' keyword
                           for compilers which do not support it.

  CC_NO_EXPLICIT           This removes references to the 'explicit' keyword
                           for compilers which do not support it.

  CC_NO_MUTABLE            This removes references to the 'mutable' keyword
                           for compilers which do not support it and controls
						   definition of CC_MUTABLE() macro.

  CC_NO_NAMESPACES         This removes all references to namespaces for 
                           compilers which do not support them.

  CC_NO_EXCEPTIONS         This removes all references to exceptions for
                           compilers which do not support them. Defining this
                           macro implies the definition of CC_NO_EXCEPTION_SPECS.
  
  CC_NO_EXCEPTION_SPECS    This macro removes all of the exception specifications
                           from function declarations.
  
  CC_NO_STATIC_CAST        This macro changes all calls to the static_cast<> 
                           operator to old C-style casts for compilers which do
                           not support them.

  CC_NO_CONST_CAST         This macro changes all calls to the const_cast<> 
                           operator to old C-style casts for compilers which do
                           not support them.

  CC_NO_REINTERPRET_CAST   This macro changes all calls to the reinterpret_cast<> 
                           operator to old C-style casts for compilers which do
                           not support them.

  CC_NO_STD_STREAMS         If the version of the standard library that is
                            included with the compiler does not yet have an
                            implementation of the standard template-based stream
                            classes, we must fall back to the older versions of
                            istream and ostream.

  CC_NO_STD_INCLUDES        Some compilers have not yet included header files
                            whose names omit the .h extension.

  CC_NO_TEMPLATE_DEFAULTS   Some compilers do not support default arguments for
                            templates. This macro is used directly in the code
                            to test if full argument specification is necessary.

  CC_NO_MEMBER_TEMPLATES    Many compilers do not support member function 
                            templates. This maro is used in the code to check
                            if declaring or calling a member function template
                            is permissable.

  CC_NO_SPECIALIZATION      Some compilers do not support template specialization
                            This will limit our use of some templates like
                            numeric_limits.

  CC_NO_PARTIAL_SPEC        Many compilers do not support partial specialization
                            of templates. This will limit out use of templates
                            like iterator_traits.

  CC_NEED_MIN_MAX			If defined, then we need definitions for min & max
							See punix.h

  CC_BROKEN_RECV			SparcWorks5 needs some funky magic to get a proper
							definition for recv().  See tcpip.h

  CC_SOCKET_USES_SIZE_T		If defined, then getsockopt() takes size_t, else it
							takes int.  Use CC_SOCKET_SIZE_T.

  CC_STDINT_HAVE			Have <stdint.h>, so use that.

  CC_STDINT_USE_INTTYPES	Have <inttypes.h>, so use that.

  CC_STDINT_LINUX			Part in <sys/types.h>, part not.  Puke.

  CC_STDINT_NONE			Don't know where stdint types are, so don't define them.

  CC_GSCVSID_VIA_IDENT		If defined, use
								#ident GSCVSID
							else use
								static char *gscvsid=GSCVSID

  CC_HAS_ALLOCA_H			Have <alloca.h>

  CC_NO_OVERLOADED_ENUMS		The C++ compiler does not support function overloading based on enums

  CC_BROKEN_TEMPLATE_FUNCTIONS 	The C++ compiler has broken support for templated functions.
  								Functions are only supported where one of the arguments is of 
								a templated type.

  CC_NO_NEW_HEADERS			Does not have <exception>--has <exception.h>.

*/

#ifdef CC_NO_BOOL

class bool
{
	public:
		bool() {}
		bool(const bool& x) : m_val(x.m_val) {}
		bool(int i) : m_val(!!i) {}

		bool operator = (const bool& x) { m_val=x.m_val; return *this; }
		bool operator = (const int& i) { m_val=!!i; return *this; }

		operator int () const { return m_val; }

	private:
		int m_val;
};

#define true  bool(1)
#define false bool(0)

#endif /* CC_NO_BOOL */

#ifdef CC_NO_EXPLICIT
#  define explicit 
#endif

#if defined(CC_BOOL_IS_INT)
typedef int bool;
#if ! defined(true)
#define true 1
#endif
#if ! defined(false)
#define false 0
#endif
#endif

#ifdef CC_NO_MUTABLE
#  define mutable 
#  define CC_MUTABLE( t ) ( (t *)this )
#else
#  define CC_MUTABLE( t ) this
#endif

#ifdef CC_NO_TYPENAME
#  define typename
#endif

#ifdef CC_NO_STATIC_CAST
#  define CC_STATIC_CAST(t,v) ((t)v)
#else
#  define CC_STATIC_CAST(t,v) static_cast< t >(v)
#endif

#ifdef CC_NO_REINTERPRET_CAST
#  define CC_REINTERPRET_CAST(t,v) ((t)v)
#else
#  define CC_REINTERPRET_CAST(t,v) reinterpret_cast< t >(v)
#endif 

#ifdef CC_NO_CONST_CAST
#  define CC_CONST_CAST(t,v) ((t)v)
#else
#  define CC_CONST_CAST(t,v) const_cast< t >(v)
#endif

#ifndef CC_NO_EXCEPTIONS
#  define CC_TRY try
#  define CC_CATCH(e) catch(e) 
#  define CC_CATCH_ALL catch(...)
#  define CC_THROW(e) throw e 
#  define CC_RETHROW throw
#  define CC_UNWIND(action) catch(...) { action; throw; }
#else /* CC_NO_EXCEPTIONS */
#  define CC_TRY
#  define CC_CATCH(e) if (false) 
#  define CC_CATCH_ALL  if (false)
#  define CC_THROW(e)  
#  define CC_RETHROW
#  define CC_UNWIND(action)
#endif /* CC_NO_EXCEPTIONS */

#if defined(CC_NO_EXCEPTIONS) || defined(CC_NO_EXCEPTION_SPECS)
#  define CC_NOTHROW
#  define CC_THROW1(e1)
#  define CC_THROW2(e1,e2)
#  define CC_THROW3(e1,e2,e3)
#  define CC_THROW4(e1,e2,e3,e4)
#else
#  define CC_NOTHROW throw()
#  define CC_THROW1(e1) throw(e1)
#  define CC_THROW2(e1,e2) throw(e1,e2)
#  define CC_THROW3(e1,e2,e3) throw(e1,e2,e3)
#  define CC_THROW4(e1,e2,e3,e4) throw(e1,e2,e3,e4)
#endif

// Short-term compatibility
// FIX-Remove this as soon as possible
#  define CC_THROW_SPEC1(e1)          CC_THROW1(e1)
#  define CC_THROW_SPEC2(e1,e2)       CC_THROW2(e1,e2)
#  define CC_THROW_SPEC3(e1,e2,e3)    CC_THROW3(e1,e2,e3)
#  define CC_THROW_SPEC4(e1,e2,e3,e4) CC_THROW4(e1,e2,e3,e4)

#ifdef CC_NO_NAMESPACES
#  define CC_BEGIN_NAMESPACE(x)
#  define CC_END_NAMESPACE
#  define CC_USING(x)        
#  define CC_USING_NAMESPACE(x) 
#  define CC_ALIAS(x,y)          
#  define CC_NS(n,x)            x
#else /* CC_NO_NAMESPACES */
#  define CC_BEGIN_NAMESPACE(x) namespace x {
#  define CC_END_NAMESPACE      }
#  define CC_USING(x)           using x
#  define CC_USING_NAMESPACE(x) using namespace x
#  define CC_ALIAS(x,y)         namespace x = y 
#  define CC_NS(n,x)            n::x
#endif /* CC_NO_NAMESPACES */

		   
#if defined(CC_USE_MS_EXTENSIONS)
#  define CC_MS_DECLSPEC(x)  __declspec( x )
#  define CC_MS_CDECL        __cdecl
#  define CC_MS_STDCALL      __stdcall
#  define CC_MS_FASTCALL     __fastcall 
#else
#  define CC_MS_DECLSPEC(x)
#  define CC_MS_CDECL
#  define CC_MS_STDCALL  
#  define CC_MS_FASTCALL  
#endif
		   
#if !defined( CC_NO_STREAMS )

#  define CC_ISTREAM		CC_NS(std,istream)
#  define CC_OSTREAM		CC_NS(std,ostream)
#  define CC_ISTRSTREAM		CC_NS(std,istringstream)
#  define CC_OSTRSTREAM		CC_NS(std,ostringstream)
#  define CC_IFSTREAM		CC_NS(std,ifstream)
#  define CC_OFSTREAM		CC_NS(std,ofstream)
#  define CC_STREAMBUF		CC_NS(std,streambuf)
#  define CC_FILEBUF		CC_NS(std,filebuf)
#  define CC_SETFILL		CC_NS(std,setfill)
#  define CC_SETPRECISION	CC_NS(std,setprecision)
#  define CC_CERR			CC_NS(std,cerr)
#  define CC_COUT			CC_NS(std,cout)
#  define CC_CIN			CC_NS(std,cin)
#  define CC_ENDL			CC_NS(std,endl)
#  define CC_ENDS			CC_NS(std,ends)

#  define CC_OSTRSTREAM_STR(s) (s.str())

#  if !defined(CC_NO_STD_STREAMS)

#    define CC_SKIP_WS      CC_NS(std,skipws)
#    define CC_SYNC         CC_NS(std,pubsync)

#    define CC_PUBSEEKOFF   pubseekoff
#    define CC_IOS_OPENMODE ::std::ios::openmode

#  else

#    if CC_COMPILER == CC_COMPILER_SW42

#      define CC_SKIP_WS      ws
#      define CC_SYNC         sync
#      define CC_PUBSEEKOFF   seekoff
#      define CC_IOS_OPENMODE ios::open_mode

#    elif CC_COMPILER == CC_COMPILER_GCC44

#      define CC_SKIP_WS      skipws
#      define CC_SYNC         sync
#      define CC_PUBSEEKOFF   pubseekoff
#      define CC_IOS_OPENMODE ios::openmode

#    elif CC_COMPILER == CC_COMPILER_GCC295

#      define CC_SKIP_WS      skipws
#      define CC_SYNC         sync
#      define CC_PUBSEEKOFF   pubseekoff
#      define CC_IOS_OPENMODE ios::openmode

#    else

#      error "unknown compiler for non Standard IOStreams"

#    endif /* CC_COMPILER */
#  endif /* CC_NO_STD_STREAMS */
#endif /* CC_NO_STREAMS */

#if defined( CC_SOCKET_USES_SIZE_T )
#	define CC_SOCKET_SIZE_T socklen_t
#else
#	define CC_SOCKET_SIZE_T int
#endif

/*
** Suppress the unused variable warning. Only needed 
** for the GSCVSID stuff below, in principle.
*/

#if defined( __GNUC__ )
#	define CC_SUPPRESS_UNUSED_VAR_WARNING __attribute__ ((unused))
#else
#	define CC_SUPPRESS_UNUSED_VAR_WARNING
#endif


/*
**	Record GSCVSID somehow
*/

#ifdef GSCVSID
#if defined( CC_GSCVSID_VIA_IDENT )
#ident GSCVSID
#else
static const char CC_SUPPRESS_UNUSED_VAR_WARNING *gscvsid = GSCVSID;

// Note: static const char gscvsid[] does not work on NT.
//       It appears to get optimized away.

// static const char gscvsid[] = GSCVSID; -- os2ibm version
// if only this would work in win32: #pragma comment( user, GSCVSID )
// static const char *gscvsid = GSCVSID; -- win32 version
#endif
#endif

#endif

