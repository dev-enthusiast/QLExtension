/* $Header: /home/cvs/src/quant/src/q_stvol.h,v 1.53 2001/01/17 23:58:53 demeor Exp $ */
/****************************************************************
**
**	Q_STVOL.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.53 $
**
****************************************************************/

#if !defined( IN_Q_STVOL_H )
#define IN_Q_STVOL_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QWINGS int Q_SVCharFcn (
	double       X,        // initial value of log of spot
	double       volSq,    // initial value of instantaneous vol squared
	double       mu,       // drift term in spot
	double       beta,     // mean reversion factor on volatility
	double       sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       rho,      // correlation of spot and volatility
	double       tau,      // time to expiration
	double       vbar,     // variance that stochastic variance is reverting to
	double       lambda,   // extra risk term in drift for volatility
	double       phi,      // free parameter for characteristic function
	double       *Fr,      // returned real part
	double       *Fi       // returned imaginary part
	);

#if 0
EXPORT_C_QWINGS double Q_SVOpt (
    Q_OPT_TYPE   callPut,  // option type
    double       fwd,      // current forward price at expiration
    double       strike,   // strike price
	double       volSq,    // initial value of instantaneous vol squared
	double       vbar,     // variance that stochastic variance is reverting to
	double       beta,     // mean reversion factor on volatility
	double       sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       rho,      // correlation of spot and volatility
	double       time,     // time to expiration
	double       ccRd,     // risk-free or domestic continuous interest rate
	double       stdev,    // standard deviation on initial variance
	double       deltax    // interval for integration
	);
#endif

EXPORT_C_QWINGS double Q_SVOptC (
    Q_OPT_TYPE   callPut,  // option type
    double       fwd,      // current forward price at expiration
    double       strike,   // strike price
	double       vol,      // instantaneous vol (short vol)
	Q_VECTOR     *lvol,    // long term limiting vol, piecewise constant
	Q_VECTOR     *lvolT,   // vector of nodes for limiting vol
	double       beta,     // mean reversion factor on volatility
	double       sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       rho,      // correlation of spot and volatility
	double       time,     // time to expiration
	double       ccRd,     // risk-free or domestic continuous interest rate
	double       stdev,    // standard deviation on initial variance
	double       deltax    // interval for integration
	);
		
EXPORT_C_QWINGS double Q_SVKO (
    Q_OPT_TYPE   	callPut,  // option type
    Q_UP_DOWN    	upOrDown, // direction of knockout
    double       	spot,     // current forward price at expiration
    double       	strike,   // strike price
    double       	outVal,   // knockout level
    double       	rebate,   // rebate 
    Q_REBATE_TYPE	rebType,  // rebate type (delayed or immediate)
    double          initvol,  // initial instantaneous volatility
	Q_VECTOR     	*lvol,    // forward-forward reverting vols
	Q_VECTOR     	*lvolT,   // time vector for vols
	double       	beta,     // mean reversion factor on volatility
	double       	sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      // correlation of spot and volatility
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	Q_VECTOR        *Rd,      // domestic rate vector
	Q_VECTOR        *RdT,     // domestic rate time vector
    int             nSteps,   // number of timesteps
    int             nPaths,   // number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   // return premium or error
	);

EXPORT_C_QWINGS int Q_SVKOPremErr (
    Q_OPT_TYPE   	callPut,  // option type
    Q_UP_DOWN    	upOrDown, // direction of knockout
    double       	spot,     // current forward price at expiration
    double       	strike,   // strike price
    double       	outVal,   // knockout level
    double       	rebate,   // rebate 
    Q_REBATE_TYPE	rebType,  // rebate type (delayed or immediate)
    double          initvol,  // initial instantaneous volatility
	Q_VECTOR     	*lvol,    // forward-forward reverting vols
	Q_VECTOR     	*lvolT,   // time vector for vols
	double       	beta,     // mean reversion factor on volatility
	double       	sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      // correlation of spot and volatility
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	Q_VECTOR        *Rd,      // domestic rate vector
	Q_VECTOR        *RdT,     // domestic rate time vector
    int             nSteps,   // number of timesteps
    int             nPaths,   // number of trials for Monte Carlo test
    Q_VECTOR        *vars     // vector of premium, raw premium error and raw error
	);

