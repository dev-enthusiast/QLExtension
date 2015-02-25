/* $Header: /home/cvs/src/quant/src/q_mrmtl.h,v 1.29 2001/02/28 17:37:20 demeor Exp $ */
/****************************************************************
**
**	Q_MRMTL.H
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.29 $
**
****************************************************************/

#if !defined( IN_Q_MRMTL_H )
#define IN_Q_MRMTL_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h>
		
EXPORT_C_QMRM double Q_MRMCovariance(
	Q_VECTOR		*OptTimes,		// Market Option Terms
	Q_VECTOR		*SpotVols,		// Implied Spot Volatilities

	double			Beta,			// MR Param - Beta
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	double			Svol,			// MR Param - Eventual Spot Volatility

	double			ExpTime1,		// As of Time of Forward 1
	double			FwdTime1,		// Maturity Weight of Forward 1
	double			ExpTime2,		// As of Time of Forward 2
	double			FwdTime2		// Maturity Weight of Forward 2
);

EXPORT_C_QMRM double Q_MRMFwdTimeOfCurve(
	Q_VECTOR		*FwdTimes,		// Forward Terms
	Q_VECTOR		*FwdRates,		// Forward Rates
	double			Beta			// MR Param - Beta
);

EXPORT_C_QMRM double Q_MRMInstantVol(
	Q_VECTOR		*OptTimes,		// Market Option Terms
	Q_VECTOR		*SpotVols,		// Implied Spot Volatilities

	double			Beta,			// MR Param - Beta
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	double			Svol,			// MR Param - Eventual Spot Volatility

	double			FwdTime			// Forward Date
);


EXPORT_C_QMRM double Q_MRMTermVol(
	Q_VECTOR		*OptTimes,		// Market Option Terms
	Q_VECTOR		*SpotVols,		// Implied Spot Volatilities

	double			Beta,			// MR Param - Beta
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	double			Svol,			// MR Param - Eventual Spot Volatility

	double			ExpTime,		// As of Time of Forward
	double			FwdTime			// Maturity Weight of Forward
);

EXPORT_C_QMRM double Q_MRMFadeOut(
	Q_OPT_TYPE      callPut,		// option type
	Q_UP_DOWN       upOrDown,		// direction of knockout
	double          forward,		// current forward price at expiration
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          reduction,      // reducton in payoff per hit
	double          endvol,         // volatility at expiration
	Q_VECTOR        *vols,          // volatility vector
	Q_VECTOR        *covs,			// covariance vector
    Q_VECTOR        *times,			// sampling points
    double          time,			// time to expiration
    double          ccRd,			// domestic rate
    Q_VECTOR        *fwds,			// vector of forward prices
    int             initial,		// initial group - computed exactly
    int             bundle			// later groups are this size
);

EXPORT_C_QMRM int Q_MRMFadeOutCurvesBundle	(
	Q_OPT_TYPE      callPut,		// option type
	Q_UP_DOWN       upOrDown,		// direction of knockout
	double          forward,		// current forward price at expiration
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          reduction,      // reducton in payoff per hit
	double          endvol,         // volatility at expiration
	Q_VECTOR        *vols,          // volatility vector
	Q_VECTOR        *covs,			// covariance vector
    Q_VECTOR        *times,			// sampling points
    double          time,			// time to expiration
    double          ccRd,			// domestic rate
    Q_VECTOR        *fwds,			// vector of forward prices
    double          *premRet,		// returned premium
    double          *delta,			// returned delta
    int             initial,		// initial group - computed exactly
    int             bundle			// later groups are this size
);

EXPORT_C_QMRM double Q_MRMFadeOutCA(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of knockout
	double          forward,        // current forward price at expiration
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          endvol,         // volatility at expiration
	Q_VECTOR        *vols,          // volatility vector
	Q_VECTOR        *covs,          // covariance vector
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          ccRd,           // domestic rate to expiration
    Q_VECTOR        *fwds,          // vector of forward prices
    int             initial,        // initial group - computed exactly
    int             bundle          // later groups are this size
);

