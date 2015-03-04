/****************************************************************
**
**	SECCOPY.H	- Security to File API
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/seccopy.h,v 1.7 1999/11/01 22:41:53 singhki Exp $
**
****************************************************************/

#if !defined( IN_SECCOPY_H )
#define IN_SECCOPY_H

#include <stdio.h>
#include <secdb.h>

#if !defined( IN_SECOBJ_H )
#   include <secobj.h>
#endif

/*
**	Prototype functions
*/

EXPORT_C_SECDB int
		SecDbAddStoredObject(		SDB_STORED_OBJECT *StoredObj, SDB_DB *DbTarget ),
		SecDbCopyToFile(			const SDB_FULL_SEC_NAME *SecName, SDB_DB *DbSource, FILE *FileDataDest ),
		SecDbStoredObjectToFile(	FILE *File, SDB_STORED_OBJECT *StoredObj ),
		SecDbStoredObjectFromFile(	FILE *File, SDB_STORED_OBJECT *StoredObj );
#endif

