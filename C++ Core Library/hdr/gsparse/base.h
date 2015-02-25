/****************************************************************
**
**	BASE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsparse/src/gsparse/base.h,v 1.2 1999/10/29 14:51:02 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSPARSE_BASE_H )
#define IN_GSPARSE_BASE_H


/*
**	GSPARSE - 
*/

#ifdef BUILDING_GSPARSE
#define EXPORT_C_GSPARSE			EXPORT_C_EXPORT
#define EXPORT_CPP_GSPARSE			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GSPARSE		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_GSPARSE	EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_GSPARSE			EXPORT_C_IMPORT
#define EXPORT_CPP_GSPARSE			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GSPARSE		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_GSPARSE	EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_GSPARSE_TEMPLATES
#undef EXPORT_TEMPLATE_GSPARSE
#define EXPORT_TEMPLATE_GSPARSE	EXPORT_TEMPLATE_BUILD
#endif



#endif

