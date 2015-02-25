/* $Header: /home/cvs/src/quant/src/q_cox.h,v 1.19 2000/03/02 15:01:56 goodfj Exp $ */
/****************************************************************
**
**	Q_COX.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.19 $
**
****************************************************************/

#if !defined( IN_Q_COX_H )
#define IN_Q_COX_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#define COX_ROSS_DEFAULT_STEPS	50

EXPORT_C_QEXOTIC void Q_CoxRossRoyalty(
	Q_OPT_TYPE  optfwd,     // CallPut Fwd
    double		spot,		// Underlyer spot price
	double		strike,		// Option strike price
	double		vol,		// Underlyer volatility
	Q_VECTOR	*times,		// Times to expirations
	Q_VECTOR	*amounts,	// Amount at each expiration
	double		payLimit,	// payout limt of option
	double		riskFree,	// First interest rate
	double		divRate,	// Second interest rate
	int			useAverage,	// If so, average payments between periods
	int			nSteps,		// Number of discete steps
	int			nQuants,	// Quantization of Balance
	double		*premRet,	// Returned option premium
	double		*deltaRet	// Returned option delta
);

EXPORT_C_QEXOTIC double Q_Royalty(
	Q_OPT_TYPE      optfwd,     // CallPutFwd	
	double			spot,		// Underlyer spot price
	double			strike,		// Option strike price
	double			vol,		// Underlyer volatility
	Q_VECTOR		*times,		// Times to expirations
	Q_VECTOR		*amounts,	// Amount at each expiration
	double			payLimit,	// payout limit of option
	double			riskFree,	// First interest rate
	double			divRate,	// Second interest rate
	int				useAverage,	// If so, average payments between periods
	int				nSteps,		// Number of discete steps
	int				nQuants,	// Quantization of Balance
	Q_RETURN_TYPE	retType     // return premium or delta
	);

EXPORT_C_QEXOTIC double Q_RoyaltyMC (
	Q_OPT_TYPE      optfwd,     // CallPutFwd	
	double		spot,		// Underlyer spot price
	double		strike,		// Option strike price
	double		vol,		// Underlyer volatility
	Q_VECTOR	*times,		// Times to expirations
	Q_VECTOR	*amounts,	// Amount at each expiration
	double		payLimit,	// payout limt of option
	double		riskFree,	// First interest rate
	double		divRate,	// Second interest rate
	int			useAverage,	// If so, average payments between periods
	int			nPaths		// Number of paths
	);

EXPORT_C_QEXOTIC double Q_OptMulti1Day (
	Q_OPT_TYPE        callPut,       // call or put
	Q_OPT_STYLE       euroAmer,      // euro or american within the day
	double            spot,          // current real-time spot price
	double            lastfix,       // last spot fix
	double            vol,           // volatility
	double            time,          // time to expiration
	double            ccRd,          // domestic interest rate
	double            ccRf,          // foreign interest rate
	int               nf,            // number of future fixes including today's
	int               fineSteps		 // number of fine steps per day
	);

EXPORT_C_QEXOTIC double Q_GroundHogDay2 (
	Q_OPT_TYPE        callPut,       // call or put
	Q_OPT_STYLE       euroAmer,      // euro or american within the day
	double            CurrSpot,      // current real-time spot price
	Q_VECTOR          *history,      // last M spot price end of day fixes
	double            vol,           // volatility
	double            time,          // time to expiration
	double            ccRd,          // domestic interest rate
	double            ccRf,          // foreign interest rate
	double            eps,           // if spot < eps*avg we can exercise
	int               nf,            // number of future fixes including today's spot
	int               fineSteps,	 // number of fine steps per day
	int               nPaths         // number of paths in Monte Carlo run
	);
							
#endif

