/****************************************************************
**
**	GSFUNCMATH.H	- various math functions
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncMath.h,v 1.4 2000/09/08 11:05:39 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCMATH_H )
#define IN_GSFUNC_GSFUNCMATH_H

#include <gscore/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsStreamMap.h>
#include <math.h>


CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsFuncExp 
** Description : exp function
****************************************************************/

class GsFuncExp : public GsFunc< double, double >
{
public:
	GsFuncExp() {}
	virtual ~GsFuncExp() {}
	
	virtual double operator()(double x ) const { return exp( x ) ; }

	typedef GsStreamMapT< GsFuncExp > GsStType;

	virtual GsStreamMap* buildStreamMapper() const
	{
		static GsStreamMap* MapCloner = GsStreamMap::find( "GsFuncExp" );

		if( !MapCloner )
			GSX_THROW( GsXInvalidOperation, "GsFuncExp::buildStreamMapper - No GsStreamMap" );

		return MapCloner->clone( this );
	}


private:
	GsFuncExp &operator=( const GsFuncExp &rhs );		
	GsFuncExp( const GsFuncExp & );
};



/****************************************************************
** Class	   : GsFuncMax
** Description : max function
****************************************************************/

template< class T >
class GsFuncMax : public GsFunc2< T, T, T >
{
public:
	GsFuncMax() {}
	virtual ~GsFuncMax() {}
	
	virtual T operator()( T x, T y ) const { return ( x < y ) ? y : x; }

private:
	GsFuncMax &operator=( const GsFuncMax &rhs );		
	GsFuncMax( const GsFuncMax & );
};



/****************************************************************
** Class	   : GsFuncMin
** Description : Min function
****************************************************************/

template< class T >
class GsFuncMin : public GsFunc2< T, T, T >
{
public:
	GsFuncMin() {}
	virtual ~GsFuncMin() {}
	
	virtual T operator()( T x, T y ) const { return ( x < y ) ? x : y; }

private:
	GsFuncMin &operator=( const GsFuncMin &rhs );		
	GsFuncMin( const GsFuncMin & );
};



/****************************************************************
** Class	   : GsFuncBinaryBinaryMax
** Description : Binary Max function ( x - y < 0 ) ? 1 : 0
****************************************************************/

template< class T >
class GsFuncBinaryMax : public GsFunc2< T, T, T >
{
public:
	GsFuncBinaryMax() {}
	virtual ~GsFuncBinaryMax() {}
	
	virtual T operator()( T x, T y ) const { return ( x < y ) ? 1 : 0; }

private:
	GsFuncBinaryMax &operator=( const GsFuncBinaryMax &rhs );		
	GsFuncBinaryMax( const GsFuncBinaryMax & );
};



/****************************************************************
** Class	   : GsFuncBinaryMax
** Description : Binary min function ( x - y > 0 ) ? 1 : 0
****************************************************************/

template< class T >
class GsFuncBinaryMin : public GsFunc2< T, T, T >
{
public:
	GsFuncBinaryMin() {}
	virtual ~GsFuncBinaryMin() {}
	
	virtual T operator()( T x, T y ) const { return ( x < y ) ? 0 : 1; }

private:
	GsFuncBinaryMin &operator=( const GsFuncBinaryMin &rhs );		
	GsFuncBinaryMin( const GsFuncBinaryMin & );
};



CC_END_NAMESPACE

#endif 
