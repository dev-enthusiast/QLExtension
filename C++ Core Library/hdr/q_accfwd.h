/* $Header: /home/cvs/src/quant/src/q_accfwd.h,v 1.5 2000/03/02 16:08:54 goodfj Exp $ */
/****************************************************************
**
**	Q_ACCFWD.H - routines to handle accumulating forwards, possibly	
**				 with mean reversion	
**	
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_Q_ACCFWD_H )
#define IN_Q_ACCFWD_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QFADE double Q_MRAccAvgFwd (
	Q_UP_DOWN       upOrDown,       // direction of trigger
	Q_VECTOR       	*avgfwds,       // forwards used for averaging
	Q_VECTOR        *fadefwds,      // forwards used for accumulating
	double          strike,         // strike price
	double          outVal,         // fade level	
	double          beta,           // mean reversion coefficient
	double          corr,           // correlation between short and long	
	double          longVol,        // long volatility
	Q_VECTOR        *svol,          // instantaneous piece-wise constant spot vols
	Q_VECTOR        *nodes,         // times for volatility curves
	Q_VECTOR        *times,         // times when fading can take place
	Q_VECTOR		*avgTimes, 		// times when forwards are averaged
	double          term,           // term of triggering future
	double          ccRd,           // risk-free rate
    int             initial,        // initial group - computed exactly
    int             bundle          // later groups are this size        
	);

EXPORT_C_QFADE double Q_MROptAccFwd (
	Q_OPT_TYPE      callPut,        // call or put
	double          optStrk,        // strike on option
	Q_VECTOR       	*fut,           // forward curve for oil
	double          strike,         // strike price	on forwards
	double          outVal,         // downward fade level	
	double          beta,           // mean reversion coefficient
	double          corr,           // correlation between short and long	
	double          longVol,        // long volatility
	Q_VECTOR        *svol,          // instantaneous piece-wise constant spot vols
	DATE            prcDate,        // pricing date
	DATE            exerdate,       // expiration date of option
	DATE            startdate,      // beginning of accumulator
	DATE            enddate,        // end of accumulator
	Q_DATE_VECTOR   *futexp,        // expiration dates of futures
	Q_VECTOR		*Rd,            // domestic rate vector
	Q_VECTOR		*rdT            // domestic rate time vector
	);

EXPORT_C_QFADE double Q_MROptAccFwdMC (
	Q_OPT_TYPE      callPut,        // call or put
	double          optStrk,        // strike on option
	Q_VECTOR       	*fut,           // forward curve for oil
	double          strike,         // strike price	on forwards
	double          outVal,         // downward fade level	
	double          beta,           // mean reversion coefficient
	double          corr,           // correlation between short and long	
	double          longVol,        // long volatility
	Q_VECTOR        *svol,          // instantaneous piece-wise constant spot vols
	DATE            prcDate,        // pricing date
	DATE            exerdate,       // expiration date of option
	DATE            startdate,      // beginning of accumulator
	DATE            enddate,        // end of accumulator
	Q_DATE_VECTOR   *futexp,        // expiration dates of futures
	Q_VECTOR		*Rd,            // domestic rate vector
	Q_VECTOR		*rdT,           // domestic rate time vector
	int             nTrials         // number of trials
	);
		
#endif

