/****************************************************************
**
**	GSRANDGAUSSCORRELATEDANTITHETIC.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsRandGaussCorrelatedAntithetic.h,v 1.4 2000/12/14 15:45:54 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSRANDGAUSSCORRELATEDANTITHETIC_H )
#define IN_GSRAND_GSRANDGAUSSCORRELATEDANTITHETIC_H

#include <gsrand/base.h>
#include <gsrand/GsRandGaussCorrelated.h>


CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRandGaussCorrelatedAntithetic 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSRAND GsRandGaussCorrelatedAntithetic : public GsRandGaussCorrelated 
{
public:
	GsRandGaussCorrelatedAntithetic();
	GsRandGaussCorrelatedAntithetic( const GsMatrix& Mat, INT32 Seed = 0 );
	GsRandGaussCorrelatedAntithetic( const GsMatrix& Mat, const GsRandGauss& Rand );
	GsRandGaussCorrelatedAntithetic( const GsRandGaussCorrelatedAntithetic &Rand );

	virtual ~GsRandGaussCorrelatedAntithetic();

	GsRandGaussCorrelatedAntithetic &operator=( const GsRandGaussCorrelatedAntithetic &rhs );

	virtual GsVector drawCorrelated();

	virtual GsTensor drawCorrelated( const GsTensor::index &NumberOfSets );

	virtual GsRandGaussCorrelated *copy() const;

private:

	bool	m_NextValueCached;

	GsVector
			m_CachedNextValue;
};


EXPORT_CPP_GSRAND GsMatrix
		*GsRandGaussCorrelatedAntitheticDrawMatrix( int Seed, const GsMatrix& Mat, int Number );
		
EXPORT_CPP_GSRAND GsTensor
		*GsRandGaussCorrelatedAntitheticDrawTensor( int Seed, const GsMatrix& Mat, const GsUnsignedVector &NumTrials );
		

CC_END_NAMESPACE

#endif 
