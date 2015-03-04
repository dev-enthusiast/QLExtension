#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_api.c,v 1.275 2015/02/27 22:49:24 khodod Exp $"
/****************************************************************
**
**	SDB_API.C	- Security database API
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_api.c,v $
**	Revision 1.275  2015/02/27 22:49:24  khodod
**	Add checks for admin rights, if we are running database administration
**	utilities.
**	AWR: #364118
**
**	Revision 1.274  2015/02/19 21:30:25  c038571
**	AWR: #363400. Make sure the OpenCount is positive before calling server.
**
**	Revision 1.273  2013/10/09 15:35:04  khodod
**	Make sure we have a valid sdb_async_results object before going crazy in
**	the SecDbAbortErrorHook.
**	AWR: #315933
**
**	Revision 1.272  2013/10/04 16:50:58  khodod
**	SecDbRemoveFromDeadPool may be called with a NULL SecPtr.
**	AWR: #315292
**
**	Revision 1.271  2013/10/03 00:48:30  khodod
**	Adding validation to the SecDb API to prevent buffer overflows and crashes
**	for evaluating strings with embedded percent signs.
**	AWR: #315292
**
**	Revision 1.270  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.269  2004/08/12 20:53:49  khodod
**	Added SecDbAuthenticate().
**	iBug #12504
**
**	
****************************************************************/

#define BUILDING_SECDB
#define		INCL_TIMEZONE
#include	<portable.h>
#include	<hash.h>
#include	<heap.h>
#include	<date.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<sectrans.h>
#include	<secdb/async_result.h>
#include	<secdrv.h>
#include	<sdbdbset.h>
#include	<sdb_int.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<sectrace.h>
#include	<secver.h>
#include	<err.h>
#include	<secexpr.h>

#include	<algorithm>
CC_USING( std::for_each );

#include    <string>
CC_USING( std::string );

#include	<stdio.h>
#include	<stdlib.h>
#include	<stddef.h>
#include	<string.h>
#include	<math.h>
#include	<time.h>


/*
**	Hash table initialization values
*/

#define SDB_DATABASE_HASH_SIZE	200


/*
**	Define values for SELECT_TOP, SELECT_BOTTOM for SecDbNameLookupManySecTypes()
**  and SecDbNameLookupSecTypes()
*/

#define SELECT_LOWEST 		 1
#define SELECT_GREATEST 	-1



/*
**	New security prefix string
*/

const char
		SecDbNewSecurityPrefix[] = "_New_";


/*
**	Variables
*/

SDB_DB	*SecDbRootDb = NULL;						// Current root database

int		DiddleCount		= 0,						// Number of active diddles
		TotalRefCount	= 0,						// Total number of references to secs
		SecDbConfigAttributes	= 0;				// SDB_DB_ATTRIB_*, set from secdb.cfg

int		SecDbInitialized = FALSE;					// First time initialization flag

long	SecDbCurrentDate;							// Currency date at startup time


void	*SecDbDatabaseBuffer;						// General purpose database buffer

const char	
		*SecDbUniqueIDFileName;						// Filename for unique identifiers

char	SecDbPath[ _MAX_PATH ],		  				// Path of SecDb data files
		SecDbApplicationName[ SDB_APP_NAME_SIZE ],	// Current application name
		SecDbUserName[ SDB_USER_NAME_SIZE ],		// Current user name
		SecDbTypesPath[ _MAX_PATH ];   				// Path of SecDb types files

HASH	*SecDbValueTypeHash,						// Hash table of value type codes
		*SecDbIndexHash,							// Hash table of indices
		*SecDbSecTypeHash,							// Hash table of classes by type
		*SecDbSecDescHash,							// Hash table of classes by name
		*SecDbDatabaseHash,							// Hash table of databases by name
		*SecDbDatabaseIDHash;						// Hash table of databases by DbID

SDB_VALUE_TYPE										// Pre-defined value types:
		SecDbValueClassValueFunc,					// 	 Class Value Func
		SecDbValueCurrentDate,						//   Current date
		SecDbValueDateCreated,		 				//   Local date of creator
		SecDbValueDbUpdated,		 				//   Name of database last updating object
		SecDbValueDiddleScope,						//	 Value func used to create a new diddle scope
		SecDbValueLastTransaction,					//   Transaction which wrote this version of the object
		SecDbValueObjectValueFunc,					// 	 Object Value Func
		SecDbValuePricingDate,						//   Pricing date
		SecDbValueSecurityData,						//   Security Data
		SecDbValueSecurityDesc,						//   Description
		SecDbValueSecurityName,		 				//   Name of the security
		SecDbValueSecurityType,		 				//   Type of the security
		SecDbValueTimeModified,		 				//   Time security was modified
		SecDbValueUpdateCount,						//   Update count of object
		SecDbValueVersion,							//   Version number of object
		SecDbValueValueFunc,						//	 Value Func of a Node
		SecDbValueValueTypeArgs;					//	 Value Type Args of a Node

// FIX - deprecated
SDB_VALUE_TYPE										// Pre-defined value types:
		SecDbValueTimeCreated,
		SecDbValueDbCreated;

DT_DATA_TYPE										// Pre-defined data types:
		DtSecurity,									//   Security
		DtSecurityList,								//	 Security List
		DtSlang,		  							//   Slang tree
		DtPointer,									//   Pointer
		DtDatabase,									//   Database handle
		DtDiddleScope,								//	 Diddle Scope handle
		DtDiddleScopeList,							//	 Diddle Scope List
		DtSecDbNode;								//	 SecDb Node

int		SecDbUseDbPath = -1;						// if Get/New/NameLookup & Delete should use fully qualified SecNames or not

static unsigned
		TrackLoadHistory = 0,                       // True if we should track SecDb object gets
		AlwaysRefresh = 0;							// True if we should always refresh a security object

static ERR_ABORT_HOOK_FUNC
		OldAbortHook = NULL;						// Old abort hook func

static DT_VALUE
		*GetByNameHistory = NULL;

#ifdef WIN32
extern bool 
    win_auth( const char *user_name, string &SecDbId, bool &IsDeveloper, bool &IsAdmin );
#endif

/*
**  static functions
*/

static int 
		SecNameEntriesMerge( SDB_SEC_NAME_ENTRY *SecNamesA, int SizeA, SDB_SEC_NAME_ENTRY *SecNamesB, int SizeB, SDB_SEC_NAME_ENTRY *Result, int ResultSize, int SelectType ),
		MapGetTypeToSelectType( int GetType );



/****************************************************************
**	Routine: InitializeNoEvalVTs
**	Returns: void
**	Action : Initializes VTs which want to be no eval
****************************************************************/

static void InitializeNoEvalVTs(
)
{
	char *VTs = strdup( SecDbConfigurationGet( "SECDB_NO_EVAL_VTS", NULL, NULL, "" ));

	for( char *VT = strtok( VTs, "," ); VT; VT = strtok( NULL, "," ))
	{
		while( *VT == ' ' ) ++VT;
		if( !*VT )
			break;

		SDB_VALUE_TYPE
				ValueType = SecDbValueTypeFromName( VT, NULL );

		ValueType->NoEval = TRUE;
	}
	free( VTs );
}


void add_cancel( long id )
{
	Sdb_Async_Failed_Request* res = new Sdb_Async_Failed_Request( id );
	res->error = "Cancelled by user";

	sdb_async_results->add_result( res );
}

/****************************************************************
**	Routine: SecDbAbortHook
**	Returns: void
**	Action:  Abort a get value if active
****************************************************************/

static void SecDbAbortHook( void )
{
    //
    // We may be called from the windowing subsystem before
    // the node API is initialized, which is how we initialize
    // the sdb_async_results data structures.
    //
    if( SecDbInitialized )
    {
        Sdb_Async_Results::long_vec_t ids;
        sdb_async_results->outstanding_request_ids( ids );
        for_each( ids.begin(), ids.end(), add_cancel );
    }
	if( SlangContext )
		SlangContext->Aborted = TRUE;

	if( OldAbortHook )
		OldAbortHook();
}


/****************************************************************
**	Routine: IsAllSpaces
**	Returns: TRUE if string is composed entirely of whitespce, else FALSE
**	Action : 
****************************************************************/

static int IsAllSpaces(
	const char *String
)
{
   	for( const char *p = String; *p; ++p )
   		if( !isspace( *p ) )
   			return FALSE;

	return TRUE;
}

/****************************************************************
**	Routine: SecDbAuthenticate
**	Returns: TRUE	-	The current user is successfully 
**			 			authenticated and authorized
**						to use the current SecDb application
**			 FALSE	-	Error 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbAuthenticate"

static bool SecDbAuthenticate( void )
{
	char SysUserName[128];
	LoginName( SysUserName, sizeof( SysUserName ) - 1 );
   
	string LegacyID;
	bool IsAdmin = true;
	bool IsDeveloper = true; 
	bool ADAccessEnabled = false;
	
#ifdef WIN32
	
	ADAccessEnabled = true;
	if( !win_auth( SysUserName, LegacyID, IsDeveloper, IsAdmin ) )
		return false;

#endif
    
	char *UserName = 0;
	if( !ADAccessEnabled || IsDeveloper ) 
	{
		UserName = getenv( "USER_NAME" );
		if( !UserName )
			UserName = getenv( "USERNAME" );
	} 
	else 
	{
		strncpy( SysUserName, LegacyID.c_str(), sizeof( SysUserName ) - 1 );
		SysUserName[ sizeof( SysUserName ) - 1 ] = 0;
		UserName = SysUserName;
	}

	if( !UserName )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Cannot determine the SecDb User Name" );

	if( IsAllSpaces( UserName ) )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": UserName is blank" );
	if( 0 == stricmp( "USERNAME", UserName ) || 0 == stricmp( "nobody", UserName ) )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": UserName '%s' is invalid", UserName );
	
	strncpy( SecDbUserName, UserName, SDB_USER_NAME_SIZE - 1 );
	SecDbUserName[ SDB_USER_NAME_SIZE - 1 ] = '\0';

	if( !IsDeveloper ) 
	{
        const char *AuthorizedApps = SecDbConfigurationGet( "SECDB_AUTHORIZED_APPS", 0, 0, 0 );
        if( !AuthorizedApps )
            return Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME ": %s\n", ErrGetString() );
        if( !StrIStr( const_cast<char *>( AuthorizedApps ), SecDbApplicationName ) )
			return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME 
				": %s is not authorized to run %s",
				SecDbUserName, SecDbApplicationName );
	}

	if( 0 == strcmp( SecDbApplicationName, "SecCon" ) || 0 == strcmp( SecDbApplicationName, "SecConB" ) ) 
	{
		if( !IsAdmin )
		{
			return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME 
				": %s is not authorized to run %s",
				SecDbUserName, SecDbApplicationName );
		}
	}
	return true;
}

/****************************************************************
**	Routine: SecDbInitialize
**	Returns: TRUE	- Security database initialized without error
**			 FALSE	- Error initializing security database
**	Summary: Initialize the security database system
**	Action : First call initializes security database
**			 Subsequent calls changes initialization data
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbInitialize"

int SecDbInitialize(
	SDB_MSG_FUNC	MsgFunc,			// Message handling function
	SDB_MSG_FUNC	ErrMsgFunc,			// Error message handling function
	const char		*ApplicationName,	// Name of application
	const char		*UserName	        // Name of user (DEPRECATED)
)
{
	static int
			SecDbAbortHookInitialized = FALSE;

	SDB_DB	*pDb;

	TIMEZONE_SETUP();

	if( !SecDbAbortHookInitialized )
	{
		OldAbortHook = ErrAbortHookFunc;
		ErrAbortHookFunc = SecDbAbortHook;
		SecDbTraceAbortFunc = SecDbTraceAbortGet;
		SecDbTraceAbortInit = FALSE;
		SecDbAbortHookInitialized = TRUE;
	}

	// Allocate database memory buffer
	if( !SecDbInitialized && !(SecDbDatabaseBuffer = malloc( SDB_DB_DATABASE_BUF_SIZE )))
		return Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": SecDbDatabaseBuffer, @" );

	// Get the date	- also set in DbObjFuncSecDb/SDB_MSG_LOAD
	SecDbCurrentDate = DateToday();

	// Set the message functions
	if( MsgFunc )
		SecDbMessageFunc 		= MsgFunc;
	if( ErrMsgFunc )
		SecDbErrorMessageFunc	= ErrMsgFunc;

	// Set the application and user names
	if( ApplicationName )
	{
		if( IsAllSpaces( ApplicationName ) )
			return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": ApplicationName is blank" );

		strncpy( SecDbApplicationName, 	ApplicationName, 	SDB_APP_NAME_SIZE - 1	);
		SecDbApplicationName[ SDB_APP_NAME_SIZE	- 1 ] = '\0';
	}


    if( !SecDbAuthenticate( ) )
        return ErrMore( ARGCHECK_FUNCTION_NAME ": Authentication failure" );

	if( SecDbInitialized )
	{
		HASH_ENTRY_PTR
				HashPtr;

		HASH_FOR( HashPtr, SecDbDatabaseIDHash )
		{
			pDb = (SDB_DB *) HashValue( HashPtr );
			// AWR: #363400
			if( pDb->OpenCount > 0 && pDb->pfChangeInitData )
				(*pDb->pfChangeInitData)( pDb );
		}
	}
	else
	{

		// Register error message function with Err API
		SecDbErrStartup( );

		// Register data types
		if( !DtInitialize( SecDbTypesPath, "datatype.dat" ))
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to initialize datatypes" );

		DtSecurity		= DtRegister( "Security",		"",	"",	DtFuncSecurity,		49, NULL, NULL );
		DtSlang			= DtRegister( "Slang",			"", "", DtFuncSlang,		50, NULL, NULL );
		DtPointer		= DtRegister( "Pointer",		"", "", DtFuncPointer,		51, NULL, NULL );
		DtDatabase		= DtRegister( "Database",		"",	"",	DtFuncDatabase,		52, NULL, NULL );
		DtDiddleScope	= DtRegister( "DiddleScope",	"",	"",	DtFuncDiddleScope,	93,	NULL, NULL );
		DtDiddleScopeList=DtRegister( "DiddleScopeList","", "", DtFuncDiddleScopeList,94,NULL,NULL );
		DtSecurityList	= DtRegister( "Security List",	"",	"",	DtFuncSecurityList,	58, NULL, NULL );
		DtSecDbNode     = DtFromName( "SecDb Node" );

		// Initialize node api
		if( !NodeInitialize( ))
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to initialize node api" );


		// Create security-value-types hash table, and get initial value types
		// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
		SecDbValueTypeHash 			= HashCreate( "Sdb Value Types", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 5000, NULL );
		SecDbValueClassValueFunc	= SecDbValueTypeFromName( "Class Value Func",		DtValueTypeInfo );
		SecDbValueCurrentDate		= SecDbValueTypeFromName( "Current Date",			DtDate		);
		SecDbValueDateCreated		= SecDbValueTypeFromName( "Date Created",			DtDate		);
		SecDbValueDbUpdated			= SecDbValueTypeFromName( "Database Updated",		DtString	);
		SecDbValueDiddleScope		= SecDbValueTypeFromName( "Diddle Scope",			DtDiddleScope);
		SecDbValueLastTransaction	= SecDbValueTypeFromName( "Last Transaction",		DtDouble	);
		SecDbValueObjectValueFunc	= SecDbValueTypeFromName( "Object Value Func",		DtValueTypeInfo );
		SecDbValuePricingDate		= SecDbValueTypeFromName( "Pricing Date",			DtDate		);
		SecDbValueSecurityData		= SecDbValueTypeFromName( "Security Data",			DtBinary	);
		SecDbValueSecurityDesc		= SecDbValueTypeFromName( "Security Description",	DtString	);
		SecDbValueSecurityName		= SecDbValueTypeFromName( "Security Name",			DtString	);
		SecDbValueSecurityType		= SecDbValueTypeFromName( "Security Type",			DtString	);
		SecDbValueTimeModified		= SecDbValueTypeFromName( "Time Modified",			DtTime		);
		SecDbValueUpdateCount		= SecDbValueTypeFromName( "Update Count",			DtDouble	);
		SecDbValueVersion			= SecDbValueTypeFromName( "Version",				DtStructure	);
		SecDbValueValueFunc			= SecDbValueTypeFromName( "Value Func",				DtValueTypeInfo);
		SecDbValueValueTypeArgs		= SecDbValueTypeFromName( "Value Type Args",		DtEach );

		// FIX - deprecated
		SecDbValueTimeCreated		= SecDbValueTypeFromName( "Time Created",			DtTime		);
		SecDbValueDbCreated			= SecDbValueTypeFromName( "Database Created",		DtString	);

		// Create the hash table used for databases & database indices
		// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
		SecDbDatabaseHash	= HashCreate( "Sdb Databases", (HASH_FUNC) HashStrHash,(HASH_CMP) HashStricmp, SDB_DATABASE_HASH_SIZE, NULL );
		SecDbDatabaseIDHash	= HashCreate( "Sdb Database IDs", NULL, NULL, SDB_DATABASE_HASH_SIZE, NULL );
		SecDbIndexHash		= HashCreate( "Sdb Indices", (HASH_FUNC) SecDbIndexHashFunction, (HASH_CMP) SecDbIndexCompareFunction, 0, NULL );

		SecDbDbSetHash		= HashCreate( "Db Set Hash", SDB_DBSET::HashFunc, SDB_DBSET::Compare, 0, NULL );
		SecDbDbSetOpsCache	= HashCreate( "Db Set Ops Hash", SDB_DBSET_OPERATION::HashFunc, SDB_DBSET_OPERATION::Compare, 0, NULL );

		SDB_DBSET_BEST::LookupHelper = new SDB_DBSET_BEST( NULL, NULL );

		// Initialize (start) the default object function
		SecDbDefaultObjFunc( NULL, SDB_MSG_START, NULL );

		InitializeNoEvalVTs();
		AlwaysRefresh = stricmp( SecDbConfigurationGet( "SECDB_ALWAYS_REFRESH", NULL, NULL, "False" ), "False" ) == 0 ? 0 : SDB_ALWAYS_REFRESH;
		TrackLoadHistory = stricmp( SecDbConfigurationGet( "SECDB_TRACK_LOADS", NULL, NULL, "False" ), "False" );

		// Set initialization state
		SecDbInitialized = TRUE;
	}

	return TRUE;
}




/****************************************************************
**	Routine: SecDbClassObjectInsert
**	Returns: void
**	Action : Keep track of an object as a member of its class. 
****************************************************************/

