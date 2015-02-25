/* $Header: /home/cvs/src/quant/src/q_crmath.h,v 1.7 2000/03/06 14:53:21 goodfj Exp $ */
/****************************************************************
**
**	Q_CRMATH.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_Q_CRMATH_H )
#define IN_Q_CRMATH_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

EXPORT_C_QBASIC int	spline(double *x, double *y, int n, double yp1, double ypn, double *y2);
EXPORT_C_QBASIC void splint(double *xa, double *ya, double *y2a, int n, double x, double *y);

#endif

