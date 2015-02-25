/****************************************************************
**
**	GSDTDEFAULTTRAITS.H	- Default Traits structurs for GsDtTraited
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtDefaultTraits.h,v 1.7 2001/11/27 22:44:59 procmon Exp $
**
****************************************************************/

#ifndef IN_GSDTLITE_GSDTDEFAULTTRAITS_H
#define IN_GSDTLITE_GSDTDEFAULTTRAITS_H

#include <gsdt/base.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsString.h>
#include <gscore/GsException.h>
#include <gsdt/GsDt.h>
#include <gsdt/GsDtTraited.h>
#include <gsdt/GsDtTraits.h>
#include <ccstream.h>


CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
**
**  default_rtti_traits
**
**  RTTI traits for GsDtTraited-aware reps
**
****************************************************************/

template< class T >
struct default_rtti_traits
{
	static GsString toString( const T& t );	
	static const char *typeName();
};

template< class T >
GsString default_rtti_traits< T >::toString( const T& t )
{ 
	return t.toString(); 
}

template< class T >
const char *default_rtti_traits< T >::typeName()
{ 
	typedef typename GsDtTypeMap< T >::GsDtType::rtti rtti;
	return rtti::typeName();
}

/****************************************************************
**
**  default_subscript_traits
**
**  Default subscripting behaviour for GsDtTraited
**
****************************************************************/

template< class T >
struct default_subscript_traits
{
	typedef typename GsDtTypeMap< T >::GsDtType::rtti rtti;
	
	inline static GsDt::Iterator *Subscript( const typename GsDtTypeMap< T >::GsDtType& t, const GsDt &Index )
	{ 
		CC_OSTRSTREAM Stream;

		Stream << "Subscript() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return NULL;
	}

	inline static GsDt::Iterator *SubscriptFirst( const typename GsDtTypeMap< T >::GsDtType& t, GsDt::IterationFlag Flag )
	{
		CC_OSTRSTREAM Stream;

		Stream << "SubscriptFirst() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return NULL;
	}

	inline static GsStatus SubscriptReplace( typename GsDtTypeMap< T >::GsDtType& t, const GsDt &Index, GsDt *NewValue, GsCopyAction Action )
	{
		CC_OSTRSTREAM Stream;

		Stream << "SubscriptReplace() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return GS_ERR;
	}

	inline static GsStatus SubscriptDelete( typename GsDtTypeMap< T >::GsDtType& t, const GsDt &Index )
	{
		CC_OSTRSTREAM Stream;

		Stream << "SubscriptDelete() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return GS_ERR;
	}

	inline static GsStatus SubscriptAppend( typename GsDtTypeMap< T >::GsDtType& t, GsDt *NewValue, GsCopyAction Action )
	{
		CC_OSTRSTREAM Stream;

		Stream << "SubscriptAppend() is not supported by " << rtti::typeName();
		GSX_THROW( GsXUnsupported, CC_OSTRSTREAM_STR( Stream ) );

		return GS_ERR;
	}
};


CC_END_NAMESPACE

#endif 
