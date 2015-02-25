/* $Header: /home/cvs/src/quant/src/q_binlkb.h,v 1.7 2000/03/02 13:32:53 goodfj Exp $ */
/****************************************************************
**
**	Q_BINLKB.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_Q_BINLKB_H )
#define IN_Q_BINLKB_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QRARE double Q_LBBT (
    Q_OPT_TYPE       callPut,       // call vs minimum or put vs maximum
	Q_OPT_STYLE      style1,        // exercise or not during lookback period 
	Q_OPT_STYLE      style2,        // exercise or not after lookback period
	double           spot,          // current spot price
	double           currMinMax,    // current min/max
	double           vol1,          // volatility to strike fix date
	double           vol2,          // volatility to expiration
	DATE             prcDate,       // pricing date
	DATE             fixDate,       // strike fixing date
	DATE             expDate,       // expiration date
	double           Rd1,           // domestic rate to strike fixing date
	double           Rd2,           // domestic rate to expiration
	double			 Rf1,           // foreign rate to strike fixing date
	double			 Rf2            // foreign rate to expiration
	);

EXPORT_C_QRARE double Q_BinLookBack (
    Q_OPT_TYPE       callPut,       // call vs minimum or put vs maximum
	Q_OPT_STYLE      style1,        // exercise or not during lookback period
	Q_OPT_STYLE      style2,        // exercise or not after lookback period
	double      	 spot,			// current spot
	double      	 currMinMax,	// current min or max
	double      	 stkDetTime,	// end of lookback period
	double      	 timeToExp,		// time to expiration of option
	double      	 shortVol,		// volatility to end of lookback period
	double      	 longVol,		// volatility to expiration
	double      	 shortDRate,	// domestic rate to end of lookback period
	double      	 longDRate,		// domestic rate to expiration
	double      	 shortFRate,	// foreign rate to end of lookback period
	double      	 longFRate,		// foreign rate to expiration
	int         	 nSteps,		// total number of timesteps
	int         	 *SampleBits    // vector of flags - 1 means a sampling point
	);

EXPORT_C_QRARE double Q_LBBTC (
	Q_OPT_TYPE  callPut,		// call vs minimum or put vs maximum
	Q_OPT_STYLE style1,		    // exercise or not during lookback period
	Q_OPT_STYLE style2,         // exercise or not after lookback period
	double      spot,			// current spot
	double      currMinMax,		// current min or max
	double      shortVol,		// volatility to end of lookback period
	double      longVol,		// volatility to expiration
	double      stkDetTime,		// end of lookback period
	double      timeToExp,		// time to expiration of option
	double      shortDRate,		// domestic rate to end of lookback period
	double      longDRate,		// domestic rate to expiration
	double      shortFRate,		// foreign rate to end of lookback period
	double      longFRate,		// foreign rate to expiration
	int         nSteps			// total number of timesteps
	);

#endif
