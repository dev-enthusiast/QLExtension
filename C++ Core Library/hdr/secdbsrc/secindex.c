#define GSCVSID "$Header: /home/cvs/src/secdb/src/secindex.c,v 1.63 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SECINDEX.C	- SecDb index utility
**
**	$Log: secindex.c,v $
**	Revision 1.63  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.62  2012/05/31 17:48:27  e19351
**	Remove OS/2 from portable and kool_ade. Move os2dir.h to shadow headers as
**	dirent.h and remove dead code from it_utils/3d that include OS/2 headers.
**	AWR: #177555
**
**	Revision 1.61  2010/10/29 20:25:03  khodod
**	Make sure to warn if SecIndex is launched against a Local database.
**	Also allow GNU-style long-option command specifications.
**	AWR: #211121
**
**	Revision 1.60  2010/10/25 19:13:44  c03833
**	Include ccmain.h to handle errors for the entire application
**
****************************************************************/

#define INCL_TIMEZONE
#include	<portable.h>
#include	<trap.h>
#include	<errno.h>
#include	<err.h>
#include	<ccmain.h>
#include	<secdb.h>
#include	<secindex.h>
#include	<secexpr.h>
#include	<secdb/sdb_exception.h>
#include	<string>

CC_USING( std::string );

#if defined( OS2 ) || defined( WIN32 )
#	include	<conio.h>
#endif

#if defined( __unix )
#	define	getch()	getchar()
#endif

/*
**	Define constants
*/

enum ARG_TYPE
{
	ARG_TYPE_INVALID,
	ARG_TYPE_DUMP_INDEX,
	ARG_TYPE_ZAP_INDEX,
	ARG_TYPE_REBUILD_INDEX,
	ARG_TYPE_VALIDATE_INDEX,
	ARG_TYPE_REPAIR_OBJECT,
	ARG_TYPE_CREATE_INDEX,
	ARG_TYPE_TRANS_SIZE,

	ARG_TYPE_ADD_CLASS_INDEX,
	ARG_TYPE_ADD_CLASS,
	ARG_TYPE_DELETE_CLASS_INDEX,
	ARG_TYPE_DELETE_CLASS,

	ARG_TYPE_LAST
};


/*
**	Variables
*/

SDB_DB	*SdbPublic;


/*
**	Prototype functions
*/

static void
		SecIndexZapIndices(		void ),
		SecIndexShowIndices(	void ),
		SecIndexDumpIndex(		SDB_INDEX *Index ),
		SecIndexCreateIndex(	char *FileName ),
		ShowUsage(				void ),
		Message(				const char *Msg );

static int
		SlangXCreateIndex(		SLANG_ARGS );

static int
		PrintFunction(			void *Handle, char *String );

static int
		AddClass(				char *IndexName, char *ClassName ),
		DeleteClass(			char *IndexName, char *ClassName );



/****************************************************************
**	Routine: MathTrapHandler
**	Returns: None
**	Action : Handle math errors
****************************************************************/

static void MathTrapHandler(
	const char	*ErrText,
	int			Continuable
)
{
	if( Continuable )
	{
		fprintf( stderr, "%s\n\nProcessing will continue, but floating point results may be incorrect", ErrText );
		fprintf( stdout, "%s\n\nProcessing will continue, but floating point results may be incorrect", ErrText );
	}
	else
	{
		fprintf( stderr, "%s\n\nProcessing will NOT continue", ErrText );
		fprintf( stdout, "%s\n\nProcessing will NOT continue", ErrText );
	}
}



/****************************************************************
**	Routine: Main
**	Returns: None
**	Action : Main
****************************************************************/

