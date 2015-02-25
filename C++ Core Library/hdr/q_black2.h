/* $Header: /home/cvs/src/quant/src/q_black2.h,v 1.6 2007/05/11 17:02:00 e13749 Exp $ */
/****************************************************************
**
**	Q_BLACK2.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_BLACK2_H )
#define IN_Q_BLACK2_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double	Q_Opt(
	Q_OPT_TYPE  	callput,  // Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,	  // Underlyer spot price
	double 			strike,	  // Option strike price
	double 			vol,	  // Underlyer volatility
	double 			term,	  // Time to expiration, actual 365
	double 			rd,		  // Domestic interest rate, continuous form
	double 			rf,		  // Foreign interest rate, continuous form
	Q_RETURN_TYPE   retType	  // return premium, delta, kappa, gamma or theta
	);					
	
EXPORT_C_QUANT double Q_OptMRCarry (
    Q_OPT_TYPE		callPut,        // option type
    double 			forward,        // forward price
    double 			strike,         // strike price
    double 			vol,            // instantaneous exchange rate volatility
    double 			shortcvol,      // instantaneous carry "volatility"
    double 			longcvol,       // vol of term carry
    double 			corr,           // correlation of carry and exchange rate
    double 			term,           // term of term carry
    double 			time,           // time to expiration
    double 			ccRd            // domestic rate
    );
	
EXPORT_C_QUANT void	Q_BlackScholes2(
    Q_OPT_TYPE type, 
    double spot, 
    double strike, 
    double vol, 
    double term,
	double r1, 
    double r2, 
    double *premRet, 
    double *deltaRet, 
    double *kappaRet, 
    double *gammaRet, 
    double *thetaRet
    );

EXPORT_C_QUANT void	Q_BlackScholes2_NoVolCheck(
    Q_OPT_TYPE type, 
    double spot, 
    double strike, 
    double vol, 
    double term,
	double r1, 
    double r2, 
    double *premRet, 
    double *deltaRet, 
    double *kappaRet, 
    double *gammaRet, 
    double *thetaRet
    );


EXPORT_C_QUANT void Q_EuropeanCall2 (
				double spot,
				double strike,
				double volatility,
				double time_to_expiration,
				double domestic_interest_rate,
				double foreign_interest_rate,
				double *price,
				double *delta,
				double *kappa,
				double *gamma,
				double *theta );

EXPORT_C_QUANT void Q_EuropeanPut2 (
				double spot,
				double strike,
				double volatility,
				double time_to_expiration,
				double domestic_interest_rate,
				double foreign_interest_rate,
				double *price,
				double *delta,
				double *kappa,
				double *gamma,
				double *theta );

#endif
