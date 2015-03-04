#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_expr.c,v 1.169 2013/08/14 19:46:21 khodod Exp $"
/****************************************************************
**
**	SDB_EXPR.C	- expression parser
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_expr.c,v $
**	Revision 1.169  2013/08/14 19:46:21  khodod
**	Begin checking return status from SlangScopeCreate. Needs more work.
**	AWR: #310820
**
**	Revision 1.168  2009/08/07 20:51:40  khodod
**	Take out the extra fprintf.
**	iBug: #61475
**
**	Revision 1.167  2009/01/15 00:48:20  khodod
**	Do not crash on large scripts.
**	iBug: #61475
**
**	Revision 1.166  2004/11/29 23:48:11  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**	
**	Revision 1.165  2004/10/20 22:19:05  khodod
**	Fixes for the SW6 compiler.
**	iBug: #16863
**	
**	Revision 1.164  2004/07/27 20:16:58  khodod
**	Read the file completely
**	iBug #12504
**	
**	Revision 1.163  2004/07/27 19:55:45  khodod
**	o Fixed validation for file-backed scripts;
**	o Corrected the SlangModuleAdd logic for multiply-parsed scripts;
**	o Modifications to index the ModuleDepTree by script basename;
**	  not fully-qualified name.
**	iBug #12504
**	
**	Revision 1.162  2004/06/10 19:46:31  khodod
**	First stab at SOX compliance.
**	iBug #12504
**	
**	Revision 1.161  2004/05/26 16:04:21  khodod
**	Reject expressions of the sort a::b() at parse time.
**	iBug: #12449
**	
**	Revision 1.160  2003/12/23 19:31:33  khodod
**	BugFix: Use the thread-safe time api routines (iBug #9277).
**	
**	Revision 1.159  2001/11/27 23:23:36  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.148  2001/08/20 22:24:18  simpsk
**	various cleanups: added missing #includes, removed deprecated CC_ macros.
**	
**	Revision 1.147  2001/08/20 20:11:40  simpsk
**	remove GsBool.
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>

#include    <secdb/slang_util.h>

#include	<secdb.h>
#include	<secexpr.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<sdb_x.h>

#include    <dtformat.h>
#include    <heap.h>
#include    <heapinfo.h>
#include    <mempool.h>
#include    <clex.h>
#include    <hash.h>
#include    <dynalink.h>
#include    <outform.h>
#include    <err.h>
#include    <kool_yap.h>

#include    <string>
CC_USING( std::string );

#include	<fstream>
CC_USING( std::ifstream );
#include    <map>
#include    <ccstl.h>

#include    <stdio.h>
#include    <stdlib.h>
#include    <errno.h>
#include    <string.h>
#include    <math.h>
#include    <timezone.h>
#include    <slang_validate.h>

#if defined( DOCUMENTATION )

/*
**	Grammar:
*/

Expression 	::=	ExpressionList
				| LBRACE ExpressionList [SEMICOLON] RBRACE
				| LPAREN Expression RPAREN
				| LBRACKET ExpressionList RBRACKET
				| [ Unary ] Expression
				| Expression Operator Expression
				| SlangCall
				| ReservedExpression
				| ConditionalExpression
				| Data Type Creator LPAREN ExpressionList RPAREN
				| ValueTypeName LPAREN ExpressionList RPAREN
				| Expression DOT Symbol LPAREN ExpressionList RPAREN
				| Expression LBRACKET ExpressionList RBRACKET
				| Number
				| String
				| Variable.

ExpressionList
			::= Expression { COMMA | SEMICOLON } ExpressionList
				| NULL.

SlangCall	::= AT_SIGN Expresssion LPAREN ExpressionList RPAREN.

SlangCall	::= DOUBLE_AT Expresssion LPAREN ExpressionList RPAREN.

ReservedExpression
			::=	ReservedFunction LPAREN ExpressionList RPAREN
				| ReservedConstant.

ConditionalExpression
			::= "if" Expression [ COLON Expression ].

ValueTypeName
			::= QUOTED_STRING | SYMBOL.
#endif


/*
**	typedefs
*/

class IncrementState
{
public:
	IncrementState( int& state, bool cond )
	  : m_state( state ),
		m_cond( cond )
	{
		if( m_cond )
			++m_state;
	}
	~IncrementState()
	{
		if( m_cond )
			--m_state;
	}
private:
	int&    m_state;

	bool const
	        m_cond;
};

typedef CC_STL_MAP( string const, SLANG_MEM_FUN_MAP ) String_To_Slang_Mem_Funs_Map;

/*
**	Define constants
*/

char const* const FILE_STATIC_SCOPE_NAME = "Private";

// Initial size of ExprListArgs stack
int const INITIAL_ARG_ALLOC		= 1024;

// One less than precedence for '@' to allow @a.b(3,4) to work.
int const PRECEDENCE_MEMBER_CALL	= 8;

int const SLANG_NODE_HEAP_INITIAL_SIZE = ((32764 - sizeof( HEAP_BLOCK )) / sizeof( SLANG_NODE ));

/*
**	Define Data
*/

SLANG_ERROR_INFO
		SlangErrorInfo;

extern HASH
		*SlangRootHash,
		*SlangFunctionHash,
		*SlangConstantsHash,
		*SlangNodeTypeHash,
		*SlangNodesByType,
		*SlangNodesByName;

static HASH
		*SlangCheckReturnHash	= 0;
DT_VALUE
		SlangLinkLog;

int		SlangCheckReturnEnabled = TRUE;

static int
		SymbolStyleTsdb = 0;

static char
		*s_SymbolStyleTsdbName = NULL;

SLANG_EXPECT_NO_VALUE_STATE
		SlangExpectNoValueState = SLANG_EXPECT_OFF;

static HEAP
		*SlangNodeHeap = NULL;

static int
		ExprListCurr = 0,
		ExprListFirst = 0;

static String_To_Slang_Mem_Funs_Map *type_name_to_mem_funs;


/*
**	ExprListArgs represents the pushdown stack which would have been constructed if we were actually
**	using yacc. It is used to parse expression lists.
**
**	The current incomplete expression list is demarcated by ( ExprListFirst, ExprListCurr ).
**	ParseExprList() ensures that there is always one empty spot before and two empty spots after
**	this expression list. ParseExpression may or may not use these spaces.
**
**	The maximum size of the stack is stored in ParseExprList::MaxArgs because it is only alloc'd/realloc'd
**	there
*/

static SLANG_NODE
		**ExprListArgs = NULL;			// [ INITIAL_ARG_ALLOC ]

static int
		ConfigEvalOnceIgnore = -1;		// set based on config variable SECDB_EVAL_ONCE_IGNORE


/*
**	Define macros
*/

#define SLANG_FUNC_LOOKUP( Node )	SlangFunctionLookup( SLANG_NODE_STRING_VALUE( (Node) ), SLANG_NODE_BUCKET( (Node) ))

#define	SLANG_NODE_TYPE_ASSIGN(NodeObject,NodeType,Ret,Flags)											\
{																										\
	if( !( (NodeObject)->FuncPtr = SlangNodeTypeLookup( NodeType, Flags )))								\
	{																									\
		ParseError( Ctx, NULL, NodeObject->ErrorInfo.ModuleName, NodeObject->ErrorInfo.ModuleType );	\
		return( Ret );																					\
	}																									\
	(NodeObject)->Type = NodeType;																		\
}

#define SLANG_NODE_CALLOC()				( (SLANG_NODE *) GSHeapCalloc( SlangNodeHeap ))


/*
**	Warning: does not recursively free subtrees, use SlangFree for that
*/

#define	SLANG_NODE_FREE( Node ) 			( GSHeapFree( SlangNodeHeap, Node ))


/*
**	pop the top exprlist off the stack
*/

#define EXPR_LIST_RESTORE()		( ExprListCurr = ExprListFirst - 1, ExprListFirst = SavedExprList )


/*
**	push a new empty exprlist on the stack, and go parse it
*/

#define EXPR_LIST_PARSE( Node_, Terminal_, EvalOnce_, PrivateScope_ )										\
	( SavedExprList = ExprListFirst, ExprListFirst = ++ExprListCurr,										\
		(ExprList = ParseExprList( Ctx, (Node_), Scope, (Terminal_), &(EvalOnce_), (PrivateScope_) )) < 0	\
			? ( EXPR_LIST_RESTORE(), FALSE )																\
			: TRUE )


/*
**	set node arguments from Args array
*/

#define SLANG_NODE_SET_ARGS( Node_, Args_, Num_ )														\
	{																									\
		(Node_)->Argc = (Num_);																			\
		(Node_)->Argv = (SLANG_NODE **) ((Num_) > 0 ? malloc( sizeof( SLANG_NODE * ) * (Num_)) : NULL);	\
		memcpy( (Node_)->Argv, (Args_), sizeof( SLANG_NODE * ) * (Num_) );								\
	}


/*
**	copy the top exprlist off the stack into the node's argument list and pop it
*/

#define SLANG_NODE_EXPR_LIST_COPY( Node_, First_ )																		\
	{																													\
		(Node_)->Argv = (SLANG_NODE **) ((Node_)->Argc > 0 ? malloc( sizeof( SLANG_NODE * ) * (Node_)->Argc ) : NULL);	\
		memcpy( (Node_)->Argv, ExprListArgs + (First_), sizeof( SLANG_NODE * ) * (Node_)->Argc );						\
		EXPR_LIST_RESTORE();																							\
	}


/*
**	used to debug eval once
*/

#define SlangEvalOnceIgnore()																										\
	( ConfigEvalOnceIgnore == -1 ?																									\
	  	( ConfigEvalOnceIgnore = stricmp( SecDbConfigurationGet( "SECDB_EVAL_ONCE_IGNORE", NULL, NULL, "FALSE" ), "TRUE" ) == 0 )	\
	    : ConfigEvalOnceIgnore )


/*
**	Prototypes
*/

static SLANG_NODE
		*Parse(						CLEXCTX *Ctx, const char *ModuleName, int ModuleType, const char *PrivateScope ),
		*ParseExpression(			CLEXCTX *Ctx, int Precedence, const char *ModuleName, int ModuleType, SLANG_SCOPE *Scope, const char *PrivateScope );

static int
		ParseExprList(				CLEXCTX *Ctx, SLANG_NODE *Node, SLANG_SCOPE *Scope, TOKEN TerminalToken, int *EvalOnce, const char *PrivateScope );

static DT_VALUE
		*SlangConstantLookup(		SLANG_NODE *Node );

static SLANG_NODE
		*ParseError(				CLEXCTX	*Ctx, const char *ErrorText, const char *ModuleName, int ModuleType );

static void
		SlangNodeFreeContents(		SLANG_NODE	*Root );

static SLANG_NODE
		*SlangNodeSetEvalOnce(		SLANG_NODE	*Node );



/****************************************************************
**	Routine: SlangCLexNew
**	Returns: New CLex context with slang defaults
**	Action : Creates a CLex context
****************************************************************/

CLEXCTX *SlangCLexNew(
	const char	*File,			// File to use
	const char	*String			// String to use
)
{
	CLEXCTX	*Ctx = CLexStart( File, String, FALSE );


	if( Ctx )
	{
		Ctx->ws_sym = TRUE;
		Ctx->tab_size = 4;		// FIX- should get from window library
	}
	return Ctx;
}



/****************************************************************
**	Routine: SlangParse
**	Returns: Pointer to root of parsed expression
**			 NULL if error during parsing
**	Summary: Parse a string into an expression
****************************************************************/

SLANG_NODE *SlangParse(
	const char	*String,		// String to parse
	const char	*ModuleName,	// Name of module (used for errors)
	int			ModuleType		// Type of module (used for errors)
)
{
	return Parse( SlangCLexNew( NULL, String ), ModuleName, ModuleType, ModuleName );
}

/****************************************************************
**	Routine: SlangParseWithScope
**	Returns: Pointer to root of parsed expression
**	         NULL if error during parsing
**	Summary: Parse a string into an expression
**	         Optionally uses a supplied name for the private scope
****************************************************************/

SLANG_NODE *SlangParseWithScope(
	const char	*String,		// String to parse
	const char	*ModuleName,	// Name of module (used for errors)
	int			ModuleType,		// Type of module (used for errors)
	const char	*PrivateScope	// Name of the private scope
)
{
	return Parse( SlangCLexNew( NULL, String ), ModuleName, ModuleType,
				  ( PrivateScope && PrivateScope[0] ) ? PrivateScope : ModuleName );
}

/****************************************************************
**	Routine: SlangParseFile
**	Returns: Pointer to root of parsed expression
**			 NULL if error during parsing
**	Summary: Parse a file as a slang expression
****************************************************************/

SLANG_NODE *SlangParseFile(
	const char	*File,			// Name of file to read and parse
	const char	*ModuleName,	// Name of module (used for errors)
	int			ModuleType		// Type of module (used for errors)
)
{
	return Parse( SlangCLexNew( File, NULL ), ModuleName, ModuleType, ModuleName );
}



/****************************************************************
**	Routine: SlangParseCLex
**	Returns: Pointer to root of parsed expression, NULL if error
**	Action : Parse given a lex context
****************************************************************/

SLANG_NODE *SlangParseCLex(
	CLEXCTX		*Ctx,			// Lex context
	const char	*ModuleName,	// Name of module (for errors)
	int			ModuleType		// Type of moule (used for errors)
)
{
	return Parse( Ctx, ModuleName, ModuleType, ModuleName );
}



/****************************************************************
**	Routine: SlangParseAttach
**	Returns: Pointer to node being attached
**	Summary: Parse and attach result to existing parse tree
**	Action : Attach a newly parsed piece of slang to an existing
**			 parse tree.
****************************************************************/

