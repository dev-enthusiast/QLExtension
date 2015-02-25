/****************************************************************
**
**	q_fadeoutmc.h	- Monte Carlo fadeout routines
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_FADEOUTMC_H )
#define IN_Q_FADEOUTMC_H

#include <q_base.h>
#include <qtypes.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsTimeSeries.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_QKNOCK double Q_FadeOutPKOAvgStrkMC(
	Q_OPT_TYPE    				OptionType,    		// call, put or fwd
	Q_UP_DOWN     				upOrDown,   	// direction of fadeout
	Q_UP_DOWN     				upOrDownKO, 	// partial knockout direction
	double	      				spot,       	// current spot price
	double        				outVal,     	// fadeout level
	double        				knockout,   	// partial knockout level
	const GsDoubleTimeSeries&	vols,      		// volatility curve
	const GsDateVector& 		avgDates,  		// averaging times
	const GsDateVector& 		fadeDates, 		// fadeout dates
	const GsDate&  				prcDate,    	// pricing date
	const GsDate&  				expDate,    	// expiration date
    double        				Discount,       // domestic discount factor to expiration
	const GsDoubleTimeSeries&   fwds,      		// forward curve
	double          			avgpast,        // average up to now
	int             			navg,           // number of past average points
	int             			npast,          // total number of past points
	int             			naccum,         // number of accumulated legs
	int           				nPaths, 	  	// number of paths
	Q_RETURN_TYPE   			retType         // number of gridpoints in average dimension
);

EXPORT_CPP_QKNOCK bool Q_FadeOutKOLimAvgStrkMC(
	Q_OPT_TYPE    				OptionType,    		// call, put or fwd
	Q_UP_DOWN     				upOrDown,   		// direction of fadeout
	Q_UP_DOWN     				upOrDownKO, 		// partial knockout direction
	double						MinStrike,			// strike floor	(HUGE_VAL for no limit)
	double						MaxStrike,			// strike cap (HUGE_VAL for no limit)
	double						StrikeOffset,		// Offset to add to average
	double						Expiration,			// time to expiration
	double        				outVal,     		// fadeout level
	double        				knockout,   		// knockout level
    double        				VanillaRebate,      // 1 for partial ko
	double	      				spot,       		// current spot price
	const GsVector&				Volatilities, 		// vols
	const GsVector&				VolatilityTimes, 	// times for vols
	const GsVector&				Forwards,			// forwards
	const GsVector&				ForwardTimes,		// times for forwards
    double        				Discount,       	// domestic discount factor to expiration
	const GsVector& 			AveragingTimes,  	// averaging times
	const GsVector& 			FadeTimes, 			// fadeout times
	double          			avgpast,        	// average up to now
	int             			navg,           	// number of past average points
	int             			npast,          	// total number of past points
	int             			naccum,         	// number of accumulated legs
	int           				nPaths, 	  		// number of paths
	double						*Premium,			// returned price
	double						*StdError			// standard error
);

EXPORT_CPP_QKNOCK GsDt* Q_FadeOutPKOLimAvgStrkMC(
	Q_OPT_TYPE    				OptionType,    		// call, put or fwd
	Q_UP_DOWN     				upOrDown,   		// direction of fadeout
	Q_UP_DOWN     				upOrDownKO, 		// partial knockout direction
	double						MinStrike,			// strike floor	(HUGE_VAL for no limit)
	double						MaxStrike,			// strike cap (HUGE_VAL for no limit)
	double						StrikeOffset,		// Offset to add to average
	double						Expiration,			// time to expiration
	double        				outVal,     		// fadeout level
	double        				knockout,   		// partial knockout level
	double	      				spot,       		// current spot price
	const GsVector&				Volatilities, 		// vols
	const GsVector&				VolatilityTimes, 	// times for vols
	const GsVector&				Forwards,			// forwards
	const GsVector&				ForwardTimes,		// times for forwards
	double        				ccRd,       		// domestic interest rate to expiration
	const GsVector& 			AveragingTimes,  	// averaging times
	const GsVector& 			FadeTimes, 			// fadeout times
	double          			avgpast,        	// average up to now
	int             			navg,           	// number of past average points
	int             			npast,          	// total number of past points
	int             			naccum,         	// number of accumulated legs
	int           				nPaths 	  			// number of paths
);

EXPORT_CPP_QKNOCK GsDt* Q_FadeOutKOLimAvgStrkMC(
	Q_OPT_TYPE    				OptionType,    		// call, put or fwd
	Q_UP_DOWN     				upOrDown,   		// direction of fadeout
	Q_UP_DOWN     				upOrDownKO, 		// partial knockout direction
	double						MinStrike,			// strike floor	(HUGE_VAL for no limit)
	double						MaxStrike,			// strike cap (HUGE_VAL for no limit)
	double						StrikeOffset,		// Offset to add to average
	double						Expiration,			// time to expiration
	double        				outVal,     		// fadeout level
	double        				knockout,   		// knockout level
    double        				VanillaRebate,      // 1 for partial ko
	double	      				spot,       		// current spot price
	const GsVector&				Volatilities, 		// vols
	const GsVector&				VolatilityTimes, 	// times for vols
	const GsVector&				Forwards,			// forwards
	const GsVector&				ForwardTimes,		// times for forwards
	double        				ccRd,       		// domestic interest rate to expiration
	const GsVector& 			AveragingTimes,  	// averaging times
	const GsVector& 			FadeTimes, 			// fadeout times
	double          			avgpast,        	// average up to now
	int             			navg,           	// number of past average points
	int             			npast,          	// total number of past points
	int             			naccum,         	// number of accumulated legs
	int           				nPaths 	  			// number of paths
);

CC_END_NAMESPACE

#endif

