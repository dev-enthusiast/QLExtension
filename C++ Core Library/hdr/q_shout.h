/* $Header: /home/cvs/src/quant/src/q_shout.h,v 1.14 2000/03/02 15:01:57 goodfj Exp $ */
/****************************************************************
**
**	Q_SHOUT.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.14 $
**
****************************************************************/

#if !defined( IN_Q_SHOUT_H )
#define IN_Q_SHOUT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QEXOTIC double Q_ShoutOTM (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
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

EXPORT_C_QEXOTIC double Q_ShoutITM (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
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

EXPORT_C_QEXOTIC double Q_Cliquet (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR        *reset,     // times for resetting
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of points in price space
    );

EXPORT_C_QEXOTIC double Q_FloatStrikeA (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			const1,	 	// first strike constant (linear term)
	double 			const2,	 	// second strike constant (constant term)
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
EXPORT_C_QEXOTIC double Q_Improvement (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          multiplier, // reset to this times spot
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
    int             nShouts,    // number of shouts allowed
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );

EXPORT_C_QEXOTIC double Q_ShoutWindow (
	Q_OPT_TYPE    	callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	 	// Underlyer spot price
	double 			strike,	 	// Option strike price
	double          offset,     // offset from spot for revised strike
    Q_VECTOR 		*vol,		// Underlyer volatility	curve
    Q_VECTOR 		*volT,      // grid for volatilities
    double          startex,    // start shout period
    double          endex,      // end shout period
    double 			time,	 	// Time to expiration
    Q_VECTOR 		*Rd,     	// Domestic rate curve
    Q_VECTOR 		*rdT,       // grid for domestic rate curve
    Q_VECTOR 		*fwds,     	// forward curve
    Q_VECTOR 		*fwdT,      // grid for forward curve
	int          	nSteps, 	// number of timesteps
	int          	gridsize	// number of intervals in price space
    );
    	            
#endif


