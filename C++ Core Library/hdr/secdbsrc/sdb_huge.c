#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_huge.c,v 1.39 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SDB_HUGE.C
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_huge.c,v $
**	Revision 1.39  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.38  2001/11/27 23:23:37  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.36  2001/06/29 15:22:14  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.35  2000/11/24 18:14:25  goodfj
**	Check for hugeness against MaxMemSize of update db
**	
**	Revision 1.34  2000/09/06 13:52:39  goodfj
**	Move (duplicated) huge obj defines to sdb_int.h
**	
**	Revision 1.33  2000/08/30 13:53:31  goodfj
**	Fixes for huge objects in subdbs
**	
**	Revision 1.32  2000/08/29 14:13:04  goodfj
**	Replaced SDB_DB_SEARCH PATH with SDB_DB_UNION
**	
**	Revision 1.31  2000/08/25 23:24:20  singhki
**	Add MaxMemSize to SDB_DB struct since different Db drivers may have different limits
**	
**	Revision 1.30  2000/07/11 17:01:45  goodfj
**	PageTableUpdate takes pDbUpdate arg
**	
**	Revision 1.29  2000/06/12 14:42:41  goodfj
**	DbUpdate fixes
**	
**	Revision 1.28  2000/02/11 21:43:31  singhki
**	modify TransProtectedGet and HugeLoad APIs to return the Db in which
**	the Get/Load succeeded
**	
**	Revision 1.27  2000/02/08 22:24:06  singhki
**	No more references to DbID for virtual databases. All virtual
**	databases have the same DbID. This removes the limit on the number of
**	virtual databases you can create in memory.
**	
**	Revision 1.26  1999/12/10 20:03:19  singhki
**	Allow unload from virtual Dbs like ClassObjectsDb
**	
**	Revision 1.25  1999/12/06 13:50:33  goodfj
**	DbUpdate is now private. New api to access it (for the time-being)
**	
**	Revision 1.24  1999/09/01 15:29:30  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.23  1999/08/07 01:40:09  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.22  1999/03/14 22:34:49  rubenb
**	change min to MIN
**	
**	Revision 1.21  1999/03/01 23:13:49  vengrd
**	Huge object support for subdbs
**	
**	Revision 1.20  1999/02/22 18:55:12  singhki
**	BugFix: Fix Huge object code to send transactions to the union Db but update the PageTable in the Update Db.
**	
**	Revision 1.19  1998/10/14 18:20:52  singhki
**	New search paths
**	
**	Revision 1.18.2.1  1998/08/28 23:33:58  singhki
**	search paths, index merge working, some problems with ~casts
**	
**	Revision 1.18  1998/07/09 02:32:34  nauntm
**	casts and fixes to quiet compiler
**	
**	Revision 1.17  1998/05/27 06:19:30  gribbg
**	Use DateToday()
**	
**	Revision 1.16  1998/02/19 17:44:10  gribbg
**	Allow get Huge Objects from Syncpoints
**	
**	Revision 1.15  1998/01/02 21:33:01  rubenb
**	added GSCVSID
**	
**	Revision 1.14  1997/11/12 19:19:38  lundek
**	Fail to load huge objects whose pages have changed underneath it
**	
**	Revision 1.13  1997/11/12 17:39:07  lundek
**	SecDbHugeDelete (support for seccopy and secsync of huge objects)
**	
**	Revision 1.12  1997/11/03 21:52:30  lundek
**	Limit object stream size by class (default: 32711)
**	
**	Revision 1.11  1997/11/03 16:42:07  lundek
**	Fix secsyncing of huge objects (move paging-intercept from SecDbTransactionAdd level to SecDbAdd/Update/Delete/Rename level).
**	
**	Revision 1.10  1997/10/29 20:41:51  lundek
**	casts
**	
**	Revision 1.9  1997/08/15 17:17:02  lundek
**	LastTransaction in SDB_DISK
**	
**	Revision 1.8  1997/08/08 15:51:30  lundek
**	BugFix: Rename of non-huge objects
**	
**	Revision 1.7  1997/07/16 12:19:09  schlae
**	Casts
**
**	Revision 1.6  1997/07/16 03:43:15  lundek
**	Fix unix warnings
**
**	Revision 1.5  1997/07/16 03:18:20  lundek
**	Huge objects
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<gmt.h>
#include	<secdb.h>
#include	<sdb_int.h>
#include	<sectrans.h>
#include	<secver.h>
#include	<secerror.h>


