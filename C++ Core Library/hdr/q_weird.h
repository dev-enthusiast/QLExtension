/* $Header: /home/cvs/src/quant/src/q_weird.h,v 1.68 2001/11/27 23:13:57 procmon Exp $ */
/****************************************************************
**
**	Q_WEIRD.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.68 $
**
****************************************************************/

#if !defined( IN_Q_WEIRD_H )
#define IN_Q_WEIRD_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QEXOTIC double Q_OptPortOpt(
	Q_OPT_TYPE   callPut,          // option type of compound option
	Q_VECTOR     *types,           // option type of underlying	as doubles
	double       spot,             // current spot price
	double       strike,           // strike of compound option
	double       lower,            // lower knockout during compound option
	double       upper,            // upper knockout during compound option
	Q_VECTOR     *quantity,        // quantities of underlying options
	Q_VECTOR     *strikes,         // strikes of underlying options
	Q_VECTOR     *lowerKO,         // lower knockouts of underlying options
	Q_VECTOR     *upperKO,         // upper knockouts of underlying options
	Q_VECTOR     *lRebate,         // lower rebates of underlying options
	Q_VECTOR     *uRebate,         // upper rebates of underlying options
	Q_VECTOR     *rebTypes,        // rebate types of underlying options
	Q_VECTOR     *vols,  		   // volatility curve
	Q_VECTOR     *volTimes,        // gridpoints for volatilities
	double       time,             // time to exp of compound option
	Q_VECTOR     *times,           // times to exp of underlying options
    Q_VECTOR     *ccRd,            // domestic rate vector
    Q_VECTOR     *RdTimes,         // gridpoints for domestic rates
    Q_VECTOR     *fwds,            // forward curve
    Q_VECTOR     *fwdTimes         // gridpoints for forward curve
);

EXPORT_C_QEXOTIC double Q_OptPortDKO(
	Q_OPT_TYPE   callPut,          // option type of compound option
	Q_VECTOR     *types,           // option type of underlying	as doubles
	double       spot,             // current spot price
	double       strike,           // strike of compound option
	double       lower,            // lower knockout during compound option
	double       upper,            // upper knockout during compound option
	Q_VECTOR     *quantity,        // quantities of underlying options
	Q_VECTOR     *strikes,         // strikes of underlying options
	Q_VECTOR     *lowerKO,         // lower knockouts of underlying options
	Q_VECTOR     *upperKO,         // upper knockouts of underlying options
	Q_VECTOR     *lRebate,         // lower rebates of underlying options
	Q_VECTOR     *uRebate,         // upper rebates of underlying options
	Q_VECTOR     *rebTypes,        // rebate types of underlying options
	Q_VECTOR     *vols,  		   // volatility curve
	Q_VECTOR     *volTimes,        // gridpoints for volatilities
	double       time,             // time to exp of compound option
	Q_VECTOR     *times,           // times to exp of underlying options
    Q_VECTOR     *ccRd,            // domestic rate vector
    Q_VECTOR     *RdTimes,         // gridpoints for domestic rates
    Q_VECTOR     *fwds,            // forward curve
    Q_VECTOR     *fwdTimes,        // gridpoints for forward curve
	double		 RangeVol,		   // vol for choosing the integration range
	double		 ReferenceSpot	   // spot for choosing the integration region
);

EXPORT_C_QEXOTIC double Q_OptPortBinOpt (
	Q_OPT_TYPE   callPut,          // option type of compound option
	Q_VECTOR     *types,           // option type of underlying	as doubles
	double       spot,             // current spot price
	double       strike,           // strike of compound option
	double       lower,            // lower knockout during compound option
	double       upper,            // upper knockout during compound option
	Q_VECTOR     *quantity,        // payoffs of underlying options
	Q_VECTOR     *strikes,         // strikes of underlying options
	Q_VECTOR     *lowerKO,         // lower knockouts of underlying options
	Q_VECTOR     *upperKO,         // upper knockouts of underlying options
	Q_VECTOR     *vols,  		   // volatility curve
	Q_VECTOR     *volTimes,        // gridpoints for volatilities
	double       time,             // time to exp of compound option
	Q_VECTOR     *times,           // times to exp of underlying options
    Q_VECTOR     *ccRd,            // domestic rate vector
    Q_VECTOR     *RdTimes,         // gridpoints for domestic rates
    Q_VECTOR     *fwds,            // forward curve
    Q_VECTOR     *fwdTimes         // gridpoints for forward curve
	);
	
