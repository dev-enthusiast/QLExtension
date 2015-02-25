/****************************************************************
**
**	GSFUNCUTIL.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncUtil.h,v 1.1 1999/12/03 20:43:41 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSFUNC_GSFUNCUTIL_H )
#define IN_GSFUNC_GSFUNCUTIL_H

#include <gscore/base.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSFUNC
void hunt( double xx[] ,unsigned long n, double x, unsigned long* jlo );

CC_END_NAMESPACE

#endif 
