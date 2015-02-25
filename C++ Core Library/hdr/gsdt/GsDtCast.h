/****************************************************************
**
**	gsdt/GsDtCast.h - 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtCast.h,v 1.9 2001/10/03 14:53:41 tsoucp Exp $
**
****************************************************************/

#ifndef IN_GSDT_GSDTCAST_H
#define IN_GSDT_GSDTCAST_H

#include <gsdt/base.h>

#include <gsdt/GsDtArray.h>

#include <vector>
#include <ccstl.h>
#include <algorithm>

CC_BEGIN_NAMESPACE( Gs )

// Note: Can't have default arguments to template funcs in SW4.2
// Note: Must have the template param class <T> as an argument to function in SW4.2
template< class T >  
CC_STL_VECTOR(T) GsDtCastFromArraySorted(
	const GsDtArray&	array, 
	bool				ignore,	// Ignore elements of the wrong type
	T *							// justToGetTheType
)
{
	CC_STL_VECTOR(T) retVec;
	retVec = GsDtCastFromArray( array, ignore, (T*) 0 );
	CC_NS(std,sort)( retVec.begin(), retVec.end() );

	return retVec;
}

template< class T >  
CC_STL_VECTOR(T) GsDtCastFromArray(
	const GsDtArray&	array, 
	bool				ignore,	// Ignore elements of the wrong type
	T *							// justToGetTheType
)
{
	CC_STL_VECTOR(T) retVec;

	for( unsigned i = 0; i < array.size(); i++ )
	{
		const GsDtGeneric< T >* gsdt = 
				GsDtGeneric< T >::Cast( array.elem( i ) );

		if( ! gsdt ) 
		{
			if( ignore )
				continue;

			GSX_THROW( GsXInvalidArgument, 
					   "Casting failed: invalid type in GsDtArray" );
		}

	 	const T& val = gsdt->data();

		retVec.push_back( val );
	}

	return retVec;
}

CC_END_NAMESPACE

#endif

