/* $Header: /home/cvs/src/quant/src/q_amort2.h,v 1.51 2001/05/14 13:53:32 demeor Exp $ */
/****************************************************************
**
**	Q_AMORT2.H	
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.51 $
**
****************************************************************/

#if !defined( IN_Q_AMORT2_H )
#define IN_Q_AMORT2_H


#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
       
    double Q_MRMAmortFloatSwap(
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
	Q_VECTOR 		*strike,   		// one strike price	per payment date
	Q_VECTOR 		*Amounts,   	// number of units per payment
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    Q_VECTOR 		*vol,			// piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    double          rangevol,       // volatility used to calculate the range of integration
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*times,	 		// payment times
    Q_VECTOR 		*Rd,     		// one domestic rate per payment time
    Q_VECTOR 		*fwds,     		// one forward price per payment time
	int          	nGrid			// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_MRMAmortFloatSwapMC2 (
    double 			spot,           // Underlyer spot price
    double          basefloat,      // to-date accumulation of floating payments
    Q_VECTOR 		*strike,        // one strike price per payment date
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    double          rangevol,       // volatility to determine range of integration
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*times,         // payment times
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // one forward price per payment time
    int             nGrid,          // number of gridpoints in integration
    int             nPaths,         // number of paths in Monte Carlo
    Q_RETURN_TYPE   retType         // return premium or delta
    );
    
EXPORT_C_QAMORT int Q_MRMAmortFloatCapSwapMC (
    double 			spot,           // Underlyer spot price
    double          basefloat,      // to-date accumulation of floating payments
    double          maxfloat,       // max accrual can get to
    Q_VECTOR 		*strike,        // one strike price per payment date
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*times,         // payment times
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // one forward price per payment time
    int             nGrid,          // number of gridpoints in integration
    int             nPaths,         // number of paths in Monte Carlo
    double          *vars           // return premium, error, raw premium, raw error
    );

EXPORT_C_QAMORT double Q_AmortSwapAMCIKO (
	Q_OPT_TYPE		callPut,        // call, put or forward
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DATE            endko,			// end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );

EXPORT_C_QAMORT double Q_AmortSwapAMCIKOMod (
	Q_OPT_TYPE		callPut,        // call, put or forward
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DATE            endko,			// end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdGrowthCurve,   	// forward growth curve
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );



EXPORT_C_QAMORT double Q_AmortOptACNW(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double			spot,			// underlyer spot price
	double 			rangespot,	 	// spot price used for centering grid
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );
EXPORT_C_QAMORT double Q_OptAmortSwap(
	Q_OPT_TYPE		callPut,        // call or put on swap
	Q_OPT_TYPE		underCP,        // underlying swap or underlying strip of options
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	double          Strike,         // strike on swap
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    DATE            optExp,         // expiration date of option
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );
EXPORT_C_QAMORT int Q_MRMPaths (
    double 			spot,           // Underlyer spot price
    double          Long,           // long price - usually final entry in forward curve
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*times,         // nodes in the paths
    Q_VECTOR 		*fwds,          // forwards
    Q_VECTOR        *fwdT,          // forward grid
    int             seed,           // seed for random number generator
    Q_MATRIX        *SpotPaths,     // returned spot paths, pre-allocated 
    Q_MATRIX        *LongPaths 		// returned long price paths, pre-allocated
    );
EXPORT_C_QAMORT double Q_AmortOptACNIKO(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );
                           
EXPORT_C_QAMORT int Q_AmortOptACNWVec(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
	DT_CURVE		*ForeignDisc, 	// foreign currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
    Q_VECTOR		*SpotVec,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
	);
	
EXPORT_C_QAMORT int Q_MRMAmortFloatCapOptMC (
	Q_OPT_TYPE		callPut,        // call, put or forward
    double 			spot,           // Underlyer spot price
    double          basefloat,      // to-date accumulation of floating payments
    double          maxfloat,       // max accrual can get to
    Q_VECTOR 		*strike,        // one strike price per payment date
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*times,         // payment times
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // one forward price per payment time
    int             nGrid,          // number of gridpoints in integration
    int             nPaths,         // number of paths in Monte Carlo
    double          *vars           // return premium, error, raw premium, raw error
    );
	
EXPORT_C_QAMORT int Q_DefineRanges(
			double 		maxRange,		// the largest spot of interest
			double 		minRange,		// the smallest spot of interest
			double 		prcntgRange,	// ratio of rangespot to extremal spot values within a given range
			double 		rangevol, 		// vol used in grid
			DATE 		prcDate, 		// pricing date
			DT_CURVE 	*strike,   		// strike curve
			int 		gridsize,		// gridsize
			Q_MATRIX    *SpotRanges		// output min and max spots
);

EXPORT_C_QAMORT int Q_MRMOptPayoffCap (
	Q_OPT_TYPE      callPut,        // option type
    double 			spot,           // Underlyer spot price
    DT_CURVE 		*strike,        // strike curve
	double          payoffCap,      // cap on sum of payoffs
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );

EXPORT_C_QAMORT int Q_MRMOptPayoffCap2 (
	Q_OPT_TYPE      callPut,        // option type
    double 			spot,           // Underlyer spot price
    DT_CURVE 		*strike,        // strike curve
	double          payoffCap,      // cap on sum of payoffs
    Q_VECTOR 		*Amounts,       // One Amount per payment date
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*Rd,            // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error          // returned standard error
    );

#endif

