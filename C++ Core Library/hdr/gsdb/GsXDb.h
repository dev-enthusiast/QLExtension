/****************************************************************
**
**	gsdb/GsXDb.h	- GsDb exceptions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdb/src/gsdb/GsXDb.h,v 1.1 1999/04/19 20:46:24 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSDB_GSXDB_H )
#define IN_GSCDB_GSXDB_H

#include	<gsdb/base.h>
#include	<gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Exceptions thrown by the GsDb library
*/

GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSDB, GsXDbError,		GsException );

CC_END_NAMESPACE
#endif
