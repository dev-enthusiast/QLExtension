/* $Header: /home/cvs/src/quant/src/q_jump.h,v 1.22 2001/11/27 23:13:49 procmon Exp $ */
/****************************************************************
**
**	Q_JUMP.H - routines to handle jump diffusion processes		
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.22 $
**
****************************************************************/

#if !defined( IN_Q_JUMP_H )
#define IN_Q_JUMP_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QWINGS int Q_JDKO (
	Q_OPT_TYPE          isCall,      // option type
	Q_UP_DOWN           upOrDown,    // direction of knockout
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              outVal,      // knockout level
	double              rebate,      // rebate upon knocking out
	Q_REBATE_TYPE       rebType,     // immediate or delayed rebate
	double              vol,         // overall volatility
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	double              ccRd,        // domestic interest rate
	double              ccRf,        // foreign interest rate
	int                 nSteps,      // number of timesteps
	int                 nPaths,      // number of trials
	double				*premium,	 // returned premium
	double				*stderror	 // returned standard error
	);

EXPORT_C_QWINGS double Q_JDOpt (
	Q_OPT_TYPE          isCall,      // option type
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              vol,         // overall volatility
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	double              ccRd,        // domestic interest rate
	double              ccRf         // foreign interest rate
	);

EXPORT_C_QWINGS double Q_JDBinOpt (
	Q_OPT_TYPE          isCall,      // option type
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              vol,         // overall volatility
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	double              ccRd,        // domestic interest rate
	double              ccRf,        // foreign interest rate
	double              payoff       // payoff if expiring in the money	
	);
	
EXPORT_C_QWINGS double Q_JDImpVol (
	Q_OPT_TYPE          isCall,      // option type
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	double              ccRd,        // domestic interest rate
	double              ccRf,        // foreign interest rate
	double				price,		 // Option premium
	double				EstimatedVol // first guess for volatility
	);

EXPORT_C_QWINGS double Q_JDKOC (
	Q_OPT_TYPE          isCall,      // option type
	Q_UP_DOWN           upOrDown,    // direction of knockout
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              outVal,      // knockout level
	double              rebate,      // rebate upon knocking out
	Q_REBATE_TYPE       rebType,     // immediate or delayed rebate
	Q_VECTOR            *vols,       // volatility of diffusion
	Q_VECTOR            *volT,       // gridpoints for volatility curve
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	Q_VECTOR            *Rd,         // domestic interest rate
	Q_VECTOR            *rdT,        // times for domestic rates
	Q_VECTOR            *fwds,       // foreign interest rate
	Q_VECTOR            *fwdT,       // gridpoints for forwards
	int                 nSteps,      // number of timesteps
	int                 nPaths,      // number of trials
	Q_RETURN_TYPE       retType      // return premium, error or raw premium
	);

EXPORT_C_QWINGS double Q_JDOptA (
	Q_OPT_TYPE          isCall,      // option type
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	Q_VECTOR            *vols,       // volatility of diffusion
	Q_VECTOR            *volT,       // gridpoints for volatility curve
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	Q_VECTOR            *Rd,         // domestic interest rate
	Q_VECTOR            *rdT,        // times for domestic rates
	Q_VECTOR            *fwds,       // foreign interest rate
	Q_VECTOR            *fwdT,       // gridpoints for forwards
	int                 nSteps       // number of timesteps
	);

EXPORT_C_QWINGS double Q_JDKODC (
	Q_OPT_TYPE          isCall,      // option type
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              lowerKO,     // knockout level
	double              upperKO,     // knockout level
	double              lowerRebate, // rebate upon knocking out downward
	double              upperRebate, // rebate upon knocking out upward
	Q_REBATE_TYPE       rebType,     // immediate or delayed rebate
	Q_VECTOR            *vols,       // volatility of diffusion
	Q_VECTOR            *volT,       // gridpoints for volatility curve
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	Q_VECTOR            *Rd,         // domestic interest rate
	Q_VECTOR            *rdT,        // times for domestic rates
	Q_VECTOR            *fwds,       // foreign interest rate
	Q_VECTOR            *fwdT,       // gridpoints for forwards
	int                 nSteps,      // number of timesteps
	int                 nPaths,      // number of trials
	Q_RETURN_TYPE       retType      // return premium, error or raw premium
	);

