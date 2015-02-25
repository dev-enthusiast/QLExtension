/* $Header: /home/cvs/src/quant/src/q_impstk.h,v 1.4 1999/12/15 15:49:41 goodfj Exp $ */
/****************************************************************
**
**	Q_IMPSTK.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_Q_IMPSTK_H )
#define IN_Q_IMPSTK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_OptImpStk (
	Q_OPT_TYPE 		callPut,         // Call or Put
	Q_OPT_STYLE     euroAmer,        // European or American
	double  		spot,            // underlying spot price
	double			EstimatedStrike, // guess on option strike price
	double			Vol,			 // volatility
	double			time,   		 // Time to option expiration, actual 365
	double			ccRd,      		 // Domestic interest rate
	double			ccRf, 	    	 // Foreign interest rate
	double			price			 // Option premium
    );
    
EXPORT_C_QUANT double Q_OptImpStkSetl (
	Q_OPT_TYPE 		type,            // Call or Put
	Q_OPT_STYLE     style,           // European or American
	double  		spot,            // underlying spot price
	double			EstimatedStrike, // guess on option strike price
	double  		Vol,             // volatility
	int     		days,       	 // days to settle
	DATE    		prcDate,    	 // pricing date
	DATE    		expDate,    	 // expiration date
	double  		ccRd,         	 // domestic interest rate
	double  		ccRf,          	 // foreign interest rate
	double			price			 // Option premium
	);
    
EXPORT_C_QUANT double Q_StdOptImpliedStrike(
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_OPT_STYLE		euroAmer,		// European or american style
	Q_OPT_ON		underType,		// Option on spot or futures
	Q_MARGIN		markToMkt,		// Daily mark to market or not
	double			spot,			// Underlyer spot price
	double			EstimatedStrike,// Estimated Strike
	double			vol,			// Option vol
	double			timeToExp,		// Time to option expiration, actual 365
	double			timeToSettle,	// Time to settlement of underlying
	double			ccRd,			// Domestic interest rate, continuous form
	double			ccRf,			// Foreign interest rate, continuous form
	double			price			// Option premium
	);

#endif

