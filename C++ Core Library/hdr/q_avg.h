/* $Header: /home/cvs/src/quant/src/q_avg.h,v 1.41 2001/02/28 17:37:22 demeor Exp $ */
/****************************************************************
**
**	Q_AVG.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.41 $
**
****************************************************************/

#if !defined( IN_Q_AVG_H )
#define IN_Q_AVG_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAVG double Q_AvgFiniteC (
	Q_OPT_TYPE    callPut,         // option type
	double        spot,            // current spot price
	double        strike,          // strike price (<0 ==> put)
	Q_VECTOR      *vol,            // vector of volatilities
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	double        Rd,              // discount rate
	Q_VECTOR      *fwds,           // vector of forward prices
	Q_VECTOR      *times,          // vector of gridpoints for curves
	int           nPoints 		   // number of actual averaging points
	);
EXPORT_C_QAVG double Q_AvgFiniteW (
	Q_OPT_TYPE    callPut,         // option type
	double        spot,            // current spot price
	double        strike,          // strike price (<0 ==> put)
	Q_VECTOR      *vol,            // vector of volatilities
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	double        Rd,              // discount rate
	Q_VECTOR      *fwds,           // vector of forward prices
	Q_VECTOR      *times,          // vector of gridpoints for curves
	int           nPoints, 		   // number of actual averaging points
	Q_RETURN_TYPE retType          // return premium or delta
	);
	
EXPORT_C_QAVG double Q_AvgFiniteMC (
	Q_OPT_TYPE    callPut,         // put or call
	double        spot,            // current spot price
	double        strike,          // strike price (<0 ==> put)
	Q_VECTOR      *vol,            // vector of volatilities
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	double        Rd,              // discount rate
	Q_VECTOR      *fwds,           // vector of forward prices
	Q_VECTOR      *times,          // vector of gridpoints for curves
	int           nPoints,		   // number of actual averaging points
	long          nPaths,          // number of trials
	Q_RETURN_TYPE retType		   // return premium or delta
	);
  
EXPORT_C_QAVG double Q_FiniteAvgOpt2 (
    Q_OPT_TYPE    callPut,         // put or call
	double        spot,            // current spot price
	double        strike,          // strike price
	Q_VECTOR      *vol,            // vector of volatilities
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	double        ccRd,            // discount rate
	Q_VECTOR      *fwds,           // vector of forward prices
	Q_VECTOR      *times           // vector of gridpoints for curves
	);

EXPORT_C_QAVG int Q_FiniteAvgOpt4 (
    Q_OPT_TYPE    callPut,         // put or call
	double        spot,            // current spot price
	double        strike,          // strike price
	Q_VECTOR      *vol,            // vector of volatilities
	Q_VECTOR      *volTimes,       // grid for volatilities
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	double        ccRd,            // discount rate
	Q_VECTOR      *fwds,           // vector of forward prices
	Q_VECTOR      *times,          // vector of gridpoints for curves
	double	      *premRet,	       // Returned option premium
	double		  *deltaRet	       // Returned option delta
	);
	
EXPORT_C_QAVG double Q_AvgSpreadC (
	Q_OPT_TYPE       callPut,     // option type
	double           LSpot,       // long side spot
	double           SSpot,       // short side spot
	Q_VECTOR         *LFwd,       // long side forward prices
	Q_VECTOR         *SFwd,       // short side forward prices
	double           strike,      // strike price on spread
	Q_VECTOR         *LVol,       // long side volatilities
	Q_VECTOR         *SVol,       // short side volatilities
	Q_VECTOR         *XVol,       // cross volatilities
	Q_VECTOR         *times,      // averaging times 
	Q_VECTOR         *LVolT,      // times for long side volatilities
	Q_VECTOR         *SVolT,      // times for short side volatilities
	Q_VECTOR         *XVolT,      // times for cross volatilities
	Q_VECTOR         *LFwdT,      // times for long side forward prices
	Q_VECTOR         *SFwdT,      // times for short side forward prices
	double           ccRd         // domestic interest rate
	);
	
