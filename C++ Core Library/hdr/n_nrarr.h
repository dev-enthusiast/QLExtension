/* $Header: /home/cvs/src/num/src/n_nrarr.h,v 1.10 2001/10/10 21:51:41 singhki Exp $ */
/****************************************************************
**
**	n_nrarr.h	- Numerical Recipes array data structure
**                Used only to interface to NR routines. 
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.10 $
**
****************************************************************/

#if !defined( IN_N_NRARR_H )
#define IN_N_NRARR_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

EXPORT_C_NUM double
		*N_NrVectorNew( long nl, long nh );

EXPORT_C_NUM void
		N_NrVectorFree( double *v,  long nl,  long nh );

EXPORT_C_NUM int
		*N_NrIntVectorNew( long nl,  long nh );

EXPORT_C_NUM void
		N_NrIntVectorFree( int *v,  long nl,    long nh );

EXPORT_C_NUM double
		**N_NrMatrixNew( long nrl, long nrh,   long ncl,   long nch );

EXPORT_C_NUM void
		N_NrMatrixFree( double **m,	long nrl,	long nrh,   long ncl,   long nch );

#endif
