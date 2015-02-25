/* $Header: /home/cvs/src/quant/src/q_fadeko.h,v 1.38 2001/05/29 14:47:27 lowthg Exp $ */
/****************************************************************
**
**	Q_FADEKO.H
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.38 $
**
****************************************************************/

#if !defined( IN_Q_FADEKO_H )
#define IN_Q_FADEKO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_FadeOutKOBundle2(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDownFO,     // direction of knockout
	Q_UP_DOWN       upOrDownKO,     // direction of knockout
	double          spot,           // current spot price
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          knockout,       // knockout barrier
	double          reduction,      // reduction in payoff per hit
	Q_VECTOR        *vols,          // volatility vector
	double          endvol,         // volatility to expiration
    Q_VECTOR        *times,         // sampling points
    Q_VECTOR        *nodes,     	// nodes that determine bundling
    double          time,           // time to expiration
    Q_VECTOR        *Rd,            // domestic rate vector
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // foreign rate
    double          forward,        // forward at expiration
    int             initial,        // initial group - computed exactly
    int             bundle          // later groups are this size
	);

EXPORT_C_QKNOCK double Q_FadeOutKOCA2 (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDownFO,     // direction of knockout
	Q_UP_DOWN       upOrDownKO,     // direction of knockout
	double          spot,           // current spot price
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          knockout,       // knockout barrier
	Q_VECTOR        *vols,          // volatility vector
	double          endvol,         // volatility to expiration
    Q_VECTOR        *times,         // sampling points
    Q_VECTOR        *nodes,     	// nodes that determine bundling
    double          time,           // time to expiration
    Q_VECTOR        *Rd,            // domestic rate vector
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // foreign rate
    double          forward,        // forward at expiration
    int             initial,        // initial group - computed exactly
    int             maxBundle       // maximum size of bundle
	);

EXPORT_C_QKNOCK double Q_FadeOutPKOBundle(
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDownFO,     // direction of knockout
	Q_UP_DOWN       upOrDownKO,     // direction of knockout
	double          spot,           // current spot price
	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          knockout,       // knockout barrier
	double          reduction,      // reduction in payoff per hit
	Q_VECTOR        *vols,          // volatility vector
	double          endvol,         // volatility to expiration
    Q_VECTOR        *times,         // sampling points
    Q_VECTOR        *nodes,     	// nodes that determine bundling
    double          time,           // time to expiration
    Q_VECTOR        *Rd,            // domestic rate vector
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // foreign rate
    double          forward,        // forward at expiration
    int             initial,        // initial group - computed exactly
    int             bundle          // later groups are this size
	);
EXPORT_C_QKNOCK double Q_FadeOutPKOCA (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDownFO,     // direction of knockout
	Q_UP_DOWN       upOrDownKO,     // direction of knockout
	double          spot,           // current spot price
   	double          strike,         // strike price
	double          outVal,         // fadeout barrier
	double          knockout,       // knockout barrier
	Q_VECTOR        *vols,          // volatility vector
	double          endvol,         // volatility to expiration
    Q_VECTOR        *times,         // sampling points
    Q_VECTOR        *nodes,     	// nodes that determine bundling
    double          time,           // time to expiration
    Q_VECTOR        *Rd,            // domestic rate vector
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // foreign rate
    double          forward,        // forward at expiration
    int             initial,        // initial group - computed exactly
    int             maxBundle       // maximum size of bundle
	);

EXPORT_C_QKNOCK double Q_TrigFadeOutPKO (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDownFO,     // direction of knockout
	Q_UP_DOWN       upOrDownKO,     // direction of knockout
    double          fwd1,           // forward to expiration of first asset
	double          spot2,          // current spot price on 2nd asset
   	double          strike,         // strike price on first asset
	double          outVal,         // fadeout barrier on 2nd asset
	double          knockout,       // knockout barrier
	Q_VECTOR        *vol1,          // volatility vector of 1st asset
	Q_VECTOR        *vol2,          // volatility vector of 2nd asset
	Q_VECTOR        *xvol,          // cross volatility vector
	double          endvol,         // ending volatility of 1st asset
    Q_VECTOR        *times,         // sampling points
	double          time,           // time to expiration	 
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // forward vector for second asset
    int             npast,          // number of past points
    int             naccum          // number of accumulated points in the past
	);