int main(
	int		argc,		// Command line argument count
	char	*argv[]		// Command line argument vectors
)
{
	SDB_INDEX
			*Index;

	enum ARG_TYPE
			ArgType = ARG_TYPE_INVALID;

	char	*Arg,
			*DatabaseName;

	char	*IndexName = NULL;


	TIMEZONE_SETUP();


/*
**	Initial check of the command line
*/

	if( argc < 2 )
		ShowUsage();

	DatabaseName = *++argv;
	--argc;


	printf( "\n"
		   	"SecIndex is potentially a very dangerous program.  Do not run it\n"
		   	"on production databases.  Do not run it on database unions.  In\n"
		   	"general, run it only on weekends.\n"
		   	"\n"
		   	"Consult with a strategist who understands SecDb indices before\n"
		   	"using this program.\n"
		   	"\n"
		   	"Hit 'return' to continue or 'Control-C' to abort." );
	fflush( stdout );
	getchar();
	
	std::string dbName = DatabaseName;
	if (dbName[0] != '!' ) {
		dbName = "!" + dbName;
	}
	if( !stricmp( dbName.substr( 0, 6 ).c_str(), "!Local" ) )
	{
		printf( 
				"\n"
				"WARNING:\n" 
				"You are running SecIndex against '%s', which is a local\n"
				"database entry. If this database is in a SecSync ring,\n" 
				"doing so will create transactions that will not be\n"
				"mirrored via SecSync. Use the fully-qualified database\n"
				"name, if that is not what you want.\n"
				"\n"
				"Hit 'return' to continue or 'Control-C' to abort.\n"
				"\n"
				, DatabaseName );
		fflush( stdout );
		getchar();
	}


/*
**	Startup SecDb
*/

	if( !(SdbPublic = SecDbStartup( DatabaseName, SDB_DEFAULT_DEADPOOL_SIZE, Message, Message, "SecIndex", getenv( "USER_NAME" ))))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		exit( 1 );
	}


/*
**	Trap math errors
*/

	TrapMathError( MathTrapHandler );


/*
**	Parse the command line
*/

	while( --argc > 0 )
	{
		Arg = *++argv;
		if( *Arg == '-' )
		{
			if(		!stricmp( Arg, "-addclass"	) || !stricmp( Arg, "--addclass" ) ) ArgType = ARG_TYPE_ADD_CLASS_INDEX;
			else if(!stricmp( Arg, "-create"	) || !stricmp( Arg, "--create" ) ) ArgType = ARG_TYPE_CREATE_INDEX;
			else if(!stricmp( Arg, "-deleteclass" ) || !stricmp( Arg, "--deleteclass" ) ) ArgType = ARG_TYPE_DELETE_CLASS_INDEX;
			else if(!stricmp( Arg, "-destroy"	) || !stricmp( Arg, "--destroy" ) )	ArgType = ARG_TYPE_ZAP_INDEX;
			else if(!stricmp( Arg, "-dump"		) || !stricmp( Arg, "--dump" ) ) ArgType = ARG_TYPE_DUMP_INDEX;
			else if(!stricmp( Arg, "-fix"		) || !stricmp( Arg, "--fix" ) ) ArgType = ARG_TYPE_REPAIR_OBJECT;
			else if(!stricmp( Arg, "-list"		) || !stricmp( Arg, "--list" ) ) SecIndexShowIndices();
			else if(!stricmp( Arg, "-rebuild" 	) || !stricmp( Arg, "--rebuild" ) ) ArgType = ARG_TYPE_REBUILD_INDEX;
			else if(!stricmp( Arg, "-size"	 	) || !stricmp( Arg, "--size" ) ) ArgType = ARG_TYPE_TRANS_SIZE;
			else if(!stricmp( Arg, "-validate"	) || !stricmp( Arg, "--validate" ) ) ArgType = ARG_TYPE_VALIDATE_INDEX;
			else if(!stricmp( Arg, "-wipeout" 	) || !stricmp( Arg, "--wipeout") ) SecIndexZapIndices();
			else
				ShowUsage();
		}
		else
		{
			switch( ArgType )
			{
				case ARG_TYPE_TRANS_SIZE:
					SecDbIndexBuildPartCount = atol( Arg );
					printf( "Transaction part count set to %ld\n",
							SecDbIndexBuildPartCount );
					break;

				case ARG_TYPE_REBUILD_INDEX:
					printf( "Rebuilding index %s...", Arg );
					fflush( stdout );
					if( SecDbIndexRebuild( Arg, SecDbRootDb ))
						printf( "\nIndex '%s' rebuilt\n", Arg );
					else
						fprintf( stderr, "\nError: %s\n", ErrGetString() );
					break;

				case ARG_TYPE_DUMP_INDEX:
					Index = SecDbIndexFromName( Arg, SecDbRootDb );
					if( Index )
						SecIndexDumpIndex( Index );
					else
						fprintf( stderr, "\nError: %s\n", ErrGetString() );
					break;

				case ARG_TYPE_VALIDATE_INDEX:
					printf( "Validating index %s...", Arg );
					fflush( stdout );
					if( SecDbIndexValidate( Arg, SecDbRootDb, 0 ))
						printf( "Index valid\n" );
					else
						fprintf( stderr, "\nError: %s\n", ErrGetString() );
					break;

				case ARG_TYPE_ZAP_INDEX:
					if( SecDbIndexDestroy( Arg, SecDbRootDb ))
						printf( "Index '%s' zapped\n", Arg );
					else
						fprintf( stderr, "Error: %s\n", ErrGetString() );
					break;

				case ARG_TYPE_REPAIR_OBJECT:
					if( SecDbIndexRepairObject( Arg, SecDbRootDb ))
						printf( "Object '%s' repaired\n", Arg );
					else
						fprintf( stderr, "Error: %s - %s\n", Arg, ErrGetString() );
					break;

				case ARG_TYPE_CREATE_INDEX:
					SecIndexCreateIndex( Arg );
					break;

				case ARG_TYPE_ADD_CLASS_INDEX:
					IndexName = Arg;
					ArgType = ARG_TYPE_ADD_CLASS;
					break;

				case ARG_TYPE_ADD_CLASS:
					if( AddClass( IndexName, Arg ))
						printf( "Class '%s' added to index '%s'\n",
								Arg, IndexName );
					else
					{
						fprintf( stderr,
								 "Error: add class '%s' to index '%s'\n",
								 Arg, IndexName );
						fprintf( stderr, "%s\n", ErrGetString() );
					}
					ArgType = ARG_TYPE_INVALID;
					break;

				case ARG_TYPE_DELETE_CLASS_INDEX:
					IndexName = Arg;
					ArgType = ARG_TYPE_DELETE_CLASS;
					break;

				case ARG_TYPE_DELETE_CLASS:
					if( DeleteClass( IndexName, Arg ))
						printf( "Class '%s' deleted from index '%s'\n",
								Arg, IndexName );
					else
					{
						fprintf( stderr,
								 "Error: delete class '%s' from index '%s'\n",
								 Arg, IndexName );
						fprintf( stderr, "%s\n", ErrGetString() );
					}
					ArgType = ARG_TYPE_INVALID;
					break;

				case ARG_TYPE_INVALID:
				case ARG_TYPE_LAST:		// appease GCC
					ShowUsage();
					break;
			}
		}
	}


