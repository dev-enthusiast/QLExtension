#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_db.c,v 1.164 2015/02/19 21:33:01 c038571 Exp $"
/****************************************************************
**
**	SDB_DB.C	- SecDb database functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_db.c,v $
**	Revision 1.164  2015/02/19 21:33:01  c038571
**	AWR: #363400. Make sure the OpenCount is positive before calling server.
**
**	Revision 1.163  2014/12/09 18:05:03  c038571
**	AWR: #352658. Bug fix updated in response to the code review.
**
**	Revision 1.162  2014/12/06 01:30:53  c038571
**	AWR: #352658. Propagate SDB_NO_SECURITY_DATABASE for database union.
**
**	Revision 1.161  2013/10/22 18:57:03  khodod
**	Fix bug where the first character was not checked for validity and
**	make sure the userdb prefix character is OK.
**	AWR: #315292
**
**	Revision 1.160  2013/10/14 04:33:44  khodod
**	The filter is too limited -- allow { and }.
**	AWR: #315292
**
**	Revision 1.159  2013/10/03 00:48:30  khodod
**	Adding validation to the SecDb API to prevent buffer overflows and crashes
**	for evaluating strings with embedded percent signs.
**	AWR: #315292
**
**	Revision 1.158  2011/11/28 00:29:29  khodod
**	Increment the ref count for system diddle scopes, so that they are
**	never accidentally deleted.
**	AWR: #177523
**
**	Revision 1.157  2011/10/27 20:12:24  c07426
**	Backing out a bad commit.
**	AWR: #229190
**
**	Revision 1.155  2011/02/15 17:07:16  khodod
**	Make sure to report correctly the name of the database table file we cannot open.
**	AWR: #222631
**
**	Revision 1.154  2007/09/26 15:57:00  khodod
**	Added logic to handle the ACL attribute for databases.
**	iBug: #49777
**	
**	Revision 1.153  2004/11/30 16:12:53  khodod
**	Added a missing include.
**	iBug: #16863
**	
**	Revision 1.152  2004/11/30 16:02:36  khodod
**	Fixed SecDbSetTimeout signature.
**	iBug: #16863
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<strings.h>		// for strcasecmp
#include	<errno.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<time.h>
#include	<kool_ade.h>
#include	<heapinfo.h>
#include	<code_ver.h>
#include	<hash.h>
#include	<dynalink.h>
#include	<secdb.h>
#include	<secindex.h>
#include	<secobj.h>
#include	<secerror.h>
#include	<secdrv.h>
#include	<sdbdbset.h>
#include	<sdb_int.h>
#include	<secnode.h>
#include	<memory>
#include	<string>

CC_USING( std::string );
CC_USING( std::auto_ptr );


/*
**	Prototype functions
*/

static void
		SecDbDbUpdateSet(	SDB_DB *pDb, SDB_DB *pDB ),
		DbToDbInfo(		SDB_DB *pDb, SDB_DB_INFO *DbInfo );

static int
		DbsCompareByMirrorGroup( const void *Ptr1, const void *Ptr2 );

static int
		ParseACL( SDB_DB *pDb, char *ACL, int *ActiveDb );

static int
		ParseACLAtom( char *Atom, const char *Group, int *PermsMask );

static SDB_DB *
        SecDbSystemDiddleScopeNew( const char *DiddleScopeName );

static HASHPAIR
		*DbsByMirrorGroup = NULL;

static int
		NumDbsByMirrorGroup = 0;


SDB_DB	
        *SecDbClassObjectsDb = NULL;



/****************************************************************
**	Routine: SetMaxMemSize
**	Returns: 
**	Action : Reduce MaxMemSize for subdbs, to allow for the object wrapper overhead
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SetMaxMemSize"

static bool SetMaxMemSize(
	SDB_DB* pDb
)
{
    //
    // local helper classes
    struct Err_Sentry
	{
	    Err_Sentry() { ErrPush();}
	    ~Err_Sentry() { ErrPop();}
	};

	struct DbFree_Sentry
	{
	    DbFree_Sentry( SDB_OBJECT*& ptr ) : m_ptr( ptr ) {}
	    ~DbFree_Sentry() { SecDbFree( m_ptr ); }
	private:
	    SDB_OBJECT*& m_ptr;
	};

	struct DbStream_Sentry
	{
	  DbStream_Sentry( SDB_M_DATA& data ) : m_data( data ) {}
	  ~DbStream_Sentry() { SecDbBStreamDestroy( m_data.Mem.Stream, true );}
	private:
	  SDB_M_DATA& m_data;
	};
	//
	//

	if( SDB_DB_SUBDB != pDb->DbOperator )
	{
		pDb->MaxMemSize = SDB_MAX_MEM_SIZE;
		return true;
	}

	static int SubDbOverhead = 0;

	if( SubDbOverhead )
	{
		pDb->MaxMemSize = SDB_MAX_MEM_SIZE - SubDbOverhead;
		return true;
	}

	Err_Sentry
	        Local_Err_Sentry;

	// Walk up to the root db
	SDB_DB* ParentDb = pDb;         // Need somewhere to create the empty wrapper object
	while( ParentDb->DbOperator == SDB_DB_SUBDB )
	    ParentDb = ParentDb->Left; 

	SDB_SEC_TYPE
	        WrapperType = SecDbClassTypeFromName( "SubDatabase Object" );

	if( !WrapperType )
	    return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get class type for subdb wrapper object" );

	// Create an empty wrapper object
	SDB_OBJECT*
	        WrapperPtr = SecDbNew( NULL, WrapperType, ParentDb );

	if( !WrapperPtr )
	    return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to create empty subdb wrapper object" );

	DbFree_Sentry
	        DbSentry( WrapperPtr ); // make sure this Db gets put back when we are done with it.

	// Set Object Name( WrapperPtr ) to its maximum size
	{
	    char    ObjectName[ SDB_SEC_NAME_SIZE ];

		for( unsigned i = 0; i < sizeof( ObjectName ) - 1; ++i )
		    ObjectName[ i ] = ' ';

		ObjectName[ sizeof( ObjectName ) - 1 ] = '\0';

		DT_VALUE
		        ObjectNameVal;

		DT_POINTER_TO_VALUE( &ObjectNameVal, ObjectName, DtString );

		if( !SecDbSetValue( WrapperPtr, SecDbValueTypeFromName( "Object Name", DtString ), &ObjectNameVal, 0 ) )
		    return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to set Object Name( empty subdb wrapper object )" );
	}
	
	// Now ask the object to stream itself
	{
	    SDB_M_DATA
		        MsgDataMem;

		DbStream_Sentry
		       Msg_Sentry( MsgDataMem ); // make sure any resources allocated for MsgDataMem are freed

		if( !SecDbMsgMem( WrapperPtr, &MsgDataMem ) )
		    return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get mem for empty subdb wrapper object" );

		// SubDbOverhead is the size of the bstream
		SubDbOverhead = BStreamDataUsed( MsgDataMem.Mem.Stream );
	}

	pDb->MaxMemSize = SDB_MAX_MEM_SIZE - SubDbOverhead;
	return true;
}


/****************************************************************
**	Routine: SecDbDbRegister
**	Returns: pDb or NULL
**	Action : Adds database to list of databases
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDbRegister"

SDB_DB *SecDbDbRegister(
	SDB_DB	*SecDb
)
{
	SDB_DB  *Db;

	SDB_DB_ID
			DbId;

	string full_db_name;

	auto_ptr<SDB_DB> db_guard( SecDb );

	// generate the fully qualified database name
	if( SecDb->DbID == SDB_DB_VIRTUAL && SecDb->DbOperator != SDB_DB_DIDDLE_SCOPE )
	{
		if( !SecDb->Right )
		{
			if( SecDb->DbOperator == SDB_DB_SUBDB )
				full_db_name = SecDb->Left->FullDbName + SDB_SUBDB_DELIMITER_STRING + SecDb->Qualifier;
			else if( SecDb->DbOperator == SDB_DB_ALIAS )
				full_db_name = SecDb->Left->FullDbName;
			else if( !SecDb->Qualifier.empty() )
				full_db_name = SecDb->Qualifier + SDB_QUALIFIER_OPEN_STRING + SecDb->Left->FullDbName + SDB_QUALIFIER_CLOSE_STRING;
			else
				full_db_name = SecDb->Left->FullDbName;
		}
		else
		{
			if( !SecDb->Qualifier.empty() )
				full_db_name =
						SecDb->Qualifier
						+ "(" + SecDb->Left->FullDbName
						+       SDB_SEARCH_DELIMITER_STRING
						+       SecDb->Right->FullDbName
						+ ")";
			else
				full_db_name = SecDb->Left->FullDbName + SDB_SEARCH_DELIMITER_STRING + SecDb->Right->FullDbName;;
		}
	}
	else
	{
 		if( SDB_SUBDB_DELIMITER == SecDb->FullDbName[0] )
 			full_db_name = SecDb->FullDbName;
 		else
 	  		full_db_name = SDB_SUBDB_DELIMITER_STRING + SecDb->FullDbName;
	}

	if( SDB_DB_CLASSFILTER == SecDb->DbOperator )
	{
		HASH_ENTRY_PTR
				HashPtr;

		int		Count = 0;


		HASH_FOR( HashPtr, SecDb->FilteredClasses )
		{
			if( 0 == Count++ )
			{
				full_db_name += SDB_CLASSFILTER_PREFIX_STRING;
				full_db_name += (char*) HashValue( HashPtr );
			}
			else
			{
				full_db_name += SDB_CLASSFILTER_DELIMITER_STRING;
				full_db_name += (char*) HashValue( HashPtr );
			}
		}
	}

	if(( Db = (SDB_DB *) HashLookup( SecDbDatabaseHash, full_db_name.c_str() )))
	{
		if( Db->DbOperator != SDB_DB_LEAF )
		{
/*
**	We're discarding SecDb, so need to detach from child dbs too, to avoid db ref leaks
*/
			if( SecDb->Left )
				SecDbDetach( SecDb->Left );
			if( SecDb->Right )
				SecDbDetach( SecDb->Right );
			return Db;
		}
		Err( ERR_DATABASE_INVALID, "Duplicate Database name: %s", SecDb->FullDbName.c_str() );
		return NULL;
	}

	DbId = SecDb->DbID;
	if( DbId == -1 )
	{
		Err( ERR_DATABASE_INVALID, "%s: Illegal Database Id, -1", SecDb->FullDbName.c_str() );
		return NULL;
	}

	if( DbId == SDB_DB_ID_ALIAS || DbId == SDB_DB_VIRTUAL )
		DbId = SDB_DB_VIRTUAL;
	else if( DbId > SDB_DB_ID_MAX )
	{
		Err( ERR_DATABASE_INVALID, "%s: Illegal Database ID (must be < %d): %d", SecDb->FullDbName.c_str(), SDB_DB_ID_MAX, SecDb->DbID );
		return NULL;
	}


	SecDb->ParentDb = SecDb;
	SecDb->DbID = DbId;

	if( !SetMaxMemSize( SecDb ) )
		return (SDB_DB *) ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to set db MaxMemSize" );

