/****************************************************************
**
**	gsquant/toeplitzsolver.h	- Toeplitz linear system solvers
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/toeplitzsolver.h,v 1.2 1999/12/02 19:10:25 lowthg Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_TOEPLITZSOLVER_H )
#define IN_GSQUANT_TOEPLITZSOLVER_H

#include	<gsquant/base.h>
#include	<gscore/types.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsVector GsToeplitzPrepFast(
	const GsVector&		R		// first column and row of the matrix
);

EXPORT_CPP_GSQUANT GsMatrix GsToeplitzMatrixCreate(
	const GsVector&		R		// first column and row of the matrix
);

EXPORT_CPP_GSQUANT void GsToeplitzPrepFast(
	const double*	R,		// the Toeplitz matrix
	double*			x,		// returns the first column of inv(R)
	double*			y,		// returns the last column of inv(R)
	int				n		// the size of the matrix ( n>=1 !).
);

CC_END_NAMESPACE

#endif



