/****************************************************************
**
**	SimpleVec.h	- simple vector inherited from GsVector
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_SIMPLEVEC_H )
#define IN_SIMPLEVEC_H

#include <gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

class SimpleVec : public GsVector
{
public:
	SimpleVec() : GsVector() {}
	SimpleVec( size_t n ) : GsVector( n ) {}
	SimpleVec( size_t n, double a ) : GsVector( n, a ) {}
	SimpleVec( const SimpleVec& rhs ) : GsVector( rhs ) {}
	SimpleVec( const GsVector& rhs ) : GsVector( rhs ) {}

	const SimpleVec& operator=( const GsVector& rhs );
	const SimpleVec& operator=( const SimpleVec& rhs );
};

CC_END_NAMESPACE

#endif