SLANG_NODE *SlangParseAttach(
	SLANG_NODE	*Root,			// Root to attach to
	const char	*File,			// Name of file to read and parse
	const char	*String,		// String to parse
	const char	*ModuleName,	// Name of module
	int			ModuleType		// Type of module
)
{
	SLANG_NODE
			*Node;


	Node = Parse( SlangCLexNew( File, String ), ModuleName, ModuleType, ModuleName );
	if( Node )
	{
		Root->Argv = (SLANG_NODE **) realloc( Root->Argv, sizeof( SLANG_NODE * ) * ++Root->Argc );
		Root->Argv[ Root->Argc - 1 ] = Node;

		SlangModuleAttachChild( &Root->ErrorInfo, &Node->ErrorInfo );
	}

	return( Node );
}



/****************************************************************
**	Routine: Parse
**	Returns: Guts of SlangParse and SlangParseFile
**	Action : Parses the string or file.
****************************************************************/

static SLANG_NODE *Parse(
	CLEXCTX		*Ctx,			// CLex context representing token stream
	const char	*ModuleName,	// Name of module
	int			ModuleType,		// Type of module
	const char	*PrivateScope	// Name of the private scope
)
{
	SLANG_NODE
			*Root = NULL;

	SLANG_SCOPE
			*Scope = NULL;

	int		ExprList,
			SavedExprList,
			EvalOnce;

	static FILE
			*LinkLog;

	static int
			FirstTime = TRUE;

	static HASH
			*ModulesBeingParsed = NULL;

	bool ModuleIsSafe = false;

	if( !Ctx )
		return NULL;

	if( FirstTime )
	{
		char	*LogFileName;

		// alloc the slang node heap
		if( !( SlangNodeHeap = GSHeapCreate( "SlangNodeHeap", sizeof( SLANG_NODE ), SLANG_NODE_HEAP_INITIAL_SIZE )))
			goto Abort;

		LogFileName = getenv("SECDB_LINK_LOG");
		if( LogFileName && *LogFileName )
		{
			if ( *LogFileName == '-' )
				LinkLog = stderr;
			else
				LinkLog = fopen( LogFileName, "a" );
		}
		ModulesBeingParsed = HashCreate( "Modules being Parsed", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );
		DTM_ALLOC( &SlangLinkLog, DtStructure );

		FirstTime = FALSE;
	}


	if( ModuleType != SLANG_MODULE_STRING )
	{
		if( HashLookup( ModulesBeingParsed, ModuleName ))
		{
			ParseError( Ctx, NULL, ModuleName, ModuleType );
			Err( ERR_UNSUPPORTED_OPERATION, "Parse cycle: already parsing this module: %s", ModuleName );
			goto Abort;
		}
		HashInsert( ModulesBeingParsed, ModuleName, (HASH_VALUE) 1 );

		string FileBuf; 
		const char *CodeBuffer = 0;
		if( Ctx->string )
			CodeBuffer = Ctx->string;
		else if( Ctx->filename )
		{
			string Line;
			ifstream fs( Ctx->filename );
			if( fs )
			{
				char Buf[ 512 ];
				while( fs.read( Buf, sizeof( Buf ) - 1 ) )
				{
					Buf[ fs.gcount() ] = 0;
					FileBuf += Buf;
				}
				if( fs.gcount() )
				{
					Buf[ fs.gcount() ] = 0;
					FileBuf += Buf;
				}
				CodeBuffer = FileBuf.c_str();
			}
		}
		if( CodeBuffer )
			ModuleIsSafe = SlangModuleVerify( ModuleName, CodeBuffer );
        
		if( LinkLog )
		{
			time_t	now;
			struct tm tm;

			memset( &tm, 0, sizeof( tm ) );
			time( &now );
			(void) localtime_r( &now, &tm ); 

			fprintf( LinkLog,
				"LINK %02d%02d%02d %02d:%02d:%02d %-16.16s %-16.16s %d|%-6.6s|%-.32s\n",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
				tm.tm_hour, tm.tm_min, tm.tm_sec,
				SecDbApplicationName, SecDbUserName,
                ModuleType, 
				( ModuleIsSafe ? "safe" : "unsafe" ), 
				ModuleName );
			fflush( LinkLog );
		}

		ERR_OFF();
		DtComponentSetNumber( &SlangLinkLog, ModuleName, DtDouble, DtComponentGetNumber( &SlangLinkLog, ModuleName, DtDouble, 0.0 ) + 1.0 );
		ERR_ON();
	}


/*
**	Create a parse-time scope
*/

	Scope = SlangScopeCreate();
	if( !Scope )
		goto Abort;

/*
**	Create the root node
*/

	if( !(Root = SLANG_NODE_CALLOC()))
	{
		ParseError( Ctx, "Memory allocation failed", ModuleName, ModuleType );
		goto Abort;
	}

	if( ModuleName )
	{
		Root->ErrorInfo.ModuleName = strdup( ModuleName );
		Root->Flags |= SLANG_NODE_FREE_MODULE;
	}
	else
		Root->ErrorInfo.ModuleName = NULL;
	Root->ErrorInfo.ModuleType		= ModuleType;
	Root->ErrorInfo.BeginningLine	= SlangErrorInfo.BeginningLine		= 1;
	Root->ErrorInfo.BeginningColumn	= SlangErrorInfo.BeginningColumn	= 1;
    
	SlangModuleAdd( &Root->ErrorInfo, ModuleIsSafe );

	SLANG_NODE_TYPE_ASSIGN( Root, SLANG_BLOCK, NULL, NULL );

	if( EXPR_LIST_PARSE( Root, EOF_TOKEN, EvalOnce, PrivateScope ))
	{
		SLANG_NODE_EXPR_LIST_COPY( Root, ExprList );

		Root->ErrorInfo.EndingLine		= Ctx->end_line;
		Root->ErrorInfo.EndingColumn	= Ctx->end_char;
	}
	else
	{
		SlangNodeFreeContents( Root );
		SLANG_NODE_FREE( Root );
		Root = NULL;
	}


/*
**	Cleanup
*/

	SlangScopeDestroy( Scope );

DoReturn:

	SlangModuleRefreshSafetyInfo( ModuleName );
	if( ModuleType != SLANG_MODULE_STRING )
		HashDelete( ModulesBeingParsed, ModuleName );

	clex_stop( Ctx );

	return( Root );

Abort:
    free( Root );
    if( Scope )
        SlangScopeDestroy( Scope );
	goto DoReturn;
}



/****************************************************************
**	Routine: SlangParseTreeCopy
**	Returns: A copy of the given tree NULL if error
**	Summary: Duplicate a parse tree
**	Action : Create a duplicate copy of a parse tree
****************************************************************/

SLANG_NODE *SlangParseTreeCopy(
	SLANG_NODE	*Root	// Copy from here down
)
{
	return SlangParseTreeCopyInt( Root, NULL, NULL );
}



/****************************************************************
**	Routine: SlangParseTreeCopyInt
**	Returns: A copy (using the given mempool) of the given tree
**	Action :
****************************************************************/

SLANG_NODE *SlangParseTreeCopyInt(
	SLANG_NODE	*Node,			// Copy from here down
	const char	*ModuleName,	// Module name passed down
	DT_SLANG	*Func			// Slang Func in which to fill in local variables
)
{
	SLANG_NODE
			*Target;

	int		ArgNum;


	if( !(Target = SLANG_NODE_CALLOC()))
	{
		Err( ERR_MEMORY, "Memory allocation failed" );
		return( NULL );
	}

	Target->Type		= Node->Type;
	Target->FuncPtr		= Node->FuncPtr;
	Target->Extra		= Node->Extra;
	Target->Flags		= Node->Flags;
	Target->Argc		= Node->Argc;
	Target->Argv		= Node->Argv;

	Target->ErrorInfo	= Node->ErrorInfo;
	if( Target->Flags & SLANG_NODE_FREE_MODULE || !ModuleName )
	{
		ModuleName = Target->ErrorInfo.ModuleName = strdup( Target->ErrorInfo.ModuleName );
		Target->Flags |= SLANG_NODE_FREE_MODULE;
	}
	else
	{
		Target->ErrorInfo.ModuleName = ModuleName;
		Target->Flags &= ~SLANG_NODE_FREE_MODULE;
	}

	if( Node->Type == SLANG_TYPE_CONSTANT || Node->Flags & SLANG_NODE_STRING_EQ_VALUE )
	{
		DT_VALUE
				Src,
				Dest;

		SLANG_NODE_GET_VALUE( Node, &Src );
		DTM_ASSIGN( &Dest, &Src );
		SLANG_NODE_SET_VALUE( Target, &Dest );
	}
	else
	{
		Target->ValueDataType	= Node->ValueDataType;
		Target->ValueData		= Node->ValueData;
	}

	if( SLANG_NODE_FREE_STRING & Node->Flags )
		Target->StringValue = strdup( Node->StringValue );
	else if( SLANG_NODE_STRING_EQ_VALUE & Node->Flags )
		Target->StringValue = (char *) Target->ValueData.Pointer;
	else
		Target->StringValue = Node->StringValue;

	if( Target->StringValue && !( Target->Flags & SLANG_NODE_NO_BUCKET ))
		SLANG_NODE_BUCKET_SET( Target, SLANG_NODE_BUCKET( Node ));

	if( Func )
	{
		int LocalNum;
		if( Node->Type == SLANG_TYPE_LOCAL_VARIABLE && !Func->LocalNames[ LocalNum = SLANG_NODE_BUCKET( Node )])
		{
			Func->LocalNames[ LocalNum ] = SLANG_NODE_STRING_VALUE( Target );
			HashInsert( Func->Locals, SLANG_NODE_STRING_VALUE( Target ), (HASH_VALUE) ( LocalNum + 1 ));
		}
		else if( Node->Flags & SLANG_NODE_PUSH_SCOPE )
			Func = NULL;
	}

	if( Node->Argc )
	{
 		Target->Argv = (SLANG_NODE **) calloc( Node->Argc, sizeof( SLANG_NODE * ));
 		for( ArgNum = 0; ArgNum < Node->Argc; ArgNum++ )
 			if( !( Target->Argv[ ArgNum ] = SlangParseTreeCopyInt( Node->Argv[ ArgNum ], ModuleName, Func )))
			{
				SlangFree( Target );
				return NULL;
			}
 	}

	if( Node->Type == SLANG_TYPE_SCOPED_VARIABLE )
		SlangVariableIncrRef( (SLANG_VARIABLE *) SLANG_NODE_BUCKET( Node->Argv[ 1 ]));

	return( Target );
}



/****************************************************************
**	Routine: SlangParseTreeEqual
**	Returns: TRUE if equal, FALSE if not
**	Summary: Compare two parse trees
**	Action : Recursively compare two parse trees and return
**			 TRUE for equal and FALSE for not equal.
****************************************************************/

int SlangParseTreeEqual(
	SLANG_NODE const *NodeA,	// Compare from here down
	SLANG_NODE const *NodeB
)
{
	int		ArgNum;


	if( 	   NodeA->Type 				!= NodeB->Type
			|| NodeA->Argc 				!= NodeB->Argc
			|| NodeA->FuncPtr			!= NodeB->FuncPtr
			|| NodeA->Extra				!= NodeB->Extra
			|| !!SLANG_NODE_STRING_VALUE( NodeA ) != !!SLANG_NODE_STRING_VALUE( NodeB )
			|| NodeA->ValueDataType		!= NodeB->ValueDataType )
		return false;

	if( NodeA->ValueDataType && !( NodeA->Flags & SLANG_NODE_NO_VALUE ))
	{
		DT_VALUE
				ValueA, ValueB, Result;


		SLANG_NODE_GET_VALUE( NodeA, &ValueA );
		SLANG_NODE_GET_VALUE( NodeB, &ValueB );
		if( !DT_OP( DT_MSG_EQUAL, &Result, &ValueA, &ValueB ) || Result.Data.Number == 0.0 )
			return false;
	}

 	if( SLANG_NODE_STRING_VALUE( NodeA ) && stricmp( SLANG_NODE_STRING_VALUE( NodeA ), SLANG_NODE_STRING_VALUE( NodeB ) ))
		return false;

 	for( ArgNum = 0; ArgNum < NodeA->Argc; ArgNum++ )
 		if( !SlangParseTreeEqual( NodeA->Argv[ ArgNum ], NodeB->Argv[ ArgNum ] ))
 			return false;

	return true;
}



/****************************************************************
**	Routine: SlangParseTreeMemSize
**	Returns: Memory used by parse tree
**	Action : Returns size of parse tree from Root down
****************************************************************/

int SlangParseTreeMemSize(
	SLANG_NODE const *Root
)
{
	int		ArgNum,
			Size;


	if( !Root )
		return 0;

	Size = SlangNodeHeap->ElementAllocSize;
	if( Root->Argc && Root->Argv )
		Size += GSHeapMemSize( Root->Argv );
	// else Argv is not a valid pointer

	if( Root->Flags & SLANG_NODE_FREE_STRING )
		Size += GSHeapMemSize( Root->StringValue );

	if( ( Root->Flags & SLANG_NODE_STRING_EQ_VALUE || Root->Type == SLANG_TYPE_CONSTANT ))
	{
		DT_VALUE
				Value, Result;

		SLANG_NODE_GET_VALUE( Root, &Value );
		if( DT_OP( DT_MSG_MEMSIZE, &Result, &Value, NULL ))
			Size += (int) Result.Data.Number;
	}

	for( ArgNum = 0; ArgNum < Root->Argc; ++ArgNum )
		Size += SlangParseTreeMemSize( Root->Argv[ ArgNum ]);

	return Size;
}



/****************************************************************
**	Routine: SlangFree
**	Returns: Nothing
**	Summary: Free a parsed expression
****************************************************************/