EXPORT_C_QWINGS double Q_SVKOFD (
    Q_OPT_TYPE   	callPut,  // option type
    Q_UP_DOWN    	upOrDown, // direction of knockout
    double       	spot,     // current forward price at expiration
    double       	strike,   // strike price
    double       	outVal,   // knockout level
    double       	rebate,   // rebate 
    Q_REBATE_TYPE	rebType,  // rebate type (delayed or immediate)
	Q_VECTOR      	*lvol,    // vector of mean volatilities
	Q_VECTOR 		*lvolT,   // time vector for lvol
	double          initvol,  // initial short-term vol
	double       	beta,     // mean reversion factor on volatility
	double       	sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       	corr,     // correlation of spot and volatility
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	Q_VECTOR        *Rd,      // domestic rate vector
	Q_VECTOR        *RdT,     // domestic rate time vector
    int             nGrid1,   // number of gridpoints in spot dimension
    int             nGrid2    // number of gridpoints in vol dimension
	);

EXPORT_C_QWINGS double Q_SVKODFD (
    Q_OPT_TYPE   	callPut,  		// option type
    double       	spot,     		// current forward price at expiration
    double       	strike,   		// strike price
    double          lowerKO,  		// lower knockout barrier
    double          upperKO,  		// upper knockout barrier
    double       	lowerRebate,   	// rebate at lower barrier
    double       	upperRebate,   	// rebate at upper barrier
    Q_REBATE_TYPE	rebType,  		// rebate type (delayed or immediate)
	Q_VECTOR      	*lvol,    		// vector of mean volatilities
	Q_VECTOR 		*lvolT,   		// time vector for lvol
	double          initvol,  		// initial short-term vol
	double       	beta,     		// mean reversion factor on volatility
	double       	sigma,    		// coeff of sqrt(vol) in stochastic term for vol
	double       	corr,     		// correlation of spot and volatility
	double       	time,     		// time to expiration
	Q_VECTOR     	*fwds,    		// forward vector
	Q_VECTOR     	*fwdT,    		// forward time vector
	Q_VECTOR        *Rd,      		// domestic rate vector
	Q_VECTOR        *RdT,     		// domestic rate time vector
    int             nGrid1,   		// number of gridpoints in spot dimension
    int             nGrid2    		// number of gridpoints in vol dimension
	);	

EXPORT_C_QWINGS double Q_SVOptA (
    Q_OPT_TYPE   	callPut,  // option type
    Q_OPT_STYLE    	euroAmer, // American or European
    double       	spot,     // current forward price at expiration
    double       	strike,   // strike price
	Q_VECTOR      	*lvol,    // vector of mean volatilities
	Q_VECTOR 		*lvolT,   // time vector for lvol
	double          initvol,  // initial short-term vol
	double       	beta,     // mean reversion factor on volatility
	double       	sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       	corr,     // correlation of spot and volatility
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	Q_VECTOR        *Rd,      // domestic rate vector
	Q_VECTOR        *RdT,     // domestic rate time vector
    int             nGrid1,   // number of gridpoints in spot dimension
    int             nGrid2    // number of gridpoints in vol dimension
	);	

EXPORT_C_QWINGS double Q_RealizedVolOpt (
	Q_OPT_TYPE      callPut,  		// option type
    double       	spot,     		// current spot price
    Q_VECTOR        *history,       // previous fixes
    double          strike,   		// strike price on volatility
    double          initmean, 		// initial mean instantaneous volatility
    double          initsd,   		// initial uncertainty in instantaneous var
	double	     	lvol,     		// forward-forward reverting vol
	double       	beta,     		// mean reversion factor on volatility
	double       	sigma,    		// coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      		// correlation of spot and volatility
	double       	time,     		// time to expiration
	double          annualize,      // multiply by sqrt(this) to get correct vol
	double	     	fwd, 	  		// forward
	double          ccRd,     		// discount rate
    int             nSteps,   		// number of timesteps
    int             nPaths,   		// number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   		// return error or option value
	);

