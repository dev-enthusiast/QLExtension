/* $Header: /home/cvs/src/quant/src/q_amort.h,v 1.38 2000/11/14 19:21:34 demeor Exp $ */
/****************************************************************
**
**	Q_AMORT.H - routines to handle amortizing swaps
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.38 $
**
****************************************************************/

#if !defined( IN_Q_AMORT_H )
#define IN_Q_AMORT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAMORT double Q_AmortFloatSwapACN (
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
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
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QUANT int Q_AmortFloatSwapACNVec (
	Q_UP_DOWN       upOrDown,       // knockout direction
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
    double      	basefloat,      // to-date accumulation of floating payments
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
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
    DT_CURVE		*ForeignDisc,	// foreign discount curve
    int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_VECTOR		*SpotVec,		// returned array of spots
    Q_VECTOR		*OptionPrice	// returned array of option prices
	);


EXPORT_C_QAMORT double Q_AmortFloatSwapLag (
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
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
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );
                
EXPORT_C_QAMORT double Q_AmortFloatSwapAMC (
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
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
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );

EXPORT_C_QAMORT double Q_AmortSwapAMC (
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
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );

EXPORT_C_QAMORT double Q_AmortOptAMC (
	Q_OPT_TYPE      callPut,        // option type on each payment
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
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );

EXPORT_C_QAMORT int Q_AmortFloatOptMC (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
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
    double          rangevol,       // volatility used for range calculations
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DATE            endKO,          // end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	double          *answers        // returned premium, error, raw premium, raw error
    );

EXPORT_C_QAMORT double Q_AmortSwapLag (
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
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AmortFloatSwapKOD (
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outValL,        // lower knockout level
	double          outValU,        // upper knockout level
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
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );
  
EXPORT_C_QAMORT int Q_AmortFloatSwapKODVec (
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
	double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outValL,        // lower knockout level
	double          outValU,        // upper knockout level
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
	DT_CURVE 		*valueDisc,		// value currency discount curve
	DT_CURVE 		*underDisc,		// under currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_VECTOR		*SpotVec,		// returned vector of spots
	Q_VECTOR		*OptionPrice	// returned vector of option prices
    );

EXPORT_C_QAMORT double Q_AmortFloatSwapKODMC (
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          outValL,        // lower knockout level
	double          outValU,        // upper knockout level
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
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );

EXPORT_C_QAMORT double Q_AmortFloatOptFFT (
	Q_OPT_TYPE      callPut,        // knockout direction
	double 			spot, 		  	// current spot price
	double 			rangespot,		// spot price about which to center the grid
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
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
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int             gridsize        // number of gridpoints - must be power of 2
    );
	  
EXPORT_C_QAMORT double Q_AmortFloatOptFFT2 (
	Q_OPT_TYPE      callPut,        // knockout direction
	double 			spot, 		  	// current spot price
	double 			rangespot,		// spot price about which to center the grid
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
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
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int             gridsize,       // number of gridpoints - must be power of 2
	int             gridsize2       // float gridpoints, not necessarily a power of 2
    );
	  
#endif
