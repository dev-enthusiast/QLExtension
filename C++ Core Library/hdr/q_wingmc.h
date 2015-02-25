/****************************************************************
**
**	Q_WINGMC.H	
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.72 $
**
****************************************************************/


#if !defined( IN_Q_WINGMC_H )
#define IN_Q_WINGMC_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h>

typedef struct 
{
  double		strike;
  Q_OPT_TYPE	optType;
}  Q_PAYOFF;

DLLEXPORT int Q_WingMC (
	double          spot,		  	// current forward price at expiration
	double          initVol,	  	// initial instantaneous volatility
	double			effVol,			// effective constant volatility
    Q_VECTOR        *lvol,		  	// volatility as a function of time
    Q_VECTOR        *lvolT,		  	// gridpoints for volatility curve
	Q_VECTOR        *beta,		  	// mean reversion factor on volatility
	Q_VECTOR        *sigma,		  	// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *rho,		  	// correlation of spot and volatility
	Q_VECTOR		*rhoJ,		 	// correlation of jumps in spot and volatility
	Q_VECTOR        *lambda,	  	// mean of poisson process
	Q_VECTOR        *jumpS_Mean,  	// mean of size of underlying jump in log terms
	Q_VECTOR        *jumpS_SD,	  	// variance of size of underlying jump in log terms
	Q_VECTOR        *jumpV_Mean,  	// mean of size of volatility jump in log terms
	Q_VECTOR        *jumpV_SD,	  	// variance of size of volatiltity jump in log terms
	Q_VECTOR        *fwds,		  	// forward vector
	Q_VECTOR        *fwdT,		  	// forward time vector
	Q_VECTOR        *times,		  	// points on the path
	int             nPaths,		  	// number of trials for Monte Carlo test
	int				saveLogSpot,	// TRUE if need log spot path, FALSE otherwise
	int             seed,		  	// initial seed parameter
	Q_MATRIX        *spotPaths,		// returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// returned matrix containing fully degenerate spot paths
	Q_MATRIX        *dVolPaths,		// returned matrix containing fully degenerate vol paths
	Q_MATRIX        *cSpotPaths,	// returned matrix containing spot paths with SV and jumps only
	Q_MATRIX        *cVolPaths,		// returned matrix containing vol paths with SV and jumps only
	N_GENERIC_FUNC_3D	localVol, 	// pointer to a function returning local vol
	void			 *Context	  	// context for the local volatility fuction
	);

DLLEXPORT int Q_WingMCL (
	double          spot,			// current spot price
	double          initVol,		// initial instantaneous volatility
	double			effVol,			// effective constant volatility
    Q_VECTOR        *lVol,			// forward-forward reverting vols
    Q_VECTOR        *lVolT,			// gridpoints for volatility curve
	Q_VECTOR        *beta,			// mean reversion factor on volatility
	Q_VECTOR        *sigma,			// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *rho,			// correlation of spot and volatility
	Q_VECTOR		 *rhoJ,			// correlation of jumps in spot and volatility
    Q_VECTOR        *lambda,		// mean of Poisson process
	Q_VECTOR        *jumpS_Mean,	// mean of size of underlying jump in log terms
	Q_VECTOR        *jumpS_SD,	  	// variance of size of underlying jump in log terms
	Q_VECTOR        *jumpV_Mean,	// mean of size of volatility jump in log terms
	Q_VECTOR        *jumpV_SD,		// variance of size of volatiltity jump in log terms
	Q_VECTOR        *fwds,			// forward vector
	Q_VECTOR        *fwdT,			// forward time vector
	Q_VECTOR        *times,			// points on the path
	int             nPaths,			// number of trials for Monte Carlo test
	int				saveLogSpot,	// TRUE if need log spot path, FALSE otherwise
	int             seed,			// initial seed parameter
	Q_MATRIX        *spotPaths,		// returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// returned matrix containing fully degenerate spot paths
	Q_MATRIX        *dVolPaths,		// returned matrix containing fully degenerate vol paths
	Q_MATRIX        *cSpotPaths,	// returned matrix containing spot paths with SV and jumps only
	Q_MATRIX        *cVolPaths,		// returned matrix containing vol paths with SV and jumps only
	Q_VECTOR  		*Times,     	// times for local volatility
	Q_VECTOR     	*Spots,     	// times for local volatility
	Q_MATRIX    	*volMatrix  	// matrix of local vols
	);