EXPORT_C_QWINGS double Q_SVKOD (
    Q_OPT_TYPE   	callPut,  		// option type
    double       	spot,     		// current forward price at expiration
    double       	strike,   		// strike price
    double       	lowerKO,   		// lower knockout level
    double       	upperKO,   		// upper knockout level
    double       	lowerRebate,   	// lower rebate
    double       	upperRebate,   	// upper rebate
    Q_REBATE_TYPE	rebType,  		// rebate type (delayed or immediate)
    double          initvol,  		// initial instantaneous volatility
	Q_VECTOR     	*lvol,    		// forward-forward reverting vols
	Q_VECTOR     	*lvolT,   		// time vector for vols
	double       	beta,     		// mean reversion factor on volatility
	double       	sigma,    		// coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      		// correlation of spot and volatility
	double       	time,     		// time to expiration
	Q_VECTOR     	*fwds,    		// forward vector
	Q_VECTOR     	*fwdT,    		// forward time vector
	Q_VECTOR        *Rd,      		// domestic rate vector
	Q_VECTOR        *RdT,     		// domestic rate time vector
    int             nSteps,   		// number of timesteps
    int             nPaths,   		// number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   		// return premium or error
	);

EXPORT_C_QWINGS double Q_SVAverage (
    Q_OPT_TYPE   	callPut,  		// option type
    double       	spot,     		// current forward price at expiration
    double          currAvg,        // current average
    double       	strike,   		// strike price
    double          initvol,  		// initial instantaneous volatility
	Q_VECTOR     	*lvol,    		// forward-forward reverting vols
	Q_VECTOR     	*lvolT,   		// time vector for vols
	double       	beta,     		// mean reversion factor on volatility
	double       	sigma,    		// coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      		// correlation of spot and volatility
	DATE            prcDate,        // pricing date
	Q_DATE_VECTOR   *avgDates,      // averaging dates
	Q_VECTOR     	*fwds,    		// forward vector
	Q_VECTOR     	*fwdT,    		// forward time vector
	double          ccRd,           // risk-free interest rate
	int             navg,           // number of past average points
    int             nPaths,   		// number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   		// return premium or error
	);

EXPORT_C_QWINGS int Q_SVPaths (
    double       	spot,     		// current forward price at expiration
    double          initvol,  		// initial instantaneous volatility
	Q_VECTOR     	*lvol,    		// forward-forward reverting vols
	Q_VECTOR     	*lvolT,   		// time vector for vols
	double       	beta,     		// mean reversion factor on volatility
	double       	sigma,    		// coeff of sqrt(vol) in stochastic term for vol
	double       	rho,      		// correlation of spot and volatility
	Q_VECTOR        *times,         // points on the path
	Q_VECTOR     	*fwds,    		// forward vector
	Q_VECTOR     	*fwdT,    		// forward time vector
    int             nPaths,   		// number of paths
    int             tmpSeed,  		// initial seed parameter
    Q_MATRIX        *spotPaths,     // returned matrix containing spot price paths
    Q_MATRIX        *volPaths       // returned matrix containing volatility paths
	);

EXPORT_C_QWINGS double Q_SVOptGen (
    Q_OPT_TYPE   callPut,  		// option type
    double       fwd,      		// current forward price at expiration
    double       strike,   		// strike price
	double       vol,      		// instantaneous vol (short vol)
	Q_VECTOR     *lvol,    		// long term limiting vol, piecewise constant
	Q_VECTOR     *lvolT,   		// vector of nodes for limiting vol
	Q_VECTOR     *beta,    		// mean reversion factor on volatility
	Q_VECTOR     *sigma,   		// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR     *rho,     		// correlation of spot and volatility
	double       time,     		// time to expiration
	double       ccRd,     		// risk-free or domestic continuous interest rate
	double       stdev,    		// standard deviation on initial variance
	double       deltaphi,   	// interval for integration
	double       tolerance      // size of last term of integral
	);

EXPORT_C_QWINGS double Q_SVKOGen (
    Q_OPT_TYPE   	callPut,  // option type
    Q_UP_DOWN    	upOrDown, // direction of knockout
    double       	spot,     // current forward price at expiration
    double       	strike,   // strike price
    double       	outVal,   // knockout level
    double       	rebate,   // rebate 
    Q_REBATE_TYPE	rebType,  // rebate type (delayed or immediate)
    double          initvol,  // initial instantaneous volatility
	Q_VECTOR     	*lvol,    // forward-forward reverting vols
	Q_VECTOR     	*lvolT,   // time vector for vols
	Q_VECTOR        *beta,    // mean reversion factor on volatility
	Q_VECTOR        *sigma,   // coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *rho,     // correlation of spot and volatility
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	Q_VECTOR        *Rd,      // domestic rate vector
	Q_VECTOR        *RdT,     // domestic rate time vector
    int             nSteps,   // number of timesteps
    int             nPaths,   // number of trials for Monte Carlo test
    Q_RETURN_TYPE   retType   // return premium or error or raw premium
	);
   