void SlangFree(
	SLANG_NODE	*Root		// Parsed expression
)
{
	if( !Root || Root->Flags & SLANG_NODE_DONT_FREE )
		return;

	SlangNodeFreeContents( Root );

	SLANG_NODE_FREE( Root );
}



/****************************************************************
**	Routine: ParseExpression
**	Returns: A parse tree or NULL
**	Action :
**				Expression 	::=	| LBRACE ExpressionList [SEMICOLON] RBRACE
**								| LPAREN Expression RPAREN
**								| LBRACKET ExpressionList RBRACKET
**								| [ Unary ] Expression
**								| Expression Operator Expression
**								| SlangCall
**								| ReservedExpression
**								| ConditionalExpression
**								| Data Type Creator LPAREN ExpressionList RPAREN
**								| ValueTypeName LPAREN ExpressionList RPAREN
**								| Expression DOT Symbol LPAREN ExpressionList RPAREN
**								| Expression LBRACKET ExpressionList RBRACKET
**								| Number
**								| String
**								| Variable.
**
****************************************************************/

static SLANG_NODE *ParseExpression(
	CLEXCTX		*Ctx,
	int			Precedence,
	const char	*ModuleName,
	int			ModuleType,
	SLANG_SCOPE	*Scope,
	const char	*PrivateScope
)
{
	SLANG_NODE
			*Node,
			*tmpNode,
			*TempExpr;

	TOKEN	NextToken;

	SLANG_NODE_TYPE
			NextType;

	int
			SavedExprList,
			ExprList,
			EvalOnce,
			NodeParseFlags;

	SLANG_FUNCTION_INFO
			*FuncInfo;


	if( !(Node = SLANG_NODE_CALLOC()))
		return( ParseError( Ctx, "Memory allocation failed", ModuleName, ModuleType ));

	Node->Type = -1;

	NextToken = clex_peektok( Ctx );

    // Make sure not to overflow the ErrorInfo fields.
	if( Ctx->peek->end_char >= SHRT_MAX || Ctx->peek->end_line >= SHRT_MAX )
	{
		char error_msg[128];
		sprintf( error_msg, 
				"Input too long: Column %d (Max: %d) Line %d (Max: %d)", 
				 Ctx->peek->end_char, 
				 SHRT_MAX,
				 Ctx->peek->end_line, 
				 SHRT_MAX );
		ErrClear(); // ParseError() will do an ErrMore unless EOF.
		ParseError( Ctx, error_msg, ModuleName, ModuleType );
		goto AbortFreeNode;
	}
	if( Ctx->peek->end_char >= SHRT_MAX )
	{
		char error_msg[32];
		sprintf( error_msg, "Input too long: Char %d (Max: %d)", Ctx->peek->end_char, SHRT_MAX );
		ErrClear(); // ParseError() will do an ErrMore unless EOF.
		//Ctx->token = EOF_TOKEN;
		ParseError( Ctx, error_msg, ModuleName, ModuleType );
		goto AbortFreeNode;
	}

	Node->ErrorInfo.ModuleName		= ModuleName;
	Node->ErrorInfo.ModuleType		= ModuleType;
	Node->ErrorInfo.BeginningLine	= SlangErrorInfo.BeginningLine		= Ctx->peek->start_line;
	Node->ErrorInfo.BeginningColumn	= SlangErrorInfo.BeginningColumn 	= Ctx->peek->start_char + 1;

	if( LBRACE == NextToken )
	{
		clex_gettok( Ctx );

		SLANG_NODE_TYPE_ASSIGN( Node, SLANG_BLOCK, NULL, NULL );

		if( !EXPR_LIST_PARSE( Node, RBRACE, EvalOnce, PrivateScope ))
			goto AbortFreeNode;
		SLANG_NODE_EXPR_LIST_COPY( Node, ExprList )
	}
	else if( LPAREN == NextToken )
	{
		clex_gettok( Ctx );

		SLANG_NODE_FREE( Node );
		if( !( Node = ParseExpression( Ctx, 0, ModuleName, ModuleType, Scope, PrivateScope )))
			return NULL;

		if( RPAREN != clex_gettok( Ctx ))
		{
			ParseError( Ctx, "Expected RPAREN", ModuleName, ModuleType );
			goto AbortFreeNode;
		}
	}
	else if( LBRACKET == NextToken )
	{
		clex_gettok( Ctx );

		SLANG_NODE_TYPE_ASSIGN( Node, SLANG_ARRAY_INIT, NULL, NULL );

		if( !EXPR_LIST_PARSE( Node, RBRACKET, EvalOnce, PrivateScope ))
			goto AbortFreeNode;
		SLANG_NODE_EXPR_LIST_COPY( Node, ExprList )
		if( EvalOnce )
			Node = SlangNodeSetEvalOnce( Node );
	}
	else if( NUMBER == NextToken )
	{
		clex_gettok( Ctx );

		SLANG_NODE_TYPE_ASSIGN( Node, SLANG_TYPE_CONSTANT, NULL, NULL );

		SLANG_NODE_SET_NUMBER( Node, DtDouble, Ctx->number );
	}
	else if( QUOTED_STRING == NextToken )	// String
	{
		clex_gettok( Ctx );
		SLANG_NODE_TYPE_ASSIGN( Node, SLANG_TYPE_CONSTANT, NULL, NULL );
		SLANG_NODE_SET_POINTER( Node, DtString, Ctx->quoted_string );
		// for old code compatability, until StringValue goes away
		SLANG_NODE_STRING_VALUE_SET( Node, (char *) Node->ValueData.Pointer );
		Node->Flags |= SLANG_NODE_STRING_EQ_VALUE;

		// Handle quoted quotes
		while( QUOTED_STRING == clex_peektok( Ctx ))
		{
			char	*tmpString;

			clex_gettok( Ctx );
			tmpString 			= (char *) Node->ValueData.Pointer;
			SLANG_NODE_SET_POINTER( Node, DtString, StrPaste( tmpString, "\"", Ctx->quoted_string, NULL ));
			SLANG_NODE_STRING_VALUE_SET( Node, (char *) Node->ValueData.Pointer );
			free( Ctx->quoted_string );
			free( tmpString );
		}
		SLANG_NODE_BUCKET_SET( Node, HashStrHash( SLANG_NODE_STRING_VALUE( Node ) ));
	}
	else if( SYMBOL == NextToken )	// Variable or Constant or ReservedFunc with no_parens
	{
		DT_VALUE
				*Constant;

		clex_gettok( Ctx );
		SLANG_NODE_SET_POINTER( Node, DtString, strdup( Ctx->symbol ));
		SLANG_NODE_STRING_VALUE_SET( Node, (char *) Node->ValueData.Pointer );
		Node->Flags |= SLANG_NODE_STRING_EQ_VALUE;
		SLANG_NODE_BUCKET_SET( Node, HashStrHash( SLANG_NODE_STRING_VALUE( Node ) ));

		if(( FuncInfo = SLANG_FUNC_LOOKUP( Node )) && ( FuncInfo->ParseFlags & SLANG_NO_PARENS ))
		{
			SLANG_NODE
					*Args[ 2 ];

			int		NumArgs = 0;
			
			if( FuncInfo->Func) {
				Node->FuncPtr = FuncInfo->Func;
			} else {
				Node->FuncPtr = SlangXDelayLoad;
			}
			Node->Type = SLANG_RESERVED_FUNC;
			Node->Flags = SLANG_NODE_NO_VALUE;
			free( Node->ValueData.Pointer );
			Node->ValueData.Pointer = FuncInfo;
			Node->StringValue = FuncInfo->Name;
			NumArgs = 0;
			EvalOnce = TRUE;

			if( FuncInfo->ParseFlags & SLANG_BIND_BLOCK )
			{
				IncrementState state( SymbolStyleTsdb, Node->StringValue == s_SymbolStyleTsdbName );

				if( !( Args[ NumArgs++ ] = ParseExpression( Ctx, 0, ModuleName, ModuleType, Scope, PrivateScope )))
					goto AbortFreeNode;
				EvalOnce = SLANG_NODE_IS_EVAL_ONCE( Args[ 0 ]);
			}
			if( FuncInfo->ParseFlags & SLANG_BIND_COLON && COLON == clex_peektok( Ctx ))
			{
	 			clex_gettok( Ctx );
				if( !( Args[ NumArgs++ ] = ParseExpression( Ctx, 0, ModuleName, ModuleType, Scope, PrivateScope )))
				{
					SlangFree( Args[ 0 ]);
					goto AbortFreeNode;
				}
				EvalOnce = EvalOnce && SLANG_NODE_IS_EVAL_ONCE( Args[ 1 ]);
			}
			SLANG_NODE_SET_ARGS( Node, Args, NumArgs )

			if( FuncInfo->ParseFlags & SLANG_PARSE_TIME )
			{
				if( !( Node->FuncPtr = SlangFunctionBind( FuncInfo ))
					|| SLANG_OK != (*Node->FuncPtr)( Node, SLANG_EVAL_PARSE_TIME, NULL, Scope ))
				{
					Ctx->token = ERROR_TOKEN;
					{
						ParseError( Ctx, NULL, ModuleName, ModuleType );
						goto AbortFreeNode;
					}
				}
			}
			if( FuncInfo->ParseFlags & SLANG_EVAL_ONCE && EvalOnce )
				Node = SlangNodeSetEvalOnce( Node );
		}
		else if( ( Constant = SlangConstantLookup( Node )))
		{
			DT_VALUE
					TmpValue;

			Node->Flags &= ~SLANG_NODE_STRING_EQ_VALUE;
			Node->Flags |= SLANG_NODE_FREE_STRING | SLANG_NODE_NO_BUCKET | SLANG_NODE_NAMED_CONST;
			DTM_ASSIGN( &TmpValue, Constant );
			SLANG_NODE_SET_VALUE( Node, &TmpValue );
			SLANG_NODE_TYPE_ASSIGN( Node, SLANG_TYPE_CONSTANT, FALSE, NULL );
		}
		else
		{
			SLANG_NODE_TYPE_ASSIGN( Node, SLANG_TYPE_VARIABLE, NULL, NULL );
		}
	}

	// Only thing allowed instead would be a prefix unary operator
	else if( MINUS != NextToken
			&& EXCLAMATION != NextToken
			&& AT_SIGN != NextToken
			&& DOUBLE_AT != NextToken
			&& DOLLAR_SIGN != NextToken
			&& AMPERSAND != NextToken
			&& PLUS_PLUS != NextToken
			&& MINUS_MINUS != NextToken
			&& POUND != NextToken
			&& ASTERISK != NextToken )
	{
		SLANG_NODE_TYPE_ASSIGN( Node, SLANG_TYPE_NULL, NULL, NULL );

		Node->ErrorInfo.EndingLine		= Ctx->end_line;
		Node->ErrorInfo.EndingColumn	= Ctx->end_char + 1;
		return( Node );
	}

	Node->ErrorInfo.EndingLine		= Ctx->end_line;
	Node->ErrorInfo.EndingColumn	= Ctx->end_char + 1;


/*
**	Look for binary operator
*/

	NextToken = clex_peektok( Ctx );

	for(;;)
	{
	    bool Unary = Node->Type == -1;
		bool LeftAssociative;
		int  NextPrecedence;

		NextType = TokenToOperator( Unary, Node->Type, NextToken, LeftAssociative, NextPrecedence );

		// Not an operator?
		if( -1 == NextType )
			break;

		// Lower precedence?
		if( NextPrecedence - LeftAssociative < Precedence )
			break;

		// Eat operator token
		clex_gettok( Ctx );

		// Create an operator node
		tmpNode = Node;
		if( !(Node = SLANG_NODE_CALLOC()))
		{
			ParseError( Ctx, "Memory allocation failed", ModuleName, ModuleType );
			goto AbortFreeBoth;
		}
		Node->ErrorInfo.ModuleName		= ModuleName;
		Node->ErrorInfo.ModuleType		= ModuleType;
		Node->ErrorInfo.BeginningLine	= tmpNode->ErrorInfo.BeginningLine;
		Node->ErrorInfo.BeginningColumn	= tmpNode->ErrorInfo.BeginningColumn;
		SLANG_NODE_TYPE_ASSIGN( Node, NextType, NULL, &NodeParseFlags );

		if( LPAREN == NextToken )
		{
			// Check for NO_PARENS funcs or "foo::bar()" type calls.
			if( tmpNode->Type == SLANG_RESERVED_FUNC || tmpNode->Type == SLANG_SCOPE_OPERATOR )
				return( ParseError( Ctx, "Unexpected argument list", ModuleName, ModuleType ));

			if( !EXPR_LIST_PARSE( Node, RPAREN, EvalOnce, PrivateScope ))
				goto AbortFreeBoth;

			FuncInfo = NULL;
			// Check for DataType(), ValueMethod(), or ReservedFunc()
			if( SLANG_TYPE_VARIABLE == tmpNode->Type || SLANG_NODE_IS_CONSTANT( tmpNode, DtString ))
			{
				SLANG_NODE_BUCKET_SET( Node, SLANG_NODE_BUCKET( tmpNode ));
				Node->StringValue = tmpNode->StringValue;

				FuncInfo = SLANG_FUNC_LOOKUP( Node );
				if( FuncInfo )
				{
					if( FuncInfo->Func ) {
						Node->FuncPtr = FuncInfo->Func;			
					} else {
						Node->FuncPtr = SlangXDelayLoad;
					}
					Node->Type = SLANG_RESERVED_FUNC;
					Node->Flags = SLANG_NODE_NO_VALUE;
					Node->StringValue = FuncInfo->Name;
					Node->ValueData.Pointer = FuncInfo;
					if( FuncInfo->ParseFlags )
					{
						if( FuncInfo->ParseFlags & SLANG_DATA_TYPE_CONSTRUCTOR )
						{
							Node->Type = SLANG_DATA_TYPE_CREATOR;
							if( !( Node->ValueDataType = DtFromName( SLANG_NODE_STRING_VALUE( tmpNode ))))
							{
								ErrMore( "Could not load datatype '%s'", SLANG_NODE_STRING_VALUE( tmpNode ));
								ParseError( Ctx, NULL, ModuleName, ModuleType );
								goto AbortFreeAll;
							}
						}
						if( FuncInfo->ParseFlags & SLANG_NO_PARENS )
						{
							ParseError( Ctx, "Unexpected argument list", ModuleName, ModuleType );
							goto AbortFreeAll;
						}
						if( FuncInfo->ParseFlags & SLANG_BIND_BLOCK )
						{
							if( !( TempExpr = ParseExpression( Ctx, 0, ModuleName, ModuleType, Scope, PrivateScope )))
								goto AbortFreeAll;
							ExprListArgs[ ExprList + Node->Argc++ ] = TempExpr;
							EvalOnce = EvalOnce && SLANG_NODE_IS_EVAL_ONCE( TempExpr );
						}
						if( FuncInfo->ParseFlags & ( SLANG_BIND_COLON | SLANG_BIND_COLON_ALWAYS ) && COLON == clex_peektok( Ctx ))
						{
	 						clex_gettok( Ctx );
							if( !( TempExpr = ParseExpression( Ctx, 0, ModuleName, ModuleType, Scope, PrivateScope )))
								goto AbortFreeAll;
							ExprListArgs[ ExprList + Node->Argc++ ] = TempExpr;
							EvalOnce = EvalOnce && SLANG_NODE_IS_EVAL_ONCE( TempExpr );
						}
						else if( FuncInfo->ParseFlags & SLANG_BIND_COLON_ALWAYS )
						{
							TempExpr = SLANG_NODE_CALLOC();
							TempExpr->ErrorInfo = Node->ErrorInfo;
							SLANG_NODE_TYPE_ASSIGN( TempExpr, SLANG_TYPE_NULL, NULL, NULL );
							ExprListArgs[ ExprList + Node->Argc++ ] = TempExpr;
						}
					}
				}
				else
				{
					Node->Flags |= SLANG_NODE_NO_VALUE;
					Node->ValueDataType = DtValueType;
					SLANG_NODE_VALUE_TYPE_SET( Node, SecDbValueTypeFromName( SLANG_NODE_STRING_VALUE( tmpNode ), NULL ));
					if( SLANG_NODE_VALUE_TYPE( Node ))
					{
						Node->StringValue = SLANG_NODE_VALUE_TYPE( Node )->Name;
						SLANG_NODE_TYPE_ASSIGN( Node, SLANG_VALUE_TYPE, NULL, NULL );
					}
					else
					{
						ErrMore( "Illegal value type" );
						ParseError( Ctx, NULL, ModuleName, ModuleType );
						goto AbortFreeAll;
					}
				}
				SlangFree( tmpNode );
			}
			else if( SLANG_AT_FUNC == tmpNode->Type || SLANG_DOUBLE_AT_FUNC == tmpNode->Type )
			{
				SLANG_NODE_TYPE_ASSIGN( Node, tmpNode->Type, NULL, NULL );
				Node->Argc++;
				ExprListArgs[ --ExprList ] = tmpNode->Argv[ 0 ];
				free( tmpNode->Argv );
				tmpNode->Argv = NULL;
				SlangFree( tmpNode );
			}
			else
			{
				SLANG_NODE_TYPE_ASSIGN( Node, SLANG_VALUE_TYPE_FUNC, NULL, NULL );
				Node->Argc++;
				ExprListArgs[ --ExprList ] = tmpNode;
			}
			tmpNode = NULL;
			SLANG_NODE_EXPR_LIST_COPY( Node, ExprList );

			// We have to wait till now because Argv wasn't set till now
			if( FuncInfo )
			{
				if( ( FuncInfo->ParseFlags & SLANG_PARSE_TIME )
					&& ( !( Node->FuncPtr = SlangFunctionBind( FuncInfo ))
						 || SLANG_OK != (*Node->FuncPtr)( Node, SLANG_EVAL_PARSE_TIME, NULL, Scope ))
				  )
				{
					Ctx->token = ERROR_TOKEN;
					ParseError( Ctx, NULL, ModuleName, ModuleType );
					goto AbortFreeNode;
				}
				if( FuncInfo->ParseFlags & SLANG_FUNC_PUSH_SCOPE )
					Node->Flags |= SLANG_NODE_PUSH_SCOPE;
				if( ( FuncInfo->ParseFlags & SLANG_EVAL_ONCE && EvalOnce ) || FuncInfo->ParseFlags & SLANG_EVAL_ONCE_ALWAYS )
					Node = SlangNodeSetEvalOnce( Node );
			}
		}
		else if( LBRACKET == NextToken )
		{
			// Handle '[' expr [, expr...] ']'
			if( !EXPR_LIST_PARSE( Node, RBRACKET, EvalOnce, PrivateScope ))
				goto AbortFreeBoth;

			Node->Argc++;
			ExprList--;
			ExprListArgs[ ExprList ] = tmpNode;
			SLANG_NODE_EXPR_LIST_COPY( Node, ExprList )

			if( Node->Argc >= SLANG_SUBSCRIPT_MAX_ARGS )
			{
				ParseError( Ctx, "Too many subscripts", ModuleName, ModuleType );
				goto AbortFreeNode;
			}
			if( EvalOnce && SLANG_NODE_IS_EVAL_ONCE( tmpNode ) )
				Node = SlangNodeSetEvalOnce( Node );
		}
		else if( SLANG_COMPONENT == Node->Type )
		{
			SLANG_NODE
					*Args[ 1 ];


			Args[ 0 ]	= tmpNode;

			switch( clex_gettok( Ctx ))
			{
				case SYMBOL:
					if( SymbolStyleTsdb && tmpNode->Type == SLANG_TYPE_VARIABLE )
					{
						char *Symbol = StrPaste( tmpNode->StringValue, ".", Ctx->symbol, NULL );
						free( tmpNode->StringValue );
						tmpNode->ValueData.Pointer = tmpNode->StringValue = Symbol;
						SLANG_NODE_BUCKET_SET( tmpNode, HashStrHash( Symbol ));
						SlangFree( Node );
						Node = tmpNode;
					}
					else
					{
						Node->Flags |= SLANG_NODE_FREE_STRING;
						SLANG_NODE_STRING_VALUE_SET( Node, strdup( Ctx->symbol ));
					}
					break;

				case QUOTED_STRING:
					Node->Flags |= SLANG_NODE_FREE_STRING;
					SLANG_NODE_STRING_VALUE_SET( Node, Ctx->quoted_string );
					break;

				default:
					ParseError( Ctx, "Right side of dot operator must be a component name", ModuleName, ModuleType );
					goto AbortFreeBoth;
			}
			if( Node->Type == SLANG_COMPONENT )
			{
				SLANG_NODE_BUCKET_SET( Node, HashStrHash( SLANG_NODE_STRING_VALUE( Node ) ));

				// Parse x.y(3,4)
				if( Precedence <= PRECEDENCE_MEMBER_CALL && clex_peektok( Ctx ) == LPAREN )
				{
					clex_gettok( Ctx );

					if( !EXPR_LIST_PARSE( Node, RPAREN, EvalOnce, PrivateScope ))
						goto AbortFreeBoth;

					SLANG_NODE_TYPE_ASSIGN( Node, SLANG_MEMBER_FUNC, NULL, NULL );
					Node->Argc++;
					ExprListArgs[ --ExprList ] = tmpNode;
					SLANG_NODE_EXPR_LIST_COPY( Node, ExprList );
				}
				else
					SLANG_NODE_SET_ARGS( Node, Args, 1 );
			}
			tmpNode = NULL;
		}
		else if( SLANG_UNARY_MINUS == Node->Type )
		{
			SLANG_NODE
					*Args[ 1 ];


			if( !(Args[ 0 ] = ParseExpression( Ctx, NextPrecedence, ModuleName, ModuleType, Scope, PrivateScope )))
				goto AbortFreeBoth;

			if( SLANG_TYPE_NULL == Args[ 0 ]->Type )
			{
				ParseError( Ctx, "Expected expression following operator", ModuleName, ModuleType );
				SlangFree( Args[ 0 ]);
				goto AbortFreeBoth;
			}

			if( tmpNode )
				SlangFree( tmpNode );
			tmpNode = NULL;

			// Turn -(3) into -3
			if( SLANG_NODE_IS_CONSTANT( Args[ 0 ], DtDouble ))
			{
				tmpNode 	   		= Node;
				Node 		   		= Args[ 0 ];
				Node->ValueData.Number *= -1.0;
				SLANG_NODE_FREE( 			tmpNode );
				tmpNode 			= NULL;
			}
			else
				SLANG_NODE_SET_ARGS( Node, Args, 1 );
		}
		else if( Unary )
		{
			SLANG_NODE
					*Args[ 1 ];


			if( !(Args[ 0 ] = ParseExpression( Ctx, NextPrecedence, ModuleName, ModuleType, Scope, PrivateScope )))
				goto AbortFreeBoth;

			SLANG_NODE_SET_ARGS( Node, Args, 1 );
			if( SLANG_TYPE_NULL == Node->Argv[ 0 ]->Type )
			{
				ParseError( Ctx, "Expected expression following operator", ModuleName, ModuleType );
				goto AbortFreeBoth;
			}

			if( tmpNode )
				SlangFree( tmpNode );
			tmpNode = NULL;
		}
		else if( SLANG_POST_DECREMENT == Node->Type
				|| SLANG_POST_INCREMENT == Node->Type )
		{
			SLANG_NODE
					*Args[ 1 ];


			Args[ 0 ]		= tmpNode;
			SLANG_NODE_SET_ARGS( Node, Args, 1 );
			tmpNode = NULL;
		}
		else	// all other operators
		{
			SLANG_NODE
					*Args[ 2 ];


			Args[ 0 ]		= tmpNode;

			if( !(Args[ 1 ] = ParseExpression( Ctx, NextPrecedence, ModuleName, ModuleType, Scope, PrivateScope )))
				goto AbortFreeBoth;

			if( SLANG_TYPE_NULL == Args[ 1 ]->Type )
			{
				ParseError( Ctx, "Expected expression following operator", ModuleName, ModuleType );
				SlangFree( Args[ 1 ]);
				goto AbortFreeBoth;
			}

			if( SLANG_SCOPE_OPERATOR == Node->Type )
			{
				// Check LHS and RHS
				if( SLANG_TYPE_VARIABLE != Args[ 0 ]->Type )
					return ParseError( Ctx, "Left hand side of :: must be a symbol", ModuleName, ModuleType );
				if( SLANG_TYPE_VARIABLE != Args[ 1 ]->Type )
					return ParseError( Ctx, "Right hand side of :: must be a symbol", ModuleName, ModuleType );

				// Assign scope pointer
				if( 0 == stricmp( FILE_STATIC_SCOPE_NAME, Args[ 0 ]->StringValue ))
					SlangNodeStringReassign( Args[ 0 ], StrPaste( "~", PrivateScope, NULL ));

				Node->StringValue = (char *) SlangVariableScopeGet( Args[ 0 ]->StringValue, TRUE );
			}
			if( Node->Type == SLANG_AT_FUNC )
			{
				if( SymbolStyleTsdb )
				{
					if( Args[ 1 ]->Type != SLANG_TYPE_VARIABLE )
					{
						ParseError( Ctx, "Expected variable following @ for Tsdb Style symbols", ModuleName, ModuleType );
						SlangFree( Args[ 1 ]);
						goto AbortFreeBoth;
					}
					else
					{
						char *Symbol = StrPaste( Args[ 0 ]->StringValue, "@", Args[ 1 ]->StringValue, NULL );
						free( Args[ 0 ]->StringValue );
						Args[ 0 ]->ValueData.Pointer = Args[ 0 ]->StringValue = Symbol;
						SLANG_NODE_BUCKET_SET( Args[ 0 ], HashStrHash( Symbol ));
						SlangFree( Args[ 1 ]);
						SlangFree( Node );
						Node = Args[ 0 ];
						Args[ 0 ] = NULL;
					}
				}
				else
				{
					SLANG_NODE_TYPE_ASSIGN( Node, SLANG_BINARY_AT_OP, NULL, &NodeParseFlags );
				}
			}
			if( Args[ 0 ])
			{
				SLANG_NODE_SET_ARGS( Node, Args, 2 );
				if( NodeParseFlags & SLANG_EVAL_ONCE && SLANG_NODE_IS_EVAL_ONCE( Args[ 0 ]) && SLANG_NODE_IS_EVAL_ONCE( Args[ 1 ]))
					Node = SlangNodeSetEvalOnce( Node );
				if( NodeParseFlags & SLANG_PARSE_TIME && SLANG_OK != (*Node->FuncPtr)( Node, SLANG_EVAL_PARSE_TIME, NULL, Scope ) )
				{
					Ctx->token = ERROR_TOKEN;
					ParseError( Ctx, NULL, ModuleName, ModuleType );
					goto AbortFreeNode;
				}
			}
		}

		Node->ErrorInfo.EndingLine		= Ctx->end_line;
		Node->ErrorInfo.EndingColumn	= Ctx->end_char + 1;

		NextToken = clex_peektok( Ctx );
	}


	if( -1 == Node->Type )
		SLANG_NODE_TYPE_ASSIGN( Node, SLANG_TYPE_NULL, NULL, NULL );

	Node->ErrorInfo.EndingLine		= Ctx->end_line;
	Node->ErrorInfo.EndingColumn	= Ctx->end_char + 1;
	return( Node );

AbortFreeAll:
	// need to free arguments allocated by ParseExprList
	for( ExprListCurr = ExprList + Node->Argc - 1; ExprListCurr >= ExprList; --ExprListCurr )
		SlangFree( ExprListArgs[ ExprListCurr ]);
	EXPR_LIST_RESTORE();

AbortFreeBoth:
	SlangFree( tmpNode );

AbortFreeNode:
	SlangFree( Node );
	return NULL;
}