void SecDbClassObjectInsert(
	SDB_OBJECT *SecPtr
)
{
	SecPtr->Class->ObjectCount++;

	SecPtr->ClassNext = SecPtr->Class->FirstSec;
	SecPtr->Class->FirstSec = SecPtr;

	if( SecPtr->ClassNext )
		SecPtr->ClassNext->ClassPrev = SecPtr;
	else
		SecPtr->Class->LastSec = SecPtr;

	SecPtr->ClassPrev = NULL;
}



/****************************************************************
**	Routine: SecDbClassObjectRemove
**	Returns: 
**	Action : 
****************************************************************/

void SecDbClassObjectRemove(
	SDB_OBJECT *SecPtr
)
{
	SecPtr->Class->ObjectCount--;

	if( SecPtr->ClassNext )
		SecPtr->ClassNext->ClassPrev = SecPtr->ClassPrev;
	else
		SecPtr->Class->LastSec = SecPtr->ClassPrev;

	if( SecPtr->ClassPrev )
		SecPtr->ClassPrev->ClassNext = SecPtr->ClassNext;
	else
		SecPtr->Class->FirstSec = SecPtr->ClassNext;
		
	SecPtr->ClassPrev = SecPtr->ClassNext = NULL;
}



/****************************************************************
**	Routine: SecDbObjectIsModified
**	Returns: True if object or a child instream object was
**           modified.
**	Summary: We only bother to check if any security in the 
**           instream graph was modified... this is adequate
**           for contained securities.  It might be worth 
**           also doing the full stored object update check,
**           although this would be a lot slower.
****************************************************************/

static int SecDbObjectIsModified(
	SDB_OBJECT *SecPtr
)
{
	static HASH
			*SecRefHash=NULL;

	SDB_VALUE_TYPE_INFO
			*ValueTypeInfo;

	SDB_ENUM_PTR
			EnumPtr;

	DT_VALUE
			*ValueResults;

	int		RetValue = FALSE,
			CreateHash = SecRefHash==NULL;

	if ( SecPtr == NULL )
		return FALSE;

	if ( SecPtr->Modified )
		return TRUE;

	if ( CreateHash )
	{
		SecRefHash = HashCreate( "SecRefDupHash", NULL, NULL, 0, NULL );
		HashInsert(	SecRefHash, (HASH_KEY) SecPtr, (HASH_VALUE) SecPtr );
	}

	for( ValueTypeInfo = SecDbValueTypeEnumFirst( SecPtr, &EnumPtr ); !RetValue && ValueTypeInfo; ValueTypeInfo = SecDbValueTypeEnumNext( EnumPtr ))
		if( ValueTypeInfo->ValueFlags & SDB_IN_STREAM )
		{
			if ( *ValueTypeInfo->DataType == DtSecurity )
			{
				ValueResults = SecDbGetValue( SecPtr, ValueTypeInfo->ValueType );
				if ( ValueResults != NULL )
				{
					SDB_OBJECT 
							*SubObj = (SDB_OBJECT*) ValueResults->Data.Pointer;
					if ( SecDbObjectIsModified( SubObj ))
						RetValue = TRUE;
				}
			}
			else if ( *ValueTypeInfo->DataType == DtSecurityList )
			{
				ValueResults = SecDbGetValue( SecPtr, ValueTypeInfo->ValueType );
				if ( ValueResults != NULL )
				{
					int     I,
							Sz = DtSize( ValueResults );
					for( I=0; I<Sz; I++ )
					{
						SDB_OBJECT 
								*SubObj = (SDB_OBJECT*) DtSubscriptGetPointer( ValueResults, I, DtSecurity, NULL );
						if ( SecDbObjectIsModified( SubObj ))
						{
							RetValue = TRUE;
							break;
						}
					}
				}
			}
		}

	SecDbValueTypeEnumClose( EnumPtr );

	if ( CreateHash )
	{
		HashDestroy( SecRefHash );
		SecRefHash = NULL;
	}

	return RetValue;
}

/****************************************************************
**	Routine: GetByName
**	Returns: Pointer to security object
**			 NULL if record could not be found
**	Summary: Load a security into memory by the name of the security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "GetByName"

static SDB_OBJECT *GetByName(
	const char	*SecName,		// Name to retrieve security by
	SDB_DB		*pDb,			// Specific database in which to look
	int			Flags,			// Bits in the DbID which are used as Get flags
	SDB_DB		*pDbPhysical	// Physical db to pass to SecDbTransProtectedGet( NULL means pDb )
)
{
	SDB_OBJECT
			*SecPtr = NULL,
			*OldSecPtr;

	SDB_DB	*SourceDb;

	SDB_STORED_OBJECT
			StoredObject;

	SDB_M_DATA
			MsgData;

	SDB_DEAD_POOL_ELEMENT
			*DeadPool;

	int		DeadPoolID,
			Success;

	static const char
			*Context = "";
				
	const char
			*ContextSave;

	if( !SecName )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": SecName missing" );
	if( !pDb )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, NULL ): pDb missing", SecName );

	if( !pDbPhysical )
		pDbPhysical = pDb;

/*
**	Check the cache first
*/
	if( !pDb->CacheHash )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, %s ): pDb->CacheHash is NULL (OpenCount=%d)", SecName, pDb->FullDbName.c_str(), pDb->OpenCount );

	OldSecPtr = (SDB_OBJECT *) HashLookup( pDb->CacheHash, SecName );

	// Take it from the deadpool if it is there
	if( OldSecPtr && OldSecPtr->ReferenceCount == 0 && pDb->DeadPoolSize )
	{
		DeadPool = pDb->DeadPool;

		DeadPoolID = OldSecPtr->DeadPoolID;
		DeadPool[ DeadPool[ DeadPoolID ].Next ].Prev = DeadPool[ DeadPoolID ].Prev;
		DeadPool[ DeadPool[ DeadPoolID ].Prev ].Next = DeadPool[ DeadPoolID ].Next;
		pDb->DeadPoolCount--;

		DeadPool[ DeadPool[ DEADPOOL_FREE_NODES ].Next ].Prev = DeadPoolID;
		DeadPool[ DeadPoolID ].Next = DeadPool[ DEADPOOL_FREE_NODES ].Next;
		DeadPool[ DeadPoolID ].Prev = DEADPOOL_FREE_NODES;
		DeadPool[ DEADPOOL_FREE_NODES ].Next = DeadPoolID;

		OldSecPtr->DeadPoolID = -1;
	}

	// Increment the current reference if we found it cache
	if( OldSecPtr )
	{
		OldSecPtr->ReferenceCount++;
		if( SecDbTraceFunc )
			(*SecDbTraceFunc)( SDB_TRACE_GET_OBJECT, OldSecPtr );
		TotalRefCount++;
		if( !( Flags & SDB_REFRESH_CACHE ))
			return OldSecPtr;	// Found in cache and !REFRESH_CACHE
	}
	else if( Flags & SDB_CACHE_ONLY )
	{
		if( !( Flags & SDB_GET_FATAL_ERRORS_ONLY ))
			Err( SDB_ERR_OBJECT_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ): @", SecName );
		return NULL;
	}


	
	// Get byte stream from database
	StatusMessage( "Loading %s", SecName );

	ContextSave = Context;
	Context = SecName;

	if( !( SourceDb = SecDbTransProtectedGet( pDb, SecName, &StoredObject, FALSE, pDbPhysical )))
	{
		ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
		goto Fail;
	}

	if( OldSecPtr )				// reload cached object
	{
		if( OldSecPtr->Class == SecDbClassInfoFromType( StoredObject.SdbDisk.Type ))
		{
			MsgData.Load.Changed		= OldSecPtr->Modified || ( Flags & SDB_ALWAYS_REFRESH ) ? TRUE
					: StoredObject.SdbDisk.LastTransaction != OldSecPtr->SecData.LastTransaction;

			if ( !MsgData.Load.Changed )
				MsgData.Load.Changed = SecDbObjectIsModified( OldSecPtr );

			SecPtr			= OldSecPtr;
			SecPtr->SecData = StoredObject.SdbDisk;
			SecPtr->Db		= pDb;

			MsgData.Load.Reload			= TRUE;
			MsgData.Load.Stream			= SecDbBStreamCreate( StoredObject.Mem, StoredObject.MemSize );
			MsgData.Load.CurrentVersion	= SecPtr->Class->ObjectVersion;
			SecDbDiskInfoGetVersion( &SecPtr->SecData, &MsgData.Load.Version );

			if( MsgData.Load.Version.Object != SecPtr->Class->ObjectVersion )
				Success = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD_VERSION, &MsgData );
			else
				Success = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD, &MsgData );
			SecDbBStreamDestroy( MsgData.Load.Stream, TRUE );
		}
		else
			Success = Err( SDB_ERR_OBJECT_MODIFIED, "Cannot reload '%s' since it is no longer of class '%s'", SecName, OldSecPtr->Class->Name );

		if( !Success )
		{
			ErrMore( "Reload( %s, %s )", SecName, pDb->FullDbName.c_str() );
			goto Fail;
		}
	}