DLLEXPORT int Q_WingMC3 (
	double          spot,			// current spot price
	double          initVol,		// initial instantaneous volatility
	Q_VECTOR        *lVol,			// forward-forward reverting vols
	Q_VECTOR        *lVolT,			// gridpoints for volatility curve
	Q_VECTOR        *beta,			// mean reversion factor on volatility
	Q_VECTOR        *sigma,			// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *rho,			// correlation of spot and volatility
	Q_VECTOR		*rhoJ,			// correlation of jumps in spot and volatility
	Q_VECTOR        *lambda,		// mean of Poisson process
	Q_VECTOR        *jumpS_Mean,	// mean of size of underlying jump in log terms
	Q_VECTOR        *jumpS_SD,		// variance of size of underlying jump in log terms
	Q_VECTOR        *jumpV_Mean,	// mean of size of volatility jump in log terms
    Q_VECTOR        *jumpV_SD,		// variance of size of volatiltity jump in log terms
	Q_VECTOR        *fwds,			// forward vector
	Q_VECTOR        *fwdT,			// forward time vector
	Q_VECTOR        *times,			// points on the path
	int             nPaths,			// number of trials for Monte Carlo test
	int             seed,			// initial seed parameter
	Q_MATRIX        *spotPaths,		// returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// returned matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// returned matrix containing degenerate volatility paths
	Q_MATRIX        *lSpotPaths,	// returned matrix containing spot price paths with local vol only
	Q_MATRIX        *lVolPaths,		// returned matrix containing volatility paths with local vol only
	Q_MATRIX        *jSpotPaths,	// returned matrix containing spot price paths with jumps only
	N_GENERIC_FUNC_3D	localVol,	// pointer to a function returning local vol
	void			*Context		// context for the local volatility fuction
	);

DLLEXPORT int Q_WingMCL3 (
	double          spot,			// current spot price
	double          initVol,		// initial instantaneous volatility
    Q_VECTOR        *lVol,			// forward-forward reverting vols
    Q_VECTOR        *lVolT,			// gridpoints for volatility curve
	Q_VECTOR        *beta,			// mean reversion factor on volatility
	Q_VECTOR        *sigma,			// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *rho,			// correlation of spot and volatility
	Q_VECTOR		 *rhoJ,			// correlation of jumps in spot and volatility
    Q_VECTOR        *lambda,		// mean of Poisson process
	Q_VECTOR        *jumpS_Mean,	// mean of size of underlying jump in log terms
	Q_VECTOR        *jumpS_SD,	  	// variance of size of underlying jump in log terms
	Q_VECTOR        *jumpV_Mean,	// mean of size of volatility jump in log terms
	Q_VECTOR        *jumpV_SD,		// variance of size of volatiltity jump in log terms
	Q_VECTOR        *fwds,			// forward vector
	Q_VECTOR        *fwdT,			// forward time vector
	Q_VECTOR        *times,			// points on the path
	int             nPaths,			// number of trials for Monte Carlo test
	int             seed,			// initial seed parameter
	Q_MATRIX        *spotPaths,		// returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// returned matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// returned matrix containing degenerate volatility paths
	Q_MATRIX        *lSpotPaths,	// returned matrix containing spot price paths with local vol only
	Q_MATRIX        *lVolPaths,		// returned matrix containing volatility paths with local vol only
	Q_MATRIX        *jSpotPaths,	// returned matrix containing spot price paths with jumps only
	Q_VECTOR  		*Times,     	// times for local volatility
	Q_VECTOR     	*Spots,     	// times for local volatility
	Q_MATRIX    	*volMatrix  	// matrix of local vols
	);

DLLEXPORT int Q_WingCorrMC (
	double          spot,			// current spot price
	double          initVol,		// initial instantaneous volatility
	Q_VECTOR        *lVol,			// forward-forward reverting vols
	Q_VECTOR        *lVolT,			// gridpoints for volatility curve
	Q_VECTOR        *beta,			// mean reversion factor on volatility
	Q_VECTOR        *sigma,			// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *alpha,			// parameter controlling the range of correlation changes
	Q_VECTOR        *spot0,			// spot level where correlation is zero
	Q_VECTOR		*rhoJ,			// correlation of jumps in spot and volatility
	Q_VECTOR        *lambda,		// mean of Poisson process
	Q_VECTOR        *jumpS_Mean,	// mean of size of underlying jump in log terms
	Q_VECTOR        *jumpS_SD,		// variance of size of underlying jump in log terms
	Q_VECTOR        *jumpV_Mean,	// mean of size of volatility jump in log terms
    Q_VECTOR        *jumpV_SD,		// variance of size of volatiltity jump in log terms
	Q_VECTOR        *fwds,			// forward vector
	Q_VECTOR        *fwdT,			// forward time vector
	Q_VECTOR        *times,			// points on the path
	int             nPaths,			// number of trials for Monte Carlo test
	int             seed,			// initial seed parameter
	Q_MATRIX        *spotPaths,		// returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// returned matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// returned matrix containing degenerate volatility paths
	Q_MATRIX        *jSpotPaths,	// returned matrix containing spot price paths with jumps only
	N_GENERIC_FUNC_3D	localVol,	// pointer to a function returning local vol
	void			*Context		// context for the local volatility fuction
	);

