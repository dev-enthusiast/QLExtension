/* $Header: /home/cvs/src/quant/src/q_pknock.h,v 1.18 2000/03/23 13:48:19 goodfj Exp $ */
/****************************************************************
**
**	Q_PKNOCK.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.18 $
**
****************************************************************/

#if !defined( IN_Q_PKNOCK_H )
#define IN_Q_PKNOCK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_KOPInterval(
	Q_OPT_TYPE      callPut,   // option type
	Q_UP_DOWN       upOrDown,  // direction of knockout
    double      	spot,	   // current spot price
    double      	strike,	   // strike price on option
    double      	knockout,  // knockout level
    double      	rebate,	   // rebate upon knockout
    Q_REBATE_TYPE   rebType,   // immediate or delayed rebate
    double      	vol0,	   // volatility to start of ko period
    double      	vol1,	   // volatility to end of ko period
    double      	vol2,	   // volatility to expiration
    double      	t0,		   // start of ko period
    double      	t1,		   // end of knockout period
    double      	t2,		   // time to expiration
    double      	ccRd0,	   // domestic rate to start of ko period
    double      	ccRd1,	   // domestic rate to end of ko period
    double      	ccRd2,	   // domestic rate to expiration
    double      	ccRf0,	   // foreign rate to start of ko period
    double      	ccRf1,	   // foreign rate to end of ko period
    double      	ccRf2	   // foreign rate to expiration
    );

EXPORT_C_QKNOCK double Q_BinA (
	Q_UP_DOWN  	    upOrDown,  // upward or downward knockout
	double      	spot,	   // current spot price
	double      	knockout,  // knockout level
	double      	rebate,	   // rebate upon knockout
	Q_REBATE_TYPE   rebType,   // immediate or delayed rebate
	double      	vol0,	   // volatility to start of ko period
	double      	vol1,	   // volatility to end of ko period
	double      	t0,		   // start of ko period
	double      	t1,		   // end of knockout period
	double      	Rd0,	   // domestic rate to start of ko period
	double      	Rd1,	   // domestic rate to end of ko period
	double      	Rf0,	   // foreign rate to start of ko period
	double      	Rf1 	   // foreign rate to end of ko period
	);
	
EXPORT_C_QKNOCK double Q_KOP2(
	Q_OPT_TYPE  	callPut,  // option type
    Q_UP_DOWN   	upOrDown, // direction of knockout
    double      	spot,	  // current spot price
    double      	strike,	  // strike price on option
    double      	knockout, // knockout level
    double      	rebate,	  // rebate upon knockout
    Q_REBATE_TYPE   rebType,  // immediate or delayed rebate
    double      	vol1,	  // volatility to end of ko period
    double      	vol2,     // volatility to expiration
    double      	t1,	      // end of knockout period
    double      	t2,	      // time to expiration
    double      	ccRd1,    // domestic rate to end of ko period
    double      	ccRd2,    // domestic rate to expiration
    double      	ccRf1,	  // foreign rate to end of ko period
    double			ccRf2	  // foreign rate to expiration
    );

EXPORT_C_QKNOCK void Q_MasterKOP2 (
	Q_OPT_TYPE    	isCall,     	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN    	upOrDown,   	// Direction of knockout
	Q_ALIVE_DEAD    knockedOut, 	// Already knocked out or not
	Q_IN_OUT    	inOrOut,    	// knockin or knockout
	double 			spot,	   		// Underlyer spot price
	double 			strike,	   		// Option strike price
    double 			outVal,	   		// Knock-out price
    double 			rebate,	   		// Knockout rebate
    Q_REBATE_TYPE   rebType,		// Immediate or delayed till exp date
    double 			shortvol,   	// volatility until end of knockout period
    double 			vol,			// Underlyer volatility
    double 			endko,      	// Time to end of knockout period
    double          endkosettle, 	// settlement time for end of ko period
    double 			time,	   		// Time to expiration
    double 			settle,     	// Time to settlement
    double      	ccRd1,      	// domestic rate to end of ko period
    double      	ccRd2,      	// domestic rate to expiration
    double      	ccRf1,	    	// foreign rate to end of ko period
    double			ccRf2,	    	// foreign rate to expiration
    double 			*premRet,   	// Returned option premium
    double 			*deltaRet   	// Returned option delta
    );
EXPORT_C_QKNOCK double Q_KOCDelayed (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown, 	// Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    double 			outVal,		// Knock-out price
    double 			rebate,	 	// Knockout rebate
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double          kotime,     // start of knockout period
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_KOPIntervalC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown,   // Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    double 			outVal,  	// knockout barrier
    double 			rebate,	 	// rebate upon knockout
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double          kotime,     // start of knockout period
    double          endko,      // end of knockout period
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_KOProRataD (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown,   // Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double          minmax,     // current extremum (if ko period is underway)
	double 			strike,	 	// Option strike price
    double 			lowerKO,  	// level where 100% (0%) knocks out
    double          upperKO,    // level where 0% (100%) knocks out 
    double          rebate,     // delayed rebate, also prorated
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    Q_VECTOR        *times,     // Fixing times where extremum is calculated
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_KOProRataMCD(
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown,   // Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double          minmax,     // current extremum (if ko period is underway)
	double 			strike,	 	// Option strike price
    double 			lowerKO,  	// level where 100% (0%) knocks out
    double          upperKO,    // level where 0% (100%) knocks out 
    double          rebate,     // delayed rebate for knocking out
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    Q_VECTOR        *times,     // Fixing times where extremum is calculated
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nPaths, 	// number of Monte Carlo simulations
	Q_RETURN_TYPE   retType     // return premium or error
    );
                            	
#endif