EXPORT_C_QWINGS int Q_SVKOPremErrGen (
    Q_OPT_TYPE   	callPut,  // option type
    Q_UP_DOWN    	upOrDown, // direction of knockout
    double       	spot,     // current forward price at expiration
    double       	strike,   // strike price
    double       	outVal,   // knockout level
    double       	rebate,   // rebate 
    Q_REBATE_TYPE	rebType,  // rebate type (delayed or immediate)
    double          initvol,  // initial instantaneous volatility
	Q_VECTOR     	*lvol,    // forward-forward reverting vols
	Q_VECTOR     	*lvolT,   // time vector for vols
	Q_VECTOR        *beta,    // mean reversion factor on volatility
	Q_VECTOR        *sigma,   // coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *rho,     // correlation of spot and volatility
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	Q_VECTOR        *Rd,      // domestic rate vector
	Q_VECTOR        *RdT,     // domestic rate time vector
    int             nSteps,   // number of timesteps
    int             nPaths,   // number of trials for Monte Carlo test
    Q_VECTOR        *vars     // vector of premium, raw premium error and raw error
	);

EXPORT_C_QWINGS int Q_SVPathsGen (
    double       	spot,     		// current forward price at expiration
    double          initvol,  		// initial instantaneous volatility
	Q_VECTOR     	*lvol,    		// forward-forward reverting vols
	Q_VECTOR     	*lvolT,   		// time vector for vols
	Q_VECTOR        *beta,    		// mean reversion factor on volatility
	Q_VECTOR        *sigma,   		// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR        *rho,     		// correlation of spot and volatility
	Q_VECTOR        *times,   		// points on the path
	Q_VECTOR     	*fwds,    		// forward vector
	Q_VECTOR     	*fwdT,    		// forward time vector
    int             nPaths,   		// number of trials for Monte Carlo test
    int             tmpSeed,  		// initial seed parameter
    Q_MATRIX        *spotPaths,     // returned matrix containing spot price paths
    Q_MATRIX        *volPaths       // returned matrix containing volatility paths
	);

EXPORT_C_QWINGS int Q_SVLogCharFn (
    double       X, 	  		// x variable representing log of spot
	double       var,      		// instantaneous square of volatility
	Q_VECTOR     *lvol,    		// long term limiting vol, piecewise constant
	Q_VECTOR     *lvolT,   		// vector of nodes for limiting vol
	Q_VECTOR     *beta,    		// mean reversion factor on volatility
	Q_VECTOR     *sigma,   		// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR     *rho,     		// correlation of spot and volatility
	double       time,          // time to expiration
	double       stdev,    		// standard deviation on initial variance
	double       phi,           // fourier dummy variable
	double       *freal,        // real part of answer
	double       *fim           // imaginary part of answer
	);

EXPORT_C_QWINGS int Q_SVLogCharFnMod (
    double       X, 	  		// x variable representing log of spot
	double       var,      		// instantaneous square of volatility
	Q_VECTOR     *lvar,    		// long term limiting vol, piecewise constant
	Q_VECTOR     *lvolT,   		// vector of nodes for limiting vol
	Q_VECTOR     *beta,    		// mean reversion factor on volatility
	Q_VECTOR     *sigma,   		// coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR     *rho,     		// correlation of spot and volatility
	double       time,          // time to expiration
	double       stdev,    		// standard deviation on initial variance
	double       phi,           // fourier dummy variable
	double       *freal,        // real part of answer
	double       *fim           // imaginary part of answer
	);
																
