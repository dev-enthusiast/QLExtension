#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_def.c,v 1.81 2015/02/25 21:22:16 c038571 Exp $"
/****************************************************************
**
**	SDB_DEF.C	- Default SecDb connect and disconnect functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_def.c,v $
**	Revision 1.81  2015/02/25 21:22:16  c038571
**	Updated revision 1.80.
**
**	Revision 1.80  2015/02/19 21:40:31  c038571
**	AWR: #363400. Make sure the OpenCount is positive before calling server.
**
**	Revision 1.79  2013/08/14 02:53:17  khodod
**	Make sure we don't crash on shutdown, if the function is somehow called
**	twice. This is the sporadic issue we are seeing in ntplot sessions.
**	#310819
**
**	Revision 1.78  2003/12/23 19:29:48  khodod
**	BugFix: Use the thread-safe time api routines (iBug #9277).
**
**	Revision 1.77  2001/10/04 19:45:59  colemje
**	Fix typo in error message
**	
**	Revision 1.76  2001/08/20 20:11:40  simpsk
**	remove GsBool.
**	
**	Revision 1.75  2001/08/13 14:21:02  singhki
**	back out expiry stuff
**	
**	Revision 1.74  2001/06/29 15:22:13  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.73  2001/03/13 18:18:47  goodfj
**	Path arg to SecDbLoadValueTypeRegistryTable
**	
**	Revision 1.72  2001/03/13 17:39:01  goodfj
**	Added SecDbLoadValueTypeRegistry
**	
**	Revision 1.71  2000/12/05 12:37:37  goodfj
**	In SecDbStartup, load config files only once, to allow successful subsequent call if we just fail to attach to startup db
**	
**	Revision 1.70  2000/12/05 12:17:47  goodfj
**	More informative error messages
**	
**	Revision 1.69  2000/05/17 23:57:47  singhki
**	use code_ver instead of compile date because compile date does not invalidate
**	
**	Revision 1.68  2000/05/17 23:37:21  singhki
**	Expire SecDbStartup 60 days after it is compiled
**	
**	Revision 1.67  2000/04/24 11:04:21  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.66  2000/02/08 04:50:23  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.65  2000/02/03 01:18:04  loflij
**	Fix NullDb condition if DbName is NULL
**	
**	Revision 1.64  1999/11/20 16:40:05  gribbg
**	Add required/optional flag to secinit.dat
**	
**	Revision 1.63  1999/11/17 23:42:06  davidal
**	Added two new attributes for VT as follows:
**	- SDB_RELEASE_VALUE;
**	- SDB_RELEASE_VALUE_WHEN_PARENTS_VALID.
**	Once set for the VT, the former will cause an immediate release of the node's value after it gets consumed
**	by a parent; the latter will do the same after *all* the parents are valid.
**	By default, both the attributes are set to FALSE; however, defining the env variable
**	SECDB_RELEASE_VALUE_WHEN_PARENTS_VALID as TRUE will set SDB_RELEASE_VALUE_WHEN_PARENTS_VALID for all
**	non-terminal nodes.
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<string.h>
#include	<kool_ade.h>
#include	<mempool.h>
#include	<dynalink.h>
#include	<secdb.h>
#include	<secexpr.h>
#include	<secindex.h>
#include	<secdrive.h>
#include	<secdrv.h>
#include	<sdb_int.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secobj.h>
#include	<timezone.h>
#include	<code_ver.h>
#include	<sdb_meta.h>


CONFIGURATION
			*SecDbConfiguration;

static SDB_DB
		*StartupDb = NULL;

extern HASH
		*SlangRootHash,
		*SlangFunctionHash,
		*SlangConstantsHash,
		*SlangEnumsHash,
		*SlangNodeTypeHash,
		*SlangNodesByType,
		*SlangNodesByName,
		*SlangVarScopeHash;

extern HEAP
		*SlangVariableHeap;



/****************************************************************
**	Routine: SecDbLoadInitTable
**	Returns: TRUE or Err()
**	Action : Load secinit.dat and call each init function
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbLoadInitTable"

static int SecDbLoadInitTable(
	const char	*Path,
	const char	*TableName
)
{
#define MAX_TOKENS	10
	char    *Tokens[ MAX_TOKENS+1 ];

	char	Buffer[ 256 ],
			FileName[ FILENAME_MAX + 1 ],
			*DllName,
			*FuncName,
			*Flag;

	FILE	*fpTable;

	SDB_INIT_FUNC
			InitFunc;

	int		Optional,
			Found;


	// Open the ASCII file containing the table
	if( !KASearchPath( Path, TableName, FileName, sizeof( FileName )))
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	if( !(fpTable = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) %s", FileName, strerror( errno ) );


	// Read and parse the file
	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		Found = StrTokenize( Buffer, MAX_TOKENS, Tokens );
		if( Found == 0 )
			continue;	// Blank line or only comments

		if( Found != 3 )
		{
			Found = -1;
			Err( ERR_FILE_INVALID, "Expected DllName, FuncName, Flag" );
		}
		if( Found < 0 )
		{
			fclose( fpTable );
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ), error parsing file", FileName );
		}

		DllName  = Tokens[0];
		FuncName = Tokens[1];
		Flag     = Tokens[2];

		if( 0 == stricmp( Flag, "Optional" ))
			Optional = TRUE;
		else if( 0 == stricmp( Flag, "Required" ))
			Optional = FALSE;
		else
		{
			fclose( fpTable );
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ), flag must be 'Optional' or 'Required', not '%s'", FileName, Flag );
		}

		// Call the init function
		if( !(InitFunc = (SDB_INIT_FUNC) DynaLink( DllName, FuncName )))
		{
			if( Optional )
				continue;
			fclose( fpTable );
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Can't find function %s", TableName, FuncName );
		}

		if( !(*InitFunc)() )
		{
			fclose( fpTable );
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed calling %s (from %s)", TableName, FuncName, DllName );
		}
	}

	// Close the file
	fclose( fpTable );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbLoadSlangMore
**	Returns: TRUE or Err()
**	Action : Load slangmor.dat and each underlying slang.dat file
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbLoadSlangMore"

static int SecDbLoadSlangMore(
	const char	*Path,
	const char	*TableName
)
{
	char	Buffer[ 256 ],
			FileName[ FILENAME_MAX + 1 ],
			*OptionStr,
			*DatName;

	FILE	*fpTable;

	bool	Optional;


	// Open the ASCII file containing the table
	if( !KASearchPath( Path, TableName, FileName, sizeof( FileName )))
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	if( !(fpTable = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) %s", FileName, strerror( errno ) );


	// Read and parse the file
	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		if( Buffer[ 0 ] == '\n' )
			continue;

		if( Buffer[ 0 ] == '/' && Buffer[ 1 ] == '/' )
			continue;

		// Dat name
		if( !(DatName = strtok( Buffer, " \t\n" )))
			continue;

		// Required/Optional
		if( !(OptionStr = strtok( NULL, " \t\n" )))
			continue;

		if( 0 == stricmp( OptionStr, "Required" ))
			Optional = FALSE;
		else if( 0 == stricmp( OptionStr, "Optional" ))
			Optional = TRUE;
		else
		{
			fclose( fpTable );
			return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): OptionStr '%s' not known",
						FileName, OptionStr );
		}


		if( !SlangFunctionTableLoad( SecDbTypesPath, DatName, Optional ))
		{
			fclose( fpTable );
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ): Failed loading '%s'", FileName, DatName );
		}
	}

	// Close the file
	fclose( fpTable );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbStartupAttach
**	Returns: SDB_DB *
**	Action : Attach to a database by name, handling NULL/None/
**			 Only called from within SecDbStartup
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbStartupAttach"

static SDB_DB *SecDbStartupAttach(
	const char	*DbName,
	int			DeadPoolSize
)
{
	int		DbFlags = SDB_FRONT;

	SDB_DB	*Db = SecDbRootDb;

	const char  *ReadOnly;


/*
**	Attach to the database
*/

	if( !DbName && !(DbName = ConfigurationGet( SecDbConfiguration, "SECDB_DATABASE" )))
	{
		if( ErrNum != ERR_NOT_FOUND )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get default db name" );
		DbName = "DEFAULT";
		ConfigurationSet( SecDbConfiguration, "SECDB_DATABASE", DbName );
	}
	ReadOnly = ConfigurationGet( SecDbConfiguration, "SECDB_DATABASE_READONLY" );
	if( ReadOnly && *ReadOnly ) 
	{
		if( 0 == stricmp( ReadOnly, "FALSE")) 
			DbFlags |= SDB_READWRITE;
		else
			DbFlags |= SDB_READONLY;
	}
	if( DbName )
		Db = SecDbAttach( DbName, DeadPoolSize, DbFlags );
	if( !Db )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to startup db" );
	return Db;
}



