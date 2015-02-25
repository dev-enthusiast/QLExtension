/****************************************************************
**
**	GSFUNCVECTORDOUBLE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncVectorDouble.h,v 1.2 2001/10/10 23:50:39 singhki Exp $
**
****************************************************************/

#ifndef IN_GSFUNC_GSFUNCVECTORDOUBLE_H
#define IN_GSFUNC_GSFUNCVECTORDOUBLE_H

#include <gscore/GsFuncHandle.h>
#include <gscore/GsVector.h>

CC_BEGIN_NAMESPACE( Gs )

typedef GsFunc< GsVector, double > GsFuncVectorDouble;
typedef GsFuncHandle< GsVector, double > GsFuncHandleVectorDouble;

CC_END_NAMESPACE

#endif 