EXPORT_C_QMRM double Q_MRMTimeBasket (
	Q_OPT_TYPE   callPut,          // option type
	double       spot,             // current spot price
	double       strike,           // strike price on basket
	Q_VECTOR	 *wts,			   // weights for basket of futures prices
	Q_VECTOR     *spotvol,         // instantaneous spot vol vector
	Q_VECTOR     *voltimes,        // times for volatilities
	double       beta,             // mean reversion coefficient
	double       longVol,          // volatility of long-term price
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *times,           // times for set of prices
	Q_VECTOR     *grid,            // times to expiration of forwards
	double       time,			   // time to expiration
	double       ccRd,             // continuous interest rate
    Q_VECTOR     *fwds             // forward curve
	);

EXPORT_C_QMRM int Q_MRMTimeBasketParms (
	double       spot,             // current spot price
	Q_VECTOR	 *wts,			   // weights for basket of futures prices
	Q_VECTOR     *spotvol,         // instantaneous spot vol vector
	Q_VECTOR     *voltimes,        // times for volatilities
	double       beta,             // mean reversion coefficient
	double       longVol,          // volatility of long-term price
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *times,           // times for set of prices
	Q_VECTOR     *grid,            // times to expiration of futures and vols
	double       time,			   // time to expiration
    Q_VECTOR     *fwds,            // forward curve
    double       *longfwd,         // returned long forward
    double       *shortfwd,        // returned short forward
    double       *longvol,		   // returned long volatility
    double       *shortvol,		   // returned short volatility
    double       *rho              // returned correlation
	);

EXPORT_C_QMRM double Q_MRMCovarianceB(
	Q_VECTOR		*OptTimes,		// Market Option Terms
	Q_VECTOR		*SpotVols,		// Implied Spot Volatilities
	Q_VECTOR		*Beta,			// MR Param - Beta vector
	double			Lvol,			// MR Param - Long Volatility
	double			Rho,			// MR Param - Spot to Long Correlation
	double			Svol,			// MR Param - Eventual Spot Volatility
	double			ExpTime1,		// As of Time of Forward 1
	double			FwdTime1,		// Maturity Weight of Forward 1
	double			ExpTime2,		// As of Time of Forward 2
	double			FwdTime2		// Maturity Weight of Forward 2
	);

EXPORT_C_QMRM double Q_MRMTimeBasketB (
	Q_OPT_TYPE   callPut,          // option type
	double       spot,             // current spot price
	double       strike,           // strike price on basket
	Q_VECTOR	 *wts,			   // weights for basket of futures prices
	Q_VECTOR     *spotvol,         // instantaneous spot vol vector
	Q_VECTOR     *voltimes,        // times for volatilities
	double       beta,             // mean reversion coefficient
	double       longVol,          // volatility of long-term price
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *times,           // times for set of prices
	Q_VECTOR     *exptimes,        // times to expiration of forwards being read
	Q_VECTOR     *grid,            // times to expiration of forwards in curve
	double       time,			   // time to expiration
	double       ccRd,             // continuous interest rate
    Q_VECTOR     *fwds             // forward curve
	);
		
EXPORT_C_QMRM int Q_MRMTimeBasketParmsB (
	double       spot,             // current spot price
	Q_VECTOR	 *wts,			   // weights for basket of futures prices
	Q_VECTOR     *spotvol,         // instantaneous spot vol vector
	Q_VECTOR     *voltimes,        // times for volatilities
	double       beta,             // mean reversion coefficient
	double       longVol,          // volatility of long-term price
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *times,           // times for set of prices
	Q_VECTOR     *exptimes,        // times to expiration of forwards being read
	Q_VECTOR     *grid,            // times to expiration of futures and vols
	double       time,			   // time to expiration
    Q_VECTOR     *fwds,            // forward curve
    double       *longfwd,         // returned long forward
    double       *shortfwd,        // returned short forward
    double       *longvol,		   // returned long volatility
    double       *shortvol,		   // returned short volatility
    double       *rho              // returned correlation
	);
	
