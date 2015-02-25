/****************************************************************
**
**	gsdb/base.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdb/src/gsdb/base.h,v 1.1 1999/04/19 20:46:24 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSDB_BASE_H )
#define IN_GSDB_BASE_H


/*
**	GSDB - SecDb database access from C++
*/

#ifdef BUILDING_GSDB
#define EXPORT_C_GSDB			EXPORT_C_EXPORT
#define EXPORT_CPP_GSDB			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GSDB		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_GSDB	EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_GSDB			EXPORT_C_IMPORT
#define EXPORT_CPP_GSDB			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GSDB		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_GSDB	EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_GSDB_TEMPLATES
#undef EXPORT_TEMPLATE_GSDB
#define EXPORT_TEMPLATE_GSDB	EXPORT_TEMPLATE_BUILD
#endif


#endif

