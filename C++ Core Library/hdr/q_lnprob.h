/* $Header: /home/cvs/src/quant/src/q_lnprob.h,v 1.5 2000/03/03 17:54:31 goodfj Exp $ */
/****************************************************************
**
**	Q_LNPROB.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_Q_LNPROB_H )
#define IN_Q_LNPROB_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

EXPORT_C_QBASIC double Q_LogNormalPDF (
	double 	s, 				   // ending spot
	double	s0, 			   // beginning spot
	double	sg, 			   // standard deviation of returns
	double	mu				   // total drift
	);

EXPORT_C_QBASIC double  Q_NormalDensity( double x );
EXPORT_C_QBASIC double Q_LogNormalDensity(double s, double s0, double sg, double mu);


#endif