/*
**	Shutdown SecDb
*/

	SecDbShutdown( TRUE );

	return 0;
}



/****************************************************************
**	Routine: AddClass
**	Returns: TRUE, if successful; FALSE, otherwise
**	Action : Add a class to an existing index.
****************************************************************/

static int AddClass(
	char *IndexName,
	char *ClassName
)
{
	SDB_SEC_TYPE
			SecurityType;


	SecurityType = SecDbClassTypeFromName( ClassName );
	if( SecurityType == 0 )
		return FALSE;

	return SecDbIndexAddClassToExisting(IndexName, SecDbRootDb, SecurityType );
}


/****************************************************************
**	Routine: DeleteClass
**	Returns: TRUE, if successful; FALSE, otherwise
**	Action : Remove a class from an existing index.
****************************************************************/

static int DeleteClass(
	char *IndexName,
	char *ClassName
)
{
	SDB_SEC_TYPE
			SecurityType;


	SecurityType = SecDbClassTypeFromName( ClassName );
	if( SecurityType == 0 )
		return FALSE;

	return SecDbIndexDeleteClassFromExisting(IndexName, SecDbRootDb,
											 SecurityType );
}


/****************************************************************
**	Routine: SecIndexShowIndices
**	Returns: None
**	Action : Display a list of indices
****************************************************************/