EXPORT_C_QKNOCK double Q_TrigFadeOutKOCN (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of knockout
 	Q_UP_DOWN       upOrDownKO,     // direction of knockout
 	double          fwd1,           // ending forward of first asset
	double          spot,           // current spot price of second asset
	double          strike,         // strike price	on first asset
	double          outVal,         // fadeout barrier on second asset
	double          knockout,       // knockout barrier	on second asset
	Q_VECTOR        *vols,          // volatility vector of second asset
	double          vol1,           // ending volatility of first asset
	double          xvol,           // ending cross volatility
	double          endvol,         // volatility to expiration	of second asset
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // forward curve of second asset
    double          forward,        // forward at expiration of second asset
    int             naccum,         // number of accumulated legs
    int             npast,          // number of total past fade days
    int             gridsize,       // number of price gridpoints
    int             nSteps          // number of time steps
	);

EXPORT_C_QKNOCK double Q_TrigFadeOutPKOCN (
	Q_OPT_TYPE      callPut,        // option type
	Q_UP_DOWN       upOrDown,       // direction of knockout
 	Q_UP_DOWN       upOrDownKO,     // direction of knockout
 	double          fwd1,           // ending forward of first asset
	double          spot,           // current spot price of second asset
	double          strike,         // strike price	on first asset
	double          outVal,         // fadeout barrier on second asset
	double          knockout,       // knockout barrier	on second asset
	Q_VECTOR        *vol1,          // volatility vector of 1st asset
	Q_VECTOR        *vol2,          // volatility vector of 2nd asset
	Q_VECTOR        *xvol,          // cross volatility vector
	double          endvol,         // volatility to expiration	of first asset
    Q_VECTOR        *times,         // sampling points
    double          time,           // time to expiration
    double          endRd,          // domestic rate to expiration
    Q_VECTOR        *fwds,          // forward curve of second asset
    int             naccum,         // number of accumulated legs
    int             npast,          // number of total past fade days
    int             gridsize,       // number of price gridpoints
    int             nSteps          // number of time steps
	);

EXPORT_C_QKNOCK double Q_FadeOutPKOAvgStrk (
    Q_OPT_TYPE    	callPut,    	// call or put
    Q_UP_DOWN     	upOrDown,   	// direction of fadeout 
    Q_UP_DOWN     	upOrDownKO, 	// partial knockout direction
    double	      	spot,       	// current spot price
    double        	outVal,     	// fadeout level
    double        	knockout,   	// partial knockout level
    DT_CURVE      	*vols,      	// volatility curve
    Q_DATE_VECTOR 	*avgDates,  	// averaging times
    Q_DATE_VECTOR 	*fadeDates, 	// fadeout dates
    DATE          	prcDate,    	// pricing date
    DATE          	expDate,    	// expiration date
    double        	ccRd,       	// domestic interest rate to expiration
    DT_CURVE      	*fwds,      	// forward curve
    double          avgpast,        // average up to now
    int             navg,           // number of past average points
    int             npast,          // total number of past points
    int             naccum,         // number of accumulated legs
    int           	nSteps,     	// number of timesteps
    int           	gridsize,   	// number of gridpoints
    int             avggrid         // number of gridpoints in average dimension
    );

EXPORT_C_QKNOCK double Q_FadeOutAvgStrk (
    Q_OPT_TYPE    callPut,    // call or put
    Q_UP_DOWN     upOrDown,   // appears at upper or lower boundary
    Q_VECTOR      *avgfwds,   // forwards at averaging points
    Q_VECTOR      *fadefwds,  // forwards at fade points
    double        endfwd,     // forward at expiration
    double        knockout,   // fadeout level
    Q_VECTOR      *avgvols,   // volatilities at averaging points
    Q_VECTOR      *fadevols,  // volatilities at fade points
    double        endvol,     // volatility at expiration
    Q_VECTOR      *avgTimes,  // averaging times
    Q_VECTOR      *times,     // fadeout times
    double        time,       // time to expiration
    double        ccRd,       // domestic interest rate to expiration
    double        avgpast,    // average up to now
    int           navg,       // number of past average points
    int           npast,      // total number of past fade points
    int           naccum,     // number of accumulated legs
    int           npts        // size of grid for integration
    );

