/****************************************************************
**
**	q_cn.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_Q_VARQTY_H )
#define IN_Q_VARQTY_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAMORT double Q_WindowedDKOVQ(
	Q_OPT_TYPE    	OptionType,			// Option type (Q_OPT_PUT, Q_OPT_CALL, Q_OPT_FWD )
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
	double			LowBarrier,			// Low Barrier
    double 			HighBarrier,		// High Barrier
	double			LowerRebate,		// Knockout rebate at the low barrier
    double 			UpperRebate,	 	// Knockout rebate at the high barrier
	double			StartLow,			// start time of low barrier
	double			EndLow,				// end time of low barrier
	double			StartHigh,			// start time of high barrier
	double			EndHigh,			// end time of high barrier
    double 			term,	 			// Time to expiration
	double			qdtime,				// Quantity determination time
	double			LowPrice,			// Spot level below which you get LowAmount of notional
	double			HighPrice,          // Spot level above which you get HighAmount of notional 
	double			LowAmount,			// The amount of notional at LowPrice
	double			HighAmount,			// The amount of notional at HighPrice
    Q_REBATE_TYPE	RebateType,			// Immediate or delayed till exp date
    Q_VECTOR 		*Volatilities,		// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,	// times for volatilities
    Q_VECTOR 		*Discounts,			// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,		// times for discount factors
    Q_VECTOR 		*Forwards, 			// forwards
    Q_VECTOR 		*ForwardTimes,		// times for forwards
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of grid points
	double			RangeVol			// volatility used to set up the grid
);

EXPORT_C_QAMORT int	Q_MultisegHiDensityVQFO_MC(
	Q_OPT_TYPE		OptionType,			// Q_OPT_CALL, Q_OPT_PUT, or Q_OPT_FWD
	double 			Strike,	 			// Option strike price
	DATE			PricingDate,		// Pricing Date
	Q_VECTOR		*EndTimes,			// End time of each segment
	Q_VECTOR		*LowKOBarriers,		// Knock-out barrier( Strictly negative means no barrier )
	Q_VECTOR		*HighKOBarriers,	// Knock-out price( "	" )
	Q_VECTOR		*LowRebates,		// rebate for knocking at the lower level
	Q_VECTOR		*HighRebates,		// rebate for knocking at the upper level
	Q_REBATE_TYPE  	RebateType,	 		// Immediate or delayed till exp date
	int				IsPartialKO,		// set to TRUE if rebate includes vanilla option
	int            	MaxFade,    		// Maximum number of fading events allowed
	int				HighDensity,		// TRUE for high density, (only has an effect in nonresurecting case)
	Q_IN_OUT		InOrOutSide,    	// Region of fading (inside or outside barrier bounds)
	Q_IN_OUT	    FadeInOrOut,    	// Accumulate or Fadeout in the Region of fading
	Q_FADEOUT_TYPE	FadeOutType,		// Q_FADEOUT_RESURRECTING / Q_FADEOUT_NON_RESURRECTING
	Q_VECTOR		*FadeTimes,			// Times when fading can occur
	Q_VECTOR		*LowFadeBarriers,	// Lower fade barrier
	Q_VECTOR		*HighFadeBarriers,	// Upper fade barrier
	DATE			QDDate,             // Quantity determination date - entire payout gets multiplied by nominal 
	double			LowPrice,			// Spot level below which you get LowAmount of notional
	double			HighPrice,          // Spot level above which you get HighAmount of notional 
	double			LowAmount,			// The amount of notional at LowPrice
	double			HighAmount,			// The amount of notional at HighPrice
	DT_CURVE  		*FwdCurve,			// Forward curve.
	DT_CURVE  		*VolCurve,			// Term vol curve.
	DT_CURVE  		*DiscFactorCurve,	// Discount Factor curve.
	double			StdErrTolerance,	// Stop if StdErr is less than this, else NumPathsBy2 reached
	int          	NumPathsBy2, 		// Maximum Num Monte Carlo paths
	int				Seed,				// MC seed (large negative number is best)
	double			*Premium,			// the price returned
	double			*StdError			// the standard deviation of the premium
);

#endif

