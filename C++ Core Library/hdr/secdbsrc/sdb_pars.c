#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_pars.c,v 1.68 2013/10/07 19:33:17 khodod Exp $"
/****************************************************************
**
**	sdb_pars.c	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_pars.c,v $
**	Revision 1.68  2013/10/07 19:33:17  khodod
**	Address obvious cases of format-string vulnerabilities.
**	#315536
**
**	Revision 1.67  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
****************************************************************/

#define BUILDING_SECDB
#include <portable.h>
#include <datatype.h>
#include <secdb.h>
#include <secexpr.h>
#include <secdrv.h>
#include <secdt.h>
#include <sdb_int.h>
#include <sdb_pars.h>
#include <memory>
#include <string>

CC_USING( std::auto_ptr );
CC_USING( std::string );


// Forward declarations.

static int
		SecDbParseDbName( 	char const**Path, SDB_DB **NewDb, int DeadPoolSize, int Flags ),
		SecDbParseSubDbName( char const**Path, SDB_DB **NewDb, int DeadPoolSize, int Flags, SDB_DB *ContainingDb );

EXPORT_C_SECDB SLANG_RET_CODE
		SlangXDbAttachmentPath( SLANG_ARGS ),
		SlangXDbStructure( SLANG_ARGS ),
		SlangXDbUpdate( SLANG_ARGS );

// An array of database name fragments to be prepended to db names
// when attempting to attach.

static DT_VALUE AttachmentPath = { NULL };

// Add new qualifiers to this table as necessary.

SDB_NAMED_QUALIFIER 
		NamedQualifiers[] =
{
	{ "ReadOnly",			SDB_QUALIFIER_READONLY 			},
	{ "ClassFilterUnion",	SDB_QUALIFIER_CLASSFILTERUNION	},
	{ "Duplicate",			SDB_QUALIFIER_DUPLICATE			},
	{ NULL }
};



/****************************************************************
**	Routine: AttachmentPathInitialize
**	Returns: 
**	Action : Initialize the AttachmentPath
****************************************************************/

