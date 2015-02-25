/****************************************************************
**
**	fqtsdb/base.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fqtsdb/src/fqtsdb/base.h,v 1.1 1999/03/24 21:50:48 gribbg Exp $
**
****************************************************************/

#if !defined( IN_FQTSDB_BASE_H )
#define IN_FQTSDB_BASE_H


/*
**	FQTSDB - 
*/

#ifdef BUILDING_FQTSDB
#define EXPORT_C_FQTSDB			EXPORT_C_EXPORT
#define EXPORT_CPP_FQTSDB		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_FQTSDB		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_FQTSDB	EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_FQTSDB			EXPORT_C_IMPORT
#define EXPORT_CPP_FQTSDB		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_FQTSDB		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_FQTSDB	EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_FQTSDB_TEMPLATES
#undef EXPORT_TEMPLATE_FQTSDB
#define EXPORT_TEMPLATE_FQTSDB	EXPORT_TEMPLATE_BUILD
#endif

#endif