DLLEXPORT int Q_OptWingMC(
	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	DT_CURVE		*disc,		// Domestic discount curve
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_VECTOR		*path_PVs,	// Returned present values of the paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);

DLLEXPORT double Q_Payoff (
	double			endingSpot,	// Value of the underlying at expiration
	double			strike,		// Option strike
	Q_OPT_TYPE		callPut 	// Option type (Q_OPT_PUT/Q_OPT_CALL etc.)
	);

DLLEXPORT double Q_PayoffL (
	double			logEndSpot,	// Logarithm of the underlying at expiration
	double			logStrike,	// Logarithm of the option strike
	double			strike,		// Option strike
	Q_OPT_TYPE		optType		// Option type (Q_OPT_PUT/Q_OPT_CALL etc.)
	);

DLLEXPORT int Q_KOWingMC(
	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT, Q_OPT_CALL etc.)
	Q_UP_DOWN     	upOrDown, 	// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_IN_OUT     	inOrOut, 	// Q_KNOCK_IN or Q_KNOCK_OUT
	Q_REBATE_TYPE	rebType,	// Q_IMMEDIATE or Q_DELAYED
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
    double 			barrier,	// Barrier value
	double 			rebate,	 	// Rebate when option expires dead
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	DT_CURVE		*disc,		// Domestic discount curve
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_MATRIX        *volPaths,	// Matrix containing volatility paths
	Q_VECTOR		*path_PVs,	// Returned present values of the paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);

DLLEXPORT double Q_Min( Q_VECTOR  *array ); 		// Find the minimum of Q_VECTOR 

DLLEXPORT double Q_Max( Q_VECTOR  *array ); 		// Find the maximum of Q_VECTOR 

DLLEXPORT double Q_ProbBarrHit(
	Q_UP_DOWN     	upOrDown, 	// Q_KNOCK_DOWN or Q_KNOCK_UP
	double 			barrier,	// Barrier value
	double			term,		// Time to expiration 
	Q_VECTOR        *path,		// Spot vector - one Monte Carlo path
	Q_VECTOR        *vpath,		// Volatility vector - one Monte Carlo path
	Q_VECTOR        *tpath		// Time vector for the path 
	);							

DLLEXPORT double Q_KOPathPV(
	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT, Q_OPT_CALL etc.)
	Q_UP_DOWN     	upOrDown, 	// Q_KNOCK_DOWN or Q_KNOCK_UP
	double 			strike,	 	// Option strike price
	double 			barrier,	// Barrier value
	double 			rebate,	 	// Rebate when option expires dead
	double			term,		// Time to expiration 
	Q_VECTOR        *path,		// Spot vector - one Monte Carlo path
	Q_VECTOR        *vpath,		// Volatility vector - one Monte Carlo path
	Q_VECTOR        *tpath,		// Time vector for the path 
	Q_VECTOR        *dpath		// Dicounting factors for the path 
	);							