/****************************************************************
**	Routine: SecDbIsExpired
**	Returns: true/false
**	Action : If more than two months have elapsed since this code
**           was compiled, return true and set the corresponding
**           error
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIsExpired"

static bool SecDbIsExpired(
)
{
	time_t 
        now = time( NULL );

	struct tm
			t;

	if( now == -1 || !localtime_r( &now, &t ))
	{
		Err( ERR_UNKNOWN, __FILE__ ": Software Bug! Cannot get current time" );
		return true; // expires if there is an error in parsing the time
	}

	t.tm_year = (int)CodeVersion / 10000 - 1900;
	t.tm_mon = ( (int) CodeVersion / 100 ) % 100;
	t.tm_mday = ( (int) CodeVersion % 100 );

	time_t version = mktime( &t );
	if( version == -1 )
	{
		Err( ERR_UNKNOWN, __FILE__ ": Software Bug! Cannot parse code version" );
		return true; // expires if there is an error in parsing the time
	}

	if( now - version > 3600*24*90 )
	{
		Err( ERR_VERSION_MISMATCH, "SecDb dlls are too old. They were compiled on %g which is more than 90 days ago. SecDb will not start!", CodeVersion );
		return true;
	}
	return false;
}



/****************************************************************
**	Routine: SecDbStartup
**	Returns: SDB_DB * of attached database if successful
**			 NULL if an error occured during startup
**	Summary: Standard method of starting the security database system
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbStartup"

SDB_DB *SecDbStartup(
	const SDB_FULL_DB_NAME		*DbName,		// Database name (NULL => use default)
	int							DeadPoolSize,	// Size of the deadpool
	SDB_MSG_FUNC				MsgFunc,	 	// Function used to display messages
	SDB_MSG_FUNC				ErrMsgFunc,	 	// Function used to display error messages
	const char					*AppName,		// Application name (use leading '-' to not change default)
	const char					*UserName		// User name
)
{
	const char
			*DbTable,
			*DeadPoolString,
			*Path;

	int		Len;

	static int
			ConfigLoaded = FALSE;

/*
**	If SecDb has already been started, only call SecDbInitialize to set the
**	message functions, and attach to the named database.
*/

	if( StartupDb )
	{
		int		NullDb = !DbName || ( DbName && (	0 == stricmp( DbName, "none" )
													||	0 == stricmp( DbName, "null" ) ));


		// Already initialized, don't set the application name
		if( *AppName == '-' )
			AppName = NULL;
		if( !SecDbInitialize( MsgFunc, ErrMsgFunc, AppName, UserName ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to initialize secdb" );

		DeadPoolString = ConfigurationGet( SecDbConfiguration, "SECDB_DEADPOOL_SIZE" );
		if( DeadPoolString )
			DeadPoolSize = atoi( DeadPoolString );

		if( !NullDb )
			StartupDb = SecDbStartupAttach( DbName, DeadPoolSize );
		if( !StartupDb )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to startup db" );
		return StartupDb;
	}

	// if this dll is expired, refuse to start
//	if( SecDbIsExpired() )
//		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Secdb is expired" );

	if( !ConfigLoaded )
	{
/*
**	Read the configuration information
*/

		SecDbConfiguration = ConfigurationLoad( "secdb.dat", NULL, NULL );
		if( !SecDbConfiguration )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load secdb.dat" );


/*
**	Get the path(s)
*/

		Path = ConfigurationGet( SecDbConfiguration, "SECDB_PATH" );
		if( Path )
		{
			strcpy( SecDbPath, Path );
			Len = strlen( SecDbPath );
			if( SecDbPath[ Len - 1 ] != ':' && SecDbPath[ Len - 1 ] != '\\' && SecDbPath[ Len - 1 ] != '/' )
			{
				SecDbPath[ Len++ ] = '/';
				SecDbPath[ Len ] = '\0';
			}
		}
		else if( ErrNum != ERR_NOT_FOUND )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get SECDB_PATH" );
		else
			*SecDbPath = '\0';
		ConfigurationSet( SecDbConfiguration, "SECDB_PATH", SecDbPath );


		Path = ConfigurationGet( SecDbConfiguration, "SECDB_TYPES" );
		if( Path )
		{
			strcpy( SecDbTypesPath, Path );
			Len = strlen( SecDbTypesPath );
			if( SecDbTypesPath[ Len - 1 ] != ':' && SecDbTypesPath[ Len - 1 ] != '\\' && SecDbTypesPath[ Len - 1 ] != '/' )
			{
				SecDbTypesPath[ Len++ ] = '/';
				SecDbTypesPath[ Len ] = '\0';
			}
		}
		else if( ErrNum != ERR_NOT_FOUND )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get SECDB_TYPES" );
		else
			strcpy( SecDbTypesPath, SecDbPath );
		ConfigurationSet( SecDbConfiguration, "SECDB_TYPES", SecDbTypesPath );


/*
**	Initialize the security database system
*/

		// Not yet initialized, set the application name
		if( *AppName == '-' )
			++AppName;
		if( !SecDbInitialize( MsgFunc, ErrMsgFunc, AppName, UserName ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to initialize secdb" );


/*
**	Initialize slang
*/

		SlangInitialize();
		SecDbErrConstants();


/*
**	Load the security object table
*/

		if( !SecDbLoadObjTable( SecDbTypesPath, "sectypes.dat" ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load sectypes.dat" );


/*
**	Load the 'server' object table
*/

		if( !SecDbDriverStartup( SecDbTypesPath, "drvfunc.dat" ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load drvfunc.dat" );


/*
**	Find the database attributes
*/

		SecDbConfigAttributes = 0;
		Path = ConfigurationGet( SecDbConfiguration, "SECDB_PRODUCTION_ENVIRONMENT" );
		if( Path )
		{
			if(	stricmp( Path, "TRUE" ) == 0 )
				SecDbConfigAttributes |= SDB_DB_ATTRIB_PROD;
		}
		else if( ErrNum != ERR_NOT_FOUND )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to ascertain production environment" );


/*
**	Load the database table
*/

		if( !ConfigurationGet( SecDbConfiguration, "SECDB_DATABASE_TABLE" ))
			ConfigurationSet( SecDbConfiguration, "SECDB_DATABASE_TABLE", "database.dat" );
		DbTable = ConfigurationGet( SecDbConfiguration, "SECDB_DATABASE_TABLE" );

		DeadPoolString = ConfigurationGet( SecDbConfiguration, "SECDB_DEADPOOL_SIZE" );
		if( DeadPoolString )
			DeadPoolSize = atoi( DeadPoolString );


/*
**	Determine location of 'unique ID' file
*/

		if( !(SecDbUniqueIDFileName = ConfigurationGet( SecDbConfiguration, "SECDB_UNIQUE_FILE" )))
		{
			if( ErrNum != ERR_NOT_FOUND )
				return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get unique file" );
			SecDbUniqueIDFileName = "secdbunq.dat";
			ConfigurationSet( SecDbConfiguration, "SECDB_UNIQUE_FILE", SecDbUniqueIDFileName );
		}


/*
**	Load the slang function and operator table
*/

		if( !SlangFunctionTableLoad( SecDbTypesPath, "slang.dat", FALSE ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load slang.dat" );
	
		if( !SlangNodeFuncTableLoad( SecDbTypesPath, "slangops.dat" ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load slangops.dat" );


/*
**	Load database.dat and attach
*/

		if( !SecDbLoadDatabaseTable( SecDbTypesPath, DbTable ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load database table" );

/*
**	Load info from valuetype registry (metadb)
*/

		if( !SecDbLoadValueTypeRegistryTable( SecDbTypesPath ) )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load valuetype registry" );

		ConfigLoaded = TRUE;
	}

	StartupDb = SecDbStartupAttach( DbName, DeadPoolSize );
	if( !StartupDb )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to startup db" );

	// Do post initialization
	if( !SDB_NODE::InitializeMore() )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to initialize SDB_NODEs" );


/*
**	Other initialization 
*/

	if( !SlangFunctionTableLoad( SecDbTypesPath, "x_squant.dat", FALSE ))
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load x_squant.dat" );
	
	if( !SlangFunctionTableLoad( SecDbTypesPath, "tsdb.dat", FALSE ))
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load tsdb.dat" );

	if( !SecDbLoadSlangMore( SecDbTypesPath, "slangmor.dat" ))
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load slangmor.dat" );

	if( !SecDbLoadInitTable( SecDbTypesPath, "secinit.dat" ))
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to load secinit.dat" );

	return StartupDb;
}



/****************************************************************
**	Routine: SecDbShutdown
**	Returns: Nothing
**	Summary: Shut down the security database system
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbShutdown"

void SecDbShutdown(
	int		FastAndLoose	// If FALSE, releases most of the SecDb resources
)
{
	SDB_DB	*pDb,
            *SecDb;

	HASH_ENTRY_PTR
			HashPtr;

	SDB_CLASS_INFO
			*ClassInfo;

	SDB_INDEX
			*Index;

	SLANG_CONTEXT
			*Context;

	SLANG_FUNCTION_INFO
			*FunctionInfo;

	SLANG_ENUM
			*Enum;

	SLANG_CONSTANT
			*Constant;

	if( !SecDbInitialized )
		return;

	if( FastAndLoose )
	{
		HASH_FOR( HashPtr, SecDbDatabaseIDHash )
		{
			pDb = (SDB_DB *) HashValue( HashPtr );
			//AWR: #363400
			if( pDb->OpenCount > 0 && pDb->pfClose ) // FIX- virtual database don't have a close function
				(*pDb->pfClose)( pDb );
		}
		return;
	}

	SlangVariableDestroyByName( NULL, NULL );
	SecDbDetach( NULL );		// Detaches all databases
	ConfigurationUnload( SecDbConfiguration );

	HASH_FOR( HashPtr, SecDbSecTypeHash )
	{
		ClassInfo = (SDB_CLASS_INFO *) HashValue( HashPtr );
		SecDbClassUnregister( ClassInfo->Name );
	}
	HashDestroy( SecDbSecTypeHash );
	HASH_FOR( HashPtr, SecDbValueTypeHash )
		free( (void *) HashKey( HashPtr ));
	HashDestroy( SecDbValueTypeHash );

	HashDestroy( SecDbDatabaseIDHash );
	HASH_FOR( HashPtr, SecDbDatabaseHash )
	{
		SecDb = (SDB_DB *) HashValue( HashPtr );
		delete SecDb;
	}
	HashDestroy( SecDbDatabaseHash );
	HASH_FOR( HashPtr, SecDbIndexHash )
	{
		Index = (SDB_INDEX *) HashValue( HashPtr );
		free( Index->Classes );
		free( Index );
	}
	HashDestroy( SecDbIndexHash );
	free( SecDbDatabaseBuffer );

	HASH_FOR( HashPtr, SlangVarScopeHash )
	{
		SlangVariableScopeDestroy( (SLANG_VARIABLE_SCOPE *) HashValue( HashPtr ));
		free( (char *) HashKey( HashPtr ));
	}
	HashDestroy( SlangVarScopeHash );

	HashDestroy( SlangNodesByName );
	HashDestroy( SlangNodesByType );
	HashDestroy( SlangRootHash );
	HASH_FOR( HashPtr, SlangConstantsHash )
	{
		Constant = (SLANG_CONSTANT *) HashValue( HashPtr );
		free( Constant->Name );
		free( Constant->EnumName );
		free( Constant );
	}
	HashDestroy( SlangConstantsHash );
	HASH_FOR( HashPtr, SlangEnumsHash )
	{
		Enum = (SLANG_ENUM *) HashValue( HashPtr );
		free( Enum->EnumName );
		HashDestroy( Enum->Members );
		free( Enum );
	}
	HashDestroy( SlangEnumsHash );
	HASH_FOR( HashPtr, SlangNodeTypeHash )
	{
		FunctionInfo = (SLANG_FUNCTION_INFO *) HashValue( HashPtr );
		free( FunctionInfo->DllPath );
		free( FunctionInfo->FuncName );
		free( FunctionInfo );
	}
	HashDestroy( SlangNodeTypeHash );
	HASH_FOR( HashPtr, SlangFunctionHash )
	{
		FunctionInfo = (SLANG_FUNCTION_INFO *) HashValue( HashPtr );
		free( FunctionInfo->Name );
		free( FunctionInfo->DllPath );
		free( FunctionInfo->FuncName );
		free( FunctionInfo );
	}
	HashDestroy( SlangFunctionHash );
	while(( Context = SlangContext ))
	{
		SlangContext = Context->Next;
		MemPoolDestroy( Context->Hash->MemPool );
		free( Context );
	}
	GSHeapDestroy( SlangVariableHeap );

	SDB_NODE::Cleanup();
	StartupDb = NULL;
	SecDbInitialized = FALSE;
}



/****************************************************************
**	Routine: SecDbConfigurationGet
**	Returns: Static string of given config variable
**			 or Default on error or if not found
**	Action : Looks up config variable in given config file
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbConfigurationGet"

const char *SecDbConfigurationGet(
	const char	*ConfigVariable,	// Variable to look up
	const char	*ConfigFile,		// Optional config file (NULL means secdb.cfg)
	const char	*ConfigPath,		// Optional config path (NULL means CONFIG_PATH)
	const char	*Default			// What to return on error or if config var not found
)
{
	static HASH
			*ConfigRemember = NULL;

	const char	
			*Ret;

	char	*Remember;

	CONFIGURATION
			*Config;


	if( !ConfigFile || 0 == stricmp( ConfigFile, "secdb.cfg" ))
		Config = SecDbConfiguration;
	else 
	{
		Remember = StrPaste( ConfigFile, ConfigPath, NULL );
		if( !Remember )
			return Default;
		if( !ConfigRemember || !( Config = (CONFIGURATION *) HashLookup( ConfigRemember, Remember )))
		{
			// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
			if( !ConfigRemember )
				ConfigRemember = HashCreate( "ConfigRemember", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );
			if( !ConfigRemember )
				return Default;
			Config = ConfigurationLoad( ConfigFile, ConfigPath, NULL );
			if( !Config )
				return Default;
			if( !HashInsert( ConfigRemember, Remember, Config ))
				return Default;
		}
		else
			free( Remember );
	}

	Ret = ConfigurationGet( Config, ConfigVariable );
	return ( Ret ? Ret : Default );
}