static void SecIndexShowIndices( void )
{
	SDB_INDEX
			*Index;

	SDB_ENUM_PTR
			EnumPtr;

	int		ClassNum,
			PartNum,
			ConstraintNum;


	for( Index = SecDbIndexEnumFirst( &EnumPtr, SecDbRootDb );
		Index;
		Index = SecDbIndexEnumNext( EnumPtr, SecDbRootDb ) )
	{
		printf( "INDEX    : %s\n", Index->Name );
		printf( "  Fields:\n" );
		for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
			printf( "    %2d - %-32s %-7s %-10s %4d\n",
					PartNum,
					Index->Parts[ PartNum ].ValueType->Name,
					(Index->Parts[ PartNum ].Flags & SDB_INDEX_NUMERIC) ? "NUMERIC" : "STRING",
					(Index->Parts[ PartNum ].Flags & SDB_INDEX_ASCENDING) ? "ASCENDING" : "DESCENDING",
					Index->Parts[ PartNum ].ByteWidth );
		printf( "  Classes:\n" );
		for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
			printf( "    %2d - %s\n", ClassNum, SecDbClassNameFromType( Index->Classes[ ClassNum ] ));
		if( Index->ConstraintCount )
		{
			printf( "  Constraints:\n" );
			for( ConstraintNum = 0; ConstraintNum < Index->ConstraintCount; ConstraintNum++ )
				if( Index->Constraints[ ConstraintNum ].DataType == DtDouble )
					printf( "    %2d - %-32s %-7s %-9s  %4d\n",
							ConstraintNum,
							Index->Constraints[ ConstraintNum ].VtName,
							"NUMERIC",
							(Index->Constraints[ ConstraintNum ].ConstraintType == SDB_CONSTRAIN_EQUAL) ? "EQUAL" : "NOT EQUAL",
							(int) Index->Constraints[ ConstraintNum ].Value.NumericValue );
				else		
					printf( "    %2d - %-32s %-7s %-9s      \"%-s\"\n",
							ConstraintNum,
							Index->Constraints[ ConstraintNum ].VtName,
							"STRING",
							(Index->Constraints[ ConstraintNum ].ConstraintType == SDB_CONSTRAIN_EQUAL) ? "EQUAL" : "NOT EQUAL",
							Index->Constraints[ ConstraintNum ].Value.StringValue );
		}
		printf( "\n" );
	}

	SecDbIndexEnumClose( EnumPtr );

	if( ERR_NONE != ErrNum )
		printf( "Failed to enumerate all indices: %s\n", ErrGetString() );
}



/****************************************************************
**	Routine: SecIndexZapIndices
**	Returns: None
**	Action : Destroy all indices
****************************************************************/

static void SecIndexZapIndices( void )
{
	SDB_INDEX
			*Index;

	SDB_ENUM_PTR
			EnumPtr;

	int		c;

	char	IndexName[ SDB_INDEX_NAME_SIZE ];


/*
**	Validate with the user!!!
*/

	printf( "Are you sure you want to zap all the indices [n]? " );
	fflush( stdout );
	c = getch();
	if( tolower(c) == 'y' )
		printf( "Yes\n" );
	else
	{
		printf( "No\n" );
		return;
	}


/*
**	Wipe out all of the indices
*/


	for ( Index = SecDbIndexEnumFirst( &EnumPtr, SecDbRootDb );
		Index;
		Index = SecDbIndexEnumNext( EnumPtr, SecDbRootDb ) )
	{
		strcpy( IndexName, Index->Name );
		if( SecDbIndexDestroy( IndexName, SecDbRootDb ))
			printf( "Index '%s' zapped\n", IndexName );
		else
			fprintf( stderr, "Error with %s: %s\n", IndexName, ErrGetString() );
	}

	SecDbIndexEnumClose( EnumPtr );

	if( ERR_NONE != ErrNum )
		printf( "Failed to zap all indices: %s\n", ErrGetString() );
}



/****************************************************************
**	Routine: SecIndexDumpIndex
**	Returns: None
**	Action : Dump a list of securities that are in an index
****************************************************************/

static void SecIndexDumpIndex(
	SDB_INDEX	*Index
)
{
	SDB_INDEX_POS
			*IndexPos;

	DT_VALUE
			StringValue;

	char	*SecName;

	long	SecNum = 0;

	int		PartNum;


	IndexPos = SecDbIndexPosCreate( Index );
	for ( SecName = SecDbIndexGet( IndexPos, SDB_GET_FIRST );
		SecName;
		SecName = SecDbIndexGet( IndexPos, SDB_GET_NEXT ) )
	{
		printf( "\"%06ld\",\"%s\"", SecNum++, SecName );
		for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		{
			StringValue.DataType = DtString;
			if( DTM_TO( &StringValue, &IndexPos->Data.Values[ PartNum ] ))
			{
				printf( ",\"%s\"", (char*) StringValue.Data.Pointer );
				DTM_FREE( &StringValue );
			}
			else
				printf( ",\"ERROR\"" );
		}
		printf( "\n" );
	}

	if( ERR_DATABASE_FAILURE == ErrNum )
		fprintf( stderr, "Error: %s\n", ErrGetString() );
	SecDbIndexPosDestroy( IndexPos );
}