/*
**	Or if no current cache, do a normal load
*/

	else
	{
		if ( TrackLoadHistory )
		{
			if ( GetByNameHistory == NULL )
			{
				if ( (GetByNameHistory = (DT_VALUE*) calloc( sizeof( DT_VALUE ), 1 )) != NULL )
					DTM_ALLOC( GetByNameHistory, DtArray );
			}
			if ( GetByNameHistory != NULL )
			{
				DT_VALUE 
						Rec;

				DTM_ALLOC( &Rec, DtStructure );
				DtComponentSetPointer( &Rec, "Name", DtString, StoredObject.SdbDisk.Name );
				DtComponentSetNumber( &Rec,  "TimeUpdated", DtTime, StoredObject.SdbDisk.TimeUpdated );
				DtComponentSetPointer( &Rec, "Context", DtString, ContextSave );
				DtAppendNoCopy( GetByNameHistory, &Rec );
			}
		}

		if( !ERR_CALLOC( SecPtr, SDB_OBJECT, 1, sizeof( *SecPtr )))
		{
			Context = ContextSave;
			return NULL;
		}
		SecPtr->SecData = StoredObject.SdbDisk;
		SecPtr->Db 		= pDb;
		SecPtr->Class 	= SecDbClassInfoFromType( SecPtr->SecData.Type );
		if( SecPtr->Class )
		{
			SecPtr->ReferenceCount	= 1;
			SecPtr->DeadPoolID		= -1;
			SecPtr->SdbClass		= SecPtr->Class;

			// Send the load message (check for version differences)
			MsgData.Load.Reload			= FALSE;
			MsgData.Load.Changed		= TRUE;
			MsgData.Load.Stream			= SecDbBStreamCreate( StoredObject.Mem, StoredObject.MemSize );
			MsgData.Load.CurrentVersion	= SecPtr->Class->ObjectVersion;
			SecDbDiskInfoGetVersion( &SecPtr->SecData, &MsgData.Load.Version );

			if( MsgData.Load.Version.Object != SecPtr->Class->ObjectVersion )
				Success = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD_VERSION, &MsgData );
			else
				Success = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD, &MsgData );
			SecDbBStreamDestroy( MsgData.Load.Stream, TRUE );

			// Insert the object into the cache
			if( Success )
			{
				SecPtr->DeadPoolID = -1;
				TotalRefCount++;

				SecDbClassObjectInsert( SecPtr );
				
				HashInsert( pDb->CacheHash, SecPtr->SecData.Name, SecPtr );
				if( SecDbTraceFunc )
					(*SecDbTraceFunc)( SDB_TRACE_GET_OBJECT, SecPtr );
			}
		}
		else
			Success = FALSE;

		if( !Success )
		{
			free( SecPtr );
			SecPtr = NULL;
			ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
		}
	}

	free( StoredObject.Mem );
	if( SecPtr )
	{
		SecPtr->Modified = FALSE;		// Mark SecPtr as unmodified
		SecPtr->SourceDb = SourceDb;
	}

	Context = ContextSave;
	return SecPtr;

Fail:
	Context = ContextSave;
	SecDbFree( OldSecPtr );
	return NULL;
}



/****************************************************************
**	Routine: GetNotFoundErrMsg
**	Returns: Nothing
**	Action : Recursively calls ErrMore for each db in search path
****************************************************************/

static void GetNotFoundErrMsg(
	SDB_DB_SEARCH_PATH	*SearchElem
)
{
	if( SearchElem->Next )
		GetNotFoundErrMsg( SearchElem->Next );
	ErrMore( "Searched %s", SearchElem->Db->FullDbName.c_str() );
}


/****************************************************************
**	Routine: SecDbGetByNameAndClassName
**	Returns: Pointer to security object
**			 NULL if record could not be found
**	Summary: Load a security into memory by the name and class of 
**			 the security. In the future, this will use Type arg
**			 to jump straight to the right db in ClassFilterUnions
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetByNameAndClassName"

SDB_OBJECT *SecDbGetByNameAndClassName(
	const char		*SecName,		// Name to retrieve security by
	const char		*ClassName,		// Security Class, ignored for now
	SDB_DB			*pDb,			// database to look in
	unsigned		Flags			// get flags
)
{
	SDB_SEC_TYPE
			SecType;

	SDB_OBJECT
			*Obj;


	if( !SecName )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( NULL ): SecName missing" );
	if( !ClassName )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, NULL ): ClassName missing", SecName );
	if( !pDb )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, %s, NULL ): pDb missing", SecName, ClassName );
	if( !( SecType = SecDbClassTypeFromName( ClassName ) ) )
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s, %s ): Invalid class", SecName, ClassName, pDb->FullDbName.c_str() );

	Obj = SecDbGetByNameAndClass( SecName, SecType, pDb, Flags );

	if( !Obj )
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s, %s )", SecName, ClassName, pDb->FullDbName.c_str() );
	
	return Obj;
}



/****************************************************************
**	Routine: SecDbGetByNameAndClass
**	Returns: Pointer to security object
**			 NULL if record could not be found
**	Summary: Load a security into memory by the name and class of 
**			 the security. In the future, this will use Type arg
**			 to jump straight to the right db in ClassFilterUnions
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetByNameAndClass"

SDB_OBJECT *SecDbGetByNameAndClass(
	const char		*SecName,		// Name to retrieve security by
	SDB_SEC_TYPE	Type,			// Security Class
	SDB_DB			*pDb,			// database to look in
	unsigned		Flags			// get flags
)
{
	SDB_DB	*pDbUpdate;

	SDB_OBJECT
			*Obj;


	if( !pDb )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, %s, NULL ): pDb missing", SecName, SecDbClassNameFromType( Type ) );
	if( !( pDbUpdate = SecDbDbUpdateGet( pDb, Type ) ) )
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s, %s ): Failed to get update db", SecName, SecDbClassNameFromType( Type ), pDb->FullDbName.c_str() );

	Obj = SecDbGetByName( SecName, pDb, Flags, pDbUpdate );

	if( !Obj )
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s, %s [%s] )", SecName, SecDbClassNameFromType( Type ), pDb->FullDbName.c_str(), pDbUpdate->FullDbName.c_str() );
	
	if( Type != Obj->SecData.Type )
	{
		Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME "( %s, %s, %s [%s] ): Object is of class %s", SecName, SecDbClassNameFromType( Type ), pDb->FullDbName.c_str(), pDbUpdate->FullDbName.c_str(), SecDbClassNameFromType( Obj->SecData.Type ) );
		SecDbFree( Obj );
		return NULL;
	}

	return Obj;
}


/****************************************************************
**	Routine: SecDbGetByName
**	Returns: Pointer to security object
**			 NULL if record could not be found
**	Summary: Load a security into memory by the name of the security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetByName"

SDB_OBJECT *SecDbGetByName(
	const char	*SecName,		// Name to retrieve security by
	SDB_DB		*pOrigDb,		// database to look in (may be union)
	unsigned	Flags,			// get flags
	SDB_DB		*pDbPhysical	// (optional, defaults to NULL) Look in this physical db
)
{
	SDB_DB	*pDb;

	
/*
**	Validate the arguments
*/
	if( !SecName )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - NULL passed as SecName" );

	if( !*SecName )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - SecName = \"\"" );

	if( strlen( SecName ) >= SDB_FULL_SEC_NAME_SIZE )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - SecName exceeds %d characters", SDB_FULL_SEC_NAME_SIZE );

	if( Flags == ( SDB_REFRESH_CACHE | SDB_CACHE_ONLY ))
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): @ SDB_REFRESH_CACHE and SDB_CACHE_ONLY are mutually exculsive", SecName );

	Flags |= AlwaysRefresh;				// FIX- Remove soon. How soon is that, exactly?

	if( !ResolveDbPath( SecName, pOrigDb, SecName ))
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ) ", SecName );
	if( !( pDb = DbToPhysicalDb( pOrigDb )))
		return (SDB_OBJECT *) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );

	SDB_OBJECT
			*SecPtr;

	// Attempt to get object from each db in search path until found or exhausted
	if( !( Flags & SDB_IGNORE_PATH ) && pDb->ParentDb->SecDbSearchPath )
	{
		SDB_DB_SEARCH_PATH
				*SearchElem;

		for( SecPtr = NULL, SearchElem = pDb->ParentDb->SecDbSearchPath; SearchElem; SearchElem = SearchElem->Next )
		{
			if( ( SecPtr = GetByName( SecName, SearchElem->Db, Flags, pDbPhysical )))
				break;
			if( SDB_ERR_OBJECT_NOT_FOUND != ErrNum )
				return( NULL );
		}
		if( !SecPtr )
		{
			Err( SDB_ERR_OBJECT_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s, %s ), @", SecName, pDb->DbName );
			GetNotFoundErrMsg( pDb->ParentDb->SecDbSearchPath );
			return NULL;
		}
	}
	else
		SecPtr = GetByName( SecName, pDb, Flags, pDbPhysical );

	return SecPtr;
}



/****************************************************************
**	Routine: SecDbGetFromSyncPoint
**	Returns: Pointer to security database record
**			 NULL if record could not be found
**	Action : Retrieve a security from the security database's
**			 syncpoint
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetFromSyncPoint"

SDB_OBJECT *SecDbGetFromSyncPoint(
	const char	*SecName,		// Name to retrieve security by
	SDB_DB		*pDb,			// database
	int			SyncPointOffset	// Number of syncpoints to look back
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_STORED_OBJECT
			StoredObject;

	SDB_M_DATA
			MsgData;

	int		Success;


/*
**	Validate the security name
*/

	if( !SecName )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @, SecName missing" );
	if( SyncPointOffset < 0 )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @, SyncPointOffset -ve" );


/*
**	Find the database
*/

	if( !( pDb = DbToPhysicalDb( pDb )))
		return (SDB_OBJECT *) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s, NULL ) @", SecName );

	if( pDb->DbOperator != SDB_DB_LEAF )
		return (SDB_OBJECT *) ErrN( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "( %s, %s ) on a virtual database", SecName, pDb->FullDbName.c_str() );


/*
**	Get the security and send it the load message
*/

	if( !(*pDb->pfGetFromSyncPoint)( pDb, SecName, SyncPointOffset, &StoredObject ))
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
	if( !SecDbHugeLoad( pDb, SyncPointOffset, &StoredObject ))
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s ): HugeLoad failed", SecName, pDb->FullDbName.c_str() );
	if( !ERR_CALLOC( SecPtr, SDB_OBJECT, 1, sizeof( *SecPtr )))
		return (SDB_OBJECT *) ErrN( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );
	SecPtr->SecData = StoredObject.SdbDisk;


/*
**	Set the name to something unique, and finish the load
*/

	sprintf( SecPtr->SecData.Name, "Security %ld #Sync#", (long) SecDbUniqueID( pDb ));

	SecPtr->Class = SecDbClassInfoFromType( SecPtr->SecData.Type );
	if( SecPtr->Class )
	{
		SecPtr->Db				= pDb;
		SecPtr->ReferenceCount	= 1;
		SecPtr->SdbClass		= SecPtr->Class;

		// Send the load message (check for version differences)
		MsgData.Load.Reload	= FALSE;
		MsgData.Load.Changed = TRUE;
		MsgData.Load.CurrentVersion	= SecPtr->Class->ObjectVersion;
		MsgData.Load.Stream = SecDbBStreamCreate( StoredObject.Mem, StoredObject.MemSize );
		SecDbDiskInfoGetVersion( &SecPtr->SecData, &MsgData.Load.Version );

		if( MsgData.Load.Version.Object != SecPtr->Class->ObjectVersion )
			Success = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD_VERSION, &MsgData );
		else
			Success = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD, &MsgData );
		SecDbBStreamDestroy( MsgData.Load.Stream, TRUE );

		if( !Success )
		{
			free( SecPtr );
			SecPtr = NULL;
			ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
		}
		else
		{
			SecPtr->DeadPoolID = -1;
			TotalRefCount++;

			SecDbClassObjectInsert( SecPtr );

			HashInsert( pDb->CacheHash, SecPtr->SecData.Name, SecPtr );
			if( SecDbTraceFunc )
				(*SecDbTraceFunc)( SDB_TRACE_GET_OBJECT, SecPtr );
		}
	}
	else
	{
		ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
		free( SecPtr );
		SecPtr = NULL;
	}

	return SecPtr;
}



/****************************************************************
**	Routine: SecDbGetSecurityNamesInRange
**	Returns: TRUE/Err
**	Action : gets the names of all securities for the specified
**	         class that fall within given range
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetSecurityNamesInRange"

int SecDbGetSecurityNamesInRange(
	const char	*SecDbClassName,
	const char	*Start,
	const char	*End,
	DT_VALUE	*ResultArray,	//already DTM_ALLOC'd
	int			*NumFound,		//number of secs found in range
	SDB_DB		*Db
)
{
	SDB_SEC_NAME_ENTRY
			*Names;

	int		GetType,
			NameCount,
			NameIndex,
			Done;

	const int
			MAX_NAMES = 100;
			
	SDB_SEC_TYPE
			Type;

	char	SrchName[ SDB_SEC_NAME_SIZE ];

			
	Type = SecDbClassTypeFromName( SecDbClassName );
	
	if( !Type )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": unable to get ClassType '%s'", SecDbClassName );
		
	if( !ERR_MALLOC( Names, SDB_SEC_NAME_ENTRY, sizeof( Names[0] ) * MAX_NAMES ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Error Allocating Memory" );

	GetType		= SDB_GET_GE;
	*NumFound	= 0;
	Done		= FALSE;
	strncpy( SrchName, Start, sizeof( SrchName ) - 1 );
		
	while( !Done && ( NameCount = SecDbNameLookupMany( SrchName, Names, MAX_NAMES, Type, GetType, Db )) )
	{
		for( NameIndex = 0; !Done && ( NameIndex < NameCount ); NameIndex++ )
		{
			// Here we get anything greater than Name.
			// We need to stop when past the End Name
			if( stricmp( Names[ NameIndex ], End ) > 0 )
			{
				Done = TRUE;
				break;
			}
			
			if( !DtAppendPointer( ResultArray, DtString , Names[ NameIndex ] ) )
			{
				free( Names );
                return ErrMore( ARGCHECK_FUNCTION_NAME ": Error adding string to Array" );
			}
            (*NumFound)++;
		}

		// if fewer than MAX_NAMES for this prefix, we're done
		if( NameCount < MAX_NAMES )
			Done = TRUE;
		else
		{
			GetType = SDB_GET_GREATER;
			strcpy( SrchName, Names[ NameCount -1 ] );
		}
	}
	free( Names );
	
	return( TRUE );
}



/****************************************************************
**	Routine: SecDbLocalUniqueID
**	Returns: SDB_UNIQUE_ID
**	Action :
****************************************************************/

static SDB_UNIQUE_ID SecDbLocalUniqueID(
	void
)
{
	static SDB_UNIQUE_ID
			UniqueID = 1;

	return UniqueID++;
}