EXPORT_C_QMRM double Q_MRMFadeOutAvg (
    Q_OPT_TYPE    callPut,    // call or put
    Q_UP_DOWN     upOrDown,   // appears at upper or lower boundary
    DT_CURVE      *fwds,      // data curve - includes averaging dates
    double        strike,     // strike price
    double        knockout,   // fadeout level
    DT_CURVE      *spotvol,   // instantaneous spot volatility curve
	double        beta,       // mean reversion coefficient
	double        longVol,    // volatility of long-term price
	double        corr,       // correlation of long and short prices
	DATE          prcDate,    // pricing date
    DATE          expDate,    // time to expiration
    double        ccRd,       // domestic interest rate to expiration
    int           npts        // size of grid for integration
    );

EXPORT_C_QMRM double Q_MRMFadeOutAvg2 (
    Q_OPT_TYPE    callPut,    // call or put
    Q_UP_DOWN     upOrDown,   // appears at upper or lower boundary
	double        spot,       // current spot price
    DT_CURVE      *fwds,      // data curve - includes averaging dates
    DT_CURVE      *fadefwds,  // data curve - includes fade dates
    double        strike,     // strike price
    double        knockout,   // fadeout level
	double        tenor,      // tenor used for fading out
    DT_CURVE      *spotvol,   // instantaneous spot volatility curve
	double        beta,       // mean reversion coefficient
	double        longVol,    // volatility of long-term price
	double        corr,       // correlation of long and short prices
	DATE          prcDate,    // pricing date
    DATE          expDate,    // time to expiration
    double        ccRd,       // domestic interest rate to expiration
    int           npts        // size of grid for integration
    );

EXPORT_C_QMRM double Q_MRMFadeOutAvgMC (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // fadeout up or down
    double 			spot,           // Underlyer spot price
    double  		strike,         // one strike price per payment date
    double          outVal,         // fadeout level
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*times,         // payment times
    double          time,           // time to expiration
    double	 		ccRd,           // one domestic rate per payment time
    Q_VECTOR 		*fwds,          // one forward price per payment time
    int             nPaths,         // number of paths in Monte Carlo
    Q_RETURN_TYPE   retType         // return premium or error
    );

EXPORT_C_QMRM double Q_MRMSpotLongCov (
	Q_VECTOR        *futexp,    	// times to expirations of futures
	Q_VECTOR        *spotvol,   	// spot volatilities between futures dates
	double          time,       	// term of this cov(spot,long)
	double			Beta,			// mean reversion coefficient
	double			LongVol,		// volatility of long-term price
	double			Correlation,	// instantaneous correlation of spot and long-term price
	double          svol            // "limiting" spot vol
	);
      
EXPORT_C_QMRM double Q_MRMBaskLongCov (
	Q_VECTOR        *futexp,    	// times to expirations of futures
	Q_VECTOR        *spotvol,   	// spot volatilities between futures dates
	double          time,       	// term of the long price
	Q_VECTOR        *times,     	// times for spots in the time basket
	Q_VECTOR        *fwds,      	// forwards at the basket times
	double			Beta,		    // mean reversion coefficient
	double			LongVol,   	    // volatility of long-term price
	double			Correlation,	// instantaneous correlation of spot and long-term price
	double          svol            // "limiting" spot vol
	);

EXPORT_C_QMRM double Q_MRMOptPortOpt (
 	Q_OPT_TYPE   	  callPut,          // option type of compound option
	double       	  spot,             // current spot price
	Q_VECTOR          *types,           // option type of underlying	as doubles
	double            strike,           // strike of compound option
	Q_VECTOR     	  *quantity,        // quantities of underlying options
	Q_VECTOR     	  *strikes,         // strikes of underlying options
	Q_VECTOR     	  *svol,  		    // piecewise constant spot volatility vector
	Q_VECTOR     	  *volTimes,        // voltility times
	double       	  longvol,          // volatility of long-term price
	double       	  beta,             // mean reversion coefficient
	double       	  corr,             // correlation of long and short prices
	Q_VECTOR     	  *grid,            // expiration times of futures
	DATE         	  prcDate,          // today's date
	DATE         	  cmpded,           // exp date of compound option
	Q_DATE_VECTOR     *startavg,        // starts of averaging for underlying options
	Q_DATE_VECTOR     *expdate,         // times to exp of underlying options
    Q_VECTOR     	  *disc,            // domestic discount vector
    Q_VECTOR     	  *rdT,             // times for domestic discount vector
    Q_VECTOR     	  *fut,             // futures price vector
	int               npts              // number of points for final integral
	);

