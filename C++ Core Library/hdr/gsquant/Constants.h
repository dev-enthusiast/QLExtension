/****************************************************************
**
**	CONSTANTS.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/Constants.h,v 1.1 1999/11/10 20:34:26 tsoucp Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_CONSTANTS_H )
#define IN_GSQUANT_CONSTANTS_H

#include <gsquant/base.h>

#include <float.h>

CC_BEGIN_NAMESPACE( Gs )

inline double GetDoublePrecision() { return DBL_EPSILON; }

CC_END_NAMESPACE

#endif 
