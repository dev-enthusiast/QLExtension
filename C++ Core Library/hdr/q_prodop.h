/* $Header: /home/cvs/src/quant/src/q_prodop.h,v 1.13 2000/03/23 13:48:20 goodfj Exp $ */
/****************************************************************
**
**	Q_PRODOP.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_Q_PRODOP_H )
#define IN_Q_PRODOP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_ProdOpt (
	Q_OPT_TYPE      callPut1,  // direction of first payoff
	Q_OPT_TYPE      callPut2,  // direction of second payoff
	double          spot1,     // spot of first asset
	double          spot2,     // spot of second asset
	double          strike1,   // first strike
	double          strike2,   // second strike
	double          vol1,      // first volatility
	double          vol2,      // second volatility
	double          corr,      // correlation between assets
	double          time,      // time to expiration
	double          ccRd,      // domestic rate
	double          ccRf1,     // yield on first asset
	double          ccRf2      // yield on second asset
	);
EXPORT_C_QKNOCK double Q_LiborProdOpt (
	Q_OPT_TYPE    	 callPut1,   // first payoff type
	Q_OPT_TYPE    	 callPut2,   // second payoff type
	double           spot,       // current spot price
	double           strike1,    // strike price on currency
	double  		 strike2,    // strike level on domestic LIBOR
	double           vol,        // currency volatility
	double           drvol,      // domestic rate volatility
	double           frvol,      // foreign rate volatility
	double           dcorr,      // correlation of domestic rate and exchange rate
	double           fcorr,      // correlation of foreign rate and exchange rate
	double           dfcorr,     // correlation of foreign rate and domestic rate
	double           time,       // time to expiration of option
	Q_VECTOR         *ccRd,      // domestic discount rate curve
	Q_VECTOR         *ccRf,      // foreign discount rate curve
	Q_VECTOR         *dtimes,    // gridpoints for domestic rate curves
	Q_VECTOR         *ftimes,    // gridpoints for foreign rate curves
	int              nSteps,     // timesteps to expiration
	int              liborSteps, // number of timesteps in LIBOR term
	int              nPaths      // number of simulations
	);
	
EXPORT_C_QKNOCK double Q_ProdOptKO (
	Q_OPT_TYPE    	callPut1,  // first payoff type
	Q_OPT_TYPE    	callPut2,  // second payoff type
	Q_UP_DOWN       upOrDown,  // direction of knockout on second asset
	double          spot1,     // spot of first asset
	double          spot2,     // spot of second asset
	double          strike1,   // first strike
	double          strike2,   // second strike
	double          knockout,  // knockout on second asset
	double          vol1,      // first volatility
	double          vol2,      // second volatility
	double          corr,      // correlation between assets
	double          time,      // time to expiration
	double          ccRd,      // domestic rate
	double          ccRf1,     // yield on first asset
	double          ccRf2      // yield on second asset
	);
	
EXPORT_C_QKNOCK double Q_SquareKOOpt (
	Q_OPT_TYPE      callPut,   // direction of first payoff
	Q_UP_DOWN       upOrDown,  // direction of knockout
	double          spot,      // spot 
	double          strike,    // strike price
	double          knockout,  // knockout price
	double          vol,       // first volatility
	double          time,      // time to expiration
	double          ccRd,      // domestic rate
	double          ccRf       // yield on first asset
	);
	
EXPORT_C_QKNOCK double Q_ProdLibOpt (
    Q_OPT_TYPE       callPut1,   // option type for first asset
    Q_OPT_TYPE       callPut2,   // option type for LIBOR payoff
	double           spot,       // current spot price
	double           strike1,    // strike price on currency
	double  		 strike2,    // strike level on domestic LIBOR
	Q_VECTOR         *vols,		 // vector of spot, foreign & dom rate vols
	Q_VECTOR         *corrs,     // vector of correlations
	double           time,       // time to expiration of option
	Q_VECTOR         *Rd,		 // domestic simple discount rate curve
	Q_VECTOR         *Rf,        // foreign simple discount rate curve
	Q_VECTOR         *dtimes,    // gridpoints for domestic rate curves
	Q_VECTOR         *ftimes,    // gridpoints for foreign rate curves
	int              nSteps,     // timesteps to expiration
	int              liborSteps, // number of timesteps in LIBOR term
	int              nPaths      // number of simulations
	);
	
EXPORT_C_QKNOCK double Q_Square1PtKO (
	Q_OPT_TYPE      callPut,   // direction of first payoff
	Q_UP_DOWN       upOrDown,  // direction of knockout
	double          fwd1,      // forward price to knockout time
	double          fwd2,      // forward price to expiration
	double          strike,    // strike price
	double          knockout,  // knockout price
	double          vol1,      // volatility to knockout time
	double          vol2,      // volatility to expiration
	double          kotime,    // time to knockout date
	double          time,      // time to expiration
	double          ccRd       // domestic rate	to expiration
	);

EXPORT_C_QKNOCK double Q_Quadratic1PtKO (
	Q_OPT_TYPE      callPut1,  // direction of first payoff
	Q_OPT_TYPE      callPut2,  // direction of second payoff
	Q_UP_DOWN       upOrDown,  // direction of knockout
	double          fwd1,      // forward price to knockout time
	double          fwd2,      // forward price to expiration
	double          strike1,   // strike price for first payoff
	double          strike2,   // strike price for second payoff
	double          knockout,  // knockout price
	double          vol1,      // volatility to knockout time
	double          vol2,      // volatility to expiration
	double          kotime,    // time to knockout date
	double          time,      // time to expiration
	double          ccRd       // domestic rate	to expiration
	);
		
#endif
