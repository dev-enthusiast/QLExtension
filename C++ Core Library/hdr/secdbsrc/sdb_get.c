#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_get.c,v 1.39 2014/12/06 01:29:38 c038571 Exp $"
/****************************************************************
**
**	SDB_GET.C	- SecDb object retrieval functions
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_get.c,v $
**	Revision 1.39  2014/12/06 01:29:38  c038571
**	AWR: #352658.
**
**	Revision 1.38  2001/07/06 10:03:15  goodfj
**	Renamed SDB_DB.SearchPath to SecDbSearchPath, to avoid clashing with winbase.h
**
**	Revision 1.37  2000/02/11 21:44:00  singhki
**	modify Get/TransProtectedGet and HugeLoad APIs to return the Db in
**	which the Get/Load succeeded
**	
**	Revision 1.36  2000/02/08 22:24:06  singhki
**	No more references to DbID for virtual databases. All virtual
**	databases have the same DbID. This removes the limit on the number of
**	virtual databases you can create in memory.
**	
**	Revision 1.35  1999/10/19 13:18:31  singhki
**	clear SecPtrs before returning
**	
**	Revision 1.34  1999/10/13 19:17:14  singhki
**	Use flag to prevent bogus errors from GetByName during GetManyByName
**	
**	Revision 1.33  1999/09/01 15:29:30  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.32  1999/08/10 19:58:08  singhki
**	Register None and Null as diddle scopes in order to preserve behaviour
**	
**	Revision 1.31  1999/08/07 01:40:09  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.30  1999/06/28 22:51:55  singhki
**	reject names >= SDB_SEC_NAME_SIZE in get
**	
**	Revision 1.29  1998/12/09 01:28:28  singhki
**	Use ERR_OFF/ON instead of ErrPush/Pop
**	
**	Revision 1.28  1998/11/16 22:59:58  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.27  1998/11/13 21:12:58  lundek
**	cleanup and be rid of link-ish warnings
**	
**	Revision 1.26.2.1  1998/10/30 00:03:17  singhki
**	DiddleColors: Initial revision
**	
**	Revision 1.26  1998/10/27 16:19:50  vengrd
**	distinguish between sub and other virtual dbs
**	
**	Revision 1.25  1998/10/22 18:35:30  vengrd
**	More subdb.  Almost register now
**	
**	Revision 1.24  1998/10/15 23:11:13  singhki
**	BugFix: Return correct SuccessCount in GetManyByName
**	
**	Revision 1.23  1998/10/14 18:20:52  singhki
**	New search paths
**	
**	Revision 1.22.2.2  1998/10/01 13:34:51  singhki
**	make GetManyByName work
**	
**	Revision 1.22.2.1  1998/08/28 23:33:57  singhki
**	search paths, index merge working, some problems with ~casts
**	
**	Revision 1.22  1998/07/09 02:32:32  nauntm
**	casts and fixes to quiet compiler
**	
**	Revision 1.21  1998/02/19 17:44:09  gribbg
**	Allow get Huge Objects from Syncpoints
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stddef.h>
#include	<string.h>
#include	<math.h>
#include	<time.h>
#include	<hash.h>
#include	<date.h>
#include	<secdb.h>
#include	<secdt.h>
#include	<secobj.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<sectrans.h>
#include	<secdrv.h>
#include	<sdb_int.h>



/****************************************************************
**	Routine: GetManyByName
**	Returns: Number of objects gotten
**	Action : Load many objects from driver
****************************************************************/

