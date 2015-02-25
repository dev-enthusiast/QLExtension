/****************************************************************
**
**	ADRETARGBYTYPE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/AdRetArgByType.h,v 1.1 2001/02/12 16:30:32 vengrd Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADRETARGBYTYPE_H )
#define IN_FCADLIB_ADRETARGBYTYPE_H

#include <fcadlib/base.h>
#include <gsdt/GsDt.h>
#include <fcadlib/adexcel.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_FCADLIBX GsStatus AdRetArgByType(
	XLOPER &XlRet,
	const GsDt &GsDtRet
);


CC_END_NAMESPACE

#endif 