/****************************************************************
**	Routine: PageGetFromDb
**	Returns: TRUE / Err()
**	Action : Look for a page.  If we are looking in a union,
**		     recursively look in all components.  If we are 
**			 looking in a subdb, look back up in the physical
**			 root of the db.
****************************************************************/

static int PageGetFromDb(
	SDB_DB 				*pDb,
	const char			*SecName,
	SDB_STORED_OBJECT	*StoredObject,
	SDB_DISK_VERSION	*Version
)
{
	// We have to look in the root database of each database
	// in a union, since that is where the huge objects will
	// be.

	switch( pDb->DbOperator )
	{
		case SDB_DB_SUBDB:
		{
			SDB_DB 	*DbForPages = pDb;

			// Walk up to the root db before looking for the object.
			while( DbForPages->DbOperator == SDB_DB_SUBDB )
				DbForPages = DbForPages->Left; 

			return PageGetFromDb( DbForPages, SecName, StoredObject, Version );
		}

		case SDB_DB_UNION:
			// Try the left and then the right.
			// FIX - better error checking?
			return (    PageGetFromDb( pDb->Left, SecName, StoredObject, Version ) 
						|| PageGetFromDb( pDb->Right, SecName, StoredObject, Version ) );

		case SDB_DB_LEAF:
			// Get from active database/transaction
			return ( SecDbTransProtectedGet( pDb, SecName, StoredObject, TRUE ) != NULL ) && SecDbDiskInfoGetVersion( &(StoredObject->SdbDisk), Version );

		default:
			// FIX - Is it always correct to simply try the left.
			return PageGetFromDb( pDb->Left, SecName, StoredObject, Version );
	}
}



/****************************************************************
**	Routine: PageTableAssembleStream
**	Returns: TRUE or FALSE
**	Action : Given a page table, reads the pages from the database
**			 and pastes them together into a single BStream
****************************************************************/

static int PageTableAssembleStream(
	SDB_DB 			*pDb,
	int				SyncPointOffset,	// -1 for current database/transaction
	const char		*PageTable,
	long			UpdateCount,
	unsigned long	*pMemSize,
	void			**pMem
)
{
	int		Count,
			PageSize;

	char	*Mem,
			*CurMem;

	const char
			*SecName;

	SDB_STORED_OBJECT
			StoredObject;

	SDB_DISK_VERSION
			Version;


	Count = 0;
	for( SecName = PageTable; *SecName; SecName += strlen( SecName ) + 1 )
		Count++;

	if( !ERR_MALLOC( Mem, char, Count * SDB_HUGE_PAGE_SIZE ))
		return FALSE;
	CurMem = Mem;

	// FIX - use GetManyByName if available
	for( SecName = PageTable; *SecName; SecName += strlen( SecName ) + 1 )
	{
		if( SyncPointOffset < 0 )
		{
			if( !PageGetFromDb( pDb, SecName, &StoredObject, &Version ) )
			{
				free( Mem );
				return FALSE;
			}
		}
		else
		{
			// Get from syncpoint
			if( 	!(*pDb->pfGetFromSyncPoint)( pDb, SecName, SyncPointOffset, &StoredObject )
				||	!SecDbDiskInfoGetVersion( &StoredObject.SdbDisk, &Version )) 
			{
				free( Mem );
				return FALSE;
			}
		}
		if( Version.Object > 0 )
		{
			// Check that the page in db is the one for this version of pagetable (since db driver does not support transaction-protected reads)
			if( StoredObject.SdbDisk.UpdateCount > UpdateCount )
			{
				free( StoredObject.Mem );
				free( Mem );
				return Err( SDB_ERR_OBJECT_MODIFIED, "@: Current page %s is for version %d", SecName, StoredObject.SdbDisk.UpdateCount );
			}
		}
		PageSize = StoredObject.MemSize - SDB_SEC_NAME_SIZE;
		memcpy( CurMem, StoredObject.Mem, PageSize );
		CurMem += PageSize;
		free( StoredObject.Mem );
	}
	*pMem 		= Mem;
	*pMemSize 	= CurMem - Mem;
	return TRUE;
}




