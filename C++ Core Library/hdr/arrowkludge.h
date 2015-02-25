/****************************************************************
**
**	ARROWKLUDGE.H	- helper class to work aroung brain damaged
**                    SW4.2 compiler.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/arrowkludge.h,v 1.11 2004/10/18 16:28:37 khodod Exp $
**
****************************************************************/

#ifndef IN_PORTABLE_ARROWKLUDGE_H
#define IN_PORTABLE_ARROWKLUDGE_H

//
// Usage:
//
// struct Foo
// {
//     typename arrowReturnKludge::type operator->() { return m_ptr;}
//     typename arrowReturnKludge::const_type operator->() const { return m_ptr;}
// };
// 



template< class _T >
struct arrowReturnKludge
{
    typedef _T*       type;
    typedef _T const* const_type;
};


#if CC_COMPILER==CC_COMPILER_SW42
template< class T_ >
struct kludge_arrow_breaker {
	kludge_arrow_breaker( T_ * d ) { }
};

class SDB_DB;
class SDB_NODE;
class SDB_OBJECT;
class calculationRequest;
class GsObserver;
class node_info;

struct SLANG_SCOPE_DIDDLE;

#define SW_ARROW_RETURN_SPECIAL( _T_ )					\
  struct arrowReturnKludge<_T_>							\
  {														\
	  typedef kludge_arrow_breaker<_T_>* type;			\
	  typedef kludge_arrow_breaker<_T_>* const_type;	\
  }

SW_ARROW_RETURN_SPECIAL( char );
SW_ARROW_RETURN_SPECIAL( signed char );
SW_ARROW_RETURN_SPECIAL( unsigned char );

SW_ARROW_RETURN_SPECIAL( short );
SW_ARROW_RETURN_SPECIAL( unsigned short );

SW_ARROW_RETURN_SPECIAL( int );
SW_ARROW_RETURN_SPECIAL( unsigned int );

SW_ARROW_RETURN_SPECIAL( long );
SW_ARROW_RETURN_SPECIAL( unsigned long );

SW_ARROW_RETURN_SPECIAL( float );
SW_ARROW_RETURN_SPECIAL( double );
SW_ARROW_RETURN_SPECIAL( long double );

SW_ARROW_RETURN_SPECIAL( char const* );
SW_ARROW_RETURN_SPECIAL( char const* const );

SW_ARROW_RETURN_SPECIAL( SDB_NODE* );
SW_ARROW_RETURN_SPECIAL( const SDB_OBJECT * );
SW_ARROW_RETURN_SPECIAL( calculationRequest* );
SW_ARROW_RETURN_SPECIAL( GsObserver* );
SW_ARROW_RETURN_SPECIAL( SDB_DB* );
SW_ARROW_RETURN_SPECIAL( node_info* );
SW_ARROW_RETURN_SPECIAL( SLANG_SCOPE_DIDDLE * );

#undef SW_ARROW_RETURN_SPECIAL

#endif // CC_COMPILER_SW42


#endif 