EXPORT_C_QAVG double Q_AvgSpread (
	Q_OPT_TYPE       callPut,     // option type
	Q_VECTOR         *LFwd,       // long side forward prices
	Q_VECTOR         *SFwd,       // short side forward prices
	double           strike,      // strike price on spread
	Q_VECTOR         *LVol,       // long side volatilities
	Q_VECTOR         *SVol,       // short side volatilities
	Q_VECTOR         *XVol,       // cross volatilities
	Q_VECTOR         *times,      // times for forwards and volatilities
	double           ccRd         // domestic interest rate
	);
	
EXPORT_C_QAVG double Q_AvgStrkSprdOpt (
    Q_OPT_TYPE     callPut,        // option type
    Q_VECTOR       *longfwds,      // long forward curve
    Q_VECTOR       *shortfwds,     // short forward curve
    Q_VECTOR       *longvols,      // long volatility curve
    Q_VECTOR       *shortvols,     // short volatility curve
    Q_VECTOR       *crossvols,     // volatility curve of long over short
    Q_VECTOR       *times,         // times for setting strike and average
    double         time,           // time to expiration settlement date
    int            index,          // index of first averaging time
    double         ccRd            // domestic rate
    );

EXPORT_C_QAVG double Q_AvgHarmonic (
	Q_OPT_TYPE      Call_Put,    // option type
	Q_VECTOR        *prices,	 // values of data curve
	double          Strike,      // strike price
	Q_VECTOR        *vols,       // volatilities at averaging dates
	DATE            PricingDate, // pricing date
	DATE            expDate,     // expiration date
	Q_DATE_VECTOR   *dates,      // dates of data curve
	double          Discount     // discount to spot date
	);

EXPORT_C_QAVG double Q_AvgHarmFwd (
	Q_VECTOR        *prices,	 // values of data curve
	Q_VECTOR        *vols,       // volatilities at averaging dates
	DATE            PricingDate, // pricing date
	Q_DATE_VECTOR   *dates       // dates of data curve
	);

EXPORT_C_QAVG double Q_FadeOutAvg (
    Q_OPT_TYPE    callPut,    // call or put
    Q_UP_DOWN     upOrDown,   // appears at upper or lower boundary
    Q_VECTOR      *fwds,      // forwards at the averaging points
    double        strike,     // strike price
    double        knockout,   // fadeout level
    Q_VECTOR      *vols,      // volatilities at averaging times
    Q_VECTOR      *times,     // fadeout and averaging times
    double        time,       // time to expiration
    double        ccRd,       // domestic interest rate to expiration
    int           npts        // size of grid for integration
    );
	  
EXPORT_C_QAVG double Q_LookBackAvg (
	Q_OPT_TYPE       callPut,           // option type - max if call, min if put
	double           spot,              // current spot price
	double           strike,            // strike price on max or min average average
	int              navg,              // number of points in the rolling average
	Q_VECTOR         *history,          // previous fixes
	Q_VECTOR         *vols,             // volatility vector for averaging times
	Q_VECTOR         *times,            // averaging times
	double           ccRd,              // domestic rate
	Q_VECTOR         *fwds,             // forward price vector
	int              nPaths,            // number of trials in monte carlo
	Q_RETURN_TYPE    retType            // return premium or error
	);

EXPORT_C_QAVG double Q_LBAvgSpotFix (
	Q_OPT_TYPE       callPut,           // option type - max if call, min if put
	double           spot,              // current spot price
	double           strike,            // strike price on max or min average average
	double           alpha,             // multiply extremum by this
	int              navg,              // number of points in the rolling average
	Q_VECTOR         *history,          // previous fixes
	Q_VECTOR         *vols,             // volatility vector for averaging times
	Q_VECTOR         *times,            // averaging times
	double           ccRd,              // domestic rate
	Q_VECTOR         *fwds,             // forward price vector
	int              nPaths,            // number of trials in monte carlo
	Q_RETURN_TYPE    retType            // return premium or error
	);