/****************************************************************
**	Routine: SecDbNew
**	Returns: Pointer to new security
**			 NULL if error creating new security
**	Summary: Create a new security and put it in the cache
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNew"

SDB_OBJECT *SecDbNew(
	const char		*NewSecName,	// Security name, NULL to create a name
	SDB_SEC_TYPE	Type,			// Security type
	SDB_DB			*pDb			// Database to create security in
)
{
	SDB_CLASS_INFO
			*ClassInfo = SecDbClassInfoFromType( Type );

	if( !ClassInfo )
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get ClassInfo" );

	return SecDbNewByClass( NewSecName, ClassInfo, pDb );
}

/****************************************************************
**	Routine: CrunchName
**	Returns: Mash a name down to size
**	Summary: 
****************************************************************/

/* #define DEL_CHAR   strcpy( Buffer+i, Buffer+i+1 ); if ( --CurrSz <= MaxLen ) return; */
#define DEL_CHAR   if (1) { char *tmp=Buffer+i; while((*tmp=tmp[1])) tmp++; if ( --CurrSz <= MaxLen ) return; } else

static void CrunchName( 
	char *Buffer, 
	int  MaxLen
)
{
	int CurrSz = strlen( Buffer );
	int i;
	int lastChar;

	if ( CurrSz <= MaxLen )
		return;
	for( i=CurrSz-1; i; i-- )
		if ( Buffer[i] == ' ' )
        {
			DEL_CHAR;
        }
	lastChar = 0;
	for( i=CurrSz-1; i; i-- )
	{
		if ( Buffer[i] == lastChar )
        {
			DEL_CHAR;
        }
		if ( Buffer[i]>='a' && Buffer[i]<='z' )
			lastChar = Buffer[i];
		else
			lastChar = 0;
	}
	for( i=CurrSz-1; i; i-- )
		if ( Buffer[i] == 'e' || Buffer[i] == 'a' || Buffer[i] == 'i' || Buffer[i] == 'o' || Buffer[i] == 'u' )
        {
			DEL_CHAR;
        }
	for( i=CurrSz-1; i; i-- )
		if ( Buffer[i] >= 'a' && Buffer[i] <= 'z' )
        {
			DEL_CHAR;
        }
	for( i=CurrSz-1; i; i-- )
    {
		DEL_CHAR;
    }
}

/****************************************************************
**	Routine: SecDbNewByClass
**	Returns: Pointer to new security
**			 NULL if error creating new security
**	Summary: Create a new security and put it in the cache
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNewByClass"

SDB_OBJECT *SecDbNewByClass(
	const char		*NewSecName,	// Security name, NULL to create a name
	SDB_CLASS_INFO	*ClassInfo,		// Security class
	SDB_DB			*pDb			// Database to create security in
)
{
	SDB_DISK_VERSION
			Version;

	SDB_OBJECT
			*SecPtr,
			*Parent = NULL;

	SDB_M_DATA
			MsgData;

	char	NameBuffer[ SDB_FULL_SEC_NAME_SIZE ],
			*SecName;

	int		Len;

	SDB_DB	*pDbUpdate;


	if( !ClassInfo )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": ClassInfo arg missing" );
	if( !pDb )
		return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb arg missing" );

	if( ClassInfo->Interface || ClassInfo->Abstract )
		return (SDB_OBJECT *) ErrN( ERR_UNSUPPORTED_OPERATION, "@: \"%s\" cannot be instantiated -- \n"
				"no interfaces or abstract classes can!", ClassInfo->Name );

	// If the security name is null, build a security name
	if( !NewSecName )
	{
		long    Id = SecDbLocalUniqueID();
		int     MaxClassNameLen = SDB_CLASS_NAME_SIZE-1 - 5, // _NEW_
				IdSpace = 0;
		long    l = Id;
		char    ClassNameBuffer[ SDB_CLASS_NAME_SIZE+1 ],
				LexicoRadixBuffer[ SDB_CLASS_NAME_SIZE+1 ];
		char    *LexicoRadixPtr = LexicoRadixBuffer+SDB_CLASS_NAME_SIZE;

		*--LexicoRadixPtr = 0;
		for( l=Id; l; l/=36 )
		{
			int c = l%36;
			char ch = c < 10 ? '0' + c : 'A' + c-10;
			*--LexicoRadixPtr = ch;
			IdSpace++;
		}
		if ( IdSpace<3 )
			IdSpace = 3;
		
		MaxClassNameLen -= IdSpace+1; // space id
		strcpy( ClassNameBuffer, ClassInfo->Name );
		CrunchName( ClassNameBuffer, MaxClassNameLen );
		sprintf( NameBuffer, "%s%s %s", SecDbNewSecurityPrefix, ClassNameBuffer, LexicoRadixPtr );
		SecName = NameBuffer;
	}
	else
	{
        if( strlen( NewSecName ) >= SDB_FULL_SEC_NAME_SIZE )
            return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - SecName exceeds %d characters", SDB_FULL_SEC_NAME_SIZE );

		strcpy( NameBuffer, NewSecName );
		SecName = NameBuffer;

		if( !ResolveDbPath( SecName, pDb, SecName ))
			return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s )", NewSecName );

		// Strip blanks off the end of the security name
		for( Len = strlen( SecName ); --Len >= 0 && SecName[ Len ] == ' '; );
		SecName[ Len + 1 ] = '\0';

		if( Len == -1 )
			return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( ) @\nName is blank" );
		if( Len >= int( SDB_SEC_NAME_SIZE - 1 ))
			return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ) @\nName exceeds %d characters", SecName, SDB_SEC_NAME_SIZE - 1 );
		if( strchr( SecName, SDB_SEARCH_DELIMITER ))
			return (SDB_OBJECT *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ) @\nNew name cannot contain '%c'", SecName, SDB_SEARCH_DELIMITER );
	}

	if( pDb == SecDbClassObjectsDb )
		pDbUpdate = pDb;
	else if( !( pDbUpdate = SecDbDbUpdateGet( pDb, ClassInfo->Type ) ) )
		return (SDB_OBJECT *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s, %s ): Failed to get update db", SecName, ClassInfo->Name, pDb->FullDbName.c_str() );

	if( !( pDb = DbToPhysicalDb( pDb )))
		return (SDB_OBJECT *) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );


/*
**	If the security already exists in the cache, then a new
**	security will not be created
*/

	SecPtr = (SDB_OBJECT *) HashLookup( pDb->CacheHash, SecName );
	if( SecPtr )
	{
		if( SecPtr->ReferenceCount )
		{
			Err( SDB_ERR_OBJECT_ALREADY_EXISTS, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );
			goto Abort;
		}
		SecDbRemoveFromDeadPool( pDb, 0, SecName );
	}


/*
**	Allocate a new security
*/

	if( !(SecPtr = (SDB_OBJECT *)calloc( 1, sizeof( SDB_OBJECT ))))
	{
		Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );
		goto Abort;
	}

	strcpy( SecPtr->SecData.Name, SecName );
	SecPtr->SecData.Type		= ClassInfo->Type;
	Version.Object	= ClassInfo->ObjectVersion;
	Version.Stream	= pDb->StreamVersion;
	SecDbDiskInfoSetVersion( &SecPtr->SecData, &Version );
	SecPtr->Db					= pDb;
	SecPtr->Class				= ClassInfo;
	SecPtr->ReferenceCount		= 1;
	SecPtr->DeadPoolID			= -1;
	SecPtr->SdbClass			= SecPtr->Class;


/*
**	Send the 'NEW' message to the security
*/

	MsgData.New.Obsolete = 0;
	if( pDb != SecDbClassObjectsDb && !SDB_MESSAGE( SecPtr, SDB_MSG_NEW, &MsgData ))
	{
		free( SecPtr );
		SecPtr = NULL;
	}
	else
	{
		HashInsert( pDb->CacheHash, SecPtr->SecData.Name, SecPtr );
		TotalRefCount++;

		SecDbClassObjectInsert( SecPtr );

		if( SecDbTraceFunc )
			(*SecDbTraceFunc)( SDB_TRACE_GET_OBJECT, SecPtr );
	}

	if( SecPtr )
		SecPtr->SourceDb = pDbUpdate;

	return SecPtr;
Abort:
	SecDbFree( Parent );
	return NULL;
}



/****************************************************************
**	Routine: SecDbMsgMem
**	Returns: TRUE	- Msg succeeded
**			 FALSE	- Error
**	Action : Send SDB_MSG_MEM to SecPtr after setting up
**			 MsgData->Mem.Version and MsgData->Mem.Stream.
**			 The caller must always call
**			 SecDbBStreamDestroy( MsgData->Mem.Stream ),
**			 regardless of whether or not the call succeeded.
****************************************************************/

int SecDbMsgMem(
	SDB_OBJECT	*SecPtr,
	SDB_M_DATA	*MsgData
)
{
	memset( MsgData, 0, sizeof( *MsgData ));

	SecDbDiskInfoGetVersion( &SecPtr->SecData, &MsgData->Mem.Version );
	M_SETUP_VERSION( &MsgData->Mem.Version, SecPtr, SecPtr->Db );
	MsgData->Mem.Stream = SecDbBStreamCreateDefault();
	return SDB_MESSAGE( SecPtr, SDB_MSG_MEM, MsgData );
}



/****************************************************************
**	Routine: SecDbNewLoad
**	Returns: TRUE	- New security loaded without error
**			 FALSE	- Error loading new security information
**	Action : Load the object specific information for a new
**			 security from an SDB_STORED_OBJECT.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNewLoad"

int SecDbNewLoad(
	SDB_OBJECT			*SecPtr,	// Pointer to new security to load
	SDB_STORED_OBJECT	*StoredObj	// Stored object info
)
{
	SDB_M_DATA
			MsgData;

	int		Status;

	SDB_DB	*pDb;

	// object load/unload can only be performed in the real database which
	// they came
	pDb = SecPtr->SourceDb;
	if( SecPtr->SecData.Type != StoredObj->SdbDisk.Type )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ), @\nSecPtr->Type[%d] != StoredObj->Type[%d]",
					SecPtr->SecData.Name, (int) SecPtr->SecData.Type, (int) StoredObj->SdbDisk.Type );

	SDB_MESSAGE( SecPtr, SDB_MSG_UNLOAD, NULL );
	SecDbHugeUnload( pDb, SecPtr->SecData.Name );

	if( !SecDbHugeLoad( pDb, -1, StoredObj ))
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	MsgData.Load.Reload	= FALSE;
	MsgData.Load.Changed = TRUE;
	MsgData.Load.Stream = SecDbBStreamCreate( StoredObj->Mem, StoredObj->MemSize );
	// FIX-What if CurrentVersion does not match stored version?
	MsgData.Load.CurrentVersion	= SecPtr->Class->ObjectVersion;
	SecDbDiskInfoGetVersion( &StoredObj->SdbDisk, &MsgData.Load.Version );
	Status = SDB_MESSAGE( SecPtr, SDB_MSG_LOAD, &MsgData );
	SecDbBStreamDestroy( MsgData.Load.Stream, TRUE );
	return Status;
}


/****************************************************************
**	Routine: SecDbInferredName
**	Returns: TRUE	- Inferred name found
**			 FALSE	- Security couldn't infer a name
**	Summary: Return the inferred name of a security
****************************************************************/

int SecDbInferredName(
	SDB_OBJECT	*SecPtr,		// Security to get the inferred name of
	char		*SecName		// Return buffer for security name
)
{
	return SecDbInferredNameWithFlags( SecPtr, SecName, FALSE );
}

/****************************************************************
**	Routine: SecDbInferredNameWithFlags
**	Returns: TRUE	- Inferred name found
**			 FALSE	- Security couldn't infer a name
**	Summary: Return the inferred name of a security
**           See SecdbRenameWithFlags for CacheOnlyInferredName.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbInferredNameWithFlags"

int SecDbInferredNameWithFlags(
	SDB_OBJECT	*SecPtr,				// Security to get the inferred name of
	char		*SecName,				// Return buffer for security name
	int			CacheOnlyInferredName	// Compute implied name without talking to disk
)
{
	SDB_M_DATA
			MsgData;

	strcpy( MsgData.Rename.NewName, SecPtr->SecData.Name );
	MsgData.Rename.CreateNewName		= TRUE;
	MsgData.Rename.NewNamePtr			= NULL;

	if( !SecDbIsNew( SecPtr) && 
		 CacheOnlyInferredName )
		 return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, NULL ) @\nCannot use Cache Only Inferred Name flag on a persisted security", SecPtr->SecData.Name );
	MsgData.Rename.CacheOnlyInferredName = CacheOnlyInferredName;

	if( !SDB_MESSAGE( SecPtr, SDB_MSG_RENAME, &MsgData ))
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	strcpy( SecName, MsgData.Rename.NewNamePtr ? MsgData.Rename.NewNamePtr : MsgData.Rename.NewName );
	free( MsgData.Rename.NewNamePtr );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbNameUsed
**	Returns: TRUE	- Security name exists in database or memory
**			 FALSE	- Security name doesn't exist
**	Summary: Determine if a security name is already used
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNameUsed"

int SecDbNameUsed(
	const char	*SecName,		// Name of security to look for
	SDB_DB		*Db				// Database to look in
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_DISK
			DiskInfo;


	if( !( Db = DbToPhysicalDb( Db )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );

	return ( (SecPtr = (SDB_OBJECT *) HashLookup( Db->CacheHash, SecName )) && SecPtr->ReferenceCount )
			|| SecDbTransProtectedDiskInfo( Db, SecName, &DiskInfo ) ;
}



/****************************************************************
**	Routine: SecDbMergeMax
**	Returns: 1/-1
**	Action : Returns 1 if a merge operation for the specified
**			 get type requires a max, 0 for min
****************************************************************/

int SecDbMergeMax(
	int	GetType							// type of get
)
{
	switch( GetType )
	{
		case SDB_GET_LESS:
		case SDB_GET_LE:
		case SDB_GET_PREV:
		case SDB_GET_LAST:
			return 1;

		default:
			return 0;
	}
}



/****************************************************************
**	Routine: SecDbNameLookupInternal
**	Returns: SDB_DB * in which the name was found
**			 NULL if not found. Is different from Db for unions
**			 or other virtual databases
**	Summary: Lookup a security name using search criteria
****************************************************************/