EXPORT_C_QEXOTIC double Q_KODExpC (
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
    double      	lowknockout,   		// starting lower knockout level
    double      	lowerGrowth,        // growth rate on lower knockout
    double      	highknockout,  		// upper knockout level
    double      	upperGrowth,        // growth rate on lower knockout
    double      	lowerRebate,   		// rebate paid upon lower ko
    double      	upperRebate,   		// rebate paid upon higher ko
    Q_REBATE_TYPE	rebType,			// Immediate or delayed till exp date
    Q_VECTOR 		*vol,				// Underlyer volatility	curve
    Q_VECTOR 		*volT,      		// grid for volatilities
    double 			time,	 			// Time to expiration
    Q_VECTOR 		*Rd,     			// Domestic rate curve
    Q_VECTOR 		*rdT,       		// grid for domestic rate curve
    Q_VECTOR 		*fwds,     			// forward curve
    Q_VECTOR 		*fwdT,      		// grid for forward curve
	int          	nSteps, 			// number of timesteps
	int          	gridsize			// number of intervals in price space
    );

EXPORT_C_QEXOTIC double Q_OptPortOptS (
	Q_OPT_TYPE   callPut,          // option type of compound option
	Q_VECTOR     *types,           // option type of underlying	as doubles
	double       spot,             // current spot price
	double       strike,           // strike of compound option
	Q_VECTOR     *quantity,        // quantities of underlying options
	Q_VECTOR     *strikes,         // strikes of underlying options
	Q_VECTOR     *vols,  		   // volatility curve
	Q_VECTOR     *volTimes,        // gridpoints for volatilities
	double       time,             // time to exp of compound option
	Q_VECTOR     *times,           // times to exp of underlying options
    Q_VECTOR     *ccRd,            // domestic rate vector
    Q_VECTOR     *RdTimes,         // gridpoints for domestic rates
    Q_VECTOR     *fwds,            // forward curve
    Q_VECTOR     *fwdTimes,        // gridpoints for forward curve
	int			 npts,		       // number of points in final integral
	double		 RangeVol,		   // vol for choosing the integration range (= HUGE_VAL for no range vol)
	double		 ReferenceSpot	   // spot for choosing the integration region
	);

EXPORT_C_QEXOTIC double Q_FloatRateNoteA (
	double        spot,      // current dollar/mex spot
	double        outVal,    // upward knockout level to go to floating accrual
	double        Floor,     // floor on accrued cetes rate
	double        spread,    // subtract this to determine strike
	double        govt,		 // spread between mexican "LIBOR" and cetes
	double        liborsprd, // amount added to LIBOR in calculating strike
	double        kosprd,    // spread on floating rate after knockout
	Q_VECTOR      *vol,      // term exchange rate volatility vector
	double        sig,       // normalized standard deviation of mex rate
	double        rho,       // correlation of log(spot) and mex rate
	double        a,         // mean reversion coefficient
	Q_VECTOR      *Rdisc,    // vector of US log(discount factors)
	Q_VECTOR      *L,        // vector of Mex log(discount factors) starting at 0
	double        credit,    // credit spread
	int           nyears,    // number of years
	int           npaths,    // number of paths
	int           nmonths,   // number of months per period
	Q_RETURN_TYPE retType	 // return premium or error
	);

EXPORT_C_QEXOTIC double Q_FloatRateNoteB (
	double        spot,      // current dollar per mex spot
	double        outVal,    // upward knockout on mex/$
	double        Floor,     // floor on accrued cetes rate
	double        ratio,     // multiply by this to determine strike
	double        govt,		 // spread between mexican LIBOR and cetes
	double        liborsprd, // amount added to LIBOR in calculating strike
	double        kosprd,    // spread on floating rate after knockout
	Q_VECTOR      *vol,      // term exchange rate volatility vector
	double        sig,       // normalized standard deviation of mex rate
	double        rho,       // correlation of log(spot) and mex rate
	double        a,         // mean reversion coefficient
	Q_VECTOR      *Rdisc,    // vector of US log(discount factors)
	Q_VECTOR      *L,        // vector of Mex log(discount factors) starting at 0
	double        credit,    // credit spread
	int           nyears,    // number of years
	int           npaths,    // number of paths
	int           nmonths,   // number of months per period
	Q_RETURN_TYPE retType	 // return premium or error
	);