/****************************************************************
**	Routine: PageTableDestroy
**	Returns: TRUE or FALSE
**	Action : Deletes all the given page objects from the database
**			 (within the current transaction context).
****************************************************************/

static int PageTableDestroy(
	SDB_DB		*pDb,
	SDB_DB		*pDbForPages,
	const char	*PageTable
)
{
	SDB_TRANS_PART
			*TransPart;

	SDB_DISK
			DiskInfo;

	const char
			*SecName;

	int		Status = TRUE;


	for( SecName = PageTable; *SecName; SecName += strlen( SecName ) + 1 )
	{
		if( !SecDbTransProtectedDiskInfo( pDb, SecName, &DiskInfo, pDbForPages ))
		{
			Status = FALSE;
			continue;
		}

		if( !ERR_CALLOC( TransPart, SDB_TRANS_PART, 1, sizeof( SDB_TRANS_PART )))
			return FALSE;

		TransPart->Type = SDB_TRAN_DELETE;
		TransPart->Data.Delete.SdbDisk = DiskInfo;

		// Following makes router-type syncing and conflict resolution easier
		// (the version checking on the owner object provides the same protection anyway)
		TransPart->Data.Delete.IgnoreErrors = TRUE;

		if( pDb->TransTail )
			pDb->TransTail->Next = TransPart;
		else
			pDb->TransHead = TransPart;
		TransPart->Prev	= pDb->TransTail;
		pDb->TransTail	= TransPart;
	}
	return Status;
}



/****************************************************************
**	Routine: PageTableUpdate
**	Returns: TRUE or FALSE
**	Action : Assembles new set of pages and for each one that is
**		     different destroys the old page and creates a new
**			 page (within the current transaction context).
****************************************************************/

