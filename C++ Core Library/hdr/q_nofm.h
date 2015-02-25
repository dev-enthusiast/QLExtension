/* $Header: /home/cvs/src/quant/src/q_nofm.h,v 1.14 2000/03/02 15:01:56 goodfj Exp $ */
/****************************************************************
**
**	Q_NOFM.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.14 $
**
****************************************************************/

#if !defined( IN_Q_NOFM_H )
#define IN_Q_NOFM_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QEXOTIC double Q_OptNOutOfM (
	Q_OPT_TYPE        callPut, 		// type of all the options
	Q_VECTOR          *fut,   		// futures vector
	Q_VECTOR          *strike, 		// strike vector
	Q_VECTOR          *svol,   		// instantaneous spot vol vector
	double            beta,         // mean reversion parameter
	double            longVol,      // long volatility
	double            corr,         // correlation of short and long
	DATE              prcDate,      // pricing date
	Q_DATE_VECTOR     *startdate,   // begin average dates
	Q_DATE_VECTOR     *expdate,     // expiration dates	for options
	Q_DATE_VECTOR     *futexp,      // expiration dates for futures	
	Q_VECTOR          *rd,          // domestic rate vector
	int               nExer,        // number of options one can exercise
	int               ngrid         // number of gridpoints in integration
	);

EXPORT_C_QEXOTIC double Q_MRMOptNOfM (
	Q_OPT_TYPE        callPut, 		// type of all the options
	double            spot,         // current spot price of the metal
	Q_VECTOR          *fwds,   		// forward curve of the metal
	Q_VECTOR          *strike, 		// strike vector
	Q_VECTOR          *svol,   		// instantaneous spot vol vector
	Q_DATE_VECTOR	  *volexp,      // dates for volatilities
	double            beta,         // mean reversion parameter
	double            longVol,      // long volatility
	double            corr,         // correlation of short and long
	DATE              prcDate,      // pricing date
	Q_DATE_VECTOR     *startdate,   // begin average dates
	Q_DATE_VECTOR     *expdate,     // expiration dates	for options
	Q_DATE_VECTOR     *settle,      // expiration settlement dates for options
	Q_DATE_VECTOR     *futexp,      // expiration dates for forward curve
	Q_VECTOR          *rd,          // domestic rate vector to expirations
	int               nExer,        // number of options one can exercise
	int               nGrid         // number of gridpoints in integration
	);
	
EXPORT_C_QEXOTIC double Q_OptNOfMGen (
	Q_OPT_TYPE        callPut, 		// type of all the options
	Q_VECTOR          *fwds,   		// forwards for all the assets
	Q_VECTOR          *strike, 		// strike vector
	Q_VECTOR          *adjvol, 		// vols of the underlying assets
	Q_VECTOR		  *rho,         // correlations of consecutive assets
	DATE              prcDate,      // pricing date
	Q_DATE_VECTOR     *expdate,     // expiration dates	for options
	Q_DATE_VECTOR     *settle,      // expiration settlement dates for options
	Q_VECTOR          *rd,          // domestic rate vector to expirations
	int               nExer,        // number of options one can exercise
	int               nGrid         // number of gridpoints in integration
	);

EXPORT_C_QEXOTIC double Q_SelectAvg  (
	Q_OPT_TYPE      callPut,         // option type
	DT_CURVE        *fwds,           // forwards at the averaging points, plus dates
	double          strike,          // strike price
	Q_VECTOR        *vols,           // volatility vector at the averaging points
	DATE            prcDate,         // pricing date
	double          ccRd,            // domestic rate
	int             nExer,           // number of points permitted in average
	int             nGrid            // number of gridpoints
	);

EXPORT_C_QEXOTIC double Q_OptNOfMGenQ (
	Q_OPT_TYPE        callPut, 		// type of all the options
	Q_VECTOR          *fwds,   		// forwards for all the assets
	Q_VECTOR          *strike, 		// strike vector
	Q_VECTOR          *quantity, 	// quantity vector
	Q_VECTOR          *adjvol, 		// vols of the underlying assets
	Q_VECTOR		  *rho,         // correlations of consecutive assets
	DATE              prcDate,      // pricing date
	Q_DATE_VECTOR     *expdate,     // expiration dates	for options
	Q_DATE_VECTOR     *settle,      // expiration settlement dates for options
	Q_VECTOR          *rd,          // domestic rate vector to settlement dates
	int               nExer,        // number of options one can exercise
	int               nGrid         // number of gridpoints in integration
	);

EXPORT_C_QEXOTIC double Q_OptNOfMGenFee (
	Q_OPT_TYPE        callPut, 		// type of all the options
	Q_VECTOR          *fwds,   		// forwards for all the assets
	Q_VECTOR          *strike, 		// strike vector
	Q_VECTOR          *fee,         // fees for not exercising
	Q_VECTOR          *adjvol, 		// vols of the underlying assets
	Q_VECTOR		  *rho,         // correlations of consecutive assets
	DATE              prcDate,      // pricing date
	Q_DATE_VECTOR     *expdate,     // expiration dates	for options
	Q_DATE_VECTOR     *settle,      // expiration settlement dates for options
	Q_VECTOR          *rd,          // domestic rate vector to settlement dates
	int               nExer,        // number of options one can exercise
	int               nGrid         // number of gridpoints in integration
	);

EXPORT_C_QEXOTIC double Q_SilverBullet (
	Q_VECTOR          *types, 		// type of all the options
	DT_CURVE          *futures,  	// futures curve
	DT_CURVE          *strikes, 	// strikes with expiration dates
	Q_VECTOR          *svol,   		// instantaneous spot vol vector
	double            beta,         // mean reversion parameter
	double            longVol,      // long volatility
	double            corr,         // correlation of short and long
	DATE              prcDate,      // pricing date
	Q_VECTOR          *quantity1,   // quantity of first nearby
	Q_VECTOR          *quantity2,   // quantity of second nearby
	Q_VECTOR          *rd,          // domestic rate vector
	int               nExer,        // number of options one can exercise
	int               nGrid1,       // number of gridpoints short (nearby) dimension
	int               nGrid2        // number of gridpoints in long dimension
	);

EXPORT_C_QEXOTIC double Q_RoyaltyCN (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          PayoffCap,      // cap on total payouts
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
    DT_CURVE 		*fwdCurve,   	// forward curve
	int             nSteps, 		// number of timesteps
	int             nBuckets,       // number of buckets of possible amounts left
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QEXOTIC double Q_RoyaltyAMC (
	Q_OPT_TYPE      callPut,        // option type on each payment
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // vector of amounts on each pay date
	double          PayoffCap,      // cap on total payouts
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve,   	// one forward price per payment time
	int          	nPaths,  		// number of timesteps
	Q_RETURN_TYPE   retType         // return premium or error
    );
    					
#endif