EXPORT_C_QMRM double Q_MRMOptPortOpt2 (
 	Q_OPT_TYPE   	  callPut,          // option type of compound option
	double       	  spot,             // current spot price
	Q_VECTOR          *types,           // option type of underlying	as doubles
	double            strike,           // strike of compound option
	Q_VECTOR     	  *quantity,        // quantities of underlying options
	Q_VECTOR     	  *strikes,         // strikes of underlying options
	Q_VECTOR     	  *svol,  		    // piecewise constant spot volatility vector
	Q_VECTOR     	  *volTimes,        // voltility times
	double       	  longVol,          // volatility of long-term price
	double       	  beta,             // mean reversion coefficient
	double       	  corr,             // correlation of long and short prices
	Q_VECTOR     	  *grid,            // expiration times of futures
	DATE         	  prcDate,          // today's date
	DATE         	  cmpded,           // exp date of compound option
	Q_DATE_VECTOR     *startavg,        // dates - starts of averaging for underlying options
	Q_DATE_VECTOR     *expdate,         // dates - expirations of underlying options
    Q_VECTOR     	  *disc,            // domestic discount vector
    Q_VECTOR     	  *rdT,             // times for domestic rates
    Q_VECTOR     	  *fut,             // futures price vector
	char              *ccy1,            // denominated asset
	char              *ccy2,			// quantity unit
	int               npts,             // number of points in spot dimension
	int               npts2             // number of points in long dimension
	);

EXPORT_C_QMRM double Q_TimeBasketGen (
	Q_OPT_TYPE               callPut,          // option type                          
	double                   spot,             // current spot price                   
	double                   strike,           // strike price on basket               
	Q_VECTOR	   	         *wts,             // weights for basket of futures prices 
	N_GENERIC_FUNC_3D        covariance,       // arbitrary covariance function for futures                                                                                
	void                     *context,         // context for covariance function
	Q_VECTOR                 *times,           // times for set of prices               
	Q_VECTOR                 *exptimes,        // times to expiration for forwards being read
	Q_VECTOR                 *grid,            // times to expiration of forwards       
	double        	         time,             // time to expiration                    
	double                   ccRd,             // continuous interest rate              
    Q_VECTOR                 *fwds             // forward curve                         
	);

EXPORT_C_QMRM int Q_TimeBasketGenParms(
	double                   spot,             // current spot price                         
	Q_VECTOR	             *wts,             // weights for basket of futures prices       
	N_GENERIC_FUNC_3D        covariance,       // arbitrary covariance function for futures                                                                                
	void                     *context,         // context for covariance function
	Q_VECTOR                 *times,           // times for set of prices                    
	Q_VECTOR                 *exptimes,        // times to expiration of forwards being read 
	Q_VECTOR                 *grid,            // times to expiration of forwards in curve
	double                   time,             // time to expiration                         
    Q_VECTOR                 *fwds,            // forward curve                              
    double                   *longfwd,         // returned long forward                      
    double                   *shortfwd,        // returned short forward                     
    double                   *longvol,         // returned long volatility                   
    double                   *shortvol,        // returned short volatility                  
    double                   *rho              // returned correlation                       
	);

