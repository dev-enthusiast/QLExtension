#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_drv.c,v 1.21 2000/12/19 17:20:43 simpsk Exp $"
/****************************************************************
**
**	SDB_DRV.C	SecDb Driver functions
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_drv.c,v $
**	Revision 1.21  2000/12/19 17:20:43  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**
**	Revision 1.20  2000/06/08 07:29:41  goodfj
**	Restore errno.h for solaris
**	
**	Revision 1.19  2000/06/07 14:05:28  goodfj
**	Fix includes
**	
**	Revision 1.18  1999/11/01 21:58:58  singhki
**	oops, no BUILDING_SECDB in executables which link with secdb
**	
**	Revision 1.17  1999/09/01 15:29:28  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.16  1998/11/16 22:59:56  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.15.4.1  1998/11/05 01:01:22  singhki
**	TempRev: hacks to support OS/2 IBM compiler in C++ mode
**	
****************************************************************/

#include	<portable.h>
#include	<dynalink.h>
#include	<mempool.h>
#include	<errno.h>
#include	<hash.h>
#include	<secdb.h>
#include	<err.h>
#include	<secerror.h>
#include	<secdrive.h>



/*
**	Structure used for storing 'server' function for objects
*/

struct SDB_CLASS_SERVER_INFO
{
	SDB_SEC_TYPE
			Type;							// Security class type

	char	DllPath[	SDB_PATH_NAME_SIZE ],
			FuncName[	SDB_FUNC_NAME_SIZE ],
			Name[	 	SDB_CLASS_NAME_SIZE ],
			Argument[	SDB_CLASS_ARG_SIZE ];

	SDB_INCREMENTAL_FUNC
			*pfIncremental;

};


/*
**	Variables
*/

static HASH
		*SecDbClassServerHash;

static int
		DriverStarted	= FALSE;


/****************************************************************
**	Routine: SecDbDriverStartup
**	Returns: TRUE/FALSE
**	Action : Startup/allocate and define resources used by database
**			 drivers
****************************************************************/

int SecDbDriverStartup(
 	const char	*Path,
	const char	*TableName
)
{
	SDB_CLASS_SERVER_INFO
			*ClassServerPtr	= NULL;

	HASH_ENTRY_PTR
			Ptr;

	MEM_POOL
			*MemPool;

	FILE	*fpTable;

	char	Buffer[ 256 ],
			FileName[ FILENAME_MAX + 1 ],
			*Str;


/*
**	Don't start twice
*/

	if( DriverStarted )
		return TRUE;


/*
**	Open the ASCII file containing the table
*/

	if( !KASearchPath( Path, TableName, FileName, sizeof( FileName )))
		return FALSE;

	if( !(fpTable = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, "SecDbDriverStartup( %s ) %s", FileName, strerror( errno ) );

	MemPool = MemPoolCreate();
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	SecDbClassServerHash = HashCreate( "Class Server Functions", (HASH_FUNC) HashIntHash, NULL, 0, MemPool );


/*
**	Read and parse the file
*/

	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		if( Buffer[ 0 ] == '/' && Buffer[ 1 ] == '/' )
			continue;

		ClassServerPtr = (SDB_CLASS_SERVER_INFO *) MemPoolCalloc( MemPool, 1, sizeof( SDB_CLASS_SERVER_INFO ));

		// Security type number
		if( !(Str = strtok( Buffer, " \t\n" )))
			break;
		ClassServerPtr->Type = (SDB_SEC_TYPE) atoi( Str );

		// Dll path
		if( !(Str = strtok( NULL, " \t\n" )))
			break;
		strcpy( ClassServerPtr->DllPath, Str );

		// Function name
		if( !(Str = strtok( NULL, " \t\n\"" )))
			break;
		strcpy( ClassServerPtr->FuncName, Str );

		// Name (quoted region)
		if( !(Str = strtok( NULL, "\t\n\"" )))
			break;
		strcpy( ClassServerPtr->Name, Str );

		// Optional arg (quoted region)
		if( (Str = strtok( NULL, "\t\n\"" )))
			strcpy( ClassServerPtr->Argument, Str );

		// Add the class
		if( !HashInsert( SecDbClassServerHash, (HASH_KEY) (long) ClassServerPtr->Type, ClassServerPtr ))
			MemPoolFree( MemPool, ClassServerPtr );

		ClassServerPtr = NULL;
	}

	if( ClassServerPtr )
		MemPoolFree( MemPool, ClassServerPtr );


/*
**	Close the file
*/

	fclose( fpTable );


/*
**	Dynalink in all of the functions in the table
*/

	HASH_FOR( Ptr, SecDbClassServerHash )
	{
		ClassServerPtr = (SDB_CLASS_SERVER_INFO *) HashValue( Ptr );

		ClassServerPtr->pfIncremental = (SDB_INCREMENTAL_FUNC *) DynaLink( ClassServerPtr->DllPath, ClassServerPtr->FuncName );
		if( !ClassServerPtr->pfIncremental )
		{
			ErrMore( "SecDbDriverStartup: pfIncremental( %s )", ClassServerPtr->Name );
			SecDbDriverShutdown();
			return FALSE;
		}
	}

	DriverStarted = TRUE;
	return TRUE;
}