EXPORT_C_QWINGS double Q_JDBinKODC (
	Q_OPT_TYPE          isCall,      // option type
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              lowerKO,     // knockout level
	double              upperKO,     // knockout level
	double              lowerRebate, // rebate upon knocking out downward
	double              upperRebate, // rebate upon knocking out upward
	Q_REBATE_TYPE       rebType,     // immediate or delayed rebate
	Q_VECTOR            *vols,       // volatility of diffusion
	Q_VECTOR            *volT,       // gridpoints for volatility curve
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	Q_VECTOR            *Rd,         // domestic interest rate
	Q_VECTOR            *rdT,        // times for domestic rates
	Q_VECTOR            *fwds,       // foreign interest rate
	Q_VECTOR            *fwdT,       // gridpoints for forwards
	double  			payoff,      // payoff for finishing in the money
	int                 nSteps,      // number of timesteps
	int                 nPaths       // number of trials
	);

EXPORT_C_QWINGS double Q_JDBinKOC (
	Q_OPT_TYPE          isCall,      // option type
	Q_UP_DOWN           upOrDown,    // direction of knockout
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              outVal,      // knockout level
	double              rebate,      // rebate upon knocking out
	Q_REBATE_TYPE       rebType,     // immediate or delayed rebate
	Q_VECTOR            *vols,       // volatility of diffusion
	Q_VECTOR            *volT,       // gridpoints for volatility curve
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	Q_VECTOR            *Rd,         // domestic interest rate
	Q_VECTOR            *rdT,        // times for domestic rates
	Q_VECTOR            *fwds,       // foreign interest rate
	Q_VECTOR            *fwdT,       // gridpoints for forwards
	double              payoff,      // payoff if expiring in the money
	int                 nSteps,      // number of timesteps
	int                 nPaths       // number of trials
	);

EXPORT_C_QWINGS double Q_JDKOCDelayed (
	Q_OPT_TYPE          isCall,      // option type
	Q_UP_DOWN           upOrDown,    // direction of knockout
	double	            spot,        // current spot
	double             	strike,		 // strike price of option
	double              outVal,      // knockout level
	double              rebate,      // rebate upon knocking out
	Q_REBATE_TYPE       rebType,     // immediate or delayed rebate
	Q_VECTOR            *vols,       // volatility of diffusion
	Q_VECTOR            *volT,       // gridpoints for volatility curve
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              kotime,      // start of knockout period
	double              time,        // time to expiration
	Q_VECTOR            *Rd,         // domestic interest rate
	Q_VECTOR            *rdT,        // times for domestic rates
	Q_VECTOR            *fwds,       // foreign interest rate
	Q_VECTOR            *fwdT,       // gridpoints for forwards
	int                 nSteps,      // number of timesteps
	int                 nPaths       // number of trials
	);

EXPORT_C_QWINGS int Q_JDPaths (
	double	            spot,        // current spot
	Q_VECTOR            *vols,       // volatility of diffusion
	Q_VECTOR            *volT,       // gridpoints for volatility curve
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	Q_VECTOR            *fwds,       // foreign interest rate
	Q_VECTOR            *fwdT,       // gridpoints for forwards
	int                 nSteps,      // number of timesteps
	int                 nPaths,      // number of trials
	int                 seed,        // seed for random number generator
    Q_MATRIX   		    *spotPaths   // returned matrix containing spot price paths
	);

EXPORT_C_QWINGS double Q_JDImpBinStrike (
	Q_OPT_TYPE          isCall,      // option type
	double	            spot,        // current spot
	double             	vol,		 // current volatility
	double              lambda,      // mean of poisson process
	double              jumpMean,    // mean of size of jump in log terms
	double              jumpSD,      // variance of size of jump in log terms
	double              time,        // time to expiration
	double              ccRd,        // domestic interest rate
	double              ccRf,        // foreign interest rate
	double				price 		 // Option premium
	);

EXPORT_C_QWINGS int Q_JDLogCharFn (
    double       X, 	  		// x variable representing log of forward
	Q_VECTOR     *lvol,    		// instantaneous diffusion volatility
	Q_VECTOR     *lvolT,   		// vector of nodes for diffusion volatility and jump parameters
	Q_VECTOR     *lambda,       // jump frequencies
	Q_VECTOR     *jumpMean, 	// jump means
	Q_VECTOR     *jumpVar,  	// jump variances
	double       time,          // time to expiration
	double       phi,           // fourier dummy variable
	double       *freal,        // real part of answer
	double       *fim           // imaginary part of answer
	);
								
EXPORT_C_QWINGS int Q_JDLogCharFnMod (
    double       X, 	  		// x variable representing log of forward
	Q_VECTOR     *lvol,    		// instantaneous diffusion volatility
	Q_VECTOR     *lvolT,   		// vector of nodes for diffusion volatility and jump parameters
	Q_VECTOR     *lambda,       // jump frequencies
	Q_VECTOR     *jumpMean, 	// jump means
	Q_VECTOR     *jumpVar,  	// jump variances
	double       time,          // time to expiration
	double       phi,           // fourier dummy variable
	double       *freal,        // real part of answer
	double       *fim           // imaginary part of answer
	);
								
#endif