/****************************************************************
**	Routine: ParseExprList
**	Returns: Index into statically allocated SLANG_NODE array / -1 if error
**	Action : Parses the interior of the arg list
**			 Stores Argc in Node->Argc if successful
**			 Makes sure that there is 1 space available before and
**			 at least two spaces available after the arg list returned
****************************************************************/

static int ParseExprList(
	CLEXCTX		*Ctx,
	SLANG_NODE	*Node,
	SLANG_SCOPE	*Scope,
	TOKEN		TerminalToken,
	int			*EvalOnce,
	const char	*PrivateScope	// the name of the private scope
)
{
	static int
			MaxArgs = 0;

	TOKEN	NextToken = NULL_TOKEN;

	SLANG_NODE
			*Expr;

	int 
		Argc;

	// Initialize argument space
	if( !ExprListArgs )
	{
		MaxArgs = INITIAL_ARG_ALLOC;
		ExprListArgs = (SLANG_NODE **) malloc( sizeof( SLANG_NODE * ) * MaxArgs );
	}
	*EvalOnce = TRUE;

	if( MaxArgs - ExprListCurr < 2 )
	{
		MaxArgs *= 2;
		ExprListArgs = (SLANG_NODE **) realloc( ExprListArgs, sizeof( SLANG_NODE * ) * MaxArgs );
	}

	// Initialize extra empty space before arguments which parser might want to use
	ExprListArgs[ ExprListCurr - 1 ] = NULL;

	if( clex_peektok( Ctx ) != TerminalToken )
	{
		while( TRUE )
		{
			// ExprListArgs might/will get realloc'd as a result of ParseExpression, therefore we must split the assignment
			// into two steps since we don't know the order of evaluation
			if( !(Expr = ParseExpression( Ctx, 0, Node->ErrorInfo.ModuleName, Node->ErrorInfo.ModuleType, Scope, PrivateScope )))
				goto Abort;
			ExprListArgs[ ExprListCurr++ ] = Expr;
			*EvalOnce = *EvalOnce && SLANG_NODE_IS_EVAL_ONCE( Expr );

			if( MaxArgs - ExprListCurr < 2 )
			{
				MaxArgs *= 2;
				ExprListArgs = (SLANG_NODE **) realloc( ExprListArgs, sizeof( SLANG_NODE * ) * MaxArgs );
			}

			if( (NextToken = clex_peektok( Ctx )) != COMMA && NextToken != SEMICOLON )
				break;
			clex_gettok( Ctx );
		}
	}

	if( TerminalToken != clex_gettok( Ctx ))
	{
		ParseError( Ctx, "Unexpected token", Node->ErrorInfo.ModuleName, Node->ErrorInfo.ModuleType );
		goto Abort;
	}

	while( ExprListCurr > ExprListFirst && ExprListArgs[ ExprListCurr - 1 ]->Type == SLANG_TYPE_NULL )
		SLANG_NODE_FREE( ExprListArgs[ --ExprListCurr ]);

	Argc = ExprListCurr - ExprListFirst;
	if( Argc > SHRT_MAX ) 
	{
		ParseError( Ctx, 
					"Too many children for this node. "
					"You may want to wrap them with a Slang block.", 
					 Node->ErrorInfo.ModuleName,
					 Node->ErrorInfo.ModuleType );
		goto Abort;
	}
	Node->Argc = Argc;

	// Make space for two extra things which the parser might want to use
	ExprListArgs[ ExprListCurr++ ] = NULL;
	ExprListArgs[ ExprListCurr++ ] = NULL;

	return ExprListFirst;

Abort:
	// Free the partial argument list which parsed correctly
	while( ExprListCurr > ExprListFirst )
		SlangFree( ExprListArgs[ --ExprListCurr ]);

	return -1;
}


