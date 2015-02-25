/****************************************************************
**
**	pyglue/base.h
**
**	Copyright 2011, Constellation Energy Group, Inc.
**
**	$Log: base.h,v $
**	Revision 1.1  2011/06/25 19:02:41  e63100
**	initial commit
**
** 
****************************************************************/

#if !defined( IN_PYGLUE_BASE_H )
#define IN_PYGLUE_BASE_H

#ifdef BUILDING_PYGLUE

#define EXPORT_C_PYGLUE			EXPORT_C_EXPORT
#define EXPORT_CPP_PYGLUE		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_PYGLUE		EXPORT_CLASS_EXPORT

#else

#define EXPORT_C_PYGLUE			EXPORT_C_IMPORT
#define EXPORT_CPP_PYGLUE		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_PYGLUE		EXPORT_CLASS_IMPORT

#endif

#endif

