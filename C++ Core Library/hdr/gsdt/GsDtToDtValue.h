/****************************************************************
**
**	GSDTTODTVALUE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtToDtValue.h,v 1.5 2001/11/27 22:44:01 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTTODTVALUE_H )
#define IN_GSDT_GSDTTODTVALUE_H

#include <gsdt/base.h>
#include <gsdt/GsDt.h>
#include <gsdt/Enums.h>
#include <datatype.h>

CC_BEGIN_NAMESPACE( Gs )

// This is the type of a function that converts a GsDt to a DT_VALUE.
typedef GsStatus ( * GsDtToDtValueTypeConverter )( const GsDt &, DT_VALUE *, GSDT_TO_DTVALUE_FLAG );

EXPORT_CPP_X_GSDT void GsDtToDtValueConverterRegister( 
	const GsType &Type, 
	const GsDtToDtValueTypeConverter Converter,
	const DT_DATA_TYPE StandardReturnType = NULL
);

EXPORT_CPP_X_GSDT GsStatus GsDtToDtValue( 
	const GsDt &GsDtVal, 
	DT_VALUE *DtVal, 
	GSDT_TO_DTVALUE_FLAG ConversionFlag = GSDT_TO_DTV_FAIL_WITH_EXCEPTION 
);

EXPORT_CPP_X_GSDT GsStatus GsDtToDtValueCast(
	const GsDt				&GsDtVal,
	DT_VALUE				*DtVal,
	DT_DATA_TYPE	 		DesiredReturnType
);

EXPORT_CPP_X_GSDT DT_DATA_TYPE GsDtEquivDtValue(
	const GsDt				&GsDtVal
);

CC_END_NAMESPACE

#endif 