DLLEXPORT int Q_OTWingMC(
	Q_UP_DOWN     	upOrDown, 	// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_REBATE_TYPE	payType,	// Payoff type: Q_IMMEDIATE or Q_DELAYED
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
	double 			payoff,	 	// Payoff when option hits the strike
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	DT_CURVE		*disc,		// Domestic discount curve
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_MATRIX        *volPaths,	// Matrix containing volatility paths
	Q_VECTOR		*path_PVs,	// Returned present values of the paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);

DLLEXPORT int Q_KODWingMC(
	Q_OPT_TYPE		callPut,		// Q_OPT_PUT or Q_OPT_CALL
	Q_REBATE_TYPE	rebType,		// Q_IMMEDIATE or Q_DELAYED
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			lowerBarrier,	// Lower barrier value
	double 			upperBarrier,	// Upper barrier value
	double 			lowerRebate,	// Rebate when option hits lower barrier
	double 			upperRebate,	// Rebate when option hits upper barrier
	double			vol,			// Average volatility (for control variate functions)
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	int				antiVarN,		// Number of antithetic variables
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	double			*Premium,		// Returned value of the option premium
	double			*StdError		// Returned standard error of the simulation
	);

DLLEXPORT int Q_AmortWingMC(
	Q_OPT_TYPE		callPut,        // Q_OPT_CALL, Q_OPT_PUT or Q_OPT_FWD
	Q_UP_DOWN       upOrDown,       // Knock-out direction
	double 			spot,	 		// Underlying spot price
	double     		barrier,      	// Knockout level
	DATE			prcDate,		// Pricing date
    DATE            koBeginDate,	// Start of knock-out period
    DATE            koEndDate,		// End of knock-out period
    Q_DATE_VECTOR   *AmortDates,  	// Amortization times
    Q_DATE_VECTOR   *AmortLag,  	// Dates where amortization kicks in
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	DT_CURVE 		*strike,   		// Strike curve
	Q_VECTOR        *Amounts,       // Vector of amounts on each pay date
	Q_VECTOR        *LowPrice,      // Low price for 100% amortization
	Q_VECTOR        *MidPrice,      // Mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // High price for 0% amortization
	Q_VECTOR        *LowPct,        // Percentage at lower level
    Q_VECTOR        *MidPct,        // Percentage of amortization middle level
    Q_VECTOR        *HighPct,       // Percentage at upper level
	int             zeroFlag,       // 1 if zero lag, otherwise
	int				antiVarN,		// Number of antithetic variables
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPath,		// Matrix containing volatility paths
	double			*Premium,		// Returned value of the option premium
	double			*StdError		// Returned standard error of the simulation
	);

DLLEXPORT double Q_AmortAdjustment(
	double 			spot,	 		// Underlying spot price
	int             n,		      	// Number of the pay date in an array
	Q_VECTOR        *LowPct,        // Percentage at lower level
	Q_VECTOR        *LowPrice,      // Low price for 100% amortization
    Q_VECTOR        *MidPct,        // Percentage of amortization middle level
	Q_VECTOR        *MidPrice,      // Mid price for some middle amortization
    Q_VECTOR        *HighPct,       // Percentage at upper level
	Q_VECTOR        *HighPrice      // High price for 0% amortization
	);

DLLEXPORT double Q_AmortPayoff (
	double			endingSpot,	// Value of the underlying at expiration
	double			strike,		// Option strike
	Q_OPT_TYPE		callPut 	// Option type (Q_OPT_PUT/Q_OPT_CALL etc.)
	);

DLLEXPORT int Q_AvgWingMC(
	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_AVG_TYPE  	avgType,	// Q_AVG_ARITHM, Q_AVG_GEOM or Q_AVG_HARM
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	DT_CURVE		*avgCurve,	// Averaging dates (with spot values for past dates) 
	DT_CURVE		*disc,		// Domestic discount curve
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_MATRIX        *volPaths,  // Matrix containing volatility paths
	Q_VECTOR		*path_PVs,	// Returned present values of the paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);
	
DLLEXPORT double Q_AvgPayoff(
	Q_OPT_TYPE		optType,	// Option type (Q_OPT_PUT/Q_OPT_CALL etc.)
	Q_AVG_TYPE  	avgType,	// Q_AVG_ARITHM, Q_AVG_GEOM or Q_AVG_HARM
	double			strike,		// Option strike
	Q_VECTOR        *spots  	// Spot value at averaging points
	);
	
DLLEXPORT int Q_ParisianWingMC(
 	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT, Q_OPT_CALL etc.)
	Q_UP_DOWN     	upOrDown, 	// Knock-out direction (Q_KNOCK_DOWN or Q_KNOCK_UP)
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
	double 			barrier,	// Barrier value
	double			vol,		// Average volatility (for control variate functions)
	double			discount,	// Domestic discout factor to expiration
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	Q_DATE_VECTOR	*koDates,	// Discrete knock-out dates
	int				nKO,		// Number of knock-outs required
	int				soFar,		// Number of knock-outs so far
	int				baseball,	// 1 if baseball option, 0 otherwise
	int				antiVarN,	// Number of antithetic variables
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_MATRIX        *volPaths,	// Matrix containing volatility paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);

DLLEXPORT int Q_HDFOWingMC(
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT, Q_OPT_CALL etc.)
	Q_IN_OUT     	inOrOut, 		// Appearing or disappearing (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_UP_DOWN     	upOrDownFO, 	// Fade-out direction (Q_KNOCK_DOWN or Q_KNOCK_UP)
	Q_UP_DOWN     	upOrDownKO, 	// Knock-out direction (Q_KNOCK_DOWN or Q_KNOCK_UP)
	Q_REBATE_TYPE	rebType,		// Knock-out rebate type (Q_IMMEDIATE or Q_DELAYED)
	Q_FADEOUT_TYPE	fadeOutType,	// Q_FADEOUT_RESURRECTING / Q_FADEOUT_NONRESURRRECTING
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			barrierFO,		// Fade-out barrier value
	double 			barrierKO,		// Knock-out barrier value
	double 			rebate,	 		// Rebate when option expires dead
	int				vanillaRebate,	// TRUE to get a vanilla option at KO, FALSE otheriwse
	int				maxFade,		// Number of fade-outs to fade out completely
	int				soFar,			// Number of fade-outs so far
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	Q_DATE_VECTOR	*fadeOutDates,	// Discrete fade-out dates
	DT_CURVE		*disc,			// Domestic discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_VECTOR		*path_PVs,		// Returned present values of the paths
	double			*Premium,		// Returned value of the option premium
	double			*StdError		// Returned standard error of the simulation
    );

DLLEXPORT int Q_ReservoirWingMC(
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			lastSpotRead,	// Last spot reading
	double			payoffPerUnit,	// Payoff per unit of the spot/strike difference
	int				accUpDownDiff,	// Accumulated up/down difference
	int 			strike,	 		// Option strike price
	int				ifStarted,		// TRUE if past first spot reading, FALSE otherwise
	DATE			prcDate,		// Pricing date
	Q_DATE_VECTOR	*sampleDates,	// Spot sampling dates
	DT_CURVE		*disc,			// Domestic discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_VECTOR		*path_PVs,		// Returned present values of the paths
	double			*Premium,		// Returned value of the option premium
	double			*StdError		// Returned standard error of the simulation
	);

DLLEXPORT int Q_KOWingMC_CV(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT, Q_OPT_CALL etc.)
	Q_UP_DOWN     	upOrDown, 		// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_IN_OUT     	inOrOut, 		// Q_KNOCK_IN or Q_KNOCK_OUT
	Q_REBATE_TYPE	rebType,		// Q_IMMEDIATE or Q_DELAYED
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			barrier,		// Barrier value
	double 			rebate,	 		// Rebate when option expires dead
	double			vol,			// Average volatility for control variates
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// Matrix containing degenerate volatility paths
	Q_MATRIX        *jSpotPaths,	// Matrix containing spot price paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*Beta			// Returned regression coefficient
	);

