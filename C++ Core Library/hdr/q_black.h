/* $Header: /home/cvs/src/quant/src/q_black.h,v 1.10 2001/01/08 17:37:05 demeor Exp $ */
/****************************************************************
**
**	Q_BLACK.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_Q_BLACK_H )
#define IN_Q_BLACK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#if 0
EXPORT_C_QUANT double	Q_OptSpot(
	double spot,     //	spot price
	double strike,	 //	strike price	(<0 means put)
	double vol,		 //	volatility
	double term,	 //	time to expiration
	double rd,		 //	domestic (discount) rate
	double rf		 //	foreign (yield) rate
	);
#endif

EXPORT_C_QUANT double Q_Intrinsic ( 
	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,		// Underlyer spot price
	double			strike		// Option strike price
	);

EXPORT_C_QUANT double Q_NormalOpt (
    Q_OPT_TYPE    callPut,    // option type
	double        forward,    // current forward price
	double        strike,     // strike price
	double        vol,        // annualized standard deviation of the price
	double        time,       // time to expiration
	double        ccRd        // discount rate to expiration
	);

EXPORT_C_QUANT double Q_BinOpt(
	Q_OPT_TYPE		callPut,	// call <=> payoff if greater than strike
	double			spot,		// Underlyer spot price
	double			strike,		// Option strike price
	double			vol,		// Underlyer volatility
	double			timeToExp,	// Time to expiration, actual 365
	double			riskFree,	// Domestic interest rate, continuous form
	double			divRate,	// Foreign interest rate, continuous form
	double			payoff,     // payoff if in the money at expiration
	Q_RETURN_TYPE	RetType		// Return type (Q_PREMIUM, Q_DELTA, ... )
	);

EXPORT_C_QUANT void	Q_BlackScholes(Q_OPT_TYPE type, double spot, double strike, double vol, double term,
						double r1, double r2, double *premRet, double *deltaRet );

EXPORT_C_QUANT void Q_Binary (
	Q_OPT_TYPE	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double		spot,		// Underlyer spot price
	double		strike,		// Option strike price
	double		vol,		// Underlyer volatility
	double		timeToExp,	// Time to expiration, actual 365
	double		riskFree,	// Domestic interest rate, continuous form
	double		divRate,	// Foreign interest rate, continuous form
	double		*premRet,	// Returned option premium
	double		*deltaRet	// Returned option delta
	);

EXPORT_C_QUANT void Q_EuropeanCall (
				double spot,
				double strike,
				double volatility,
				double time_to_expiration,
				double domestic_interest_rate,
				double foreign_interest_rate,
				double *price,
				double *delta );

EXPORT_C_QUANT void Q_EuropeanPut (
				double spot,
				double strike,
				double volatility,
				double time_to_expiration,
				double domestic_interest_rate,
				double foreign_interest_rate,
				double *price,
				double *delta );

#endif

