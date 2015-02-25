/* $Header: /home/cvs/src/quant/src/q_impvol.h,v 1.4 1999/12/15 15:49:41 goodfj Exp $ */
/****************************************************************
**
**	Q_IMPVOL.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_Q_IMPVOL_H )
#define IN_Q_IMPVOL_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_OptImpVol (
	Q_OPT_TYPE	callPut,		// Option type (OPT_PUT or OPT_CALL)
	Q_OPT_STYLE	euroAmer,		// European or american style
	double  	spot,           // underlying spot price
	double		strike,			// Option strike price
	double		time,   		// Time to option expiration, actual 365
	double		ccRd,    		// Domestic interest rate
	double		ccRf, 	    	// Foreign interest rate
	double		price,			// Option premium
	double		EstimatedVol	// first guess for volatility
    );

EXPORT_C_QUANT double 	Q_StdOptImpliedVol(
			Q_OPT_TYPE		callPut,		// Option type (OPT_PUT or OPT_CALL)
			Q_OPT_STYLE 	euroAmer,		// European or american style
			Q_OPT_ON		spotFut,		// Option on spot or futures
			Q_MARGIN		markToMkt,		// Daily mark to market or not
			double			spot,			// Underlyer spot price
			double			strike,			// Option strike price
			double			timeToExp,		// Time to option expiration, actual 365
			double			timeToSettle,	// Time to settlement of underlying
			double			ccRd,			// Domestic interest rate, continuous form
			double			ccRf,			// Foreign interest rate, continuous form
			double			price,			// Option premium
			double			EstimatedVol	// first guess for volatility
			);

#endif

