#define GSCVSID "$Header: /home/cvs/src/secdb/src/seccomp.c,v 1.14 2010/10/25 19:13:28 c03833 Exp $"
/****************************************************************
**
**	SECCOMP.C	-- SecDb slang compiler
**
**	$Log: seccomp.c,v $
**	Revision 1.14  2010/10/25 19:13:28  c03833
**	Include ccmain.h to handle errors for the entire application
**
**	Revision 1.13  2001/11/27 23:23:42  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.11  2001/04/16 18:01:44  simpsk
**	eliminate "void main"
**	
**	Revision 1.10  2000/12/19 17:20:45  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.9  2000/07/14 13:12:09  goodfj
**	Use SecDbSourceDbGet
**	
**	Revision 1.8  1999/09/01 15:29:35  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.7  1999/08/07 01:40:14  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.6  1999/03/14 16:21:39  rubenb
**	change min/max to MIN/MAX
**	
**	Revision 1.5  1998/02/24 00:38:15  singhki
**	Converted mempools to heaps, added Eval_Once, default/named arguments
**	
**	Revision 1.4  1998/01/02 21:33:22  rubenb
**	added GSCVSID
**	
**	Revision 1.3  1997/12/19 01:58:07  gribbg
**	Change SecDbError to Err and SDB_ERR to ERR
**	
**	Revision 1.2  1996/09/11 02:54:03  gribbg
**	New Trees Merge
**	
**	Revision 1.1.2.1  1996/07/25 20:05:29  lundek
**	New FastAndLoose arg to SecDbShutdown
**
**	Revision 1.1  1996/01/17 23:51:28  ERICH
**	NT Compatibility
**
**	
**	   Rev 1.1   17 Jan 1996 18:51:28   ERICH
**	NT Compatibility
**
**	   Rev 1.0   13 Dec 1995 00:42:48   LUNDEK
**	Initial revision.
**
****************************************************************/

#define BUILDING_SECDB
#define INCL_TIMEZONE
#define INCL_FILEIO
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<errno.h>
#include	<date.h>
#include	<time.h>
#include	<hash.h>
#include	<dynalink.h>
#include	<outform.h>
#include	<trap.h>
#include	<ccmain.h>
#include	<secdb.h>
#include	<secexpr.h>
#include	<kool_ade.h>
#include <sdb_x.h>

/*
**	Define constants
*/

#define	ARG_TYPE_INVALID		0
#define	ARG_TYPE_SECURITY		1
#define	ARG_TYPE_FILENAME		2
#define	ARG_TYPE_EXPRESSION		3


/*
**	Variables
*/

static char
		Buffer[ 4096 ];


/*
**	Prototype functions
*/

static void
		Message(				char *Msg ),
		ErrorMessage(			char *Msg ),
		DisplaySlangError(		char *Type ),
		ShowUsage(				void ),
		CompileAndShowResults(	SLANG_NODE *Expression, int Flags ),
		MathTrapHandler(		char *ErrText, int Continuable );



/****************************************************************
**	Routine: main
**	Returns: None
**	Action : Main module of command line script evaluator
****************************************************************/

int main(
	int		argc,		// Command line argument count
	char	*argv[]		// Command line argument vectors
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_VALUE_PARAMS
			ValueParams;

	SDB_VALUE_TYPE
			ValueTypeExpression;

	DT_VALUE
			ArrayValue;

	SLANG_NODE
			*Expression;

	int		ArgType			= ARG_TYPE_INVALID,
			InteractiveMode	= TRUE,
			ArgNum,
			Flags			= 0;

	char	*Arg,
			*DatabaseName,
			*Script;

	SDB_DB	*SourceDb;


	TIMEZONE_SETUP();


/*
**	Startup SecDb
*/

	// Check for database name
	if( argc > 1 && *argv[1] != '-' )
	{
		DatabaseName = *++argv;
		--argc;
	}
	else
		DatabaseName = "NONE";

	if( -1 == SecDbStartup( DatabaseName, SDB_DEFAULT_DEADPOOL_SIZE, Message, ErrorMessage, "SecComp", getenv( "USER_NAME" )))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		return 1;
	}



/*
**	Trap math errors
*/

	TrapMathError( MathTrapHandler );


/*
**	Parse the command line
*/

	while( argc-- > 1 )
	{
		Arg = *++argv;
		if( *Arg == '-' )
			switch( tolower( Arg[1] ))
			{
				case 'e':	ArgType = ARG_TYPE_EXPRESSION;			break;
				case 'f':	ArgType = ARG_TYPE_FILENAME;			break;
				case 's':	ArgType = ARG_TYPE_SECURITY;			break;
				default :	ShowUsage();
			}

		else
		{
			switch( ArgType )
			{
				case ARG_TYPE_SECURITY:
					SourceDb  = SecDbSourceDbGet();
			   
					if( SourceDb )
						SecPtr = SecDbGetByName( Arg, SourceDb, 0 );
					if( SourceDb && SecPtr )
					{
						ValueTypeExpression = SecDbValueTypeFromName( "Expression", NULL );
						SDB_SET_VALUE_PARAMS( &ValueParams, ValueTypeExpression, 0, 0 );
						if( !SecDbGetValue( SecPtr, &ValueParams, &Script, NULL ))
							fprintf( stderr, "Error in %s: %s\n", Arg, ErrGetString() );
						else
						{
							Expression = SlangParse( Script, Arg, SLANG_MODULE_OBJECT );
							if( Expression )
							{
								CompileAndShowResults( Expression, Flags );
								SlangFree( Expression );
							}
							else
								DisplaySlangError( "Parsing" );
						}
						SecDbFree( SecPtr );
					}
					else
						fprintf( stderr, "%s\n", ErrGetString() );
					break;

				case ARG_TYPE_FILENAME:
					Expression = SlangParseFile( Arg, Arg, SLANG_MODULE_FILE );
					if( Expression )
					{
						CompileAndShowResults( Expression, Flags );
						SlangFree( Expression );
					}
					else
						DisplaySlangError( "Parsing" );
					break;

				case ARG_TYPE_EXPRESSION:
					Expression = SlangParse( Arg, Arg, SLANG_MODULE_STRING );
					if( Expression )
					{
						CompileAndShowResults( Expression, Flags );
						SlangFree( Expression );
					}
					else
						DisplaySlangError( "Parsing" );
					break;

				case ARG_TYPE_INVALID:
					DtSubscriptSetPointer( &ArrayValue, ArgNum++, DtString, Arg );
					SlangVariableSet( "Argv", &ArrayValue, NULL, FALSE );
					break;
			}
			ArgType			= ARG_TYPE_INVALID;
			InteractiveMode	= FALSE;
		}
	}


