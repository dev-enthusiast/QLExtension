/****************************************************************
**
**	ltaddin/base.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltsecdb/src/ltsecdb/base.h,v 1.1 2000/07/13 19:13:27 shahia Exp $
**
****************************************************************/

#if !defined( IN_LTSECDB_BASE_H )
#define IN_LTSECDB_BASE_H


/*
**	LTADDIN - 
*/

#ifdef BUILDING_LTSECDB
#define EXPORT_C_LTSECDB			EXPORT_C_EXPORT
#define EXPORT_CPP_LTSECDB			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_LTSECDB		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_LTSECDB	EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_LTSECDB			EXPORT_C_IMPORT
#define EXPORT_CPP_LTSECDB			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_LTSECDB		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_LTSECDB	EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_LTSECDB_TEMPLATES
#undef EXPORT_TEMPLATE_LTSECDB
#define EXPORT_TEMPLATE_LTSECDB	EXPORT_TEMPLATE_BUILD
#endif



#endif

