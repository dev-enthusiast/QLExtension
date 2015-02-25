/* $Header: /home/cvs/src/quant/src/q_gasdev.h,v 1.5 2000/03/03 17:54:30 goodfj Exp $ */
/****************************************************************
**
**	Q_GASDEV.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_Q_GASDEV_H )
#define IN_Q_GASDEV_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

EXPORT_C_QBASIC double Q_GaussDev (  
	int	Seed		// any integer 
);

EXPORT_C_QBASIC double Q_GasDev(int *idum);

EXPORT_C_QBASIC double Q_IntervalMax(
	double S0,						 // beginning spot price
	double S1,						 // ending spot price
	double vol,						 // volatility
	double t,						 // length of interval
	int *idum						 // random seed
	);

EXPORT_C_QBASIC double Q_IntervalMin(
	double S0,						 // beginning spot price
	double S1,						 // ending spot price
	double vol,						 // volatility
	double t,						 // length of interval
	int *idum						 // random seed
	);

#endif

