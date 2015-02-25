/* $Header: /home/cvs/src/num/src/n_vctrin.h,v 1.12 2001/11/27 23:02:39 procmon Exp $ */
/****************************************************************
**
**	N_VCTRIN.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_N_VCTRIN_H )
#define IN_N_VCTRIN_H

#if !defined( IN_DATE_H )
#include <date.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

typedef enum
{
	Q_REF_CURVE_INTERP_LOG_LINEAR,
	Q_REF_CURVE_INTERP_LINEAR

} Q_REF_CURVE_INTERP_METHOD;

EXPORT_C_NUM double N_VectorInterp(
    const Q_VECTOR    *xVector,	   // vector of x values
	const Q_VECTOR    *yVector,	   // vector of y values
    double	 xValue			   // arbitrary x value
);

EXPORT_C_NUM double N_VectorInterp2(
    const Q_VECTOR    *xVector,	   // vector of x values
	const Q_VECTOR    *yVector,	   // vector of y values
    double	 xValue			   // arbitrary x value
);

EXPORT_C_NUM double N_CubeInterp(
    const Q_VECTOR    *xVector,	   // vector of x values
    const Q_VECTOR    *yVector,	   // vector of y values
    double	 xValue			   // arbitrary x value
);

EXPORT_C_NUM double N_CubeInterpNotSmooth(
    const Q_VECTOR    *xVector,	   	// vector of x values
    const Q_VECTOR    *yVector,	   	// vector of y values
    double	    x 			   		// arbitrary x value
);

EXPORT_C_NUM int N_CubeInterpVector( 	
    const Q_VECTOR	*xVector,	   	// vector of x values
    const Q_VECTOR	*yVector,	   	// vector of y values
	const Q_VECTOR	*xInterp,		// vector of x values to be interpolated
	double			*yInterp,		// y array to return the interpolated values
	double			yp1,			// gradient at left of grid
	double			ypn,			// gradient at right of grid
	double			*Temp1,			// temp storage vector (size = size( Temp1 ))
	double			*Temp2			// temp storage vector (size = size( Temp2 ))
);
EXPORT_C_NUM int N_CubeInterpEquiSpacedVector( 	
    double			MinX,	   		// minimum x value
	double			MaxX,			// maximum x value
    const Q_VECTOR	*yVector,	   	// vector of y values
	const Q_VECTOR	*xInterp,		// vector of x values to be interpolated
	double			*yInterp,		// y array to return the interpolated values
	double			yp1,			// gradient at left of grid
	double			ypn,			// gradient at right of grid
	double			*Temp1,			// temp storage vector (size = size( Temp1 ))
	double			*Temp2			// temp storage vector (size = size( Temp2 ))
);
  
EXPORT_C_NUM double N_GeoInterp(
	const Q_VECTOR    *xVector,	   // vector of x values
	const Q_VECTOR    *yVector,	   // vector of y values
    double	 xValue			   // arbitrary x value
);

EXPORT_C_NUM double N_InterpVolCurve(
	DT_CURVE		*VolCurve,			// Volatility Curve
	DATE			BaseDate,			// Pricing Date
	DATE			InterpDate			// Interpolating Date
);

EXPORT_C_NUM double N_RefCurveInterp(				
	DT_CURVE 		*RefCurve, 			// Reference Curve or discount curve
	DATE 			InterpDate 			// Interpolating Date
);

EXPORT_C_NUM double N_InterestRate(
	DT_CURVE	*RefCurve,
	DATE		From,
	DATE		To
);

EXPORT_C_NUM DT_CURVE *N_VectorsToCurve(
	const Q_VECTOR		*Dates,				//	Given
	const Q_VECTOR		*Values				//	Given
);

EXPORT_C_NUM int N_GetMaxIndex(
	double	*V,
	int		Size
);

EXPORT_C_NUM int N_VectorInterpolate(
	Q_VECTOR	*Vector,
	Q_VECTOR	*VectorTimes,
	double		*TimePoints,
	double		*InterpolatedY,
	int			NumPoints
);

EXPORT_C_NUM double N_BiCubeInterp ( 
	Q_VECTOR         *xVector,      // points along the x axis
	Q_VECTOR         *yVector,      // points along the y axis
	Q_MATRIX         *Values,       // Values in the grid
	double           x,             // particular value on the x axis
	double           y              // particular value on the x axis
	);

#endif

