/****************************************************************
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secview/src/secview/base.h,v 1.1 2001/07/05 13:37:16 shahia Exp $
**
****************************************************************/

#ifndef IN_SECVIEW_BASE_H
#define IN_SECVIEW_BASE_H

#if defined( BUILDING_SECVIEW )

#define EXPORT_C_SECVIEW		EXPORT_C_EXPORT
#define EXPORT_CPP_SECVIEW	EXPORT_CPP_EXPORT
#define EXPORT_CLASS_SECVIEW	EXPORT_CLASS_EXPORT

#else

#define EXPORT_C_SECVIEW		EXPORT_C_IMPORT
#define EXPORT_CPP_SECVIEW	EXPORT_CPP_IMPORT
#define EXPORT_CLASS_SECVIEW	EXPORT_CLASS_IMPORT

#endif

#endif 
