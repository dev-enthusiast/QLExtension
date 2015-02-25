/* $Header: /home/cvs/src/quant/src/q_swap4.h,v 1.25 2000/05/05 23:36:40 rozena Exp $ */
/****************************************************************
**
**	Q_SWAP4.H	
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.25 $
**
****************************************************************/

#if !defined( IN_Q_SWAP4_H )
#define IN_Q_SWAP4_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QAMORT double Q_AmortOptACN(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
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
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AmortSwapKOD(
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // Number of ounces for each payment
	double          lowerKO,        // lower knockout level
	double          upperKO,        // upper knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization dates
    DATE            lowerkoDate,    // start of lower knockout period
    DATE            upperkoDate,	// start of upper knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );
           
EXPORT_C_QAMORT double Q_AmortSwapACN(
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
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
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AccreteFwd(
	Q_UP_DOWN       upOrDown,       // knockin direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	Q_VECTOR        *LeaseAdj,      // constant lease rate adjustments to strike
	double          outVal,         // knockout level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QAMORT double Q_AmortSwapACNIKO(
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
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
    DATE            koDate,         // start of knockout period
    DATE            endko,          // end of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

int Q_AdjustGrid (
	int          *gridsize,    // ingoing gridsize - gets adjusted
	double       *deltax,      // ingoing spacing - gets adjusted
	Q_VECTOR     *LowPrice,    // low amortization levels	
	Q_VECTOR     *HighPrice,   // high amortization levels	
	double       *lowlimit,    // lower limit - gets adjusted	
	double       *highlimit,   // upper limit - gets adjusted	
	double       outVal        // knockout level	
	);	
            		            		
EXPORT_C_QAMORT int Q_AmortSwapKODWVec(
	double 			maxSpot,	 	// the largest spot of the range
	double 			minSpot,	 	// the smallest spot of the range
    DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // Number of ounces for each payment
	double          lowerKO,        // lower knockout level
	double          upperKO,        // upper knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization dates
    DATE            lowerkoDate,    // start of lower knockout period
    DATE            upperkoDate,	// start of upper knockout period
    DATE            endko,          // end of knockout periods
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
    DT_CURVE 		*ForeignDisc,  	// foreign currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_VECTOR 		*SpotVec ,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
    );
	  
EXPORT_C_QAMORT int Q_AmortSwapKODVec(
	double 			maxSpot,	 	// the largest spot of the range
	double 			minSpot,	 	// the smallest spot of the range
    DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // Number of ounces for each payment
	double          lowerKO,        // lower knockout level
	double          upperKO,        // upper knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          rangevol,       // volatility used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization dates
    DATE            lowerkoDate,    // start of lower knockout period
    DATE            upperkoDate,	// start of upper knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
    DT_CURVE 		*ForeignDisc,  	// foreign currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_VECTOR 		*SpotVec ,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
    );

EXPORT_C_QAMORT int Q_AmortSwapACNVec(
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			maxSpot,	 	// the largest spot of the range
	double 			minSpot,	 	// the smallest spot of the range
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
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*DomesticDisc,  // domestic discount curve
    DT_CURVE 		*ForeignDisc,  	// foreign currency discount curve
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	Q_VECTOR 		*SpotVec ,		// returned array of spots
	Q_VECTOR		*OptionPrice	// returned array of option prices
    );

#endif

