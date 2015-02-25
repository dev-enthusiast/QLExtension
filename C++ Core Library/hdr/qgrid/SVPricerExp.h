/****************************************************************
**
**	SVPricerExp.h	- SVPricer functions which are exported to slang 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.11 $
**
****************************************************************/

#if !defined( IN_SVPRICEREXP_H )
#define IN_SVPRICEREXP_H

#include <qgrid/base.h>
#include <qgrid/SVPricer.h>
#include <gscore/types.h>
#include <qgrid/enums.h>
#include <qgrid/Pricer.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QGRID SVPricer QGridSVPricerDefault(
	double	a	// dummy argument
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_CreateHeston(
	double 			Spot,					// initial spot
	double 			InitialVol,				// initial vol
	double 			LongVol,				// mean reversion level
	double 			beta,					// mean reversion rate
	double 			sigma,					// vol of vol
	double			rho,					// correlation
	const GsVector& ForwardGrowth,			// forward growth factors
	const GsVector& ForwardGrowthTimes,		// times of forward growth factors
	const GsVector& Discounts,				// discount factors
	const GsVector& DiscountTimes,			// times of discount factors
	const GsVector&	GridAnchoringVols,		// grid anchoring vols
	const GsVector& GridAnchoringVolTimes	// times of grid anchoring vols
);

EXPORT_CPP_QGRID double QGridSVPricer_Spot(
	const SVPricer& model	// the SVPricer
);

