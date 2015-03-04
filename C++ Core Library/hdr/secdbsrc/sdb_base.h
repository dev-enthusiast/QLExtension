/****************************************************************
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdb_base.h,v 1.7 2001/11/27 23:23:35 procmon Exp $
**
****************************************************************/

#if !defined( IN_SDB_BASE_H )
#define IN_SDB_BASE_H

#if defined( BUILDING_SECDB )

#define EXPORT_C_SECDB		EXPORT_C_EXPORT
#define EXPORT_CPP_SECDB	EXPORT_CPP_EXPORT
#define EXPORT_CLASS_SECDB	EXPORT_CLASS_EXPORT

#else

#define EXPORT_C_SECDB		EXPORT_C_IMPORT
#define EXPORT_CPP_SECDB	EXPORT_CPP_IMPORT
#define EXPORT_CLASS_SECDB	EXPORT_CLASS_IMPORT

#endif

#endif 
