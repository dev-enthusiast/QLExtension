/****************************************************************
**
**	q_amortM.h	-  amort routines that return the whole matrix of 
**                   prices spanning both time and spot grid      
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_Q_AMORTM_H )
#define IN_Q_AMORTM_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAMORT int Q_AmortOptZeroLagMat (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double          minSpot,        // smallest Spot for which we need accurate value
	double          maxSpot,        // largest Spot for which we need accurate value
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    Q_DATE_VECTOR   *AmortLag,      // dates when amortizations kick in
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
	DT_CURVE		*ForeignDisc, 	// Foreign currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_VECTOR		*SpotVec,		// returned spot array
	int				*SpotVecIndex,	// index of the first non-trivial element of the returned arrays
    Q_VECTOR		**PriceMat,		// returned option price matrix, includes values in space and time directions
	Q_VECTOR		*TimeStamp		// returned time markings corresponding to prices vector 
	);

#endif