EXPORT_C_QKNOCK double Q_FadeOutCapKO (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,   // direction of fadeout
	Q_UP_DOWN       upOrDownKO, // direction of knockout
	Q_IN_OUT        inOrOut,    // fades out or accumulates
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          outVal,     // fadeout barrier
	double          knockout,   // knockout barrier
	double          rebate,     // delayed rebate for knocking out
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
	Q_VECTOR        *times,     // fade points
    double	 		ccRd,     	// Domestic rate to expiration
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    int             maxFade,    // number of fades allowed
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_FadeOutCapKOR (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,   // direction of fadeout	or accumulation
	Q_UP_DOWN       upOrDownKO, // direction of knockout
	Q_IN_OUT        inOrOut,    // fades out or accumulates
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          outVal,     // fadeout barrier
	double          knockout,   // knockout barrier
	double          rebate,     // delayed rebate for knocking out
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
	Q_VECTOR        *times,     // fade points
    double	 		ccRd,     	// Domestic rate to expiration
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    int             maxFade,    // number of fades allowed
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_FadeOutCapKOMC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,   // direction of fadeout or accumulation
	Q_UP_DOWN       upOrDownKO, // direction of knockout
	Q_IN_OUT        inOrOut,    // fades out or accumulates
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          outVal,     // fadeout barrier
	double          knockout,   // knockout barrier
	double          rebate,     // delayed rebate for knocking out
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
	Q_VECTOR        *times,     // fade points
    double	 		ccRd,     	// Domestic rate to expiration
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    int             maxFade,    // number of fades allowed
	int          	nPaths, 	// number of timesteps
	Q_RETURN_TYPE   retType     // return premium or error
    );

EXPORT_C_QKNOCK int Q_FadeOutCapKOV (
	Q_OPT_TYPE    	callPut,	     // Option type (Q_OPT_PUT or Q_OPT_CALL)   
	Q_UP_DOWN       upOrDown,        // direction of fadeout                    
	Q_UP_DOWN       upOrDownKO,      // direction of knockout                   
	Q_IN_OUT        inOrOut,         // fades out or accumulates                
	double 			spot,	 	     // Underlyer spot price                    
	double 			strike,	 	     // Option strike price                     
	double          outVal,          // fadeout barrier                         
	double          knockout,        // knockout barrier                        
	double          rebate,          // delayed rebate for knocking out         
    Q_VECTOR 		*vol,		     // Underlyer volatility curve              
    Q_VECTOR 		*volT,           // grid for volatilities                   
    double 			time,	 	     // Time to expiration                      
	Q_VECTOR        *times,          // fade points                             
    double	 		ccRd,     	     // Domestic rate to expiration             
    Q_VECTOR 		*fwds,     	     // forward curve                           
    Q_VECTOR 		*fwdT,           // grid for forward curve                  
    int             maxFade,         // number of fades allowed                 
	int          	nSteps, 	     // number of timesteps                     
	int          	gridsize,	     // number of intervals in price space      
	Q_VECTOR        *Spot,	 		 // return spot vector
	Q_VECTOR        *Price	 		 // return option price vector
    );

EXPORT_C_QKNOCK double Q_HighDensityFOKOR(
	Q_OPT_TYPE    	callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,		// direction of fadeout
	Q_UP_DOWN       upOrDownKO,		// direction of knockout
	Q_IN_OUT        inOrOut,		// fades out or accumulates
	double 			spot,	 		// Underlyer spot price
	double 			strike,	 		// Option strike price
	double          outVal,			// fadeout barrier
	double          knockout,		// knockout barrier
	double          rebate,			// delayed rebate for knocking out
    Q_VECTOR 		*vol,			// Underlyer volatility	curve
    Q_VECTOR 		*volT,			// grid for volatilities
    double 			time,	 		// Time to expiration
	Q_VECTOR        *times,			// fade points
    double	 		ccRd,     		// Domestic rate to expiration
    Q_VECTOR 		*fwds,     		// forward curve
    Q_VECTOR 		*fwdT,			// grid for forward curve
    int             maxFade,		// number of fades allowed
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	double			RangeVol		// volatility used to set up the grid
    );

EXPORT_C_QKNOCK double Q_HighDensityFOKO(
	Q_OPT_TYPE    	callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN       upOrDown,		// direction of fadeout
	Q_UP_DOWN       upOrDownKO,		// direction of knockout
	Q_IN_OUT        inOrOut,		// fades out or accumulates
	double 			spot,	 		// Underlyer spot price
	double 			strike,	 		// Option strike price
	double          outVal,			// fadeout barrier
	double          knockout,		// knockout barrier
	double          rebate,			// delayed rebate for knocking out
    Q_VECTOR 		*vol,			// Underlyer volatility	curve
    Q_VECTOR 		*volT,			// grid for volatilities
    double 			time,	 		// Time to expiration
	Q_VECTOR        *times,			// fade points
    double	 		ccRd,     		// Domestic rate to expiration
    Q_VECTOR 		*fwds,     		// forward curve
    Q_VECTOR 		*fwdT,			// grid for forward curve
    int             maxFade,		// number of fades allowed
	int          	nSteps, 		// number of timesteps
	int          	gridsize,		// number of intervals in price space
	double			RangeVol			// volatility used to set up the grid
    );
            			
#endif