EXPORT_C_QWINGS int Q_SVOpt_Get_lvol (
    Q_OPT_TYPE   callPut,  // option type
    double       fwd,      // current forward price at expiration
    double       strike,   // strike price
	double       vol,      // instantaneous vol (short vol)
	Q_VECTOR     *lvol,    // OUTPUT long term limiting vol, piecewise constant
	Q_VECTOR     *lvolT,   // vector of nodes for limiting vol
	double       beta,     // mean reversion factor on volatility
	double       sigma,    // coeff of sqrt(vol) in stochastic term for vol
	double       rho,      // correlation of spot and volatility
	double       ccRd,     // risk-free or domestic continuous interest rate
	double       stdev,    // standard deviation on initial variance
	double       deltax,   // interval for integration
	Q_VECTOR	 *priceT,  // price of options of maturities given by lvolT
	Q_VECTOR	 *price,   // price of options of maturities given by lvolT
	double		lvolMin,
	double		lvolMax,
	double		epsilon
);

EXPORT_C_QWINGS int Q_SVBondOpt (
    Q_OPT_TYPE   	callPut,  // option type
    double       	spot,     // current bond yield
    double       	strike,   // strike price
    Q_VECTOR        *coupon,  // coupon vector for bond
    Q_VECTOR        *couponT, // times when coupons are paid
    double          period,   // period used for calculating bond price
	Q_VECTOR      	*lvol,    // vector of mean volatilities
	Q_VECTOR 		*lvolT,   // time vector for lvol
	double          initvol,  // initial short-term vol
	Q_VECTOR       	*beta,    // mean reversion factor on volatility
	Q_VECTOR       	*sigma,   // coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR       	*corr,    // correlation of spot and volatility
	double          start,    // start of option period
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	double          ccRd,     // rate used for discounting
	Q_VECTOR		*SVec,    // output spot vector, pre-allocated
	Q_VECTOR		*VVec,    // output volatility vector, pre-allocated
    Q_MATRIX        *OptMat   // output option price matrix, pre-allocated
	);	

EXPORT_C_QWINGS int Q_SVBondOptATM (
    Q_OPT_TYPE   	callPut,  // option type
    double       	spot,     // current bond yield
    Q_VECTOR        *coupon,  // coupon vector for bond
    Q_VECTOR        *couponT, // times when coupons are paid
    double          period,   // period used for calculating bond price
	Q_VECTOR      	*lvol,    // vector of mean volatilities
	Q_VECTOR 		*lvolT,   // time vector for lvol
	double          initvol,  // initial short-term vol
	Q_VECTOR       	*beta,    // mean reversion factor on volatility
	Q_VECTOR       	*sigma,   // coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR       	*corr,    // correlation of spot and volatility
	double          start,    // start of option period
	double       	time,     // time to expiration
	Q_VECTOR     	*fwds,    // forward vector
	Q_VECTOR     	*fwdT,    // forward time vector
	double          ccRd,     // rate used for discounting
	Q_VECTOR		*SVec,    // output spot vector, pre-allocated
	Q_VECTOR		*VVec,    // output volatility vector, pre-allocated
    Q_MATRIX        *OptMat   // output option price matrix, pre-allocated
	);	
	
EXPORT_C_QWINGS double Q_BondPrice (
	Q_VECTOR      *coupon,  // coupon vector
	Q_VECTOR      *couponT, // coupon times vector
	double        period,   // period used to define the yield
	double        yield,    // bond yield
	double        time      // time in the future at which price is desired
	);

EXPORT_C_QWINGS int Q_SVBondOptFitter (
    double       	spot,      // current bond yield
	Q_MATRIX        *strikes,  // strike prices for calls
    Q_MATRIX       	*prices,   // prices of calls
    Q_VECTOR        *coupon,   // coupon vector for bond
    Q_VECTOR        *couponT,  // times when coupons are paid
    double          period,    // period used for calculating bond price
	double          initvol,   // initial short-term vol
	Q_VECTOR        *times,    // times to expiration
	Q_VECTOR     	*fwds,     // forward vector
	Q_VECTOR     	*fwdT,     // forward time vector
	Q_VECTOR        *ccRd,     // one discount rate per option expiration
	int             nGrid1,    // gridsize in spot dimension
	int             nGrid2,    // gridsize in volatility dimension
	double          tolerance, // parameter determining when to stop amoeba
	Q_VECTOR       	*lvol,     // backed out limiting volatilities
	Q_VECTOR       	*beta,     // backed out mean reversion factor on volatility
	Q_VECTOR       	*sigma,    // backed out coeff of sqrt(vol) in stochastic term for vol
	Q_VECTOR       	*corr,     // backed out correlation of spot and volatility
	Q_VECTOR        *Error     // vector of least-squares errors
	);
	
