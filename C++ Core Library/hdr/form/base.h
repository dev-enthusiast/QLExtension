/****************************************************************
**
**	BASE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/form/src/form/base.h,v 1.3 2004/10/22 19:59:46 khodod Exp $
**
****************************************************************/

#ifndef IN_FORM_BASE_H
#define IN_FORM_BASE_H

#ifdef BUILDING_FORM
#define EXPORT_C_FORM		EXPORT_C_EXPORT
#define EXPORT_CPP_FORM		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_FORM	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_FORM		EXPORT_C_IMPORT
#define EXPORT_CPP_FORM		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_FORM	EXPORT_CLASS_IMPORT
#endif

#endif 