static int AttachmentPathInitialize(	
)
{
	DTM_ALLOC( &AttachmentPath, DtArray );
	
	if(    !DtSubscriptSetPointer( &AttachmentPath, 0, DtString, ".!" )
	    || !DtSubscriptSetPointer( &AttachmentPath, 1, DtString, "!"  ) )
	{
		return ErrMore( "Unable to initialize attachment path to default value" );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SlangXDbAttachmentPath
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Set or get AttachmentPath
****************************************************************/

SLANG_RET_CODE SlangXDbAttachmentPath( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Path", &DtArray, "An array of strings indicating locations where databases will be attached", SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS	}
			};

	SLANG_ARG_VARS;


	SLANG_ARG_PARSE();

	// First time processing.

	if(    ( AttachmentPath.DataType == NULL )
		&& !AttachmentPathInitialize() )
	{
		SLANG_ARG_FREE();
		SLANG_NULL_RET( Ret );
		return ErrMore( "Unable to initialize default attachment path" );
	}

	if( DtArray == SLANG_TYPE( 0 ) )
	{
		// We are setting
		if( !DTM_ASSIGN( &AttachmentPath, &(SLANG_VAL(0)) ) )
		{
			SLANG_ARG_FREE();
			SLANG_NULL_RET( Ret );
			return ErrMore( "Unable to assign new attachment path" );
		}
	}
	else if( DtNull != SLANG_TYPE( 0 ) )
	{
		SLANG_ARG_FREE();
		SLANG_NULL_RET( Ret );
		return Err( ERR_INVALID_ARGUMENTS, "Erroneous parameter 0 of type %s", SLANG_TYPE( 0 )->Name );
	}

	// Return the new or existing value.

	Ret->Type = SLANG_TYPE_VALUE;

	if( !DTM_ASSIGN( &(Ret->Data.Value), &AttachmentPath ) )
	{
		SLANG_ARG_FREE();
		SLANG_NULL_RET( Ret );
		return ErrMore( "Unable to get attachment path to return" );
	}
		
	SLANG_ARG_FREE();

	return SLANG_OK;
}



/****************************************************************
**	Routine: SecDbDbOperatorName
**	Returns: Operator type name
**	Action : Convert DbOperator type into string
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDbOperatorName"

const char *SecDbDbOperatorName(
	SDB_DB_OPERATOR	Operator
)
{
	static const char
			*OperatorName[] = 
			{
				"Leaf",
				"Qualifier",
				"SearchPath",
				"ReadOnly",
				"Alias",
				"SubDb",
				"DiddleScope",
				"ClassFilter",
				"ClassFilterUnion",
				"Duplicated",
			},
			*Unknown = 
				"Unknown Db Type";


	if( Operator < 0 )
		return (const char *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Invalid DbOperator %d", Operator );

	if( (size_t) Operator >= sizeof( OperatorName ) / sizeof( OperatorName[0] ) )
		return Unknown;

	return OperatorName[ Operator ];
}


/****************************************************************
**	Routine: DbStructure
**	Returns: Success / Err()
**	Action : Produce a structure describing the structure of the
**			 given DB.  Put the result in the given DT_VALUE.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DbStructure"

int DbStructure(
	SDB_DB *pDb,
	DT_VALUE *Struct
)
{
	DTM_ALLOC( Struct, DtStructure );

	// Full name and type are pulled from the db.
	{
		DT_VALUE
				DbType,
				FilteredClassesValue,
				DatabaseValue;

		DT_DATABASE
				*Database;

		const char
				*TypeName;
			

		// Give it the database via the cheezy little DT_DATABASE
		// structure.  We have to attach, since when the UseCount
		// drops to zero we will detach.

		Database = (DT_DATABASE *)malloc( sizeof( DT_DATABASE ) );

		Database->UseCount = 1;
		Database->Db = pDb;
		if( !SecDbReattach( Database->Db ) )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": reattach failed" );

		DT_POINTER_TO_VALUE( &DatabaseValue, Database, DtDatabase );		
		DT_COMPONENT_GIVE( Struct, "Database", &DatabaseValue );


		// And the type.

		DT_NUMBER_TO_VALUE( &DbType, pDb->DbOperator, DtDouble );
		DT_COMPONENT_GIVE( Struct, "Database Type", &DbType );	  

		if( ( TypeName = SecDbDbOperatorName( pDb->DbOperator ) ) )
			DtComponentSetPointer( Struct, "Database Type Name", DtString, TypeName );

		// And ClassFilter stuff

		if( pDb->FilteredClasses )
		{
			DT_VALUE
					NullValue;

			HASH_ENTRY_PTR
					HashPtr;


			DTM_INIT( &NullValue );
			DTM_ALLOC( &FilteredClassesValue, DtStructure );

			HASH_FOR( HashPtr, pDb->FilteredClasses )
				DT_COMPONENT_GIVE( &FilteredClassesValue, HashValue( HashPtr ), &NullValue );

			DT_COMPONENT_GIVE( Struct, "ClassesFiltered", &FilteredClassesValue );
		}
	}

	// If it is virtual, there is more to report.
	if( pDb->DbOperator != SDB_DB_LEAF )
	{
		DT_VALUE
				LeftSubStruct,
				RightSubStruct;

		switch( pDb->DbOperator )
		{
			case SDB_DB_UNION:
				// We have both left and right.  Do the right first, and fall through to the left case.

				if( NULL == pDb->Right )
					return Err( ERR_INVALID_ARGUMENTS, "@: Database with operator %d should have right child.", pDb->DbOperator );

				DTM_INIT( &RightSubStruct );
				if( !DbStructure( pDb->Right, &RightSubStruct ) )
					return FALSE;

				DT_COMPONENT_GIVE( Struct, "Right Database", &RightSubStruct );	  
				
			case SDB_DB_CLASSFILTERUNION:
			case SDB_DB_CLASSFILTER:
			case SDB_DB_READONLY:
			case SDB_DB_DUPLICATED:
			case SDB_DB_ALIAS:
			case SDB_DB_SUBDB:
				// We have only left.

				if( NULL == pDb->Left )
					return Err( ERR_INVALID_ARGUMENTS, "@: Database with operator %d should have left child.", pDb->DbOperator );

				DTM_INIT( &LeftSubStruct );
				if( !DbStructure( pDb->Left, &LeftSubStruct ) )
					return FALSE;
	
				DT_COMPONENT_GIVE( Struct, "Left Database", &LeftSubStruct );	  
				
			default:
				break;

		}
	}

	return TRUE;
}



/****************************************************************
**	Routine: SlangXDbStructure
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for DbStructure
****************************************************************/

SLANG_RET_CODE SlangXDbStructure( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Database",	&DtDatabase,	"Database (optional) - root db if ommitted", SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS	}
			};

	SLANG_ARG_VARS;

	SDB_DB	*pDb;


	SLANG_ARG_PARSE();

	// If no arg, use the root db.  Otherwise, us the db given in the arg.

	if( DtNull == SLANG_TYPE( 0 ) )
		pDb = SecDbRootDb;
	else
	{
		DT_DATABASE
				*Database = (DT_DATABASE *)SLANG_PTR(0);


		pDb = Database->Db;
	}

	if( NULL == pDb )
	{
		SLANG_ARG_FREE();
		SLANG_NULL_RET( Ret );
		return Err( ERR_INVALID_ARGUMENTS, "Invalid Database" );
	}

	// Get the actual structure of the DB.

	Ret->Type = SLANG_TYPE_VALUE;

	if( !DbStructure( pDb, &(Ret->Data.Value) ) )
	{
		SLANG_NULL_RET( Ret );
		return ErrMore( "Unable to get structure of database" );
	}
		
	SLANG_ARG_FREE();

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXDbUpdate
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for DbUpdate
****************************************************************/

SLANG_RET_CODE SlangXDbUpdate( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Database",	&DtDatabase,	"root db if omitted", 					SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS	},
				{ "Class",		&DtString,		"returns default update db if omitted",	SLANG_ARG_OPTIONAL	}
			};

	SLANG_ARG_VARS;

	SDB_DB	*pDb,
			*pDbUpdate;

	DT_DATABASE
			*DtDbUpdate;

	SDB_SEC_TYPE
			SecType = (SDB_SEC_TYPE) 0;


	SLANG_ARG_PARSE();

	// If no arg, use the root db.  Otherwise, us the db given in the arg.

	if( DtNull == SLANG_TYPE( 0 ) )
		pDb = SecDbRootDb;
	else
	{
		DT_DATABASE
				*Database = (DT_DATABASE *)SLANG_PTR(0);


		pDb = Database->Db;
	}

	if( NULL == pDb )
	{
		SLANG_ARG_FREE();
		SLANG_NULL_RET( Ret );
		return Err( ERR_INVALID_ARGUMENTS, "Invalid Database" );
	}


	if( !SLANG_ARG_OMITTED( 1 ) )
	{
		SecType = SecDbClassTypeFromName( SLANG_STR(1) );
		if( !SecType )
			return SlangEvalError( SLANG_ERROR_PARMS, "Invalid class '%s':\n%s", SLANG_STR(1), ErrGetString() );
	}

	DtDbUpdate = (DT_DATABASE *)calloc( 1, sizeof( DT_DATABASE ) );

	pDbUpdate = SecDbDbUpdateGet( pDb, SecType );

	if( !pDbUpdate )
	{
		SLANG_ARG_FREE();
		SLANG_NULL_RET( Ret );
		return ErrMore( "Failed to get update db" );
	}

	DtDbUpdate->Db = SecDbAttach( pDbUpdate->FullDbName.c_str(), SDB_DEFAULT_DEADPOOL_SIZE, SDB_BACK );
	DtDbUpdate->UseCount = 1;
	
	Ret->Type = SLANG_TYPE_VALUE;

	Ret->Data.Value.DataType	= DtDatabase;
	Ret->Data.Value.Data.Pointer= DtDbUpdate;

	SLANG_ARG_FREE();

	return SLANG_OK;
}



