/* $Header: /home/cvs/src/quant/src/q_rate.h,v 1.9 2000/05/17 10:31:19 goodfj Exp $ */
/****************************************************************
**
**	Q_RATE.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_Q_RATE_H )
#define IN_Q_RATE_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


EXPORT_C_QBASIC double	Q_s2cRate(
	double rd,		  // simple rate to convert
	double term		  // time from the present in years
	);

EXPORT_C_QBASIC double	Q_c2sRate(
	double rd,		  // continuous rate to convert
	double term		  // time from the present in years
	);
	
EXPORT_C_QBASIC double	Q_ToContinuous(double Rate, DATE start, DATE end, Q_COMPOUNDING_TYPE rateType, int dayCountType);
EXPORT_C_QBASIC double	Q_FromContinuous(double ContRate, DATE start, DATE end, Q_COMPOUNDING_TYPE rateType, int dayCountType);
EXPORT_C_QBASIC double	Q_DiscountFactor(double Rate, double term );

EXPORT_C_QBASIC double Q_ToContTerm	(
	double	rate,				// Rate to convert
	double  term,               // time period
	Q_COMPOUNDING_TYPE
			rateType,		    // Interest rate type
	int		dayCount			// Day count convention
	);

EXPORT_C_QBASIC double Q_FromContTerm (
	double	rate,				// Rate to convert
	double  term,               // time period
	Q_COMPOUNDING_TYPE
			rateType,		    // Interest rate type
	int		dayCount			// Day count convention
	);

#endif