SDB_DB *SecDbNameLookupInternal(
	char			*SecName,	// Starting name to find
	char			*FoundName, // where to put found name
	SDB_SEC_TYPE	SecType,	// Security type - or 0 for all securities
	int				GetType,	// Type of get operation
	SDB_DB			*Db			// Database to use
)
{
	SDB_SEC_NAME_ENTRY
			SecNames[ 2 ];

	SDB_DB	*FoundDb[ 2 ];


	FoundDb[ 0 ] = NULL;

	if( Db->IsVirtual() )
	{
		if( !Db->Right )
			return SecDbNameLookupInternal( SecName, FoundName, SecType, GetType, Db->Left );

		// lookup in left database, if not found then return lookup in right database
		strcpy( SecNames[ 0 ], SecName );
		if( !(FoundDb[ 0 ] = SecDbNameLookupInternal( SecNames[ 0 ], SecNames[ 0 ], SecType, GetType, Db->Left )))
			return SecDbNameLookupInternal( SecName, FoundName, SecType, GetType, Db->Right );

		strcpy( SecNames[ 1 ], SecName );
		if(( FoundDb[ 1 ] = SecDbNameLookupInternal( SecNames[ 1 ], SecNames[ 1 ], SecType, GetType, Db->Right )))
		{
			int WhichOne = ( stricmp( SecNames[ 0 ], SecNames[ 1 ]) >= 0 ) != SecDbMergeMax( GetType );
			// need to merge
			strcpy( FoundName, SecNames[ WhichOne ]);
			return FoundDb[ WhichOne ];
		}
		// Only Left lookup succeeded, return it
	}
	else
	{
		if(( SecType ?
			 (*Db->pfNameLookupByType)( Db, GetType, SecName, SecType, SecNames, 1 )
			 : (*Db->pfNameLookup)( Db, GetType, SecName, SecNames, 1 )
		   ))
			FoundDb[ 0 ] = Db;
	}

	if( FoundDb[ 0 ] )
		strcpy( FoundName, SecNames[ 0 ] );
	return FoundDb[ 0 ];
}



/****************************************************************
**	Routine: SecDbNameLookup
**	Returns: TRUE	- Next name gotten without error
**			 FALSE	- No more names
**	Summary: Lookup a security name using search criteria
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNameLookup"

int SecDbNameLookup(
	char			*SecName,	// Starting name to find and return results in
	SDB_SEC_TYPE	SecType,	// Security type - or 0 for all securities
	int				GetType,	// Type of get operation
	SDB_DB			*Db 		// Database to use
)
{
	char	*NameToLookup;

	if( strlen( SecName ) >= SDB_FULL_SEC_NAME_SIZE )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "@ - SecName exceeds %d characters", SDB_FULL_SEC_NAME_SIZE );

	if( !ResolveDbPath( SecName, Db, NameToLookup ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecName );

	if( !( Db = DbToPhysicalDb( Db )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );

	return SecDbNameLookupInternal( NameToLookup, NameToLookup, SecType, GetType, Db ) != NULL;
}



/****************************************************************
**	Routine: SecDbNameLookupMany
**	Returns: TRUE	- Next name gotten without error
**			 FALSE	- No more names
**	Summary: Lookup a set of security name using search criteria
**	Action : Get a list of security names that match the lookup
**			 criteria
**	SeeAlso: SecDbNameLookup
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNameLookupMany"

int SecDbNameLookupMany(
	const char			*SecName,	// Starting name
	SDB_SEC_NAME_ENTRY	*SecNames,	// Returned table of names
	int					TableSize,	// Max number of table entries
	SDB_SEC_TYPE		SecType,	// Security type - or 0 for all securities
	int					GetType,	// Type of get operation
	SDB_DB				*pDb 		// Database to use
)
{
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );

	if( pDb->IsVirtual() )
	{
		SDB_SEC_NAME_ENTRY
				*Table[ 2 ];

		int		Size[ 2 ],
				Index[2],
				i, Max;

		if( !pDb->Right )
			return SecDbNameLookupMany( SecName, SecNames, TableSize, SecType, GetType, pDb->Left );

		Table[ 0 ] = (SDB_SEC_NAME_ENTRY *) malloc( sizeof( SDB_SEC_NAME_ENTRY ) * ( TableSize + 1 ));
		Table[ 1 ] = (SDB_SEC_NAME_ENTRY *) malloc( sizeof( SDB_SEC_NAME_ENTRY ) * ( TableSize + 1 ));

		Size[ 0 ] = SecDbNameLookupMany( SecName, Table[ 0 ], TableSize, SecType, GetType, pDb->Left );
		Size[ 1 ] = SecDbNameLookupMany( SecName, Table[ 1 ], TableSize, SecType, GetType, pDb->Right );

		Max = SecDbMergeMax( GetType );

		// setup a min (or max) terminator so that we can safely compare against it
		strcpy( Table[ 1 ][ Size[ 1 ]], strcpy( Table[ 0 ][ Size[ 0 ]], Max > 0 ? "" : "\xff" ));

		for( Index[ 0 ] = Index[ 1 ] = i = 0;
			 Index[ 0 ] <= Size[ 0 ] && Index[ 1 ] <= Size[ 1 ] && i < TableSize; )
		{
			// decide which table has the max (or min)
			int cmp = stricmp( Table[ 0 ][ Index[ 0 ]], Table[ 1 ][ Index[ 1 ]]);
			int MaxTable = (cmp >= 0) != Max;

			// copy the correct entry, and advance it's index
			strcpy( SecNames[ i++ ], Table[ MaxTable ][ Index[ MaxTable ]++ ]);

			// if both entries were equal, skip the duplicate
			if( cmp == 0 )
				++Index[ MaxTable ^ 1 ];
		}

		// if terminator got copied, remove it
		if( Index[ 0 ] > Size[ 0 ] || Index[ 1 ] > Size[ 1 ])
			--i;

		free( Table[ 0 ]);
		free( Table[ 1 ]);
		return i;
	}

	if( SecType )
		return (*pDb->pfNameLookupByType)( pDb, GetType, SecName, SecType, SecNames, TableSize );

	return (*pDb->pfNameLookup)( pDb, GetType, SecName, SecNames, TableSize );
}



/****************************************************************
**	Routine: SecDbFreeDiskRecord
**	Returns: nothing
**	Action : Free disk record for a security
****************************************************************/

void SecDbFreeDiskRecord(
	SDB_STORED_OBJECT	*DiskObj
)
{
	free( DiskObj->Mem );
	DiskObj->Mem = NULL;
}



/****************************************************************
**	Routine: SecDbGetDiskRecord
**	Returns: TRUE	- Disk record loaded
**			 FALSE	- Couldn't load disk record
**	Action : Load disk record for a security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetDiskRecord"

int SecDbGetDiskRecord(
	SDB_STORED_OBJECT	*DiskObj,	// Pointer to returned disk record
	const char			*SecName,	// Name of security to lookup
	SDB_DB				*pDb		// Database to look in
)
{
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) null Db @", SecName );

	if( !SecDbTransProtectedGet( DbToPhysicalDb( pDb ), SecName, DiskObj, FALSE ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s ) @", SecName, pDb->FullDbName.c_str() );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbGetDiskInfo
**	Returns: TRUE	- Disk info retrieved without error
**			 FALSE	- Error retrieving disk info
**	Summary: Get the disk info structure for a security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetDiskInfo"

int SecDbGetDiskInfo(
	SDB_DISK	*DiskInfo,		// Pointer to returned disk information
	const char	*SecName,		// Name of security to lookup
	SDB_DB		*pDb,			// Database to look in
	unsigned	Flags
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_DB_SEARCH_PATH
			TempElem,
			*SearchElem;

	if( strlen( SecName ) >= SDB_FULL_SEC_NAME_SIZE )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - SecName exceeds %d characters", SDB_FULL_SEC_NAME_SIZE );

	if( !ResolveDbPath( SecName, pDb, SecName )) // FIX- don't cast away const
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecName );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) @", SecName );


/*
**	Look for object in each db in search path
*/

	if( !pDb->ParentDb->SecDbSearchPath || ( Flags & SDB_IGNORE_PATH ))
	{
		TempElem.Next	= NULL;
		TempElem.Db		= pDb;
		SearchElem		= &TempElem;
	}
	else
		SearchElem 		= pDb->ParentDb->SecDbSearchPath;

	for( ; SearchElem; SearchElem = SearchElem->Next )
	{
		if( !(Flags & SDB_REFRESH_CACHE )) // first check in cache
		{
			SecPtr = (SDB_OBJECT *) HashLookup( SearchElem->Db->CacheHash, SecName );
			if( SecPtr )
			{
				*DiskInfo = SecPtr->SecData;
				return TRUE;
			}
		}

		if( Flags & SDB_CACHE_ONLY )
			continue;

		if( SecDbTransProtectedDiskInfo( SearchElem->Db, SecName, DiskInfo )) // check in database
			return TRUE;
	}
	return Err( SDB_ERR_OBJECT_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ): @", SecName );
}



/****************************************************************
**	Routine: SecDbDiskInfoGetVersion
**	Returns:
**	Action :
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDiskInfoGetVersion"

int SecDbDiskInfoGetVersion(
	const SDB_DISK			*DiskInfo,
	SDB_DISK_VERSION		*Version
)
{
	int		VerInfo = DiskInfo->VersionInfo;


	memset( Version, 0, sizeof( *Version ));
	Version->Object = (VerInfo >> SDB_DIS_OBJ_VER )	   & SDB_DIM_OBJ_VER;
	Version->Stream = ((VerInfo >> 15) & 1) | ( ((VerInfo >> 11) & 2 ) );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbDiskInfoSetVersion
**	Returns:
**	Action :
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDiskInfoSetVersion"

int SecDbDiskInfoSetVersion(
	SDB_DISK				*DiskInfo,
	const SDB_DISK_VERSION	*Version
)
{
	DiskInfo->VersionInfo = -1;
	if( (Version->Object & SDB_DIM_OBJ_VER) != Version->Object )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ", @: Bad Object Version (%d)", Version->Object );
	if( (Version->Stream & SDB_DIM_STREAM_VER) != Version->Stream )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ", @: Bad Stream Version (%d)", Version->Stream );

	DiskInfo->VersionInfo =	(Version->Object << SDB_DIS_OBJ_VER)
			| ( ( Version->Stream & 1 ) << 15 )
			| ( ( Version->Stream & 2 ) << 11 );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbDiskInfoToStructure
**	Returns: TRUE or FALSE
**	Action : Put SDB_DISK fields in a structure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDiskInfoToStructure"

int SecDbDiskInfoToStructure(
	const SDB_DISK	*SdbDisk,
	DT_VALUE		*Struct
)
{
	SDB_DISK_VERSION
			Version;

	SDB_DB_INFO
			DbInfo;

	const char
			*ClassName;

	DT_BINARY
			Binary;


	DTM_ALLOC( Struct, DtStructure );

	DtComponentSetPointer( Struct, "Name", 			DtString, 	SdbDisk->Name );
	ClassName = SecDbClassNameFromType( SdbDisk->Type );
	if( ClassName )
		DtComponentSetPointer( Struct, "Type", DtString, ClassName );
	else
		DtComponentSetNumber( Struct, "Type", DtDouble, SdbDisk->Type );
	DtComponentSetNumber(  Struct, "TimeUpdated",		DtTime,		SdbDisk->TimeUpdated );
	DtComponentSetNumber(  Struct, "UpdateCount",		DtDouble,	SdbDisk->UpdateCount );
	DtComponentSetNumber(  Struct, "DateCreated", 		DtDate, 	(double) (DATE) SdbDisk->DateCreated );
	DtComponentSetNumber(  Struct, "LastTransaction", 	DtDouble, 	SdbDisk->LastTransaction );

	SecDbDiskInfoGetVersion( SdbDisk, &Version );
	DtComponentSetNumber(  Struct, "ObjectVersion",	DtDouble,	Version.Object );
	DtComponentSetNumber(  Struct, "StreamVersion",	DtDouble,	Version.Stream );
	Binary.Size 	= sizeof( SdbDisk->VersionInfo );
	Binary.Memory	= (void *) &SdbDisk->VersionInfo;
	DtComponentSetPointer( Struct, "VersionInfo", 	DtBinary, 	&Binary );

	// FIX - deprecated
	DtComponentSetNumber(  Struct, "TimeCreated",	DtTime,		DATE_TO_TIME( (DATE) SdbDisk->DateCreated ));
	DtComponentSetNumber(  Struct, "DbCreated", DtNull,		0 );

	if( !SdbDisk->DbIDUpdated )
		DtComponentSetNumber(  Struct, "DbUpdated", DtNull,		0 );
	else if( SecDbDbInfoFromID( SdbDisk->DbIDUpdated, &DbInfo ))
		DtComponentSetPointer( Struct, "DbUpdated", DtString,	DbInfo.DbName );
	else
		DtComponentSetNumber(  Struct, "DbUpdated", DtDouble,	SdbDisk->DbIDUpdated );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbClassInfoToStructure
**	Returns: TRUE or FALSE
**	Action : Allocates and fills in a DtStructure with the given
**			 ClassInfo
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbClassInfoToStructure"

int SecDbClassInfoToStructure(
	const SDB_CLASS_INFO	*ClassInfo,
	DT_VALUE				*Value
)
{
	int		Status;


	Status = DTM_ALLOC( Value, DtStructure );
	if( !Status )
		return ErrMore( ARGCHECK_FUNCTION_NAME );
	Status = Status && DtComponentSetNumber(	Value, "Type",				DtDouble, (int) ClassInfo->Type		);
	Status = Status && DtComponentSetPointer(	Value, "DllPath",			DtString, ClassInfo->DllPath		);
	Status = Status && DtComponentSetPointer(	Value, "FuncName",			DtString, ClassInfo->FuncName		);
	Status = Status && DtComponentSetPointer(	Value, "Name",				DtString, ClassInfo->Name			);
	Status = Status && DtComponentSetPointer(	Value, "Argument",			DtString, ClassInfo->Argument		);
	Status = Status && DtComponentSetNumber(	Value, "ObjectVersion",		DtDouble, ClassInfo->ObjectVersion	);
	Status = Status && DtComponentSetNumber(	Value, "MaxStreamSize",		DtDouble, ClassInfo->MaxStreamSize	);
	Status = Status && DtComponentSetNumber(	Value, "ObjectCount",		DtDouble, ClassInfo->ObjectCount	);
	Status = Status && DtComponentSetNumber(	Value, "Temporary",			DtDouble, ClassInfo->Temporary		);
	Status = Status && DtComponentSetNumber(	Value, "Interface",			DtDouble, ClassInfo->Interface		);
	Status = Status && DtComponentSetNumber(	Value, "Abstract",			DtDouble, ClassInfo->Abstract		);
	Status = Status && DtComponentSetNumber(	Value, "MaxUnparentedNodes",DtDouble, ClassInfo->MaxUnparentedNodes	);
	Status = Status && DtComponentSetNumber(	Value, "GCBlockSize",		DtDouble, ClassInfo->GCBlockSize	);


/*
**	Implemented Interfaces and ValueTypes are only available if the class has already been loaded
*/
	if( Status )
	{
		DT_VALUE
				Interfaces;


		Status = Status && DTM_ALLOC( &Interfaces, DtStructure );
	
		if( Status && ClassInfo->ImplementedInterfaces )
		{
			for( SecDbInterfacesMap::const_iterator iter = ClassInfo->ImplementedInterfaces->begin();
				 iter != ClassInfo->ImplementedInterfaces->end(); ++iter )
				Status = Status && DtComponentSetPointer( &Interfaces, (*iter).first->Name, DtString, (*iter).second );
		}
		Status = Status && DtComponentSetPointer(	Value, "Implemented Interfaces",	DtStructure, DT_VALUE_TO_POINTER( &Interfaces )	);
		Status = Status && DTM_FREE( &Interfaces );
	}		

	if( Status )
	{
		DT_VALUE
				VtiValue,
				ValueTypes;


		Status = Status && DTM_ALLOC( &ValueTypes, DtArray );

		if( ClassInfo->ValueTable )
		{
			for( SDB_VALUE_TYPE_INFO* Vti = ClassInfo->ValueTable; Vti->Name && Status; ++Vti )
			{
				Status = Status && DtValueTypeInfoFromVti( &VtiValue, Vti );
				Status = Status && DtAppend( &ValueTypes, &VtiValue );
				Status = Status && DTM_FREE( &VtiValue );
			}
		}
		if( Status )
			Status = Status && DtComponentReplacePointer( Value, "ValueTypes",	DtArray, DT_VALUE_TO_POINTER( &ValueTypes )	);
		else
			DTM_FREE( &ValueTypes );
	}

	if( !Status )
	{
		ErrPush();
		DTM_FREE( Value );
		DTM_INIT( Value );
		ErrPop();
		ErrMore( ARGCHECK_FUNCTION_NAME );
	}
	return Status;
}



