/****************************************************************
**
**	GSFUNCPIECEWISEPOLYINTEGRAL.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncPiecewisePolyIntegral.h,v 1.4 2000/06/14 13:22:51 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCPIECEWISEPOLYINTEGRAL_H )
#define IN_GSQUANT_GSFUNCPIECEWISEPOLYINTEGRAL_H

#include <gsquant/base.h>

#include <gsquant/GsFuncPiecewisePoly.h>

#include <gscore/GsFuncHandle.h>

#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

// ....	FIX: replace this with B splines later ....
class EXPORT_CLASS_GSQUANT GsFuncPiecewisePolyIntegral : 
	public GsFuncPiecewisePoly
{
public:
	GsFuncPiecewisePolyIntegral( const GsVector& knots,
								 const GsVector& coefficients,
								 double initialValue = 0.,
								 bool isRightClosed = FALSE );
	GsFuncPiecewisePolyIntegral( const GsFuncPiecewisePoly& p,
								 double initialValue = 0. );
	virtual ~GsFuncPiecewisePolyIntegral() {}

protected:
	void constructorHelper( double initialValue );
};

template< class BasisFunc >
class GsFuncIntegrator : 
	public GsFunc< BasisFunc, GsFuncHandle< double, double > >
{
public:
	GsFuncIntegrator( double initialValue ): m_initialValue( initialValue ) {}
	virtual ~GsFuncIntegrator() {}

	virtual GsFuncHandle< double, double > operator() ( BasisFunc p ) const
	{
		return GsFuncHandle< double, double >( 
			new BasisFunc( p.integral( m_initialValue ) ) );
	}
				
protected:
	double m_initialValue;
};

// .... Addins ....
EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncPiecewisePolyIntegralCreate(	const GsVector& knots,
												const GsVector& coefficients,
												double initialValue );
CC_END_NAMESPACE

#endif 





