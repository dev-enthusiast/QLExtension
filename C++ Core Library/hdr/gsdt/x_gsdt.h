/****************************************************************
**
**	x_gsdt.h	- Exports from the slang library for GsDt
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/x_gsdt.h,v 1.20 2004/12/01 17:19:35 khodod Exp $
**
****************************************************************/

#ifndef IN_X_GSDT_H
#define IN_X_GSDT_H

#include <gsdt/base.h>
#include <gsdt/GsDt.h>
#include <gsdt/Enums.h>
#include <datatype.h>

CC_BEGIN_NAMESPACE( Gs )

extern EXPORT_CPP_X_GSDT DT_DATA_TYPE
		DtGsDt,
		DtGsDtIter;


// FIX - Take these out and find all projects that include this file
// just to get them and have them include GsDt[To|From]DtValue.h instead.

#if !defined( IN_GSDT_GSDTTODTVALUE_H )
EXPORT_CPP_X_GSDT GsStatus GsDtToDtValue( const GsDt &GsDtVal, DT_VALUE *DtVal, GSDT_TO_DTVALUE_FLAG ConversionFlag = GSDT_TO_DTV_FAIL_WITH_EXCEPTION );
#endif

#if !defined( IN_GSDT_GSDTFROMDTVALUE_H )
EXPORT_CPP_X_GSDT GsDt *GsDtFromDtValue( DT_VALUE *Val );
#endif

CC_END_NAMESPACE

#endif

