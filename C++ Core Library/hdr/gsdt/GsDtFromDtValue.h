/****************************************************************
**
**	GSDTFROMDTVALUE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtFromDtValue.h,v 1.4 2001/11/27 22:43:58 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTFROMDTVALUE_H )
#define IN_GSDT_GSDTFROMDTVALUE_H

#include <gsdt/base.h>

CC_BEGIN_NAMESPACE( Gs )

// This is the type of a function that converts a GsDt from a DT_VALUE.
typedef GsDt * ( * GsDtFromDtValueTypeConverter )( DT_VALUE * );

EXPORT_CPP_X_GSDT void GsDtFromDtValueConverterRegister( const DT_DATA_TYPE &Type, const GsDtFromDtValueTypeConverter Converter );

EXPORT_CPP_X_GSDT GsDt *GsDtFromDtValue( DT_VALUE *Val );

CC_END_NAMESPACE

#endif 
