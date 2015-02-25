/****************************************************************
**
**	GSHISTOGRAM.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsHistogram.h,v 1.1 1999/05/17 15:51:41 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSHISTOGRAM_H )
#define IN_GSHISTOGRAM_H

#include <gsquant/base.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsMatrix * GsHistogram( const GsVector &Values, double BucketSize );

CC_END_NAMESPACE

#endif 