/*
**	Interactive mode?
*/

	if( InteractiveMode )
	{
		fprintf( stderr, "Slang Compiler:\n" );
		while( gets( Buffer ))
		{
			Expression = SlangParse( Buffer, "Keyboard", SLANG_MODULE_KEYBOARD );
			if( Expression )
			{
				CompileAndShowResults( Expression, Flags );
				SlangFree( Expression );
			}
			else
				DisplaySlangError( "Parsing" );
		}
	}


/*
**	Shutdown SecDb
*/

	SecDbShutdown( TRUE );
	return 0;
}



/****************************************************************
**	Routine: Message
**	Returns: None
**	Action : Display SecDb messages
****************************************************************/

static void Message(
	char	*Msg
)
{
	if( !isatty( fileno( stderr )))
		fprintf( stderr, "%s\n", (Msg ? Msg : "" ));
	else
		fprintf( stderr, "%-78.78s\r", (Msg ? Msg : "" ));
	fflush( stderr );
}



/****************************************************************
**	Routine: ErrorMessage
**	Returns: None
**	Action : Display SecDb messages
****************************************************************/

static void ErrorMessage(
	char	*Msg
)
{
	fprintf( stderr, "ERROR: %s\n", (Msg ? Msg : "" ));
	fflush( stderr );
}



/****************************************************************
**	Routine: DisplayParseError
**	Returns: None
**	Action : Display a parser error
****************************************************************/

static void DisplaySlangError(
	char	*Type
)
{
	fprintf( stderr, "%s failed @ %s( %s ) - (%d,%d) - (%d,%d)\n%s\n",
			Type,
			SlangModuleNameFromType( SlangErrorInfo.ModuleType ),
			NULLSTR(SlangErrorInfo.ModuleName),
			SlangErrorInfo.BeginningLine,
			SlangErrorInfo.BeginningColumn,
			SlangErrorInfo.EndingLine,
			SlangErrorInfo.EndingColumn,
			ErrGetString() );
}



/****************************************************************
**	Routine: MathTrapHandler
**	Returns: None
**	Action : Handle math errors
****************************************************************/

