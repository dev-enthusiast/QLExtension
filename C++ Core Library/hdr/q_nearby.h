/* $Header: /home/cvs/src/quant/src/q_nearby.h,v 1.11 2000/03/02 16:08:56 goodfj Exp $ */
/****************************************************************
**
**	Q_NEARBY.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.11 $
**
****************************************************************/

#if !defined( IN_Q_NEARBY_H )
#define IN_Q_NEARBY_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QMRM double Q_MROptANearDiv (
    Q_OPT_TYPE      callPut,    // option type
	double          strike,     // strike price of option
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            beginExer,  // beginning of exercise period
	DATE            expDate,    // expiration date
    Q_VECTOR        *grid,      // times to expiration for forward prices
    Q_VECTOR        *Rd,        // domestic rate vector
    Q_VECTOR        *RdTimes,   // times for domestic rates
    Q_VECTOR        *fwds,      // forward curve of oil
    Q_VECTOR        *divTimes,  // times when dividend is paid to holder
    double          dividend,   // fixed dividend 
    int             nGrid       // number of intervals in first dimension
    );

EXPORT_C_QMRM double Q_MROptANrKDisc (
    Q_OPT_TYPE      callPut,    // option type
	double          strike,     // strike price of option
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            beginExer,  // beginning of exercise period
	DATE            expDate,    // expiration date
    Q_VECTOR        *grid,      // times to expiration for forward prices
    Q_VECTOR        *Rd,        // domestic rate vector
    Q_VECTOR        *RdTimes,   // times for domestic rates
    Q_VECTOR        *fwds,      // forward curve of oil
    int             nGrid       // number of intervals in first dimension
    );

EXPORT_C_QMRM double Q_MRKOOptANearS (
    Q_OPT_TYPE      callPut,    // option type
    Q_UP_DOWN       upOrDown,   // direction of knockout
	double          strike,     // strike price of option
	double          outVal,     // knockout level
	double          rebate,     // rebate paid upon knockout
	Q_REBATE_TYPE   rebType,    // immediate or delayed rebate
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            beginExer,  // beginning of exercise period
	DATE            expDate,    // expiration date
    Q_VECTOR        *grid,      // times to expiration for forward prices
    Q_VECTOR        *Rd,        // domestic rate vector
    Q_VECTOR        *RdTimes,   // times for domestic rates
    Q_VECTOR        *fwds,      // forward curve of oil
    int             nNearby,    // option on this nearby contract
    int             nGrid       // number of intervals in first dimension
    );

EXPORT_C_QMRM double Q_OptANearby (
    Q_OPT_TYPE      callPut,    // option type
	double          strike,     // strike price of option
	Q_VECTOR       	*expvol,    // volatility to expiration of futures
	Q_VECTOR        *nearvol,   // volatility to previous expiration of futures
	Q_VECTOR        *SwitchCor, // correlation of consecutive futures at switch
	DATE            prcDate,    // pricing date
	DATE            beginExer,  // beginning of exercise period
	DATE            expDate,    // expiration date
    Q_VECTOR        *grid,      // times to expiration for forward prices
    Q_VECTOR        *Rd,        // domestic rate vector
    Q_VECTOR        *RdTimes,   // times for domestic rates
    Q_VECTOR        *fwds,      // forward curve of oil
    int             nGrid       // number of intervals in first dimension
    );

EXPORT_C_QMRM double Q_MRPKONear (
    Q_OPT_TYPE      callPut,    // option type
    Q_UP_DOWN       upOrDown,   // direction of knockout
	double          strike,     // strike price of option
	double          outVal,     // knockout level
	double          rebate,     // rebate paid upon knockout
	Q_REBATE_TYPE   rebType,    // immediate or delayed rebate
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            beginKO,    // beginning of knockout period
	DATE            expDate,    // expiration date
    Q_VECTOR        *grid,      // times to expiration for forward prices
    Q_VECTOR        *Rd,        // domestic rate vector
    Q_VECTOR        *RdTimes,   // times for domestic rates
    Q_VECTOR        *fwds,      // forward curve of oil
    int             nNearby,    // option on this nearby contract
    int             nGrid       // number of intervals in first dimension
    );

EXPORT_C_QMRM double Q_MROptANearV (
    Q_OPT_TYPE      callPut,    // option type
	DT_CURVE        *strikes,   // strike curve
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            beginExer,  // beginning of exercise period
	DATE            expDate,    // expiration date
	DT_CURVE        *futures,   // futures curve
	DT_CURVE        *RdCurve,   // interest rate curve
    int             nGrid       // number of intervals in first dimension
    );
                        
#endif

