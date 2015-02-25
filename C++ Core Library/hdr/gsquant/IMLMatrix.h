/****************************************************************
**
**	IMLMatrix.h	- matrix class for use with IML++ fns
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_IMLMATRIX_H )
#define IN_IMLMATRIX_H

#include <math.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>

CC_BEGIN_NAMESPACE( Gs )

class IMLMatrix : public GsMatrix
{
	public:
		GsVector operator*( const GsVector& vec ) const { return product( *this, vec ); }
		GsVector solve( const GsVector& vec ) const { return product( *this, vec ); }
		GsVector trans_mult( const GsVector& vec ) const { return product( vec, *this ); }
		GsVector trans_solve( const GsVector& vec ) const { return product( vec, *this ); }
};

CC_END_NAMESPACE
#endif

