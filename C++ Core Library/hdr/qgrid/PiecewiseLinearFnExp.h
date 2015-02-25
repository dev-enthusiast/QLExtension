/****************************************************************
**
**	qgrid/DVPricerExp.h	- Declarations of DVPricer routines exported to Slang
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/PiecewiseLinearFnExp.h,v 1.7 2001/08/22 15:21:16 lowthg Exp $
**
****************************************************************/

#if !defined( IN_QGRID_PIECEWISELINEARFNEXP_H )
#define IN_QGRID_PIECEWISELINEARFNEXP_H

#include <qgrid/base.h>
#include <qgrid/DVPricer.h>
#include <gscore/types.h>
#include <qgrid/enums.h>
#include <gsquant/GsFuncPiecewiseLinear.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID GsFuncPiecewiseLinear QGridCreatePiecewiseLinearFn(
	double 			LowGradient,	// the gradient to the left of XValues
	double 			HighGradient,	// the gradient to the right of XValues
	const GsVector&	XValues,		// the set of x values at which the function is non-linear (must be non-decreasing)
	const GsVector&	YValues			// the values of the function at XValues
);

EXPORT_CPP_QGRID GsFuncPiecewiseLinear QGridCreatePiecewiseLinearOptionPayout(
	Q_OPT_TYPE	OptionType,			// the option style (Q_OPT_PUT, Q_OPT_CALL, Q_OPT_FWD)
	double		Strike				// the strike
);

EXPORT_CPP_QGRID double QGridPiecewiseLinearFn(
	const GsFuncPiecewiseLinear& f,	// the GsFuncPiecewiseLinear
	double x					// the x value
);

EXPORT_CPP_QGRID GsVector QGridPiecewiseLinearFnValues(
	const GsFuncPiecewiseLinear&	f,	// the GsFuncPiecewiseLinear
	const GsVector&				x	// the x value
);

EXPORT_CPP_QGRID GsVector QGridPiecewiseLinearFnSmooth(
	const GsFuncPiecewiseLinear&	f,	// the GsFuncPiecewiseLinear
	const GsVector&				x	// the x value
);

EXPORT_CPP_QGRID GsVector QGridPiecewiseLinearFnAltSmooth(
	const GsFuncPiecewiseLinear&	f,	// the GsFuncPiecewiseLinear
	const GsVector&				x	// the x value
);

EXPORT_CPP_QGRID GsVector QGridPiecewiseLinearFnProject(
	const GsFuncPiecewiseLinear&	f,	// the GsFuncPiecewiseLinear
	const GsVector&				x	// the x value
);

EXPORT_CPP_QGRID GsFuncPiecewiseLinear QGridPiecewiseLinearFnAddDbl(
	const GsFuncPiecewiseLinear&	f,	// the piecewise linear fn
	double						a	// double to add to f
);

EXPORT_CPP_QGRID GsFuncPiecewiseLinear QGridPiecewiseLinearFnMultiplyDbl(
	const GsFuncPiecewiseLinear&	f,	// the piecewise linear fn
	double						a	// double to multiply f by
);

CC_END_NAMESPACE
#endif

