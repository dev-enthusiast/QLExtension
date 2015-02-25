/* $Header: /home/cvs/src/quant/src/q_lake.h,v 1.6 2000/03/02 12:26:39 goodfj Exp $ */
/****************************************************************
**
**	Q_LAKE.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_LAKE_H )
#define IN_Q_LAKE_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT double Q_OptALake	(
	Q_OPT_TYPE		callPut,	// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 			spot,		// spot price
	double 			strike,		// strike price
	double 			volatility,	// volatility
	double 			time,		// time to expiration 
	double 			ccRd,		// domestic (discount) interest rates
	double 			ccRf,		// foreign (yield or dividend) interest rates
	Q_RETURN_TYPE	retType		// return premium or delta
	);
	
EXPORT_C_QUANT int Q_Lake	(
	Q_OPT_TYPE	optype,		// Option type (Q_OPT_PUT or Q_OPT_CALL)
	double 		spot,
	double 		strike,
	double 		volatility,
	double 		time_to_expiration,
	double 		domestic_interest_rate,
	double 		foreign_interest_rate,
	double 		*price,
	double 		*delta,
	double 		*bdry 		// early exercise trigger
	);

#endif

