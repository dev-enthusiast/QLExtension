/****************************************************************
**
**	gscore/GsDateEnums.h	- Enums for gsdate
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsDateEnums.h,v 1.3 2001/11/27 22:43:08 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_ENUMS_H )
#define IN_GSDATE_ENUMS_H

#include	<gscore/GsEnum.h>

CC_BEGIN_NAMESPACE( Gs )

enum LT_ADJUSTED_ACCRUAL
{
	LT_AA_DEFAULT,
	LT_AA_ADJUSTED,
	LT_AA_UNADJUSTED
};

GS_ENUM_DECLARE( LT_ADJUSTED_ACCRUAL );

enum LT_BUSINESS_DAY_CONVENTION
{
	LT_BDC_DEFAULT,
	LT_BDC_FOLLOWING,
	LT_BDC_MODIFIED_FOLLOWING,
	LT_BDC_PREVIOUS,
	LT_BDC_UNADJUSTED
};

GS_ENUM_DECLARE( LT_BUSINESS_DAY_CONVENTION );

enum LT_INCLUDE_END
{
	LT_IE_DEFAULT,		
	LT_IE_ALWAYS,			
	LT_IE_NEVER			
};
 
GS_ENUM_DECLARE( LT_INCLUDE_END );

enum LT_STUB_TYPE
{
	LT_ST_DEFAULT,
	LT_ST_SHORT,
	LT_ST_LONG,
	LT_ST_FULL
};

GS_ENUM_DECLARE( LT_STUB_TYPE );

CC_END_NAMESPACE
#endif