/****************************************************************
**	Routine: SecDbObjectCheckSum
**	Returns: An allocated structure with the object's checksum
**			 in the main table and in each index
**	Action : Ask the server for the appropriate checksums
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbObjectCheckSum"

SDB_OBJECT_CHECK_SUM *SecDbObjectCheckSum(
	const char	*SecName,
	SDB_DB		*pDb
)
{
	if( !pDb )
		return (SDB_OBJECT_CHECK_SUM *) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( %s, NULL ) @", SecName );

	if( pDb->IsVirtual() )
		return (SDB_OBJECT_CHECK_SUM *) ErrN( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "( %s, %s ): @ when a database is virtual", SecName, pDb->FullDbName.c_str() );

	return (*pDb->pfGetObjCheckSum)( pDb, SecName );
}



/****************************************************************
**	Routine: SecDbObjectCheckSumFree
**	Returns: Nothing
**	Action : Free a checksum structure
****************************************************************/

void SecDbObjectCheckSumFree(
	SDB_OBJECT_CHECK_SUM	*CheckSum
)
{
	SDB_OBJECT_CHECK_SUM
			*Sum,
			*Next;


	for( Sum = CheckSum; Sum; Sum = Next )
	{
		Next = Sum->Next;
		free( Sum );
	}
}



/****************************************************************
**	Routine: SecDbFree
**	Returns: Nothing
**	Summary: Free a security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFree"

void SecDbFree(
	SDB_OBJECT	*SecPtr			// Pointer to the security to free
)
{
	SDB_DEAD_POOL_ELEMENT
			*DeadPool;

	SDB_DB	*pDb;

	int		DeadPoolID;


	if( !SecPtr )
		return;

	if( SecPtr->ReferenceCount <= 0 )
		Err( SDB_ERR_OBJECT_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) - @, Reference count (%d) <= 0!\n", SecPtr->SecData.Name, SecPtr->ReferenceCount );

	else
	{
		TotalRefCount--;
		if( SecDbTraceFunc )
			(*SecDbTraceFunc)( SDB_TRACE_FREE_OBJECT, SecPtr );
		if( --SecPtr->ReferenceCount == 0 )
		{
			pDb = DbToPhysicalDb( SecPtr->Db );
			if( !pDb )					// orphaned SecPtr, free
				SecPtr->Free();
			// If the security did not come from a database, and was not
			// written to a database - remove it.
			else if( SecDbIsNew( SecPtr ) || 0 == pDb->DeadPoolSize )
				SecDbRemoveFromDeadPool( pDb, 0, SecPtr->SecData.Name );

			else
			{
				static int
						CleaningDeadPool = FALSE;

				// If the dead-pool is full AND not currently cleaning
				// dead-pool, remove the oldest member
				if( CleaningDeadPool )
					SecDbRemoveFromDeadPool( pDb, 0, SecPtr->SecData.Name );
				else
				{
					CleaningDeadPool = TRUE;
					DeadPool = pDb->DeadPool;

					while( DeadPool[ DEADPOOL_FREE_NODES ].Next == DEADPOOL_FREE_NODES )
						SecDbRemoveFromDeadPool( pDb, 0, DeadPool[ DeadPool[ DEADPOOL_USED_NODES ].Next ].SecPtr->SecData.Name );

					// Add the security to the deadpool
					DeadPoolID = DeadPool[ DEADPOOL_FREE_NODES ].Next;
					DeadPool[ DeadPool[ DeadPoolID ].Next ].Prev = DeadPool[ DeadPoolID ].Prev;
					DeadPool[ DeadPool[ DeadPoolID ].Prev ].Next = DeadPool[ DeadPoolID ].Next;

					DeadPool[ DeadPoolID ].SecPtr = SecPtr;
					SecPtr->DeadPoolID = DeadPoolID;
					DeadPool[ DeadPoolID ].Next = DEADPOOL_USED_NODES;
					DeadPool[ DeadPoolID ].Prev = DeadPool[ DEADPOOL_USED_NODES ].Prev;
					DeadPool[ DeadPool[ DeadPoolID ].Prev ].Next = DeadPoolID;
					DeadPool[ DEADPOOL_USED_NODES ].Prev = DeadPoolID;
					pDb->DeadPoolCount++;
					CleaningDeadPool = FALSE;
				}
			}
		}
	}
}



/****************************************************************
**	Routine: SecDbGetByInference
**	Returns: TRUE	- Security gotten or created by inference
**			 FALSE	- Couldn't get or create by inference
**	Summary: Get or create a security by inferring it's existence
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetByInference"

int SecDbGetByInference(
	SDB_OBJECT	**pSecPtr, 	// Pointer to object pointer
	int			CreateFlag	// SDB_GET enum flags: create, cache_only, etc
)
{
	static int
			FirstTime = TRUE;

	static char
			*Disable = NULL;

	SDB_OBJECT
			*NewSecPtr;

	SDB_M_DATA
			MsgData;

	int		Len;

	char	*NewName;


/*
**	First, if this object already came from disk, just return true
*/

	if( !SecDbIsNew( *pSecPtr ))
		return TRUE;


/*
**	Check SDB_GET_BY_INFERENCE_DISABLE envvar
*/

	if( FirstTime )
	{
		FirstTime	= FALSE;
		Disable		= getenv( "SDB_GET_BY_INFERENCE_DISABLE" );
	}
	if( Disable )
	{
		CreateFlag |= SDB_GET_CACHE_ONLY;
		CreateFlag &= ~SDB_GET_CREATE;
	}

/*
**	Get the implied name of the security
*/

	if(( CreateFlag & SDB_GET_CREATE ) && ( CreateFlag & SDB_GET_CACHE_ONLY ))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "(): Invalid flags %d", CreateFlag );

	*MsgData.Rename.NewName				= '\0';
	MsgData.Rename.CreateNewName		= TRUE;
	MsgData.Rename.NewNamePtr			= NULL;
	MsgData.Rename.CacheOnlyInferredName = ( CreateFlag & SDB_GET_CACHE_ONLY_INFERRED_NAME ) == 
											 SDB_GET_CACHE_ONLY_INFERRED_NAME;

	if( !SDB_MESSAGE( *pSecPtr, SDB_MSG_RENAME, &MsgData ))
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	NewName = MsgData.Rename.NewNamePtr ? MsgData.Rename.NewNamePtr : MsgData.Rename.NewName;
	for( Len = strlen( NewName ); --Len >= 0 && NewName[ Len ] == ' '; )
		NewName[ Len ] = '\0';

/*
**	Try to get the security using the implied name, if the security doesn't
**	exist than perhaps it should be created
*/

	if( CreateFlag & SDB_GET_CACHE_ONLY )
		NewSecPtr = SecDbGetByName( NewName, (*pSecPtr)->Db, SDB_CACHE_ONLY );
	else
		NewSecPtr = SecDbGetByName( NewName, (*pSecPtr)->Db, 0 );

	if( NewSecPtr )
	{
		free( MsgData.Rename.NewNamePtr );
		SecDbFree( *pSecPtr );
		*pSecPtr = NewSecPtr;

		if( ( CreateFlag & SDB_GET_CREATE ) && SecDbIsNew( NewSecPtr ))
			if( !SecDbAdd( *pSecPtr ))
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to add new security" );

		return TRUE;
	}
	else
	{
		if( !SecDbRename( *pSecPtr, NewName ))
		{
			free( MsgData.Rename.NewNamePtr );
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to rename security" );
		}
		free( MsgData.Rename.NewNamePtr );

		if( CreateFlag & SDB_GET_CREATE )
		{
		 	if( SecDbAdd( *pSecPtr ))
				return TRUE;
		}
		else
			return TRUE;
	}

	return ErrMore( ARGCHECK_FUNCTION_NAME );
}



