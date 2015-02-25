/****************************************************************
**
**	TZ_BASE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/time/src/time_base.h,v 1.3 2002/01/02 17:08:27 khodod Exp $
**
****************************************************************/


#if !defined( IN_TIME_BASE_H )
#define IN_TIME_BASE_H

#ifdef BUILDING_TIMEZONE
#define EXPORT_C_TIMEZONE		EXPORT_C_EXPORT
#define EXPORT_CPP_TIMEZONE		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_TIMEZONE	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_TIMEZONE		EXPORT_C_IMPORT
#define EXPORT_CPP_TIMEZONE		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_TIMEZONE	EXPORT_CLASS_IMPORT
#endif

#endif 
