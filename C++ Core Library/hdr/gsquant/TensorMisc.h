/****************************************************************
**
**	TENSORMISC.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/TensorMisc.h,v 1.7 2000/09/01 09:55:26 deeka Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_TENSORMISC_H )
#define IN_GSQUANT_TENSORMISC_H

#include <gsquant/base.h>
#include <gscore/GsTensor.h>
#include <gsdt/GsDtTensor.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsTensor *GsTensorNew( const GsUnsignedVector &Dimension );
EXPORT_CPP_GSQUANT GsUnsignedVector *GsTensorDim( const GsTensor &Tensor );

EXPORT_CPP_GSQUANT GsTensor *GsTensorReduceSum( 			const GsTensor &Tensor, unsigned ReductionDimension );
EXPORT_CPP_GSQUANT GsTensor *GsTensorReduceProduct( 		const GsTensor &Tensor, unsigned ReductionDimension );
EXPORT_CPP_GSQUANT GsTensor *GsTensorReduceMin( 			const GsTensor &Tensor, unsigned ReductionDimension );
EXPORT_CPP_GSQUANT GsTensor *GsTensorReduceMax( 			const GsTensor &Tensor, unsigned ReductionDimension );
EXPORT_CPP_GSQUANT GsTensor *GsTensorMaxDouble(				const GsTensor &Tensor, double val );
EXPORT_CPP_GSQUANT GsTensor *GsTensorMax( 					const GsTensor &lhs, const GsTensor &rhs );
EXPORT_CPP_GSQUANT GsTensor *GsTensorMinDouble(				const GsTensor &Tensor, double val );
EXPORT_CPP_GSQUANT GsTensor *GsTensorMin( 					const GsTensor &lhs, const GsTensor &rhs );
EXPORT_CPP_GSQUANT GsTensor *GsTensorAbs( 					const GsTensor &lhs );


EXPORT_CPP_GSQUANT GsTensor *GsTensorReduceSumOfSquares( 	const GsTensor &Tensor, unsigned ReductionDimension );

EXPORT_CPP_GSQUANT GsTensor *GsTensorMean( 					const GsTensor &Tensor, unsigned ReductionDimension );
EXPORT_CPP_GSQUANT GsTensor *GsTensorVariance( 				const GsTensor &Tensor, unsigned ReductionDimension );

EXPORT_CPP_GSQUANT GsMatrix *GsMatrixFromTensor( const GsTensor &Tensor );
EXPORT_CPP_GSQUANT GsVector *GsVectorFromTensor( const GsTensor &Tensor );

EXPORT_CPP_GSQUANT GsVector *GsTensorExtractVector(			const GsTensor &Tensor, 
															const GsUnsignedVector &StartPosition,
															const GsUnsignedVector &EndPosition );

EXPORT_CPP_GSQUANT GsTensor *GsTensorFromVector(const GsVector &Vector );

CC_END_NAMESPACE

#endif 