/****************************************************************
**	Routine: SecDbIncrementReference
**	Returns: Same SecPtr passed as an argument
**	Summary: Increment an object's reference count
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIncrementReference"

SDB_OBJECT *SecDbIncrementReference(
	SDB_OBJECT	*SecPtr
)
{
	if( SecPtr )
	{
		if( SecPtr->ReferenceCount <= 0 )
			return (SDB_OBJECT *) ErrN( SDB_ERR_OBJECT_INVALID, ARGCHECK_FUNCTION_NAME "( %s ) @ -- reference count is zero", SecPtr->SecData.Name );
		SecPtr->ReferenceCount++;
		if( SecDbTraceFunc )
			(*SecDbTraceFunc)( SDB_TRACE_GET_OBJECT, SecPtr );
		TotalRefCount++;
	}
	return SecPtr;
}



/****************************************************************
**	Routine: SecDbReload
**	Returns: TRUE	- Reloaded without errors
**			 FALSE	- Error reloading security
**	Summary: Reload a single security, or all loaded securities
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbReload"

int SecDbReload(
	SDB_OBJECT	*SecPtr		// NULL to reload all loaded securities
)
{
	int		RetValue;


	if( !SecPtr )
		RetValue = SecDbDbReload( SecDbRootDb, 0 );
	else if( SecDbIsNew( SecPtr ))
		RetValue = TRUE;
	else if( SecDbGetByName( SecPtr->SecData.Name, SecPtr->Db, SDB_REFRESH_CACHE ))
	{
		SecDbFree( SecPtr );
		RetValue = TRUE;
	}
	else
		RetValue = FALSE;

	return RetValue;
}



/****************************************************************
**	Routine: SecDbDbReload
**	Returns: TRUE	- Reloaded without errors
**			 FALSE	- Error reloading securities
**	Action : Reload all of the objects in a database
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDbReload"

int SecDbDbReload(
	SDB_DB		*Db,
	unsigned	Flags
)
{
	SDB_OBJECT
			*SecPtr;

	HASH_ENTRY_PTR
			Ptr;

	int		RetValue = TRUE;

	SDB_DB_SEARCH_PATH
			TempElem,
			*SearchElem;


	if( !( Db = DbToPhysicalDb( Db )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "() @" );
	SecDbRemoveFromDeadPool( Db, Flags, NULL );

	if( !Db->ParentDb->SecDbSearchPath || ( Flags & SDB_IGNORE_PATH ))
	{
		TempElem.Next	= NULL;
		TempElem.Db 	= Db;
		SearchElem		= &TempElem;
	}
	else
		SearchElem		= Db->ParentDb->SecDbSearchPath;

	for( ; SearchElem; SearchElem = SearchElem->Next )
	{
		HASH_FOR( Ptr, Db->CacheHash )
		{
			SecPtr = (SDB_OBJECT *) HashValue( Ptr );
			if( !SecDbIsNew( SecPtr ))
			{
				if( GetByName( SecPtr->SecData.Name, Db, SDB_REFRESH_CACHE | AlwaysRefresh, Db ))
					SecDbFree( SecPtr );
				else
					RetValue = FALSE;
			}
		}
	}

	return RetValue;
}



/****************************************************************
**	Routine: SecDbCreateDeadPool
**	Returns: TRUE	- Deadpool created without error
**			 FALSE	- Error creating deadpool
**	Action : Create a deadpool for a database
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbCreateDeadPool"

int SecDbCreateDeadPool(
	SDB_DB	*pDb,			// Database to operate on
	int		DeadPoolSize	// Size of deadpool
)
{
	SDB_DEAD_POOL_ELEMENT
			*DeadPool;

	int		DeadPoolID;


	pDb->DeadPoolSize = DeadPoolSize;
	if( DeadPoolSize )
	{
		DeadPoolSize += DEADPOOL_FIRST_NODE;

		pDb->DeadPool = DeadPool = (SDB_DEAD_POOL_ELEMENT *) calloc( DeadPoolSize, sizeof( SDB_DEAD_POOL_ELEMENT ));
		if( !DeadPool )
			return Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ", @" );
		for( DeadPoolID = DEADPOOL_FIRST_NODE; DeadPoolID < DeadPoolSize; DeadPoolID++ )
		{
			DeadPool[ DeadPoolID ].Prev = DeadPoolID - 1;
			DeadPool[ DeadPoolID ].Next = DeadPoolID + 1;
		}
		DeadPool[ DEADPOOL_USED_NODES ].Prev = DEADPOOL_USED_NODES;
		DeadPool[ DEADPOOL_USED_NODES ].Next = DEADPOOL_USED_NODES;
		DeadPool[ DEADPOOL_FREE_NODES ].Prev = DeadPoolSize - 1;
		DeadPool[ DEADPOOL_FREE_NODES ].Next = DEADPOOL_FIRST_NODE;
		DeadPool[ DEADPOOL_FIRST_NODE ].Prev = DEADPOOL_FREE_NODES;
		DeadPool[ DeadPoolSize - 1 ].Next	 = DEADPOOL_FREE_NODES;
	}
	else
		pDb->DeadPool = NULL;

	return TRUE;
}



/****************************************************************
**	Routine: SecDbRemoveFromDeadPool
**	Returns: TRUE	- Security removed from at least one deadpool
**			 FALSE	- Security not removed from any deadpool
**	Summary: Remove a security from all the deadpools in the search path
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbRemoveFromDeadPool"

int SecDbRemoveFromDeadPool(
	SDB_DB		*Db,					// Database to operate on
	unsigned	Flags,					// Db Flags
	const char	*SecName				// Security to remove, NULL to remove everything!
)
{
	SDB_DEAD_POOL_ELEMENT
			*DeadPool;

	SDB_OBJECT
			*SecPtr;

	int		DeadPoolID,
			Token,
			Status = FALSE;

	SDB_DB_SEARCH_PATH
			TempElem,
			*SearchElem;
            
	// Make a local copy of SecName. It can point to the Sec we want to
	// free, then we will crash in the following loop. 
	string sSecName;
	if( SecName )
	{
		sSecName = SecName;
		SecName = sSecName.c_str();
	    if( strlen( SecName ) >= SDB_FULL_SEC_NAME_SIZE )
		    return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ) @\nName exceeds %d characters", SecName, SDB_FULL_SEC_NAME_SIZE );

        if( !ResolveDbPath( SecName, Db, SecName ) )
            return ErrMore( ARGCHECK_FUNCTION_NAME "() %s", SecName );
	}

	if( !( Db = DbToPhysicalDb( Db )))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( ) @" );

/*
**	Remove from deadpool in any db in search path
*/

	if( !Db->ParentDb->SecDbSearchPath || ( Flags & SDB_IGNORE_PATH ))
	{
		TempElem.Next	= NULL;
		TempElem.Db		= Db;
		SearchElem		= &TempElem;
	}
	else
		SearchElem 		= Db->ParentDb->SecDbSearchPath;

	for( ; SearchElem; SearchElem = SearchElem->Next )
	{
		DeadPool = SearchElem->Db->DeadPool;

		if( !SecName )					// handle special case of ALL securities in deadpool
		{
			if( SearchElem->Db->DeadPoolSize )
			{
				while( SearchElem->Db->DeadPoolCount )
				{
					Token = 0;
					SDB_NODE::Destroy( NULL, &Token );
					while( (DeadPoolID = DeadPool[ DEADPOOL_USED_NODES ].Next) != DEADPOOL_USED_NODES )
					{
						SecPtr = DeadPool[ DeadPoolID ].SecPtr;

						DeadPool[ DeadPool[ DeadPoolID ].Next ].Prev = DeadPool[ DeadPoolID ].Prev;
						DeadPool[ DeadPool[ DeadPoolID ].Prev ].Next = DeadPool[ DeadPoolID ].Next;
						SearchElem->Db->DeadPoolCount--;

						DeadPool[ DeadPool[ DEADPOOL_FREE_NODES ].Next ].Prev = DeadPoolID;
						DeadPool[ DeadPoolID ].Next = DeadPool[ DEADPOOL_FREE_NODES ].Next;
						DeadPool[ DeadPoolID ].Prev = DEADPOOL_FREE_NODES;
						DeadPool[ DEADPOOL_FREE_NODES ].Next = DeadPoolID;

						SecDbHugeUnload( SearchElem->Db, SecPtr->SecData.Name );

						// Remove ALL cached values
						NodeDestroyCache( SecPtr, TRUE );
					}
					SDB_NODE::Destroy( NULL, &Token );
					Status = TRUE;
				}
			}
		}
		else							// standard case of a single security, by name
		{
			if( !(SecPtr = (SDB_OBJECT *) HashLookup( SearchElem->Db->CacheHash, SecName )))
			{
				Status = Status || Err( SDB_ERR_OBJECT_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ), @", SecName );
				continue;
			}

			if( SecPtr->ReferenceCount )
			{
				Status = Status || Err( SDB_ERR_OBJECT_IN_USE, ARGCHECK_FUNCTION_NAME "( %s ), @", SecName );
				continue;
			}

			if( SearchElem->Db->DeadPoolSize && -1 != (DeadPoolID = SecPtr->DeadPoolID))
			{
				DeadPool[ DeadPool[ DeadPoolID ].Next ].Prev = DeadPool[ DeadPoolID ].Prev;
				DeadPool[ DeadPool[ DeadPoolID ].Prev ].Next = DeadPool[ DeadPoolID ].Next;
				SearchElem->Db->DeadPoolCount--;

				DeadPool[ DeadPool[ DEADPOOL_FREE_NODES ].Next ].Prev = DeadPoolID;
				DeadPool[ DeadPoolID ].Next = DeadPool[ DEADPOOL_FREE_NODES ].Next;
				DeadPool[ DeadPoolID ].Prev = DEADPOOL_FREE_NODES;
				DeadPool[ DEADPOOL_FREE_NODES ].Next = DeadPoolID;
			}

			SecDbHugeUnload( SearchElem->Db, SecPtr->SecData.Name );

			// Remove ALL cached values
			NodeDestroyCache( SecPtr, TRUE );

			Status = TRUE;
		}
	}

	return Status;
}



/****************************************************************
**	Routine: SecDbDbFromDbIDFunc
**	Returns: Database pointer if ID is valid
**			 NULL if ID is invalid
**	Action : Convert database ID into a database pointer
**			 non inline version of the same
****************************************************************/

SDB_DB *SecDbDbFromDbIDFunc(
	SDB_DB_ID_WITH_FLAGS	DbID
)
{
	return SecDbDbFromDbID( DbID );		// this looks like a cyclic defn but it isn't
										// because the SecDb project always compiles in C++ mode
}



/****************************************************************
**	Routine: SecDbStatistics
**	Returns: TRUE	- Success
**			 FALSE	- Failure
**	Summary: Retrieve security database statistics
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbStatistics"

int SecDbStatistics(
	SDB_STATISTICS	*Stats,		// Pointer to statistics structure
	SDB_DB			*pDb		// Database to get info for
)
{
	HEAP_STATS
			HeapStats;


	if( !pDb )
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "() @" );

	memset( Stats, 0, sizeof( *Stats ));

	// Global statistics
	Stats->ValueTypeUsage	= SecDbValueTypeHash->KeyCount;
	Stats->SecTypeUsage		= SecDbSecTypeHash->KeyCount;
	Stats->DataTypeUsage	= DtNamesHash->KeyCount;

	// FIX - THESE SHOULD BE ON A BY DATABASE LEVEL
	Stats->DiddleCount		= (long) DiddleCount;
	Stats->ObjectRefCount	= (long) TotalRefCount;

	// Database statistics
	Stats->DeadPoolSize		= pDb->DeadPoolSize;
	Stats->DeadPoolUsage	= pDb->DeadPoolCount;
	Stats->ObjectCacheUsage	= pDb->CacheHash ? pDb->CacheHash->KeyCount : 0;

	GSHeapStatistics( SDB_NODE::NodeHeap(), &HeapStats );
	Stats->ValueCacheCount	= HeapStats.UsedElementCount;
	// FIX newtrees - does not include: Nodev, DtSize( Value ), or Literals
	Stats->ValueCacheMemory	= HeapStats.UsedMemory;

	return TRUE;
}



/****************************************************************
**	Routine: SecDbInvalidate
**	Returns: TRUE or FALSE
**	Action : Invalidates given node
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbInvalidate"

int SecDbInvalidate(
	SDB_OBJECT		*SecPtr,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	DT_VALUE		**Argv
)
{
	SDB_NODE
			*pNode;

	SDB_M_DATA
			MsgData;

	SDB_VALUE_TYPE_INFO
			*Vti;

	pNode = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::CHECK_SUPPORTED, SecDbRootDb, SDB_NODE::SHARE );
	if( !pNode )
		return ErrNum != ERR_NOT_FOUND;

	// VTI must exist unless this was a diddle on non-existant node
	Vti = pNode->VtiGet();
	if( Vti )
	{
		if( Vti->ValueFlags & SDB_IN_STREAM )
			SecPtr->Modified = TRUE;

		MsgData.Invalidate.Argc = Argc;
		MsgData.Invalidate.Argv = Argv;
		if( !SDB_MESSAGE_VT_TRACE( SecPtr, SDB_MSG_INVALIDATE, &MsgData, Vti ))
			return FALSE;
	}

	SDB_DBSET_BEST
			*Best;

	int		Success = TRUE;

	for( SDB_NODE::nodes_iterator it = SDB_NODE::nodes().lower_bound( pNode ); it != SDB_NODE::nodes().end() && equal( pNode, *it ); ++it )
		if( (*it)->DbSetGet()->BaseDb() && ( Best = SDB_DBSET_BEST::Lookup( (*it)->DbSetGet(), pNode->DbSetGet()->BaseDb() )) && Best->m_Shareable )
			Success = (*it)->InvalidateSafe( SDB_CACHE_DIDDLE ) && Success;

	return Success;
}



/****************************************************************
**	Routine: SecDbGetFlags
**	Returns: The setflags for a given node, 0 if it does not exist
**	Action : Finds given node and extracts CacheFlags
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetFlags"

SDB_SET_FLAGS SecDbGetFlags(
	SDB_OBJECT		*SecPtr,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	DT_VALUE		**Argv
)
{
	SDB_NODE
			*pNode;


	pNode = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::CHECK_SUPPORTED, SecDbRootDb, SDB_NODE::SHARE );
	if( !pNode || !pNode->ValidGet() )
		return 0;

	return pNode->CacheFlagsGet();
}



/****************************************************************
**	Routine: SearchPathAppend
**	Returns: DbID of appended db
**	Action : Append child db (unless found already in path)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SearchPathAppend"

static SDB_DB *SearchPathAppend(
	SDB_DB		*ParentDb,
	const char	*ChildName
)
{
	SDB_DB	*ChildDb;

	SDB_DB_SEARCH_PATH
			*SearchElem,
			**pSearch;

	ChildDb = SecDbAttach( ChildName, ParentDb->DeadPoolSize, 0 );
	if( !ChildDb )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach" );
	for( pSearch = &ParentDb->SecDbSearchPath; *pSearch; pSearch = &(*pSearch)->Next )
		if( (*pSearch)->Db == ChildDb )
		{
			SecDbDetach( ChildDb );
			return ChildDb;
		}
	if( ChildDb->ParentDb != ChildDb )
	{
		SecDbDetach( ChildDb );
		Err( ERR_DATABASE_IN_USE, "@: %s is already in search path of %s", ChildDb->DbName, ChildDb->ParentDb->DbName );
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": %s cannot be added to searchpath for %s", ChildDb->DbName, ParentDb->DbName );
	}
	if( !ERR_CALLOC( SearchElem, SDB_DB_SEARCH_PATH, 1, sizeof( SDB_DB_SEARCH_PATH )))
		return (SDB_DB *) Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );
	ChildDb->ParentDb = ParentDb;
	SearchElem->Db = ChildDb;
	*pSearch = SearchElem;
	return( ChildDb );
}



/****************************************************************
**	Routine: SecDbSearchPathAppend
**	Returns: DbID of child database
**	Action : Appends a new db to the end of search path for given
**			 db.  If child db is already in search path (in
**			 any position, just returns TRUE and does nothing).
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbSearchPathAppend"

SDB_DB *SecDbSearchPathAppend(
	SDB_DB		*ParentDb,
	const char	*ChildName
)
{
	if( !( ParentDb = DbToPhysicalDb( ParentDb )))
		return (SDB_DB *) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME ": Null Db" );
/*
**	Make default element for myself (and make parent read only)
*/

	if( !ParentDb->SecDbSearchPath )
		if( !SearchPathAppend( ParentDb, ParentDb->DbName ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME );


/*
**	Append child db
*/

	return( SearchPathAppend( ParentDb, ChildName ));
}



/****************************************************************
**	Routine: SecDbSearchPathDelete
**	Returns: TRUE or FALSE
**	Action : Deletes a db from the search path for given parent
**			 db.  If child db is not in search path returns
**			 FALSE.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbSearchPathDelete"

int SecDbSearchPathDelete(
	SDB_DB	*ParentDb,
	SDB_DB	*ChildDb
)
{
	SDB_DB_SEARCH_PATH
			*SearchElem,
			**pSearch;


	if( !( ParentDb = DbToPhysicalDb( ParentDb )) || !ChildDb )
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "() Null Parent/Child Db" );


/*
**	Delete child db
*/

	for( pSearch = &ParentDb->SecDbSearchPath; *pSearch; pSearch = &(*pSearch)->Next )
		if( (*pSearch)->Db == ChildDb )
		{
			SearchElem = *pSearch;
			*pSearch = SearchElem->Next;
			free( SearchElem );

			if( ParentDb->SecDbSearchPath )
			{
				// Check if myself is last item in path, then just eliminate whole list
				if( !ParentDb->SecDbSearchPath->Next && ParentDb->SecDbSearchPath->Db == ParentDb )
					if( !SecDbSearchPathDelete( ParentDb, ParentDb ))
						return ErrMore( ARGCHECK_FUNCTION_NAME );
			}

			ChildDb->ParentDb = ChildDb;
			if( !SecDbDetach( ChildDb ))
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to detach from child db" );

			return( TRUE );
		}

	return Err( ERR_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s, %s ) @: %s", ParentDb->DbName, ChildDb->DbName, ChildDb->DbName );
}



