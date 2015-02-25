/****************************************************************
**
**	LtObjHnd.H		- Functions relating to object handles
**					  (not particularly Liberty, but separated out
**					  from LtSecDbObj to avoid prodver complications)
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltsecdb/src/ltsecdb/LtObjHnd.h,v 1.3 2001/01/26 19:51:58 johnsoc Exp $
**
****************************************************************/

#if !defined( IN_LTSECDB_LTOBJHND_H )
#define IN_LTSECDB_LTOBJHND_H

#include <ltsecdb/base.h>
#include <secdb.h>
#include <secdb/LtDtArgs.h>
#include <gscore/GsDate.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_LTSECDB bool SetObjHandleAccessFlags( bool newStatus );

EXPORT_CPP_LTSECDB GsStringVector GetAccessedObjHandles( void );

EXPORT_CPP_LTSECDB GsDt* GetExcelObject(const GsDt& Obj);

EXPORT_CPP_LTSECDB GsString SetExcelObject(
	const GsString&		HandleName,
	const GsDt&			Obj);

CC_END_NAMESPACE

#endif 