EXPORT_CPP_QGRID double QGridSVPricer_V(
	const SVPricer& model	// the SVPricer
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_SetSpot(
	const SVPricer& model,	// the SVPricer
	double			Spot	// value to set the spot to
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_SetV(
	const SVPricer& model,	// the SVPricer
	double			V   	// value to set the spot to
);

EXPORT_CPP_QGRID GsMatrix QGridSVPricer_GetDiscRates(
	const SVPricer&	model,		// the SVPricer
	double 			MinLogSpot,	// min of spot range
	double 			MaxLogSpot,	// max of spot range
	int 			nSpots,		// number of spot points
	double 			MinV,		// min of vol range
	double 			MaxV,		// max of vol range
	int 			nV,			// number of vol points
	double 			s,			// initial time
	double 			t			// final time
);

EXPORT_CPP_QGRID GsMatrix QGridSVPricer_GetDrift(
	const SVPricer&	model,		// the SVPricer
	double 			MinLogSpot,	// min of spot range
	double 			MaxLogSpot,	// max of spot range
	int 			nSpots,		// number of spot points
	double 			MinV,		// min of vol range
	double 			MaxV,		// max of vol range
	int 			nV,			// number of vol points
	double 			s,			// initial time
	double 			t			// final time
);

EXPORT_CPP_QGRID GsMatrix QGridSVPricer_GetVolFn(
	const SVPricer&	model,		// the SVPricer
	double 			MinLogSpot,	// min of spot range
	double 			MaxLogSpot,	// max of spot range
	int 			nSpots,		// number of spot points
	double 			MinV,		// min of vol range
	double 			MaxV,		// max of vol range
	int 			nV,			// number of vol points
	double 			s,			// initial time
	double 			t			// final time
);

EXPORT_CPP_QGRID GsMatrix QGridSVPricer_GetVolDrift(
	const SVPricer&	model,		// the SVPricer
	double 			MinLogSpot,	// min of spot range
	double 			MaxLogSpot,	// max of spot range
	int 			nSpots,		// number of spot points
	double 			MinV,		// min of vol range
	double 			MaxV,		// max of vol range
	int 			nV,			// number of vol points
	double 			s,			// initial time
	double 			t			// final time
);

EXPORT_CPP_QGRID GsMatrix QGridSVPricer_GetVolVol(
	const SVPricer&	model,		// the SVPricer
	double 			MinLogSpot,	// min of spot range
	double 			MaxLogSpot,	// max of spot range
	int 			nSpots,		// number of spot points
	double 			MinV,		// min of vol range
	double 			MaxV,		// max of vol range
	int 			nV,			// number of vol points
	double 			s,			// initial time
	double 			t			// final time
);

EXPORT_CPP_QGRID GsMatrix QGridSVPricer_GetSVCorrelations(
	const SVPricer&	model,		// the SVPricer
	double 			MinLogSpot,	// min of spot range
	double 			MaxLogSpot,	// max of spot range
	int 			nSpots,		// number of spot points
	double 			MinV,		// min of vol range
	double 			MaxV,		// max of vol range
	int 			nV,			// number of vol points
	double 			s,			// initial time
	double 			t			// final time
);

EXPORT_CPP_QGRID GsVector QGridSVPricer_SetPayout(
	const SVPricer& model,					// the SVPricer
	double 			MinLogSpot,				// the min of the spot values (in log space)
	double			MaxLogSpot,				// the max of the spot values (in log space)
	int				nSpots,					// the number of spots
	int				nVols,					// the number of vols
	const 			GsFuncPiecewiseLinear& f	// the payout
);

EXPORT_CPP_QGRID double QGridSVPricer_Price(
	const SVPricer& model,					// the SVPricer
	const GsVector& Values,					// the values on the grid
	double			MinLogSpot,             // the min of the spot values (in log space) 
	double			MaxLogSpot,				// the max of the spot values (in log space)
	int				nSpots,					// the number of spots
	double			MinV,					// the min of V
	double			MaxV,					// the max of V
	int				nVols					// the number of vols
);

EXPORT_CPP_QGRID GsVector QGridSVPricer_StepBack(
	const SVPricer& model,					// the SVPricer
	const GsVector& Values,					// values to be stepped back
	double			MinLogSpot,             // the min of the spot values (in log space) 
	double			MaxLogSpot,				// the max of the spot values (in log space)
	int				nSpots,					// the number of spots
	double			MinV,					// the min of V
	double			MaxV,					// the max of V
	int				nVols,					// the number of vols
	double			LowRebate,				// rebate at min spot
	double			HighRebate,				// rebate at max spot
	double			s,						// initial time
	double			t						// final time
);

EXPORT_CPP_QGRID GsVector QGridSVPricer_StepForwards(
	const SVPricer& model,					// the SVPricer
	const GsVector& Values,					// values to be stepped back
	double			MinLogSpot,             // the min of the spot values (in log space) 
	double			MaxLogSpot,				// the max of the spot values (in log space)
	int				nSpots,					// the number of spots
	double			MinV,					// the min of V
	double			MaxV,					// the max of V
	int				nVols,					// the number of vols
	double			s,						// initial time
	double			t						// final time
);

EXPORT_CPP_QGRID GsVector QGridSVPricer_InitialDensity(
	const SVPricer& model,					// the SVPricer
	double			MinLogSpot,             // the min of the spot values (in log space) 
	double			MaxLogSpot,				// the max of the spot values (in log space)
	int				nSpots,					// the number of spots
	double			MinV,					// the min of V
	double			MaxV,					// the max of V
	int				nVols					// the number of vols
);

EXPORT_CPP_QGRID double QGridSVPricer_Tolerance(
	const SVPricer&	grid	// the SVPricer
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_ToleranceSet(
	const SVPricer&	grid,		// the SVPricer
	double			Tolerance	// the tolerance
);

EXPORT_CPP_QGRID double QGridSVPricer_tAccuracy(
	const SVPricer&	grid	// the SVPricer
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_tAccuracySet(
	const SVPricer&	grid,		// the SVPricer
	double			tAccuracy	// the accuracy
);

EXPORT_CPP_QGRID int QGridSVPricer_SpotGridSize(
	const SVPricer& grid		// the SVPricer
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_SpotGridSizeSet(
	const SVPricer& grid,		// the SVPricer
	int				gridsize	// the grid size
);

EXPORT_CPP_QGRID int QGridSVPricer_VolGridSize(
	const SVPricer& grid		// the SVPricer
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_VolGridSizeSet(
	const SVPricer& grid,		// the SVPricer
	int				gridsize	// the grid size
);

EXPORT_CPP_QGRID GsString QGridSVPricer_Solver(
	const SVPricer& grid		// the SVPricer
);

EXPORT_CPP_QGRID SVPricer QGridSVPricer_SolverSet(
	const SVPricer& grid,		// the SVPricer
	const GsString&	SolverType	// type of solver
);

CC_END_NAMESPACE

#endif

