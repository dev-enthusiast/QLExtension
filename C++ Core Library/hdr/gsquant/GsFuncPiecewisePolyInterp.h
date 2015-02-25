/****************************************************************
**
**	GSFUNCPIECEWISEPOLYINTERP.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncPiecewisePolyInterp.h,v 1.5 2000/06/16 20:55:44 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCPIECEWISEPOLYINTERP_H )
#define IN_GSQUANT_GSFUNCPIECEWISEPOLYINTERP_H

#include <gsquant/base.h>

#include <gsquant/GsFuncPiecewisePoly.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSQUANT GsFuncPiecewisePolyInterp : 
	public GsFuncPiecewisePoly
{
public:
	GsFuncPiecewisePolyInterp( const GsVector& knots,
							   const GsVector& values,
							   const GsVector& coefficients,
							   bool  isRightClosed = FALSE );
	virtual ~GsFuncPiecewisePolyInterp() {}

	virtual GsFuncPiecewisePolyInterp integral( double initialValue ) const;

protected:
	GsFuncPiecewisePolyInterp( const GsFuncPiecewisePoly& p );
	void computeCoefficients( const GsVector& coefficients,
							  const GsVector& values );

	inline static size_t coefficientsSizeIncrement( size_t knotsSize );
};

// FIX: terrible kludge. Will go away with new handle design ....
class EXPORT_CLASS_GSQUANT GsFuncPiecewisePolyInterpGen :
	public GsFunc< const GsVector&, GsFuncPiecewisePoly >
{
public:
	GsFuncPiecewisePolyInterpGen( const GsVector& knots,
								  const GsVector& values );
	virtual ~GsFuncPiecewisePolyInterpGen() {}

	virtual GsFuncPiecewisePoly operator()( const GsVector& c ) const
	{
		return GsFuncPiecewisePolyInterp( m_knots, m_values, c );
	}

protected:
	GsVector m_knots;
	GsVector m_values;
};


// .... Addins ....
EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncPiecewisePolyInterpCreate( const GsVector& knots, 
											  const GsVector& values,
											  const GsVector& coeffs );
// .... Inlines ....
size_t
GsFuncPiecewisePolyInterp::coefficientsSizeIncrement( size_t knotsSize ) 
{
	if( knotsSize < 2 )

		GSX_THROW( GsXInvalidArgument, 
				   "knots vector needs to contains a single node" );

	return 2 * ( knotsSize - 1 );
}

CC_END_NAMESPACE

#endif 



