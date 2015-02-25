/****************************************************************
**
**	qgrid/PiecewiseLinearFn.h	- Definition of class PiecewiseLinearFn
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: PiecewiseLinearFn.h,v $
**	Revision 1.25  2001/08/22 15:21:16  lowthg
**	changed all uses of PiecewiseLinearFn to GsFuncPiecewiseLinear
**
**	Revision 1.24  2001/08/16 13:40:39  lowthg
**	removed PiecewiseLinearFn and #defined it
**	to be the same as GsFuncPiecewiseLinear
**	
**	Revision 1.23  2001/08/08 18:27:55  lowthg
**	removed some PiecewiseLinearFn memberfns
**	
**	Revision 1.22  2001/03/30 20:06:00  lowthg
**	added SmoothLogSpace
**	
**	Revision 1.21  2001/03/05 20:57:17  lowthg
**	used GsFuncPiecewiseLinear::vector rather than CC_STL_VECTOR( double )
**	
**	Revision 1.20  2001/01/26 17:38:41  brownb
**	iterators are not pointers
**	
**	Revision 1.19  2000/12/11 20:07:25  lowthg
**	speedups for multiplication by piecewise linear functions in BSPricer
**	
**	Revision 1.18  2000/11/30 16:19:11  lowthg
**	new functions Linear() and Constant()
**	
**	Revision 1.17  2000/11/23 13:36:53  lowthg
**	new constructor taking STL vectors
**	
**	Revision 1.16  2000/11/16 17:56:13  lowthg
**	BSPricerFourier now chooses slightly more sensible grid size
**	
**	Revision 1.15  2000/11/03 14:40:20  lowthg
**	fixed compilation probs
**	
**	Revision 1.14  2000/11/03 14:01:53  lowthg
**	inherited PiecewiseLinearFn from GsFuncPiecewiseLinear in gsquant
**	
**	Revision 1.13  2000/11/03 11:48:42  lowthg
**	changed PiecewiseLinearFn to use CC_STL_VECTOR( double ) and removed
**	SetXVals, SetYVals, SetnVals member functions
**	
**	Revision 1.12  2000/10/30 17:37:42  lowthg
**	fixed bug in constructor
**	
**	Revision 1.11  2000/07/19 14:12:02  lowthg
**	Added new VectorMaxProj function and a Project member function to PiecewiseLinearFn
**	
**	Revision 1.10  2000/07/18 13:44:34  lowthg
**	Corrected bug with SetYValues
**	
**	Revision 1.9  2000/07/18 12:53:11  lowthg
**	Added a VectorMaxSmooth function
**	
**	Revision 1.8  2000/07/12 21:15:25  lowthg
**	Modified BSPricer to deal with grid resizing, and added extra member functions
**	to PiecewiseLinearFn
**	
**	Revision 1.7  2000/06/29 12:56:38  lowthg
**	Changed the way that BSPricer & PiecewiseLinearFn treats forward growth
**	and discount factor curves
**	
**	Revision 1.6  2000/05/25 17:48:25  lowthg
**	Added checks for increasing variance curve and positive growth/discount curves
**	
**	Revision 1.5  2000/05/19 20:26:44  lowthg
**	extra constructor added to create variance curves, discount curves, etc
**	
**	Revision 1.4  2000/02/01 19:02:35  lowthg
**	extensions to PiecewiseLinearFn
**	
**	Revision 1.3  2000/01/25 18:06:14  lowthg
**	Added more functionality to PiecewiseLinearFn
**	
**	Revision 1.2  2000/01/25 15:36:38  lowthg
**	Exported PiecewiseLinearFn
**	
**	Revision 1.1  2000/01/24 22:05:34  lowthg
**	Added PiecewiseLinearFn, KnockoutInfo (to replace KnockoutData) and
**	FXGrid (to replace FXGridSolver).
**	
**
****************************************************************/

#if !defined( IN_Q_GRID_PIECEWISELINEARFN_H )
#define IN_Q_GRID_PIECEWISELINEARFN_H

