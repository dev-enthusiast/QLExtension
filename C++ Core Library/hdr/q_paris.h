/* $Header: /home/cvs/src/quant/src/q_paris.h,v 1.23 2000/10/24 17:12:14 rozena Exp $ */
/****************************************************************
**
**	Q_PARIS.H	
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.23 $
**
****************************************************************/

#if !defined( IN_Q_PARIS_H )
#define IN_Q_PARIS_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QEXOTIC double Q_Parisian(
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,			// direction of fadeout
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
	double          outVal,				// fadeout barrier
    Q_VECTOR 		*Volatilities,		// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,	// grid for volatilities
    double 			time,	 			// Time to expiration
	Q_VECTOR        *koTimes,			// fade points
    double	 		Discount,     		// Domestic discount factor to expiration
    Q_VECTOR 		*Forwards, 			// forward curve
    Q_VECTOR 		*ForwardTimes,		// grid for forward curve
    int             nKO,				// number of knockouts required
    int             sofar,				// number of knockouts so far
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of intervals in price space
	double			RangeVol			// volatility used to fix the grid size
);

EXPORT_C_QEXOTIC int Q_ParisianVec(
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,			// direction of fadeout
	double          minSpot,        	// smallest Spot for which we need accurate value
	double          maxSpot,        	// largest Spot for which we need accurate value
	double 			strike,	 			// Option strike price
	double          outVal,				// fadeout barrier
    DT_CURVE 		*Vols,				// Underlyer volatility	curve
	double			RangeVol,			// volatility used to fix the grid size
    DATE            prcDate,        	// pricing date
    DATE            expDate,        	// maturity date
	Q_DATE_VECTOR   *koDates,			// fade points
    double	 		Discount,     		// Domestic discount factor to expiration
    DT_CURVE 		*FwdFactor, 		// forward curve / spot
    int             nKO,				// number of knockout required
    int             soFar,				// number of knockouts hit so far
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of intervals in price space
	Q_VECTOR		*SpotVec,			// returned spot array
	Q_VECTOR		*PriceVec			// returned option price array
);

EXPORT_C_QEXOTIC double Q_Baseball(
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,			// direction of fadeout
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
	double          outVal,				// fadeout barrier
    double 			time,	 			// Time to expiration
	Q_VECTOR        *times,				// fade points
	double			Rebate,				// rebate that we get when fully KO'd (delayed payment)
	Q_REBATE_TYPE	RebateType,			// Q_IMMEDIATE or Q_DELAYED
    Q_VECTOR 		*Volatilities,		// Underlyer volatility	curve
    Q_VECTOR 		*VolatilityTimes,	// grid for volatilities
	Q_VECTOR		*Discounts,	  		// discount factors
	Q_VECTOR		*DiscountTimes,		// times of discount factors
    Q_VECTOR 		*Forwards, 			// forward curve
    Q_VECTOR 		*ForwardTimes,		// grid for forward curve
    int             nKO,				// number of knockout required
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of intervals in price space
	double			RangeVol			// volatility used to fix the grid size
);

EXPORT_C_QEXOTIC int Q_BaseballVec(
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,			// direction of fadeout
	double          minSpot,        	// smallest Spot for which we need accurate value
	double          maxSpot,        	// largest Spot for which we need accurate value
	double 			strike,	 			// Option strike price
	double          outVal,				// fadeout barrier
    DT_CURVE 		*Vols,				// Underlyer volatility	curve
	double			RangeVol,			// volatility used to fix the grid size
    DATE            prcDate,        	// pricing date
    DATE            expDate,        	// maturity date
    Q_DATE_VECTOR   *koDates,  			// fade points
	double			Rebate,				// rebate that we get when fully KO'd (delayed payment)
	Q_REBATE_TYPE	RebateType,			// Q_IMMEDIATE or Q_DELAYED
	DT_CURVE		*DiscCrv,	  		// discount factors
    DT_CURVE 		*FwdFactors, 		// forward curve
    int             nKO,				// number of knockout required
	int          	nSteps, 			// number of timesteps
	int          	nGrid,				// number of intervals in price space
	Q_VECTOR		*SpotVec,			// returned spot array
	Q_VECTOR		*PriceVec			// returned option price array
);

EXPORT_C_QEXOTIC double Q_ParisianMC (
    Q_OPT_TYPE    	callPut,    // Option type (Q_OPT_PUT or Q_OPT_CALL)
    Q_UP_DOWN       upOrDown,   // up or down
    double 			spot,       // Underlyer spot price
    double 			strike,     // Option strike price
    double          outVal,     // knockout level
    Q_VECTOR 		*vol,       // Underlyer volatility curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,       // Time to expiration
    Q_VECTOR        *kotimes,   // times at which it can knock out
    Q_VECTOR 		*Rd,        // Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,      // forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    int             nko,        // number of knockouts required
    int             sofar,      // number of knockouts so far
    int          	nPaths,     // number of paths for Monte Carlo
    Q_RETURN_TYPE   retType     // return error or premium
    );

EXPORT_C_QEXOTIC double Q_LongestRunMC (
    Q_OPT_TYPE    	callPut,    // Option type (Q_OPT_PUT or Q_OPT_CALL)
    Q_UP_DOWN       upOrDown,   // up or down
    double 			spot,       // Underlyer spot price
    double 			strike,     // Option strike price
    double          outVal,     // knockout level
    Q_VECTOR 		*vol,       // Underlyer volatility curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,       // Time to expiration
    Q_VECTOR        *kotimes,   // times at which it can knock out
    double  		ccRd,       // Domestic rate
    Q_VECTOR 		*fwds,      // forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    int             current,    // size of current run
    int             sofar,      // maximum run so far
    int          	nPaths,     // number of paths for Monte Carlo
    double          notional,   // increase in notional per leg not faded
    Q_RETURN_TYPE   retType     // return error or premium
    );
    
EXPORT_C_QEXOTIC int Q_MRMParisian (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of knockout
   	int             consec,         // 1 if consecutive, 0 if not ("baseball")
    double 			spot,           // Underlyer spot price
    double	 		strike,  	    // strike price
	double          outVal,         // knockout value
    double          tenor,          // knocks out against this tenor
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *koDates,  		// dates where it can knockout
	DATE            expDate,        // expiration date of option
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    double	 		ccRd,           // domestic rate to expiration
    Q_VECTOR 		*fwds,          // vector of forwards
    Q_VECTOR 		*fwdT,          // vector of forward times
	int             currko,         // number of knockouts in a row so far
	int             nko,            // number of knockouts in a row needed
    int             nPaths,         // number of paths in Monte Carlo
    double          *premium,       // returned premium
	double          *error,         // returned standard error
	double          *rawprem,       // returned raw premium
	double          *rawerror       // returned raw error
    );
	  
#endif
