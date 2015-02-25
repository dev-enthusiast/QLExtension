/****************************************************************
**
**	ADGETARGBYTYPE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/AdGetArgByType.h,v 1.1 2001/02/12 10:35:40 vengrd Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADGETARGBYTYPE_H )
#define IN_FCADLIB_ADGETARGBYTYPE_H

#include <fcadlib/base.h>
#include <gsdt/GsDt.h>
#include <fcadlib/adexcel.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_FCADLIBX GsDt *AdGetArgByType( 
	const char *ArgName,
	const GsType &ArgType,
	const XLOPER &XlArg
);
	

CC_END_NAMESPACE

#endif 