/****************************************************************
**	Routine: SecIndexCreateIndex
**	Returns: None
**	Action : Create an index from an index file schema
****************************************************************/

static void SecIndexCreateIndex(
	char	*FileName
)
{
	static int
			FirstTime	= TRUE;

	SLANG_SCOPE
			*Scope;

	SLANG_NODE
			*Expression;

	SLANG_RET
			Ret;

	int		RetCode;


	if( FirstTime )
	{
		SlangFunctionRegister( "CreateIndex", "", "", SlangXCreateIndex, 0 );
		SlangContextSet( "Print Handle",	(void *) stdout			);
		SlangContextSet( "Print Function",	(void *) PrintFunction	);
		FirstTime = FALSE;
	}

	Expression = SlangParseFile( FileName, FileName, SLANG_MODULE_FILE );
	if( Expression )
	{
		Scope = SlangScopeCreate();
		RetCode = SlangEvaluate( Expression, SLANG_EVAL_LVALUE, &Ret, Scope );
		if( SLANG_OK == RetCode || SLANG_EXIT == RetCode )
		{
			SLANG_RET_FREE( &Ret );
		}
		else if (SLANG_EXCEPTION == RetCode)
		{

			CC_NS(Gs, GsString) sErr = SecDbExceptionToString(Ret);
			fprintf( stderr, "Slang Exception encountered\n %s \n", sErr.c_str());
			fprintf( stderr, "Error: %s\n", ErrGetString());
			SLANG_RET_EXCEPT_FREE( &Ret );
		}
		else
			fprintf( stderr, "Error: %s\n", ErrGetString());
		SlangFree( Expression );
		SlangScopeDestroy( Scope );
	}
	else
		fprintf( stderr, "Error: %s\n", ErrGetString());
}



/****************************************************************
**	Routine: ClassCmp
**	Returns: <, >, or == 0 like strcmp
**	Action : compares sectypes
****************************************************************/

static int ClassCmp(
	const void	*Class1,
	const void	*Class2
)
{
	return( *(SDB_SEC_TYPE *)Class2 - *(SDB_SEC_TYPE *)Class1 );
}



/****************************************************************
**	Routine: SlangXCreateIndex
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Create an index
****************************************************************/

static int SlangXCreateIndex( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "IndexName",	&DtString,	"Index name"			},
				{ "Parts",		&DtArray,	"Table of index parts"	},
				{ "Classes",	&DtArray,	"Array of classes"		}
			};

	SLANG_ARG_VARS;

	SDB_OBJECT
			*SecPtr;

	SDB_SEC_TYPE
			SecType;

	SDB_INDEX
			*Index,
			*OldIndex;

	SDB_VALUE_TYPE
			ValueType;

	DT_ARRAY
			*Array;

	char	*ClassName,
			*IndexName;

	const char
			*VtName;

	double	Order,
			Size,
			Store;

	int		Flags,
			VtNum,
			ClassNum,
			Same;

	DT_VALUE
			*TestVal;

	SLANG_ARG_PARSE();

/*
**	Preliminary validation
*/

	if( !DtArrayValidate( (DT_ARRAY *) SLANG_PTR(1), DtStructure ))
	{
		SLANG_ARG_FREE();
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s: Parts must be a table of structures\n%s", Root->StringValue, ErrGetString() );
	}

	if( !DtArrayValidate( (DT_ARRAY *) SLANG_PTR(2), DtString ))
	{
		SLANG_ARG_FREE();
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s: Classes must be an array of strings\n%s", Root->StringValue, ErrGetString() );
	}


