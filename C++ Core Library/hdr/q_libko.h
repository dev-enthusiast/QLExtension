/* $Header: /home/cvs/src/quant/src/q_libko.h,v 1.7 2000/03/02 13:32:54 goodfj Exp $ */
/****************************************************************
**
**	Q_LIBKO.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_Q_LIBKO_H )
#define IN_Q_LIBKO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QRARE double Q_TrigLib (
	Q_OPT_TYPE       callPut,    // option type
	Q_UP_DOWN        upOrDown,   // direction of knockout
	Q_KO_TYPE        koType,     // ending or continuous knockout
	double           spot,       // current spot price
	double           strike,     // strike price on currency
	double  		 knockout,   // knockout level on domestic LIBOR
	double           rebate,     // rebate paid upon knockout
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

EXPORT_C_QRARE double Q_TrigLibSprdMC2 (
	Q_OPT_TYPE       callPut,    // option type
	Q_UP_DOWN        upOrDown,   // direction of knockout
	double           spot,       // current spot price
	double           strike,     // strike price on currency
	double  		 knockout,   // knockout level on LIBOR spread
	double           rebate,     // rebate paid upon knockout
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

EXPORT_C_QRARE double Q_TrigLibKOFX (
	Q_OPT_TYPE       callPut,    // option type
	Q_UP_DOWN        upOrDown,   // direction of knockout
	Q_KO_TYPE        koType,     // ending or continuous knockout
	double           spot,       // current spot price
	double           strike,     // strike price on currency
	double  		 knockout,   // knockout level on domestic LIBOR
	double           rebate,     // rebate paid upon knockout
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
	
#endif