/****************************************************************
**	Routine: SecDbDriverShutdown
**	Returns: None
**	Action : Shutdown and free any resources allocated by Startup
****************************************************************/

void SecDbDriverShutdown( void )
{
	// Free the incremental function hash and its contents
	if( SecDbClassServerHash )
	{
		MemPoolDestroy( SecDbClassServerHash->MemPool );
		SecDbClassServerHash = NULL;
	}

	DriverStarted = FALSE;
}



/****************************************************************
**	Routine: SecDbDriverIncremental
**	Returns: TRUE or FALSE
**	Action : Finds and calls the class's incremental function
****************************************************************/

int SecDbDriverIncremental(
	SDB_DRIVER_INCREMENTAL	*Incremental,
	SDB_DISK				*CurSdbDisk,
	void					*CurMem,
	unsigned int			CurMemSize,
	SDB_DISK				*MsgSdbDisk,
	void					*MsgMem,
	unsigned int 			MsgMemSize,
	int						Direction
)
{
	SDB_CLASS_SERVER_INFO
			*ClassServerPtr;


/*
**	Get incremental function from class
*/

	ClassServerPtr = (SDB_CLASS_SERVER_INFO *) HashLookup( SecDbClassServerHash, (HASH_KEY) (long) MsgSdbDisk->Type );
	if( !ClassServerPtr )
		return Err( ERR_UNSUPPORTED_OPERATION, "@: No incremental function registered for '%d' class",
				CurSdbDisk ? CurSdbDisk->Type : -1 );


/*
**	Fill in what is known of Incremental
*/

	memset( Incremental, 0, sizeof( *Incremental ));
	Incremental->Insert = CurMemSize == 0U || !CurSdbDisk;

	if( !Incremental->Insert && ( MsgSdbDisk->TimeUpdated < CurSdbDisk->TimeUpdated || ( MsgSdbDisk->TimeUpdated == CurSdbDisk->TimeUpdated && MsgSdbDisk->DbIDUpdated < CurSdbDisk->DbIDUpdated )))
		Incremental->StoredObject.SdbDisk = *CurSdbDisk;	// Current wins except for update count
	else
		Incremental->StoredObject.SdbDisk = *MsgSdbDisk;	// New one wins except for update count

	if( Incremental->Insert )
	{
		Incremental->StoredObject.SdbDisk.UpdateCount	= 0L;
		Incremental->StoredObject.SdbDisk.DbIDUpdated	= Incremental->StoredObject.SdbDisk.DbIDUpdated;
	}
	else
		Incremental->StoredObject.SdbDisk.UpdateCount	= CurSdbDisk->UpdateCount + 1;


/*
**	Check that an object of the same name but different class has not been
**	created.
*/

	if( !Incremental->Insert && CurSdbDisk->Type != MsgSdbDisk->Type )
		return( Err( SDB_ERR_OBJECT_ALREADY_EXISTS, "@: %s", CurSdbDisk->Name ));


/*
**	Mempool
*/

	if( !( Incremental->MemPool = MemPoolCreate( )))
		return( Err( ERR_MEMORY, "@" ));


/*
**	Call the increment function
*/

	return( (*ClassServerPtr->pfIncremental)( Incremental, CurMem, CurMemSize, MsgMem, MsgMemSize, Direction ));
}



