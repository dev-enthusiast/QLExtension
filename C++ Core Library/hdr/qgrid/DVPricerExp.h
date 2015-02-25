/****************************************************************
**
**	qgrid/DVPricerExp.h	- Declarations of DVPricer routines exported to Slang
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/DVPricerExp.h,v 1.18 2000/09/22 13:58:22 lowthg Exp $
**
****************************************************************/

#if !defined( IN_QGRID_DVPRICEREXP_H )
#define IN_QGRID_DVPRICEREXP_H

#include <qgrid/base.h>
#include <qgrid/DVPricer.h>
#include <gscore/types.h>
#include <qgrid/enums.h>
#include <qgrid/Pricer.h>

CC_BEGIN_NAMESPACE( Gs )
                 
EXPORT_CPP_QGRID DVPricer QGridDVPricer(
	double					Spot,	   				// spot
	const GsVector&			Discounts,				// discount factors
	const GsVector&			ForwardGrowth,			// forward growth curve
	const GsVector&			Times,					// times
	const GsVector&			Strikes,				// number of strikes
	const GsMatrix&			Calls					// call prices
);

EXPORT_CPP_QGRID DVPricer QGridDVPricerFromTermVars(
	double					Spot,	   				// spot
	const GsVector&			Discounts,				// discount factors
	const GsVector&			ForwardGrowth,			// forward growth curve
	const GsVector&			Times,					// times
	const GsVector&			Strikes,				// number of strikes
	const GsMatrix&			Vars					// term variances
);

EXPORT_CPP_QGRID DVPricer QGridDVPricerFromLocalVars(
	double				Spot,						// spot
	const GsVector&		Times,						// times
	const GsVector&		Strikes,					// strikes
	const GsVector&		Discounts,					// discount factors
	const GsVector&		ForwardGrowth,				// forward growth factors
	const GsMatrix&		LocalVars,					// local variance matrix
	const GsVector&		Spots						// spots at which to perform Crank-Nicholson
);

EXPORT_CPP_QGRID double QGridDVPricer_Spot(
	const DVPricer&			model					// the DVPricer
);

EXPORT_CPP_QGRID DVPricer QGridDVPricer_SetSpot(
	const DVPricer&				model,				// the DVPricer
	double						Spot				// THE SPOT
);

EXPORT_CPP_QGRID GsVector QGridDVPricer_SpotGrid(
	const DVPricer&		model				// the DVPricer
);

EXPORT_CPP_QGRID GsVector QGridDVPricer_TimeGrid(
	const DVPricer&		model				// the DVPricer
);

EXPORT_CPP_QGRID GsMatrix QGridDVPricer_LocalVols(
	const DVPricer&		model,			// the DVPricer
	const GsVector&		Times,			// the time grid
	const GsVector&		Spots			// the spot grid
);

EXPORT_CPP_QGRID GsMatrix QGridDVPricer_GetCallPrices(
	const DVPricer&		model,			// the DVPricer
	double				Spot,			// the pricing spot
	double				Time,			// the pricing time
	const GsVector&		TimesOut,		// the expiration times
	const GsMatrix&		StrikesOut		// the strikes
);

EXPORT_CPP_QGRID GsVector QGridDVPricer_StepBack(
	const DVPricer&		model,			// the DVPricer
	const GsVector&		Values,	  		// the values at t
	double				s,				// the initial time
	double				t,				// the final time
	double				LowBarrier,		// the low knockout
	double				HighBarrier,	// the high knockout
	double				LowRebate,		// the low rebate
	double				HighRebate,		// the high rebate
	double				RebateTime		// the time when the rebates are paid
);

EXPORT_CPP_QGRID double QGridDVPricer_tAccuracy(
	const	DVPricer&	model		// the DVPricer
);

EXPORT_CPP_QGRID DVPricer QGridDVPricer_tAccuracySet(
	const	DVPricer&	model,		// the DVPricer
	double				tAccuracy	// the accuracy
);

EXPORT_CPP_QGRID GsString QGridDVPricer_Description(
	const	DVPricer&	model	// the DV Pricer
);

EXPORT_CPP_QGRID double QGridDVPricer_DiscountFactor(
	const	DVPricer&	model,	// the DV Pricer
	double				Time	// Time
);

EXPORT_CPP_QGRID double QGridDVPricer_ForwardGrowthFactor(
	const	DVPricer&	model,	// the DV Pricer
	double	Time				// Time
);

CC_END_NAMESPACE

#endif

