/****************************************************************
**
**	n_pwlfun.h	- routines for piecewise linear functions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_N_PWLFUN_H )
#define IN_N_PWLFUN_H

#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

typedef struct
{
	double	lowgradient;
	double	highgradient;
	double*	XVals;
	double* YVals;
	int		nVals;
} PiecewiseLinearFn;

EXPORT_C_NUM int N_MultiplyByPiecewiseLinear(
	double*						gridvalues,	// pointer to the grid points to be multiplied
	const double*				gridlevels,	// pointer to the spot levels	
	int							nGrid,		// number of grid points
	const PiecewiseLinearFn*	fn			// pointer to the function to multiply by
);

EXPORT_C_NUM double N_ConstantBasisInterpolate(
	double						x0,			// the left of the basis function
	double						x1,			// the value at which we want to interpolate
	double						x2,			// the right of the basis function
	const PiecewiseLinearFn*	fn,			// the piecewise linear function
	int							LeftKnot,	// the knot of fn just to the left of midpoint of x0 and x1
	int*						NewLeftKnot	// returns the knot of fn just to the left of midpoint of x1 and x2
);

EXPORT_C_NUM double N_LinearBasisInterpolate(
	double						x0,			// the left of the basis function
	double						x1,			// the value at which we want to interpolate
	double						x2,			// the right of the basis function
	const PiecewiseLinearFn*	fn,			// the piecewise linear function
	int							LeftKnot,	// a knot of fn to the left of x0 and x1
	int*						NewLeftKnot	// returns the knot of fn just to the left of x1
);

#endif

