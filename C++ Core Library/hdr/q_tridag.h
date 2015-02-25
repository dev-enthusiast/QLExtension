/* $Header: /home/cvs/src/quant/src/q_tridag.h,v 1.3 2000/07/18 16:55:08 lowthg Exp $ */
/****************************************************************
**
**	Q_TRIDAG.H
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/


#if !defined( IN_Q_TRIDAG_H )
#define IN_Q_TRIDAG_H

#include <q_base.h>

#ifndef IN_QTYPES_H
#include	<qtypes.h>
#endif

EXPORT_C_QBASIC int Q_TriDiagonal( Q_VECTOR *a, Q_VECTOR *b, Q_VECTOR *c, Q_VECTOR *r, Q_VECTOR *u, 
	double *gam, int n );

EXPORT_C_QBASIC int Q_TriDiagConst(       
	double     a,	   // lower diagonal of matrix
	double     b,	   // central diagonal
	double     c,	   // upper diagonal
	Q_VECTOR   *r,	   // right side of matrix equation
	Q_VECTOR   *u,	   // what we're solving for
	double     *gam,   // work array - allocate in advance
	int        n	   // size of matrix
	);
#endif
