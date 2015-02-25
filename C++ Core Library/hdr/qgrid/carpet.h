/****************************************************************
**
**	qgrid/carpet.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/carpet.h,v 1.6 2000/04/20 19:37:54 lowthg Exp $
**
****************************************************************/

#if !defined( IN_QGRID_CARPET_H )
#define IN_QGRID_CARPET_H

#include	<qgrid/base.h>
#include	<gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )
                 
EXPORT_CPP_QGRID GsMatrix QGridCallsFromProbabilities(
	double					Spot,					// spot
	const GsVector&			Discounts,				// discount factors
	const GsVector&			ForwardGrowth,			// forward growth curve
	const GsVector&			Strikes,			    // strikes
	const GsMatrix&			Prices				    // enter the Arrow-Debreu prices, calls returned
);

EXPORT_CPP_QGRID void QGridCallsFromProbabilityVector(
	double				Forward,		// the forward price
	double				Discount,		// the discount factor
	const double*		Strikes,		// strikes
	const double*		Prices,			// the Arrow-Debreu prices
	double*				Calls, 			// the call prices to be returned
	int					nGrid			// number of points on the grid
);

EXPORT_CPP_QGRID GsMatrix QGridProbabilities(
	double					Spot,	   				// spot
	const GsVector&			Discounts,				// discount factors
	const GsVector&			ForwardGrowth,			// forward growth curve
	const GsVector&			Times,					// times
	const GsVector&			Strikes,				// strikes
	const GsMatrix&			Vols,					// instantaneous vols
	int						DiscretisationFactor	// number of steps per Time point
);

EXPORT_CPP_QGRID GsMatrix QGridLevyProbabilities(
	double					Spot,	   				// spot
	const GsVector&			Times,					// the maturity times
	const GsVector&			Discounts,				// discount factors at the expiry times
	const GsVector&			ForwardGrowth,			// forward growth factors at the expiry times
	double					MinGrid,				// minimum grid point
	double					MaxGrid,				// maximum grid point
	const GsVector&			JumpRate				// the jump distribution
);

EXPORT_CPP_QGRID GsMatrix QGridCallsToImplicitDVModel(
	double					Spot,	   				// spot
	const GsVector&			Discounts,				// discount factors
	const GsVector&			ForwardGrowth,			// forward growth curve
	const GsVector&			Times,					// times
	const GsVector&			Strikes,				// number of strikes
	const GsMatrix&			Calls					// call prices
);

CC_END_NAMESPACE
#endif