/****************************************************************
**	Routine: SecDbSearchPathSet
**	Returns: Allocated, negative-one-terminated list, of db ids
**			 NULL for error
**	Action : Sets the search path for the given database.
**			 (Does not assume that the it needs to add itself,
**			 i.e., typically first db in search path is ParentDb
**			 itself.)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbSearchPathSet"

SDB_DB **SecDbSearchPathSet(
	SDB_DB		*ParentDb,
	const char	**DbNames
)
{
	const char
			**ChildName;

	SDB_DB
			**OldDbs,
			**pDb;


/*
**	Remember current search path, then remove it entirely
*/

	if( !( ParentDb = DbToPhysicalDb( ParentDb )) )
		return (SDB_DB**) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "() Null Parent Db" );
	
	OldDbs = SecDbSearchPathGet( ParentDb );
	if( !OldDbs )
		return (SDB_DB **) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get old search path" );

	while( ParentDb->SecDbSearchPath )
		if( !SecDbSearchPathDelete( ParentDb, ParentDb->SecDbSearchPath->Db ))
		{
			free( OldDbs );
			return (SDB_DB **) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get delete search path" );
		}


/*
**	If set to just myself, no need for all the machinery
*/

	if( DbNames[ 0 ] && !DbNames[ 1 ] && 0 == stricmp( DbNames[ 0 ], ParentDb->DbName ))
	{
		free( OldDbs );
		return( SecDbSearchPathGet( ParentDb ));
	}


/*
**	Append each name in search list
*/

	for( ChildName = DbNames; *ChildName; ChildName++ )
	{
		// Call SearchPathAppend which will do the attach and eliminate duplicates
		if( !SearchPathAppend( ParentDb, *ChildName ))
			goto BACKOUT;
	}
	free( OldDbs );
	return( SecDbSearchPathGet( ParentDb ));


/*
**	On error, attempt to restore the original search path
*/

BACKOUT:
	ErrPush();
	while( ParentDb->SecDbSearchPath )
		if( !SecDbSearchPathDelete( ParentDb, ParentDb->SecDbSearchPath->Db ))
			break;
	if( !( OldDbs[ 0 ] == ParentDb && !OldDbs[ 1 ] ))
		for( pDb = OldDbs; *pDb; pDb++ )
			SearchPathAppend( ParentDb, (*pDb)->FullDbName.c_str() );
	free( OldDbs );
	ErrPop();
	return( NULL );
}



/****************************************************************
**	Routine: SecDbSearchPathGet
**	Returns: Allocated, negative-one-terminated list, of db ids
**			 NULL for error
**	Action : Retrieves the current search path for the given db
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbSearchPathGet"

SDB_DB **SecDbSearchPathGet(
	SDB_DB	*ParentDb
)
{
	SDB_DB	**RetDbs,
			**pRetDb;

	int		Size;

	SDB_DB_SEARCH_PATH
			*SearchElem;


	if( !( ParentDb = DbToPhysicalDb( ParentDb )) )
		return (SDB_DB **) ErrN( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "( NULL )" );

	ParentDb = ParentDb->ParentDb;
	if( !ParentDb->SecDbSearchPath )
	{
		if( !ERR_CALLOC( RetDbs, SDB_DB *, 2, sizeof( SDB_DB * )))
			return (SDB_DB **) ErrN( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );
		RetDbs[ 0 ] = ParentDb;
		RetDbs[ 1 ] = NULL;
		return( RetDbs );
	}

	Size = 0;
	for( SearchElem = ParentDb->SecDbSearchPath; SearchElem; SearchElem = SearchElem->Next )
		Size++;

	if( !ERR_CALLOC( RetDbs, SDB_DB *, Size + 1, sizeof( SDB_DB * )))
		return (SDB_DB **) ErrN( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );
	pRetDb = RetDbs;

	for( SearchElem = ParentDb->SecDbSearchPath; SearchElem; SearchElem = SearchElem->Next )
		*pRetDb++ = SearchElem->Db;
	*pRetDb = NULL;
	return( RetDbs );
}



/****************************************************************
**	Routine: GetTypeToSelectType
**	Returns: SELECT_GREATER or SELECT_LOWEST
**	Action : gets the appropriate SELECT Type based on the GetType
**           (SDB_GET_LAST, SDB_GET_LESS, etc.)
****************************************************************/

static int MapGetTypeToSelectType(
	int	GetType 
)
{
	int SelectType;

	switch( GetType ) {
		case SDB_GET_LAST:
		case SDB_GET_LESS:
		case SDB_GET_LE:
		case SDB_GET_PREV:
			SelectType = SELECT_GREATEST;
			break;

		default:
			SelectType = SELECT_LOWEST;
			break;
	}
	
	return( SelectType );
}



/****************************************************************
**	Routine: SecNameEntriesMerge
**	Returns: int (number of items in merged list)
**	Action : Merges the contents of 2 sorted SDB_SEC_NAME_ENTRY
**           arrays into the result array of given size
**
**           if the SelectType is SELECT_LOWEST, it will merge the
**           arrays such that the lowest members are chosen. SELECT_GREATEST
**           merges so that the greatest members are chosen.
****************************************************************/

static int SecNameEntriesMerge(
	SDB_SEC_NAME_ENTRY	*SecNamesA,		// first array to merge
	int					SizeA,			// number of items in SecNamesA
	SDB_SEC_NAME_ENTRY	*SecNamesB,		// second array to merge
	int					SizeB,			// number of items in SecNamesB
	SDB_SEC_NAME_ENTRY	*Result,		// output param. Result of merge
	int					ResultSize,		// max size 
	int					SelectType		// SELECT_GREATEST, SELECT_LOWEST
)
{
	
	int		MaxSize = ResultSize,
			CurrA,
			CurrB,
			Total;

	char	*PickedItem;

	
	// pick the items from each list in alphabetical order, discard losers
	CurrA = CurrB = Total = 0;
	while( ( Total < MaxSize ) && ( CurrA < SizeA || CurrB < SizeB ))
	{
		if( CurrA >= SizeA )
			PickedItem = (char *) SecNamesB[ CurrB++ ];
		else if( CurrB >= SizeB )
			PickedItem = (char *) SecNamesA[ CurrA++ ];
		else if( stricmp( SecNamesA[ CurrA ], SecNamesB[ CurrB ] ) > 0 )
		{
            if( SelectType == SELECT_GREATEST )
				PickedItem = (char *) SecNamesA[ CurrA++ ];
			else
				PickedItem = (char *) SecNamesB[ CurrB++ ];
		}
		else if( SelectType == SELECT_LOWEST )
            PickedItem = (char *) SecNamesA[ CurrA++ ];
		else
			PickedItem = (char *) SecNamesB[ CurrB++ ];

		strcpy( Result[ Total++ ], PickedItem );
	}

	return( Total );
}



/****************************************************************
**	Routine: SecDbNameLookupManySecTypes
**	Returns: TRUE	- Next name gotten without error
**			 FALSE	- No more names
**	Summary: Lookup a set of security name using search criteria
**	Action : Get a list of security names that match the lookup
**			 criteria
**	SeeAlso: SecDbNameLookup, SecDbNameLookupMany
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNameLookupManySecTypes"

int SecDbNameLookupManySecTypes(
	const SDB_SEC_NAME	*SecName,	// Starting name
	SDB_SEC_NAME_ENTRY	*SecNames,	// Returned table of names
	int					TableSize,	// Max number of table entries
	SDB_SEC_TYPE		*SecTypes,	// Array of Security types
	int					NumSecTypes,// Number of Security Types
	int					GetType,	// Type of get operation
	SDB_DB				*pDb 		// Database to use
)
{
	int		CurrSecType,
			NameCount,
			MergeCount,
			CurrName,
			SelectType;

	SDB_SEC_NAME_ENTRY
			*Merged,
			*NamesForType;
	

	// if only one sec type, just call SecDbNameLookupMany (fewer copies)
	if( NumSecTypes < 2 )
		return( SecDbNameLookupMany( SecName, SecNames, TableSize, SecTypes[ 0 ], GetType, pDb ));

	if( !ERR_CALLOC( Merged, SDB_SEC_NAME_ENTRY, TableSize, sizeof( SDB_SEC_NAME_ENTRY )))
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	if( !ERR_CALLOC( NamesForType, SDB_SEC_NAME_ENTRY, TableSize, sizeof( SDB_SEC_NAME_ENTRY )))
	{
		free( Merged );
		return ErrMore( ARGCHECK_FUNCTION_NAME );
	}

	// figure out if we want the greatest or lowest values
	SelectType = MapGetTypeToSelectType( GetType );

	// get the list for each security types and merge SecNames
	MergeCount = 0;
	for( CurrSecType = 0; CurrSecType < NumSecTypes; CurrSecType++ )
	{
		NameCount = SecDbNameLookupMany( SecName, NamesForType, TableSize, SecTypes[ CurrSecType ], GetType, pDb );

		// merge with the previous 
		if( NameCount )
		{
			MergeCount = SecNameEntriesMerge( SecNames, MergeCount, NamesForType, NameCount, Merged, TableSize, SelectType );

			// copy the merged names to the SecNames array
			for( CurrName = 0; CurrName < MergeCount; CurrName++ )
				strcpy( SecNames[ CurrName ], Merged[ CurrName ] );
		}
	}

	free( Merged );
	free( NamesForType );

	return( MergeCount );
}



/****************************************************************
**	Routine: SecDbNameLookupSecTypes
**	Returns: TRUE	- Next name gotten without error
**			 FALSE	- No more names
**	Summary: Lookup a security name using search criteria
**	Action : Get a list of security names that match the lookup
**			 criteria
**	SeeAlso: SecDbNameLookup, SecDbNameLookupMany
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbNameLookupSecTypes"

int SecDbNameLookupSecTypes(
	SDB_SEC_NAME		*SecName,	// Starting name
	SDB_SEC_TYPE		*SecTypes,	// Array of Security types
	int					NumSecTypes,// Number of Security Types
	int					GetType,	// Type of get operation
	SDB_DB				*Db 		// Database to use
)
{
	int		CurrSecType,
			NameCount,
			SelectType,
			Replace,
			NameFound;

	SDB_SEC_NAME_ENTRY
			Candidate = "",
			SearchString;


	if( NumSecTypes < 2 )
		return( SecDbNameLookup( SecName, SecTypes[ 0 ], GetType, Db ));

	// figure out if we want the greatest or lowest value
	SelectType = MapGetTypeToSelectType( GetType );

	// get the name for each security types
	NameFound = FALSE;
	for( CurrSecType = 0; CurrSecType < NumSecTypes; CurrSecType++ )
	{
		// reset the search string to the original input
		strncpy( SearchString, SecName, sizeof( SearchString ) - 1 );
		SearchString[ sizeof( SearchString ) - 1 ] = '\0';

		NameCount = SecDbNameLookup( SearchString, SecTypes[ CurrSecType ], GetType, Db );

		if( !NameCount )
			continue;
		
		// figure out if we should replace the SecName from the previous class
		Replace = FALSE;
		if( '\0' == *Candidate )
		{
			// first class, always replace
			Replace = TRUE;
		}
		else if( stricmp( Candidate, SearchString ) < 0 )
		{
			if( SelectType == SELECT_GREATEST )
				Replace = TRUE;
		}
		else if( SelectType == SELECT_LOWEST )
			Replace = TRUE;

		if( Replace )
		{
			strncpy( Candidate, SearchString, sizeof( Candidate ) - 1 );
			Candidate[ sizeof( Candidate ) - 1 ] = '\0';

			NameFound = TRUE;
		}
	}

	if( NameFound )
		strcpy( SecName, Candidate );

	return( NameFound );
}



/****************************************************************
**	Routine: SecDbLookupDb
**	Returns: SDB_DB * / NULL
**	Action : Looks up a Db by an ID
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbLookupDb"

SDB_DB *SecDbLookupDb(
	SDB_DB_ID	DbId
)
{
	return (SDB_DB *) HashLookup( SecDbDatabaseIDHash, (HASH_KEY) (long) DbId );
}

/****************************************************************
**	Routine: SecDbGetLoadHistory
**	Returns: 
**	Action : 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbGetLoadHistory"

DT_VALUE* SecDbGetLoadHistory(
)
{
	return GetByNameHistory;
}


/****************************************************************
**	Routine: SecDbStoredObjectFromStructure
**	Returns: true/error
**	Action : converts a structure to a SDB_STORED_OBJECT
****************************************************************/

bool SecDbStoredObjectFromStructure(
	SDB_STORED_OBJECT* obj,
	DT_VALUE *val
)
{
	DT_VALUE* sdb_disk = DtComponentGet( val, "SdbDisk", 0 );
	if( !sdb_disk )
		return ErrMore( "Expected SdbDisk" );
	char const* type = (char const*) DtComponentGetPointer( sdb_disk, "Type", DtString, 0 );
	if( !type )
		return ErrMore( "Expected SdbDisk.Type" );

	memset( obj, 0, sizeof( *obj ) );
	if( !( obj->SdbDisk.Type = SecDbClassTypeFromName( type ) ) )
		return ErrMore( "No such class type '%s'", type );

	char const* name = (char const*) DtComponentGetPointer( sdb_disk, "Name", DtString, 0 );
	if( name )
		strcpy( obj->SdbDisk.Name, name );
	obj->SdbDisk.LastTransaction = (SDB_TRANS_ID) DtComponentGetNumber( sdb_disk, "LastTransaction", DtDouble, -1 );

	SDB_DISK_VERSION version;
	version.Stream = (int) DtComponentGetNumber( sdb_disk, "StreamVersion", DtDouble, 0. );
	version.Object = (int) DtComponentGetNumber( sdb_disk, "ObjectVersion", DtDouble, 0. );
	if( !SecDbDiskInfoSetVersion( &obj->SdbDisk, &version ) )
		return ErrMore( "Failed to set version: {stream: %d, object: %d}", version.Stream, version.Object );

	DT_BINARY* binary = (DT_BINARY*) DtComponentGetPointer( val, "Mem", DtBinary, 0 );
	if( !binary )
	{
		obj->MemSize = 0;
		obj->Mem = 0;
	}
	else
	{
		obj->MemSize = binary->Size;
		obj->Mem = malloc( binary->Size );
		memcpy( obj->Mem, binary->Memory, binary->Size );
	}
	return true;
}