static void MathTrapHandler(
	char	*ErrText,
	int		Continuable
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
**	Routine: ShowUsage
**	Returns: None
**	Action : Display help and exit
****************************************************************/

static void ShowUsage( void )
{
	fprintf( stderr,
			"Usage: SecComp [Database] [-e Expression] [-f Filename] [-s Security] ...\n"
			"\t-e Expression        Compile expression from the command line\n"
			"\t-f Filename          Compile script from a file\n"
			"\t-s Security          Compile Slang security\n"
			);

	exit( 1 );
}




















enum SC_MSG
{
	SC_MSG_CREATE,
	SC_MSG_BUILD_GRAPH,
	SC_MSG_C_CODE_TRAVERSE,
	SC_MSG_C_CODE_DECL,
	SC_MSG_C_CODE_PUSH,
	SC_MSG_C_CODE_FREE,
	SC_MSG_C_CODE_EXEC

};

struct SC_CONTEXT;
struct SC_VERTEX;
struct SC_SCOPE;
struct SC_SLANG_DATA;
struct SC_FUNCTION_INFO;

typedef (*SC_MSG_FUNC)( 				SC_MSG Msg, SC_CONTEXT *Context, SC_VERTEX *Vertex );

struct SC_FUNCTION_INFO
{
	SC_MSG_FUNC
			ScFunc;

	char	*FuncName;

	char	*DllPath;
};

struct SC_SCOPE
{
	HASH	*Variables;

	int		Ordinal;
};

struct SC_CONTEXT
{
	FILE	*Output,
			*Error;

	SC_SCOPE
			*FileScope,
			*LocalScope;

	int		PRetStackSize,
			CtxStackSize,
			PRetStackMaxSize,
			CtxStackMaxSize,
			IndentLevel;

	char	*IndentString;
};

struct SC_VERTEX
{
	SLANG_NODE
			*Expression;

	SC_MSG_FUNC
			VertexType;

	SC_MSG_FUNC
			ScDataType;

	int		Argc,
			Edgec,
			ArgAlloc,
			EdgeAlloc;

	SC_VERTEX
			**Argv,
			**Edgev;

	void	*VertexData;
};

struct SC_SLANG_DATA
{
	HASH	*SymTabRead,
			*SymTabWrite;
};


static HASH
		*ScFuncMap = NULL;


#define	SC_VTX_OP( Msg, Context, Vtx )				((*(Vtx)->VertexType)( Msg, Context, Vtx ))
#define	SC_DT_OP( Msg, Context, Vtx )				((*(Vtx)->ScDataType)( Msg, Context, Vtx ))
#define EDGE_ALLOC( Vertex, NumEdges, NumArgs )		\
	(( (Vertex)->EdgeAlloc >= NumEdges || ERR_REALLOC( (Vertex)->Edgev, SC_VERTEX *, ((Vertex)->EdgeAlloc = (NumEdges)) * sizeof( *(Vertex)->Edgev ))) \
	&& ( (Vertex)->ArgAlloc >= NumArgs || ERR_REALLOC( (Vertex)->Argv, SC_VERTEX *, ((Vertex)->ArgAlloc = (NumArgs)) * sizeof( *(Vertex)->Argv ))))

#define VertexFree( Vertex );		// FIX



/****************************************************************
**	Routine: LookupFuncPtr
**	Returns:
**	Action :
****************************************************************/

static SC_MSG_FUNC LookupFuncPtr(
	SLANG_FUNC	FuncPtr
)
{
	SC_FUNCTION_INFO
			*FunctionInfo;


	FunctionInfo = (SC_FUNCTION_INFO *) HashLookup( ScFuncMap, (HASH_KEY) FuncPtr );
	if( !FunctionInfo )
		return( NULL );
	return( FunctionInfo->ScFunc );
}



/****************************************************************
**	Routine: VertexCreate
**	Returns: New vertex
**	Action :
****************************************************************/

static SC_VERTEX *VertexCreate(
	SLANG_NODE 	*Expression,
	SC_MSG_FUNC	ScDataType,
	SC_CONTEXT	*Context
)
{
	SC_VERTEX
			*Vtx;


	if( !ERR_CALLOC( Vtx, SC_VERTEX, 1, sizeof( *Vtx )))
		return( NULL );
	Vtx->Expression = Expression;
	Vtx->ScDataType	= ScDataType;
	Vtx->VertexType	= LookupFuncPtr( Expression->FuncPtr );
	if( !SC_VTX_OP( SC_MSG_CREATE, Context, Vtx ))
		return( NULL );
	return( Vtx );
}


/****************************************************************
**	Routine: CScopeCreate
**	Returns:
**	Action :
****************************************************************/

static SC_SCOPE *CScopeCreate( void )
{
	SC_SCOPE
			*Scope;

	if( !ERR_CALLOC( Scope, SC_SCOPE, 1, sizeof( *Scope )))
		return( NULL );
	Scope->Variables = HashCreate( "CScope", NULL, NULL, 0, NULL );
	return( Scope );
}



/****************************************************************
**	Routine: CScopeDestroy
**	Returns:
**	Action :
****************************************************************/

static void CScopeDestroy(
	SC_SCOPE	*Scope
)
{
	HASH_ENTRY_PTR
			Enum;


	HASH_FOR( Enum, Scope->Variables )
		free( HashValue( Enum ));
	HashDestroy( Scope->Variables );
	free( Scope );
}



/****************************************************************
**	Routine: CScopeAdd
**	Returns:
**	Action :
****************************************************************/

static int CScopeAdd(
	SC_SCOPE	*Scope,
	char		*Format,
	SC_VERTEX	*Vtx
)
{
	char	NameBuf[ 256 ];


	sprintf( NameBuf, Format, Scope->Ordinal++ );
	return( HashInsert( Scope->Variables, (HASH_KEY) Vtx, (HASH_VALUE) strdup( NameBuf )));
}



/****************************************************************
**	Routine: CScopeLookup
**	Returns:
**	Action :
****************************************************************/

static char *CScopeLookup(
	SC_SCOPE	*Scope,
	SC_VERTEX	*Vtx
)
{
	return( (char *) HashLookup( Scope->Variables, (HASH_KEY) Vtx ));
}



/****************************************************************
**	Routine: CStringConst
**	Returns: Single static buffer
**	Action : Encloses string in quotes and translates newlines,
**			 double quotes, etc.
****************************************************************/

static char *CStringConst(
	char	*String
)
{
	static char
			Buffer[ 1024 ],
			*BufBnd = Buffer + sizeof( Buffer ) - 8;

	char	*BufPtr;


	if( !String )
		return( "NULL" );

	BufPtr = Buffer;
	*BufPtr++ = '\"';
	while( *String && BufPtr < BufBnd )
	{
		if( *String == '\n' )
		{
			*BufPtr++ = '\\';
			*BufPtr++ = 'n';
			String++;
		}
		else if( *String == '\r' )
		{
			*BufPtr++ = '\\';
			*BufPtr++ = 'r';
			String++;
		}
		else if( *String == '\"' || *String == '\\' )
		{
			*BufPtr++ = '\\';
			*BufPtr++ = *String++;
		}
		else if( *String < ' ' || *String > '~' )
			BufPtr += sprintf( BufPtr, "\\x%x", *String );
		else
			*BufPtr++ = *String++;
	}
	*BufPtr++ = '\"';
	*BufPtr++ = '\0';

	return( Buffer );
}



/****************************************************************
**	Routine: CNumericConst
**	Returns: Single static buffer
**	Action : C numeric constant
****************************************************************/

static char *CNumericConst(
	double	Number
)
{
	static char
			Buffer[ 64 ];


	sprintf( Buffer, "%.30g", Number );
	return( Buffer );
}



/****************************************************************
**	Routine: CFunctionConst
**	Returns:
**	Action :
****************************************************************/

static char *CFunctionConst(
	SLANG_FUNC	FuncPtr
)
{
	SC_FUNCTION_INFO
			*FunctionInfo;


	FunctionInfo = (SC_FUNCTION_INFO *) HashLookup( ScFuncMap, (HASH_KEY) FuncPtr );
	if( !FunctionInfo )
		return( NULL );
	return( FunctionInfo->FuncName );
}



/****************************************************************
**	Routine: CCodeFunctionHeader
**	Returns: TRUE or FALSE
**	Action :
****************************************************************/

static int CCodeFunctionHeader(
	const char	*CFunctionName,
	SC_CONTEXT	*Context
)
{
	HASH_ENTRY_PTR
			Enum;


	fprintf( Context->Output, "\n\n\n" );
	fprintf( Context->Output, "/****************************************************************\n" );
	fprintf( Context->Output, "**\tRoutine: %s\n", CFunctionName );
	fprintf( Context->Output, "**\tReturns: Evaluation return code (SLANG_ERROR, SLANG_OK ...,)\n" );
	fprintf( Context->Output, "**\tAction : \n" );
	fprintf( Context->Output, "****************************************************************/\n" );
	fprintf( Context->Output, "\n" );
	fprintf( Context->Output, "static int %s( SLANG_ARGS )\n", CFunctionName );
	fprintf( Context->Output, "{\n" );
	fprintf( Context->Output, "\tSCM_LOCAL(%d,%d)\n", Context->PRetStackMaxSize, Context->CtxStackMaxSize );
	HASH_FOR( Enum, Context->LocalScope->Variables )
		if( !SC_DT_OP( SC_MSG_C_CODE_DECL, Context, (SC_VERTEX *) HashKey( Enum )))
			return( FALSE );
	fprintf( Context->Output, "\n\n\tSCM_PREAMBLE(%d)\n\n", Context->PRetStackMaxSize );
	return( TRUE );
}



/****************************************************************
**	Routine: CCodeFunctionTrailer
**	Returns: TRUE or FALSE
**	Action :
****************************************************************/

static int CCodeFunctionTrailer(
	SC_CONTEXT	*Context,
	int			NullReturn
)
{
	HASH_ENTRY_PTR
			Enum;


	if( NullReturn )
		fprintf( Context->Output, "\tRet->Type = SLANG_TYPE_STATIC_VALUE;\n\tRet->Data.Value.DataType = DtNull;\n" );
	fprintf( Context->Output, "\nSCM_ERROR_LABEL:\n" );
	HASH_FOR( Enum, Context->LocalScope->Variables )
		if( !SC_DT_OP( SC_MSG_C_CODE_FREE, Context, (SC_VERTEX *) HashValue( Enum )))
			return( FALSE );
	fprintf( Context->Output, "\tif( SLANG_ERROR == RetCode )\n" );
	fprintf( Context->Output, "\t\treturn SlangEvalError( SLANG_ERROR_PARMS, NULL );\n" );
	fprintf( Context->Output, "\treturn( RetCode );\n" );
	fprintf( Context->Output, "}\n" );
	return( TRUE );
}



/****************************************************************
**	Routine: CCodeFunction
**	Returns: TRUE or FALSE
**	Action :
****************************************************************/

static int CCodeFunction(
	const char	*CFunctionName,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex,
	int			NullReturn
)
{
	SC_CONTEXT	RemContext;


	RemContext = *Context;

	Context->IndentLevel		= 1;
	Context->LocalScope			= CScopeCreate();
	Context->PRetStackSize		=
	Context->CtxStackSize		=
	Context->PRetStackMaxSize	=
	Context->CtxStackMaxSize	= 0;

	if( !SC_VTX_OP( SC_MSG_C_CODE_TRAVERSE, Context, Vertex ))
		return( FALSE );

	CCodeFunctionHeader( CFunctionName, Context );

	if( !SC_VTX_OP( SC_MSG_C_CODE_EXEC, Context, Vertex ))
		return( FALSE );

	CCodeFunctionTrailer( Context, NullReturn );

	CScopeDestroy( Context->LocalScope );

	*Context = RemContext;
	return( TRUE );
}



/****************************************************************
**	Routine: CCodeFileHeader
**	Returns:
**	Action :
****************************************************************/

static int CCodeFileHeader(
	SC_CONTEXT	*Context
)
{
	HASH_ENTRY_PTR
			Enum;

	int		RemIndentLevel;


	fprintf( Context->Output, "#include	<portable.h>\n" );
	fprintf( Context->Output, "#include	<stdlib.h>\n" );
	fprintf( Context->Output, "#include	<stdio.h>\n" );
	fprintf( Context->Output, "#include	<hash.h>\n" );
	fprintf( Context->Output, "#include	<secdb.h>\n" );
	fprintf( Context->Output, "#include	<secexpr.h>\n" );
	fprintf( Context->Output, "#include	<secview.h>\n" );
	fprintf( Context->Output, "#include	<seccomp.h>\n\n" );
	RemIndentLevel = Context->IndentLevel;
	Context->IndentLevel = 0;
	HASH_FOR( Enum, Context->FileScope->Variables )
		if( !SC_DT_OP( SC_MSG_C_CODE_DECL, Context, (SC_VERTEX *) HashKey( Enum )))
			return( FALSE );
	Context->IndentLevel = RemIndentLevel;
	return( TRUE );
}



/****************************************************************
**	Routine: CCodeFileMain
**	Returns:
**	Action :
****************************************************************/

static int CCodeFileMain(
	SC_CONTEXT	*Context
)
{
	fprintf( Context->Output, "\n\n\n\n" );
	fprintf( Context->Output, "/****************************************************************\n" );
	fprintf( Context->Output, "**	Routine: main\n" );
	fprintf( Context->Output, "**	Returns: 0 - success or exit code\n" );
	fprintf( Context->Output, "**	Action : \n" );
	fprintf( Context->Output, "****************************************************************/\n" );
	fprintf( Context->Output, "\n" );
	fprintf( Context->Output, "int main(\n" );
	fprintf( Context->Output, "	int		Argc,\n" );
	fprintf( Context->Output, "	char	**Argv\n" );
	fprintf( Context->Output, ")\n" );
	fprintf( Context->Output, "{\n" );
	fprintf( Context->Output, "	SLANG_RET\n" );
	fprintf( Context->Output, "			Ret;\n" );
	fprintf( Context->Output, "			\n" );
	fprintf( Context->Output, "	SLANG_NODE\n" );
	fprintf( Context->Output, "			Root;\n" );
	fprintf( Context->Output, "			\n" );
	fprintf( Context->Output, "	int		RetCode;\n" );
	fprintf( Context->Output, "\n" );
	fprintf( Context->Output, "\n" );
	fprintf( Context->Output, "	TIMEZONE_SETUP();\n" );
	fprintf( Context->Output, "	\n" );
	fprintf( Context->Output, "	if( -1 == SecViewStartup( NULL ))\n" );
	fprintf( Context->Output, "	{\n" );
	fprintf( Context->Output, "		fprintf( stderr, \"%%s\\n\", ErrGetString());\n" );
	fprintf( Context->Output, "		exit( 1 );\n" );
	fprintf( Context->Output, "	}\n" );
	fprintf( Context->Output, "	SecViewSlangSetContext();\n" );
	fprintf( Context->Output, "	SlangContextSet( \"Debug Function\", NULL );\n" );
	fprintf( Context->Output, "	SlangContextSet( \"Error Function\", (void *) ScSecViewErrorFunc );\n" );
	fprintf( Context->Output, "	\n" );
	fprintf( Context->Output, "	memset( &Root, 0, sizeof( Root ));\n" );
	fprintf( Context->Output, "	\n" );
	fprintf( Context->Output, "	RetCode = OuterBlock( &Root, SLANG_EVAL_RVALUE, &Ret, SlangScopeCreate());\n" );
	fprintf( Context->Output, "	if( SLANG_OK == RetCode )\n" );
	fprintf( Context->Output, "		SecViewSlangBrowseOutput();\n" );
	fprintf( Context->Output, "	\n" );
	fprintf( Context->Output, "	ErrPush();\n" );
	fprintf( Context->Output, "	SecViewShutdown();\n" );
	fprintf( Context->Output, "	ErrPop();\n" );
	fprintf( Context->Output, "	\n" );
	fprintf( Context->Output, "	if( SLANG_OK != RetCode )\n" );
	fprintf( Context->Output, "		fprintf( stderr, \"%%s\\n\", ErrGetString());\n" );
	fprintf( Context->Output, "	\n" );
	fprintf( Context->Output, "	exit( 0 );\n" );
	fprintf( Context->Output, "}\n" );
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtPRet
**	Returns: TRUE or FALSE
**	Action : Default datatype handler (usually SLANG_RET value)
****************************************************************/

int ScDtPRet(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_C_CODE_TRAVERSE:
   			Context->PRetStackSize++;
			return( CScopeAdd( Context->LocalScope, "TmpVal%d", Vertex ));

		case SC_MSG_C_CODE_DECL:
			fprintf( Context->Output, "%.*sSLANG_RET\t%s;\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_FREE:
			fprintf( Context->Output, "%.*sSLANG_RET_FREE( &%s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			fprintf( Context->Output, "%.*sSCM_PUSH_PRET( &%s, %s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ),
					CStringConst( Vertex->Expression->StringValue ));
   			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_EXEC:
			{
				int		ArgNum,
						RemPRetStack,
						RemCtxStack;

				RemPRetStack	= Context->PRetStackSize;
				RemCtxStack		= Context->CtxStackSize;

				for( ArgNum = 0; ArgNum < Vertex->Argc; ArgNum++ )
					if( !SC_VTX_OP( SC_MSG_C_CODE_PUSH, Context, Vertex->Argv[ ArgNum ] ))
						return( FALSE );

				fprintf( Context->Output, "%.*sSCM_CALL( &%s, %s, %d, %d );\n",
						Context->IndentLevel,
						Context->IndentString,
						CScopeLookup( Context->LocalScope, Vertex ),
						CScopeLookup( Context->FileScope, Vertex->Edgev[ 0 ] ),	// FIX - check existance
						Context->PRetStackSize - RemPRetStack,
						Context->CtxStackSize - RemCtxStack );

				Context->PRetStackSize	= RemPRetStack;
				Context->CtxStackSize	= RemCtxStack;
			}
			break;
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtPRetPtr
**	Returns: TRUE or FALSE
**	Action : SLANG_RET* value
****************************************************************/

int ScDtPRetPtr(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_C_CODE_TRAVERSE:
			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_DECL:
			fprintf( Context->Output, "%.*sSLANG_RET\t*%s;\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_FREE:
			fprintf( Context->Output, "%.*sSLANG_RET_FREE( %s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			fprintf( Context->Output, "%.*sSCM_PUSH_PRET( %s, %s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ),
					CStringConst( Vertex->Expression->StringValue ));
			Context->PRetStackSize++;
			return( TRUE );
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtRet
**	Returns: TRUE or FALSE
**	Action : SLANG_RET *Ret argument
****************************************************************/

int ScDtRet(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_C_CODE_TRAVERSE:
			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			fprintf( Context->Output, "%.*sSCM_PUSH_PRET( Ret, NULL );\n",
					Context->IndentLevel,
					Context->IndentString );
			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_EXEC:
			{
				int		ArgNum,
						RemPRetStack,
						RemCtxStack;

				RemPRetStack	= Context->PRetStackSize;
				RemCtxStack		= Context->CtxStackSize;

				for( ArgNum = 0; ArgNum < Vertex->Argc; ArgNum++ )
					if( !SC_VTX_OP( SC_MSG_C_CODE_PUSH, Context, Vertex->Argv[ ArgNum ] ))
						return( FALSE );

				fprintf( Context->Output, "%.*sSCM_CALL( Ret, %s, %d, %d );\n",
						Context->IndentLevel,
						Context->IndentString,
						CScopeLookup( Context->FileScope, Vertex->Edgev[ 0 ] ),	// FIX - check existance
						Context->PRetStackSize - RemPRetStack,
						Context->CtxStackSize - RemCtxStack );

				Context->PRetStackSize	= RemPRetStack;
				Context->CtxStackSize	= RemCtxStack;
			}
			break;
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtVoid
**	Returns: TRUE or FALSE
**	Action : void value
****************************************************************/

int ScDtVoid(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_C_CODE_TRAVERSE:
			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			fprintf( Context->Output, "%.*sSCM_PUSH_PRET( NULL, %s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CStringConst( Vertex->Expression->StringValue ));
			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_EXEC:
			if( Vertex->ScDataType == ScDtPRet || Vertex->ScDataType == ScDtVoid || Vertex->ScDataType == ScDtRet )
			{
				int		ArgNum,
						RemPRetStack,
						RemCtxStack;

				RemPRetStack	= Context->PRetStackSize;
				RemCtxStack		= Context->CtxStackSize;

				for( ArgNum = 0; ArgNum < Vertex->Argc; ArgNum++ )
					if( !SC_VTX_OP( SC_MSG_C_CODE_PUSH, Context, Vertex->Argv[ ArgNum ] ))
						return( FALSE );

				fprintf( Context->Output, "%.*sSCM_CALL( NULL, %s, %d, %d );\n",
						Context->IndentLevel,
						Context->IndentString,
						CScopeLookup( Context->FileScope, Vertex->Edgev[ 0 ] ),	// FIX - check existance
						Context->PRetStackSize - RemPRetStack,
						Context->CtxStackSize - RemCtxStack );

				Context->PRetStackSize	= RemPRetStack;
				Context->CtxStackSize	= RemCtxStack;
			}
			break;
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtDouble
**	Returns: TRUE or FALSE
**	Action : double value
****************************************************************/

int ScDtDouble(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_C_CODE_TRAVERSE:
			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_DECL:
			fprintf( Context->Output, "%.*sdouble\t%s;\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			fprintf( Context->Output, "%.*sSCM_PUSH_DOUBLE( %s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CNumericConst( Vertex->Expression->Value->Data.Number ));
			Context->PRetStackSize++;
			return( TRUE );
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtString(
**	Returns: TRUE or FALSE
**	Action : char* value
****************************************************************/

int ScDtString(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_C_CODE_TRAVERSE:
			Context->PRetStackSize++;
			return( TRUE );

		case SC_MSG_C_CODE_DECL:
			fprintf( Context->Output, "%.*schar\t*%s;\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->LocalScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			fprintf( Context->Output, "%.*sSCM_PUSH_STRING( %s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CStringConst( Vertex->Expression->StringValue ));
			Context->PRetStackSize++;
			return( TRUE );
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtSlangCommon
**	Returns: TRUE or FALSE
**	Action : SLANG_FUNC value
****************************************************************/

static int ScDtSlangCommon(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex,
	int			NullReturn
)
{
	SC_SLANG_DATA
			*SlangData = (SC_SLANG_DATA *) Vertex->VertexData;

	int		ArgNum;

	SC_VERTEX
			*RefVertex;


	switch( Msg )
	{
		case SC_MSG_CREATE:
			if( !ERR_MALLOC( SlangData, SC_SLANG_DATA, sizeof( *SlangData )))
				return( FALSE );
			// FIX - SlangData->SymTabRead = HashCreate...
			// FIX - SlangData->SymTabWrite = HashCreate...
			Vertex->VertexData = SlangData;
			if( !EDGE_ALLOC( Vertex, 1, 0 ))
				return( FALSE );
			Vertex->Edgev[ 0 ] = VertexCreate( Vertex->Expression, NullReturn ? ScDtVoid : ScDtRet, Context );
			Vertex->Edgec = 1;
			if( !CScopeAdd( Context->FileScope, "ScFunc%d", Vertex ))
				return( FALSE );
			return( TRUE );

		case SC_MSG_C_CODE_TRAVERSE:
			if( Vertex->Edgec != 1 )
				return( TRUE );	// FIX - Error?
			if( !CCodeFunction( CScopeLookup( Context->FileScope, Vertex ), Context, Vertex->Edgev[ 0 ], NullReturn ))
				return( FALSE );
			Vertex->Edgec = 0;
			Context->PRetStackSize++;
			Context->CtxStackSize += Vertex->Argc;
			return( TRUE );

		case SC_MSG_C_CODE_DECL:
			fprintf( Context->Output, "%.*sint %s(\tSLANG_ARGS );\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->FileScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			fprintf( Context->Output, "%.*sSCM_PUSH_SLANG( %s );\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->FileScope, Vertex ));
			Context->PRetStackSize++;
			for( ArgNum = 0; ArgNum < Vertex->Argc; ArgNum++ )
			{
				RefVertex = Vertex->Argv[ ArgNum ];
				fprintf( Context->Output, "%*.sSCM_PUSH_CTX( %s%s );\n",
						Context->IndentLevel,
						Context->IndentString,
						RefVertex->ScDataType == ScDtPRetPtr ? "" : "&",	// FIX - should check that else == ScDtDefault?
						CScopeLookup( Context->LocalScope, RefVertex ));
				Context->CtxStackSize++;
			}
			return( TRUE );
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScDtSlangNull
**	Returns: TRUE or FALSE
**	Action : SLANG_FUNC value
****************************************************************/

int ScDtSlangNull(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	return( ScDtSlangCommon( Msg, Context, Vertex, TRUE ));
}



/****************************************************************
**	Routine: ScDtSlang
**	Returns: TRUE or FALSE
**	Action : SLANG_FUNC value
****************************************************************/

int ScDtSlang(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	return( ScDtSlangCommon( Msg, Context, Vertex, FALSE ));
}



/****************************************************************
**	Routine: ScDtSlangConst
**	Returns: TRUE or FALSE
**	Action : extern SLANG_FUNC
****************************************************************/

int ScDtSlangConst(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_CREATE:
			if( !CScopeAdd( Context->FileScope, CFunctionConst( Vertex->Expression->FuncPtr ), Vertex ))
				return( FALSE );
			return( TRUE );

		case SC_MSG_C_CODE_DECL:
			fprintf( Context->Output, "%.*sint %s(\tSLANG_ARGS );\n",
					Context->IndentLevel,
					Context->IndentString,
					CScopeLookup( Context->FileScope, Vertex ));
			return( TRUE );

		case SC_MSG_C_CODE_PUSH:
			return( Err( ERR_UNSUPPORTED_OPERATION, "@: Cannot push extern SLANG_FUNC" ));
	}
	return( TRUE );
}



/****************************************************************
**	Routine: ScVtxDouble
**	Returns:
**	Action :
****************************************************************/

int ScVtxDouble(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_CREATE:
			Vertex->ScDataType = ScDtDouble;
			break;
	}
	return( SC_DT_OP( Msg, Context, Vertex ));
}



/****************************************************************
**	Routine: ScVtxString
**	Returns:
**	Action :
****************************************************************/

int ScVtxString(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	switch( Msg )
	{
		case SC_MSG_CREATE:
			Vertex->ScDataType = ScDtString;
			break;
	}
	return( SC_DT_OP( Msg, Context, Vertex ));
}



/****************************************************************
**	Routine: ScVtxBlock
**	Returns:
**	Action :
****************************************************************/

int ScVtxBlock(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	int		ArgNum;


	switch( Msg )
	{
		case SC_MSG_CREATE:	// assumes all the args are arbitrary-execute RVALUEs; SlangX function has side-effects; side-effects do not have effects on variables beyond the arg expressions
			if( !EDGE_ALLOC( Vertex, 0, Vertex->Expression->Argc ))
				return( FALSE );

			for( ArgNum = 0; ArgNum < Vertex->Expression->Argc; ArgNum++ )
			{
				Vertex->Argv[ ArgNum ] = VertexCreate( Vertex->Expression->Argv[ ArgNum ], ArgNum == Vertex->Expression->Argc - 1 ? Vertex->ScDataType : ScDtVoid, Context );
				if( !Vertex->Argv[ ArgNum ] )
					return( FALSE );
			}
			Vertex->Argc = Vertex->Expression->Argc;
			break;

		case SC_MSG_C_CODE_TRAVERSE:
			for( ArgNum = 0; ArgNum < Vertex->Argc; ArgNum++ )
				if( !SC_VTX_OP( Msg, Context, Vertex->Argv[ ArgNum ] ))
					return( FALSE );
			return( TRUE );

		case SC_MSG_C_CODE_EXEC:
			for( ArgNum = 0; ArgNum < Vertex->Argc; ArgNum++ )
				if( !SC_VTX_OP( Msg, Context, Vertex->Argv[ ArgNum ] ))
					return( FALSE );
			return( TRUE );
	}
	return( SC_DT_OP( Msg, Context, Vertex ));
}



/****************************************************************
**	Routine: ScVtxDefault
**	Returns:
**	Action :
****************************************************************/

int ScVtxDefault(
	SC_MSG		Msg,
	SC_CONTEXT	*Context,
	SC_VERTEX	*Vertex
)
{
	int		ArgNum,
			RemPRetStack,
			RemCtxStack;


	switch( Msg )
	{
		case SC_MSG_CREATE:	// assumes all the args are arbitrary-execute RVALUEs; SlangX function has side-effects; side-effects do not have effects on variables beyond the arg expressions
			if( Vertex->ScDataType == ScDtPRet || Vertex->ScDataType == ScDtVoid || Vertex->ScDataType == ScDtRet )
			{
				if( !EDGE_ALLOC( Vertex, 1, Vertex->Expression->Argc ))
					return( FALSE );

				for( ArgNum = 0; ArgNum < Vertex->Expression->Argc; ArgNum++ )
				{
					Vertex->Argv[ ArgNum ] = VertexCreate( Vertex->Expression->Argv[ ArgNum ], ScDtSlang, Context );
					if( !Vertex->Argv[ ArgNum ] )
						return( FALSE );
				}
				Vertex->Argc = Vertex->Expression->Argc;

				Vertex->Edgev[ 0 ] = VertexCreate( Vertex->Expression, ScDtSlangConst, Context );
				if( !Vertex->Edgev[ 0 ] )
					return( FALSE );
				Vertex->Edgec = 1;
			}
			break;

		case SC_MSG_C_CODE_TRAVERSE:
			if( Vertex->ScDataType == ScDtPRet || Vertex->ScDataType == ScDtVoid || Vertex->ScDataType == ScDtRet )
			{
				RemPRetStack	= Context->PRetStackSize;
				RemCtxStack		= Context->CtxStackSize;

				for( ArgNum = 0; ArgNum < Vertex->Argc; ArgNum++ )
					if( !SC_VTX_OP( Msg, Context, Vertex->Argv[ ArgNum ] ))
						return( FALSE );

				Context->PRetStackMaxSize	= MAX( Context->PRetStackMaxSize, Context->PRetStackSize - RemPRetStack );
				Context->CtxStackMaxSize	= MAX( Context->CtxStackMaxSize, Context->CtxStackSize - RemCtxStack );
				Context->PRetStackSize		= RemPRetStack;
				Context->CtxStackSize		= RemCtxStack;

				if( !SC_VTX_OP( Msg, Context, Vertex->Edgev[ 0 ] ))
					return( FALSE );
			}
			break;
	}
	return( SC_DT_OP( Msg, Context, Vertex ));
}



/****************************************************************
**	Routine: FuncMapInit
**	Returns:
**	Action :
****************************************************************/

static int FuncMapInit( void )
{
	char	TableName[ 512 ],
			Buffer[ 256 ];

	FILE	*fpTable;

	SC_FUNCTION_INFO
			*FunctionInfo;

	SLANG_FUNC
			SlangFunc;


	if( !ScFuncMap )
	{
		ScFuncMap = HashCreate( "ScFuncMap", NULL, NULL, 0, NULL );


/*
**	Predefined SlangFuncs
*/

#define REGISTER( SlangFuncExt, ScFuncName )								\
{																			\
	int SlangX##SlangFuncExt( SLANG_ARGS );									\
																			\
	if( !ERR_CALLOC( FunctionInfo, SC_FUNCTION_INFO, 1, sizeof( *FunctionInfo )))				\
		return( FALSE );													\
																			\
	FunctionInfo->DllPath	= "secdb";										\
	FunctionInfo->FuncName	= "SlangX"#SlangFuncExt;						\
	FunctionInfo->ScFunc	= ScFuncName;									\
	HashInsert( ScFuncMap, (HASH_KEY) SlangX##SlangFuncExt, (HASH_VALUE) FunctionInfo );\
}

		REGISTER( Assign, 			ScVtxDefault	);
		REGISTER( Operator,			ScVtxDefault	);
		REGISTER( LogicalAnd,		ScVtxDefault	);
		REGISTER( LogicalOr,		ScVtxDefault	);
		REGISTER( UnaryNot,			ScVtxDefault	);
		REGISTER( UnaryMinus,		ScVtxDefault	);
		REGISTER( PreIncrement,		ScVtxDefault	);
		REGISTER( PreDecrement,		ScVtxDefault	);
		REGISTER( PostIncrement,	ScVtxDefault	);
		REGISTER( PostDecrement,	ScVtxDefault	);
		REGISTER( ScopeOperator,	ScVtxDefault	);
		REGISTER( Block,			ScVtxBlock		);
		REGISTER( While,			ScVtxDefault	);
		REGISTER( ArrayInit,		ScVtxDefault	);
		REGISTER( ValueMethod,		ScVtxDefault	);
		REGISTER( DataTypeCreator,	ScVtxDefault	);
		REGISTER( Subscript,		ScVtxDefault	);
		REGISTER( Star,				ScVtxDefault	);
		REGISTER( Component,		ScVtxDefault	);
		REGISTER( Variable,			ScVtxDefault	);
		REGISTER( Null,				ScVtxDefault	);
		REGISTER( Double,			ScVtxDouble 	);
		REGISTER( String,			ScVtxString 	);
		REGISTER( Global,			ScVtxDefault	);
		REGISTER( DoGlobal,			ScVtxDefault	);
		REGISTER( Sticky,			ScVtxDefault	);
		REGISTER( StickyDiddle,		ScVtxDefault	);
		REGISTER( Null,				ScVtxDefault	);
		REGISTER( While,			ScVtxDefault	);
		REGISTER( If,				ScVtxDefault	);
		REGISTER( Link,				ScVtxDefault	);
		REGISTER( LinkFile,			ScVtxDefault	);
		REGISTER( Constant,			ScVtxDefault	);
		REGISTER( Enum,				ScVtxDefault	);

#undef REGISTER
	}


/*
**	Get all the loadable functions from slang.dat
*/

	sprintf( TableName, "%sslang.dat", SecDbTypesPath );
	if( !( fpTable = fopen( TableName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, "FuncMapInit( %s ) %s", TableName, strerror( errno ));

	if( !ERR_CALLOC( FunctionInfo, SC_FUNCTION_INFO, 1, sizeof( *FunctionInfo )))
		return( FALSE );

	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		if( Buffer[ 0 ] == '/' && Buffer[ 1 ] == '/' )
			continue;

		// Discard Name (quoted region)
		if( !strtok( Buffer, "\t\n\"" ))
			break;

		// Dll path
		if( !( FunctionInfo->DllPath = strtok( NULL, " \t\n" )))
			break;
		FunctionInfo->DllPath = strdup( FunctionInfo->DllPath );

		// Function name
		if( !( FunctionInfo->FuncName = strtok( NULL, " \t\n" )))
			break;
		FunctionInfo->FuncName = strdup( FunctionInfo->FuncName + 1 );

		FunctionInfo->ScFunc = ScVtxDefault;

		// Register the function (ignore unloaded and unloadable funcs)
		if( DynaLinkIsLoaded( FunctionInfo->DllPath ))
		{
			SlangFunc = (SLANG_FUNC) DynaLink( FunctionInfo->DllPath, FunctionInfo->FuncName );
			if( SlangFunc )
			{
				HashInsert( ScFuncMap, (HASH_KEY) SlangFunc, (HASH_VALUE) FunctionInfo );
				if( !ERR_CALLOC( FunctionInfo, SC_FUNCTION_INFO, 1, sizeof( *FunctionInfo )))
					break;
			}
		}
	}

	fclose( fpTable );
	return( TRUE );
}



/****************************************************************
**	Routine: SlangCompile
**	Returns: TRUE or FALSE
**	Action : Compile an expression
****************************************************************/

int SlangCompile(
	SLANG_NODE	*Expression,
	int			Flags,
	FILE		*Output,
	FILE		*Error
)
{
	SC_VERTEX
			*Vertex;

	SC_CONTEXT
			Context;


	if( !FuncMapInit())
		return( FALSE );

	memset( &Context, 0, sizeof( Context ));
	Context.FileScope = CScopeCreate();

	Vertex = VertexCreate( Expression, ScDtVoid, &Context );
	if( !Vertex )
		return( FALSE );


/*
**	Write a C file
*/

	Context.Output			= Output;
	Context.Error			= Error;
	Context.IndentLevel		= 1;
	Context.IndentString	= "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

	// Put String( Expression ) at top of file
	{
		SLANG_DATA_TYPE
				DataTypeSlang;

		DT_VALUE
				TmpValue,
				StringValue;

		int		Status = TRUE;

		DataTypeSlang.Expression = Expression;
		TmpValue.DataType = DtSlang;
		TmpValue.Data.Pointer = &DataTypeSlang;
		StringValue.DataType = DtString;
		Status = Status && DTM_TO( &StringValue, &TmpValue );
		if( Status )
			fprintf( Output, "#if 0%s\n#endif\n\n", StringValue.Data.Pointer );
		DTM_FREE( &StringValue );
		if( !Status )
			return( FALSE );
	}

	CCodeFileHeader( &Context );
	CCodeFunction( "OuterBlock", &Context, Vertex, TRUE );
	CCodeFileMain( &Context )

	VertexFree( Vertex );
	CScopeDestroy( Context.FileScope );

	return( TRUE );
}










/****************************************************************
**	Routine: CompileAndShowResults
**	Returns: None
**	Action : Compile an expression, and print error if it fails
****************************************************************/

static void CompileAndShowResults(
	SLANG_NODE		*Expression,
	int			 	Flags
)
{
	if( !SlangCompile( Expression, Flags, stdout, stderr ))
		fprintf( stderr, "Compilation failed:\n%s\n", ErrGetString());
}