EXPORT_C_QWINGS int Q_SVBondOptFitOne (
    double       	spot,      // current bond yield
	Q_VECTOR        *strikes,  // strike prices for calls
    Q_VECTOR       	*prices,   // prices of calls
    Q_VECTOR        *coupon,   // coupon vector for bond
    Q_VECTOR        *couponT,  // times when coupons are paid
    double          period,    // period used for calculating bond price
	double          initvol,   // initial short-term vol
	Q_VECTOR        *times,    // times to expiration of the options
	Q_VECTOR     	*fwds,     // forward vector
	Q_VECTOR     	*fwdT,     // forward time vector
	Q_VECTOR        *ccRd,     // one discount rate per option expiration
	int             nGrid1,    // gridsize in spot dimension
	int             nGrid2,    // gridsize in volatility dimension
	int             fixed,     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none
	double          tolerance, // parameter determining when to stop amoeba
	double       	*lvol,     // backed out limiting volatility
	double       	*beta,     // backed out mean reversion factor on volatility
	double       	*sigma,    // backed out coeff of sqrt(vol) in stochastic term for vol
	double       	*corr,     // backed out correlation of spot and volatility
	double          *Error,    // vector of least-squares error
	double          *options,  // return option values
	int             fittype    // 0 if price-based, 1 if volatility-based
	);

EXPORT_C_QWINGS double Grid3Convolve(
	double		*A,
	Q_MATRIX	*Mat,
	int			i,
	int			j
	);

typedef struct {
	Q_OPT_TYPE   	callPut;   // option type
	double       	spot;      // current spot yield
	Q_VECTOR       	*strikes;  // array of strikes for expiration T
	Q_VECTOR        *prices;   // market option prices, used for objective function
	Q_VECTOR        *coupon;   // array of coupons
	Q_VECTOR        *couponT;  // array of coupon payment times
	double          period;    // period used in calculating price from yield
	Q_VECTOR      	*lvol;     // array of mean-reverting limiting volatilities
	Q_VECTOR 		*lvolT;    // nodes for lvol, beta, corr and sigma
	double          initvol;   // initial instantaneous volatility
	Q_VECTOR       	*beta;     // mean reversion coefficients
	Q_VECTOR       	*sigma;    // vol of vol
	Q_VECTOR       	*corr;     // correlation between the 2 Wiener terms
	double       	time;      // time to expiration
	Q_VECTOR     	*fwds;     // forward yield curve
	Q_VECTOR     	*fwdT;     // nodes for forward yield curve
	double          ccRd;      // rate used for discounting
	int             nGrid1;    // size of spot grid
	int             nGrid2;    // size of volatility grid
	} Q_BONDOPT_PARMS;		

typedef struct {
	Q_OPT_TYPE   	callPut;   // option type
	double       	spot;      // current spot yield
	Q_VECTOR       	*strikes;  // array of strikes for expiration T
	Q_VECTOR        *prices;   // market option prices, used for objective function
	Q_VECTOR        *coupon;   // array of coupons
	Q_VECTOR        *couponT;  // array of coupon payment times
	double          period;    // period used in calculating price from yield
	double          initvol;   // initial instantaneous volatility
	Q_VECTOR       	*times;    // times to expiration of options
	Q_VECTOR     	*fwds;     // forward yield curve
	Q_VECTOR     	*fwdT;     // nodes for forward yield curve
	Q_VECTOR        *ccRd;     // rates used for discounting the options
	int             nGrid1;    // size of spot grid
	int             nGrid2;    // size of volatility grid
	double          extra;     // extra parameter that is fixed
	int             fixed;     // 1 fixes beta, 2 corr, 3 lvol, 4 sigma, 0 none 
	double          *options;  // returned option values, if desired
	int             fittype;   // 0 if price-based, 1 if volatility-based
	} Q_BONDOPT_PARMS_SIMPLE;		

