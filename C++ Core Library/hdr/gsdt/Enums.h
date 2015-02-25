/****************************************************************
**
**	ENUMS.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/Enums.h,v 1.1 2001/01/26 17:12:41 johnsoc Exp $
**
****************************************************************/

#if !defined( IN_GSDT_ENUMS_H )
#define IN_GSDT_ENUMS_H

#include <gsdt/base.h>

CC_BEGIN_NAMESPACE( Gs )

enum GSDT_TO_DTVALUE_FLAG
{
	GSDT_TO_DTV_FAIL_WITH_EXCEPTION = 0,
	GSDT_TO_DTV_FAIL_WITH_DTGSDT
};

CC_END_NAMESPACE

#endif 