DLLEXPORT double Q_Mean( Q_VECTOR  *array ); 		// Find the mean of Q_VECTOR

DLLEXPORT int Q_AvgWingMC_CV(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_AVG_TYPE  	avgType,		// Q_AVG_ARITHM, Q_AVG_GEOM or Q_AVG_HARM
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	DT_CURVE		*avgCurve,		// Averaging dates (with spot values for past dates) 
	DT_CURVE		*disc,			// Domestic discount curve
	DT_CURVE		*fdisc,			// Foreign discount curve
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// Matrix containing degenerate volatility paths
	Q_MATRIX        *jSpotPaths,	// Matrix containing spot price paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*Beta			// Returned regression coefficient for control variate
	);

DLLEXPORT int Q_OptWingMC_CV(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			effVol,	 		// Effective volatility of the underlying
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	DT_CURVE		*disc,			// Domestic discount curve
	DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *jSpotPaths,	// Matrix containing spot price paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal		// Returned regression coefficient
	);

DLLEXPORT int Q_OTWingMC_CV(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_UP_DOWN     	upOrDown, 		// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_REBATE_TYPE	payType,		// Payoff type: Q_IMMEDIATE or Q_DELAYED
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			payoff,	 		// Payoff when option hits the strike
	double			effVol,			// Effective volatility for control variates
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// Matrix containing degenerate volatility paths
	Q_MATRIX        *jSpotPaths,	// Matrix containing spot price paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal		// Returned regression coefficient
	);

DLLEXPORT int Q_HDFOWingMC_CV(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT, Q_OPT_CALL etc.)
	Q_IN_OUT     	inOrOut, 		// Appearing or disappearing (Q_KNOCK_IN or Q_KNOCK_OUT)
	Q_UP_DOWN     	upOrDownFO, 	// Fade-out direction (Q_KNOCK_DOWN or Q_KNOCK_UP)
	Q_UP_DOWN     	upOrDownKO, 	// Knock-out direction (Q_KNOCK_DOWN or Q_KNOCK_UP)
	Q_REBATE_TYPE	rebType,		// Knock-out rebate type (Q_IMMEDIATE or Q_DELAYED)
	Q_FADEOUT_TYPE	fadeOutType,	// Q_FADEOUT_RESURRECTING / Q_FADEOUT_NONRESURRRECTING
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			barrierFO,		// Fade-out barrier value
	double 			barrierKO,		// Knock-out barrier value
	double 			rebate,	 		// Rebate when option knocks out
	double			vol,			// Average volatility for control variates
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	int				nGrid,			// Number of grid points for Q_MultiHDFO
	int				vanillaRebate,	// TRUE to get a vanilla option at KO, FALSE otherwise
	int				maxFade,		// Number of fade-outs to fade out completely
	int				soFar,			// Number of fade-outs so far
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	Q_DATE_VECTOR	*fadeOutDates,	// Discrete fade-out dates
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR		*lvol,			// Volatility curve for the underlying
	Q_VECTOR		*lvolT,			// Times for volatilities
	Q_VECTOR		*fwds, 			// Forwards
	Q_VECTOR		*fwdT,			// Times for forwards
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// Matrix containing degenerate volatility paths
	Q_MATRIX        *jSpotPaths,	// Matrix containing spot price paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*Beta			// Returned regression coefficient
	);