EXPORT_C_QWINGS int Q_HigginsStep (
	Q_VECTOR        *Values,    // values at the end of the time step
	double          lowX,       // low value of log of spot
	double          lowV,       // low value of square of volatility
	double          lowC,       // low value of variable driving correlation
	double          lRebate,    // lower rebate in spot direction
	double          uRebate,    // upper rebate in spot direction
	double          dx,         // grid spacing of log of spot
	double          dv,         // grid spacing of square of volatility
	double          dc,         // grid spacing of variable driving correlation
	double          drift,      // spot drift (r-y)
	double          phi,        // constant term in drift of volatility squared
	double          beta,       // mean reversion for vol squared
	double          sigma,      // coeff of vol in variance SDE
	double          theta,      // constant term in drift of correlation variable
	double          alpha,      // mean reversion for correlation variable
	double          omega,      // normal "vol" of correlation variable
	double          rhoxc,      // correlation of spot and correlation variable
	double          rhovc,      // correlation of vol squared and correlation variable
	double          deltat,     // time step
	double          ccRd,       // discount rate during this time step
	int             ngx,        // number of gridpoints in spot direction
	int             ngv,        // number of gridpoints in vol direction
	int             ngc,        // number of gridpoints in correlation variable dimension
	double          tolerance,  // tolerance in conjugate gradient algorithm
	Q_VECTOR        *NewValues	// return modified values
	);

EXPORT_C_QWINGS int Q_ConvertParms (
	double     *realparms,   // vector of 4 real numbers
	double     *beta,        // beta output
	double     *corr,        // output correlation
	double     *lvol,        // output long volatility
	double     *sigma        // output volatility of volatility
	);

EXPORT_C_QWINGS int Q_ConvertParmsInv (
	double     *realparms,   // vector of 4 real numbers to be modified
	double     beta,         // beta input
	double     corr,         // input correlation
	double     lvol,         // input long volatility
	double     sigma         // input volatility of volatility
	);

EXPORT_C_QWINGS int Q_HigginsLN (
	Q_VECTOR        *Values,    // values at the end of the time step
	double          lowX,       // low value of log of spot
	double          lowV,       // low value of square of volatility
	double          lowC,       // low value of variable driving correlation
	double          lRebate,    // lower rebate in spot direction
	double          uRebate,    // upper rebate in spot direction
	double          dx,         // grid spacing of log of spot
	double          dv,         // grid spacing of log of volatility
	double          dc,         // grid spacing of variable driving correlation
	double          drift,      // spot drift (r-y)
	double          phi,        // constant term in drift of log of volatility
	double          beta,       // mean reversion for log of volatility
	double          sigma,      // volatility of volatility
	double          theta,      // constant term in drift of correlation variable
	double          alpha,      // mean reversion for correlation variable
	double          omega,      // normal "vol" of correlation variable
	double          rhoxc,      // correlation of spot and correlation variable
	double          rhovc,      // correlation of log(vol) and correlation variable
	double          deltat,     // time step
	double          ccRd,       // discount rate during this time step
	int             ngx,        // number of gridpoints in spot direction
	int             ngv,        // number of gridpoints in vol direction
	int             ngc,        // number of gridpoints in correlation variable dimension
	double          tolerance,  // tolerance in conjugate gradient algorithm
	Q_VECTOR        *NewValues	// return modified values
	);

EXPORT_C_QWINGS int Q_HigginsGen (
	Q_VECTOR        *Values,    // values at the end of the time step
	double          lowX,       // low value of log of spot
	double          lowV,       // low value of square of volatility
	double          lowC,       // low value of variable driving correlation
	double          lRebate,    // lower rebate in spot direction
	double          uRebate,    // upper rebate in spot direction
	double          dx,         // grid spacing of log of spot
	double          dv,         // grid spacing of volatility squared
	double          dc,         // grid spacing of variable driving correlation
	double          drift,      // spot drift (r-y)
	double          lvol,       // limiting value of volatility
	double          phi,        // stochastic term in vol squared is v^phi*sigma*dz2
	double          beta,       // mean reversion for log of volatility
	double          sigma,      // volatility of volatility
	double          theta,      // constant term in drift of correlation variable
	double          alpha,      // mean reversion for correlation variable
	double          omega,      // normal "vol" of correlation variable
	double          rhoxc,      // correlation of spot and correlation variable
	double          rhovc,      // correlation of log(vol) and correlation variable
	double          deltat,     // time step
	double          ccRd,       // discount rate during this time step
	int             ngx,        // number of gridpoints in spot direction
	int             ngv,        // number of gridpoints in vol direction
	int             ngc,        // number of gridpoints in correlation variable dimension
	double          tolerance,  // tolerance in conjugate gradient algorithm
	Q_VECTOR        *NewValues	// return modified values
	);

#endif

