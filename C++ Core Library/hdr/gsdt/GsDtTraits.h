/****************************************************************
**
**	GSDTTRAITS.H	- Traits class for generic gsdts
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtTraits.h,v 1.2 2011/05/24 14:03:05 khodod Exp $
**
****************************************************************/

#ifndef IN_GSDTLITE_GSDTTRAITS_H
#define IN_GSDTLITE_GSDTTRAITS_H

#include <gsdt/base.h>
#include <gscore/gsdt_fwd.h>
#include <gsdt/GsDtTypeMap.h>
#include <gsdt/GsDtDefaultTraits.h>
#include <gsdt/GsDtTraited.h>

CC_BEGIN_NAMESPACE( Gs )


/*
** Macros for declaring simple, traited gsdt types.
*/

#define DEFAULT_GSDTTRAITED( T ) \
GsDtTraited< T, default_rtti_traits< T >, default_subscript_traits< T >, GsDtDefaultIterator< T >  >; 

#define TYPEMAP_GSDTTRAITED( T )                                                                                           \
template<>                                                                                                                 \
struct GsDtTypeMap< T >                                                                                                    \
{                                                                                                                          \
  typedef GsDtTraited< T, default_rtti_traits< T >, default_subscript_traits< T >, GsDtDefaultIterator< T > > GsDtType;    \
};

#define SPECIALIZE_GSDTTRAITED_TYPENAME_FWD_DECL( Name, T )                                                                \
GsString GsDtTraited< T, default_rtti_traits< T >, default_subscript_traits< T >, GsDtDefaultIterator< T > >::myTypeName();

#define SPECIALIZE_GSDTTRAITED_TYPENAME( Name, T )                                                                         \
GsString GsDtTraited< T, default_rtti_traits< T >, default_subscript_traits< T >, GsDtDefaultIterator< T > >::myTypeName() \
{                                                                                                                          \
	return GsString( Name );                                                                                               \
}

// Add entries to the type map here
// eg.
//   class Foo;
//   TYPEMAP_GSDTTRAITED( Foo );

CC_END_NAMESPACE

#endif 