static int PageTableUpdate(
	SDB_DB			*pDb,			// Database (may be a union etc)
	SDB_DB			*pDbForPages,	// Physical db that owns the huge pages
	const char		*Owner,			// Name of huge object
	const char		*PageTable,		// Current page table; NULL for new (or newly huge) objects
	long			UpdateCount,
	unsigned long	*pMemSize,		// sizeof( **pMem ); NULL for rename
	void			**pMem			// In: object stream; Out: new page table; NULL for rename
)
{
	unsigned long
			MemLeft = 0,
			PageSize;

	int		Count;

	const char
			*SecName;

	char	*NewSecName		= NULL,
			*NewPageTable 	= NULL,
			*CurMem			= NULL;

	SDB_TRANS_TYPE
			Type;

	SDB_TRANS_PART
			*TransPart;

	SDB_STORED_OBJECT
			StoredObject;

	SDB_DISK_VERSION
			Version;


	if( pMemSize )
	{
		CurMem	= (char *) *pMem;
		MemLeft = *pMemSize;
		Count	= 1 + (( MemLeft - 1 ) / SDB_HUGE_PAGE_SIZE );

		if( !ERR_CALLOC( NewPageTable, char, Count + 1, SDB_HUGE_PAGE_NAME_SIZE ))
			return FALSE;
		NewSecName	= NewPageTable;
	}


/*
**	Write each page
*/

	SecName	= PageTable;
	while( !pMemSize || MemLeft > 0 )
	{
		// Use existing page
		if( SecName && *SecName )
		{
			if( !SecDbTransProtectedGet( pDb, SecName, &StoredObject, TRUE, pDbForPages ))
				goto FAILED;
			Type 		= SDB_TRAN_UPDATE;
			if( pMemSize )
				NewSecName += 1 + sprintf( NewSecName, "%s", SecName );
		}

		// or create new page
		else
		{
			if( !pMemSize )
				break;

			memset( &StoredObject, 0, sizeof( StoredObject ));
			sprintf( StoredObject.SdbDisk.Name, SDB_HUGE_PAGE_NAME_FORMAT, (long) SecDbUniqueID( pDbForPages ));
			StoredObject.SdbDisk.Type			= SDB_HUGE_PAGE_SEC_TYPE;
			StoredObject.SdbDisk.TimeUpdated	= GMTGetTime();
			StoredObject.SdbDisk.DateCreated	= (unsigned short) DateToday();
			StoredObject.SdbDisk.DbIDUpdated	= pDbForPages->DbID;
			Type 		= SDB_TRAN_INSERT;
			NewSecName += 1 + sprintf( NewSecName, "%s", StoredObject.SdbDisk.Name );
		}

		if( pMemSize )
			PageSize	= MIN( SDB_HUGE_PAGE_SIZE, MemLeft );
		else
			PageSize 	= StoredObject.MemSize - SDB_SEC_NAME_SIZE;

		// Add a transaction part for the page object (if it has changed)
		if( !pMemSize
				|| Type == SDB_TRAN_INSERT // (in case we are inserting a page-full of zeros)
				|| StoredObject.MemSize != ( PageSize + SDB_SEC_NAME_SIZE )
				|| 0 != memcmp( StoredObject.Mem, CurMem, PageSize ))
		{
			StoredObject.Mem = realloc( StoredObject.Mem, PageSize + SDB_SEC_NAME_SIZE );
			if( !StoredObject.Mem )
				goto FAILED;

			if( pMemSize )
				memcpy( StoredObject.Mem, CurMem, PageSize );
			memset( (char *) StoredObject.Mem + PageSize, 0, SDB_SEC_NAME_SIZE );
			strcpy( (char *) StoredObject.Mem + PageSize, Owner );
			StoredObject.MemSize = PageSize + SDB_SEC_NAME_SIZE;

			if( !ERR_CALLOC( TransPart, SDB_TRANS_PART, 1, sizeof( SDB_TRANS_PART )))
				goto FAILED;

			TransPart->Type = Type;

			TransPart->Data.Op.OldSdbDisk	= StoredObject.SdbDisk;
			TransPart->Data.Op.StoredObject = StoredObject;

			TransPart->Data.Op.StoredObject.SdbDisk.TimeUpdated 	= GMTGetTime();
			TransPart->Data.Op.StoredObject.SdbDisk.DbIDUpdated 	= pDbForPages->DbID;
			TransPart->Data.Op.StoredObject.SdbDisk.UpdateCount		= UpdateCount;

			Version.Stream = 0;
			Version.Object = 1;
			SecDbDiskInfoSetVersion( &TransPart->Data.Op.StoredObject.SdbDisk, &Version );

			if( pDb->TransTail )
				pDb->TransTail->Next = TransPart;
			else
				pDb->TransHead = TransPart;
			TransPart->Prev	= pDb->TransTail;
			pDb->TransTail	= TransPart;
		}
		else
			free( StoredObject.Mem );


		if( pMemSize )
		{
			MemLeft -= PageSize;
			CurMem	+= PageSize;
		}
		if( SecName && *SecName )
			SecName += strlen( SecName ) + 1;
	}


/*
**	Delete all unused old pages
*/

	if( SecName )
		if( !PageTableDestroy( pDb, pDbForPages, SecName ))
			goto FAILED;


/*
**	New mem is just the new page table
*/

	if( pMemSize )
	{
		free( *pMem );
		*NewSecName 	= '\0';
		*pMem 			= NewPageTable;
		*pMemSize		= ( NewSecName - NewPageTable ) + 1;
	}
	return TRUE;

FAILED:
	free( NewPageTable );
	return FALSE;
}



/****************************************************************
**	Routine: SecDbHugeLoad
**	Returns: Db in which load succeeded / NULL
**	Action : If the object mem contains a page table for a huge
**			 object, constructs the stream and keeps track of the
**			 page table.
****************************************************************/

