/****************************************************************
**
**	fiinstr/base.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/base.h,v 1.5 2001/11/27 21:58:26 procmon Exp $
**
****************************************************************/

#if !defined( IN_FIINSTR_BASE_H )
#define IN_FIINSTR_BASE_H


/*
**	FIINSTR - 
*/

#ifdef BUILDING_FIINSTR
#define EXPORT_C_FIINSTR			EXPORT_C_EXPORT
#define EXPORT_CPP_FIINSTR			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_FIINSTR		EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_FIINSTR			EXPORT_C_IMPORT
#define EXPORT_CPP_FIINSTR			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_FIINSTR		EXPORT_CLASS_IMPORT
#endif


#endif

