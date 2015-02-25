/* $Header: /home/cvs/src/quant/src/q_bimax.h,v 1.12 2000/03/02 13:32:52 goodfj Exp $ */
/****************************************************************
**
**	Q_BIMAX.H  	routines for joint distribution of the max or min
**				of two correlated normal random walks.
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_Q_BIMAX_H )
#define IN_Q_BIMAX_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
		
EXPORT_C_QRARE double Q_ModBessel (
	double         nu,    // order of bessel function
	double         x      // argument		
	);
#endif

