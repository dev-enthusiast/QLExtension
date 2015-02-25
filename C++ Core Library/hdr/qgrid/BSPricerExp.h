/****************************************************************
**
**	BSPricerExp.h	- BSPricer functions expoted to slang
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.22 $
**
****************************************************************/

#if !defined( IN_BSPRICEREXP_H )
#define IN_BSPRICEREXP_H

#include <qgrid/base.h>
#include <qgrid/BSPricer.h>
#include <gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID BSPricer QGridBSPricer(
	double			Spot,					// spot
	const GsVector&	Discounts,				// discount factors
	const GsVector& DiscountTimes,			// discount times
	const GsVector& ForwardGrowth,			// forward growth factors
	const GsVector& ForwardTimes,			// forward growth times
	const GsVector& Volatilities,			// implied volatilities
	const GsVector& VolatilityTimes			// volatility times
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer2(
	double			Spot,					// spot
	const GsVector&	Discounts,				// discount factors
	const GsVector& DiscountTimes,			// discount times
	const GsVector& ForwardGrowth,			// forward growth factors
	const GsVector& ForwardTimes,			// forward growth times
	const GsVector& Volatilities,			// implied volatilities
	const GsVector& VolatilityTimes,		// volatility times
	const GsVector&	GridAnchoringVols,		// grid anchoring vols
	const GsVector&	GridAnchoringVolTimes	// times of the grid anchoring vols
);

EXPORT_CPP_QGRID GsFuncPiecewiseLinear QGridBSPricer_LogDiscounts(
	const BSPricer&		model		// the BSPricer
);

EXPORT_CPP_QGRID GsFuncPiecewiseLinear QGridBSPricer_LogForwardGrowth(
	const BSPricer&		model		// the BSPricer
);

EXPORT_CPP_QGRID GsFuncPiecewiseLinear QGridBSPricer_Variances(
	const BSPricer&		model		// the BSPricer
);

EXPORT_CPP_QGRID GsString QGridBSPricer_Description(
	const	BSPricer&	model	// the BS Pricer
);

EXPORT_CPP_QGRID GsVector QGridBSPricer_StepBack(
	const BSPricer&	model,			// the BS Pricer
	const GsVector& Values,			// the grid
	double			MinGrid,		// minimum value of spot on the grid
	double			MaxGrid,		// maximum value of spot on the grid
	double			s,              // the initial time                   
	double			t,              // the final time                     
	double			LowBarrier,     // the low knockout                   
	double			HighBarrier,    // the high knockout                  
	double			LowRebate,      // the low rebate                     
	double			HighRebate,     // the high rebate                    
	double			RebateTime      // the time when the rebates are paid 
);

EXPORT_CPP_QGRID double QGridBSPricer_tAccuracy(
	const BSPricer&	model		// the BS Pricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_tAccuracySet(
	const BSPricer&	model,		// the BS Pricer
	double			tAccuracy	// the tAccuracy
);

EXPORT_CPP_QGRID int QGridBSPricer_GridSize(
	const BSPricer&	model		// the BS Pricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_GridSizeSet(
	const BSPricer&	model,		// the BS Pricer
	int				GridSize	// the grid size
);

EXPORT_CPP_QGRID double QGridBSPricer_Spot(
	const BSPricer&	model	// the BS Pricer
);

EXPORT_CPP_QGRID double QGridBSPricer_Range(
	const BSPricer&	model,	// the BS Pricer
	double Time				// time
);

EXPORT_CPP_QGRID double QGridBSPricer_InitialDiscount(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID double QGridBSPricer_InitialForward(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_UseLinearTimeSteps(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_UseSqrtTimeSteps(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_UseLinearNumTimeSteps(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_UseSqrtNumTimeSteps(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID double QGridBSPricer_GridResizeRate(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_GridResizeRateSet(
	const BSPricer&	model,	// the BSPricer
	double			rate	// grid resize rate ( > 0. )
);

EXPORT_CPP_QGRID int QGridBSPricer_MinSteps(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_MinStepsSet(
	const BSPricer&	model,	// the BSPricer
	int				nSteps	// min number of steps
);

EXPORT_CPP_QGRID int QGridBSPricer_MinInitialSteps(
	const BSPricer&	model	// the BSPricer
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_MinInitialStepsSet(
	const BSPricer&	model,	// the BSPricer
	int				nSteps	// min number of steps
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_GridSizeParamsSet(
	const BSPricer&	model,	// the BSPricer
	double MinRange,		// min of min grid range
	double MaxRange,		// max of min grid range
	double nStdDevs			// no of std devs to go outside min range
);

EXPORT_CPP_QGRID BSPricer QGridBSPricer_ReturnTypeSet(
	const BSPricer&	model,		// the BSPricer
	const GsString& ReturnType  // price, grid or function
);

EXPORT_CPP_QGRID GsString QGridBSPricer_ReturnType(
	const BSPricer&	model		// the BSPricer
);

CC_END_NAMESPACE

#endif