EXPORT_C_QEXOTIC double Q_OptPortOpt1Asset1TenorNaive(
	Q_OPT_TYPE   callPut,          // option type of compound option
	double       strike,           // strike of compound option
	double       spot,             // current spot price
    double       vol1,             // volatility to compound option
	Q_VECTOR     *quantity,        // quantities of underlying options
	Q_VECTOR     *types,           // option type of underlying	as doubles
	Q_VECTOR     *strikes,         // strikes of underlying options
	Q_VECTOR     *vols,  		   // volatilities of underlying options
	double       TimeCmpExp,       // time to exp of compound option
	double       TimeUndExp,       // time to exp of underlying option (vols are all to this date)
    double       TimeUndExpSet,    // time to settlement of underlying options 
 	double       RdCmpExp,         // domestic rate to exp of compound option
    double       RdUndExpSet,      // domestic rate to settlement of underlying options 
 	double       RfCmpExp,         // foreign rate to exp of compound option
    double       RfUndExpSet,      // foreign rate to settlement of underlying options 
    int          npts              // number of nodes for integration       
);

EXPORT_C_QEXOTIC double Q_AmortLiborGold3 (
	int           swapflag,     // do we include principal payments?
	int           leaseflag,    // do we include floating lease payments?
	int           liborflag,    // do we include floating libor payments?
	double        spot,      	// current dollar per gold spot
	Q_VECTOR      *strikes,  	// strike vector 
	double        outValL,      // lower knockout level
	double        outValU,      // upper knockout level
	double        fixedAdj,     // adjustment to fixed side (per annum)
	double        floatAdj,     // adjustment to floating side (per annum)
	double        floor,        // floor on libor interest 
	double        cap,          // cao on libor interest
    Q_VECTOR 	  *Amounts,     // One Amount per payment date
    Q_VECTOR      *LowPrice,    // low price for 100% amortization
    Q_VECTOR      *HighPrice,   // high price for 0% amortization
    Q_VECTOR      *LowPct,      // percentage at lower level
    Q_VECTOR      *HighPct,     // percentage at upper level
    int           koindex,      // payment where knockout starts
	Q_VECTOR      *vols,     	// term exchange rate volatility vector
	Q_VECTOR      *voltimes, 	// term exchange rate volatility grid vector
	double        sigr,       	// normalized standard deviation of domestic rate
	double        sigy,       	// normalized standard deviation of lease rate
	double        rhor,      	// correlation of log(spot) and domestic short rate
	double        rhoy,      	// correlation of log(spot) and lease rate
	double        rhory,      	// correlation of lease rate and domestic short rate
	double        ar,         	// mean reversion coefficient for domestic rate
	double        ay,         	// mean reversion coefficient for lease rate
	Q_VECTOR      *Rd,       	// domestic term discount factors	 
	Q_VECTOR      *RdT,      	// domestic term interest rate grid	 
	Q_VECTOR      *Rf,       	// gold term discount factors (lease rates)
	Q_VECTOR      *RfT,      	// gold term interest rate grid
	int           npds,  	  	// number of periods
	int           npaths,    	// number of paths
	int           nmonths,   	// number of months per period
	Q_RETURN_TYPE retType	 	// return premium or error
	);

EXPORT_C_QEXOTIC double Q_CmpdSpread(
	Q_OPT_TYPE   callPut,          // option type of compound option
	Q_OPT_TYPE   callPut1,         // option type of first underlying option
	Q_OPT_TYPE   callPut2,         // option type of second underlying option
	Q_UP_DOWN    upOrDown,         // direction of knockout
	double       spot,             // current spot price
	double       strike,           // strike of compound option
	double       quantity1,        // quantity of first underlying option
	double       quantity2,        // quantity of second underlying option
	double       strike1,          // strike of first underlying option
	double       strike2,          // strike of first underlying option
	double       knockout,         // knockout of second underlying option
	double       vol,              // volatility to time
	double       vol1,  		   // volatility to time1
	double       vol2,             // volatility to time2
	double       time,             // time to exp of compound option
	double       time1,            // time to exp of first underlying option
	double       time2,            // time to exp of second underlying option
	double       ccRd,             // domestic rate to time
	double       ccRd1,            // domestic rate to time1
	double       ccRd2,            // domestic rate to time2
	double       ccRf,             // foreign rate to time,
	double       ccRf1,            // foreign rate to time1
	double       ccRf2             // foreign rate to time2
);