#include <qgrid/base.h>
#include <gscore/types.h>
#include <qgrid/enums.h>
#include <gsdt/GsDtGeneric.h>
#include <gsquant/GsFuncPiecewiseLinear.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
**	Routine: GsFuncPiecewiseLinearFromCurve
**	Returns: 
**	Action : creates a piecewise linear function from the inputted
**		curve.
**
**	if Type = QGRID_DISCOUNT_CURVE, then the GsFuncPiecewiseLinear
**		is minus the log of the curve.
**	if Type = QGRID_GROWTH_CURVE, then the GsFuncPiecewiseLinear
**		is the log of the curve
**	if Type = QGRID_VOLATILITY_CURVE, then the GsFuncPiecewiseLinear
**	is the variance curve (variance = vol * vol * time).
**
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearFromCurve(
	GsFuncPiecewiseLinear&	f,
	const double* 		Values,
	const double* 		Times,
	int 				nTimes,
	QGRID_CURVE_TYPE 	Type
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearSmooth // Smooth
**	Returns: void
**	Action : Interpolates the function at the given x values, using
**		the linear basis function method
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearSmooth(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	const double* x,				// x values to interpolate at
	double*	y,						// outputted y values
	int nValues						// number of values
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearSmooth // Smooth
**	Returns: GsVector
**	Action : Interpolates the function at the given x values, using
**		the linear basis function method
****************************************************************/

EXPORT_CPP_QGRID GsVector GsFuncPiecewiseLinearSmooth(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	const GsVector&	x  				// x values to interpolate at
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearAltSmooth // AltSmooth
**	Returns: void
**	Action : Interpolates the function at the given x values, using
**		the constant basis function method
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearAltSmooth(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	const double* x,				// x values to interpolate at
	double*	y,						// outputted y values
	int nValues						// number of values
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearAltSmooth // AltSmooth
**	Returns: GsVector
**	Action : Interpolates the function at the given x values, using
**		the constant basis function method
****************************************************************/

EXPORT_CPP_QGRID GsVector GsFuncPiecewiseLinearAltSmooth(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	const GsVector&	x  				// x values to interpolate at
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearProject // Project
**	Returns: GsVector
**	Action : Interpolates the function at the given x values, by
**		orthogonally projecting onto piecewise linear functions
**		with nodes at given x values
****************************************************************/

EXPORT_CPP_QGRID GsVector GsFuncPiecewiseLinearProject(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	const GsVector& x				// x values to interpolate
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearSmoothMultiply // Multiply
**	Returns: void
**	Action : Interpolates the function at the given x values, using
**		the linear basis function method, and multiplied my these
**		values
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearSmoothMultiply(
    const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	const double* GridLevels,		// x values at which to multiply
	double* GridValues,				// values to be multiplied
	int nGrid						// number of values to multiply
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearSmoothMultiply // Multiply
**	Returns: void
**	Action : Interpolates the function at the given x values, using
**		the linear basis function method, and multiplied my these
**		values
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearSmoothMultiply(
    const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	const GsVector& GridLevels,		// x values at which to multiply
	GsVector& GridValues			// values to be multiplied
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearSmoothLogSpace // SmoothLogSpace
**	Returns: void
**	Action : Interpolates the function at the given x values, using
**		the linear basis function method. The log of the x values
**		are linearly spaced between Minx and Maxx
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearSmoothLogSpace(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	double Minx,					// log of min of x values
	double Maxx,					// log of max of x values
	int nValues,					// number of values
	double* yVals					// outputted y values
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearSmoothMultiplyLog // SmoothMultiplyLogSpace
**	Returns: void
**	Action : Multiplies the y values by the interpolated values.
**		The interpolated values are calculated using the linear
**		basis function method. The log of the x values are
**		linearly spaced from Minx to Maxx
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearSmoothMultiplyLog(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	double Minx,					// log of min of x values
	double Maxx,					// log of max of x values
	int nValues,					// number of values
	double* yVals					// outputted y values
);

/****************************************************************
**	Routine: GsFuncPiecewiseLinearSmoothMultiplyLog2 // SmoothMultiplyLogSpace2
**	Returns: void
**	Action : Multiplies the y values by the interpolated values.
**		The interpolated values are calculated using the linear
**		basis function method. The x values are on a geometric
**		sequence.
****************************************************************/

EXPORT_CPP_QGRID void GsFuncPiecewiseLinearSmoothMultiplyLog2(
	const GsFuncPiecewiseLinear& f, // the piecewise linear fn
	double ExpMinx,					// min of x values
	double Expdx,					// ratio of successive x values
	int nValues,					// number of values
	double* yVals					// outputted y values
);

CC_END_NAMESPACE

#endif

