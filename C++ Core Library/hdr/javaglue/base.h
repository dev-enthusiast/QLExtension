/****************************************************************
**
**	javaglue/base.h
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/javaglue/src/javaglue/base.h,v 1.2 2000/09/28 23:02:23 singhki Exp $
** 
****************************************************************/

#if !defined( IN_JAVAGLUE_BASE_H )
#define IN_JAVAGLUE_BASE_H

#ifdef BUILDING_JAVAGLUE
#define EXPORT_C_JAVAGLUE		EXPORT_C_EXPORT
#define EXPORT_CPP_JAVAGLUE	EXPORT_CPP_EXPORT
#define EXPORT_CLASS_JAVAGLUE	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_JAVAGLUE		EXPORT_C_IMPORT
#define EXPORT_CPP_JAVAGLUE	EXPORT_CPP_IMPORT
#define EXPORT_CLASS_JAVAGLUE	EXPORT_CLASS_IMPORT
#endif

#endif

