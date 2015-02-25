/****************************************************************
**
**	SECERROR.H	- Security Database Error Handling
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secerror.h,v 1.14 2001/08/22 20:22:42 singhki Exp $
**
****************************************************************/

#if !defined( IN_SECERROR_H )
#define IN_SECERROR_H

//#include	<err.h>

#define	SDB_ERR_CHILD_LIST_NOT_FOUND		1200
#define SDB_ERR_GET_VALUE_ABORTED			1201

#define	SDB_ERR_OBJECT_INVALID				1300
#define	SDB_ERR_OBJECT_NOT_FOUND			1301
#define	SDB_ERR_OBJECT_ALREADY_EXISTS		1302
#define	SDB_ERR_OBJECT_MODIFIED				1303
#define	SDB_ERR_OBJECT_IN_USE				1304

// Fatal node errors. GetValue will not allow these to be skipped with PassErrors
#define SDB_NODE_FIRST_FATAL_ERROR			1305
#define SDB_ERR_VALUE_CHANGED				1305
#define SDB_ERR_INVALIDATING_LOCKED_VALUE	1306
#define SDB_ERR_CYCLE_DETECTED				1307
#define SDB_NODE_LAST_FATAL_ERROR			1307

#define SDB_ERR_NODE_DB_CHANGED				1308
#define SDB_ERR_COMPILE_FAILED				1309

#define	SDB_ERR_CLASS_FAILURE				1400
#define	SDB_ERR_CLASS_NOT_FOUND				1401

#define	SDB_ERR_TRANSACTION_FAILURE			1600

#define	SDB_ERR_LOCK_TIMEOUT				1700
#define	SDB_ERR_LOCK_FAILURE				1701

#define	SDB_ERR_INDEX_INVALID				1800
#define	SDB_ERR_INDEX_FAILURE				1801
#define	SDB_ERR_INDEX_NOT_FOUND				1802
#define	SDB_ERR_INDEX_ALREADY_EXISTS		1803

#define	SDB_ERR_NOT_NEEDED					1900

#define	SDB_ERR_SLANG_EVAL					2000
#define	SDB_ERR_SLANG_ARG					2001

#endif
