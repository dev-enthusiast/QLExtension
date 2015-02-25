/****************************************************************
**
**	GSABSGAUSS.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsAbsGauss.h,v 1.2 2001/02/01 10:38:21 goodfj Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSABSGAUSS_H )
#define IN_GSRAND_GSABSGAUSS_H

#include <ccstl.h>
#include <vector>
#include <gsrand/base.h>
#include <gsrand/GsRandUniform.h>
#include <gscore/types.h>
#include <gscore/GsIntegerVector.h>
#include <gscore/GsTensor.h>
#include <gscore/GsMatrix.h>
#include <gscore/GsDblBuffer.h>


CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsABSGauss 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSRAND GsAbsGauss
{
public:

	GsAbsGauss( const GsMatrix& Mat, size_t PathLen, INT32 Seed );
	GsAbsGauss( const GsAbsGauss &Other );

	~GsAbsGauss() {};

	GsAbsGauss &operator=( const GsAbsGauss &rhs );

	GsTensor DrawAll( const int &NumberOfPath );

	GsTensor DrawAllCorrelated(const int &NumberOfPath );

	bool	ValidNumPath(const int &Path ) const;
	
private:

	INT32 m_Seed;

	GsMatrix m_Mat;

	GsRandUniform	m_UniformRand;

	int GetIndex( int RelativeIndex);

	void InitialiseBox();

	void GetRandPerm( GsIntegerVector &Res);
	
	void ConstructGrid();

	typedef GsTensor::StridedRef ElemRef;

	typedef ElemRef::iterator ResRef;

	void BucketRand( ResRef	&Res,
					 GsIntegerVector::iterator	&ResIndexBegin,
					 GsIntegerVector::iterator	&ResIndexEnd,
					 GsIntegerVector &NewResIndex,
					 int &LocalIndex );

	

	void OneStepDraw(ResRef &Res);

	void Initialise();

	int m_NumPath;
	int	m_NumTime;
	int m_NumFactor;

	int m_GridSize;

	int m_BucketSize;

	int m_AntitheticRef;
	
	GsIntegerVector m_TempBucketIndex;

	GsIntegerVector m_LocalIndex;

	GsVector m_UniformDraw;

	GsVector m_Grid;
	
	GsIntegerVector m_ResIndex1;
	GsIntegerVector m_ResIndex2;
	GsIntegerVector *m_OldResIndexPt;
	GsIntegerVector *m_NewResIndexPt;
	
	struct DrawBox
	{
		int Content;
		int	BackInc;
		int FwdInc;
	};

	typedef CC_STL_VECTOR( DrawBox ) DrawBoxVector;

	DrawBoxVector m_RemainBox; 

	void DrawUnCorr( GsTensor &Result );

	void CorrelatedVector( GsTensor::index &UnCorrIndex, GsTensor &UnCorrResult, GsMatrix &CholMat, GsVector &TempBuffer );
};	

EXPORT_CPP_GSRAND GsTensor
		*GsAbsGaussDrawTensor( int Seed, const GsMatrix& Mat, const GsUnsignedVector &NumTrials, int Correlate = 0 );

EXPORT_CPP_GSRAND int	
		GsAbsGaussValidNumPath(int Path );

CC_END_NAMESPACE

#endif