/****************************************************************
**	Routine: ParseError
**	Returns: NULL
**	Action : Puts together an error message with char number, etc.
**		     and sends it to Err.
****************************************************************/

static SLANG_NODE *ParseError(
	CLEXCTX		*Ctx,
	const char	*ErrorText,
	const char	*ModuleName,
	int			ModuleType
)
{
	// FIX-Yuck.  Have to cast ModuleName because it is constant to
	// everyone else, but this function.
	if( SlangErrorInfo.ModuleName )
		free( (char *) SlangErrorInfo.ModuleName );
	if( ModuleName )
		SlangErrorInfo.ModuleName = strdup( ModuleName );
	else
		SlangErrorInfo.ModuleName = NULL;
	SlangErrorInfo.ModuleType		= ModuleType;
	SlangErrorInfo.EndingLine		= Ctx->end_line;
	SlangErrorInfo.EndingColumn  	= Ctx->end_char + 1;

	if( ERROR_TOKEN == Ctx->token )
	{
		SlangErrorInfo.BeginningLine   = Ctx->end_line;
		SlangErrorInfo.BeginningColumn = Ctx->end_char + 1;
		ErrMore( "Error in expression" );
	}
	else
	{
		if( ErrorText )
			Err( ERR_UNSUPPORTED_OPERATION, "%s", ErrorText );
		if( Ctx->token == SYMBOL )
			ErrMore( "Error in expression on token %s:%s", clex_tokenname( Ctx->token ), Ctx->symbol );
		else
			ErrMore( "Error in expression on token %s", clex_tokenname( Ctx->token ));
	}
	return NULL;
}



/****************************************************************
**	Routine: SlangNodeTypeRegister
**	Returns: TRUE or FALSE
**	Action : Binds the given node type to the given function
**			 address.
****************************************************************/

int SlangNodeTypeRegister(
	SLANG_NODE_TYPE		NodeType,	// Type of node
	const char			*DllPath,	// Path and filename of dll
	const char			*FuncName,	// C function name within dll
	SLANG_FUNC			Func,	 	// Pointer to function
	SLANG_PARSE_FLAGS	ParseFlags	// Parser control flags
)
{
	SLANG_FUNCTION_INFO
			*FunctionInfo;


	if( !SlangNodeTypeHash )
		SlangNodeTypeHash = HashCreate( "Slang NodeTypes", NULL, NULL, 0, NULL );

	/*
	 * Note: if a slang function is registered more than once, Purify
	 * will see a small memory leak.  WBR 941214
	 */
	FunctionInfo = (SLANG_FUNCTION_INFO *) calloc( 1, sizeof( SLANG_FUNCTION_INFO ));
	FunctionInfo->NodeType	= NodeType;
	FunctionInfo->DllPath	= (DllPath  ? strdup( DllPath ) : NULL);
	FunctionInfo->FuncName	= (FuncName ? strdup( FuncName ) : NULL);
	FunctionInfo->Func		= Func;
	FunctionInfo->ParseFlags= ParseFlags;

	return HashInsert( SlangNodeTypeHash, (HASH_KEY) NodeType, FunctionInfo );
}



/****************************************************************
**	Routine: SlangNodeTypeLookup
**	Returns: TRUE	- No error (Func still must be checked!)
**			 FALSE	- Error (Text in ErrBuf)
**	Action : Lookup (and possibly load) a slang node
****************************************************************/

SLANG_FUNC SlangNodeTypeLookup(
	SLANG_NODE_TYPE		NodeType,
	int					*pParseFlags
)
{
	SLANG_FUNCTION_INFO
			*FunctionInfo;


	if( !(FunctionInfo = (SLANG_FUNCTION_INFO *) HashLookup( SlangNodeTypeHash, (HASH_KEY) NodeType )))
	{
		Err( ERR_UNSUPPORTED_OPERATION, "NodeType '%s' is not supported", SlangNodeNameFromType( NodeType ));
		return NULL;
	}

	if( pParseFlags )
	{
		*pParseFlags = FunctionInfo->ParseFlags;
		if( SlangEvalOnceIgnore() )
			*pParseFlags &= ~SLANG_EVAL_ONCE;
	}

	if( FunctionInfo->Func )
		return( FunctionInfo->Func );

	if( !(FunctionInfo->Func = (SLANG_FUNC) DynaLink( FunctionInfo->DllPath, FunctionInfo->FuncName )))
		return NULL;

	return( FunctionInfo->Func );
}



/****************************************************************
**	Routine: SlangNodeNameFromType
**	Returns: Node name associated with type
**	Action : Lookup the ascii name of a node type
****************************************************************/

const char *SlangNodeNameFromType(
	SLANG_NODE_TYPE		NodeType
)
{
	char const* Name = (char const*) HashLookup( SlangNodesByType, (HASH_KEY) NodeType );

	return Name ? Name : "Unknown";
}



/****************************************************************
**	Routine: MyCallback
**	Returns:
**	Action :
****************************************************************/

