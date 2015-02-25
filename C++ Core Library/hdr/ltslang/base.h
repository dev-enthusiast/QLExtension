/****************************************************************
**
**	BASE.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltslang/src/ltslang/base.h,v 1.1 2001/01/03 13:37:18 vengrd Exp $
**
****************************************************************/

#if !defined( IN_LTSLANG_BASE_H )
#define IN_LTSLANG_BASE_H

#ifdef BUILDING_LTSLANG
#define EXPORT_C_LTSLANG			EXPORT_C_EXPORT
#define EXPORT_CPP_LTSLANG			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_LTSLANG		EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_LTSLANG			EXPORT_C_IMPORT
#define EXPORT_CPP_LTSLANG			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_LTSLANG		EXPORT_CLASS_IMPORT
#endif

#endif 