DLLEXPORT int Q_OptWingMC_CV3(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	double 			vol,	 		// Volatility of the underlying
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	Q_VECTOR  		*TimesVM,     	// Times for local volatility matrix
	Q_VECTOR     	*SpotsVM,     	// Spots for local volatility matrix
	Q_MATRIX    	*volMatrix,  	// Local volatility matrix
	DT_CURVE		*disc,			// Domestic discount curve
	DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *lSpotPaths,	// Matrix containing spot price paths with local vol only
	Q_MATRIX        *jSpotPaths,	// Matrix containing spot price paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
    double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal   	// Returned regression coefficient
	);

DLLEXPORT int Q_GenerateCVParameters (
	int				CVtype,				// Control variate type
	Q_VECTOR		*pathPVs,			// Array of payoffs Q_VECTORs
	double			*PremiumCV,			// Array of control variate premiums
	double			*BenchmarkPrice,	// Array of benchmark price
	double			*Beta,          	// Returned array of CV regression coefficients
    double			*CtrlVariate,		// Returned (generalized) control variate premium
	double			*Benchmark,			// Returned (generalized) benchmark price
	double			*CorrCoeff,			// Returned (generalized) correlation coefficient
	double			*BetaTotal   		// Returned (generalized) regression coefficient
	);

DLLEXPORT int Q_SingleCVParameters (
	Q_VECTOR        *regPathPVs,	// Payoffs of the target option on the regular paths
	Q_VECTOR        *pathPVsCV_1,	// Payoffs of the control variate on the degenerate paths
	double			*BetaCV_1,		// Returned beta coefficient for the first control variate
	double			*CorrCoeff		// Returned generalized correlation coefficient
	);

DLLEXPORT int Q_DoubleCVParameters (
	Q_VECTOR        *regPathPVs,	// Payoffs of the target option on the regular paths
	Q_VECTOR        *pathPVsCV_1,	// Payoffs of the first  CV on the degenerate paths
	Q_VECTOR        *pathPVsCV_2,	// Payoffs of the second CV on the degenerate paths
	double			*BetaCV_1,		// Returned beta coefficient for the first  control variate
	double			*BetaCV_2,		// Returned beta coefficient for the second control variate
	double			*CorrCoeff		// Returned generalized correlation coefficient
	);				   

DLLEXPORT int Q_TripleCVParameters (
	Q_VECTOR        *regPathPVs,	// Payoffs of the target option on the regular paths
	Q_VECTOR        *pathPVsCV_1,	// Payoffs of the first  CV on the degenerate paths
	Q_VECTOR        *pathPVsCV_2,	// Payoffs of the second CV on the degenerate paths
	Q_VECTOR        *pathPVsCV_3,	// Payoffs of the third  CV on the degenerate paths
	double			*BetaCV_1,		// Returned beta coefficient for the first  control variate
	double			*BetaCV_2,		// Returned beta coefficient for the second control variate
	double			*BetaCV_3,		// Returned beta coefficient for the third  control variate
	double			*CorrCoeff		// Returned generalized correlation coefficient
	);

DLLEXPORT int Q_OptWingMC_CV2(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	double 			initVol,	 	// Instantaneous volatility
	double			meanVol,		// Mean volatility
	double			effVol,			// Effective constant volatility
	double			beta,			// Strength of mean reversion
	double			sigma,			// Volatility of volatility
	double			effRho,			// Efective Heston's correlation
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	DT_CURVE		*disc,			// Domestic discount curve
	DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *cSpotPaths,	// Matrix containing spot price paths with SV and jumps
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
    double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal  	// Returned regression coefficient
	);

DLLEXPORT int Q_OTWingMC_CV2(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_UP_DOWN     	upOrDown, 		// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_REBATE_TYPE	payType,		// Payoff type: Q_IMMEDIATE or Q_DELAYED
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			payoff,	 		// Payoff when option hits the strike
	double 			initVol,	 	// Instantaneous volatility
	double			meanVol,		// Mean volatility
	double			effVol,			// Effective constant volatility
	double			beta,			// Strength of mean reversion
	double			sigma,			// Volatility of volatility
	double			effRho,			// Efective Heston's correlation
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// Matrix containing degenerate volatility paths
	Q_MATRIX        *cSpotPaths,	// Matrix containing spot price paths with jumps only
	Q_MATRIX        *cVolPaths,		// Matrix containing volatility paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal		// Returned regression coefficient
	);

