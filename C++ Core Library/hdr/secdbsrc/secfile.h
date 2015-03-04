/****************************************************************
**
**	SECFILE.H	- SecDb File structures and functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secfile.h,v 1.13 2004/11/29 23:48:13 khodod Exp $
**
****************************************************************/

#if !defined( IN_SECFILE_H )
#define IN_SECFILE_H

#include <stddef.h>
#include <sys/types.h>
#include <sdb_base.h>
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif
#ifndef IN_SECDB_H
#include <secdb.h>
#endif

typedef long						SDB_TIME;
typedef int32_t						SDB_FILE_HANDLE;
typedef long						SDB_FILE_SIZE;

struct SDB_FILE_INFO
{
	SDB_TIME
			LastAccess,		// st_atime
			LastUpdate;		// st_mtime

	SDB_FILE_SIZE
			Length;			// st_size

};


/*
**	File access functions
*/

EXPORT_C_SECDB int
		SecDbFileInfoToStructure(	SDB_FILE_INFO *Info, DT_VALUE *Struct ),
		SecDbFileInfoFromStructure(	SDB_FILE_INFO *Info, DT_VALUE *Struct );

EXPORT_C_SECDB SDB_FILE_HANDLE
		SecDbFileOpen(		SDB_DB *Db, char const* FileName );

EXPORT_C_SECDB int
		SecDbFileClose(		SDB_DB *Db, SDB_FILE_HANDLE File ),
		SecDbFileInfoGet(	SDB_DB *Db, SDB_FILE_HANDLE File, SDB_FILE_INFO *Info );

EXPORT_C_SECDB SDB_FILE_SIZE
		SecDbFileRead(		SDB_DB *Db, SDB_FILE_HANDLE File, void *Data, SDB_FILE_SIZE Length, SDB_FILE_SIZE Position );

// Writes operate inside a transaction
EXPORT_C_SECDB int
		SecDbFileDelete(	SDB_DB *Db, SDB_FILE_HANDLE File ),
		SecDbFileInfoSet(	SDB_DB *Db, SDB_FILE_HANDLE File, SDB_FILE_INFO *Info ),
		SecDbFileWrite(		SDB_DB *Db, SDB_FILE_HANDLE File, const void *Data, SDB_FILE_SIZE Length, SDB_FILE_SIZE Position );

#endif

