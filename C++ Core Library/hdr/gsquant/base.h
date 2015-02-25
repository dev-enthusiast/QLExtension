/****************************************************************
**
**	gsquant/base.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/base.h,v 1.2 1999/12/20 15:54:30 gribbg Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_BASE_H )
#define IN_GSQUANT_BASE_H


/*
**	GSQUANT - 
*/

#ifdef BUILDING_GSQUANT
#define EXPORT_C_GSQUANT			EXPORT_C_EXPORT
#define EXPORT_CPP_GSQUANT			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GSQUANT		EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_GSQUANT			EXPORT_C_IMPORT
#define EXPORT_CPP_GSQUANT			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GSQUANT		EXPORT_CLASS_IMPORT
#endif



/*
**	GSQUANTE - 
*/

#ifdef BUILDING_GSQUANTE
#define EXPORT_C_GSQUANTE			EXPORT_C_EXPORT
#define EXPORT_CPP_GSQUANTE			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GSQUANTE		EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_GSQUANTE			EXPORT_C_IMPORT
#define EXPORT_CPP_GSQUANTE			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GSQUANTE		EXPORT_CLASS_IMPORT
#endif

#endif

