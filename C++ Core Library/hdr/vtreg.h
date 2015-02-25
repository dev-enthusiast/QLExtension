/* $Header: /home/cvs/src/secmeta/src/vtreg.h,v 1.12 2001/07/24 10:30:00 goodfj Exp $ */
/****************************************************************
**
**	VTREG.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secmeta/src/vtreg.h,v 1.12 2001/07/24 10:30:00 goodfj Exp $
**
****************************************************************/

#if !defined( IN_VTREG_H )
#define IN_VTREG_H

#include <secdb.h>

#define 	VTR_FLAG_GENERIC		0x0001	// Generic value type widely used among objects.
#define		VTR_FLAG_DOWNSTREAM		0x0002	// Downstream systems are known to care.
#define		VTR_FLAG_TRADABLE		0x0004	// Tradable securities use the VT.
#define		VTR_FLAG_NAMES_SEC		0x0008	// VT names another security

#define		VTR_FLAG_ERROR			0x8000	// VT has an error, e.g. conflicting types in different classes.

DLLEXPORT int
		VtrObjFuncVtRegistryEntry(SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData );

DLLEXPORT SDB_OBJECT 
		*VtrGetEntry( SDB_DB *MetadataDb, char *VtName );


// The maximum number of class IDs that we are willing to write into a
// VT Registry Entry.

#define		VTRE_MAX_CLASSES_PER_VT		2000

#endif 
