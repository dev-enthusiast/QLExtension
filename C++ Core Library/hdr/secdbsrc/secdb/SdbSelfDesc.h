/****************************************************************
**
**	SDBSELFDESC.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/SdbSelfDesc.h,v 1.2 2001/07/17 00:40:51 vengrd Exp $
**
****************************************************************/

#ifndef IN_SECDB_SDBSELFDESC_H
#define IN_SECDB_SDBSELFDESC_H

#include <secdb/base.h>
#include <datatype/SelfDescStream.h>
#include <secdb.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_SECDB int SdbSelfDescStreamWrite(
	SDB_OBJECT *SecPtr,
	BSTREAM	   *BStream,
	int			StreamVersion	
);


EXPORT_CPP_SECDB int SecDbSelfDescribe(
	SDB_DB	   				*pDb,
	SDB_OBJECT 				*SecPtr,
	GS_SELF_DESC_OPERATION	StreamOperation
);


EXPORT_CPP_SECDB bool SecDbSelfDescribingStreams();


CC_END_NAMESPACE

#endif 