/*
**	Create an instance of each class to force value type initialization
*/

	Array = (DT_ARRAY *) SLANG_PTR(2);
	if( Array->Size == 0 )
	{
		SLANG_ARG_FREE();
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s: Class array empty", Root->StringValue );
	}

	for( ClassNum = 0; ClassNum < Array->Size; ClassNum++ )
	{
		ClassName = (char *) Array->Element[ ClassNum ].Data.Pointer;
		if( !(SecType = SecDbClassTypeFromName( ClassName )))
		{
			SLANG_ARG_FREE();
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
		}
		if( !(SecPtr = SecDbNew( NULL, SecType, SecDbRootDb )))
		{
			SLANG_ARG_FREE();
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
		}
		SecDbFree( SecPtr );
	}


/*
**	Check all value types for correctness
*/

	Array = (DT_ARRAY *) SLANG_PTR(1);
	if( Array->Size == 0 )
	{
		SLANG_ARG_FREE();
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s: Parts table empty", Root->StringValue );
	}
	for( VtNum = 0; VtNum < Array->Size; VtNum++ )
	{
		VtName	= (char *) DtComponentGetPointer( &Array->Element[ VtNum ], "ValueType", DtString, "" );
		Store	= DtComponentGetNumber( &Array->Element[ VtNum ], "Store", DtDouble, 1.0 );
		ValueType = SecDbValueTypeFromName( VtName, NULL );
		if(	!*VtName || !ValueType || ( ( Store != 0.0 ) && ( !ValueType->DataType || !ValueType->DataType->Func )))
		{
			SLANG_ARG_FREE();
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
		}
	}


/*
**	Destroy and recreate index
*/

	IndexName = (char *) SLANG_PTR(0);
	Index = SecDbIndexNew( IndexName, SecDbRootDb );
	if( !Index )
	{
		SLANG_ARG_FREE();
		return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
	}

	// Add each value type to index parts and/or index constraints 
	Array = (DT_ARRAY *) SLANG_PTR(1);
	for( VtNum = 0; VtNum < Array->Size; VtNum++ )
	{
		VtName 	= (char *) DtComponentGetPointer( &Array->Element[ VtNum ], "ValueType", DtString, "" );
		Store	= DtComponentGetNumber( &Array->Element[ VtNum ], "Store", DtDouble, 1.0 );
		TestVal = DtComponentGet( &Array->Element[ VtNum ], "Constraint", NULL );

		if( TestVal )
		{
			Index->Flags	= SDB_INDEX_CONSTRAINED;

			if( !SecDbIndexAddConstraint( Index, VtName, SDB_CONSTRAIN_EQUAL, TestVal ))
			{
				SLANG_ARG_FREE();
				return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
			}
		}

		if( Store != 0.0 )
		{
			Order	= DtComponentGetNumber( &Array->Element[ VtNum ], "Order", DtDouble, 1.0 );
			Size	= DtComponentGetNumber( &Array->Element[ VtNum ], "Size", DtDouble, 0.0 );

			if( Order >= 0.0 )
				Flags = SDB_INDEX_ASCENDING;
			else
				Flags = SDB_INDEX_DESCENDING;

			ValueType = SecDbValueTypeFromName( VtName, NULL );
			if( !SecDbIndexAddPart( Index, ValueType, Flags, (int) Size ))
			{
				SLANG_ARG_FREE();
				return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
			}
		}
	}

	// Add each class
	Array = (DT_ARRAY *) SLANG_PTR(2);
	if( Array->Size == 0 )
	{
		SLANG_ARG_FREE();
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s: Class array empty", Root->StringValue );
	}
	for( ClassNum = 0; ClassNum < Array->Size; ClassNum++ )
	{
		ClassName = (char *) Array->Element[ ClassNum ].Data.Pointer;
		SecType = SecDbClassTypeFromName( ClassName );
		if( !SecDbIndexAddClass( Index, SecType, FALSE ))
		{
			SLANG_ARG_FREE();
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
		}
	}


