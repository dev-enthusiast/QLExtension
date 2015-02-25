/* $Header: /home/cvs/src/quant/src/q_cphedg.h,v 1.6 2000/03/02 10:26:57 goodfj Exp $ */
/****************************************************************
**
**	Q_CPHEDG.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_CPHEDG_H )
#define IN_Q_CPHEDG_H

#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

	
EXPORT_C_QUANT double Q_OptPofExer (
	Q_OPT_TYPE callPut,		// call or put
	double spot, 			// spot price
	double strike, 			// strike price 
	double vol, 			// volatility 
	double time, 			// time to expiration 
	double ccRd, 			// domestic (disc) continuous interest rate
	double ccRf				// foreign (yield) continuous interest rate
	);
#endif
