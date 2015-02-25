/****************************************************************
**
**	X_GSDTFUNCCALL.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/x_gsdtfunccall.h,v 1.4 2001/11/27 22:44:02 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_X_GSDTFUNCCALL_H )
#define IN_GSDT_X_GSDTFUNCCALL_H

#include <gsdt/base.h>
#include <datatype.h>
#include <secexpr.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_X_GSDT 
		GsStatus GsDtFuncCallConvertArg( DT_VALUE *Func, size_t Argc, DT_VALUE *Argv, DT_VALUE *Result );  

CC_END_NAMESPACE

#endif 
