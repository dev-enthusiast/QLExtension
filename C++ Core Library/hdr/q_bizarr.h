/* $Header: /home/cvs/src/quant/src/q_bizarr.h,v 1.12 2000/04/13 12:37:57 demeor Exp $ */
/****************************************************************
**
**	Q_BIZARR.H	
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_Q_BIZARR_H )
#define IN_Q_BIZARR_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QRARE double Q_MRMAvgCapFOFwd (
	Q_OPT_TYPE        callPut,         // option type
	Q_UP_DOWN         upOrDown,        // direction of knockout
	DT_CURVE          *fwdCurve,       // forward curve
	double            strike,          // strike price
	double            outVal,          // knockout level
	double            rebate,          // delayed rebate for fading
	DT_CURVE          *volCurve,       // volatility curve
	double       	  beta,            // mean reversion coefficient
	double            longVol,         // volatility of long-term price
	double            corr,            // correlation of long and short prices
	DATE              prcDate,         // pricing date
	Q_DATE_VECTOR     *fadedates,      // vector of fade dates
    Q_DATE_VECTOR     *avgDates,       // vector of averaging dates
	DATE              fwdexp,          // expiration of fading forward
	double            ccRd,            // interest rate
	int               maxfade,         // maximum number of fades
	int               nPaths,          // number of paths for Monte Carlo
	Q_RETURN_TYPE     retType		   // return premium or error
	);

EXPORT_C_QRARE double Q_MaxPrevMonth (
	double            spot,         // current spot price
	DT_CURVE          *fwdCurve,	// forward curve
	DT_CURVE          *volCurve,    // volatility curve
	double            beta,         // mean reversion coefficient
	double            longvol,      // volatility of long price
	double            corr,         // correlation of short and long
	DATE              prcDate,		// current pricing date
	Q_DATE_VECTOR     *exdates,     // decision dates, including expiry
	int			      ngrid         // number of gridpoints
	);

EXPORT_C_QRARE double Q_GasStorage (
	double       spot,		 	// current cash price for a full unit
	double       storage,       // fee to store full unit for a year
    double	 	 pumpfee,    	// cost to put in or take out fractional unit during a period
    double       curramt,       // current amount in storage
	Q_VECTOR     *svol,      	// piecewise constant spot volatility vector
    Q_VECTOR 	 *volTimes,  	// volatility times
    double       rangevol,   	// volatility to determine range of integration
    double       rangespot,     // reference spot used for centering the grid
	double       beta,       	// mean reversion
	double       longvol,    	// long volatility
	double       corr,       	// instantaneous correlation between short and long
	double       time,          // time to expiration
	double       period,        // length of time it takes to pump fractional unit 
	Q_VECTOR     *fwds,      	// forward vector
	Q_VECTOR     *fwdT,      	// forward nodes
 	double	     ccRd,        	// interest rate
	int          nGrid1,     	// number of gridpoints in short dimension
	int          nGrid2,     	// number of gridpoints in long dimension
	int          nSteps,      	// number of timesteps
	int          endunits,      // number of fractional units left at end of option
	int          nDivide,       // number of fractional units per full unit
	double       tolerance      // tolerance parameter for conjugate gradient
	);

EXPORT_C_QRARE double Q_ChooseVarQuantOpt(
	Q_OPT_TYPE      callPut,        // option type
	double 			spot,	 		// Underlyer spot price
	double          rangespot,      // spot used to center the grid
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *LowPrice,      // offset from spot on low side
	Q_VECTOR        *HighPrice,     // offset from spot on high side
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *QuantDates,  	// dates when quantities are decided
    Q_DATE_VECTOR   *ExpDates,  	// expirations of variable quantity options
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	int          	npts			// number of points in final integrals
    );

EXPORT_C_QRARE int Q_ChooseVarQuantOptFFT(
	Q_OPT_TYPE      callPut,        // option type
	double          rangespot,      // spot used to center the grid
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *LowPrice,      // offset from spot on low side
	Q_VECTOR        *HighPrice,     // offset from spot on high side
	Q_VECTOR        *LowPct,        // percentage at lower level
	Q_VECTOR        *HighPct,       // percentage at upper level
	DT_CURVE 		*vol,			// volatility curve
	double          rangevol,       // volatility used to calculate the range of integration
	DATE            prcDate,        // pricing date
	Q_DATE_VECTOR   *QuantDates,  	// dates when quantities are decided
	Q_DATE_VECTOR   *ExpDates,  	// expirations of variable quantity options
	DT_CURVE 		*RdCurve,  		// domestic rate curve
	DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	gridlog,		// log2 of number of gridpoints
	double          bound,          // return spots between rangespot*(1-bound) and rangespot*(1+bound)
	Q_VECTOR        *SpotVec,       // returned spot array
	Q_VECTOR        *OptionPrice    // returned price array
    );
    			
#endif

