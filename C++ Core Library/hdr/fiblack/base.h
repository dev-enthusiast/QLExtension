/****************************************************************
**
**	fiblack/base.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiblack/src/fiblack/base.h,v 1.4 2000/01/02 20:35:02 gribbg Exp $
**
****************************************************************/

#if !defined( IN_FIBLACK_BASE_H )
#define IN_FIBLACK_BASE_H


/*
**	FIBLACK - 
*/

#ifdef BUILDING_FIBLACK
#define EXPORT_C_FIBLACK			EXPORT_C_EXPORT
#define EXPORT_CPP_FIBLACK			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_FIBLACK		EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_FIBLACK			EXPORT_C_IMPORT
#define EXPORT_CPP_FIBLACK			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_FIBLACK		EXPORT_CLASS_IMPORT
#endif


#endif

