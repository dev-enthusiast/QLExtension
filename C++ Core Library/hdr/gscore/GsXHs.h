/****************************************************************
**
**	gscore/GsXHs.h	- Holiday Schedule exceptions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsXHs.h,v 1.1 1999/03/22 16:53:53 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSXHS_H )
#define IN_GSCORE_GSXHS_H

#include	<gscore/base.h>
#include	<gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Exceptions thrown by the Holiday Schedule subsystem
*/

GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSDATE, GsXHsDbError,			GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSDATE, GsXHsNoSchedule,		GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSDATE, GsXHsInvalidExpression,	GsException );

CC_END_NAMESPACE
#endif
