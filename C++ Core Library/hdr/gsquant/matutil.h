/****************************************************************
**
**	gsquant/matutil.h	- GsMatrix utility functions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/matutil.h,v 1.2 2001/02/16 15:32:55 benhae Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_MATUTIL_H )
#define IN_GSQUANT_MATUTIL_H

#include	<gsquant/base.h>
#include	<gscore/GsMatrix.h>
#include	<gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsMatrix
		GsMatrixTranspose( const GsMatrix& Mat );

EXPORT_CPP_GSQUANT GsMatrix
		GsMatrixInverse( const GsMatrix& Mat );

EXPORT_CPP_GSQUANT int
		GsMatrixLuDecompose( 	GsMatrix& Mat, GsVector& Permutation );

EXPORT_CPP_GSQUANT void 
		GsMatrixLuSolver( GsMatrix& Mat, GsVector& Cst, GsVector& Permutation );

EXPORT_CPP_GSQUANT bool
		GsMatrixCholeskyDecompose( GsMatrix& Mat);

CC_END_NAMESPACE
#endif

