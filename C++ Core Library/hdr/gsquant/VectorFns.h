/****************************************************************
**
**	VectorFns.h	- vector functions for use with IML++ fns
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_VECTORFNS_H )
#define IN_VECTORFNS_H

#include <math.h>
#include <gscore/err.h>
#include <gscore/GsException.h>
#include <gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

double dot(
	const GsVector& vec1,
	const GsVector& vec2
);

double norm(
	const GsVector& vec
);

CC_END_NAMESPACE
#endif