static int GetManyByName(
	SDB_DB				*pDb,
	SDB_DB				*ParentDb,			// Parent Db which the SecPtrs should belong to
	unsigned			DbIdFlags,
	int					Count,
	const char			**SecNames,
	SDB_OBJECT			**SecPtrs,
	const char			**GetNames,
	SDB_STORED_OBJECT	**StoredObjectList
)
{
	int SuccessCount;

	if( !pDb->Left )					// real database, go do the GetManyByName
	{
		int SecNum;

		SuccessCount = 0;
		// Fill up the precache
		pDb->PreCacheDepth++;
		if( pDb->pfGetManyByName )
		{
			SDB_STORED_OBJECT
					*StoredObjectPtr;

			int		GetCount = 0;

			// Do not load objects that are already loaded (if possible)
			for( SecNum = 0; SecNum < Count; SecNum++ )
				if( NULL == SecPtrs[ SecNum ] && NULL != SecNames[ SecNum ] )
					if(( DbIdFlags & SDB_REFRESH_CACHE )
					   || NULL == ( SecPtrs[ SecNum ] = SecDbGetByName( SecNames[ SecNum ], ParentDb, SDB_CACHE_ONLY | SDB_IGNORE_PATH | SDB_GET_FATAL_ERRORS_ONLY )))
					{
						if( !HashLookup( pDb->PreCacheHash, SecNames[ SecNum ]))
							GetNames[ GetCount++ ] = SecNames[ SecNum ];
					}

			// Call the database get function and stuff each successfully gotten object into the precache
			if( GetCount > 0 )
				if( (*pDb->pfGetManyByName)( pDb, GetNames, GetCount, StoredObjectList ))
					for( SecNum = 0; SecNum < GetCount; SecNum++ )
					{
						if(( StoredObjectPtr = StoredObjectList[ SecNum ]) == NULL )
						{
							StoredObjectPtr = (SDB_STORED_OBJECT *) calloc( 1, sizeof( *StoredObjectPtr ));
							strcpy( StoredObjectPtr->SdbDisk.Name, GetNames[ SecNum ] );
						}
						HashInsert( pDb->PreCacheHash, StoredObjectPtr->SdbDisk.Name, StoredObjectPtr );
					}
		}

		// do a GetByName to move objects from precache to cache
		for( SecNum = 0; SecNum < Count; SecNum++ )
			if( NULL == SecPtrs[ SecNum ])
				SuccessCount += NULL != ( SecPtrs[ SecNum ] = SecDbGetByName( SecNames[ SecNum ], ParentDb, SDB_IGNORE_PATH | SDB_GET_FATAL_ERRORS_ONLY ));
			else
				++SuccessCount;

		// Clear out the pre-cache when we get down to zero
		pDb->PreCacheDepth--;
		if( pDb->PreCacheDepth == 0 && pDb->PreCacheHash->KeyCount > 0 )
			SecDbFlushPreCache( pDb );
	}
	else
	{
		// get from underlying databases
		SuccessCount = GetManyByName( pDb->Left, ParentDb, DbIdFlags, Count, SecNames, SecPtrs, GetNames, StoredObjectList );
		if( SuccessCount < Count && pDb->Right )
			SuccessCount = GetManyByName( pDb->Right, ParentDb, DbIdFlags, Count, SecNames, SecPtrs, GetNames, StoredObjectList );
	}
	return SuccessCount;
}



/****************************************************************
**	Routine: SecDbGetManyByName
**	Returns: Number of objects gotten
**	Action : Load many objects from the database.
****************************************************************/