SDB_DB *SecDbHugeLoad(
	SDB_DB				*pDb,
	int					SyncPointOffset,
	SDB_STORED_OBJECT	*Object
)
{
	HASH_ENTRY_PTR
			Entry;

	char	*PageTable,
			*Name;


	if( SDB_IS_HUGE( Object->SdbDisk.VersionInfo ))
	{
		// Free current page table if it is already stored
		Entry = HashLookupPtr( pDb->PageTableHash, Object->SdbDisk.Name );
		if( Entry )
		{
			Name 		= (char *) HashKey( Entry );
			PageTable 	= (char *) HashValue( Entry );
			free( PageTable );
			HashDeleteByBucket( pDb->PageTableHash, Name, HashBucket( Entry ));
		}
		else
			Name = strdup( Object->SdbDisk.Name );


		// Construct expanded stream while keeping track of page table in the hash
		PageTable = (char *) Object->Mem;
		if( !PageTableAssembleStream( pDb, SyncPointOffset, PageTable, Object->SdbDisk.UpdateCount, &Object->MemSize, &Object->Mem ))
		{
			free( Name );
			ErrMore( "Assembling pages for %s, version %d", Object->SdbDisk.Name, Object->SdbDisk.UpdateCount );
			return NULL;
		}
		HashInsert( pDb->PageTableHash, Name, PageTable );

		// Mark disk as not being huge since its pagetable is now in the hash
		SDB_SET_HUGE( Object->SdbDisk.VersionInfo, FALSE );
	}
	return pDb;
}



/****************************************************************
**	Routine: SecDbHugeUpdate
**	Returns: TRUE or FALSE
**	Action : Add, update, or delete all the page objects in order
**			 to change given object to its current stream (within
**			 current transaction)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbHugeUpdate"

int SecDbHugeUpdate(
	SDB_DB				*pDb,			// Database
	SDB_DISK			*OldDisk,		// SdbDisk before update
	SDB_DISK			*NewDisk,		// SdbDisk after update (In/Out); NULL for delete
	unsigned long		*pMemSize,		// sizeof( **pMem ); NULL for delete or rename
	void				**pMem			// In: object stream; Out: new page table; NULL for delete or rename
)
{
	char	*PageTable,
			*Name;

	HASH_ENTRY_PTR
			Entry;

	SDB_STORED_OBJECT
			DiskObject;

	int		Status,
			Update,
			Rename,
			WasHuge,
			TooBig;

	SDB_DB	*pDbForPageTableHash,	// This is the db where PageTableHash lives
			*pDbForPages;			// This is the db where huge page objs live


	pDbForPages = SecDbDbUpdateGet( pDb, OldDisk->Type );

	if( !pDbForPages )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get db for pages" );

	pDbForPageTableHash = pDbForPages;

	if( NewDisk )
	{
/*
**	In case NewDisk->Type != OldDisk->Type, and updating the obj will move it to a different db
*/
		SDB_DB	*pDbForPagesNew = SecDbDbUpdateGet( pDb, NewDisk->Type );


		if( pDbForPagesNew != pDbForPages )	// Disallow this for the moment
			return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Update would move %s from %s to %s (not supported", OldDisk->Name, pDbForPages->FullDbName.c_str(), pDbForPagesNew->FullDbName.c_str() );
	}

	if( SDB_DB_SUBDB == pDbForPages->DbOperator )
	{
/*
**	Walk up out of subdatabases to find a physical place for
**	the pages.  We could have subdbs and other virtual types nested arbitrarily.
*/
		while( pDbForPages->DbOperator != SDB_DB_LEAF )
		{
			while( pDbForPages->DbOperator == SDB_DB_SUBDB )
				pDbForPages = pDbForPages->Left;
			
			if( !( pDbForPages = SecDbDbUpdateGet( pDbForPages, NewDisk ? NewDisk->Type : OldDisk->Type ) ) )
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get db for pages" );
		}
	}

	if( NewDisk )
		NewDisk->DbIDUpdated = pDbForPages->DbID;

