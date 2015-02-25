/****************************************************************
**
**	GOB_BASE.H	- EXPORT macro declarations for gob
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gob/src/gob_base.h,v 1.1 1999/12/21 23:22:43 singhki Exp $
**
****************************************************************/

#if !defined( IN_GOB_BASE_H )
#define IN_GOB_BASE_H

#if defined( BUILDING_GOB )

#define EXPORT_C_GOB		EXPORT_C_EXPORT
#define EXPORT_CPP_GOB		EXPORT_CPP_EXPORT
#define EXPORT_CLASS_GOB	EXPORT_CLASS_EXPORT

#else

#define EXPORT_C_GOB		EXPORT_C_IMPORT
#define EXPORT_CPP_GOB		EXPORT_CPP_IMPORT
#define EXPORT_CLASS_GOB	EXPORT_CLASS_IMPORT

#endif

#endif 
