/* $Header: /home/cvs/src/secdb/src/secdb/base.h,v 1.5 2001/11/27 23:25:03 procmon Exp $ */
/****************************************************************
**
**	secdb/base.h	- Basic secdb datatype stuff
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/base.h,v 1.5 2001/11/27 23:25:03 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECDB_BASE_H )
#define IN_SECDB_BASE_H


#ifdef BUILDING_SECDB
#define EXPORT_C_SECDB		EXPORT_C_EXPORT
#define EXPORT_CPP_SECDB	EXPORT_CPP_EXPORT
#define EXPORT_CLASS_SECDB	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_SECDB		EXPORT_C_IMPORT
#define EXPORT_CPP_SECDB	EXPORT_CPP_IMPORT
#define EXPORT_CLASS_SECDB	EXPORT_CLASS_IMPORT
#endif

#ifdef BUILDING_SECGSDT
#define EXPORT_C_SECGSDT		EXPORT_C_EXPORT
#define EXPORT_CPP_SECGSDT	EXPORT_CPP_EXPORT
#define EXPORT_CLASS_SECGSDT	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_SECGSDT		EXPORT_C_IMPORT
#define EXPORT_CPP_SECGSDT	EXPORT_CPP_IMPORT
#define EXPORT_CLASS_SECGSDT	EXPORT_CLASS_IMPORT
#endif

#endif