/*
**	Compare to existing index -- if identical do nothing
*/

	OldIndex = SecDbIndexFromName( IndexName, SecDbRootDb );
	if( OldIndex )
	{
		Same = TRUE;
		Same = Same && Index->PartCount  == OldIndex->PartCount;
		Same = Same && Index->ClassCount == OldIndex->ClassCount;
		Same = Same && Index->ConstraintCount  == OldIndex->ConstraintCount;
		Same = Same && Index->Flags == OldIndex->Flags;
		for( VtNum = 0; Same && VtNum < Index->PartCount; VtNum++ )
		{
			Same = Same && Index->Parts[ VtNum ].ValueType 	== OldIndex->Parts[ VtNum ].ValueType;
			Same = Same && Index->Parts[ VtNum ].Flags		== OldIndex->Parts[ VtNum ].Flags;
			Same = Same && Index->Parts[ VtNum ].ByteWidth	== OldIndex->Parts[ VtNum ].ByteWidth;
		}
		if( Same )
		{
			qsort( Index->Classes, 		Index->ClassCount, sizeof( *Index->Classes ), ClassCmp );
			qsort( OldIndex->Classes,	Index->ClassCount, sizeof( *Index->Classes ), ClassCmp );
			for( ClassNum = 0; Same && ClassNum < Index->ClassCount; ClassNum++ )
				Same = Index->Classes[ ClassNum ] == OldIndex->Classes[ ClassNum ];
		}
		for( VtNum = 0; Same && VtNum < Index->ConstraintCount; VtNum++ )
		{
			Same = Same && !stricmp( Index->Constraints[ VtNum ].VtName, OldIndex->Constraints[ VtNum ].VtName );
			Same = Same && Index->Constraints[ VtNum ].DataType 		== OldIndex->Constraints[ VtNum ].DataType;
			Same = Same && Index->Constraints[ VtNum ].ConstraintType	== OldIndex->Constraints[ VtNum ].ConstraintType;
	
			if( Index->Constraints[ VtNum ].DataType == DtDouble )
				Same = Same && Index->Constraints[ VtNum ].Value.NumericValue == OldIndex->Constraints[ VtNum ].Value.NumericValue;
			else	
				Same = Same && !stricmp( Index->Constraints[ VtNum ].Value.StringValue, OldIndex->Constraints[ VtNum ].Value.StringValue );

		}

		if( Same )
		{
			Ret->Type = SLANG_TYPE_VALUE;
			Ret->Data.Value.DataType	= DtDouble;
			Ret->Data.Value.Data.Number = Err( ERR_NOTHING_CHANGED, "@ in '%s'", IndexName );;

			SLANG_ARG_FREE();

			return SLANG_OK;
		}
	}


/*
**	Go tell the database
*/

	SecDbIndexDestroy( IndexName, SecDbRootDb );
	if( !SecDbIndexAdd( Index ))
	{
		SLANG_ARG_FREE();
		return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
	}

	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value.DataType	= DtDouble;
	Ret->Data.Value.Data.Number = TRUE;

	SLANG_ARG_FREE();

	return SLANG_OK;
}



/****************************************************************
**	Routine: PrintFunction
**	Returns:  0 - success
**			 -1 - failure
**	Action : Slang evaluator print function
****************************************************************/

static int PrintFunction(
	void	*Handle,
	char	*String
)
{
	if( fputs( String, (FILE *) Handle ) >= 0 )
		return 0;

	Err( ERR_FILE_FAILURE, "fputs failed: %s", strerror( errno ) );
	return -1;
}



/****************************************************************
**	Routine: ShowUsage
**	Returns: None
**	Action : Display usage of SecIndex
****************************************************************/

static void ShowUsage( void )
{
	fputs( 

"Usage: secindex DatabaseName ...\n"
"\n"
"    --addclass    IndexName ClassName    add class to an existing index\n"
"    --create      FileName               create an index\n"
"    --deleteclass IndexName ClassName    delete class from an existing index\n"
"    --destroy     IndexName              destroy an index\n"
"    --dump        IndexName              dump the contents of an index\n"
"    --fix         ObjectName             fix objects in all indices\n"
"    --list        IndexName              list the indices\n"
"    --size        TransSize              size in parts of transactions\n"
"    --rebuild     IndexName              rebuild an index\n"
"    --validate    IndexName              validate an index\n"
"    --wipeout                            destroy all indices\n",

		   stderr );

	exit( 1 );
}



/****************************************************************
**	Routine: Message
**	Returns: None
**	Action : Display messages for SecDb
****************************************************************/

static void Message(
	const char	*Msg
)
{
	if( !Msg )
		Msg = "";

	fprintf( stderr, "%s\n", Msg );
}
