/****************************************************************
**
**	GSFUNCMISCTEST.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncMiscTest.h,v 1.8 2011/08/16 14:20:21 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCMISCTEST_H )
#define IN_GSQUANT_GSFUNCMISCTEST_H

#include <gsquant/base.h>
#include <gsquant/GsFuncSpecial.h>
#include <gsquant/GsFuncPiecewisePolyIntegral.h>

#include <gscore/GsFuncHandle.h>
#include <gscore/GsVector.h>

#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSQUANT GsFuncTest : 
	public GsFunc< const GsVector&, GsVector >
{
public:
	GsFuncTest( size_t dim ) : m_dim( dim ) {}
	~GsFuncTest() {}

	virtual GsVector operator()( const GsVector& x ) const;

private:
	size_t m_dim;
};

EXPORT_CPP_GSQUANT 
GsFuncVecToVecFunc* GsFuncTestMake( unsigned dim );

EXPORT_CPP_GSQUANT double GsFuncTestPP( const GsVector& knots,
										const GsVector& coefficients,
										double x );
EXPORT_CPP_GSQUANT double GsFuncTestPPI( const GsVector& knots,
										 const GsVector& coefficients,
										 double x );
EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncTestGenPPI( const GsVector& knots, 
							   const GsVector& coefficients );
EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncTestGenPPICurve( const GsVector& knots, 
									const GsVector& coefficients );

EXPORT_CPP_GSQUANT 
double GsFuncScFuncValue( const GsFuncScFunc& h,
						  double x );
EXPORT_CPP_GSQUANT 
double GsFuncScFunctionalValue( const GsFuncScFunctional& h,
								const GsFuncScFunc& x );
EXPORT_CPP_GSQUANT 
double GsFuncVecToScFuncValue( const GsFuncVecToScFunc& h,
							   const GsVector& x );
EXPORT_CPP_GSQUANT 
GsVector GsFuncVecToVecFuncValue( const GsFuncVecToVecFunc& h,
								  const GsVector& x );

EXPORT_CPP_GSQUANT 
GsFuncScFunc GsFuncScFuncCreatePiecewisePoly( const GsVector& knots, const GsMatrix& coeffs);

CC_END_NAMESPACE

#endif 


