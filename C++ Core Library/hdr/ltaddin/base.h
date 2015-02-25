/****************************************************************
**
**	ltaddin/base.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltaddin/src/ltaddin/base.h,v 1.1 1999/07/15 12:32:51 thompcl Exp $
**
****************************************************************/

#if !defined( IN_LTADDIN_BASE_H )
#define IN_LTADDIN_BASE_H


/*
**	LTADDIN - 
*/

#ifdef BUILDING_LTADDIN
#define EXPORT_C_LTADDIN			EXPORT_C_EXPORT
#define EXPORT_CPP_LTADDIN			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_LTADDIN		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_LTADDIN	EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_LTADDIN			EXPORT_C_IMPORT
#define EXPORT_CPP_LTADDIN			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_LTADDIN		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_LTADDIN	EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_LTADDIN_TEMPLATES
#undef EXPORT_TEMPLATE_LTADDIN
#define EXPORT_TEMPLATE_LTADDIN	EXPORT_TEMPLATE_BUILD
#endif



#endif

