/* $Header: /home/cvs/src/quant/src/q_binkod.h,v 1.23 2000/03/03 14:51:53 goodfj Exp $ */
/****************************************************************
**
**	Q_BINDKO.H  routines to handle adjustable binary double knockouts
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.23 $
**
****************************************************************/

#if !defined( IN_Q_BINDKO_H )
#define IN_Q_BINDKO_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QKNOCK double Q_BinKODAdj (
    double    spot,           // spot price
    double    A,              // lower knockout
    double    B,              // upper knockout
    double    vol,            // volatility
    double    time,           // time to expiration
    double    ccRd,           // domestic rate
    double    ccRf,           // foreign rate
    int       nadj,           // maximum number of adjustments
    int       nsteps          // number of timesteps in the integral
    );

EXPORT_C_QKNOCK double Q_OptimalRangeB (
    double    spot,           // spot price
    double    width,          // fixed width of range
    double    vol,            // volatility
    double    time,           // time to expiration
    double    ccRd,           // domestic rate
    double    ccRf            // foreign rate
    );

EXPORT_C_QKNOCK double Q_BinKODCoi (
	Q_OPT_TYPE    	callPut,			// Option type (Q_OPT_PUT or Q_OPT_CALL)
	Q_IN_OUT     	inOrOut, 			// appearing or disappearing
	double 			spot,	 			// Underlyer spot price
	double 			strike,	 			// Option strike price
    double      	lowknockout,   		// lower knockout level
    double      	highknockout,  		// upper knockout level
    double      	lowerRebate,   		// rebate paid upon lower ko
    double      	upperRebate,   		// rebate paid upon higher ko
    Q_REBATE_TYPE	rebType,			// Immediate or delayed till exp date
    Q_VECTOR 		*vol,				// Underlyer volatility	curve
    Q_VECTOR 		*volT,      		// grid for volatilities
    double 			time,	 			// Time to expiration
    Q_VECTOR 		*Rd,     			// Domestic rate curve
    Q_VECTOR 		*rdT,       		// grid for domestic rate curve
    Q_VECTOR 		*fwds,     			// forward curve
    Q_VECTOR 		*fwdT,      		// grid for forward curve
    double          payoff,				// payoff if in the money at expiration
	int          	nSteps, 			// number of timesteps
	int          	gridsize			// number of intervals in price space
    );

EXPORT_C_QKNOCK double Q_BinKODDisc2 (
    double     		spot,		  // current spot
    double     		lowerKO, 	  // lower knockout level
    double     		upperKO, 	  // lower knockout level
	DT_CURVE     	*volCurve,    // volatility curve
	DATE            prcDate,      // pricing date
    Q_DATE_VECTOR   *koDates,     // dates on which it can knock out
	double     		rate,         // domestic rate
	DT_CURVE        *fwdCurve,    // forward curve
    int        		nSteps		  // number of steps per day
    );
        
#endif