static int MyCallback(
	YAP_PARSER	*, //Yap,
	YAP_MSG		Msg,
	DT_VALUE	*FieldValues,
	int			FieldCount,
	YAP_PARSER	* //Parent
)
{
	if( YAP_MSG_ITEM_PARSED == Msg )
	{
		if( FieldCount != 3 )
			return Err( ERR_INCOMPLETE, "@ - Missing arguments" );

		SlangNodeTypeRegister(
				(SLANG_NODE_TYPE) (long) FieldValues[ 0 ].Data.Pointer,
				(char *)		  FieldValues[ 1 ].Data.Pointer,
				(char *)		  FieldValues[ 2 ].Data.Pointer,
				NULL,
				0 );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SlangNodeFuncTableLoad
**	Returns: TRUE/FALSE
**	Action : Load in information from data table of node types
****************************************************************/

int SlangNodeFuncTableLoad(
	const char	*Path,
	const char	*FileName
)
{
	YAP_PARSER* Yap = YapCreate( FileName, MyCallback );

	YapAddField( Yap, "Node Type", 	YAP_FIELD_TYPE_STRHASH,		SlangNodesByName );
	YapAddField( Yap, "DLL", 		YAP_FIELD_TYPE_DT,			&DtString );
	YapAddField( Yap, "C Function",	YAP_FIELD_TYPE_DT, 			&DtString );

	int const Count = YapParse( Yap, FileName, NULL, Path );
	YapDestroy( Yap );

	if( Count == -1 )
		return ErrMore( "Loading node table from '%s'", FileName );

	return TRUE;
}



/****************************************************************
**	Routine: SlangFunctionTableLoad
**	Returns: TRUE	- Function table loaded without error
**			 FALSE	- Error loading functions from table
**	Summary: Load a table of slang functions
**	Action : Load a function table from an ASCII file into a hash
**			 table.  The format for the ASCII file is:
**
**				FunctionName	- Name of slang function
**				DllPath			- Path and filename for the dll
**				FunctionName	- Name of the function in the dll
**				ParseFlags		- Parser control flags
**
****************************************************************/

int SlangFunctionTableLoad(
	const char	*Path,			// Path down which to hunt for the file
	const char	*TableName,		// Name of the 'slang.dat' file
	bool		OkIfMissing		// Don't complain if the file can't be found
)
{
	char	Buffer[ 256 ],
			FileName[ FILENAME_MAX + 1 ],
			*Str;

	FILE	*fpTable;

	SLANG_FUNCTION_INFO
			FunctionInfo;

	int
			LineNum = 0;


/*
**	Open the ASCII file containing the table
*/

	if( !KASearchPath( Path, TableName, FileName, sizeof( FileName )))
		return OkIfMissing;

	if( !(fpTable = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, "LoadFunctionTable( %s ) %s", FileName, strerror( errno ));


/*
**	Read and parse the file
*/

	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		LineNum++;

		if( Buffer[ 0 ] == '/' && Buffer[ 1 ] == '/' )
			continue;

		// Name (quoted region)
		if( !(FunctionInfo.Name = strtok( Buffer, "\t\n\"" )))
			break;

		// Dll path
		if( !(FunctionInfo.DllPath = strtok( NULL, " \t\n" )))
			break;

		// Function name
		if( !(FunctionInfo.FuncName = strtok( NULL, " \t\n" )))
			break;

		// Parse flags
		FunctionInfo.ParseFlags = 0;
		while( ( Str = strtok( NULL, " \t\n" )))
		{
			if( !stricmp( Str, "BIND_BLOCK" ))
				FunctionInfo.ParseFlags	|= SLANG_BIND_BLOCK;
			else if( !stricmp( Str, "NO_PARENS" ))
				FunctionInfo.ParseFlags	|= SLANG_NO_PARENS;
			else if( !stricmp( Str, "BIND_COLON" ))
				FunctionInfo.ParseFlags	|= SLANG_BIND_COLON;
			else if( !stricmp( Str, "BIND_COLON_ALWAYS" ))
				FunctionInfo.ParseFlags |= SLANG_BIND_COLON_ALWAYS;
			else if( !stricmp( Str, "PARSE_TIME" ))
				FunctionInfo.ParseFlags	|= SLANG_PARSE_TIME;
			else if( !stricmp( Str, "DEPRECATED" ))
				FunctionInfo.ParseFlags	|= SLANG_DEPRECATED;
			else if( !strnicmp( Str, "EVAL_ONCE", 9 ))
				FunctionInfo.ParseFlags	|= !stricmp( Str + 9, "_ALWAYS" ) ? SLANG_EVAL_ONCE_ALWAYS : SLANG_EVAL_ONCE;
			else if( !stricmp( Str, "CHECK_RETURN" ))
				FunctionInfo.ParseFlags	|= SLANG_CHECK_RETURN;
			else if( !stricmp( Str, "DATA_TYPE_CONSTRUCTOR" ))
				FunctionInfo.ParseFlags |= SLANG_DATA_TYPE_CONSTRUCTOR;
			else if( !stricmp( Str, "PUSH_SCOPE" ))
				FunctionInfo.ParseFlags |= SLANG_FUNC_PUSH_SCOPE;
			else if( !stricmp( Str, "MEM_FUN" ))
				FunctionInfo.ParseFlags |= SLANG_MEM_FUN;

			else
				return Err( ERR_FILE_INVALID, "%s: Unexpected parse flag '%s'", FileName, Str );
		}

		if( FunctionInfo.ParseFlags & SLANG_DATA_TYPE_CONSTRUCTOR )
		{
			if( FunctionInfo.ParseFlags & SLANG_NO_PARENS )
				return Err( ERR_UNSUPPORTED_OPERATION, "%s: DataType Constructors cannot be No_Parens", FunctionInfo.Name );
		}

		// Register the function
 		if( !SlangFunctionRegister(
 				FunctionInfo.Name,
 				FunctionInfo.DllPath,
 				FunctionInfo.FuncName,
 				NULL,
 				FunctionInfo.ParseFlags ))
			ErrMsg( ERR_LEVEL_ERROR, "%s(%d): %s", FileName, LineNum, ErrGetString());
	}


/*
**	Close the file
*/

	fclose( fpTable );

	SLANG_FUNCTION_INFO
			*Info = SlangFunctionLookup( "SymbolStyleTsdb", HashStrHash( "SymbolStyleTsdb" ));
	s_SymbolStyleTsdbName = Info ? Info->Name : NULL;

	return TRUE;
}



/****************************************************************
**	Routine: SlangFunctionRegisterWithHandle
**	Returns: TRUE  - Function registered without error
**			 FALSE - Error registering the function
**	Summary: Add a function to Slang
****************************************************************/

int SlangFunctionRegisterWithHandle(
	const char 			*Name, 	 	// Slang function name
	const char			*DllPath,	// Path and filename of dll
	const char			*FuncName,	// C function name within dll
	SLANG_FUNC			Func,	 	// Pointer to function
	SLANG_PARSE_FLAGS 	ParseFlags,	// Flags- Block, Parens, etc.
	void				*Handle		// function specific handle
)
{
	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
	if( !SlangFunctionHash )
		SlangFunctionHash 	= HashCreate( "Slang Functions", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );

	if( HashLookup( SlangFunctionHash, Name ))
		return Err( ERR_UNSUPPORTED_OPERATION, "duplicate entry: %s", Name );

	SLANG_FUNCTION_INFO* FunctionInfo = (SLANG_FUNCTION_INFO *) calloc( 1, sizeof( SLANG_FUNCTION_INFO ));
	FunctionInfo->Name 		= strdup( Name );
	FunctionInfo->DllPath	= (DllPath  ? strdup( DllPath ) : NULL);
	FunctionInfo->FuncName	= (FuncName ? strdup( FuncName ) : NULL);
	FunctionInfo->Func		= Func;
	FunctionInfo->ParseFlags= ParseFlags;
	FunctionInfo->Handle	= Handle;

	if( ParseFlags & SLANG_MEM_FUN )
	{
		string name = Name;
		size_t pos = name.find( "::" );
		if( pos == string::npos || pos == 0 )
			return Err( ERR_INVALID_ARGUMENTS, "@ - SlangFunctionRegister %s says it is a mem fun but has no ::", Name );

		free( FunctionInfo->Name );
		string type_name = name.substr( 0, pos );

		string cons_name = type_name;
		cons_name += "::";
		cons_name += type_name;

		if( name == cons_name )
		{
			// Is a Constructor, register it under "new TypeName"
			name = "new ";
			name += type_name;
			FunctionInfo->Name = strdup( name.c_str() );

			FunctionInfo->ParseFlags = ParseFlags & ~SLANG_MEM_FUN;
		}
		else
		{
			name.replace( pos, 2, "__" );

			FunctionInfo->Name = strdup( name.c_str() );

			if( !type_name_to_mem_funs )
				type_name_to_mem_funs = new String_To_Slang_Mem_Funs_Map;

			SLANG_MEM_FUN_MAP& mem_funs = (*type_name_to_mem_funs)[ type_name ];
			mem_funs[ FunctionInfo->Name + type_name.size() + 2 ] = FunctionInfo;
		}
	}

	if( ParseFlags & SLANG_CHECK_RETURN )
		SlangCheckReturnRegister( FunctionInfo->Name );

	return HashInsert( SlangFunctionHash, FunctionInfo->Name, FunctionInfo );
}



/****************************************************************
**	Routine: SlangFunctionRegister
**	Returns: TRUE  - Function registered without error
**			 FALSE - Error registering the function
**	Summary: Add a function to Slang
****************************************************************/

int SlangFunctionRegister(
	const char 			*Name, 	 	// Slang function name
	const char			*DllPath,	// Path and filename of dll
	const char			*FuncName,	// C function name within dll
	SLANG_FUNC			Func,	 	// Pointer to function
	SLANG_PARSE_FLAGS 	ParseFlags 	// Flags- Block, Parens, etc.
)
{
	return SlangFunctionRegisterWithHandle( Name, DllPath, FuncName, Func, ParseFlags, NULL );
}



/****************************************************************
**	Routine: SlangFunctionUsage
**	Returns: TRUE/FALSE
**	Action : Get the usage of a slang function and format the
**			 result into the error buffer (this will happen
**			 automatically through the function's own macros)
****************************************************************/

int SlangFunctionUsage(
	const char	*FunctionName		// Name of function to get usage for
)
{
	SLANG_SCOPE
			*Scope;

	void	*DebugFunction,
			*ErrorFunction,
			*AbortFunction;

	int		RetVal;


	SLANG_NODE
	        Node;

	memset( &Node, 0, sizeof( Node ));
	SLANG_NODE_STRING_VALUE_SET( &Node, (char *) FunctionName ); // FIX- shouldn't cast away const
	SLANG_NODE_BUCKET_SET( &Node, HashStrHash( FunctionName ));
	Node.Type			= SLANG_RESERVED_FUNC;
	Node.Argc			= SLANG_ARGC_USAGE;

	SLANG_FUNCTION_INFO* const FuncInfo = SLANG_FUNC_LOOKUP( &Node );

	if( !FuncInfo )
	{
		Err( ERR_NOT_FOUND, "FunctionUsage: %s - Not Found", FunctionName );
		return FALSE;
	}

	if( !( Node.FuncPtr = SlangFunctionBind( FuncInfo )))
	{
		ErrMore( "FunctionUsage: %s - @", FunctionName );
		return FALSE;
	}

	SLANG_RET
	        Ret;

	SLANG_NULL_RET( &Ret );

	ErrClear();

	DebugFunction = SlangContextGet( "Debug Function" );
	ErrorFunction = SlangContextGet( "Error Function" );
	AbortFunction = SlangContextGet( "Abort Function" );

	SlangContextSet( "Debug Function", NULL );
	SlangContextSet( "Error Function", NULL );
	SlangContextSet( "Abort Function", NULL );

	Scope = SlangScopeCreate( );
	RetVal = SlangEvaluate( &Node, (FuncInfo->ParseFlags & SLANG_PARSE_TIME) ? SLANG_EVAL_PARSE_TIME : SLANG_EVAL_RVALUE, &Ret, Scope );
	SlangScopeDestroy( Scope );

	SlangContextSet( "Debug Function",	DebugFunction );
	SlangContextSet( "Error Function", 	ErrorFunction );
	SlangContextSet( "Abort Function", 	AbortFunction );

	if( RetVal != SLANG_ERROR )
	{
		SLANG_RET_FREE( &Ret );
		return Err( ERR_UNSUPPORTED_OPERATION, "%s doesn't support usage description", FunctionName );
	}

	if( FuncInfo->ParseFlags & SLANG_PARSE_TIME )
		ErrMore( "Function evaluates at PARSE TIME\n" );

	return TRUE;
}



/****************************************************************
**	Routine: SlangFunctionInfo
**	Returns: TRUE/FALSE
**	Action : Get the structured usage of a slang function and
**			 fills that into a structure along with other
**			 function info from slang.dat
****************************************************************/

int SlangFunctionInfo(
	const char	*FunctionName,		// Name of function to get usage for
	DT_VALUE	*RetStruct			// Structure created with info
)
{
	SLANG_FUNCTION_INFO
			*FunctionInfo;

	SLANG_NODE
			Node;

	SLANG_SCOPE
			*Scope;

	SLANG_RET
			Ret;

	void	*DebugFunction,
			*ErrorFunction,
			*AbortFunction;

	int		Status,
			RetVal;


	memset( &Node, 0, sizeof( Node ));
	SLANG_NODE_STRING_VALUE_SET( &Node, (char *) FunctionName ); // FIX- shouldn't cast away const
	SLANG_NODE_BUCKET_SET( &Node, HashStrHash( FunctionName ));
	Node.Type			= SLANG_RESERVED_FUNC;
	Node.Argc			= SLANG_ARGC_INFO;

	// Force a DynaLink if needed
	if(	!( FunctionInfo = SLANG_FUNC_LOOKUP( &Node )))
	{
		Err( ERR_NOT_FOUND, "FunctionInfo: %s - Not Found", FunctionName );
		goto Abort;
	}
	else if( !( Node.FuncPtr = SlangFunctionBind( FunctionInfo )))
	{
		ErrMore( "FunctionInfo: %s - @", FunctionName );
		goto Abort;
	}

	SLANG_NULL_RET( &Ret );

	DebugFunction = SlangContextGet( "Debug Function" );
	ErrorFunction = SlangContextGet( "Error Function" );
	AbortFunction = SlangContextGet( "Abort Function" );

	SlangContextSet( "Debug Function", NULL );
	SlangContextSet( "Error Function", NULL );
	SlangContextSet( "Abort Function", NULL );

	Scope = SlangScopeCreate( );
	RetVal = SlangEvaluate( &Node, (FunctionInfo->ParseFlags & SLANG_PARSE_TIME) ? SLANG_EVAL_PARSE_TIME : SLANG_EVAL_RVALUE, &Ret, Scope );
	SlangScopeDestroy( Scope );

	SlangContextSet( "Debug Function",	DebugFunction );
	SlangContextSet( "Error Function", 	ErrorFunction );
	SlangContextSet( "Abort Function", 	AbortFunction );

	if( !DTM_ALLOC( RetStruct, DtStructure ))
		return( FALSE );

	if( RetVal != SLANG_ARG_INFO )
	{
		SLANG_RET_FREE( &Ret );
		Ret.Type = SLANG_TYPE_VALUE;
		Status = DTM_ALLOC( &Ret.Data.Value, DtStructure );
	}
	else
		Status = TRUE;
	if( Status && SlangFunctionUsage( FunctionName ))
		Status = DtComponentSetPointer( &Ret.Data.Value, "Text", DtString, ErrGetString());
	Status = Status && DtComponentSet( RetStruct, "Usage", &Ret.Data.Value );
	SLANG_RET_FREE( &Ret );

	Status = Status && DtComponentSetPointer( RetStruct, "Name", 		DtString, NULLSTR( FunctionInfo->Name ));
	Status = Status && DtComponentSetPointer( RetStruct, "DllPath",	DtString, NULLSTR( FunctionInfo->DllPath ));
	Status = Status && DtComponentSetPointer( RetStruct, "FuncName",	DtString, NULLSTR( FunctionInfo->FuncName ));
	Status = Status && DtComponentSetPointer( RetStruct, "NodeType",	DtString, NULLSTR( SlangNodeNameFromType( FunctionInfo->NodeType )));
	Status = Status && DtComponentSetNumber(  RetStruct, "ParseFlags",	DtDouble, (double) FunctionInfo->ParseFlags );

	if( !Status )
	{
		DTM_FREE( RetStruct );
		DTM_INIT( RetStruct );
		return( FALSE );
	}
	return( TRUE );

Abort:
	return FALSE;
}



/****************************************************************
**	Routine: SlangFunctionLookup
**	Returns: SLANG_FUNCTION_INFO/NULL if not found
**	Action : Lookup a slang function
****************************************************************/

SLANG_FUNCTION_INFO *SlangFunctionLookup(
	const char		*FunctionName,		// function name
	unsigned long	HashValue			// hash value of function
)
{
	return (SLANG_FUNCTION_INFO *)
			HashLookupByBucket( SlangFunctionHash, FunctionName, HashValue );
}



/****************************************************************
**	Routine: SlangFunctionBind
**	Returns: Pointer to SLANG_FUNC/Err
**	Action : Fill in FuncPtr of Node ( dynalink if necessary )
****************************************************************/

SLANG_FUNC SlangFunctionBind(
	SLANG_FUNCTION_INFO	*FunctionInfo	// FunctionInfo
)
{
	if( !FunctionInfo->Func )
	{
		if( !(FunctionInfo->Func = (SLANG_FUNC) DynaLink( FunctionInfo->DllPath, FunctionInfo->FuncName )))
			return NULL;			// Dynalink failed

		if( SlangEvalOnceIgnore() )
			FunctionInfo->ParseFlags &= ~SLANG_EVAL_ONCE;
	}

	return FunctionInfo->Func;
}



/****************************************************************
**	Routine: SlangEnumFirstFunction
**	Returns: Pointer to first function name
**			 NULL if no functions
**	Summary: Start enumeration of function names
****************************************************************/

char *SlangEnumFirstFunction(
	SDB_ENUM_PTR *EnumPtr		// Enumeration pointer
)
{
	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));

	(*EnumPtr)->Count = SlangFunctionHash->KeyCount;
	(*EnumPtr)->Ptr = HashFirst( SlangFunctionHash );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;
	return( (char *) HashKey( (*EnumPtr)->Ptr ));
}



