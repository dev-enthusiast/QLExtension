/* $Header: /home/cvs/src/quant/src/q_contop.h,v 1.21 2001/09/10 19:50:11 demeor Exp $ */
/****************************************************************
**
**	Q_CONTOP.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.21 $
**
****************************************************************/

#if !defined( IN_Q_CONTOP_H )
#define IN_Q_CONTOP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_TrigOpt    (  
	Q_OPT_TYPE  callPut,		 // option type
	Q_UP_DOWN   upOrDown,		 // direction of knockout on second asset
    double      spot,			 // first spot
    double      spot2,			 // second spot
    double      strike,			 // first strike
    double      strike2,		 // second strike (ending knockout level)
    double      knockout,		 // knockout level on second asset
    double      vol,			 // first volatility
	double      vol2,			 // second volatility
	double      corr,			 // correlation
	double      time,			 // time to expiration
	double      ccRd,			 // domestic interest rate
	double      ccRf,			 // first dividend rate
	double      ccRs			 // second dividend rate
	);

EXPORT_C_QKNOCK double Q_DoubleTrigOpt    (  
	Q_OPT_TYPE  	callPut,   // option type
    double      	spot,	   // spot price, first asset
    double      	spot2,	   // spot price, second asset
    double      	strike,	   // strike price
    double      	lowerko,   // lower ko level on second asset
    double      	upperko,   // upper ko level on second asset  
    double      	vol,	   // volatility, first asset
	double      	vol2,	   // volatility, second asset
	double      	corr,	   // correlation of asset returns
	double      	time,	   // time to expiration
	double      	ccRd,	   // domestic (discount) interest rate
	double      	ccRf,	   // foreign (yield) interest rate
	double      	ccRs,	   // dividend rate of second asset
	Q_RETURN_TYPE	retType    // return premium or delta
	);
EXPORT_C_QKNOCK double Q_TrigAvgOpt    (  
	Q_OPT_TYPE  callPut,   // option type
	Q_UP_DOWN   upOrDown,  // upward or downward trigger
    double      spot,	   // spot price, first asset
    double      spot2,	   // spot price, second asset
    double      strike,	   // strike price on average
    double      knockout,  // term ko level on second asset to start of averaging  
    Q_VECTOR    *vols,     // vector of volatilities, first asset
	double      vol2,	   // volatility, second asset
	double      corr,	   // correlation of asset returns
	Q_VECTOR    *times,	   // vector of averaging points
	double      ccRd,	   // domestic (discount) interest rate
	Q_VECTOR    *fwds,	   // forwards for each averaging point
	double      fwd2	   // forward price of second asset at start of avg
	);
EXPORT_C_QKNOCK double Q_TrigOptN    (  
	Q_OPT_TYPE  callPut,   // option type
	Q_UP_DOWN   upOrDown,  // upward or downward trigger
    double      spot,	   // spot price, first asset
    double      spot2,	   // spot price, second asset
    double      strike,	   // strike price
    double      strike2,   // ending ko level on second asset
    double      knockout,  // term ko level on second asset  
    double      vol,	   // volatility, first asset
	double      vol2,	   // volatility, second asset
	double      corr,	   // correlation of asset returns
	double      time,	   // time to expiration
	double      ccRd,	   // domestic (discount) interest rate
	double      ccRf,	   // foreign (yield) interest rate
	double      ccRs,	   // dividend rate of second asset
	int         npts       // number of points in integral
	);

EXPORT_C_QKNOCK double Q_TrigOptC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown, 	// Direction of trigger on first asset
	double 			spot1,	 	// payoff asset spot price
	double 			spot,	 	// trigger asset spot price
	double 			strike,	 	// strike on payoff asset
	double          strike2,    // ending knockout on trigger asset
    double 			outVal,		// term knockout price on trigger asset
    double        	vol1,       // term volatility on payoff asset
    Q_VECTOR 		*vol,		// Underlyer volatility	curve on trigger asset
    Q_VECTOR 		*volT,      // grid for volatilities
    double          corr,       // term correlation
    double 			time,	 	// Time to expiration
    double  		ccRd,     	// domestic rate to expiration
    double 			fwd1,       // term forward for payoff asset
    Q_VECTOR 		*fwds,     	// forward curve for trigger asset
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double  Q_TrigOptDiscC (
	Q_OPT_TYPE  	callPut,          // option type
	Q_UP_DOWN   	upOrDown,  		  // direction of knockout
	double 			fwd1,	 	      // payoff asset forward price
	double      	spot,			  // current spot price	of triggering asset
	double      	strike,	    	  // strike on payoff asset
	double      	outVal,  		  // term knockout price on trigger asset
    double        	vol1,             // term volatility on payoff asset
    Q_VECTOR 		*vol,	          // Underlyer volatility curve on trigger asset
    Q_VECTOR 		*volT,            // grid for volatilities
    double          corr,             // term correlation between assets
	double      	timeToExp,		  // time to expiration
	double      	ccRd,	    	  // domestic rate
    Q_VECTOR 		*fwds,            // forward curve for trigger asset
    Q_VECTOR 		*fwdT,            // grid for forward curve
	int         	nkopts,			  // number of knockout points
	int         	nstepsperko 	  // number of steps between knockout points
	);

