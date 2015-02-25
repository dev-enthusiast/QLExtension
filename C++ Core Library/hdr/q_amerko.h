/* $Header: /home/cvs/src/quant/src/q_amerko.h,v 1.9 2000/03/02 13:32:52 goodfj Exp $ */
/****************************************************************
**
**	Q_AMERKO.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_Q_AMERKO_H )
#define IN_Q_AMERKO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QRARE double Q_MR1KOA  (
	Q_OPT_TYPE     callPut,     // option type
	Q_OPT_STYLE    amerEuro,    // option style
	Q_UP_DOWN      upOrDown,	// direction of knockout
  	double         spot,        // current spot price
	double         strike,      // strike price
	double         outVal,      // knockout price
	double         rebate,      // rebate upon hitting knockout
	Q_REBATE_TYPE  rebType,     // immediate or delayed rebate
	Q_VECTOR       *vol,        // volatility curve
	double         beta,        // mean reversion coefficient
	double         exerTime,    // start exercise time, if American
	double         timeToExp,   // time to expiration
	Q_VECTOR       *times,      // gridpoints for the curves
	Q_VECTOR       *Rd,         // domestic interest rate curve
	Q_VECTOR       *fwd,        // forward price curve
	int            nsteps,      // number of timesteps
	int            gridsize     // fineness of grid for integration
    );

EXPORT_C_QRARE double Q_MR1KOADel  (
	Q_OPT_TYPE     callPut,     // option type
	Q_OPT_STYLE    amerEuro,    // option style
	Q_UP_DOWN      upOrDown,	// direction of knockout
	double         spot,        // current spot price
	double         strike,      // strike price
	double         outVal,      // knockout price
	double         rebate,      // rebate upon hitting knockout
	Q_REBATE_TYPE  rebType,     // immediate or delayed rebate
	Q_VECTOR       *vol,        // volatility curve
	double         beta,        // mean reversion coefficient
	double         exerTime,    // start exercise time, if American
	double         timeToExp,   // time to expiration
	Q_VECTOR       *times,      // gridpoints for the curves
	Q_VECTOR       *Rd,         // domestic interest rate curve
	Q_VECTOR       *fwd,        // forward price curve
	int            nsteps,      // number of timesteps
	int            gridsize     // fineness of grid for integration
    );

EXPORT_C_QRARE int Q_CurveOption1  (
	Q_OPT_TYPE     callPut,     // option type
	Q_OPT_STYLE    amerEuro,    // option style
	Q_UP_DOWN      upOrDown,	// direction of knockout
	double         spot,        // current spot price
	double         strike,      // strike price
	double         outVal,      // knockout price
	double         rebate,      // rebate upon hitting knockout
	Q_REBATE_TYPE  rebType,     // immediate or delayed rebate
	Q_VECTOR       *vol,        // volatility curve
	double         beta,        // mean reversion coefficient
	double         exerTime,    // start exercise time
	double         timeToExp,   // time to expiration
	Q_VECTOR       *times,      // gridpoints for the volatility curve
	Q_VECTOR       *dtimes,     // gridpoints for domestic rate curve
	Q_VECTOR       *ftimes,     // gridpoints for foreign rate curve
	Q_VECTOR       *Rd,         // domestic interest rate curve
	Q_VECTOR       *fwd,        // forward price curve
	int            nsteps,      // number of timesteps
	int            gridsize,    // fineness of grid for integration
	double         *prem,       // returned premium
	double         *adjdelta    // returned "adjusted" delta
	);

#endif