/****************************************************************
**	Routine: SlangEnumNextFunction
**	Returns: Pointer to next function name
**			 NULL if no more function names
**	Summary: Get the next function name
****************************************************************/

char *SlangEnumNextFunction(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( SlangFunctionHash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;
	return( (char *) HashKey( EnumPtr->Ptr ));
}



/****************************************************************
**	Routine: SlangEnumFunctionClose
**	Returns: Nothing
**	Summary: End enumeration of function names
****************************************************************/

void SlangEnumFunctionClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}






/****************************************************************
**	Routine: SlangRegisterConstantValue
**	Returns: TRUE	- Constant registered without error
**			 FALSE	- Error registering the constant
**	Action : Register a constant
****************************************************************/

int SlangRegisterConstantValue(
	const char	*Name,			// Name of constant
	DT_VALUE	*Value			// Value of constant, you give ownership
)
{
	SLANG_CONSTANT
			*Constant;


	Constant = (SLANG_CONSTANT *) HashLookup( SlangConstantsHash, Name );
	if( Constant )
	{
		DTM_FREE( &Constant->ConstValue );
		Constant->ConstValue = *Value;
		return TRUE;
	}

	Constant = (SLANG_CONSTANT *) malloc( sizeof( *Constant ));
	Constant->Name		= strdup( Name );
	Constant->EnumName	= NULL;
	Constant->ConstValue= *Value;

	return( HashInsert( SlangConstantsHash, Constant->Name, Constant ));
}



/****************************************************************
**	Routine: SlangRegisterConstant
**	Returns: TRUE	- Constant registered without error
**			 FALSE	- Error registering the constant
**	Action : Register a constant
****************************************************************/

int SlangRegisterConstant(
	const char	*Name,			// Name of constant
	double		Value			// Value of constant
)
{
	SLANG_CONSTANT
			*Constant;


	Constant = (SLANG_CONSTANT *) HashLookup( SlangConstantsHash, Name );
	if( Constant )
	{
		DTM_FREE( &Constant->ConstValue );
		Constant->ConstValue.DataType 		= DtDouble;
		Constant->ConstValue.Data.Number	= Value;
		return TRUE;
	}

	Constant 						= (SLANG_CONSTANT *) malloc( sizeof( *Constant ));
	Constant->Name					= strdup( Name );
	Constant->EnumName				= NULL;
	Constant->ConstValue.DataType 	= DtDouble;
	Constant->ConstValue.Data.Number= Value;

	return( HashInsert( SlangConstantsHash, Constant->Name, Constant ));
}



/****************************************************************
**	Routine: SlangConstantLookup
**	Returns: DT_VALUE * / NULL if not found
**	Action : Lookup a constant by name. The DT_VALUE returned
**			 is owned by the constan hash. No guarentees are made
**			 on it's lifetime.
****************************************************************/

static DT_VALUE *SlangConstantLookup(
	SLANG_NODE	*Node
)
{
	SLANG_CONSTANT
			*Constant;

	char	*Name	= SLANG_NODE_STRING_VALUE( Node );

	HASH_BUCKET
			Bucket	= SLANG_NODE_BUCKET( Node );


	Constant = (SLANG_CONSTANT *) HashLookupByBucket( SlangConstantsHash, Name, Bucket );
	if( Constant )
		return &Constant->ConstValue;

	return NULL;
}



/****************************************************************
**	Routine: SlangNumConstants
**	Returns: Number of constants defined
**	Action : Return size of SlangConstantsHash
****************************************************************/

int SlangNumConstants(
	void
)
{
	return (int) SlangConstantsHash->KeyCount;
}



/****************************************************************
**	Routine: SlangEnumFirstConstant
**	Returns: Pointer to first constant name
**	Action : Enumerate the first constant name
****************************************************************/

SLANG_CONSTANT *SlangEnumFirstConstant(
	SDB_ENUM_PTR	*EnumPtr	// Enumeration pointer
)
{
	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));

	(*EnumPtr)->Count = SlangConstantsHash->KeyCount;
	(*EnumPtr)->Ptr = HashFirst( SlangConstantsHash );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;
	return( (SLANG_CONSTANT *) HashValue( (*EnumPtr)->Ptr ));
}


/****************************************************************
**	Routine: SlangEnumNextConstant
**	Returns: Next constant name
**			 NULL	- No more constants
**	Action : Return the next constant's name
****************************************************************/

SLANG_CONSTANT *SlangEnumNextConstant(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( SlangConstantsHash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;
	return( (SLANG_CONSTANT *) HashValue( EnumPtr->Ptr ));
}



/****************************************************************
**	Routine: SlangEnumConstantClose
**	Returns: None
**	Action : End enumeration of constants
****************************************************************/

void SlangEnumConstantClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}



/****************************************************************
**	Routine: SlangRegisterEnum
**	Returns: TRUE	- Enum constant registered without error
**			 FALSE	- Error registering the enum constant
**	Action : Register an enum constant
****************************************************************/

int SlangRegisterEnum(
	const char	*EnumName,		// Name of enumeration
	const char	*Name,			// Name of constant within enumeration
	double		Value			// Value of constant
)
{
	SLANG_ENUM
			*Enum;

	SLANG_CONSTANT
			*Constant;


	Constant = (SLANG_CONSTANT *) HashLookup( SlangConstantsHash, Name );
	if( Constant )
	{
		if( stricmp( EnumName, Constant->EnumName ) )
			return Err( ERR_INVALID_ARGUMENTS, "RegisterEnum, @: Can't change %s from %s to %s", Name, Constant->EnumName, EnumName );

		DTM_FREE( &Constant->ConstValue );
		Constant->ConstValue.DataType		= DtDouble;
		Constant->ConstValue.Data.Number	= Value;
		return TRUE;
	}

	Constant = (SLANG_CONSTANT *) malloc( sizeof( *Constant ));
	Constant->Name					= strdup( Name );
	Constant->EnumName				= strdup( EnumName );
	Constant->ConstValue.DataType	= DtDouble;
	Constant->ConstValue.Data.Number= Value;

	if( !HashInsert( SlangConstantsHash, Constant->Name, Constant ))
		return FALSE;

	if( !(Enum = (SLANG_ENUM *)HashLookup( SlangEnumsHash, EnumName )))
	{
		Enum = (SLANG_ENUM *) malloc( sizeof( *Enum ) );
		Enum->EnumName = strdup( EnumName );
		// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
		Enum->Members  = HashCreate( Enum->EnumName, (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 10, NULL );
		if( !HashInsert( SlangEnumsHash, Enum->EnumName, Enum ))
			return FALSE;
	}
	return HashInsert( Enum->Members, Constant->Name, Constant );
}



/****************************************************************
**	Routine: SlangEnumFirstEnum
**	Returns: Pointer to first enum name
**	Action : Enumerate the first enum name
****************************************************************/

SLANG_ENUM *SlangEnumFirstEnum(
	SDB_ENUM_PTR	*EnumPtr	// Enumeration pointer
)
{
	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));

	(*EnumPtr)->Count = SlangEnumsHash->KeyCount;
	(*EnumPtr)->Ptr = HashFirst( SlangEnumsHash );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;
	return( (SLANG_ENUM *) HashValue( (*EnumPtr)->Ptr ));
}



/****************************************************************
**	Routine: SlangEnumNextEnum
**	Returns: Next enum name
**			 NULL	- No more enums
**	Action : Return the next enum's name
****************************************************************/