EXPORT_C_QEXOTIC int Q_OTHeston1DApprox(
	Q_UP_DOWN       upOrDown,       	// knockout direction
	Q_REBATE_TYPE	RebateType,			// Immediate or delayed till exp date
	double          RangeSpot,        	// Good values are computed between this value and the barrier 
	double 			OutVal,				// Barrier
	double			vol_0,				// initial vol
	double			RangeVol,			// volatility used to fix the grid size
	double			Xi,					// vol of var
	double 			Rho,				// correlation between var and spot processes
	double			Phi,				// stochastic term of var is proportional to Var^Phi
	double			Beta,				// mean reverting vol coefficient
	DATE            prcDate,        	// pricing date
	DATE			expDate,			// expiry
	DT_CURVE 		*MeanVols,			// Mean reverting vols
	DT_CURVE 		*DiscCrv,  			// domestic discount curve
	DT_CURVE		*ForeignDisc, 		// Foreign currency discount curve
	int				DepthOfIter,		// iteration depth
	double			varShock,			// initial var diddle for the derivatives
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of grid points
	Q_VECTOR		*SpotVec,			// spot grid, input and output
	Q_VECTOR		*PriceVec			// prices grid, input and output
);


EXPORT_C_QEXOTIC int Q_HestonPhiDiffuseA(
	DATE            StartDiff,		// start of diffusion period
	DATE			EndDiff,		// end of diffusion period
	DT_CURVE 		*DiscCrv,  		// domestic discount curve
	DT_CURVE		*ForeignDisc, 	// Foreign currency discount curve
	DT_CURVE 		*MeanVols,		// Mean reverting vols
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			Beta,			// mean reversion strength
	double			Xi,				// vol of vol
	double 			Rho,   			// correlation between var and spot processes
	double			Phi,			// stoch term in SDE for v (==inst vol^2) is proportional to v^Phi
	int				DepthOfIter,	// depth of iteration in var direction
	Q_REBATE_TYPE	RebateType,		// Immediate or delayed till exp date
	Q_BARRIER_TYPE	BarrierType,	// single/double/no barrier
	double			LowerRebate,	// 
	double			UpperRebate,	// 
	int          	nSteps, 		// number of timesteps
	double			DeltaX,			// grid spacing, log of spot
	Q_VECTOR		*PriceVec,		// prices grid, input and output
	double			varDiddle
);

	  
EXPORT_C_QEXOTIC int Q_HigginsDiffuseApprox(
	DATE            StartDiff,		// start of diffusion period
	DATE			EndDiff,		// end of diffusion period
	DT_CURVE 		*DiscCrv,  		// domestic discount curve
	DT_CURVE		*ForeignDisc, 	// Foreign currency discount curve
	DT_CURVE 		*MeanVols,		// Mean reverting vols
	double			InitVol,		// current instantaneous volatility	(sqrt of current vol^2)
	double			Beta,			// mean reversion strength
	double			Gamma,			// mean reversion strength of "z", the transformed spot/vol correlation
	double			Sigma,			// vol of vol
	double			Xi,				// the "volatility" of the transformed spot/vol correlation
	double			InitCorr,		// initial spot/vol correlation
	double			MeanCorr,	 	// mean correlation level
	double			CorrCS,			// correlation between the spot/vol correlation and spot
	double			CorrCV,			// correlation between the spot/vol correlation and vol^2
	double			Phi,			// stoch term in SDE for v (==inst vol^2) is proportional to v^Phi
	int				VIter,			// depth of iteration in var direction
	int				ZIter,			// depth of iteration in Z direction
	int          	nSteps, 		// number of timesteps
	Q_REBATE_TYPE	RebateType,		// Immediate or delayed till exp date
	Q_BARRIER_TYPE	BarrierType,	// single/double/no barrier
	double			LowerRebate,	// 
	double			UpperRebate,	// 
	double			DeltaX,			// grid spacing, log of spot
	Q_VECTOR		*PriceVec		// prices grid, input and output
);
	  

#endif
