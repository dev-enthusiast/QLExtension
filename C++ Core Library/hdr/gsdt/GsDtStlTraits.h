/****************************************************************
**
**	GSDTSTLTRAITS.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtStlTraits.h,v 1.10 2011/05/24 14:03:05 khodod Exp $
**
****************************************************************/

#ifndef IN_GSDTLITE_GSDTSTLTRAITS_H
#define IN_GSDTLITE_GSDTSTLTRAITS_H

#include <gsdt/base.h>
#include <gscore/gsdt_fwd.h>
#include <gsdt/GsDtTypeMap.h>
#include <gsdt/GsDtTraits.h>
#include <gsdt/GsDtStlIterator.h>
#include <vector>

CC_BEGIN_NAMESPACE( Gs )

template< class T >
struct stl_subscript_traits;

/*
** Some helper macros
*/

#define DEFAULT_STL_GSDTTRAITED( T ) \
GsDtTraited< T, default_rtti_traits< T >, stl_subscript_traits< T >, GsDtDefaultStlIterator< T > >;   

#define TYPEMAP_STL_GSDTTRAITED( T )                                                                                       \
template<>                                                                                                                 \
struct GsDtTypeMap< T >                                                                                                    \
{                                                                                                                          \
  typedef GsDtTraited< T, default_rtti_traits< T >, stl_subscript_traits< T >, GsDtDefaultStlIterator< T > > GsDtType;     \
};

#define SPECIALIZE_STL_GSDTTRAITED_TYPENAME_FWD_DECL( Name, T )                                                            \
template<>                                                                                                                 \
GsString GsDtTraited< T, default_rtti_traits< T >, stl_subscript_traits< T >, GsDtDefaultStlIterator< T > >::myTypeName();  

#define SPECIALIZE_STL_GSDTTRAITED_TYPENAME( Name, T )                                                                     \
template<>                                                                                                                 \
GsString GsDtTraited< T, default_rtti_traits< T >, stl_subscript_traits< T >, GsDtDefaultStlIterator< T > >::myTypeName()  \
{                                                                                                                          \
	return GsString( Name );                                                                                               \
}

// Add STL entries to the type map here

TYPEMAP_STL_GSDTTRAITED( CC_STL_VECTOR( double ) )


/****************************************************************
**
**  stl_subscript_traits
**
**  Default Stl subscripting behaviour for GsDtTraited
**
****************************************************************/

template< class T >
struct stl_subscript_traits
{
	typedef typename GsDtTypeMap< T >::GsDtType::rtti rtti;

	inline static GsDt::Iterator *Subscript( typename GsDtTypeMap< T >::GsDtType& t, const GsDt &Index )
	{
		CC_OSTRSTREAM Stream;

		Stream << "Subscript() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return NULL; 
	}

	inline static GsDt::Iterator *SubscriptFirst( typename GsDtTypeMap< T >::GsDtType& t, GsDt::IterationFlag Flag )
	{
		return new typename GsDtTypeMap< T >::GsDtType::iterator( &t );
	}

	inline static GsStatus SubscriptReplace( typename GsDtTypeMap< T >::GsDtType& t, const GsDt &Index, GsDt *NewValue, GsCopyAction Action )
	{
		CC_OSTRSTREAM Stream;

		Stream << "StlTraits::SubscriptReplace() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return GS_ERR;
	}

	inline static GsStatus SubscriptDelete( typename GsDtTypeMap< T >::GsDtType& t, const GsDt &Index )
	{
		CC_OSTRSTREAM Stream;

		Stream << "StlTraits::SubscriptDelete() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return GS_ERR;
	}

	inline static GsStatus SubscriptAppend( typename GsDtTypeMap< T >::GsDtType& t, GsDt *NewValue, GsCopyAction Action )
	{
		CC_OSTRSTREAM Stream;

		Stream << "StlTraits::SubscriptAppend() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return GS_ERR;
	}
};

CC_END_NAMESPACE

#endif 