int SecDbGetManyByName(
	SDB_DB		*pDb,			// ID of database to get from
	unsigned	Flags,			// Flags
	const char 	**SecNames,		// Array of object names
	int			Count,			// Number of objects to get
	SDB_OBJECT	**SecPtrs		// Returned array of object pointers
)
{
	SDB_STORED_OBJECT
			**StoredObjectList;

	const char
			**GetNames;

	int		SecNum,
			SuccessCount = 0;

	SDB_DB_SEARCH_PATH
			TempElem,
			*SearchElem;

	memset( SecPtrs, 0, sizeof( SDB_OBJECT * ) * Count );

	if( !( pDb = DbToPhysicalDb( pDb )))
		return 0;

	// Allocate the buffers which GetManyByName will use
	if( !ERR_CALLOC( GetNames, const char *, Count, sizeof( *GetNames )))
		return 0;
	if( !ERR_CALLOC( StoredObjectList, SDB_STORED_OBJECT *, Count, sizeof( *StoredObjectList )))
		return 0;

	// First get anything available in current cache
	if( !( Flags & SDB_REFRESH_CACHE ))
		for( SecNum = 0; SecNum < Count; SecNum++ )
			if( NULL != SecNames[ SecNum ] )
			{
				SecPtrs[ SecNum ] = SecDbGetByName( SecNames[ SecNum ], pDb, SDB_CACHE_ONLY | SDB_GET_FATAL_ERRORS_ONLY );
				SuccessCount += NULL != SecPtrs[ SecNum ];
			}

	// Do GetManyByName in each db in search path until all are found
	if( !pDb->ParentDb->SecDbSearchPath || ( Flags & SDB_IGNORE_PATH ))
	{
		TempElem.Next	= NULL;
		TempElem.Db		= pDb;
		SearchElem		= &TempElem;
	}
	else
		SearchElem 		= pDb->ParentDb->SecDbSearchPath;

	for( ; SearchElem && SuccessCount < Count; SearchElem = SearchElem->Next )
	{
		// Go do the GetManyByName from the database
		SuccessCount = GetManyByName( SearchElem->Db, SearchElem->Db, Flags,
									  Count, SecNames, SecPtrs, GetNames, StoredObjectList );
	}

	free( StoredObjectList );
	free( GetNames );
	return SuccessCount;
}



/****************************************************************
**	Routine: SecDbGetMany
**	Returns: Number of objects gotten
**	Action : Load many objects from the database.
****************************************************************/

int SecDbGetMany(
	SDB_DB		*Db,			// ID of database to get from
	unsigned	Flags,			// Flags
	DT_VALUE	*SecNames,		// DtArray value of object names (or DtStructure, in which case the tag names are used)
	DT_VALUE	*SecPtrList,	// Returned DtSecurityList value
	int			PassErrors		// If not set, fail if not all objects gotten
)
{
	DT_SECURITY_LIST
			*SecList;

	char	**NamesList;

	int		SecNum,
			Status,
			Gotten;

	HASH_ENTRY_PTR
			HashEntry;
			
	DT_VALUE
			Null,
			Security;


	DTM_INIT( SecPtrList );

	if( !ERR_MALLOC( SecList, DT_SECURITY_LIST, sizeof( *SecList )))
		return FALSE;

	SecList->Size = DtSize( SecNames );
	if( !ERR_CALLOC( SecList->SecPtrs, SDB_OBJECT*, SecList->Size, sizeof( SDB_OBJECT * )))
		return FALSE;

	if( !ERR_MALLOC( NamesList, char *, SecList->Size * sizeof( *NamesList )))
		return FALSE;
		
	SecPtrList->DataType = DtSecurityList;
	SecPtrList->Data.Pointer = SecList;

	if( SecNames->DataType == DtStructure )
	{
		SecNum = 0;
		HASH_FOR( HashEntry, (HASH *) SecNames->Data.Pointer )
			NamesList[ SecNum++ ] = (char *) HashKey( HashEntry );
		Status = ( SecList->Size == SecDbGetManyByName( Db, Flags, (const char **) NamesList, SecList->Size, SecList->SecPtrs ) || PassErrors );
	}
	else
	{
		for( SecNum = 0; SecNum < SecList->Size; SecNum++ )
			NamesList[ SecNum ] = (char *) DtSubscriptGetPointer( SecNames, SecNum, DtString, NULL );
		Gotten = SecDbGetManyByName( Db, Flags, (const char **) NamesList, SecList->Size, SecList->SecPtrs );
		if( SecList->Size > Gotten )
		{
			// If not all of the securities were gotten, maybe they can
			// be gotten directly from source (e.g., if source is already 
			// DtSecurity)
			
			DTM_INIT( &Null );
			DTM_INIT( &Security );
			
			for( SecNum = 0; SecNum < SecList->Size; SecNum++ )
				if( !SecList->SecPtrs[ SecNum ] )
				{
					Security.DataType = DtSecurity;
					if( DTM_TO( &Security, DtSubscriptGet( SecNames, SecNum, &Null )))
					{
						SecList->SecPtrs[ SecNum ] = (SDB_OBJECT *) Security.Data.Pointer;
						Gotten++;
					}
					else if( ERR_UNSUPPORTED_OPERATION == ErrNum )
					{
						DTM_FREE( SecPtrList );
						DTM_INIT( SecPtrList );
				 		free( NamesList );
				 		Err( ERR_UNSUPPORTED_OPERATION, "GetMany: Array elements must be securities, or names of securities, etc" );
				 		return FALSE;
					}
					else if( !PassErrors )
						break;
				}
		}
		Status = ( SecList->Size == Gotten ) || PassErrors;
	}


	if( !Status )
	{
		// Refresh first error
		for( SecNum = 0; SecNum < SecList->Size; SecNum++ )
			if( !SecList->SecPtrs[ SecNum ] )
				SecDbGetByName( NamesList[ SecNum ], Db, Flags );
		ERR_OFF();
		DTM_FREE( SecPtrList );
		DTM_INIT( SecPtrList );
		ERR_ON();
	}
	free( NamesList );
	return Status;
}