/*
**	Check DbID
*/

	if( SecDb->DbID != SDB_DB_VIRTUAL && HashLookup( SecDbDatabaseIDHash, (HASH_KEY) (long) SecDb->DbID ))
	{
		Err( ERR_FILE_INVALID, "%s: Duplicate Database ID: %d", full_db_name.c_str(), SecDb->DbID );
		return 0;
	}

	// Copy strings
	Db = SecDb;

	SecDb->FullDbName = full_db_name;
	SecDb->DbName = SecDb->FullDbName.c_str();

	switch( SecDb->DbOperator )
	{
		case SDB_DB_DIDDLE_SCOPE:
			break;

		case SDB_DB_SUBDB:
            SecDb->DatabasePath = "None";
            SecDb->MirrorGroup  = "None";
            SecDb->Region       = SecDb->Left->Region;
            SecDb->Location     = SecDb->Left->Location;
            SecDb->DatabasePath = SecDb->Left->DatabasePath;
            SecDb->Argument     = "";
            SecDb->LockServer   = "";
            SecDb->DllPath      = "subdbdrv";
            SecDb->FuncName     = "_SubDbDrvInit";
			break;

		default:
			if( Db->DllPath.empty() ) // Find a database from which we can copy strings
			{
				Db = Db->Left;
				if( !Db->DbPhysical )
					break;
				if( Db->DatabasePath.empty() && SecDb->Right && !( Db = SecDb->Right )->DbPhysical )
					break;
			}
			if( Db == SecDb ) break;
			SecDb->MaxMemSize   = Db->MaxMemSize;
			SecDb->MirrorGroup	= Db->MirrorGroup;
			SecDb->Region		= Db->Region;
			SecDb->Location		= Db->Location;
			SecDb->DatabasePath	= Db->DatabasePath;
			SecDb->Argument		= Db->Argument;
			SecDb->LockServer	= Db->LockServer;
			SecDb->DllPath		= Db->DllPath;
			SecDb->FuncName		= Db->FuncName;
			SecDb->DbAttributes = Db->DbAttributes;
			break;
	}

	if( SecDb->DbOperator == SDB_DB_DUPLICATED )
		SecDb->DbAttributes |= SDB_DB_ATTRIB_DUPLICATED;

	// Set stream version
	if( SecDb->DbAttributes & SDB_DB_ATTRIB_OLD_STREAM )
		SecDb->StreamVersion = SDB_STREAM_OLD;
	else
		SecDb->StreamVersion = SDB_STREAM_NEW;

	// Register the database
	HashInsert( SecDbDatabaseHash, SecDb->FullDbName.c_str(), SecDb );
	if( SecDb->DbID != SDB_DB_VIRTUAL )
		HashInsert( SecDbDatabaseIDHash, (HASH_KEY) (long) SecDb->DbID, SecDb );

	SecDb->DbSingleton = SDB_DBSET( SecDb );
	SecDb->DbDepth = SecDb->Right ? SecDb->Right->DbDepth + 1 : 1;

	return db_guard.release();
}