/****************************************************************
**	Routine: SecDbParseQualifier
**	Returns: 
**	Action : 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbParseQualifier"

static SDB_QUALIFIER SecDbParseQualifier(
	char const** Path
)
{
	SDB_NAMED_QUALIFIER
			*Qualifier;

	size_t	ParenPos;
	
	const char	*Paren;


	if( NULL != ( Paren = strchr( *Path, SDB_QUALIFIER_OPEN ) ) )
	{
		ParenPos = Paren - *Path;

		for( Qualifier = NamedQualifiers; Qualifier->Name; Qualifier++ )
			// FIX - Lengths of qualifiers should all be precomputed.
			if( ( ParenPos == strlen( Qualifier->Name )			 
			    && !strnicmp( Qualifier->Name, *Path, ParenPos ) ) )
			{
				*Path = Paren + 1;
				return Qualifier->Qualifier;
			}
	}

	return SDB_QUALIFIER_ERROR;
}



/****************************************************************
**	Routine: SecDbParseQualifiedExpr
**	Returns: Pointer to database or NULL with Err set.
**	Action : Parse a given path, up to a closing '}', then
**			 apply the given qualifier to it and return the
**			 result.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbParseQualifiedExpr"

static SDB_DB *SecDbParseQualifiedExpr(
	char const**Path,
	SDB_QUALIFIER Qualifier, 
	int	DeadPoolSize,
	int	Flags
)
{
	SDB_DB	*pDb,
			*RegisteredDb;

	static unsigned long
			NextDuplicatedId = 1;

	char	Buffer[ 80 ];

	if( NULL == ( pDb = SecDbParseAndAttach( Path, DeadPoolSize, Flags ) ) )
	{
		ErrMore( ARGCHECK_FUNCTION_NAME ": recursive call to SecDbParseAndAttach( %s ) failed.", *Path );
		return NULL;
	}

	if( SDB_QUALIFIER_CLOSE != **Path )
	{
		Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Expected '%c' at '%s'", SDB_QUALIFIER_CLOSE, *Path );
		SecDbDetach( pDb );
		return NULL;
	} else
		++*Path;		

	auto_ptr<SDB_DB> QualifiedDb( new SDB_DB );

	QualifiedDb->DbOperator = SDB_DB_QUALIFIER;
	QualifiedDb->Left = pDb;
	QualifiedDb->Right = NULL;
	QualifiedDb->DbID = SDB_DB_VIRTUAL;

	switch( Qualifier )
	{
		case SDB_QUALIFIER_READONLY:
			QualifiedDb->Qualifier = "ReadOnly";
			QualifiedDb->DbOperator = SDB_DB_READONLY;
			break;

		case SDB_QUALIFIER_CLASSFILTERUNION:
			QualifiedDb->Qualifier = "ClassFilterUnion";
			QualifiedDb->DbOperator = SDB_DB_CLASSFILTERUNION;
			break;

		case SDB_QUALIFIER_DUPLICATE:
			sprintf( Buffer, "Duplicate %ld", NextDuplicatedId++ );
            QualifiedDb->Qualifier = Buffer;
			QualifiedDb->DbOperator = SDB_DB_DUPLICATED;
			break;

		default:
			Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Unsupported qualifier %d", (int)Qualifier );
			return NULL;
	}

	if( NULL == (RegisteredDb = SecDbDbRegister( QualifiedDb.release() ) ) )
	{
		ERR_OFF();
		SecDbDetach( pDb );
		ERR_ON();
		return NULL;
	}
	
	if( !DbAttach( RegisteredDb, DeadPoolSize, Flags ) )
		return NULL;

	return RegisteredDb;
}



/****************************************************************
**	Routine: SecDbParseDbNameWithAttachmentPath
**	Returns: Pointer to database or NULL with Err set.
**	Action : Parse a db or subdb name.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbParseDbNameWithAttachmentPath"

static SDB_DB *SecDbParseDbNameWithAttachmentPath(
	char const** Path,
	int	DeadPoolSize,
	int	Flags
)
{
	DT_VALUE
			Subscript,
			Value,
			ValueString;

	SDB_DB 	*pDb = 0;

	char	*Prefix,
			**ErrorsByAttachment = 0;

	string	FullName;

	static int
			NumComponents;

	int     Index;


	// First time processing.
	{
		DT_VALUE
				Size;


		if( ( AttachmentPath.DataType == NULL ) && !AttachmentPathInitialize() )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "Unable to initialize default attachment path" );

		DT_OP( DT_MSG_SIZE, &Size, &AttachmentPath, NULL );
		NumComponents = (int) DT_VALUE_TO_NUMBER( &Size );
	}

	if( NumComponents )
		ErrorsByAttachment = (char **) calloc( NumComponents, sizeof( char * ) );

	Index = 0;
 	DTM_FOR_VALUE( &Subscript, &Value, &AttachmentPath )
	{
		ValueString.DataType = DtString;
		DTM_TO( &ValueString, &Value );
			
		Prefix 			 = (char *) DT_VALUE_TO_POINTER( &ValueString );
		FullName 		 = Prefix;
		FullName         += *Path;
		ErrClear();

		char const* tmp = FullName.c_str();
		if ( SDB_SUBDB_DELIMITER == FullName[0] ) // Absolute
		{
			if(    !SecDbParseDbName( &tmp, &pDb, DeadPoolSize, Flags ) 
				&& ( ERR_DATABASE_INVALID != ErrNum ) )
			{
				ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to parse %s", FullName.c_str() );
				return NULL;
			}
		}	
		else if( SecDbRootDb ) // Relative to current root Db, if we have a root Db
		{
			if( !SecDbParseSubDbName( &tmp, &pDb, DeadPoolSize, Flags, SecDbRootDb ) )
			{
				if( ERR_DATABASE_INVALID != ErrNum ) 
				{
					ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to parse subdb name %s", FullName.c_str() );
					return NULL;
				}
			}
            else
            {
			    SecDbReattach( SecDbRootDb );
            }
		}
		else
		{
			Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME ": Don't know how to handle %s", FullName.c_str() );
			pDb = NULL;
		}

		if( NULL != pDb )
		{
			*Path += (tmp - FullName.c_str()) - strlen( Prefix );	
			DTM_FREE( &ValueString );
			DTM_FOR_VALUE_BREAK( &Subscript, &Value, &AttachmentPath );
			break;
		}

		ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to attach to %s", FullName.c_str() );
		ErrorsByAttachment[ Index ] = strdup( ErrGetString() );

		DTM_FREE( &ValueString );
		Index++;
	}


	if( NULL == pDb )
	{
		ErrClear();
		Err( ERR_INVALID_ARGUMENTS, ERR_STR_DONT_CHANGE );

		if( !NumComponents )
			ErrMore( "Attachment path has no components" );
		else
		{
			for( Index = 0; Index < NumComponents; ++Index )
				ErrMore( "%s", ErrorsByAttachment[ Index ] ); 
		}

		ErrMore( ARGCHECK_FUNCTION_NAME ": Unable to attach to database '%s'", *Path );
	}		

	if( NumComponents )
	{
		for( Index = 0; Index < NumComponents; ++Index )
			if( ErrorsByAttachment[ Index ] )
				free( ErrorsByAttachment[ Index ] ); 
		free( ErrorsByAttachment );
	}

	return pDb;
}	



/****************************************************************
**	Routine: SecDbParseSubDbName
**	Returns: Pointer to database or NULL with Err set.
**	Action : Parse a subdb name.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbParseSubDbName"

static int SecDbParseSubDbName(
	char const** Path,
	SDB_DB **NewDb, 
	int	DeadPoolSize,
	int	Flags,
	SDB_DB *ContainingDb
)
{
	SDB_DB	*pDb = NULL;
			
	size_t
			DelimPos;

	char	Delim;
			
 
	*NewDb = NULL;
	DelimPos = strcspn( *Path, SDB_DELIMITER_STRING SDB_QUALIFIER_CLOSE_STRING ); 

	Delim = (*Path)[ DelimPos ];

	// Catch the special "." indicating the current db.

	if( ( 1 == DelimPos ) && ( '.' == **Path ) )
	{
		if( SDB_SUBDB_DELIMITER == Delim )
		{
			// Skip over it and look for more of the path.
			*Path += 2;
			return SecDbParseSubDbName( Path, NewDb, DeadPoolSize, Flags, ContainingDb );
		} 
		else
		{
			// We're not going anywhere else.
			*NewDb = ContainingDb;
			return TRUE;
		}
	}

	char sub_db_name[ SDB_SEC_NAME_SIZE ];

	int name_len = min( (int) DelimPos, (int) SDB_SEC_NAME_SIZE - 1 );
	strncpy( sub_db_name, *Path, name_len );
	sub_db_name[ name_len ] = 0;

	// Check and see if we can find a subdb object with the desired name.
	
	if( !SecDbNameLookup( sub_db_name, SecDbClassTypeFromName( "SubDatabase" ), SDB_GET_EQUAL , ContainingDb))
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME ": '%s' is not a subdb in %s", sub_db_name, ContainingDb->FullDbName.c_str() );

	{
		SDB_DB* SubDb = new SDB_DB;

		SubDb->DbOperator = SDB_DB_SUBDB;
		SubDb->Left = ContainingDb;
		SubDb->DbID = SDB_DB_VIRTUAL;
		SubDb->Qualifier = sub_db_name;

		if( NULL == (pDb = SecDbDbRegister( SubDb )))
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Unable to register subdatabase %s", sub_db_name );

		// If there is more to the path we have to attach to what we
		// have so far and continue, otherwise, attach to what we have
		// with the flags we were given.

		{
			int AttachmentFlags = ( SDB_SUBDB_DELIMITER == Delim ) ? SDB_BACK : Flags;


			if( !DbAttach( pDb, DeadPoolSize, AttachmentFlags ))
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Unable to attach to subdatabase %s", *Path );
		}

		if( SDB_SUBDB_DELIMITER == Delim ) 
		{					
			SDB_DB
					*RecursiveSubDb;


			*Path = *Path + DelimPos + 1;
			
			if( !SecDbParseSubDbName( Path, &RecursiveSubDb, DeadPoolSize, Flags, pDb ) )
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Recursive SecDbParseSubDbName() failed for subdb %s", *Path );

			pDb = RecursiveSubDb;			
		}
		else
		{
			*Path += DelimPos;
		}
	}

	*NewDb = pDb;
	return TRUE;
}



/****************************************************************
**	Routine: SecDbParseAlias
**	Returns: NULL/Err()
**	Action : Deal with an alias
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbParseAlias"

static SDB_DB *SecDbParseAlias(
	const char *Path,
	char const** AliasPath,
	int	DeadPoolSize,
	int	Flags
)
{
	SDB_DB	*TargetDb;

	char const* OriginalAliasPath = *AliasPath;


	if( NULL == ( TargetDb = SecDbParseAndAttach( AliasPath, DeadPoolSize, Flags ) ) )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Unable to attach to db alias '%s'->'%s'", Path, OriginalAliasPath );

	return TargetDb;
}



/****************************************************************
**	Routine: SecDbParseDbName
**	Returns: Pointer to database or NULL with Err set.
**	Action : Parse a db or subdb name beginning with an initial
**			 '!' to indicate that we begin at the root.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbParseDbName"

static int SecDbParseDbName(
	char const** Path,
	SDB_DB **NewDb,
	int	DeadPoolSize,
	int	Flags
)
{
	SDB_DB	*pDb = NULL;

	char	Delim;

	char const
			*OriginalPath = *Path;

	int		InitialBangs,
			DelimPos,
			AttachFlags;

			
	if( SDB_SUBDB_DELIMITER != **Path && SDB_USERDB_PREFIX != **Path )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Expected '%c' at '%s'", SDB_SUBDB_DELIMITER, *Path );
/*
**	Treat leading ~ as !Dev!Home!
*/
	if( SDB_USERDB_PREFIX == **Path )
	{
		SDB_DB	*ParentDb,
				*HomeDb;

		char const* ParentDbName = "!Dev";
		char const* HomeDbName = "Home";

		if( !SecDbParseDbName( &ParentDbName, &ParentDb, DeadPoolSize, Flags ) )
		{
			ErrMore( ARGCHECK_FUNCTION_NAME ": In attaching to '%s'", ParentDbName );
			return FALSE;
		}

		if( !SecDbParseSubDbName( &HomeDbName, &HomeDb, DeadPoolSize, Flags, ParentDb ) )
		{
			ErrMore( ARGCHECK_FUNCTION_NAME ": In attaching to '%s'", HomeDbName );
			ErrPush();
			SecDbDetach( ParentDb );
			ErrPop();
			return FALSE;
		}

		++*Path;

		if( !SecDbParseSubDbName( Path, NewDb, DeadPoolSize, Flags, HomeDb ) )
		{
			ErrPush();
			SecDbDetach( HomeDb );
			ErrPop();
			return ErrMore( ARGCHECK_FUNCTION_NAME ": In attaching to %s", *Path );
		}

		return TRUE;
	}

	// Lot's of bangs right up front are ok.
	for( InitialBangs = 1; SDB_SUBDB_DELIMITER == (*Path)[ InitialBangs ]; InitialBangs++ )
		;

	// Mark off the portion that might be a db name.

	DelimPos = InitialBangs + strcspn( *Path + InitialBangs, SDB_DELIMITER_STRING SDB_QUALIFIER_CLOSE_STRING ); 

	Delim = (*Path)[ DelimPos ];

	string db_name( *Path, DelimPos );

	// Try to attach.

	pDb = DbFromDbName( db_name.c_str() );
	if( !pDb )
		return Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME ": @ : Unknown database '%s'", OriginalPath );

	*Path += DelimPos;
		
	// There may be subdatabases specified within this db.  If so,
	// then we will attach it to the back and parse the rest of the
	// path.  Otherwise, just parse as the flags tell us to and we're
	// done.

	AttachFlags = ( SDB_SUBDB_DELIMITER == **Path ) ? SDB_BACK : Flags;

	if( pDb->DbAttributes & SDB_DB_ATTRIB_ALIAS )
	{
		char const* NewPath	= pDb->Argument.c_str();

		pDb = SecDbParseAlias( OriginalPath, &NewPath, DeadPoolSize, Flags );

		if( !pDb )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to parse alias %s", OriginalPath );
	}
	else if( !DbAttach( pDb, DeadPoolSize, AttachFlags )) 
		return ErrMore( "Unable to attach to database '%s'", pDb->FullDbName.c_str() );			

	if( SDB_SUBDB_DELIMITER == **Path ) 
	{
		++*Path;

		if( !SecDbParseSubDbName( Path, NewDb, DeadPoolSize, Flags, pDb ) )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Attaching to %s", *Path );
		
		return TRUE;
	}

	*NewDb = pDb;	
	return TRUE;
}	


