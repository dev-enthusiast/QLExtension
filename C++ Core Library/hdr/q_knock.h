/* $Header: /home/cvs/src/quant/src/q_knock.h,v 1.24 2000/03/23 13:48:18 goodfj Exp $ */
/****************************************************************
**
**	Q_KNOCK.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.24 $
**
****************************************************************/

#if !defined( IN_Q_KNOCK_H )
#define IN_Q_KNOCK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QKNOCK double Q_KOProb (
	Q_UP_DOWN   upOrDown,
    double      spot,
	double      outVal,
	double      vol,
	double      time,
	double      ccRd,
	double      ccRf
	);

EXPORT_C_QKNOCK double Q_KOLife (
	Q_UP_DOWN   upOrDown,
    double      spot,
	double      outVal,
	double      vol,
	double      time,
	double      ccRd,
	double      ccRf
	);

EXPORT_C_QKNOCK double Q_KOLifeC (
	double 			spot,	 	// Underlyer spot price
    double 			outVal,		// Knock-out price
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK void Q_MasterDiscKO(
	Q_OPT_TYPE  	callPut,      // option type
	Q_UP_DOWN   	upOrDown,     // direction of knockout
	Q_ALIVE_DEAD    knockedOut,	  // Already knocked out or not
	Q_IN_OUT    	inOrOut,   	  // knockin or knockout
	double 			spot,	   	  // Underlyer spot price
	double 			strike,	   	  // Option strike price
    double 			outVal,	   	  // Knock-out price
    double 			rebate,	   	  // Knockout rebate
    Q_REBATE_TYPE   rebType,   	  // Immediate or delayed till exp date
    double 			vol,	   	  // Underlyer volatility
    double 			time,	   	  // Time to expiration
    int         	*kosteps, 	  // vector of times for knockout
    double 			settle,    	  // Time to settlement
    double 			ccRd,      	  // Domestic rate, or risk free rate
    double 			ccRf,      	  // Foreign rate, or dividend rate
    double      	range,        // range for grid
	int         	nSteps,	      // number of steps
    double 			*premRet,  	  // Returned option premium
    double 			*deltaRet  	  // Returned option delta
    );

EXPORT_C_QKNOCK int Q_ExplicitDiscKO (
	Q_OPT_TYPE  	callPut,      // option type
	Q_UP_DOWN   	upOrDown,     // direction of knockout
    double     		spot,		  // current spot
    double     		strike,		  // current strike
    double     		outVal,		  // knockout level
    double     		rebate,		  // rebate
    Q_REBATE_TYPE   rebType,   	  // Immediate or delayed till exp date
    int        		nSteps,		  // number of steps
    int        		*kosteps,     // vector of (1 if ko, 0 if not)
    double     		range,		  // ratio of highest price to middle
	double     		vol,          // volatility
	double     		timeToExp,    // time to expiration
	double     		rate,         // domestic rate
	double     		frate,        // foreign rate
	double     		*premRet,     // returned premium
	double     		*delta        // returned delta
    );

EXPORT_C_QKNOCK double Q_KOBin (
	Q_OPT_TYPE  	isCall,       // option type
	Q_UP_DOWN   	upOrDown,     // direction of knockout
	Q_IN_OUT    	inOrOut,   	  // knockin or knockout
	double 			spot,	 	  // Underlyer spot price
	double 			strike,	 	  // Option strike price
    double 			outVal,	 	  // Knock-out price
    double 			vol,		  // Underlyer volatility
    double 			time,	 	  // Time to expiration
    double 			settle,   	  // Time from settlement date to expiration settlement date
    double 			ccRd,     	  // Domestic rate, or risk free rate
    double 			ccRf,     	  // Foreign rate, or dividend rate
    double 			payoff,   	  // final payoff for winning
	Q_RETURN_TYPE   retType       // return premium or delta
	);
    
EXPORT_C_QKNOCK int Q_BinaryKO(
	Q_OPT_TYPE  	callPut,      // option type
	Q_UP_DOWN   	upOrDown,     // direction of knockout
	Q_ALIVE_DEAD    knockedOut,	  // Already knocked out or not
	Q_IN_OUT    	inOrOut,   	  // knockin or knockout
	double 			spot,	 	  // Underlyer spot price
	double 			strike,	 	  // Option strike price
    double 			outVal,	 	  // Knock-out price
    double 			vol,		  // Underlyer volatility
    double 			time,	 	  // Time to expiration
    double 			settle,   	  // Time from settlement date to expiration settlement date
    double 			ccRd,     	  // Domestic rate, or risk free rate
    double 			ccRf,     	  // Foreign rate, or dividend rate
    double 			payoff,   	  // final payoff for winning
    double 			*premRet, 	  // Returned option premium
    double 			*deltaRet 	  // Returned option delta
    );

EXPORT_C_QKNOCK double Q_KO (
	Q_OPT_TYPE    	isCall,	    // Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown, 	// Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    double 			outVal,		// Knock-out price
    double 			rebate,	 	// Knockout rebate
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    double 			vol,		// Underlyer volatility
    double 			time,	 	// Time to expiration
    double 			ccRd,     	// Domestic rate, or risk free rate
    double 			ccRf,     	// Foreign rate, or dividend rate
    Q_RETURN_TYPE	retType     // return premium or delta
    );

EXPORT_C_QKNOCK double Q_KOC (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown, 	// Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    double 			outVal,		// Knock-out price
    double 			rebate,	 	// Knockout rebate
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_KOCoi (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown, 	// Direction of Knockout
	Q_IN_OUT     	inOrOut, 	// appearing or disappearing
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    double 			outVal,		// Knock-out price
    double 			rebate,	 	// Knockout rebate
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_BinKOCoi (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown, 	// Direction of Knockout
	Q_IN_OUT     	inOrOut, 	// appearing or disappearing
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    double 			outVal,		// Knock-out price
    double 			rebate,	 	// Knockout rebate
    Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    double          payoff,     // payoff for finishing in the money
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_KOFwd (
	Q_UP_DOWN   	upOrDown,     // direction of knockout
	double 			spot,	 	  // Underlyer spot price
	double 			strike,	 	  // Option strike price
    double 			outVal,	 	  // Knock-out price
    double 			vol,		  // Underlyer volatility
    double 			time,	 	  // Time to expiration
    double 			ccRd,     	  // Domestic rate, or risk free rate
    double 			ccRf    	  // Foreign rate, or dividend rate
	);

EXPORT_C_QKNOCK void Q_KnockOut(
	Q_OPT_TYPE    	isCall,	    // Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN     	upOrDown, 	// Direction of Knockout
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double 			out,		// Knock-out price
	double 			rebate,	 	// Knockout rebate
	Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
	double 			vol,		// Underlyer volatility
	double 			time,	 	// Time to expiration
	double 			settle,   	// Time to settlement
	double 			ccRd,     	// Domestic rate, or risk free rate
	double 			ccRf,     	// Foreign rate, or dividend rate
	double 			*premRet, 	// Returned option premium
	double 			*deltaRet 	// Returned option delta
	);

EXPORT_C_QKNOCK double Q_KnockOutAvgLife (
	Q_UP_DOWN   upOrDown, 	// Direction of knockout
	double 		spot,	 	// Underlyer spot price
	double 		outVal,	 	// Knock-out price
	double 		vol, 	 	// Underlyer volatility
	double 		time,	 	// Time to expiration
	double 		settle,   	// Time to settlement
	double 		ccRd,     	// Domestic rate, or risk free rate
	double 		ccRf      	// Foreign rate, or dividend rate
	);

EXPORT_C_QKNOCK double Q_KnockOutProb (
	Q_UP_DOWN   upOrDown, 	// Direction of knockout
	double 		spot,	 	// Underlyer spot price
	double 		outVal,	 	// Knock-out price
	double 		vol, 	 	// Underlyer volatility
	double 		time,	 	// Time to expiration
	double 		settle,   	// Time to settlement
	double 		ccRd,     	// Domestic rate, or risk free rate
	double 		ccRf      	// Foreign rate, or dividend rate
	);

EXPORT_C_QKNOCK void Q_MasterKO(
	Q_OPT_TYPE  	isCall,	   	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN   	upOrDown,   // Direction of knockout
	Q_ALIVE_DEAD	knockedOut, // Already knocked out or not
	Q_IN_OUT		inOrOut,    // knockin or knockout
	double 			spot,	    // Underlyer spot price
	double 			strike,	    // Option strike price
	double 			outVal,	    // Knock-out price
	double 			rebate,	    // Knockout rebate
	Q_REBATE_TYPE	rebType,	// Immediate or delayed till exp date
	double 			vol,		// Underlyer volatility
	double 			time,	    // Time to expiration
	double 			settle,     // Time to settlement
	double 			ccRd,       // Domestic rate, or risk free rate
	double 			ccRf,       // Foreign rate, or dividend rate
	double 			*premRet,   // Returned option premium
	double 			*deltaRet   // Returned option delta
	);

EXPORT_C_QKNOCK void Q_NewKO(
	Q_OPT_TYPE    	isCall,	  // Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_UP_DOWN    	upOrDown, // Direction of knockout
	double 			spot,	  // Underlyer spot price
	double 			strike,	  // Option strike price
	double 			outVal,   // Knock-out price
	double 			rebate,	  // Knockout rebate
	Q_REBATE_TYPE   rebType,  // Immediate or delayed till exp date
	double 			vol,	  // Underlyer volatility
	double 			time,	  // Time to expiration
	double 			ccRd,	  // Domestic rate, or risk free rate
	double 			ccRf,	  // Foreign rate, or dividend rate
	double 			*premRet  // Returned option premium
	);

#endif