/****************************************************************
**	Routine: SecDbLoadDatabaseTable
**	Returns: TRUE	- Database table loaded without error
**			 FALSE	- Error loading database table
**	Action : Load a database table from an ASCII file
**			 into a hash table.  The format for the ASCII file is:
**
**				DbName			- Numeric identifier for the type
**				DbID			- Database identifier
**				MirrorGroup		- Mirror group
**				Location		- Physical location of database
**				Region			- Global region (NYC/LDN/HKG/TKO/SGP)
**				DllPath			- Path and filename for the dll
**				FunctionName	- Name of the function in the dll
**				DatabasePath	- Path for the database
**				Extra			- Name of the database
**				LockServer		- Name of the lock server
**				Attributes      - 
**
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbLoadDatabaseTable"

int SecDbLoadDatabaseTable(
	const char	*Path,
	const char	*TableName
)
{
#define MAX_TOKENS	21
	char	Buffer[ 256 ],
			*Tokens[ MAX_TOKENS+ 1 ],
			FileName[ FILENAME_MAX + 1 ],
			*Str;

	const char
			*Config;

	FILE	*fpTable;

	int		Argc,
			Tok,
			Line,
			TmpId,
			Status;


/*
**	Open the ASCII file containing the table
*/

	if( !KASearchPath( Path, TableName, FileName, sizeof( FileName )))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to find %s on %s", TableName, Path );

	if( !(fpTable = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) %s", FileName, strerror( errno ));
	Line = 0;


/*
**	Read and parse the file
*/

	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		int ActiveDb = TRUE;
		Argc = StrTokenize( Buffer, MAX_TOKENS, Tokens );
		Line++;

		auto_ptr<SDB_DB> SecDb( new SDB_DB );

		switch( Argc )
		{
			default:
				fclose( fpTable );
				if( Argc < 0 )
					return ErrMore( ARGCHECK_FUNCTION_NAME "( %s ) error tokenizing", FileName );
				else
					return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s:%d ) bad token count", FileName, Line );
			case 0:
				continue;	// Blank line or only comments

			// Should be CONFIG
			case 2:
				if( 0 != strcmp( Tokens[ 1 ], "CONFIG" ) )
				{
					fclose( fpTable );
					return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s:%d ) bad token count (expected 2 for CONFIG)", FileName, Line );
				}
				SecDb->FullDbName = Tokens[0];
				sprintf( Buffer, "SECDB_ALIAS.%s", SecDb->FullDbName.c_str() );
				Config = SecDbConfigurationGet( Buffer, NULL, NULL, NULL );
				if( !Config )
				{
					SecDb->Argument = "secdb.cfg: missing '";
					SecDb->Argument += Buffer;
					SecDb->Argument += "'";
				}
				else
					SecDb->Argument = Config;
				SecDb->DbID = SDB_DB_ID_ALIAS;
				SecDb->DbAttributes	= SDB_DB_ATTRIB_ALIAS;
				SecDb->MirrorGroup	= "";
				SecDb->Region		= "";
				SecDb->Location		= "";
				SecDb->DllPath		= "secdb";
				SecDb->FuncName		= "";
				SecDb->DatabasePath	= "";
				SecDb->LockServer	= "";
				break;

			// Should be ALIAS
			case 3:
				if( 0 != strcmp( Tokens[ 1 ], "ALIAS" ) )
				{
					fclose( fpTable );
					return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s:%d ) bad token count (expected 3 for ALIAS)", FileName, Line );
				}
				SecDb->FullDbName = Tokens[0];
				if( Argc == 3 )
					SecDb->Argument = Tokens[ 2 ];
				SecDb->DbID = SDB_DB_ID_ALIAS;
				SecDb->DbAttributes	= SDB_DB_ATTRIB_ALIAS;
				SecDb->MirrorGroup	= "";
				SecDb->Region		= "";
				SecDb->Location		= "";
				SecDb->DllPath		= "secdb";
				SecDb->FuncName		= "";
				SecDb->DatabasePath	= "";
				SecDb->LockServer	= "";
				break;

			case  9:	case 14: 
			case 10:    case 15:
			case 11:    case 16:
			case 12:    case 17:
			case 13:    case 18:
				SecDb->FullDbName = Tokens[0];
				TmpId = atoi( Tokens[1] );
				if( TmpId <= 0 || TmpId >= SDB_DB_ID_MAX )
				{
					fclose( fpTable );
					return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s:%d ) bad DbID", FileName, Line );
				}
				SecDb->DbID = TmpId;
				SecDb->MirrorGroup	= Tokens[2];
				SecDb->Location		= Tokens[3];
				SecDb->Region		= Tokens[4];
				SecDb->DllPath		= Tokens[5];
				SecDb->FuncName		= Tokens[6];
				SecDb->DatabasePath	= Tokens[7];
				SecDb->Argument		= Tokens[8];
				SecDb->LockServer	= Tokens[9];

				if( SecDb->DllPath.empty() ) // hack to make sure DllPath is always non null so we can copy strings when
					SecDb->DllPath = "<null>"; // we construct unions

				// Attribute flags
				for( Tok = 10; Tok < Argc; Tok++ )
				{
					Str = Tokens[ Tok ];
					if( !strcasecmp( Str, "Prod" ))
						SecDb->DbAttributes	|= SDB_DB_ATTRIB_PROD;
					else if( !strcasecmp( Str, "ProdOnly" ))
						SecDb->DbAttributes	|= SDB_DB_ATTRIB_PRODONLY;
					else if( !strcasecmp( Str, "Dev" ))
						SecDb->DbAttributes	|= SDB_DB_ATTRIB_DEV;
					else if( !strcasecmp( Str, "ReadOnly" ))
						SecDb->DbAttributes	|= SDB_DB_ATTRIB_READONLY;
					else if( !strcasecmp( Str, "OldStream" ))
						SecDb->DbAttributes	|= SDB_DB_ATTRIB_OLD_STREAM;
					else if( !strcasecmp( Str, "Obsolete" ))
						SecDb->DbAttributes	|= SDB_DB_ATTRIB_OBSOLETE;
					else if( !strcasecmp( Str, "Qualifiers" ))
						SecDb->DbAttributes	|= SDB_DB_ATTRIB_QUALIFIERS;
					else if( !strncmp( Str, "ACL:", 4 ) ) 
					{
						if( !ParseACL( SecDb.get(), Str+4, &ActiveDb ) )
						{
							fclose( fpTable );
							return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", FileName );
						}
					}
				}
				break;
		}

		if( ActiveDb )
			Status = NULL != SecDbDbRegister( SecDb.release() );

		if( !Status )
		{
			fclose( fpTable );
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", FileName );
		}
	}


/*
**	Close the file
*/

	fclose( fpTable );

	DbsByMirrorGroup = HashSortedArray( SecDbDatabaseHash, (HASH_PAIR_CMP) DbsCompareByMirrorGroup );
	NumDbsByMirrorGroup = SecDbDatabaseHash->KeyCount;

	SecDbSystemDiddleScopeNew( "Null" );
	SecDbSystemDiddleScopeNew( "None" );
	{
		SDB_DB	*OrphansDb = SecDbSystemDiddleScopeNew( "Orphans" );
		if( !OrphansDb )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Cannot construct Orphans Db" );
		OrphansDb->CacheHash = HashCreate( "Orphaned Objects", 	(HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );
	}
	if( !( SecDbClassObjectsDb = SecDbSystemDiddleScopeNew( "Class Objects" )))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Cannot construct Class Objects Db" );

	SecDbClassObjectsDb->CacheHash = HashCreate( "Class Objects", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );
	SecDbClassObjectsDb->DbPhysical = SecDbClassObjectsDb;

	return TRUE;
}

