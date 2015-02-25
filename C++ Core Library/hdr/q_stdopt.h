/* $Header: /home/cvs/src/quant/src/q_stdopt.h,v 1.9 2007/05/11 17:05:19 e13749 Exp $ */
/****************************************************************
**
**	Q_STDOPT.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_Q_STDOPT_H )
#define IN_Q_STDOPT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_OptAStd(
	Q_OPT_TYPE		type,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE		style,		// European or american style
	double      	spot,       // current spot price
	double      	strike,     // strike price of option
	double      	vol,        // volatility
	int         	days,       // days to settle
	DATE        	prcDate,    // pricing date
	DATE        	expDate,    // expiration date
	double      	ccRd,       // domestic interest rate
	double      	ccRf,       // foreign interest rate
	Q_RETURN_TYPE   retType  	// return premium or delta
	);
	
EXPORT_C_QUANT double Q_OptASetlKap (
	Q_OPT_TYPE	type,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE	style,		// European or american style
	double      spot,       // current spot price
	double      strike,     // strike price of option
	double      vol,        // volatility
	int         days,       // days to settle
	DATE        prcDate,    // pricing date
	DATE        expDate,    // expiration date
	double      ccRd,       // domestic interest rate
	double      ccRf        // foreign interest rate
	);
	
EXPORT_C_QUANT double Q_OptASetlTheta (
	Q_OPT_TYPE		type,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE		style,		// European or american style
	double      	spot,       // current spot price
	double      	strike,     // strike price of option
	double      	vol,        // volatility
	int         	days,       // days to settle
	DATE        	prcDate,    // pricing date
	DATE        	expDate,    // expiration date
	double      	ccRd,       // domestic interest rate
	double      	ccRf        // foreign interest rate
	);
	
EXPORT_C_QUANT void 	Q_StdOpt(
			Q_OPT_TYPE		callPut,		// Option type (OPT_PUT or OPT_CALL)
			Q_OPT_STYLE		euroAmer,		// European or american style
			Q_OPT_ON		spotFut,		// Option on spot or futures
			Q_MARGIN		markToMkt,  	// Daily mark to market or not
			double			spot,			// Underlyer spot price
			double			strike,			// Option strike price
			double			vol,			// Underlyer volatility
			double			timeToExp,		// Time to option expiration, actual 365
			double			timeToSettle,	// Time to settlement of underlying
			double			ccRd,			// Domestic interest rate, continuous form
			double			ccRf,			// Foreign interest rate, continuous form
			double			*premRet,		// Returned option premium
			double			*deltaRet		// Returned option delta
			);

EXPORT_C_QUANT double Q_OptStd(
	Q_OPT_TYPE		type,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE		style,		// European or american style
	Q_OPT_ON		spotFut,	// Option on spot or futures
	Q_MARGIN		markToMkt,  // Daily mark to market or not
	double      	spot,       // current spot price
	double      	strike,     // strike price of option
	double      	vol,        // volatility
	int         	days,       // days to settle
	DATE        	prcDate,    // pricing date
	DATE        	expDate,    // expiration date
	double      	ccRd,       // domestic interest rate
	double      	ccRf,       // foreign interest rate
	Q_RETURN_TYPE   retType  	// return premium or delta
	);


EXPORT_C_QUANT double Q_StdOptGamma(
	Q_OPT_TYPE	callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE	euroAmer,		// European or american style
	Q_OPT_ON	underType,		// Option on spot or futures
	Q_MARGIN	markToMkt,		// Daily mark to market or not
	double		spot,			// Underlyer spot price
	double		strike,			// Option strike price
	double		vol,			// Underlyer volatility
	double		timeToExp,		// Time to option expiration, actual 365
	double		timeToSettle,	// Time to settlement of underlying
	double		ccRd,			// Domestic interest rate, continuous form
	double		ccRf			// Foreign interest rate, continuous form
	);


EXPORT_C_QUANT double Q_StdOptKappa(
	Q_OPT_TYPE	callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE	euroAmer,		// European or american style
	Q_OPT_ON	underType,		// Option on spot or futures
	Q_MARGIN	markToMkt,		// Daily mark to market or not
	double		spot,			// Underlyer spot price
	double		strike,			// Option strike price
	double		vol,			// Underlyer volatility
	double		timeToExp,		// Time to option expiration, actual 365
	double		timeToSettle,	// Time to settlement of underlying
	double		ccRd,			// Domestic interest rate, continuous form
	double		ccRf			// Foreign interest rate, continuous form
	);

EXPORT_C_QUANT double Q_StdOptTheta(
	Q_OPT_TYPE	callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE	euroAmer,		// European or american style
	Q_OPT_ON	underType,		// Option on spot or futures
	Q_MARGIN	markToMkt,		// Daily mark to market or not
	double		spot,			// Underlyer spot price
	double		strike,			// Option strike price
	double		vol,			// Underlyer volatility
	double		timeToExp,		// Time to option expiration, actual 365
	double		timeToSettle,	// Time to settlement of underlying
	double		ccRd,			// Domestic interest rate, continuous form
	double		ccRf			// Foreign interest rate, continuous form
	);

EXPORT_C_QUANT double Q_OptionStripFut (
	DT_CURVE	*Fwds,		    // Forwards
	DT_CURVE	*Strikes,       // Strikes
    DT_CURVE    *WeightCurve,   // Weight Curve
	Q_OPT_TYPE	CallPut,		// Option Type
	long		Today,			// Pricing Date
	long		ExpSetDate,		// Expiration Settlement date
	double		IntRate,		// Domestic interest rate
	DT_CURVE	*TermVols		// Term Vols
	);

EXPORT_C_QUANT double Q_OptionStripFut2(
	DT_CURVE        *WeightCurveOpen,   // Weight Curve Open
	DT_CURVE        *DailyStrikesOpen,  // Daily Strikes Open
	DT_CURVE        *DailyFwdsOpen,     // Daily Forwards Open
	DT_CURVE        *ExpDatesOpen,      // Expiration Dates Daily Open
    char            *OptionType,        // Option Type e.g. "Call"
	DATE		    Today,			    // Pricing Date
	DATE		    ExpSetDate,		    // Expiration Settlement date
	double		    IntRate,		    // Domestic interest rate
    double		    FlatVol,            // Vol corresp. to Q_OptDailyImpliedVol
    Q_RETURN_TYPE   RetType             // E.g. Q_KAPPA
	);


EXPORT_C_QUANT double Q_OptDailyImpliedVol(
	DT_CURVE *WeightCurveOpen,  // Weight Curve Open
	DT_CURVE *DailyStrikesOpen, // Daily Strikes Open
	DT_CURVE *DailyVols,        // Daily Vols Open
	DT_CURVE *DailyFwdsOpen,    // Daily Forwards Open
	DT_CURVE *ExpDatesOpen,     // Expiration Dates Daily Open
	double   AccumAmt,          // Accumulated Amount
    DATE     Today,             // Pricing Date
    DATE     Exp,               // Expiration Settlement Date 
    double   IntRate,           // Domestric Interest Rate
	double   SpotPrice,         // Spot Price of strip
	char     *OptionType        // Option type
);



#endif

