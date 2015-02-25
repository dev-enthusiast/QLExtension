/****************************************************************
**
**	ka_base.h - should be kool_ade/base.h
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/ka_base.h,v 1.5 2001/11/27 22:49:05 procmon Exp $
**
****************************************************************/

#if !defined( IN_KA_BASE_H )
#define IN_KA_BASE_H


/*
**	KOOL_ADE - 
**
**	Note: These EXPORT macros are crippled to only export C functions
**	until we get rid of the .def files on NT.
*/

#ifdef BUILDING_KOOL_ADE
#define EXPORT_C_KOOL_ADE			EXPORT_C_EXPORT
#define EXPORT_CPP_KOOL_ADE			EXPORT_C_EXPORT
#define EXPORT_CLASS_KOOL_ADE		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_KOOL_ADE	EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_KOOL_ADE			EXPORT_C_IMPORT
#define EXPORT_CPP_KOOL_ADE			EXPORT_C_IMPORT
#define EXPORT_CLASS_KOOL_ADE		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_KOOL_ADE	EXPORT_TEMPLATE_IMPORT
#endif

#ifdef BUILDING_KOOL_ADE_TEMPLATES
#undef EXPORT_TEMPLATE_KOOL_ADE
#define EXPORT_TEMPLATE_KOOL_ADE	EXPORT_TEMPLATE_BUILD
#endif


#endif

