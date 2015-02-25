/* $Header: /home/cvs/src/quant/src/q_oilopt.h,v 1.28 2000/03/02 16:08:57 goodfj Exp $ */
/****************************************************************
**
**	Q_OILOPT.H  routines to evaluate American options on rolling
**				first nearby futures or spot oil.  Uses a 2-factor
**				finite difference model which is designed to be
**				consistent with Jim's volatility model and
**				Gabillon's model of oil term structure.
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.28 $
**
****************************************************************/

#if !defined( IN_Q_OILOPT_H )
#define IN_Q_OILOPT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QMRM double Q_InterpA (
	Q_VECTOR            *fwds,     // curve forward (futures) prices for oil
	Q_VECTOR            *times,    // times to expirations for futures
	double              spot,      // current cash price of oil
	double              Long,      // current very long-term price of oil
	double              beta,      // mean reversion coefficient
	double              time       // time we are interpolating to
	);

EXPORT_C_QMRM double Q_GetFwd (
	Q_VECTOR            *fwds,     // curve forward (futures) prices for oil
	Q_VECTOR            *times,    // times to expirations for futures
	double              spot,      // current cash price of oil
	double              Long,      // current very long-term price of oil
	double              beta,      // mean reversion coefficient
	double              time       // time we are interpolating to
	);

EXPORT_C_QMRM double Q_IntegrateVar (
	Q_VECTOR           *spotVol,  // instantaneous vol between futures dates
	double             longVol,   // volatility of long futures price
	double             corr,      // instantaneous correlation
	double             beta,      // mean reversion coefficient
	Q_VECTOR		   *times,    // times to expirations for futures
	double             time       // time to integrate to
	);

EXPORT_C_QMRM double Q_GetFwdA (
	Q_VECTOR            *fwds,     // curve forward (futures) prices for oil
	Q_VECTOR            *times,    // times to expirations for futures
	double              spot,      // current cash price of oil
	double              Long,      // current very long-term price of oil
	Q_VECTOR            *spotVol,  // instantaneous vol between futures dates
	double              longVol,   // volatility of long futures price
	double              corr,      // instantaneous correlation
	double              beta,      // mean reversion coefficient
	double              t,         // start of term for A function
	double              T          // end of term for A function
	);

EXPORT_C_QMRM int Q_GetDeltaVector (
	Q_VECTOR            *fwds,     // curve forward (futures) prices for oil
	Q_VECTOR            *times,    // times to expirations for futures
	double              spot,      // current cash price of oil
	double              Long,      // current very long-term price of oil
	Q_VECTOR            *spotVol,  // instantaneous vol between futures dates
	double              longVol,   // volatility of long futures price
	double              corr,      // instantaneous correlation
	double              beta,      // mean reversion coefficient
	double              ccRd,      // discount rate
	double              T,         // end of term
	int                 nSteps,    // number of steps
	Q_VECTOR            *delta     // returned delta vector
	);

EXPORT_C_QMRM double Q_MROptNear (
    Q_OPT_TYPE      callput,    // call or put
	double          strike,     // strike price of option
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
    Q_VECTOR        *grid,      // expiration dates for forward prices
    double          Rd,  		// domestic rate
    Q_VECTOR        *fwds       // forward curve of oil
    );

EXPORT_C_QMRM double Q_EuroOilOption (
    Q_OPT_TYPE      callPut,    // option type
	double          strike,     // strike price of option
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
    DATE            *fwdDate,   // expiration dates for forward prices
    double          ccRd,       // domestic rate
    Q_VECTOR        *fwds       // forward curve of oil
    );

EXPORT_C_QMRM double Q_MROptFut (
    Q_OPT_TYPE      callPut,    // option type
    Q_OPT_STYLE     style,      // option style
	double          strike,     // strike price of option
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
    Q_VECTOR        *grid,      // expiration dates for forward prices
    double          Rd,  		// domestic rate
    Q_VECTOR        *fwds,      // forward curve of oil
    int             contract	// index of contract
    );

EXPORT_C_QMRM double Q_NearOption (
    Q_OPT_TYPE      callPut,    // option type
    Q_OPT_STYLE     amerEuro,   // option style
	double          strike,     // strike price of option
	Q_VECTOR       	*vol1,      // vector of term volatilities
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
    DATE            *fwdDate,   // expiration dates for forward prices
    double          ccRd,       // domestic rate
    Q_VECTOR        *fwds,      // forward curve of oil
    int             contract    // the contract the option is on
    );
EXPORT_C_QMRM double Q_MRKOANear (
    Q_OPT_TYPE      callput,    // call or put
	Q_OPT_STYLE     amereuro,   // American or European option
    Q_UP_DOWN       upOrDown,   // direction of knockout
	double          spot,    	// current	spot price of oil (<0 => american)
	double          Long,   	// very long futures price of oil
	double          strike,     // strike price of option
	double          outVal,     // knockout price
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
    Q_VECTOR        *KODates,   // vector of dates on which option can knock out
    Q_VECTOR        *grid,      // expiration dates for forward prices
    double          Rd,  		// domestic rate
    Q_VECTOR        *fwds,      // forward curve of oil
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2		// number of intervals in second dimension
    );

