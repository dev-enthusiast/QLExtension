/****************************************************************
**
**	rtsdb/base.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/rtsdb/src/rtsdb/base.h,v 1.3 1998/12/01 21:59:23 gribbg Exp $
**
****************************************************************/

#if !defined( IN_RTSDB_BASE_H )
#define IN_RTSDB_BASE_H

/*
**	RTSDB - 
*/

#ifdef BUILDING_RTSDB
#define EXPORT_C_RTSDB				EXPORT_C_EXPORT
#define EXPORT_CPP_RTSDB			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_RTSDB			EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_RTSDB		EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_RTSDB				EXPORT_C_IMPORT
#define EXPORT_CPP_RTSDB			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_RTSDB			EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_RTSDB		EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_RTSDB_TEMPLATES
#undef EXPORT_TEMPLATE_RTSDB
#define EXPORT_TEMPLATE_RTSDB		EXPORT_TEMPLATE_BUILD
#endif

#endif

