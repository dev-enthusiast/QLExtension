#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_onum.c,v 1.12 1999/09/01 15:29:31 singhki Exp $"
/****************************************************************
**
**	SDB_ONUM.C	- SecDb cached object enumeration functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_onum.c,v $
**	Revision 1.12  1999/09/01 15:29:31  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**
**	Revision 1.11  1999/08/10 19:58:08  singhki
**	Register None and Null as diddle scopes in order to preserve behaviour
**	
**	Revision 1.10  1999/08/07 01:40:10  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.9  1998/11/16 22:59:59  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.8.4.1  1998/10/30 00:03:18  singhki
**	DiddleColors: Initial revision
**	
**	Revision 1.8  1998/01/02 21:33:06  rubenb
**	added GSCVSID
**	
**	Revision 1.7  1996/12/23 03:06:21  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**	
**	Revision 1.6  1996/01/16 17:59:56  ERICH
**	NT Compatibility
**
**	   Rev 1.5   09 Jan 1995 12:35:54   DUBNOM
**	Changed database search mechanism
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stddef.h>
#include	<string.h>
#include	<hash.h>
#include	<secdb.h>
#include	<secdrv.h>
#include	<secerror.h>
#include	<secindex.h>
#include	<sdb_int.h>



/****************************************************************
**	Routine: SecDbObjectEnumFirst
**	Returns: Pointer to first object in cache
**			 NULL if the object cache is empty or database is invalid
**	Summary: Start the enumeration of cached objects
****************************************************************/

SDB_OBJECT *SecDbObjectEnumFirst(
	SDB_DB			*pDb,		// Database to enumerate objects for
	SDB_ENUM_PTR	*EnumPtr	// Pointer to pointer to return
)
{
	if( !( pDb = DbToPhysicalDb( pDb )))
	{
		Err( ERR_DATABASE_INVALID, "ObjectEnumFirst() @" );
		return NULL;
	}

	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));
	(*EnumPtr)->Hash	= pDb->CacheHash;
	(*EnumPtr)->Count	= (*EnumPtr)->Hash->KeyCount;
	(*EnumPtr)->Ptr		= HashFirst( (*EnumPtr)->Hash );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;
	return( (SDB_OBJECT *) HashValue( (*EnumPtr)->Ptr ));
}



/****************************************************************
**	Routine: SecDbObjectEnumNext
**	Returns: Pointer to next object in cache
**			 NULL if there are no more objects in the cache
**	Summary: Return the next object in the object cache
****************************************************************/

SDB_OBJECT *SecDbObjectEnumNext(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( EnumPtr->Hash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;
	return( (SDB_OBJECT *) HashValue( EnumPtr->Ptr ));
}



/****************************************************************
**	Routine: SecDbObjectEnumClose
**	Returns: Nothing
**	Summary: End enumeration of cached objects
****************************************************************/

void SecDbObjectEnumClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}

