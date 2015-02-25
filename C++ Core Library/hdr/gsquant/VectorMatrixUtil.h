/****************************************************************
**
**	VECTORMATRIXUTIL.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/VectorMatrixUtil.h,v 1.11 2001/11/27 22:46:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_VECTORMATRIXUTIL_H )
#define IN_GSQUANT_VECTORMATRIXUTIL_H

#include <gsquant/base.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>
#include <gscore/GsUnsignedVector.h>
#include <gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT 
		GsVector GsVectorNewVal( unsigned n, double value );

EXPORT_CPP_GSQUANT 
		double GsVectorDotProduct( const GsVector& vec1, const GsVector& vec2 );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorPlusVector( const GsVector& vec1, const GsVector& vec2 );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorMinusVector( const GsVector& vec1, const GsVector& vec2 );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorTimesVector( const GsVector& vec1, const GsVector& vec2 );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorDivVector( const GsVector& vec1, const GsVector& vec2 );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorLog( const GsVector& vec );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorExp( const GsVector& vec );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorSqrt( const GsVector& vec );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorPow( const GsVector& Bases, 
							  const GsVector &Exponents );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorTimesScalar( const GsVector& vec, double s );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorDivScalar(	const GsVector& vec, double s );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorPlusScalar( const GsVector& vec, double s );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorMinusScalar( const GsVector& vec, double s );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorEqual(	const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorNotEqual( const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorLess( const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorLessEqual(	const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorGreater( const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorGreaterEqual( const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorMin( const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorMax( const GsVector& a, const GsVector& b );
EXPORT_CPP_GSQUANT 
		GsVector GsVectorIf( const GsVector& Cond, 
							 const GsVector& a, 
							 const GsVector& b 	);
EXPORT_CPP_GSQUANT 
		GsUnsignedVector GsUnsignedVectorComp( const GsUnsignedVector& vec );
EXPORT_CPP_GSQUANT
		GsUnsignedVector GsUnsignedVectorNewVal( unsigned n, unsigned value );

EXPORT_CPP_GSQUANT double GsVectorSize( const GsVector &Vec );

EXPORT_CPP_GSQUANT GsUnsignedVector GsVectorTruth( const GsVector &Vector );

EXPORT_CPP_GSQUANT GsVector *GsMatrixSize( const GsMatrix &Mat );

EXPORT_CPP_GSQUANT GsVector *GsMatrixRow(  const GsMatrix &Mat, unsigned RowNum );
EXPORT_CPP_GSQUANT GsVector *GsMatrixCol(  const GsMatrix &Mat, unsigned ColNum );
EXPORT_CPP_GSQUANT GsVector *GsMatrixDiag( const GsMatrix &Mat,      int DagNum );

EXPORT_CPP_GSQUANT GsMatrix GsMatrixProduct( const GsMatrix &M1, const GsMatrix &M2 );
EXPORT_CPP_GSQUANT GsVector GsVectorTimesMatrix( const GsVector &V, const GsMatrix &M );
EXPORT_CPP_GSQUANT GsVector GsMatrixTimesVector( const GsMatrix &M, const GsVector &V );

EXPORT_CPP_GSQUANT GsMatrix GsMatrixDiagCreate( const GsVector &V );

EXPORT_CPP_GSQUANT unsigned PolyMergeVectorsNoDuplicates(
	GsVector::const_iterator *Current,
	GsVector::const_iterator *End,
	unsigned NumInputs,
	GsVector::iterator Result
);

CC_END_NAMESPACE

#endif 
