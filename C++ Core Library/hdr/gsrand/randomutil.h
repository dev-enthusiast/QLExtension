/****************************************************************
**
**	gsrand/randomutil.h	- addin functions for random numbers
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/randomutil.h,v 1.2 2000/09/03 16:27:14 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_RANDOMUTIL_H )
#define IN_GSRAND_RANDOMUTIL_H

#include	<gsrand/base.h>
#include	<gscore/GsVector.h>
#include	<gscore/GsMatrix.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSRAND GsVector
		*GsRandUniformDrawVector(int Seed, int Number),
		*GsRandUniformRangeDrawVector(int Seed, double Lo, double Hi, int Number),
		*GsRandGaussDrawVector(int Seed, int Number),
		*GsRandGaussCorrelatedDrawVector(int Seed, const GsMatrix& Mat);

EXPORT_CPP_GSRAND GsMatrix
		*GsRandGaussCorrelatedDrawMatrix(int Seed, const GsMatrix& Mat, int Number);
		
EXPORT_CPP_GSRAND GsTensor
		*GsRandGaussCorrelatedDrawTensor(int Seed, const GsMatrix& Mat, const GsUnsignedVector & );
		

CC_END_NAMESPACE

#endif 
