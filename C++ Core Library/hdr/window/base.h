/****************************************************************
**
**	BASE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/window/src/window/base.h,v 1.4 2004/10/22 19:59:23 khodod Exp $
**
****************************************************************/

#ifndef IN_WINDOW_BASE_H
#define IN_WINDOW_BASE_H

#ifdef BUILDING_WINDOW
#define EXPORT_C_WINDOW			EXPORT_C_EXPORT
#define EXPORT_CPP_WINDOW		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_WINDOW		EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_WINDOW			EXPORT_C_IMPORT
#define EXPORT_CPP_WINDOW		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_WINDOW		EXPORT_CLASS_IMPORT
#endif

#ifdef BUILDING_WEVENT
#define EXPORT_C_WEVENT			EXPORT_C_EXPORT
#define EXPORT_CPP_WEVENT		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_WEVENT		EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_WEVENT			EXPORT_C_IMPORT
#define EXPORT_CPP_WEVENT		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_WEVENT		EXPORT_CLASS_IMPORT
#endif

#endif 
