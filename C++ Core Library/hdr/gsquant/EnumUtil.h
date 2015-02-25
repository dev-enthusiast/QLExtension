/****************************************************************
**
**	gsquant/EnumUtil.h	- Random enum addin utilities
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/EnumUtil.h,v 1.3 2000/05/18 19:40:35 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_ENUMUTIL_H )
#define IN_GSQUANT_ENUMUTIL_H

#include	<gsquant/base.h>
#include	<gscore/GsString.h>
#include	<gscore/GsEnum.h>
#include    <gsdt/GsDtDictionary.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT GsString
		GsEnumHelp( GsString EnumName, int Flags );

EXPORT_CPP_GSQUANT GsStringVector
		GsEnumListAll();

EXPORT_CPP_GSQUANT GsDtDictionary* 
		GsEnumStruct( const GsString&	EnumName );


CC_END_NAMESPACE
#endif