/*
**	Get old page table
*/

	Entry = HashLookupPtr( pDbForPageTableHash->PageTableHash, OldDisk->Name );
	if( Entry )
	{
		PageTable 	= (char *) HashValue( Entry );
		Name 		= (char *) HashKey( Entry );
		HashDeleteByBucket( pDbForPageTableHash->PageTableHash, Name, HashBucket( Entry ));
		free( Name );

		// Mark old disk as being huge since its pagetable is now gone from the hash
		SDB_SET_HUGE( OldDisk->VersionInfo, TRUE );
	}
	else if( SDB_IS_HUGE( OldDisk->VersionInfo ))
	{
		if( !SecDbTransProtectedGet( pDb, OldDisk->Name, &DiskObject, TRUE ))
			return FALSE;

		// Check since last TransProtectedGet
		if(		OldDisk->DateCreated != DiskObject.SdbDisk.DateCreated ||
				OldDisk->TimeUpdated != DiskObject.SdbDisk.TimeUpdated ||
				OldDisk->DbIDUpdated != DiskObject.SdbDisk.DbIDUpdated ||
				OldDisk->UpdateCount != DiskObject.SdbDisk.UpdateCount )
		{
			free( DiskObject.Mem );
			return Err( SDB_ERR_OBJECT_MODIFIED, "%s: @", OldDisk->Name );
		}

		// And just make sure it is really a huge page table
		if( !SDB_IS_HUGE( DiskObject.SdbDisk.VersionInfo ))
		{
			free( DiskObject.Mem );
			return Err( SDB_ERR_OBJECT_MODIFIED, "Expected %s to be huge-type object", OldDisk->Name );
		}
		PageTable = (char *) DiskObject.Mem;
	}
	else
		PageTable = NULL;


/*
**	If too big to fit in single object, make it a huge object;
**	or if it is no longer huge, delete all the old pages.
*/

	Update	= NewDisk && pMemSize;
	Rename	= NewDisk && !pMemSize;
	WasHuge	= PageTable != NULL;
	TooBig	= pMemSize && *pMemSize > pDbForPageTableHash->MaxMemSize;
	
	if(( Update && TooBig ) || ( Rename && WasHuge ))
	{
		Status = PageTableUpdate( pDb, pDbForPages, NewDisk->Name, PageTable, NewDisk->UpdateCount, pMemSize, pMem );
		if( Status )
			SDB_SET_HUGE( NewDisk->VersionInfo, TRUE );
	}
	else if( WasHuge )
	{
		Status = PageTableDestroy( pDb, pDbForPages, PageTable );
		if( Status && NewDisk )
			SDB_SET_HUGE( NewDisk->VersionInfo, FALSE );
	}
	else
		Status = TRUE;


	free( PageTable );
	return( Status );
}



/****************************************************************
**	Routine: SecDbHugeUnload
**	Returns: Nothing
**	Action : Frees page table if one exists
****************************************************************/

void SecDbHugeUnload(
	SDB_DB		*pDb,
	const char	*SecName
)
{
	HASH_ENTRY_PTR
			Entry;

	char	*Name;


	if( !pDb )
		return;

	// ClassObjectsDb has no PageTableHash
	if( !pDb->PageTableHash || !( Entry = HashLookupPtr( pDb->PageTableHash, SecName )))
		return;

	Name = (char *) HashKey( Entry );
	free( (char *) HashValue( Entry ));
	HashDeleteByBucket( pDb->PageTableHash, SecName, HashBucket( Entry ));
	free( Name );
}



/****************************************************************
**	Routine: SecDbHugeDelete
**	Returns: TRUE/FALSE
**	Action : Deletes huge pages for given object from database
****************************************************************/

int SecDbHugeDelete(
	SDB_DB		*pDb,
	const char	*SecName
)
{
	SDB_DISK
			OldSdbDisk;


	if( SecDbTransProtectedDiskInfo( pDb, SecName, &OldSdbDisk ))
	{
		// Add transaction parts to delete the original pages
		// (rely on driver version-checking on pages even though top object may be IgnoreErrors;
		// this insures that page layout does not change between now and the commit).
		return( SecDbHugeUpdate( pDb, &OldSdbDisk, NULL, NULL, NULL ) || ErrNum == SDB_ERR_OBJECT_NOT_FOUND );
	}
	return TRUE;
}
