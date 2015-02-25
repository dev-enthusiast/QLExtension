/****************************************************************
**
**	GSTHREAD_BASE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/gsthread_base.h,v 1.1 2001/04/10 16:51:44 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_GSTHREAD_BASE_H
#define IN_GSTHREAD_GSTHREAD_BASE_H

#ifdef BUILDING_GSTHREAD

#define EXPORT_C_GSTHREAD     EXPORT_C_EXPORT
#define EXPORT_CPP_GSTHREAD   EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GSTHREAD EXPORT_CLASS_EXPORT

#else

#define EXPORT_C_GSTHREAD     EXPORT_C_IMPORT
#define EXPORT_CPP_GSTHREAD   EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GSTHREAD EXPORT_CLASS_IMPORT

#endif
#endif 