/****************************************************************
 **	Routine: ParseACLAtom
 **	Returns: True/Err
 **	Action : Parse one ACL atom (rule) and apply the specified 
 **          permission operations to the mask corresponding to 
 **          this rule. If the rule specifies a group other than
 **          the current group, just return.
 ****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "ParseACLAtom"

int
ParseACLAtom( char *Atom, const char *Group, int *Mask )
{
	std::string savedAtom = Atom;
	char *ptr, *perms;
	char Op;

	ptr = Atom;
	if( *ptr == '+' || *ptr == '-' || *ptr == '=' )
		return Err( ERR_INVALID_ARGUMENTS, 
					ARGCHECK_FUNCTION_NAME "( %s ): invalid ACL atom", savedAtom.c_str() );
    
	// Look for the first permission operator.
	while( *ptr && *ptr != '+' && *ptr != '-' && *ptr != '=' )
		++ptr;

	if( *ptr == 0 )
		return Err( ERR_INVALID_ARGUMENTS, 
					ARGCHECK_FUNCTION_NAME "( %s ): invalid ACL atom", savedAtom.c_str() );

	Op = *ptr;
	*ptr = 0;
	++ptr;

	// Net group does not match this rule, so skip it.
	if( strcmp( Atom, Group ) != 0 ) 
		return TRUE;

	perms = ptr;
	if( Op == '=' )
		*Mask = 0; // Reset the mask.
    
	while( *perms )
	{
		if( *perms == 'R' )
		{
			if( Op == '-' )
				*Mask &= ~2;
			else
				*Mask |= 2;
		}
		else if( *perms == 'W' )
		{
			if( Op == '-' )
				*Mask &= ~1;
			else
				*Mask |= 1;
		}
		else
			return Err( ERR_INVALID_ARGUMENTS, 
						ARGCHECK_FUNCTION_NAME "( %s ): invalid permissions op: %c", 
						savedAtom.c_str(), *perms );
        
		++perms;
	}

	return TRUE;    
}

/****************************************************************
 **	Routine: ParseACL
 **	Returns: True/Err
 **	Action : Parse the ACL and set the appropriate attributes 
 **          for SecDb. ActiveDb is set to FALSE if the db is
 **          not visible for the current session.
 ****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "ParseACL"

int
ParseACL( SDB_DB *SecDb, char *ACL, int *ActiveDb )		   
{
	std::string savedAcl = ACL;
	int PermsMask = 3; // By default can read and write (R == 2; W == 1).
	const char *Group = SecDbConfigurationGet( "NET_GROUP_SECDB", NULL, NULL, NULL );
	if( !Group )
		return ErrMore( ARGCHECK_FUNCTION_NAME "( ACL ): Cannot determine net group" );
       
	for( char *Tok = strtok( ACL, "," ); Tok; Tok = strtok( NULL, "," ) )
	{
		if( !ParseACLAtom( Tok ? Tok : ACL, Group, &PermsMask ) )
			return ErrMore( "ParseACL( %s )", ACL );
	}
	
	if( PermsMask == 0 )
		*ActiveDb = 0;
	else if( PermsMask == 1 )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Write-only access not supported", savedAcl.c_str() );
	else if( PermsMask == 2 )
		SecDb->DbAttributes	|= SDB_DB_ATTRIB_READONLY;
	// else read-write access...
        
	return TRUE;
}

/****************************************************************
 **	Routine: DbAttach
 **	Returns: True/Err
 **	Action : attempts to attach to the specified database
 **			 will detach from any sub databases in case of
 **			 failure
 ****************************************************************/

