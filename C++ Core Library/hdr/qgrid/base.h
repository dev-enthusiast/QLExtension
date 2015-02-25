/****************************************************************
**
**	qgrid/base.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/base.h,v 1.2 2001/06/12 20:22:42 johnsoc Exp $
**
****************************************************************/

#if !defined( IN_QGRID_BASE_H )
#define IN_QGRID_BASE_H


/*
**	QGRID - 
*/

#ifdef BUILDING_QGRID
#define EXPORT_C_QGRID			EXPORT_C_EXPORT
#define EXPORT_CPP_QGRID			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_QGRID		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_QGRID	EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_QGRID			EXPORT_C_IMPORT
#define EXPORT_CPP_QGRID			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_QGRID		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_QGRID	EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_QGRID_TEMPLATES
#undef EXPORT_TEMPLATE_QGRID
#define EXPORT_TEMPLATE_QGRID	EXPORT_TEMPLATE_BUILD
#endif

#ifdef BUILDING_QGRIDGSDT
#define EXPORT_C_QGRIDGSDT		EXPORT_C_EXPORT
#define EXPORT_CPP_QGRIDGSDT	EXPORT_CPP_EXPORT
#define EXPORT_CLASS_QGRIDGSDT	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_QGRIDGSDT		EXPORT_C_IMPORT
#define EXPORT_CPP_QGRIDGSDT	EXPORT_CPP_IMPORT
#define EXPORT_CLASS_QGRIDGSDT	EXPORT_CLASS_IMPORT
#endif


#endif

