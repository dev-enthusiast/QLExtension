/* $Header: /home/cvs/src/quant/src/q_newfo.h,v 1.28 2000/03/01 14:57:24 goodfj Exp $ */
/****************************************************************
**
**	Q_NEWFO.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.28 $
**
****************************************************************/

#if !defined( IN_Q_NEWFO_H )
#define IN_Q_NEWFO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QUANT double Q_FadeOutC(
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
    Q_VECTOR        *fwd            // vector of forward prices
	);

EXPORT_C_QUANT double Q_FadeOutKOQuick(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDownFO,     // direction of knockout
	Q_UP_DOWN       upOrDownKO,     // direction of knockout
	double          spot,           // current spot price
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          knockout,       // knockout barrier
	double          reduction,      // reduction in payoff per hit
	Q_VECTOR        *vols,          // volatility vector
	double          endvol,         // volatility to expiration
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    Q_VECTOR        *Rd,            // domestic rate vector
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // foreign rate
    double          forward,        // forward at expiration
    int             nCorr,          // number of correlation intervals
    int             nGrid           // number of intervals in each dimension
	);

EXPORT_C_QUANT double Q_FadeOutDC(
	Q_OPT_TYPE      callPut,        // option type
	double          forward,        // current forward price at expiration
	double          strike,         // strike price
	double          outVal,         // lower fadeout barrier
	double          outVal2,        // upper fadeout barrier
	double          endvol,         // volatility at expiration
	Q_VECTOR        *vols,          // volatility vector
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          ccRd,           // domestic rate to expiration
    Q_VECTOR        *fwds           // vector of forward prices
	);

EXPORT_C_QUANT double Q_FadeOutDouble(
	Q_OPT_TYPE      callPut,        // option type
	double          spot,        	// current spot price
	double          strike,         // strike price
	double          outVal,         // lower fadeout barrier
	double          outVal2,        // upper fadeout barrier
	double			reduction,		// reducton in payoff per hit
	Q_VECTOR        *vols,          // volatility vector
	double          endvol,         // volatility at expiration
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          ccRd,           // domestic rate to expiration
    Q_VECTOR        *fwds,          // vector of forward prices
	double          forward,        // current forward price at expiration
	Q_RETURN_TYPE	rettype			// return type, premium or delta
	);

EXPORT_C_QUANT int Q_Sort (
	DATE    *dates,             // array of dates
	int     n       			// size of array
	);
EXPORT_C_QUANT double Q_TrigFadeOut(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of knockout
	double          forward,        // current forward price at expiration
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          endvol,         // volatility at expiration
	Q_VECTOR        *vols,          // volatilities at the sampling points
	Q_VECTOR        *vols2,         // volatilities of 2nd asset at the sampling points
	Q_VECTOR        *xvols,         // cross volatilities at the sampling points
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          ccRd,           // domestic rate to expiration
    Q_VECTOR        *fwds           // forward prices of second asset at the sampling points
	);
#endif

