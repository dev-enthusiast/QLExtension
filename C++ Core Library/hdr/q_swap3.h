/* $Header: /home/cvs/src/quant/src/q_swap3.h,v 1.36 2000/03/03 11:04:58 goodfj Exp $ */
/****************************************************************
**
**	Q_SWAP3.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.36 $
**
****************************************************************/

#if !defined( IN_Q_SWAP3_H )
#define IN_Q_SWAP3_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAMORT double Q_InvMult (
    Q_VECTOR        *Values,  //term structure of volatilities
    Q_VECTOR		*Weights  // vector of discount factors
	);

EXPORT_C_QAMORT double Q_EqualVar (
    Q_VECTOR        *Volatilities,  //term structure of volatilities
    Q_VECTOR		*VolTimes,	    // vector of tiumes for vols
    double          Term,           // amortization levels
    Q_VECTOR        *Times,         // Equal Variance Times
    int             nSteps     	    //number of steps
	);

EXPORT_C_QAMORT double Q_AmortSwapCN(
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	Q_VECTOR 		*strike,   		// one strike price	per payment date
	Q_VECTOR 		*Amounts,     	// notional amount per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    Q_VECTOR 		*vol,			// one volatility per payment date
    double          rangevol,       // volatility used to calculate the range of integration
    Q_VECTOR   		*AmortIndex,  	// indices of amortization dates in payment dates
    double          kotime,         // start of knockout period
    Q_VECTOR 		*times,	 		// payment times
    Q_VECTOR 		*Rd,     		// one domestic rate per payment time
    Q_VECTOR 		*fwds,     		// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

#endif