/****************************************************************
**	Routine: SecDbDriverIndexIncremental
**	Returns: TRUE or FALSE
**	Action : Fills in the value table for the given index
****************************************************************/

int SecDbDriverIndexIncremental(
	SDB_DRIVER_INCREMENTAL	*Incremental,
	SDB_INDEX_DESCRIPTOR	*IndexDesc,
	DT_VALUE				*Values
)
{
	int		PartNum;

	DT_VALUE_RESULTS
			*Value;


	if( !IndexDesc )
		return( FALSE );

	if( !Incremental->Values )
	{
		Err( ERR_UNSUPPORTED_OPERATION, "@: No values returned from increment function" );
		return( ErrMore( "Incremental update of '%s' for index '%s'", Incremental->StoredObject.SdbDisk.Name, IndexDesc->Name ));
	}

	for( PartNum = 0; PartNum < IndexDesc->PartCount; PartNum++ )
	{
		Value = (DT_VALUE_RESULTS *) HashLookup( Incremental->Values, (HASH_KEY) IndexDesc->Parts[ PartNum ].ValueTypeName );
		if( Value )
			Values[ PartNum ].Data = *Value;
		else
		{
			Err( ERR_UNSUPPORTED_OPERATION, "@: %s", IndexDesc->Parts[ PartNum ].ValueTypeName );
			return( ErrMore( "Incremental update of '%s' for index '%s'", Incremental->StoredObject.SdbDisk.Name, IndexDesc->Name ));
		}
	}
	return( TRUE );
}



/****************************************************************
**	Routine: SecDbDriverIncrementalFree
**	Returns: Nothing
**	Action : Frees the values hash table
****************************************************************/

void SecDbDriverIncrementalFree(
	SDB_DRIVER_INCREMENTAL	*Incremental
)
{
	if( Incremental->MemPool )
	{
		MemPoolDestroy( Incremental->MemPool );
		Incremental->Values = NULL;
		Incremental->MemPool = NULL;
	}
}



/****************************************************************
**	Routine: SecDbDriverAddValue
**	Returns: TRUE or FALSE
**	Action : For use by class's incremental function to pass
**			 back values for subsequent incremental index trans
**			 parts
****************************************************************/

int SecDbDriverAddValue(
	SDB_DRIVER_INCREMENTAL	*Incremental,
	char					*ValueTypeName,
	DT_VALUE_RESULTS		*ValueResults
)
{
	char	*Key;

	DT_VALUE_RESULTS
			*Value;


	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	if( !Incremental->Values )
		if( !( Incremental->Values = HashCreate( "SecDbDriverIncremental", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, Incremental->MemPool )))
			return( FALSE );

	if(
			!( Key = MemPoolStrDup( Incremental->MemPool, ValueTypeName )) ||
			!( Value = (DT_VALUE_RESULTS *) MemPoolMalloc( Incremental->MemPool, sizeof( *Value ))))
		return( Err( ERR_MEMORY, "@" ));

	*Value = *ValueResults;
	HashInsert( Incremental->Values, (HASH_KEY) Key, (HASH_VALUE) Value );
	return( TRUE );
}



/****************************************************************
**	Routine: SecDbDriverResolveInsert
**	Returns:
**		SDB_RESOLVE_FAIL		- Cannot resolve the versions -- leave Current
**		SDB_RESOLVE_IGNORE		- Versions compatible -- leave Current
**		SDB_RESOLVE_UPDATE		- Versions compatible -- put in Proposed
**	Action : Memcmp of memory, earliest time wins
****************************************************************/

SDB_RESOLVE_ACTION SecDbDriverResolveInsert(
	SDB_STORED_OBJECT		*Proposed,
	SDB_STORED_OBJECT		*Current
)
{
	if( Proposed->MemSize != Current->MemSize )
		return( SDB_RESOLVE_FAIL );

	if( 0 != memcmp( Proposed->Mem, Current->Mem, Current->MemSize ))
		return( SDB_RESOLVE_FAIL );

	if( Proposed->SdbDisk.DateCreated < Current->SdbDisk.DateCreated )
		return( SDB_RESOLVE_UPDATE );
	else
		return( SDB_RESOLVE_IGNORE );
}