DLLEXPORT int Q_WingSCorrMC (
	double          spot,			// Underlying spot price
	double          initVol,		// Instantaneous volatility
	double          initRho,		// Initial spot/vol correlation
	double			effVol,			// Effective constant volatility
	double			effRho,			// Effective spot/vol correlation
	Q_VECTOR        *lVol,			// Forward-forward reverting vols
	Q_VECTOR        *lVolT,			// Gridpoints for lVol
	Q_VECTOR        *beta,			// Mean-reversion strength
	Q_VECTOR        *sigma,			// Volatility of volatility
	Q_VECTOR        *gamma, 		// Mean-reversion strength of Z
	Q_VECTOR        *meanRho,		// Mean level of spot/vol correlation
	Q_VECTOR        *eps,			// Volatility of Z
	Q_VECTOR		*rhoCS,			// Correlation between Z and spot
	Q_VECTOR		*rhoCV,			// Correlation between Z and stoch vol
	Q_VECTOR		*rhoJ,			// Correlation of jumps in spot and volatility
	Q_VECTOR        *lambda,		// Mean of Poisson process for jumps
	Q_VECTOR        *jumpS_Mean,	// Mean of spot jumps in log terms
	Q_VECTOR        *jumpS_SD,		// Std dev of spot jumps in log terms
	Q_VECTOR        *jumpV_Mean,	// Mean of volatility jumps
    Q_VECTOR        *jumpV_SD,		// Std dev of volatility jumps
	Q_VECTOR        *fwds,			// Forwards vector
	Q_VECTOR        *fwdT,			// Gridpoints for forwards vector
	Q_VECTOR        *times,			// Gridpoints for Monte Carlo paths
	int             nPaths,			// Number of Monte Carlo paths
	int				saveLogSpot,	// TRUE if need log spot path, FALSE otherwise
	int             seed,			// Starting seed for random numbers gen.
	Q_MATRIX        *spotPaths,		// Returned matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Returned matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Returned matrix containing fully degenerate spot paths
	Q_MATRIX        *dVolPaths,		// Returned matrix containing fully degenerate vol paths
	Q_MATRIX        *cSpotPaths,	// Returned matrix containing spot paths with SV and jumps only
	Q_MATRIX        *cVolPaths,		// Returned matrix containing vol paths with SV and jumps only
	N_GENERIC_FUNC_3D	localVol,	// Pointer to a local vol function
	void			*Context		// Context for the local vol fuction
	);
	
