/****************************************************************
**
**	fiquant/GsRandGaussCorrelated.h	- class GsRandGaussCorrelated
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsRandGaussCorrelated.h,v 1.10 2000/12/14 15:45:54 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSRANDGAUSSCORRELATED_H )
#define IN_GSRAND_GSRANDGAUSSCORRELATED_H
#include <gsrand/base.h>	
#include <gscore/GsMatrix.h>
#include <gscore/GsTensor.h>
#include <gsrand/GsRandGauss.h>

CC_BEGIN_NAMESPACE( Gs )

/*
** class GsRandGaussCorrelated - produce Gaussian random numbers
*/

class EXPORT_CLASS_GSRAND GsRandGaussCorrelated
{
public:
	GsRandGaussCorrelated();
	GsRandGaussCorrelated( const GsMatrix& Mat, INT32 Seed = 0 );
	GsRandGaussCorrelated( const GsMatrix& Mat, const GsRandGauss& Rand );
	GsRandGaussCorrelated( const GsRandGaussCorrelated& Rand );
	
	GsRandGaussCorrelated& operator=( const GsRandGaussCorrelated& Rand );
	
	virtual ~GsRandGaussCorrelated();

	virtual GsVector drawCorrelated();
	virtual GsMatrix drawCorrelated( size_t Number );
	virtual GsTensor drawCorrelated( const GsTensor::index &NumberOfSets );

	void ModifyCorrelationMatrix( const GsMatrix &Mat );

	GsRandGauss getUnderlying() const { return m_Rand; }

	virtual GsRandGaussCorrelated *copy() const;

	const GsMatrix &CholeskyDecomposedCorrelationMatrix() const { return m_CholeskyDecomposed; }

protected:

	void CholeskyDecomposedCorrelationMatrixSet( const GsMatrix &CholeskyDecomposed )  
	{ 
		m_CholeskyDecomposed = CholeskyDecomposed; 
	}

private:

	GsMatrix
			m_CholeskyDecomposed;

	GsRandGauss
			m_Rand;
};

CC_END_NAMESPACE

#endif 





