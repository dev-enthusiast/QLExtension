/* $Header: /home/cvs/src/quant/src/q_fwdfwd.h,v 1.2 2000/03/03 17:54:29 goodfj Exp $ */
/****************************************************************
**
**	Q_FWDFWD.H		- Forward-forward Functions
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/


#if !defined( IN_Q_FWDFWD_H )
#define IN_Q_FWDFWD_H

#include <q_base.h>

EXPORT_C_QBASIC double Q_ForwardInterestRate	( double RateAC, double RateAB, double TermAC, double TermAB );
EXPORT_C_QBASIC double Q_ForwardVolatility		( double VolAC, double VolAB, double TermAC, double TermAB );

#endif