DLLEXPORT int Q_OptWingMCL(
	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	DT_CURVE		*disc,		// Domestic discount curve
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_VECTOR		*path_PVs,	// Returned present values of the paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);

DLLEXPORT int Q_OTWingMCL(
	Q_UP_DOWN     	upOrDown, 	// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_REBATE_TYPE	payType,	// Payoff type: Q_IMMEDIATE or Q_DELAYED
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
	double 			payoff,	 	// Payoff when option hits the strike
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	DT_CURVE		*disc,		// Domestic discount curve
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_MATRIX        *volPaths,	// Matrix containing volatility paths
	Q_VECTOR		*path_PVs,	// Returned present values of the paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);

DLLEXPORT double Q_ProbBarrHitL(
	Q_UP_DOWN     	upOrDown, 	// Q_KNOCK_DOWN or Q_KNOCK_UP
	double 			logBarrier,	// Logarithm of barrier value
	double			logEndSpot, // Logarithm of the ending spot value
	double			term,		// Time to expiration 
	Q_VECTOR        *path,		// Spot vector - one Monte Carlo path
	Q_VECTOR        *vpath,		// Volatility vector - one Monte Carlo path
	Q_VECTOR        *tpath		// Time vector for the path 
	);							

DLLEXPORT int Q_OTWingMCLF(
	Q_UP_DOWN     	upOrDown, 	// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_REBATE_TYPE	payType,	// Payoff type: Q_IMMEDIATE or Q_DELAYED
	double			spot,		// Underlying spot price
	double 			strike,	 	// Option strike price
	double 			payoff,	 	// Payoff when option hits the strike
	DATE			prcDate,	// Pricing date
	DATE			expDate,	// Expiration date
	DT_CURVE		*disc,		// Domestic discount curve
	Q_VECTOR        *times,		// Time vector for spot paths 
	Q_MATRIX        *spotPaths,	// Matrix containing spot price paths
	Q_MATRIX        *volPaths,	// Matrix containing volatility paths
	Q_VECTOR		*path_PVs,	// Returned present values of the paths
	double			*Premium,	// Returned value of the option premium
	double			*StdError	// Returned standard error of the simulation
	);

DLLEXPORT int Q_OTWingMC_CV2F(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_UP_DOWN     	upOrDown, 		// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_REBATE_TYPE	payType,		// Payoff type: Q_IMMEDIATE or Q_DELAYED
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			payoff,	 		// Payoff when option hits the strike
	double 			initVol,	 	// Instantaneous volatility
	double			meanVol,		// Mean volatility
	double			effVol,			// Effective constant volatility
	double			beta,			// Strength of mean reversion
	double			sigma,			// Volatility of volatility
	double			effRho,			// Efective Heston's correlation
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// Matrix containing degenerate volatility paths
	Q_MATRIX        *cSpotPaths,	// Matrix containing spot price paths with jumps only
	Q_MATRIX        *cVolPaths,		// Matrix containing volatility paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal		// Returned regression coefficient
	);

DLLEXPORT int Q_OptLNWingMC_CV2(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	double 			initVol,	 	// Instantaneous volatility
	double			meanVol,		// Mean volatility
	double			effVol,			// Effective constant volatility
	double			beta,			// Strength of mean reversion
	double			sigma,			// Volatility of volatility
	double			effRho,			// Efective Heston's correlation
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	DT_CURVE		*disc,			// Domestic discount curve
	DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *cSpotPaths,	// Matrix containing spot price paths with SV and jumps
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
    double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal  	// Returned regression coefficient
	);

DLLEXPORT int Q_OptPhiWingMC_CV2(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_OPT_TYPE		callPut,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	double 			initVol,	 	// Instantaneous volatility
	double			meanVol,		// Mean volatility
	double			effVol,			// Effective constant volatility
	double			beta,			// Strength of mean reversion
	double			sigma,			// Volatility of volatility
	double			effRho,			// Efective Heston's correlation
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	double			phi,			// Exponent in the stoch. vol term
	DT_CURVE		*disc,			// Domestic discount curve
	DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *cSpotPaths,	// Matrix containing spot price paths with SV and jumps
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
    double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal  	// Returned regression coefficient
	);

DLLEXPORT int Q_OTPhiWingMC_CV2(
	int				CVtype,			// 0 if no CV, 1 if BSM, 2 if jumps, 12 if BSM+jumps
	Q_UP_DOWN     	upOrDown, 		// Q_KNOCK_DOWN or Q_KNOCK_UP
	Q_REBATE_TYPE	payType,		// Payoff type: Q_IMMEDIATE or Q_DELAYED
	double			spot,			// Underlying spot price
	double 			strike,	 		// Option strike price
	double 			payoff,	 		// Payoff when option hits the strike
	double 			initVol,	 	// Instantaneous volatility
	double			meanVol,		// Mean volatility
	double			effVol,			// Effective constant volatility
	double			beta,			// Strength of mean reversion
	double			sigma,			// Volatility of volatility
	double			effRho,			// Efective Heston's correlation
	double			lambda,      	// Mean of Poisson process for jumps
	double			jumpMean,    	// Mean of the jump size in log terms
	double			jumpSD,      	// Variance of the jump size in log terms
	double			phi,			// Exponent in the stoch. vol term
	DATE			prcDate,		// Pricing date
	DATE			expDate,		// Expiration date
	DT_CURVE		*disc,			// Domestic discount curve
    DT_CURVE		*fdisc,			// Foreign discount curve
	Q_VECTOR        *times,			// Time vector for spot paths 
	Q_MATRIX        *spotPaths,		// Matrix containing spot price paths
	Q_MATRIX        *volPaths,		// Matrix containing volatility paths
	Q_MATRIX        *dSpotPaths,	// Matrix containing degenerate spot price paths
	Q_MATRIX        *dVolPaths,		// Matrix containing degenerate volatility paths
	Q_MATRIX        *cSpotPaths,	// Matrix containing spot price paths with jumps only
	Q_MATRIX        *cVolPaths,		// Matrix containing volatility paths with jumps only
	double			*Premium,		// Returned value of the option premium
	double			*StdError,		// Returned standard error of the simulation
	double			*RawPremium,	// Returned value of the option premium without CV
	double			*RawStdError,	// Returned standard error of the simulation without CV
	double			*CtrlVariate,	// Returned value of the control variate on degenerate paths
	double			*Benchmark,		// Returned benchmark price
	double			*CorrCoeff,		// Returned correlation coefficient
	double			*BetaTotal		// Returned regression coefficient
	);
				   
#endif

