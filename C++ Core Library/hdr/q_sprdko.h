/* $Header: /home/cvs/src/quant/src/q_sprdko.h,v 1.15 2000/04/03 17:26:46 goodfj Exp $ */
/****************************************************************
**
**	Q_SPRDKO.H	header file for q_sprdko.c
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.15 $
**
****************************************************************/

#if !defined( IN_Q_SPRDKO_H )
#define IN_Q_SPRDKO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QRARE int Q_LiborSpreadKO (
	Q_OPT_TYPE    callPut,         // option type
	Q_UP_DOWN     upOrDown,        // direction of knockout
	Q_KO_TYPE     koType,          // ending or continuous knockout
	Q_IN_OUT      inOrOut,         // knockin or knockout
	double        spot,            // current spot price
	double        strike,          // strike on currency option
	double        outVal,          // knockout level on LIBOR spread
	double        rebate,          // rebate paid upon knockout
    double        vol,             // spot volatiiity
    double        carryVol,        // carry volatility
    double        corr,            // correlation 
    double        time,            // time to expiration
    Q_VECTOR      *times,          // vector of gridpoints
    Q_VECTOR      *ccRd,           // vector of domestic rates
    Q_VECTOR      *fwds,           // vector of forward prices
    int           nSteps,          // number of timesteps
    int           liborSteps,      // number of steps in LIBOR term
    int			  npaths,          // number of trials for Monte Carlo run
    double        *premium,        // returned premium
    double        *delta           // returned delta
	);
		
EXPORT_C_QRARE double Q_SprdKOFEM(
	Q_OPT_TYPE  callPut,         // option type
	Q_UP_DOWN   upOrDown,        // direction of knockout
	double 		Spot1,			 // Initial value of first security S1
	double 	    Spot2,			 // Initial value of second security S2
	double	    Strike,			 // Strike
	double      Knockout,		 // Knockout
	double      Vol1,			 // Value of first volatility	
	double      Vol2,			 // Value of second volatility
	double      Rho,		 	 // Correlation coefficient between the two securities
 	double      ExpirationTime,	 // Time to Expiration
	double      Rd,				 // Domestic rate of interest
	double      Rf1,		 	 // Interest Rate for S1
	double      Rf2				 // Interest Rate for S2
);

#endif
