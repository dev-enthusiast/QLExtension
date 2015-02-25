/* $Header: /home/cvs/src/quant/src/q_koavg.h,v 1.19 2000/03/23 13:48:19 goodfj Exp $ */
/****************************************************************
**
**	Q_KOAVG.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.19 $
**
****************************************************************/

#if !defined( IN_Q_KOAVG_H )
#define IN_Q_KOAVG_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QKNOCK double Q_KODiscAvg (
	Q_OPT_TYPE       callPut,           // option type
	Q_UP_DOWN        upOrDown,          // direction of knockout
	double           spot,              // current spot price
	double           strike,            // strike price on average
	double           outVal,            // knockout level on spot
	double           rebate,            // rebate upon knockout
	Q_REBATE_TYPE    rebType,           // type of rebate
	Q_VECTOR         *vols,             // volatility vector
	Q_VECTOR         *volT,             // volatility time vector
	double           time,              // time to expiration
	Q_VECTOR         *times,            // averaging times
	Q_VECTOR         *rd,               // domestic rate vector
	Q_VECTOR         *rdT,              // domestic rate times
	Q_VECTOR         *fwds,             // forward price vector
	Q_VECTOR         *fwdT,             // forward price times vector
	int              nPaths             // number of trials in monte carlo
	);

EXPORT_C_QKNOCK double Q_KOOnRunAvgCN (
	Q_OPT_TYPE       callPut,           // option type
	Q_UP_DOWN        upOrDown,          // direction of knockout
	double           spot,              // current spot price
	double           strike,            // strike price on average
	double           average,           // to-date average
	int              npoints,           // number of points already averaged
	double           outVal,            // knockout level on average
	Q_VECTOR         *vols,             // volatility vector at averaging points
	double           lastvol,           // volatility at expiration
	DATE             prcDate,           // pricing date
	DATE             expDate,           // expiration date
	Q_VECTOR         *dates,            // dates for averaging
	int              daySteps,          // number of timesteps per day
	double           ccRd,              // domestic rate
	Q_VECTOR         *fwds,             // forward prices at averaging points
	double           lastfwd,           // forward price at expiration
	int              nGrid1,            // number of intervals in spot dimension
	int              nGrid2             // number of intervals in average dimension
	);

EXPORT_C_QKNOCK double Q_ParisAvg (
    Q_OPT_TYPE		callPut,                // call or put
    Q_UP_DOWN		upOrDown,               // knocks out downward or upward
    double			Spot,                   // current spot price
    double			strike,                 // strike price
    double			outVal,                 // knockout level
    int				koperiod,               // knockout period in days
    DT_CURVE		*VolCurve,              // volatility curve
    DATE			PricingDate,            // pricing date
    DATE			startavg,               // start of averaging period
    DATE			ExpirationDate,         // expiration date
    double			ccRd,                   // domestic interest rate to expiration
    DT_CURVE		*FwdCurve,              // forward curve
    int				nPaths,                 // number of Monte Carlo paths
    char   			*denom,                 // overlying currency
    char			*quantunit              // quantity unit
    );

EXPORT_C_QKNOCK double Q_PKOFutAvg (
	Q_OPT_TYPE       callPut,           // option type
	Q_UP_DOWN        upOrDown,          // direction of knockout
	double           future,            // current futures price
	double           strike,            // strike price on average
	double           average,           // to-date average if already started
	double           outVal,            // knockout level on spot
	double           rebate,            // rebate upon  knockout
	Q_REBATE_TYPE    rebType,           // immediate or delayed rebate
	DT_CURVE         *volCurve,         // volatility curve	of the future
	DATE             prcDate,           // pricing date
	DATE             beginKO,           // begin knockout date
	DATE             startAvg,          // first average date
	DATE             expDate,           // expiration date
	DT_CURVE         *rdCurve,          // domestic rate curve
	int              daySteps,          // number of timesteps per day
	int              nGrid              // number of prices in the grid
	);
    	
EXPORT_C_QKNOCK double Q_KOPDF(
					double      beginspot,
					double      endspot,
					double      knockout,
					double      vol,
					double      time,
					double      ccRd,
					double      ccRf
					);
#endif
