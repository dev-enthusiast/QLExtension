/* $Header: /home/cvs/src/quant/src/q_mulfok.h,v 1.13 2000/03/03 14:52:05 goodfj Exp $ */
/****************************************************************
**
**	q_mulfok.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_Q_MULFOK_H )
#define IN_Q_MULFOK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_MultipleDFODKOc(
	Q_OPT_TYPE		isCall,	 		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 		// Underlyer spot price
	double 			strike,	 		// Option strike price
	Q_VECTOR		*endPeriod,		// End of each time period
    Q_VECTOR		*lowOutVal,		// Knock-out price( Strictly negative means no barrier )
    Q_VECTOR		*highOutVal,	// Knock-out price( "	" )
    Q_VECTOR		*lowRebate, 	// rebate for knocking at the lower level
    Q_VECTOR		*highRebate,	// rebate for knocking at the upper level
    Q_REBATE_TYPE   rebType,	 	// Immediate or delayed till exp date
	Q_VECTOR		*FadeOutTimes,	// Fade out low level
	Q_VECTOR		*lowFadeOutVal,	// Fade out low level
	Q_VECTOR		*highFadeOutVal,// Fade out low level
    Q_FADEOUT_TYPE fadeOutType, 	// Immediate or delayed till exp date
    Q_VECTOR 		*Vol,			// Underlyer volatility	curve
    Q_VECTOR 		*VolT,      	// grid for volatilities
    Q_VECTOR 		*Rd,     		// Domestic rate curve
    Q_VECTOR 		*RdT,       	// grid for domestic rate curve
    Q_VECTOR 		*Fwd,     		// forward curve
    Q_VECTOR 		*FwdT,	      	// grid for forward curve
    double	 		AssetDiscretizationFactor, // discretization multiplicator in the x direction
    double	 		tDiscretizationFactor, // discretization multiplicator in the t direction
	double			AnchorVol		// Vol used to anchor the grid if the barriers are far apart
);

EXPORT_C_QKNOCK int Q_MultisegHiDensityFO_MC(
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