SLANG_ENUM *SlangEnumNextEnum(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( SlangEnumsHash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;
	return( (SLANG_ENUM *) HashValue( EnumPtr->Ptr ));
}



/****************************************************************
**	Routine: SlangEnumEnumClose
**	Returns: None
**	Action : End enumeration of enums
****************************************************************/

void SlangEnumEnumClose(
	SDB_ENUM_PTR EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
		free( EnumPtr );
}

/****************************************************************
**	Routine: SlangCheckReturnRegister
**	Returns: Nothing
**	Action : Register a function pointer as CHECK_RETURN ready
****************************************************************/

int SlangCheckReturnRegister(
	const char	*Name
)
{
	if( !SlangCheckReturnHash )
	{
		SlangCheckReturnHash = HashCreate( "Slang Check Return", NULL, NULL, 17, NULL );
		if( getenv( "SLANG_CHECK_RETURN_DISABLED" ))
		{
			ErrMsg( ERR_LEVEL_INFO, "Warning: Slang Check Return optimization disabled" );
			SlangCheckReturnEnabled = FALSE;
		}
	}
	return HashInsert( SlangCheckReturnHash, (HASH_KEY) Name, (HASH_VALUE) "Yes" );
}



/****************************************************************
**	Routine: SlangHasReturn
**	Returns: TRUE/FALSE
**	Action : Does this node end with a Return()?
****************************************************************/

int SlangHasReturn(
	SLANG_NODE	*Root
)
{
	static SLANG_SCOPE
			*HasReturnScope = SlangScopeCreate();

	if( !SlangCheckReturnEnabled )
		return FALSE;

	if( !HashLookup( SlangCheckReturnHash, (HASH_KEY) SLANG_NODE_STRING_VALUE( Root )))
		return FALSE;
	return (*Root->FuncPtr)( Root, SLANG_EVAL_HAS_RETURN, NULL, HasReturnScope );
}



/****************************************************************
**	Routine: SlangExpectNoValue
**	Returns: SLANG_RET_CODE
**	Action : Check that EvalFlag is SLANG_EVAL_NO_VALUE and report
**			 error depending on error level check
****************************************************************/

SLANG_RET_CODE SlangExpectNoValue(
	SLANG_NODE		*Root,
	SLANG_EVAL_FLAG	EvalFlag,
	SLANG_RET		*Ret,
	SLANG_SCOPE		*Scope
)
{
	if( SlangExpectNoValueState == SLANG_EXPECT_OFF )
		return SLANG_OK;

	if( EvalFlag != SLANG_EVAL_NO_VALUE )
	{
		if( SlangExpectNoValueState == SLANG_EXPECT_ERROR )
			return SlangEvalError( SLANG_ERROR_PARMS,
					"%s: Expected to be evaluated in NO_VALUE mode", SLANG_NODE_STRING_VALUE( Root ) );
		else
		{
			static SLANG_PRINT_FUNCTION
					PrintFunction;

			static void
					*PrintHandle;

			static long
					UpdateCount;

			char	Buf[ 512 ];


			if( UpdateCount != SlangContext->UpdateCount )
			{
				UpdateCount		= SlangContext->UpdateCount;
				PrintFunction	= (SLANG_PRINT_FUNCTION) SlangContextGet( "Print Function" );
				PrintHandle		= SlangContextGet( "Print Handle" );
			}

			sprintf( Buf, "%s: Expected to be evaluated in NO_VALUE mode\n   @ %d,%d in %s\n",
					 SLANG_NODE_STRING_VALUE( Root ),
					 Root->ErrorInfo.BeginningLine, Root->ErrorInfo.BeginningColumn,
					 NULLSTR( Root->ErrorInfo.ModuleName ));
			if( PrintFunction )
				(*PrintFunction)( PrintHandle, Buf );
			else
				ErrMsg( ERR_LEVEL_INFO, Buf );
		}
	}
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangNodeReassignType
**	Returns: void
**	Action : Reassign node type
****************************************************************/

void SlangNodeReassignType(
	SLANG_NODE		*Root,		// Root of tree whose type to reassign
	SLANG_NODE_TYPE	NewType		// New Type to set node to
)
{
	void	*Extra;

	SLANG_ERROR_INFO
			ErrorInfo;

	int		FreeModule;


	ErrorInfo = Root->ErrorInfo;
	Extra = Root->Extra;
	FreeModule = Root->Flags & SLANG_NODE_FREE_MODULE;

	Root->Flags &= ~SLANG_NODE_FREE_MODULE;	// we restore ErrorInfo and FREE_MODULE, so make sure it isn't freed
	SlangNodeFreeContents( Root );

	memset( Root, 0, sizeof( SLANG_NODE ));

	Root->Type = NewType;
	Root->FuncPtr = SlangNodeTypeLookup( NewType, NULL );
	Root->Extra = Extra;	// restore extra data
	Root->ErrorInfo = ErrorInfo;
	if( FreeModule )
		Root->Flags |= SLANG_NODE_FREE_MODULE;
}



/****************************************************************
**	Routine: SlangNodeFreeContents
**	Returns: void
**	Action : Frees contents of the subtree represented by a node
**			 will not free the node itself
****************************************************************/

static void SlangNodeFreeContents(
	SLANG_NODE	*Root
)
{
	int		ArgNum;

	DT_VALUE
			Tmp;


/*
**	Argv is a valid pointer iff Argv is non null and Argc is non zero
**
**	Otherwise we must not free Argv as it may not be a pointer
*/

	if( Root->Argv )
	{
		if( Root->Type == SLANG_TYPE_SCOPED_VARIABLE )
			SlangVariableFree( (SLANG_VARIABLE *) SLANG_NODE_BUCKET( Root->Argv[ 1 ]));

		if( Root->Argc )
		{
			for( ArgNum = 0; ArgNum < Root->Argc; ++ArgNum )
				SlangFree( Root->Argv[ ArgNum ]);
			free( Root->Argv );
		}
		Root->Argv = NULL;
	}

	if( Root->Flags & SLANG_NODE_FREE_STRING)
		free( Root->StringValue );
	if( Root->Flags & SLANG_NODE_FREE_MODULE)
		free( (void *) Root->ErrorInfo.ModuleName );

	if( Root->ValueDataType && !( Root->Flags & SLANG_NODE_NO_VALUE ))
	{
		SLANG_NODE_GET_VALUE( Root, &Tmp );
		DTM_FREE( &Tmp );
	}
}



/****************************************************************
**	Routine: SlangNodeSetEvalOnce
**	Returns: SLANG_NODE*
**	Action : Marks a node as eval once, the node may change
**			 so it returns a pointer to the final node
**			 In the process, it will also remove any first level
**			 eval once from its children
****************************************************************/

static SLANG_NODE *SlangNodeSetEvalOnce(
	SLANG_NODE	*Node
)
{
	SLANG_NODE
			*NodePtr,
			*EvalOnceNode = SLANG_NODE_IS_EVAL_ONCE( Node ) ? Node : NULL;

	int		ArgNum;


	// walk through the arguments and remove EVAL_ONCE nodes
	if( Node->Argc )
	{
		for( ArgNum = 0; ArgNum < Node->Argc; ++ArgNum )
		{
			NodePtr = Node->Argv[ ArgNum ];
			if( NodePtr->Type & SLANG_TYPE_EVAL_ONCE )
			{
				Node->Argv[ ArgNum ] = NodePtr->Argv[ 0 ];
				if( !EvalOnceNode )
					EvalOnceNode = NodePtr;	// cache this node so we don't need to allocate one at the top
				else
				{
					free( NodePtr->Argv );
					SLANG_NODE_FREE( NodePtr );
				}
			}
		}
	}
	if( EvalOnceNode != Node )	// if node isn't already eval once
	{
		if( !EvalOnceNode )		// if we don't have a cache node, allocate one
		{
			EvalOnceNode = SLANG_NODE_CALLOC();
			EvalOnceNode->Type = SLANG_TYPE_EVAL_ONCE;
			EvalOnceNode->FuncPtr = (SLANG_FUNC) SlangXEvalOnce;
			EvalOnceNode->Argv = (SLANG_NODE **) malloc( sizeof( SLANG_NODE * ) );
			EvalOnceNode->Argc = 1;
		}
		EvalOnceNode->Argv[ 0 ] = Node;
		EvalOnceNode->ErrorInfo = Node->ErrorInfo;
	}
	return EvalOnceNode;
}



/****************************************************************
**	Routine: SlangNodeStringReassign
**	Returns: void
**	Action : Frees the node's string and sets it to the one passed
**			 in. The node ends up owning String
****************************************************************/

void SlangNodeStringReassign(
	SLANG_NODE	*Node,
	char		*String
)
{
	if( String == NULL )
	{
		if( Node->Flags & SLANG_NODE_STRING_EQ_VALUE )
			free( Node->ValueData.Pointer );
		if( Node->Flags & SLANG_NODE_FREE_STRING )
			free( Node->StringValue );
		Node->ValueData.Pointer = Node->StringValue = NULL;
		Node->Flags &= ~( SLANG_NODE_FREE_STRING | SLANG_NODE_STRING_EQ_VALUE );
	}
	else
	{
		if( Node->Flags & SLANG_NODE_FREE_STRING )
		{
			free( Node->StringValue );
			Node->StringValue = String;
		}
		else if( Node->Type == SLANG_DATA_TYPE_CREATOR )
		{
			Node->StringValue = String;
			Node->Flags |= SLANG_NODE_FREE_STRING;
		}
		else
		{
			if( Node->Flags & SLANG_NODE_STRING_EQ_VALUE )
				free( Node->ValueData.Pointer );
			SLANG_NODE_SET_POINTER( Node, DtString, String );
			Node->StringValue = (char *) Node->ValueData.Pointer;
			Node->Flags |= SLANG_NODE_STRING_EQ_VALUE;
		}
	}
}



/****************************************************************
**	Routine: SlangNodeApply
**	Returns: Root of parse tree representing (apply func args)
**	Action : generates a parse tree which Applies func to args
**			 ErrorInfo must not go away while evaluating the tree
****************************************************************/

SLANG_NODE *SlangNodeApply(
	DT_VALUE			*Func,
	DT_VALUE			*Args,
	SLANG_ERROR_INFO	*ErrorInfo
)
{
	SLANG_NODE
			*Root;

	SLANG_FUNCTION_INFO
			*FuncInfo;

	int		Arg,
			FirstArg = 0;

	SLANG_NODE
			**Argv;

	DT_VALUE
			TmpValue;


	Root = SLANG_NODE_CALLOC();
	Root->ErrorInfo = *ErrorInfo;
	if( Func->DataType == DtSlang )		// slang function call
	{
		Root->FuncPtr = SlangNodeTypeLookup( Root->Type = SLANG_AT_FUNC, NULL );
		Root->Argc = DtSize( Args ) + 1;
		Argv = Root->Argv = (SLANG_NODE **) malloc( sizeof( SLANG_NODE *) * Root->Argc );

		*Argv = SLANG_NODE_CALLOC();
		(*Argv)->FuncPtr = SlangNodeTypeLookup( (*Argv)->Type = SLANG_TYPE_CONSTANT, NULL );
		(*Argv)->ErrorInfo = *ErrorInfo;

		DTM_ASSIGN( &TmpValue, Func );
		SLANG_NODE_SET_VALUE( (*Argv), &TmpValue );
		++Argv;
	}
	else if( Func->DataType == DtString )
	{
		Root->Type = SLANG_RESERVED_FUNC;
		Root->StringValue = (char *) Func->Data.Pointer;
		SLANG_NODE_BUCKET_SET( Root, HashStrHash( (char *) Func->Data.Pointer ));
		Root->Flags = SLANG_NODE_NO_VALUE;

		if( !( FuncInfo = SLANG_FUNC_LOOKUP( Root )))	// value type reference
		{
			Root->ValueDataType = DtValueType;
			SLANG_NODE_VALUE_TYPE_SET( Root, SecDbValueTypeFromName( Root->StringValue, NULL ));
			if( !SLANG_NODE_VALUE_TYPE( Root ))
			{
				ErrMore( "Illegal value type: %s", Root->StringValue );
				goto Abort;
			}
			Root->StringValue = SLANG_NODE_VALUE_TYPE( Root )->Name;
			Root->FuncPtr = SlangNodeTypeLookup( SLANG_VALUE_TYPE, NULL );
			Root->Type = SLANG_VALUE_TYPE;
		}
		else if( !( Root->FuncPtr = SlangFunctionBind( FuncInfo ))) // SlangX function
		{
			ErrMore( "Couldn't bind %s to a node", Root->StringValue );
			goto Abort;
		}
		else							// is a SlangX func, make StringValue point into FuncInfo
		{
			Root->StringValue = FuncInfo->Name;
			if( FuncInfo->ParseFlags & SLANG_DATA_TYPE_CONSTRUCTOR )
			{
				Root->Type = SLANG_DATA_TYPE_CREATOR;
				if( !( Root->ValueDataType = DtFromName( FuncInfo->Name )))
				{
					ErrMore( "Could not load datatype '%s'", FuncInfo->Name );
					goto Abort;
				}
			}
		}
		Root->Argc = DtSize( Args );
		Argv = Root->Argv = (SLANG_NODE **) malloc( sizeof( SLANG_NODE *) * Root->Argc );
	}
	else
	{
		Root->Argc = DtSize( Args );
		if( Root->Argc < 1 )
		{
		  BadMemberFuncArgs:
			Err( ERR_INVALID_ARGUMENTS, "Member function call: usage Apply( A, [ \"B\", ... ] )" );
			goto Abort;
		}

		if( !( Root->StringValue = (char *) DtSubscriptGetPointer( Args, 0, DtString, NULL ))) // FIX- shouldn't cast away const but I know it isn't going to be freed
			goto BadMemberFuncArgs;
		Root->Type = SLANG_MEMBER_FUNC;
		Root->FuncPtr = SlangNodeTypeLookup( Root->Type, NULL );

		Argv = Root->Argv = (SLANG_NODE **) malloc( sizeof( SLANG_NODE *) * Root->Argc );

		*Argv = SLANG_NODE_CALLOC();
		(*Argv)->FuncPtr = SlangNodeTypeLookup( (*Argv)->Type = SLANG_TYPE_CONSTANT, NULL );
		(*Argv)->ErrorInfo = *ErrorInfo;

		DTM_ASSIGN( &TmpValue, Func );
		SLANG_NODE_SET_VALUE( *Argv, &TmpValue );
		++Argv;
		FirstArg = 1;
	}

	for( Arg = FirstArg; Argv < Root->Argv + Root->Argc; ++Argv, ++Arg )
	{
		*Argv = SLANG_NODE_CALLOC();
		(*Argv)->FuncPtr = SlangNodeTypeLookup( (*Argv)->Type = SLANG_TYPE_CONSTANT, NULL );
		(*Argv)->ErrorInfo = *ErrorInfo;

		DTM_ASSIGN( &TmpValue, DtSubscriptGet( Args, Arg, NULL ));
		SLANG_NODE_SET_VALUE( *Argv, &TmpValue );
	}
	return Root;

Abort:
	free( Root->StringValue );
	SLANG_NODE_FREE( Root );
	return NULL;
}



/****************************************************************
**	Routine: SlangApplyEach
**	Returns: RetCode, e.g., SLANG_OK
**	Action : Evaluates Root for each set of args implied by the
** 			 given EachArray (args missing from EachArray are
**			 left as-is for each evaluation).
****************************************************************/

SLANG_RET_CODE SlangApplyEach(
	SLANG_NODE		*Root,
	SLANG_EVAL_FLAG	EvalFlag,
	SLANG_RET		*Ret,
	SLANG_SCOPE		*Scope,
	DT_VALUE		**EachArray		// Array of size Root->Argc with NULLs or pointers to Each()s
)
{
	SLANG_RET_CODE
			RetCode = SLANG_OK;

	int		Elem,
			ArgNum,
			NewSize,
			Eachiness = -1;

	SLANG_NODE
			TempRoot,
			**EachNode;

	SLANG_RET
			TempRet;

	DT_VALUE
			*EachArg;


/*
**	Construct a fake Root where we will replace Each() args with constant nodes,
**	filling these with each value in turn
*/

	DTM_INIT( &Ret->Data.Value );

	TempRoot = *Root;
	if( !ERR_CALLOC( TempRoot.Argv, SLANG_NODE *, sizeof( SLANG_NODE *), 2 * TempRoot.Argc ))
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	memcpy( TempRoot.Argv, Root->Argv, sizeof( SLANG_NODE *) * TempRoot.Argc );

	EachNode = &TempRoot.Argv[ TempRoot.Argc ];
	for( ArgNum = 0; ArgNum < TempRoot.Argc; ArgNum++ )
	{
		EachArg = EachArray[ ArgNum ];
		if( EachArg && EachArg->DataType == DtEach )
		{
			TempRoot.Argv[ ArgNum ] = SLANG_NODE_CALLOC();
			if( !TempRoot.Argv[ ArgNum ] )
				return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
			TempRoot.Argv[ ArgNum ]->FuncPtr = SlangNodeTypeLookup( TempRoot.Argv[ ArgNum ]->Type = SLANG_TYPE_CONSTANT, NULL );
			TempRoot.Argv[ ArgNum ]->ErrorInfo = Root->Argv[ ArgNum ]->ErrorInfo;
			EachNode[ ArgNum ] = TempRoot.Argv[ ArgNum ];

			NewSize = DtSize( EachArg );
			if( Eachiness == -1 )
				Eachiness = NewSize;
			else if( Eachiness != NewSize )
			{
				RetCode = SlangEvalError( SLANG_ERROR_PARMS, "SlangApplyEach: Mismatched size of Each() args" );
				goto Cleanup;
			}
		}
	}

	Ret->Type = SLANG_TYPE_VALUE;
	if( !DTM_ALLOC( &Ret->Data.Value, DtEach ))
		// Do not bother with cleanup--heap is hosed anyway
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );


/*
**	Evaluate the fake root repeatedly for each each() elem
*/

	for( Elem = 0; Elem < Eachiness; Elem++ )
	{
		int 	Status = TRUE;


		// Fill in Each() args with values for given Elem number
		for( ArgNum = 0; ArgNum < TempRoot.Argc; ArgNum++ )
			if( EachNode[ ArgNum ] )
				SLANG_NODE_SET_VALUE( EachNode[ ArgNum ], DtSubscriptGet( EachArray[ ArgNum ], Elem, NULL ));

		// Evaluate function
		RetCode = SlangEvaluate( &TempRoot, EvalFlag, &TempRet, Scope );
		if( RetCode != SLANG_OK )
			goto Cleanup;

		// Steal the value from TempRet if we can (to avoid copying)
		if( TempRet.Type == SLANG_TYPE_VALUE )
		{
			Status = DtAppendNoCopy( &Ret->Data.Value, &TempRet.Data.Value );
			TempRet.Type = SLANG_TYPE_STATIC_VALUE;
		}
		else
			Status = DtAppend( &Ret->Data.Value, &TempRet.Data.Value );

		SLANG_RET_FREE( &TempRet );
		if( !Status )
			goto Cleanup;
	}


Cleanup:
	for( ArgNum = 0; ArgNum < TempRoot.Argc; ArgNum++ )
		if( EachNode[ ArgNum ] )
			SLANG_NODE_FREE( TempRoot.Argv[ ArgNum ] );
	free( TempRoot.Argv );

	if( RetCode != SLANG_OK )
		SLANG_RET_FREE( Ret );

	// Return the exception if it occures
	if(SLANG_EXCEPTION == RetCode)
	{
		*Ret = TempRet;
	}

	return RetCode;
}



/****************************************************************
**	Routine: SlangMemFunMapLookup
**	Returns: SLANG_MEM_FUN_MAP* / NULL
**	Action : looks up the mem fun map for a given GsType
****************************************************************/

SLANG_MEM_FUN_MAP const* SlangMemFunMapLookup(
	string const& type
)
{
	if( !type_name_to_mem_funs )
		return 0;

	String_To_Slang_Mem_Funs_Map::iterator it = type_name_to_mem_funs->find( type );
	if( it == type_name_to_mem_funs->end() )
		return 0;

	return &(*it).second;
}
