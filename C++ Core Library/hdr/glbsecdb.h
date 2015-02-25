/* $Header: /home/cvs/src/dialog/src/glbsecdb.h,v 1.3 1998/10/09 22:01:23 procmon Exp $ */
/****************************************************************
**
**	GLBSECDB.H	- Generic List Box SecPick
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_GLBSECDB_H )
#define IN_GLBSECDB_H


#define SECPICK_CLASS_NAME_SEPARATOR ','

#define MAX_SECPICK_TYPES 10


/*
**	Structure used to hold Secpick instance data
*/

typedef struct SecPickStructure
{
	SDB_DB_ID
			DbID;

	SDB_SEC_TYPE
			SecTypes[ MAX_SECPICK_TYPES ];	// SecTypes[0] = 0 for all classes

	int		SecNamesSize,
			NumSecTypes;

	SDB_SEC_NAME_ENTRY
			*SecNames;
			
} SEC_PICK;



/*
**	Prototype functions
*/

DLLEXPORT GLB_DATA_DRIVER
		*SecPickDataDriver(		void );
		
#endif

