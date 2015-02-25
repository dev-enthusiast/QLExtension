/****************************************************************
**
**	n_base.h	- EXPORT definitions
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.1 $
**
****************************************************************/

#if !defined( IN_N_BASE_H )
#define IN_N_BASE_H

#ifdef BUILDING_NUM
#define EXPORT_C_NUM		EXPORT_C_EXPORT
#define EXPORT_CPP_NUM		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_NUM	EXPORT_CLASS_EXPORT
#else
#define EXPORT_C_NUM		EXPORT_C_IMPORT
#define EXPORT_CPP_NUM		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_NUM	EXPORT_CLASS_IMPORT
#endif

#endif