EXPORT_C_QMRM double Q_KOOilOption (
    Q_OPT_TYPE      callPut,    // option type
    Q_OPT_STYLE     amerEuro,   // option style
    Q_UP_DOWN       upOrDown,   // direction of knockout
	double          spot,    	// current	spot price of oil
	double          Long,   	// very long futures price of oil
	double          strike,     // strike price of option
	double          outVal,     // knockout price
	Q_VECTOR       	*vol1,      // instantaneous spot vol vector
	double          vol2,       // volatility of long price
	double          corr,       // instantaneous correlation of spot and long
	double          beta,       // reversion of spot toward long price
	DATE            prcDate,    // pricing date
	DATE            expDate,    // expiration date
	DATE            *koDate,    // dates where the option can knock out
	int             nko,        // number of knockout dates
    DATE            *fwdDate,   // expiration dates for forward prices
    double          ccRd,       // domestic rate
    Q_VECTOR        *fwds,      // forward curve of oil
    int             nGrid1,     // number of intervals in first dimension
    int             nGrid2		// number of intervals in second dimension
    );
EXPORT_C_QMRM double Q_MROptANear4 (
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
EXPORT_C_QMRM double Q_OptPortOptOil (
	Q_OPT_TYPE   callPut,          // option type of compound option
	Q_VECTOR     *types,           // option type of underlying	as doubles
	double       strike,           // strike of compound option
	Q_VECTOR     *quantity,        // quantities of underlying options
	Q_VECTOR     *strikes,         // strikes of underlying options
	Q_VECTOR     *svol,  		   // piecewise constant spot volatility vector
	double       longvol,          // volatility of long-term price
	double       beta,             // mean reversion coefficient
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *grid,            // expiration dates of futures
	DATE         prcDate,          // today's date
	DATE         cmpded,           // time to exp of compound option
	Q_VECTOR     *startavg,        // starts of averaging for underlying options
	Q_VECTOR     *expdate,         // times to exp of underlying options
    Q_VECTOR     *rd,              // domestic rate vector at futures expirations
    Q_VECTOR     *fut              // futures price vector
	);

EXPORT_C_QMRM double Q_MRTimeBasket (
	Q_OPT_TYPE   callPut,          // option type of compound option
	Q_VECTOR	 *wts,			   // weights for basket of futures prices
	double       strike,           // strike price
	double       longVol,          // volatility of long-term price
	Q_VECTOR     *spotvol,         // instantaneous spot vol vector
	double       beta,             // mean reversion coefficient
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *times,           // times for set of futures prices
	Q_VECTOR     *index,           // which future for each element of basket
	Q_VECTOR     *grid,            // times to expiration of futures
	double       time,			   // time to expiration
	double       ccRd,   		   // discount rate
    Q_VECTOR     *fut              // futures price vector
	);
EXPORT_C_QMRM int Q_MRTimeBasketParms (
	Q_VECTOR	 *wts,			   // weights for basket of futures prices
	double       longVol,          // volatility of long-term price
	Q_VECTOR     *spotvol,         // instantaneous spot vol vector
	double       beta,             // mean reversion coefficient
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *times,           // times for set of futures prices
	Q_VECTOR     *index,           // which future for each element of basket
	Q_VECTOR     *grid,            // times to expiration of futures
	double       time,			   // time to expiration
	double       ccRd,   		   // discount rate
    Q_VECTOR     *fut,             // futures price vector
    double       *longfwd,         // returned long forward
    double       *shortfwd,        // returned short forward
    double       *longvol,		   // returned long volatility
    double       *shortvol,		   // returned short volatility
    double       *rho              // returned correlation
	);
EXPORT_C_QMRM double Q_OptPortOptOil3 (
	Q_OPT_TYPE   	callPut,     // option type of compound option
	Q_VECTOR     	*types,      // option type of underlying	as doubles
	double       	strike,      // strike of compound option
	Q_VECTOR     	*quantity,   // quantities of underlying options
	Q_VECTOR     	*strikes,    // strikes of underlying options
	Q_VECTOR     	*svol,  	 // piecewise constant spot volatility vector
	double       	longvol,     // volatility of long-term price
	double       	beta,        // mean reversion coefficient
	double       	corr,        // correlation of long and short prices
	Q_DATE_VECTOR   *grid,       // expiration dates of futures
	DATE         	prcDate,     // today's date
	DATE         	cmpded,      // time to exp of compound option
	Q_DATE_VECTOR   *startavg,   // starts of averaging for underlying options
	Q_DATE_VECTOR   *expdate,    // times to exp of underlying options
    Q_VECTOR     	*rd,         // domestic rate vector at futures expirations
    Q_VECTOR     	*fut,        // futures price vector
    int             npts 		 // number of nodes for integration
	);

EXPORT_C_QMRM double Q_GetFwdAV (
	Q_VECTOR            *fwds,     // curve forward (futures) prices for oil
	Q_VECTOR            *times,    // times to expirations for futures
	double              spot,      // current cash price of oil
	double              Long,      // current very Long-term price of oil
	Q_VECTOR            *spotVol,  // instantaneous vol between vol nodes
	Q_VECTOR            *volTimes, // nodes for volatilities
	double              LongVol,   // volatility of Long futures price
	double              corr,      // instantaneous correlation
	double              beta,      // mean reversion coefficient
	double              t,         // start of term for A function
	double              T          // end of term for A function
	);
					
#endif