EXPORT_C_QKNOCK double Q_TrigOptEnd    (  
	Q_OPT_TYPE  callPut,   // option type
	Q_UP_DOWN   upOrDown,  // upward or downward trigger
    double      spot,	   // spot price, first asset
    double      spot2,	   // spot price, second asset
    double      strike,	   // strike price
    double      outVal,    // ending ko level on second asset
    double      vol,	   // volatility, first asset
	double      vol2,	   // volatility, second asset
	double      corr,	   // correlation of asset returns
	double      time,	   // time to expiration
	double      ccRd,	   // domestic (discount) interest rate
	double      ccRf,	   // foreign (yield) interest rate
	double      ccRs	   // dividend rate of second asset
	);
EXPORT_C_QKNOCK double Q_TrigAmortSwap(
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	double          spot2,          // spot of triggering asset
	Q_VECTOR 		*strike,   		// one strike price	per payment date
	Q_VECTOR 		*Amounts,     	// notional amount per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    Q_VECTOR 		*vol,			// one volatility per payment date
    Q_VECTOR 		*vol2,			// one volatility of triggering asset per payment date
    Q_VECTOR 		*xvol,          // one cross volatility per payment date
    double          rangevol,       // volatility used to calculate the range of integration
    double          kotime,         // start of knockout period
    Q_VECTOR 		*times,	 		// payment times
    Q_VECTOR 		*Rd,     		// one domestic rate per payment time
    Q_VECTOR 		*fwds,     		// one forward price per payment time
    Q_VECTOR 		*fwds2,    		// forwards of second asset
	int          	nSteps, 		// number of timesteps
	int          	gridsize		// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_TrigAmortSwapMC(
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	double          spot2,          // spot of triggering asset
	Q_VECTOR 		*strike,   		// one strike price	per payment date
	Q_VECTOR 		*Amounts,     	// notional amount per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    Q_VECTOR 		*vol,			// one volatility per payment date
    Q_VECTOR 		*vol2,			// one volatility of triggering asset per payment date
    Q_VECTOR 		*xvol,          // one cross volatility per payment date
    double          kotime,         // start of knockout period
    Q_VECTOR 		*times,	 		// payment times
    Q_VECTOR 		*Rd,     		// one domestic rate per payment time
    Q_VECTOR 		*fwds,     		// one forward price per payment time
    Q_VECTOR 		*fwds2,    		// forwards of second asset
	int          	nPaths,			// number of simulations
	Q_RETURN_TYPE   retType         // return premium or error
    );
    		    					
EXPORT_C_QKNOCK int Q_MasterTrigOpt (
	Q_OPT_TYPE      callPut,
	Q_UP_DOWN       upOrDown,
	Q_IN_OUT        inOrOut,
	Q_ALIVE_DEAD    knocked,
	double      	spot,
	double      	spot2,
	double      	strike,
	double      	strike2,
	double      	knockout,
	double      	vol,
	double      	vol2,
	double      	corr,
	double      	time,
	double      	settle,
	double      	ccRd,
	double      	ccRf,
	double      	ccRs,
	double      	*prem,
	double      	*delta
	);
	
EXPORT_C_QKNOCK double Q_TrigFOEndKO (
	Q_OPT_TYPE  callPut,    // option type
	Q_UP_DOWN   upOrDown,   // upward or downward trigger fadeout
	Q_UP_DOWN   upOrDownKO, // upward or downward trigger at second point
    double      fwd1,	    // forward price of first asset                 
	double      spot2,      // spot price of second asset
    Q_VECTOR    *fwd2,      // forward vector for second asset
    Q_VECTOR    *fwd2T,     // forward times for second asset
    double      strike,	    // strike price                                 
    double      outVal,     // fadeout level
    double      knockout,   // knockout level
    Q_VECTOR    *vol,	    // volatility vector for first asset
    Q_VECTOR    *volT,	    // volatility time vector for first asset
	Q_VECTOR    *vol2,	    // volatility vector for second asset
	Q_VECTOR    *vol2T,	    // volatility time vector for second asset
	Q_VECTOR    *xvol,	    // cross volatility vector
	Q_VECTOR    *xvolT,	    // cross volatility time vector
	Q_VECTOR    *times,     // fadeout times
	double      time,	    // time to expiration 
	double      reduction,  // lost notional for each fade
	double      ccRd 	    // domestic (discount) interest rate            
	);

EXPORT_C_QKNOCK double Q_TrigOpt2Pt    (
	Q_OPT_TYPE  callPut,   // option type
	Q_UP_DOWN   upOrDown1, // upward or downward trigger at first point
	Q_UP_DOWN   upOrDown2, // upward or downward trigger at second point
    double      fwd1,	   // forward price of first asset
    Q_VECTOR    *fwd2,     // forward price of 2nd asset at knockout times
    double      strike,	   // strike price
    Q_VECTOR    *outVal,   // knockout levels at the trigger times
    Q_VECTOR    *vol,	   // volatility, first asset, ko times, expiration
	Q_VECTOR    *vol2,	   // volatility, second asset at ko points
	Q_VECTOR    *xvol,	   // cross volatility, at ko points
	Q_VECTOR    *kotimes,  // knockout times 1 and 2
	double      time,	   // time to expiration
	double      ccRd 	   // domestic (discount) interest rate
	);
	
#endif