/****************************************************************
**	Routine: SecDbFlushPreCache
**	Returns: 
**	Action : 
****************************************************************/

void SecDbFlushPreCache(
	SDB_DB		*pDb
)
{
	SDB_STORED_OBJECT
			*StoredObjectPtr;

	HASH_ENTRY_PTR
			EnumPtr;


	HASH_FOR( EnumPtr, pDb->PreCacheHash )
	{
		StoredObjectPtr = (SDB_STORED_OBJECT *) HashValue( EnumPtr );
		HashDeleteByBucket( pDb->PreCacheHash, StoredObjectPtr->SdbDisk.Name, HashBucket( EnumPtr ));
		free( StoredObjectPtr->Mem );
		free( StoredObjectPtr );
	}
}



/****************************************************************
**	Routine: SecDbGetFromDatabase
**	Returns: Db from which the get succeeded / NULL if failed
**	Action : Retrieve object data from the precache or the
**			 database.  Remove the object data if it came from
**			 the precache.
****************************************************************/

SDB_DB *SecDbGetFromDatabase(
	SDB_DB				*pDb,
	const char			*SecName,
	SDB_STORED_OBJECT	*StoredObject,
	int					Raw
)
{
	SDB_STORED_OBJECT
			*StoredObjectPtr;

	HASH_ENTRY_PTR
			EntryPtr;

	SDB_DB	*FoundDb;

	if( pDb == NULL ) return NULL; 
			
	if( pDb->IsVirtual() )
		return ( FoundDb = SecDbGetFromDatabase( pDb->Left, SecName, StoredObject, Raw ))
				|| ( pDb->Right && ( FoundDb = SecDbGetFromDatabase( pDb->Right, SecName, StoredObject, Raw ))) ? FoundDb : NULL;

	if( pDb->PreCacheHash->KeyCount == 0 || NULL == (EntryPtr = HashLookupPtr( pDb->PreCacheHash, SecName )))
	{
		if( strlen( SecName ) >= SDB_SEC_NAME_SIZE )
		{
			Err( ERR_INVALID_ARGUMENTS, "SecName must be < %d characters", SDB_SEC_NAME_SIZE );
			return NULL;
		}
		if( !(*pDb->pfGetByName)( pDb, SecName, StoredObject ))
		{
			ErrMore( "Get( %s )", SecName );
			return NULL;
		}
	}
	else
	{
		StoredObjectPtr = (SDB_STORED_OBJECT *) HashValue( EntryPtr );
		HashDeleteByBucket( pDb->PreCacheHash, SecName, HashBucket( EntryPtr ));
		if( StoredObjectPtr->SdbDisk.DateCreated == 0 )
		{
			free( StoredObjectPtr );
			Err( SDB_ERR_OBJECT_NOT_FOUND, "@ - Get( %s )", SecName );
			return NULL;
		}
		
		*StoredObject = *StoredObjectPtr;
		free( StoredObjectPtr );
	}
	
	if( Raw )
		return pDb;
	return SecDbHugeLoad( pDb, -1, StoredObject );
}