EXPORT_C_QAVG double Q_AvgVolW (
	Q_VECTOR      *fwds,           // vector of forwards
	Q_VECTOR      *vol,            // vector of volatilities
	Q_VECTOR      *times,          // vector of times for averaging
	double        time             // time to expiration
	);
	  
EXPORT_C_QAVG double Q_LimitAvgStrk (
	Q_OPT_TYPE       callPut,           // option type
	double           spot,              // current futures price
	double           average,           // to-date average if already started
	double           offset,            // fixed amount added to average
	double           cap,               // upper limit on average
	double           floor,             // lower limit on average
	DT_CURVE         *volCurve,         // volatility curve	of the future
	DATE             prcDate,           // pricing date
	Q_DATE_VECTOR    *avgdates,         // averaging dates
	DATE             expDate,           // expiration date
	DT_CURVE         *fwdCurve,         // forward curve
	DT_CURVE         *rdCurve,          // domestic rate curve
	int              nAvg,              // number of past averaging points to date
	int              nSteps,            // number of timesteps
	int              nGrid1,            // number of prices in the spot grid
	int              nGrid2             // number of prices in the average grid
	);
		    	    
EXPORT_C_QAVG double Q_LimitAvgStrkQ (
	Q_OPT_TYPE       callPut,           // option type
	double           spot,              // current futures price
	double           average,           // to-date average if already started
	double           offset,            // fixed amount added to average
	double           cap,               // upper limit on average
	double           floor,             // lower limit on average
	DT_CURVE         *volCurve,         // volatility curve	of the future
	DATE             prcDate,           // pricing date
	Q_DATE_VECTOR    *avgdates,         // averaging dates
	DATE             expDate,           // expiration date
	DT_CURVE         *fwdCurve,         // forward curve
	double           ccRd,              // domestic rate curve
	int              nAvg               // number of past averaging points to date
	);

EXPORT_C_QAVG double Q_FadeOutAvgMC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,   // direction of fadeout or accumulation
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          outVal,     // fadeout barrier
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
	Q_VECTOR        *times,     // fade and averaging points
    double	 		ccRd,     	// Domestic rate to expiration
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nPaths, 	// number of timesteps
	Q_RETURN_TYPE   retType     // return premium or error
    );
		
EXPORT_C_QAVG double Q_AdjustedVol (
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	Q_VECTOR      *times,          // vector of gridpoints for curves
	Q_VECTOR      *vol             // vector of volatilities
	);

EXPORT_C_QAVG int Q_ArithAvgOpt (
	Q_OPT_TYPE    callPut,         // put or call
	double        spot,            // current spot price
	double        strike,          // strike price
	Q_VECTOR      *vol,            // vector of volatilities
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	double        ccRd,            // discount rate
	Q_VECTOR      *fwds,           // vector of forward prices
	Q_VECTOR      *times,          // vector of gridpoints for curves
	double	      *premRet,	       // Returned option premium
	double		  *deltaRet,       // Returned option delta
	long          nPaths           // number of trials
	);

EXPORT_C_QAVG double Q_FiniteAvgFwd (
	double        spot,            // current spot price
	Q_VECTOR      *avgTimes,       // vector of times for averaging
	Q_VECTOR      *fwds,           // vector of forward prices
	Q_VECTOR      *times           // vector of gridpoints for curves
	);

