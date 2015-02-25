/****************************************************************
**
**	ADRDATE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/adrdate.h,v 1.4 2001/03/13 13:03:13 goodfj Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADRDATE_H )
#define IN_FCADLIB_ADRDATE_H

#include <fcadlib/base.h>
#include <fcadlib/autoarg.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRDateVector.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_FCADLIB GsStatus 
		AdGsRDateFromGsDt( const char* Name, const GsDt* Obj, GsRDate& Value ),
		AdGsDateFromGsDt( const char* Name, const GsDt* Obj, GsDate& Value ),
		AdGsDateVectorFromGsDt( const char* Name, const GsDt* Obj, AutoArgRef<GsDateVector>& Value ),
		AdGsRDateVectorFromGsDtArray( const char* Name, const GsDt* Obj, AutoArgRef<GsRDateVector>& Value ),
		AdGsRDateFromString( const char* Name, const char* String, GsRDate& Value ),
		AdGsDateFromString( const char* Name, const char* String, GsDate& Value ),
		AdGsDateVectorFromString( const char* Name, const char* String, AutoArgRef<GsDateVector>& Value );

CC_END_NAMESPACE

#endif 



