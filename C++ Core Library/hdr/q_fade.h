/* $Header: /home/cvs/src/quant/src/q_fade.h,v 1.26 2000/03/03 12:04:45 goodfj Exp $ */
/****************************************************************
**
**	Q_FADE.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.26 $
**
****************************************************************/
 
#if !defined( IN_Q_FADE_H )
#define IN_Q_FADE_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QFADE double Q_FadeoutFwd (
	Q_UP_DOWN        upOrDown, // direction of trigger
	double           spot,     // current spot price
	double           strike,   // strike price
	double           outVal,   // knockout level
	double           vol,      // volatility
	DATE             prcDate,  // pricing date
	DATE             expDate,  // expiration date
	double           Rd,       // domestic interest rate
	double           Rf        // foreign interest rate
	);
	
EXPORT_C_QFADE double Q_FadeOutCA(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of knockout
	double          forward,        // current forward price at expiration
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          endvol,         // volatility at expiration
	Q_VECTOR        *vols,          // volatility vector
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          ccRd,           // domestic rate to expiration
    Q_VECTOR        *fwds,          // vector of forward prices
    int             initial,        // initial group - computed exactly
    int             bundle          // later groups are this size        
	);

EXPORT_C_QFADE int Q_FadeOutCurvesBundle	(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of knockout
	double          spot,           // current spot price 
	double          forward,        // current forward price at expiration
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          reduction,      // reducton in payoff per hit
	double          endvol,         // volatility at expiration
	Q_VECTOR        *vols,          // volatility vector
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          ccRd,           // domestic rate
    Q_VECTOR        *fwds,          // vector of forward prices
    double          *premRet,       // returned premium
    double          *delta,         // returned delta
    int             initial,        // initial group - computed exactly
    int             bundle          // later groups are this size        
	);
	  
EXPORT_C_QFADE double Q_FadeStrkAndKO (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of fadeout
	Q_UP_DOWN       upOrDownKO,     // direction of knockout
	double          spot,           // current spot price
	double          strike,         // initial strike price
	double          incrementK,     // strike increases by this amount every fade
	double          incrementKO,    // knockout increases by this amount every fade
	double          outVal,         // fadeout barrier
	double          knockout,       // initial knockout level
	Q_VECTOR        *vol,           // volatility vector - one per sample
	double          endvol,         // volatility at expiration
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    Q_VECTOR        *fwds,      	// vector of forward prices
    double          endfwd,         // forward price at expiration
    double          ccRd,           // domestic rate
    int             nPaths          // number of trials for Monte Carlo run
	);

EXPORT_C_QFADE double Q_DoubleFadeStrk (
	Q_OPT_TYPE      callPut,      // option type
	Q_UP_DOWN       upOrDown2,    // direction of fade on second asset
	Q_UP_DOWN       upOrDown3,    // direction of fade on third asset
	double          fwd1,         // last forward for first asset
	double          spot2,        // spot price of second asset
	double          spot3,        // spot price of third asset
	Q_VECTOR        *fwds2,       // forwards for second asset
	Q_VECTOR        *fwds3,       // forwards for third asset
	double          strike,       // initial strike
	double          diff,         // maximum differential in strike
	double          ko2,          // fade level on second asset
	double          ko3,          // fade level on third asset
	Q_VECTOR        *vol1,        // volatilities of first asset
	Q_VECTOR        *vol2,        // volatilities of second asset
	Q_VECTOR        *vol3,        // volatilities of third asset
	Q_VECTOR        *rho12,       // term correlations of first and second
	Q_VECTOR        *rho13,       // term correlations of first and third
	Q_VECTOR        *rho23,       // term correlations of second and third
	DATE            prcDate,      // pricing date
	Q_DATE_VECTOR   *dates,       // fade dates
	double          ccRd,         // domestic interest rate
	int             nPaths        // number of paths
	);
				
#endif
