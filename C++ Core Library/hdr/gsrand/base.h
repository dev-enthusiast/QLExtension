/****************************************************************
**
**	gsrand/base.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/base.h,v 1.1 1999/05/06 05:16:31 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_BASE_H )
#define IN_GSRAND_BASE_H


/*
**	GSRAND - 
*/

#ifdef BUILDING_GSRAND
#define EXPORT_C_GSRAND				EXPORT_C_EXPORT
#define EXPORT_CPP_GSRAND			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GSRAND			EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_GSRAND		EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_GSRAND				EXPORT_C_IMPORT
#define EXPORT_CPP_GSRAND			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GSRAND			EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_GSRAND		EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_GSRAND_TEMPLATES
#undef EXPORT_TEMPLATE_GSRAND
#define EXPORT_TEMPLATE_GSRAND		EXPORT_TEMPLATE_BUILD
#endif


#endif

