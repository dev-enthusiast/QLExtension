/****************************************************************
**
**	BASE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/datatype/src/datatype/base.h,v 1.1 2001/06/12 23:01:59 vengrd Exp $
**
****************************************************************/

#ifndef IN_DATATYPE_BASE_H
#define IN_DATATYPE_BASE_H

#ifdef BUILDING_DATATYPE
#define EXPORT_C_DATATYPE		EXPORT_C_EXPORT
#define EXPORT_CPP_DATATYPE		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_DATATYPE	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_DATATYPE		EXPORT_C_IMPORT
#define EXPORT_CPP_DATATYPE		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_DATATYPE	EXPORT_CLASS_IMPORT
#endif

#endif 