/****************************************************************
**	Routine: ParseFilteredClasses
**	Returns: TRUE/Err
**	Action : Parse the given path, which may contain database
**			 search paths, subdatabase paths, modifiers, and/or
**			 aliases.  This and related support functions implement
**			 a recursive-descent parser.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "ParseFilteredClasses"

static int ParseFilteredClasses(
	char const** Path,			// path string
	SDB_DB		*pDb			// Database
)
{
   	char	ClassName[ SDB_CLASS_NAME_SIZE + 1 ];

	const char
   			*Delimiter;

   	SDB_SEC_TYPE
   			ClassId;

	size_t	sz;


   	while( TRUE )
   	{
   		++*Path;

		if( !*Path )
			break;

   		Delimiter = strpbrk( *Path, SDB_DELIMITER_AND_QUALIFIER_STRING );		// Next delimiter

   		if( !Delimiter )
   			sz = strlen( *Path );
		else
			sz = Delimiter - *Path;

   		memset( ClassName, 0, sizeof( ClassName ) );
   		strncpy( ClassName, *Path, min( sizeof( ClassName ), sz ) );

   		if( !( ClassId = SecDbClassTypeFromName( ClassName ) ) )
   		{
   			ErrMore( ARGCHECK_FUNCTION_NAME ": Invalid class '%s' in classfilter '%s'", ClassName, *Path );
   			SecDbDetach( pDb );
   			return FALSE;
   		}

   		if( !pDb->FilteredClasses )
   			pDb->FilteredClasses = HashCreate( "Class Filter", HashIntHash, NULL, 0, NULL );

   		HashInsert( pDb->FilteredClasses, (HASH_KEY) (long) ClassId, strdup( ClassName ) );

		if( !Delimiter )	// We're at the end of the string
		{
			*Path += sz;
			break;
		}
		else
			*Path = Delimiter;

   		if( SDB_CLASSFILTER_DELIMITER != *Delimiter )
   			break;
   	}

	return TRUE;
}


/****************************************************************
**	Routine: SecDbParseAndAttach
**	Returns: Pointer to database or NULL with Err set.
**	Action : Parse the given path, which may contain database
**			 search paths, subdatabase paths, modifiers, and/or
**			 aliases.  This and related support functions implement
**			 a recursive-descent parser.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbParseAndAttach"

SDB_DB *SecDbParseAndAttach(
	char const**Path,			// path string
	int			DeadPoolSize,	// Size of the databases deadpool
	int			Flags			// Placement of databases opened within the linked list
)
{
	int		Success;

	SDB_DB	*Left = NULL,
			*Right = NULL;

	SDB_QUALIFIER
			Qualifier;

	char const* OriginalPath = *Path;


	// Try to parse out an expression.

	if( ( SDB_SUBDB_DELIMITER == **Path ) || ( SDB_USERDB_PREFIX == **Path ) )
		// Could be a full DB name starting at ! (root)
		Success = SecDbParseDbName( Path, &Left, DeadPoolSize, Flags );
	else if ( SDB_QUALIFIER_ERROR != ( Qualifier = SecDbParseQualifier( Path ) ) )
		// A qualified DB.
		Success = ( NULL != ( Left = SecDbParseQualifiedExpr( Path, Qualifier, DeadPoolSize, Flags ) ) );
	else
	{		
		// Try relative to the various path locations.

		Success = ( NULL != ( Left = SecDbParseDbNameWithAttachmentPath( Path, DeadPoolSize, Flags ) ) );
	}

	if( !Success )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to %s", OriginalPath );

	// Check for classes to filter in this db

	if( SDB_CLASSFILTER_PREFIX == **Path )
	{
		SDB_DB* OldLeft = Left;
		auto_ptr<SDB_DB> FilterDb( new SDB_DB );


   		FilterDb->DbOperator = SDB_DB_CLASSFILTER;
   		FilterDb->Left = Left;
   		FilterDb->Right = NULL;
   		FilterDb->DbID = SDB_DB_VIRTUAL;

		if( !ParseFilteredClasses( Path, FilterDb.get() ) )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to parse filtered classes in %s", *Path );

   		if( !( Left = SecDbDbRegister( FilterDb.release() ) ) )
   		{
   			SecDbDetach( OldLeft );
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to register classfilterdb %s", *Path );
   		}

   		if( !DbAttach( Left, DeadPoolSize, Flags ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to classfilterdb %s", *Path );
	}
	// Check for more search path components.

	if( SDB_SEARCH_DELIMITER == **Path )
	{
		++*Path;

		if( NULL == ( Right = SecDbParseAndAttach( Path, DeadPoolSize, Flags ) ) )
		{
			SecDbDetach( Left );
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to union db %s", OriginalPath );
		}

		auto_ptr<SDB_DB> PathDb( new SDB_DB );

   		PathDb->DbOperator = SDB_DB_UNION;
   		PathDb->Left = Left;
   		PathDb->Right = Right;
   		PathDb->DbID = SDB_DB_VIRTUAL;

		SDB_DB* new_db;
   		if( !( new_db = SecDbDbRegister( PathDb.release() ) ) )
   		{
   			SecDbDetach( Left );
   			SecDbDetach( Right );
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to register union db %s", *Path );
   		}

   		if( !DbAttach( new_db, DeadPoolSize, Flags ))
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to attach to union db %s", *Path );
		Left = new_db;
   	}

	return Left;
}