int DbAttach(
	SDB_DB	*Db,
	int	DeadPoolSize,
	int	Flags
)
{
	SDB_OBJECT
			*SecPtrSecDb;

	SDB_OBJECT
			*SecPtrVer;

	SDB_DB	*pDbUpdate;


	// this can never happen for a database with sub databases
	if( Db->DbAttributes & SDB_DB_ATTRIB_OBSOLETE )
		return Err( ERR_DATABASE_OBSOLETE, "Attach( %s ), @", Db->FullDbName.c_str() );

	 // if already open, just increment the open count
	if( Db->OpenCount > 0 )
	{
		// If FRONT then make this the root database
		if(( Flags & SDB_FRONT ) || !SecDbRootDb )
			SecDbRootDb = Db;

		Db->OpenCount++;
/*
**	Recursively reattach to child dbs to increment their OpenCounts
*/
		if( Db->Left )
			SecDbReattach( Db->Left );
		if( Db->Right )
			SecDbReattach( Db->Right );

		return TRUE;
	}

	// AWR: 352658
	// Set the SDB_NO_SECURITY_DATABASE property to the node
	// if all its child nodes have it
	bool propagate_no_sec_db = false;
	if( Db->Left && Db->Left->DbType & SDB_NO_SECURITY_DATABASE )
		propagate_no_sec_db = true;
	if( Db->Right && !( Db->Right->DbType & SDB_NO_SECURITY_DATABASE ) )
		propagate_no_sec_db = false;

	if( propagate_no_sec_db )
		Db->DbType |= SDB_NO_SECURITY_DATABASE;

	switch( Db->DbOperator )
	{
		case SDB_DB_LEAF:
		case SDB_DB_SUBDB:
		case SDB_DB_DUPLICATED:
		{
			// Initialize database driver (because it hasnt been loaded yet)
			if( !Db->pfInit )
			{
				Db->pfInit = (SDB_DB_INIT_FUNC) DynaLink( Db->DllPath.c_str(), Db->FuncName.c_str() );
				if( !Db->pfInit )
					return ErrMore( "Attach( %s )", Db->FullDbName.c_str() );

				Db->MemUsed = 0;
				(*Db->pfInit)( Db );
			}

			// Are we allowed to attach & write to this database?
			if( !( SecDbConfigAttributes & SDB_DB_ATTRIB_PROD ))
			{
				if( Db->DbAttributes & SDB_DB_ATTRIB_PRODONLY )
					return Err( ERR_DATABASE_INVALID, "Attach( %s ) Database available from production only", Db->FullDbName.c_str() );
				else if( Db->DbAttributes & SDB_DB_ATTRIB_PROD )
					Db->DbType |= SDB_NEVER_WRITE;
			}

			// Grab bits that init might have put in DbType.
			Flags |= ( Db->DbType & ( SDB_SYNTHETIC | SDB_NO_SECURITY_DATABASE ) );

			if( Db->DbAttributes & SDB_DB_ATTRIB_READONLY )
				Db->DbType |= SDB_NEVER_WRITE;

			if( Db->DbType & SDB_NEVER_WRITE )
				Db->DbType = Flags | SDB_READONLY | SDB_NEVER_WRITE;
			else
				Db->DbType = Flags;

			if( !Db->pfOpen || !(*Db->pfOpen)( Db ))
				return ErrMore( "Attach( %s )", Db->FullDbName.c_str() );

			if( Db->DbType & SDB_SYNTHETIC )
				SecDbDbUpdateSet( Db, NULL );
			else	
				SecDbDbUpdateSet( Db, Db );	// update db is self

			Db->DbPhysical = Db;		// physical db is self
			break;
		}

		case SDB_DB_DIDDLE_SCOPE:
			SecDbDbUpdateSet( Db, NULL );		// no update db
			Db->DbPhysical = NULL;		// no physical db
			Db->DbType = SDB_NEVER_WRITE | SDB_READONLY | ( Db->DbType & SDB_NO_SECURITY_DATABASE );
			break;

		case SDB_DB_CLASSFILTERUNION:
			SecDbDbUpdateSet( Db, NULL );	// Call SecDbDbUpdateGet to find what the actual update db is!
			goto DbUpdateDone;

		default:
			if( !( pDbUpdate = SecDbDefaultDbUpdateGet( Db->Left ) ) )
				pDbUpdate = SecDbDefaultDbUpdateGet( Db->Right );

			SecDbDbUpdateSet( Db, pDbUpdate );

			DbUpdateDone:
			if( !Db->Left->DbPhysical )
				Db->DbPhysical = Db->Right ? Db->Right->DbPhysical : NULL;
			else
				Db->DbPhysical = Db;

			if( Db->DbOperator == SDB_DB_READONLY || ( Db->Left->DbType & SDB_READONLY ))
				Db->DbType |= SDB_NEVER_WRITE | SDB_READONLY;
			break;
	}

	// If FRONT then make this the root database
	if(( Flags & SDB_FRONT ) || !SecDbRootDb )
		SecDbRootDb = Db;

	Db->OpenCount	= 1;
	if( Db->DbPhysical != Db )
		return TRUE;


	// Create cache hash tables
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	Db->CacheHash 		= HashCreate( "Object Cache", 	(HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, SDB_OBJECT_CACHE_INITIAL_SIZE, NULL );
	Db->PreCacheHash 	= HashCreate( "Object PreCache",(HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, SDB_OBJECT_PRECACHE_INITIAL_SIZE, NULL );
	Db->PageTableHash 	= HashCreate( "Page Tables", 	(HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, SDB_PAGETABLE_INITIAL_SIZE, NULL );

	// Create the deadpool
	if( !SecDbCreateDeadPool( Db, DeadPoolSize ))
		return FALSE;


/*
**	Load the indices from the database
**
**	NOTE:	This reference will be freed in SecDbDetach via SecDbGetByName()
**			followed by two SecDbFree()s
*/

	if( !( Db->DbType & SDB_NO_SECURITY_DATABASE ) )
	{
		SecPtrSecDb = SecDbGetByName( "Security Database", Db, SDB_REFRESH_CACHE | SDB_IGNORE_PATH );
		if( !SecPtrSecDb )
			return ErrMore( "Failed in attempt to DbAttach() - couldn't get the Security Database security object." );
	}

	{
		static bool expr_children = strcasecmp( SecDbConfigurationGet( "SECDB_EXPRESSION_CHILDREN", NULL, NULL, "ENABLED" ), "ENABLED" ) == 0;

		if( expr_children )
			SecDbNew( "SlangEvalObject", SecDbClassTypeFromName( "Slang Eval Class GOB" ), Db );
	}

/*
**	Check the database/client version information
**
**	If the "Security Database Version" object does not exist, allow all access
*/

	SecPtrVer = SecDbGetByName( "Security Database Version", Db, SDB_REFRESH_CACHE | SDB_IGNORE_PATH );
	if( SecPtrVer )
	{
		DT_VALUE
				*Value = SecDbGetValue( SecPtrVer, SecDbValueTypeFromName( "Client Version Minimum", NULL ));

		double DbVer = Value ? Value->Data.Number : 0;

		SecDbFree( SecPtrVer );
		if( !Value )
			return FALSE;

		if( DbVer > CodeVersion )
		{
			Err( ERR_DATABASE_INVALID, "Program code version invalid.\n"
				 "  Database requires at least: %12.3f"
				 "  Program code is only:       %12.3f", DbVer, CodeVersion );
			return FALSE;
		}
	}

	return TRUE;
}



/****************************************************************
**	Routine: DbsCompareByMirrorGroup
**	Returns: 
**	Action : 
****************************************************************/

static int DbsCompareByMirrorGroup(
	const void	*Ptr1,
	const void	*Ptr2
)
{
	SDB_DB	*Db1 = (SDB_DB *) ((HASHPAIR *) Ptr1 )->Value,
			*Db2 = (SDB_DB *) ((HASHPAIR *) Ptr2 )->Value;

	int		Cmp;

	if( (Cmp = strcasecmp( Db1->MirrorGroup.c_str(), Db2->MirrorGroup.c_str() )) || (Cmp = strcasecmp( Db1->DbName, Db2->DbName )))
		return Cmp;
	return 0;
}



/****************************************************************
**	Routine: DbFromDbName
**	Returns: SDB_DB * / NULL
**	Action : Looks up a database name and returns the SDB_DB
**			 structure for it
****************************************************************/

SDB_DB *DbFromDbName(
	const char	*DbName
)
{
	return (SDB_DB *) HashLookup( SecDbDatabaseHash, DbName );
}



/****************************************************************
**	Routine: SecDbAttach
**	Returns: SDB_DB * / NULL
**	Summary: Open and attach to a SecDb database
****************************************************************/

SDB_DB *SecDbAttach(
    const char    *DbPath,        // Name of database to attach (and initial search path)
    int            DeadPoolSize,    // Size of the databases deadpool
    int            Flags            // Placement of database within linked list
)
{
    char    *Path = strdup( DbPath );

    SDB_DB    *Db = NULL;

    // remove trailing qualifiers/delimiters
    {
        char* Ptr;

        for( Ptr = Path + strlen( Path );
             Ptr > Path && (Ptr[ -1 ] == SDB_SEARCH_DELIMITER || Ptr[ -1 ] == SDB_SUBDB_DELIMITER);
             --Ptr);
        *Ptr = 0;
    }
   
    char const* Ptr;

    // Validate the input path so we don't end up evaluating garbage and possibly
    // crashing.
    for( Ptr = Path; *Ptr; Ptr++ )
    {
        if( !( isalnum( *Ptr ) || isspace( *Ptr ) || strchr( SDB_DELIMITER_AND_QUALIFIER_STRING SDB_USERDB_PREFIX_STRING, *Ptr ) ) )
            return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, "@ - Invalid DbPath: %s\n", Path );
    }
    Ptr = Path;
    Db = SecDbParseAndAttach( &Ptr, DeadPoolSize, Flags );

    free( Path );
    return Db;
}


	
/****************************************************************
**	Routine: SecDbReattach
**	Returns: TRUE/FALSE
**	Summary: Increment the open count on a SecDb database
****************************************************************/

int SecDbReattach(
	SDB_DB	*pDb
)
{
	if( pDb->OpenCount <= 0 )
		return Err( ERR_DATABASE_INVALID, "SecDbReattach( %s ), database not open", pDb->FullDbName.c_str() );

	pDb->OpenCount++;

/*
**	Recursively reattach to child dbs to increment their OpenCounts
*/
	if( pDb->Left )
		SecDbReattach( pDb->Left );
	if( pDb->Right )
		SecDbReattach( pDb->Right );

	return TRUE;
}

/****************************************************************
**	Routine: SecDbGetOrphansDb
**	Returns: OrphansDb
**	Summary: Get the Orhans Database
****************************************************************/

SDB_DB *SecDbGetOrphansDb()
{
	static SDB_DB *OrphansDb = NULL;

	if( !OrphansDb )
		OrphansDb = SecDbAttach( "!Orphans", 0, 0 );
	return (OrphansDb);
}

/****************************************************************
**	Routine: SecDbDetach
**	Returns: TRUE	- Detached the database without error
**			 FALSE	- Error detaching the database
**	Summary: Detach a SecDb database
****************************************************************/

int SecDbDetach(
	SDB_DB		*pDb	// Database to detach, NULL for all
)
{
	SDB_OBJECT
			*SecPtr;

	int		Token;

	HASH_ENTRY_PTR
			HashPtr;


/*
**	If NULL, close and detach all of the databases
*/

	if( !pDb )
	{
		HASH_FOR( HashPtr, SecDbDatabaseIDHash )
			if( !SecDbDetach( (SDB_DB *) HashValue( HashPtr ) ))
				return FALSE;
		SecDbRootDb = NULL;
		return TRUE;
	}

	// Do not decrement OpenCount just yet else SecDbDbFromDbID will refuse to look it up
	// as it thinks the Db has already been closed
	if( pDb->OpenCount == 1 && pDb->DbPhysical == pDb )
	{
		if( !( pDb->DbType & SDB_NO_SECURITY_DATABASE ) )
		{
			// Free 'Security Database' object
			SecPtr = SecDbGetByName( "Security Database", pDb, SDB_CACHE_ONLY | SDB_IGNORE_PATH );
			if( SecPtr )
			{
				SecDbFree( SecPtr );
				SecDbFree( SecPtr );
			}
		}

		SecDbRemoveFromDeadPool( pDb, 0, NULL );

		// Remove cached values for still-referenced objects
		Token = 0;
		SDB_NODE::Destroy( NULL, &Token );
		HASH_FOR( HashPtr, pDb->CacheHash )
			NodeDestroyCache( (SDB_OBJECT *) HashValue( HashPtr ), FALSE );
		SDB_NODE::Destroy( NULL, &Token );
		SecDbRemoveFromDeadPool( pDb, 0, NULL );

		if( pDb->CacheHash->KeyCount > 0 )
		{
			SDB_DB *OrphansDb = SecDbGetOrphansDb();

			static int
					NextOrphan = 1;

			DT_VALUE
					TmpVal;
		
			Token = 0;
			SDB_NODE::Destroy( NULL, &Token );
			TmpVal.DataType = DtSecurity;
			HASH_FOR( HashPtr, pDb->CacheHash )
			{
				char	NewName[ SDB_SEC_NAME_SIZE + 30 ];

				SDB_OBJECT *SecPtr = (SDB_OBJECT *) HashValue( HashPtr );
				TmpVal.Data.Pointer = SecPtr;
				SDB_NODE::Destroy( SDB_NODE::FindLiteral( &TmpVal, pDb ), &Token );

				sprintf( NewName, "~Orphan~%d%s", NextOrphan++, SecPtr->SecData.Name );
				strncpy( SecPtr->SecData.Name, NewName, SDB_SEC_NAME_SIZE - 1 );
				SecPtr->SecData.Name[ SDB_SEC_NAME_SIZE - 1 ] = '\0';
				SecPtr->Db = OrphansDb;
				// FIX: make the Orphans DB take a ref from the Sec. This will make
				// The Secs in Orphans DB have a min Ref of 1. This would not have been necesarry
				// If the literals in the graph didn't have an implicit ref on sec's
				// Until we fix that, this seems to be resonable solution to avoid
				// red boxes in UseDatabase constructs where the vars are holding secs
				// with refcount of 0.
				++SecPtr->ReferenceCount;
				HashInsert( OrphansDb->CacheHash, SecPtr->SecData.Name, SecPtr );
			}

			SDB_NODE::Destroy( NULL, &Token );
		}

		SecDbIndexClose( NULL, pDb, TRUE );
		if( pDb->pfClose )
			(*pDb->pfClose)( pDb );

		HashDestroy( pDb->CacheHash );
		pDb->CacheHash = NULL;

		HashDestroy( pDb->PreCacheHash );
		pDb->PreCacheHash = NULL;

		HashDestroy( pDb->PageTableHash );
		pDb->PageTableHash = NULL;

		if( pDb->DeadPoolSize > 0 )
		{
			free( pDb->DeadPool );
			pDb->DeadPool = NULL;
		}

		--pDb->OpenCount;

		while( pDb->SecDbSearchPath )
		{
			SDB_DB_SEARCH_PATH	*SearchElem = pDb->SecDbSearchPath;

			pDb->SecDbSearchPath = SearchElem->Next;
			SecDbDetach( SearchElem->Db );
			free( SearchElem );
		}

		if( pDb->Left )
			SecDbDetach( pDb->Left );
		if( pDb->Right )
			SecDbDetach( pDb->Right );

		if( SDB_DB_VIRTUAL == pDb->DbID )
			HashDelete( SecDbDatabaseHash, pDb->FullDbName.c_str() );

		if( SecDbRootDb == pDb )
		{
			// randomly pick another open Db as the RootDb
			SecDbRootDb = NULL;
			HASH_FOR( HashPtr, SecDbDatabaseIDHash )
			{
				SDB_DB *Db = (SDB_DB *) HashValue( HashPtr );
				// AWR: #363400
				if( Db->OpenCount > 0 )
				{
					SecDbRootDb = Db;
					break;
				}
			}
		}
	}
	else
	{
		--pDb->OpenCount;

		if( pDb->Left )
			SecDbDetach( pDb->Left );
		if( pDb->Right )
			SecDbDetach( pDb->Right );
	}

	if( pDb->OpenCount == 0 && pDb->DbOperator == SDB_DB_DIDDLE_SCOPE )
	{									// union which used it has gone, no way anyone could be using it. kill kill
		HashDelete( SecDbDatabaseHash, pDb->FullDbName.c_str() );
		delete pDb;
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbSetRootDatabase
**	Returns: Previous root database if successful
**			 NULL if invalid or unattached database
**	Summary: Choose new database for SDB_ROOT_DATABASE
**	Action : Set SecDbRootDb
****************************************************************/

SDB_DB *SecDbSetRootDatabase(
	SDB_DB	*Db
)
{
	SDB_DB	*PrevRootDb = SecDbRootDb;

	// AWR: #363400
	if( !Db || Db->OpenCount <= 0 )
	{
		Err( ERR_DATABASE_INVALID, "SetRootDatabase(): NULL or unattached Db" );
		return NULL;
	}
	SecDbRootDb = Db;
	return PrevRootDb;
}



/****************************************************************
**	Routine: SecDbSetTimeout
**	Returns: TRUE, when successful; FALSE, otherwise
**	Action : Set the timeout for database I/O to Timeout seconds.
**			 Calling this function with a timeout value of zero will
**			 restore blocking behavior, which is the default.
****************************************************************/

int SecDbSetTimeout(
	SDB_DB	*pDb,
	int		Timeout
)
{
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, "SecDbSetTimeout( NULL ) - @" );

	if( !pDb->pfSetTimeout )
		return Err( ERR_UNSUPPORTED_OPERATION, "SecDbSetTimeout( %s ) - @", pDb->FullDbName.c_str() );

	return (*pDb->pfSetTimeout)( pDb, Timeout );
}



/****************************************************************
**	Routine: SecDbSetRetryCommits
**	Returns: TRUE, when successful; FALSE, otherwise
**	Action : Allow the client to retry failed transaction commits,
**			 depending on the value of AllowRetries.  The default is
**			 to allow clients to retry commits, but in very rare cases
**			 this allows a client to commit the same transaction twice.
****************************************************************/

int SecDbSetRetryCommits(
	SDB_DB		*pDb,
	int			AllowRetries
)
{
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, "SecDbSetRetryCommits( NULL ) - @" );

	if( !pDb->pfSetRetryCommits )
		return Err( ERR_UNSUPPORTED_OPERATION, "SecDbSetRetryCommits( %s ) - @", pDb->FullDbName.c_str() );

	return (*pDb->pfSetRetryCommits)( pDb, AllowRetries );
}



/****************************************************************
**	Routine: SecDbDbValidate
**	Returns: TRUE	- Database validates successfully
**			 FALSE	- Database failed validation
**	Summary: Validate a database
**	Action : Send a validate message to the database driver.
**			 CAUTION:  This could take a long time.
****************************************************************/

int SecDbDbValidate(
	SDB_DB	*pDb	// Database ID to detach, SDB_ROOT_DATABASE for all
)
{
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, "SecDbDbValidate( NULL ) - @" );

	if( !pDb->pfDbValidate )
		return Err( ERR_UNSUPPORTED_OPERATION, "SecDbDbValidate( %s ) - @", pDb->FullDbName.c_str() );

	return (*pDb->pfDbValidate)( pDb );
}



/****************************************************************
**	Routine: DbToDbInfo
**	Returns: None
**	Action : Copy the fields from an internal database structure
**			 to the published DbInfo structure
****************************************************************/

static void DbToDbInfo(
	SDB_DB		*pDb,
	SDB_DB_INFO	*DbInfo
)
{
	SDB_DB	*DbUpdate;


	DbInfo->DbName 			= pDb->FullDbName.c_str();
	DbInfo->Db				= pDb;

	if( ( DbUpdate = SecDbDefaultDbUpdateGet( pDb ) ) )
	{
        DbInfo->Location        = DbUpdate->Location     .c_str();
        DbInfo->Region          = DbUpdate->Region       .c_str();
        DbInfo->MirrorGroup     = DbUpdate->MirrorGroup  .c_str();
        DbInfo->DllPath         = DbUpdate->DllPath      .c_str();
        DbInfo->FuncName        = DbUpdate->FuncName     .c_str();
        DbInfo->DatabasePath    = DbUpdate->DatabasePath .c_str();
        DbInfo->Argument        = DbUpdate->Argument     .c_str();
        DbInfo->LockServer      = DbUpdate->LockServer   .c_str();
    }
    else
    {
        DbInfo->Region          = pDb->Region            .c_str();
        DbInfo->Location        = pDb->Location          .c_str();
        DbInfo->MirrorGroup     = pDb->MirrorGroup       .c_str();
        DbInfo->DllPath         = pDb->DllPath           .c_str();
        DbInfo->FuncName        = pDb->FuncName          .c_str();
        DbInfo->DatabasePath    = pDb->DatabasePath      .c_str();
        DbInfo->Argument        = pDb->Argument          .c_str();
        DbInfo->LockServer      = pDb->LockServer        .c_str();
	}
	DbInfo->Qualifier 		= pDb->Qualifier.c_str();

	DbInfo->DbID			= pDb->DbID;
	DbInfo->OpenCount		= pDb->OpenCount;
	DbInfo->DbAttributes	= pDb->DbAttributes;

	if( pDb->DbType & SDB_READONLY )
		DbInfo->DbAttributes |= SDB_DB_ATTRIB_READONLY;
}



/****************************************************************
**	Routine: SecDbDbInfoToStructure
**	Returns: TRUE or FALSE
**	Action : Put SDB_DB_INFO fields in a structure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDbInfoToStructure"

int SecDbDbInfoToStructure(
	const SDB_DB_INFO	*DbInfo,
	DT_VALUE			*Value
)
{
	int		Status,
			OtherBits;

	DT_VALUE
			Attrib;

	SDB_DB	*pDb,
			*pDbUpdate;


	Status = DTM_ALLOC( Value, DtStructure ) && DTM_ALLOC( &Attrib, DtStructure );
	if( !Status )
		return ErrMore( ARGCHECK_FUNCTION_NAME );

	pDb = DbInfo->Db;

	Status = Status && DtComponentSetPointer( Value, "DbName",			DtString, DbInfo->DbName		);
	Status = Status && DtComponentSetPointer( Value, "Location",		DtString, DbInfo->Location		);
	Status = Status && DtComponentSetPointer( Value, "Region",			DtString, DbInfo->Region		);
	Status = Status && DtComponentSetPointer( Value, "MirrorGroup",		DtString, DbInfo->MirrorGroup	);
	Status = Status && DtComponentSetPointer( Value, "DLLPath",			DtString, DbInfo->DllPath		);
	Status = Status && DtComponentSetPointer( Value, "Function",		DtString, DbInfo->FuncName		);
	Status = Status && DtComponentSetPointer( Value, "Argument",		DtString, DbInfo->Argument		);
	Status = Status && DtComponentSetNumber(  Value, "DbID",		   	DtDouble, DbInfo->DbID			);
	Status = Status && DtComponentSetNumber(  Value, "OpenCount",		DtDouble, DbInfo->OpenCount		);
	Status = Status && DtComponentSetNumber(  Value, "MaxMemSize",		DtDouble, pDb ? pDb->MaxMemSize : 0.0	);
	Status = Status && DtComponentSetPointer( Value, "DbUpdate",		DtString, pDb && ( pDbUpdate = SecDbDefaultDbUpdateGet( pDb ) ) ? pDbUpdate->FullDbName.c_str() : DbInfo->DbName );

	Status = Status && DtComponentSetNumber( &Attrib, "Prod",		DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_PROD			));
	Status = Status && DtComponentSetNumber( &Attrib, "ProdOnly",	DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_PRODONLY     ));
	Status = Status && DtComponentSetNumber( &Attrib, "Dev",		DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_DEV          ));
	Status = Status && DtComponentSetNumber( &Attrib, "ReadOnly",	DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_READONLY     ));
	Status = Status && DtComponentSetNumber( &Attrib, "OldStream",	DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_OLD_STREAM   ));
	Status = Status && DtComponentSetNumber( &Attrib, "Obsolete",	DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_OBSOLETE     ));
	Status = Status && DtComponentSetNumber( &Attrib, "Qualifiers",	DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_QUALIFIERS   ));
	Status = Status && DtComponentSetNumber( &Attrib, "Alias",		DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_ALIAS		));
	Status = Status && DtComponentSetNumber( &Attrib, "Duplicated",	DtDouble, !!( DbInfo->DbAttributes & SDB_DB_ATTRIB_DUPLICATED   ));

	// Some of these may be missing for certain types of databases.

	if( NULL != DbInfo->DatabasePath )
		Status = Status && DtComponentSetPointer( Value, "DatabasePath", 	DtString, DbInfo->DatabasePath 	);

	if( NULL != DbInfo->LockServer )
		Status = Status && DtComponentSetPointer( Value, "LockServer",		DtString, DbInfo->LockServer	);

	if( NULL != DbInfo->Qualifier )
		Status = Status && DtComponentSetPointer( Value, "Qualifier",		DtString, DbInfo->Qualifier	);

	OtherBits = DbInfo->DbAttributes & ~(
			SDB_DB_ATTRIB_PROD      		|
			SDB_DB_ATTRIB_PRODONLY          |
			SDB_DB_ATTRIB_DEV               |
			SDB_DB_ATTRIB_READONLY          |
			SDB_DB_ATTRIB_OLD_STREAM		|	
			SDB_DB_ATTRIB_OBSOLETE  		|		
			SDB_DB_ATTRIB_QUALIFIERS		|
			SDB_DB_ATTRIB_ALIAS				|
			SDB_DB_ATTRIB_DUPLICATED );
	if( OtherBits )
		Status = Status && DtComponentSetNumber( &Attrib, "UnknownBits",	DtDouble, OtherBits   );

	Status = Status && DtComponentSet(  Value, "DbAttributes", &Attrib );
	DTM_FREE( &Attrib );

	// Stick in the structure of the database.
	if( Status && pDb && pDb->OpenCount > 0 )
	{
		DT_VALUE
				DbStruct,
				Name;

		DT_POINTER_TO_VALUE( &Name, "Database Structure", DtString );
 
		if( ( Status = DbStructure( pDb, &DbStruct ) ) )
			Status = DtComponentReplace( Value, &Name, &DbStruct );
	}

	// FIX - these alternate spellings (to be backward compatible with SlangXListDatabases) should be removed
	Status = Status && DtComponentSetPointer( Value, "Mirror Group",   	DtString, DbInfo->MirrorGroup	);
	Status = Status && DtComponentSetPointer( Value, "DLL Path",		DtString, DbInfo->DllPath		);
	if( NULL != DbInfo->DatabasePath )
		Status = Status && DtComponentSetPointer( Value, "Database Path", 	DtString, DbInfo->DatabasePath	);
	if( NULL != DbInfo->LockServer )
		Status = Status && DtComponentSetPointer( Value, "Lock Server",		DtString, DbInfo->LockServer	);
	Status = Status && DtComponentSetNumber(  Value, "Open Count",		DtDouble, DbInfo->OpenCount		);
	Status = Status && DtComponentSetNumber(  Value, "ID",				DtDouble, DbInfo->DbID			);

	if( !Status )
	{
		DTM_FREE( Value );
		DTM_INIT( Value );
	}
	return( Status );
}



/****************************************************************
**	Routine: SecDbDbInfoEnumFirst
**	Returns: Pointer to first database's information.
**			 NULL if there is no database information found.
**	Action : Start enumeration of database information
****************************************************************/

SDB_DB_INFO *SecDbDbInfoEnumFirst(
	SDB_ENUM_PTR	*EnumPtr		// Pointer to pointer to return
)
{
	SDB_DB_INFO
			*DbInfo;

	SDB_DB	*pDb;


/*
**	Initialize the structure that will hold the enumerated information
*/

	*EnumPtr = (SDB_ENUM_STRUCT *) calloc( 1, sizeof( SDB_ENUM_STRUCT ));
	(*EnumPtr)->Hash	= SecDbDatabaseHash;
	(*EnumPtr)->Count	= SecDbDatabaseHash->KeyCount;
	(*EnumPtr)->Ptr		= HashFirst( (*EnumPtr)->Hash );
	(*EnumPtr)->Extra	= (SDB_DB_INFO *) calloc( 1, sizeof( SDB_DB_INFO ));
	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;

	pDb = (SDB_DB *) HashValue( (*EnumPtr)->Ptr );
	DbInfo = (SDB_DB_INFO *) (*EnumPtr)->Extra;
	DbToDbInfo( pDb, DbInfo );
	return DbInfo;
}



/****************************************************************
**	Routine: SecDbDbInfoEnumNext
**	Returns: Pointer to next database's information
**			 NULL if there is no more database information
**	Action : Enumerate the next database's information
****************************************************************/

SDB_DB_INFO *SecDbDbInfoEnumNext(
	SDB_ENUM_PTR	EnumPtr		// Enumeration pointer
)
{
	SDB_DB_INFO
			*DbInfo;

	SDB_DB	*pDb;


	EnumPtr->Ptr	= HashNext( EnumPtr->Hash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;

	pDb = (SDB_DB *) HashValue( EnumPtr->Ptr );
	DbInfo = (SDB_DB_INFO *) EnumPtr->Extra;
	DbToDbInfo( pDb, DbInfo );
	return DbInfo;
}



/****************************************************************
**	Routine: SecDbDbInfoEnumClose
**	Returns: Nothing
**	Summary: End enumeration of database info
****************************************************************/

void SecDbDbInfoEnumClose(
	SDB_ENUM_PTR	EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
	{
		free( EnumPtr->Extra );
		free( EnumPtr );
	}
}



/****************************************************************
**	Routine: SecDbDbInfoFromName
**	Returns: TRUE	- Information found and returned
**			 FALSE	- Database could not be found
**	Summary: Retrieve information about a database by database name
****************************************************************/

int SecDbDbInfoFromName(
	const char	*DbName,
	SDB_DB_INFO	*DbInfo
)
{
	SDB_DB	*pDb;

	if( !(pDb = (SDB_DB *) HashLookup( SecDbDatabaseHash, DbName )))
	{
		if( ( pDb = SecDbAttach( DbName, 0, SDB_READWRITE )) == NULL )
			return Err( ERR_DATABASE_INVALID, "DbInfoFromName( %s ) @", DbName );
		DbToDbInfo( pDb, DbInfo );
		SecDbDetach( pDb );
		return TRUE;
	}
	DbToDbInfo( pDb, DbInfo );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbDbInfoFromDb
**	Returns: TRUE	- Information found and returned
**			 FALSE	- Database could not be found
**	Summary: Retrieve information about a database by database ID
****************************************************************/

int SecDbDbInfoFromDb(
	SDB_DB		*pDb,		// Database ID to get info for
	SDB_DB_INFO	*DbInfo		// Returned database info
)
{
	if( !pDb )
		return Err( ERR_DATABASE_INVALID, "DbInfoFromID( NULL Db ) @" );

	DbToDbInfo( pDb, DbInfo );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbDbInfoFromID
**	Returns: TRUE	- Information found and returned
**			 FALSE	- Database could not be found
**	Summary: Retrieve information about a database by database ID
****************************************************************/

int SecDbDbInfoFromID(
	SDB_DB_ID	DbID,		// Database ID to get info for
	SDB_DB_INFO	*DbInfo		// Returned database info
)
{
	SDB_DB	*pDb = DbID == SDB_ROOT_DATABASE ? SecDbRootDb : (SDB_DB *) HashLookup( SecDbDatabaseIDHash, (HASH_KEY) (long) DbID );

	return SecDbDbInfoFromDb( pDb, DbInfo );
}

/****************************************************************
**	Routine: SecDbSystemDiddleScopeNew
**	Returns: Database pointer for diddle scope
**	Action : creates/returns a new diddle scope
****************************************************************/

SDB_DB *SecDbSystemDiddleScopeNew( 
    const char *DiddleScopeName 
)
{

    SDB_DB *Db = SecDbDiddleScopeNew( DiddleScopeName );
    // Increment reference count so that we never 
    // delete these...
    Db->OpenCount++;
	Db->DbType |= SDB_NO_SECURITY_DATABASE;
    return( Db );
}

/****************************************************************
**	Routine: SecDbDiddleScopeNew
**	Returns: Database pointer for diddle scope
**	Action : creates/returns a new diddle scope
****************************************************************/

SDB_DB *SecDbDiddleScopeNew(
	const char	*DiddleScopeName
)
{
	SDB_DB* NewDb = new SDB_DB;

	NewDb->DbID = SDB_DB_VIRTUAL;
	NewDb->DbOperator = SDB_DB_DIDDLE_SCOPE;
	NewDb->FullDbName = DiddleScopeName; // FIX- shouldn't be doing this

	NewDb->MirrorGroup = "None";
	NewDb->Location    = "Memory";
	NewDb->DatabasePath= "Memory";
	NewDb->DllPath     = "<none>";

	return SecDbDbRegister( NewDb );
}



/****************************************************************
**	Routine: SecDbDbUpdateSet
**	Returns: Nothing
**	Action : Set DbUpdate for database
****************************************************************/

static void SecDbDbUpdateSet(
	SDB_DB	*pDb,
	SDB_DB	*pDbUpdate
)
{
	pDb->DbUpdateNowPrivate = pDbUpdate;
}
