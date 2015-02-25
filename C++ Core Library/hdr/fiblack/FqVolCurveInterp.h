/****************************************************************
**
**	FQVOLCURVEINTERP.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/FqVolCurveInterp.h,v 1.8 2001/02/26 23:16:29 xuyi Exp $
**
****************************************************************/

#if !defined( IN_FQVOLCURVEINTERP_H )
#define IN_FQVOLCURVEINTERP_H

#include <vector>
#include <ccstl.h>
#include <fiblack/base.h>

#include <gscore/GsVolatilityCurve.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsDoubleVector.h>
#include <gsdt/GsDtArray.h>

#include <fiblack/FqVolCurveConstraint.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_FIBLACK 
GsVolatilityCurve FqBsVolCurveCreateFromConstraintCFR(
	const GsDtArray&,
	const GsDate&
	);

EXPORT_CPP_FIBLACK 
GsVolatilityCurve FqBsVolCurveCreateFromConstraintSmooth(
	const GsDtArray&,
	const GsDate&
	);

EXPORT_CPP_FIBLACK 
GsVolatilityCurve FqBsVolCurveCreateFromConstraintCFR(
	const CC_STL_VECTOR( FqVolCurveConstraint )&,
	const GsDate&
	);

EXPORT_CPP_FIBLACK 
GsVolatilityCurve FqBsVolCurveCreateFromConstraintSmooth(
	const CC_STL_VECTOR( FqVolCurveConstraint )&,
	const GsDate&
	);


EXPORT_CPP_FIBLACK 
GsVolatilityCurve FqBsVolCurveCreateFromSamples(
	const GsDateVector&, 
	const GsDoubleVector&
	);

EXPORT_CPP_FIBLACK 
double FqBsVolLookupByTerm(
	const GsVolatilityCurve&, 
	double
	);

EXPORT_CPP_FIBLACK 
double FqBsVolLookupByDate(
	const GsVolatilityCurve&, 
	const GsDate&
	);

EXPORT_CPP_FIBLACK 
double FqBsVolCurveConstraintApply(
	const FqVolCurveConstraint&,
	const GsVolatilityCurve&
	);

EXPORT_CPP_FIBLACK 
double FqBsVolFromCurve(
	const GsVolatilityCurve& VolCurve, 	// curve
	const GsDate& dt   					// date
);

EXPORT_CPP_FIBLACK
double FqBsTermVolFromCurve(
	const GsVolatilityCurve& VolCurve, 	
	const GsDate& dt   					
);

EXPORT_CPP_FIBLACK 
GsVolatilityCurve GsVolatilityCurveFromGsFunc(
	const GsFuncScFunc &Func,
	GsDate StartDate		
);

EXPORT_CPP_FIBLACK 
GsFuncScFunc GsVolatilityCurveToGsFunc(
	const GsVolatilityCurve &VolCurve
);

EXPORT_CPP_FIBLACK 
GsVector FqBsVolVectorFromCurve(
	const GsVolatilityCurve& VolCurve,
	const GsDateVector& Dates  	
);

EXPORT_CPP_FIBLACK
GsVector FqBsTermVolVectorFromCurve(
	const GsVolatilityCurve& VolCurve, 	
	const GsDateVector& Dates 			
);

EXPORT_CPP_FIBLACK
double FqBsInterpTermVolFromCurves(
	const GsVolatilityCurve& VolCurveAbove,
	const GsVolatilityCurve& VolCurveBelow,
	const GsRDate& tenorAbove,				
	const GsRDate& tenorBelow,				
	const GsRDate& requiredTenor,			
	const GsDate& dt   						
);

EXPORT_CPP_FIBLACK
GsVector FqBsInterpTermVolVectorFromCurves(
	const GsVolatilityCurve& VolCurveAbove,
	const GsVolatilityCurve& VolCurveBelow,
	const GsRDate& tenorAbove,				
	const GsRDate& tenorBelow,				
	const GsRDate& requiredTenor,			
	const GsDateVector& dt 					
);

CC_END_NAMESPACE

#endif 




