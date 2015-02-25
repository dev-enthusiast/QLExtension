/* $Header: /home/cvs/src/quant/src/q_kogen.h,v 1.9 2000/03/06 15:28:58 goodfj Exp $ */
/****************************************************************
**
**	Q_KOGEN.H
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
****************************************************************/

#if !defined( IN_Q_KOGEN_H )
#define IN_Q_KOGEN_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_KOProbNAssetsNIntervalsMC(
	Q_UP_DOWN       upOrDown,                // direction of knockout
	double			KO,		                 // KO barrier level
    DATE            Today,                   // Pricing Date 
    Q_VECTOR        *Forwards,               // Forward prices of all the assets A[i] to times T[i]
    Q_VECTOR        *RollDates,              // Dates specifying endpoints of all the various T[i]
    Q_VECTOR        *VolsToRollDates,        // Term volatilities for all assets A[i] to times T[i]
    Q_VECTOR        *VolsToPrevRollDates,    // Term volatilities for all assets A[i] to times T[i-1] (first element should be zero)
	Q_VECTOR        *ConsecCorrsToRollDates, // Term correlations betweens assets A[i] and A[i+1] to times T[i] (last element should be zero)
	int				nSimulation		         // number of Monte Carlo simulations
    );

EXPORT_C_QKNOCK double Q_KOFwdNAssetsNIntervalsMC(
	Q_UP_DOWN       upOrDown,                // direction of knockout
    Q_IN_OUT        inOrOut,                 // appearing or disappearing
	double			KO,		                 // KO barrier level
    DATE            Today,                   // Pricing Date 
    Q_VECTOR        *Forwards,               // Forward prices of all the assets A[i] to times T[i]
    Q_VECTOR        *RollDates,              // Dates specifying endpoints of all the various T[i]
    Q_VECTOR        *VolsToRollDates,        // Term volatilities for all assets A[i] to times T[i]
    Q_VECTOR        *VolsToPrevRollDates,    // Term volatilities for all assets A[i] to times T[i-1] (first element should be zero)
	Q_VECTOR        *ConsecCorrsToRollDates, // Term correlations betweens assets A[i] and A[i+1] to times T[i] (last element should be zero)
	int				nSimulations	         // number of Monte Carlo simulations
);

EXPORT_C_QKNOCK double Q_KOOptionNAssetsNIntervalsMC(
	Q_UP_DOWN       upOrDown,                // direction of knockout
    Q_IN_OUT        inOrOut,                 // appearing or disappearing
    Q_OPT_TYPE      callPut,                 // call or put 
	double			KO,		                 // KO barrier level
    double          Strike,                  // strike
    DATE            Today,                   // Pricing Date 
    DATE            ExpSetDate,              // Expiration settlement date
    double          IR,                      // Domestic interest rate
    Q_VECTOR        *Forwards,               // Forward prices of all the assets A[i] to times T[i]
    Q_VECTOR        *RollDates,              // Dates specifying endpoints of all the various T[i]
    Q_VECTOR        *VolsToRollDates,        // Term volatilities for all assets A[i] to times T[i]
    Q_VECTOR        *VolsToPrevRollDates,    // Term volatilities for all assets A[i] to times T[i-1] (first element should be zero)
	Q_VECTOR        *ConsecCorrsToRollDates, // Term correlations betweens assets A[i] and A[i+1] to times T[i] (last element should be zero)
	int				nSimulations	         // number of Monte Carlo simulations
);
#endif