EXPORT_C_QMRM int Q_MRMTimeBasketGeo(
	double       spot,             // current spot price
	Q_VECTOR	 *wts,			   // weights for basket of futures prices
	Q_VECTOR     *spotvol,         // instantaneous spot vol vector
	Q_VECTOR     *voltimes,        // times for volatilities
	double       beta,             // mean reversion coefficient
	double       longVol,          // volatility of long-term price
	double       corr,             // correlation of long and short prices
	Q_VECTOR     *times,           // times for set of prices
	Q_VECTOR     *exptimes,        // times to expiration of forwards being read
	double       begin,            // time in the future at which this is calculated
	double       time,			   // time to expiration
    double       *longvol,		   // returned long volatility
    double       *shortvol,		   // returned short volatility
    double       *rho              // returned correlation
	);

EXPORT_C_QMRM double Q_MRMOptPortFadeOut (
    Q_OPT_TYPE    callPut,    // call or put - compound option
	Q_VECTOR      *types,     // call or put - underlying fadeouts
    Q_UP_DOWN     upOrDown,   // appears at upper or lower boundary
	double        fwd,        // forward at expiration of compound option
    DT_CURVE      *fwds,      // array of data curves - includes averaging dates
    DT_CURVE      *fadefwds,  // array of data curves - includes fade dates
    double        strike,     // strike price, compound option
    Q_VECTOR      *strikes,   // strike prices, underlying fadeouts
    Q_VECTOR      *Amounts,   // amounts of each option
    double        knockout,   // fadeout level
	double        tenor,      // tenor used for fading out
    DT_CURVE      *spotvol,   // instantaneous spot volatility curve
	double        beta,       // mean reversion coefficient
	double        longVol,    // volatility of long-term price
	double        corr,       // correlation of long and short prices
	DATE          prcDate,    // pricing date
	DATE          cmpdExp,    // expiration of compound option
    Q_DATE_VECTOR *expDates,  // expiration dates of underlying fadeouts
    double        ccRd1,      // domestic interest rate to compound expiration
    Q_VECTOR      *ccRd,      // domestic interest rate to underlying expirations
    int           nGrid1,     // size of grid in X dimension
    int           nGrid2      // size of grid in Y dimension
    );	  

EXPORT_C_QMRM double Q_MRMOptFadeOut (
    Q_OPT_TYPE    callPut,    // call or put - compound option
	Q_OPT_TYPE    callPut2,   // call or put - underlying fadeout
    Q_UP_DOWN     upOrDown,   // appears at upper or lower boundary
	double        fwd,        // forward at expiration of compound option
    DT_CURVE      *fwds,      // data curve - includes averaging dates
    DT_CURVE      *fadefwds,  // data curve - includes fade dates
    double        strike,     // strike price, compound option
    double        strike2,    // strike price, underlying fadeout
    double        knockout,   // fadeout level
	double        tenor,      // tenor used for fading out
    DT_CURVE      *spotvol,   // instantaneous spot volatility curve
	double        beta,       // mean reversion coefficient
	double        longVol,    // volatility of long-term price
	double        corr,       // correlation of long and short prices
	DATE          prcDate,    // pricing date
	DATE          cmpdExp,    // expiration of compound option
    DATE          expDate,    // time to expiration	of underlying option
    double        ccRd1,      // domestic interest rate to compound expiration
    double        ccRd2,      // domestic interest rate to underlying expiration
    int           nGrid1,     // size of grid in X dimension
    int           nGrid2      // size of grid in Y dimension
    );

EXPORT_C_QMRM double Q_MRMFadeOutAvgHDMC(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // fadeout up or down
    double 			spot,           // Underlyer spot price
    double  		strike,         // one strike price per payment date
    double          outVal,         // fadeout level
    Q_VECTOR 		*vol,           // piecewise constant spot volatility vector
    Q_VECTOR 		*volTimes,      // volatility times
    double          beta,           // mean reversion coefficient
    double          longVol,        // volatility of long price
    double			corr,           // correlation between short and long
    Q_VECTOR 		*times,         // fade times and averaging times
    double          time,           // time to expiration
    double	 		ccRd,           // overall domestic rate
    Q_VECTOR 		*fwds,          // one forward price per averaging time
	int             maxfade,        // number of fades necessary to knock out completely
    int             nPaths,         // number of paths in Monte Carlo
    Q_RETURN_TYPE   retType         // return premium or error
    );
			    
#endif