EXPORT_C_QAVG int	Q_MultiHiDenFO_AvgK_MC(
	Q_OPT_TYPE		OptionType,			// Q_OPT_CALL, Q_OPT_PUT, or Q_OPT_FWD
	double			HistoricalAvg,		// Avg so far
	int				NumHistAvg,			// Number of historical points to date
	double          Offset,            	// fixed amount added to average
	double          StrikeCap,         	// upper limit on (average plus offset)
	double          StrikeFloor,     	// lower limit on (average plus offset)
	double			StrikeMultiplier,	// the strike is multiplied by this
	Q_DATE_VECTOR   *AvgDates,    		// averaging dates
	DATE		  	PricingDate,		// Pricing Date
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

EXPORT_C_QAVG double Q_MultiLimitAvgStrike(
	Q_OPT_TYPE		OptionType,				// Option type (Q_OPT_CALL, Q_OPT_PUT or Q_OPT_FWD).
	double			Spot,					// spot
	Q_VECTOR		*AveragingTimes,   		// future averaging times
	int				TotalAveragingTimes,	// number of future + past averaging times
	double			AvgSoFar,				// Average of past averaging times
	double			Offset,					// offset to the strike
	double			MinStrike,				// Minimum strike
	double			MaxStrike,				// Maximum strike
	double			StrikeMultiplier,		// strike is multiplied by this
	Q_VECTOR		*EndSegments,			// end of each time segment
	Q_VECTOR		*LowerBarrierLevels,	// low knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*UpperBarrierLevels,	// high knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*LowerRebates,			// upper rebates for knocking out
	Q_VECTOR		*UpperRebates,			// upper rebates for knocking out
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
    Q_VECTOR 		*Volatilities,			// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,		// times for volatilities
    Q_VECTOR 		*Discounts,				// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,			// times for discount factors
    Q_VECTOR 		*Forwards, 				// forwards
    Q_VECTOR 		*ForwardTimes,			// times for forwards
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	int				nLayers,				// number of layers
	double			ReferenceSpot,			// spot used to set up the grid
	double			RangeVol				// volatility used to set up the grid
);

EXPORT_C_QAVG double Q_MultiLimitAvgStrikeAlt(
	Q_OPT_TYPE		OptionType,				// Option type (Q_OPT_CALL, Q_OPT_PUT or Q_OPT_FWD).
	double			Spot,					// spot
	Q_VECTOR		*AveragingTimes,   		// future averaging times
	int				TotalAveragingTimes,	// number of future + past averaging times
	double			AvgSoFar,				// Average of past averaging times
	double			Offset,					// offset to the strike
	double			MinStrike,				// Minimum strike
	double			MaxStrike,				// Maximum strike
	double			StrikeMultiplier,		// strike is multiplied by this
	Q_VECTOR		*EndSegments,			// end of each time segment
	Q_VECTOR		*LowerBarrierLevels,	// low knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*UpperBarrierLevels,	// high knockout values ( if <= 0 then it doesn't exist)
	Q_VECTOR		*LowerRebates,			// upper rebates for knocking out
	Q_VECTOR		*UpperRebates,			// upper rebates for knocking out
	Q_REBATE_TYPE	RebateType,				// immediate or delayed until expiry date
    Q_VECTOR 		*Volatilities,			// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,		// times for volatilities
    Q_VECTOR 		*Discounts,				// Domestic discount factors
    Q_VECTOR 		*DiscountTimes,			// times for discount factors
    Q_VECTOR 		*Forwards, 				// forwards
    Q_VECTOR 		*ForwardTimes,			// times for forwards
	int          	nSteps, 				// number of timesteps
	int          	nGrid,					// number of grid points
	int				nLayers,				// number of layers
	int				Hierarchy,				// the hierarchy of the layering
	double			RangeVol				// volatility used to set up the grid
);

EXPORT_C_QAVG double Q_FadeOutAvgHDMC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,   // direction of fadeout or accumulation
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          outVal,     // fadeout barrier
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
	Q_VECTOR        *times,     // fade and averaging points
    double	 		ccRd,     	// Domestic rate to expiration
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int             maxfade,    // number of fades required for knockout
	int          	nPaths, 	// number of timesteps
	Q_RETURN_TYPE   retType     // return premium or error
    );
	  
#endif
