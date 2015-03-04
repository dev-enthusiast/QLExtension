/**************************************************************** -*-c++-*-
**
**	SECEXPR.H	- Security database expression evaluation
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secexpr.h,v 1.191 2014/10/21 21:39:38 e038613 Exp $
**
****************************************************************/

#ifndef IN_SECEXPR_H
#define IN_SECEXPR_H

#include	<secdb.h>

#include	<hash.h>		// for HashStrHash and Hash
#include	<port_int.h>	// for intptr_t

#include	<memory>
#include	<stack>
#include	<ccstl.h>
#include	<map>
#include	<string> 		// for std::string

#include	<time.h>
#include	<stdio.h>
#include    <strings.h>		// for strcasecmp

// Forward declarations

struct SLANG_DATA_TYPE;
struct SLANG_VARIABLE;
typedef SLANG_DATA_TYPE DT_SLANG;

/*
**	Define constants
*/

unsigned const SLANG_STRING_SIZE		= 128;			// Max size of string returned by SlangEvaluate
unsigned const SLANG_VARIABLE_NAME_SIZE	= 64;			// Max variable name size + 1
unsigned const SLANG_MAX_NESTING_DEPTH	= 256;			// Max function nesting depth


/*
**	Standard function arguments
*/

#define	SLANG_ARGS					\
		SLANG_NODE		*Root,		\
		SLANG_EVAL_FLAG	EvalFlag,	\
		SLANG_RET		*Ret,		\
		SLANG_SCOPE		*Scope

#define	SLANG_ERROR_PARMS		Root, EvalFlag, Ret, Scope


/*
**	Argument flags
*/

int const SLANG_ARG_NULLS		= 0x0001;		// Allow null args
int const SLANG_ARG_OPTIONAL	= 0x0002;		// Optional argument
int const SLANG_ARG_LVALUE		= 0x0004;		// Arg is LVALUE
int const SLANG_ARG_VAR_FLAG	= 0x0008;		// Arg is Variable (stricter than LVALUE)
int const SLANG_ARG_BLOCK		= 0x0010;		// Arg is {Block} (unevaluated)
int const SLANG_ARG_EACH		= 0x0020;		// Arg can be an Each(); do not automatically expand it

int const SLANG_ARG_VARIABLE	= (SLANG_ARG_VAR_FLAG | SLANG_ARG_LVALUE);


/*
**	Eval flags
*/

/*
**	Eval flags
*/

enum SLANG_EVAL_FLAG
{
	SLANG_EVAL_RVALUE		 = TRUE,	// Get value of expression
	SLANG_EVAL_NO_VALUE		 = -4,		// Evaluate, but don't need to return a value
	SLANG_EVAL_PARSE_TIME	 = -1,		// Get value of expression at parse-time
	SLANG_EVAL_LVALUE		 = FALSE,	// Get pointer to expression
	SLANG_EVAL_LVALUE_CREATE = -2,		// Get pointer to expression (create uninitialized variable if necessary)
	SLANG_EVAL_LVALUE_NEWREF = -3,		// Get pointer to expression and set REFERENCE bit
	SLANG_EVAL_HAS_RETURN	 = -5		// Does this block/if/while/... end in Return()?
};

// FIX-Should be a define to control this
#if 1
#define DEBUG_EXPECT_NO_VALUE													\
	if( SlangExpectNoValueState != SLANG_EXPECT_OFF )							\
	{																			\
		SLANG_RET_CODE const RetCode = SlangExpectNoValue( SLANG_ERROR_PARMS );	\
		if( RetCode != SLANG_OK )												\
			return RetCode;														\
	}
#else
#define DEBUG_EXPECT_NO_VALUE
#endif


/*
**	Negative argc values for usage info
*/

int const SLANG_ARGC_USAGE		= (-1);
int const SLANG_ARGC_INFO		= (-2);


/*
**	Module types
*/

int const SLANG_MODULE_UNKNOWN	= 0;
int const SLANG_MODULE_FILE		= 1;
int const SLANG_MODULE_OBJECT	= 2;
int const SLANG_MODULE_STRING	= 3;
int const SLANG_MODULE_KEYBOARD	= 4;
int const SLANG_MODULE_USER		= 5;			// User supplied from here on


/*
**	Codes returned by SlangEvaluate
*/

typedef int SLANG_RET_CODE;
SLANG_RET_CODE const SLANG_RC_VALUE_FLAG	= 0x0100;		// Does Ret contain a valid value (which must be freed)?
SLANG_RET_CODE const SLANG_RC_OK_FLAG		= 0x0200;		// Did the SlangEvaluate succeed?
SLANG_RET_CODE const SLANG_RC_EARLY_RETURN	= 0x0400;		// Return early from this function? (Error, AbortXXX, Return)
SLANG_RET_CODE const SLANG_RC_ARG_PARSE_RET = 0x0800;		// SlangArgParse did everything recursively, return immediately

SLANG_RET_CODE const SLANG_OK				= 0x0000;		// 03
SLANG_RET_CODE const SLANG_ERROR			= 0x0001;		// 04
SLANG_RET_CODE const SLANG_ABORT			= 0x0002;		// 04
SLANG_RET_CODE const SLANG_BREAK			= 0x0003;		// 00
SLANG_RET_CODE const SLANG_RETURN			= 0x0004;		// 05
SLANG_RET_CODE const SLANG_CONTINUE		    = 0x0005;		// 00
SLANG_RET_CODE const SLANG_EXIT			    = 0x0006;		// 05
SLANG_RET_CODE const SLANG_RETRY			= 0x0007;		// 00
SLANG_RET_CODE const SLANG_BACKUP			= 0x0008;		// 00
SLANG_RET_CODE const SLANG_TRANS_ABORT		= 0x0009;		// 04
SLANG_RET_CODE const SLANG_ARG_INFO		    = 0x000A;		// 00
SLANG_RET_CODE const SLANG_EXCEPTION		= 0x000B;		// 05
SLANG_RET_CODE const SLANG_RDB_TRANS_ABORT	= 0x000C;		//

/*
**	Flags for Slang Trim function
*/

int const SLANG_TRIM_LEADING	= 0x0001;
int const SLANG_TRIM_TRAILING	= 0x0002;


/*
**	Define types
*/

struct SLANG_SCOPE;						// forward declare
struct SLANG_SCOPE_FUNCTION;
struct SLANG_NODE;
struct SLANG_RET;

typedef int SLANG_NODE_TYPE;
typedef int SLANG_PARSE_FLAGS;
typedef	SLANG_RET_CODE (*SLANG_FUNC)( SLANG_ARGS );
typedef int (*SLANG_ABORT_FUNC)( void* Handle, SLANG_NODE* Root, SLANG_SCOPE* Scope );
typedef int (*SLANG_ERROR_FUNCTION)( void* Handle, SLANG_NODE* Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET* Ret, SLANG_SCOPE* Scope, int ErrCode );
typedef int (*SLANG_ATTACH_PROFILER)( SLANG_NODE* Root, int Detach );

typedef SLANG_RET_CODE (*SLANG_VAR_GET_FUNC)( SLANG_SCOPE_FUNCTION *Scope, const char *VarName, SLANG_RET *Ret, SLANG_EVAL_FLAG EvalCode );
typedef int (*SLANG_VAR_GET_VAR_FUNC)( SLANG_SCOPE_FUNCTION *Scope, const char *VarName, SLANG_VARIABLE *Variable );

enum SLANG_EXPECT_NO_VALUE_STATE
{
	SLANG_EXPECT_OFF,			// No checking
	SLANG_EXPECT_LOG,			// Just log via ErrMsg( ERR_LEVEL_INFO )
	SLANG_EXPECT_ERROR			// Fail at run time
};

struct lexical_context;				// forward declare
struct SlangDebugContextStructure;	// forward declare


/*
**	SLANG_ERROR_FUNCTION error codes
*/

int const SLANG_ERROR_FLAG		= 0x1000;
int const SLANG_WARN_FLAG		= 0x2000;

int const SLANG_ERROR_UNKNOWN	= 0x1000;		// Generic error
int const SLANG_WARN_NO_VALUE	= 0x2001;		// Expected No Value eval mode
int const SLANG_WARN_NO_RETURN	= 0x2002;		// Expected Return() in function


/*
**	Slang Node Types
*/

// Value Type
int const SLANG_VALUE_TYPE			  = 0x0000;

// Operators
SLANG_NODE_TYPE const SLANG_OPERATOR_MASK         = 0x0100;
SLANG_NODE_TYPE const SLANG_OPERATOR_BASE         = 0x0100;
SLANG_NODE_TYPE const SLANG_ASSIGN				  = SLANG_OPERATOR_BASE | 0x00;
SLANG_NODE_TYPE const SLANG_ADD_ASSIGN			  = SLANG_OPERATOR_BASE | 0x01;
SLANG_NODE_TYPE const SLANG_SUBTRACT_ASSIGN		  = SLANG_OPERATOR_BASE | 0x02;
SLANG_NODE_TYPE const SLANG_MULTIPLY_ASSIGN		  = SLANG_OPERATOR_BASE | 0x03;
SLANG_NODE_TYPE const SLANG_DIVIDE_ASSIGN		  = SLANG_OPERATOR_BASE | 0x04;
SLANG_NODE_TYPE const SLANG_COLON_ASSIGN		  = SLANG_OPERATOR_BASE | 0x05;
SLANG_NODE_TYPE const SLANG_ADD					  = SLANG_OPERATOR_BASE | 0x06;
SLANG_NODE_TYPE const SLANG_SUBTRACT			  = SLANG_OPERATOR_BASE | 0x07;
SLANG_NODE_TYPE const SLANG_MULTIPLY			  = SLANG_OPERATOR_BASE | 0x08;
SLANG_NODE_TYPE const SLANG_DIVIDE				  = SLANG_OPERATOR_BASE | 0x09;
SLANG_NODE_TYPE const SLANG_UNARY_MINUS			  = SLANG_OPERATOR_BASE | 0x0A;
SLANG_NODE_TYPE const SLANG_EQUAL				  = SLANG_OPERATOR_BASE | 0x0B;
SLANG_NODE_TYPE const SLANG_NOT_EQUAL			  = SLANG_OPERATOR_BASE | 0x0C;
SLANG_NODE_TYPE const SLANG_GREATER_THAN		  = SLANG_OPERATOR_BASE | 0x0D;
SLANG_NODE_TYPE const SLANG_LESS_THAN			  = SLANG_OPERATOR_BASE | 0x0E;
SLANG_NODE_TYPE const SLANG_GREATER_OR_EQUAL	  = SLANG_OPERATOR_BASE | 0x0F;
SLANG_NODE_TYPE const SLANG_LESS_OR_EQUAL		  = SLANG_OPERATOR_BASE | 0x10;
SLANG_NODE_TYPE const SLANG_LOGICAL_AND			  = SLANG_OPERATOR_BASE | 0x11;
SLANG_NODE_TYPE const SLANG_LOGICAL_OR			  = SLANG_OPERATOR_BASE | 0x12;
SLANG_NODE_TYPE const SLANG_UNARY_NOT			  = SLANG_OPERATOR_BASE | 0x13;
SLANG_NODE_TYPE const SLANG_ADDRESS_OF			  = SLANG_OPERATOR_BASE | 0x14;
SLANG_NODE_TYPE const SLANG_AND_ASSIGN			  = SLANG_OPERATOR_BASE | 0x15;
SLANG_NODE_TYPE const SLANG_POST_INCREMENT		  = SLANG_OPERATOR_BASE | 0x16;
SLANG_NODE_TYPE const SLANG_POST_DECREMENT		  = SLANG_OPERATOR_BASE | 0x17;
SLANG_NODE_TYPE const SLANG_PRE_INCREMENT		  = SLANG_OPERATOR_BASE | 0x18;
SLANG_NODE_TYPE const SLANG_PRE_DECREMENT		  = SLANG_OPERATOR_BASE | 0x19;
SLANG_NODE_TYPE const SLANG_COMPARE				  = SLANG_OPERATOR_BASE | 0x1A;
SLANG_NODE_TYPE const SLANG_SCOPE_OPERATOR		  = SLANG_OPERATOR_BASE | 0x1B;
SLANG_NODE_TYPE const SLANG_POUND				  = SLANG_OPERATOR_BASE | 0x1C;
SLANG_NODE_TYPE const SLANG_STAR				  = SLANG_OPERATOR_BASE | 0x1D;
SLANG_NODE_TYPE const SLANG_BINARY_AT_OP		  = SLANG_OPERATOR_BASE | 0x1E;
inline bool SLANG_IS_OPERATOR(SLANG_NODE_TYPE Type) { return (Type & SLANG_OPERATOR_MASK ) != 0;}

// Syntactical constructs
SLANG_NODE_TYPE const SLANG_CONSTRUCT_MASK        = 0x0300;
SLANG_NODE_TYPE const SLANG_CONSTRUCT_BASE        = 0x0300;
SLANG_NODE_TYPE const SLANG_COMPONENT			  = SLANG_CONSTRUCT_BASE | 0x00;
SLANG_NODE_TYPE const SLANG_BLOCK				  = SLANG_CONSTRUCT_BASE | 0x01;	// { ... }
SLANG_NODE_TYPE const SLANG_SUBSCRIPT			  = SLANG_CONSTRUCT_BASE | 0x02;
SLANG_NODE_TYPE const SLANG_WHILE				  = SLANG_CONSTRUCT_BASE | 0x04;
SLANG_NODE_TYPE const SLANG_ARRAY_INIT			  = SLANG_CONSTRUCT_BASE | 0x05;
inline bool SLANG_IS_CONSTRUCT(SLANG_NODE_TYPE Type) { return (Type & SLANG_CONSTRUCT_MASK ) != 0;}

// Functions
SLANG_NODE_TYPE const SLANG_FUNC_MASK             = 0x0400;
SLANG_NODE_TYPE const SLANG_FUNC_BASE             = 0x0400;
SLANG_NODE_TYPE const SLANG_RESERVED_FUNC		  = SLANG_FUNC_BASE | 0x00;
SLANG_NODE_TYPE const SLANG_AT_FUNC				  = SLANG_FUNC_BASE | 0x01;
SLANG_NODE_TYPE const SLANG_DOUBLE_AT_FUNC 		  = SLANG_FUNC_BASE | 0x02;
SLANG_NODE_TYPE const SLANG_MEMBER_FUNC	 		  = SLANG_FUNC_BASE | 0x03;
SLANG_NODE_TYPE const SLANG_VALUE_TYPE_FUNC		  = SLANG_FUNC_BASE | 0x04;
SLANG_NODE_TYPE const SLANG_TYPE_DOLLAR			  = SLANG_FUNC_BASE | 0x05;
inline bool SLANG_IS_FUNC(SLANG_NODE_TYPE Type) { return (Type & SLANG_FUNC_MASK ) != 0;}

// Base types
SLANG_NODE_TYPE const SLANG_BASE_TYPE_MASK        = 0x0800;
SLANG_NODE_TYPE const SLANG_BASE_TYPE_BASE        = 0x0800;
SLANG_NODE_TYPE const SLANG_TYPE_VALUE			  = SLANG_BASE_TYPE_BASE | 0x00;
SLANG_NODE_TYPE const SLANG_TYPE_VARIABLE		  = SLANG_BASE_TYPE_BASE | 0x05;
SLANG_NODE_TYPE const SLANG_TYPE_NULL			  = SLANG_BASE_TYPE_BASE | 0x06;
SLANG_NODE_TYPE const SLANG_TYPE_STATIC_VALUE	  = SLANG_BASE_TYPE_BASE | 0x07;
SLANG_NODE_TYPE const SLANG_TYPE_RET			  = SLANG_BASE_TYPE_BASE | 0x08;
SLANG_NODE_TYPE const SLANG_TYPE_NO_VALUE		  = SLANG_BASE_TYPE_BASE | 0x09;
SLANG_NODE_TYPE const SLANG_TYPE_CONSTANT		  = SLANG_BASE_TYPE_BASE | 0x0A;
SLANG_NODE_TYPE const SLANG_TYPE_VARIABLE_INDEX   = SLANG_BASE_TYPE_BASE | 0x0B;
SLANG_NODE_TYPE const SLANG_TYPE_LOCAL_VARIABLE   = SLANG_BASE_TYPE_BASE | 0x0C;
SLANG_NODE_TYPE const SLANG_TYPE_SCOPED_VARIABLE  = SLANG_BASE_TYPE_BASE | 0x0D;
inline bool SLANG_IS_BASE_TYPE(SLANG_NODE_TYPE Type) { return (Type & SLANG_BASE_TYPE_MASK ) != 0;}

SLANG_NODE_TYPE const SLANG_VALUE_POINTER		  = 0x1000;

SLANG_NODE_TYPE const SLANG_DATA_TYPE_CREATOR	  = 0x2000;     // data type creator functions, e.g. Date( "11mar92" )

// replace with evaluated child
SLANG_NODE_TYPE const SLANG_TYPE_EVAL_ONCE        = 0x4000;

/*
**	Slang Node Flags
*/

int const SLANG_NODE_FREE_STRING      = 0x0001;     // StringValue belongs to us
int const SLANG_NODE_FREE_MODULE      = 0x0002;     // ErrorInfo.ModuleName belongs to us
int const SLANG_NODE_STRING_EQ_VALUE  = 0x0004; 	// StringValue == ValueData.Pointer
int const SLANG_NODE_NO_BUCKET        = 0x0008; 	// Do not copy the bucket when copying parse trees
int const SLANG_NODE_NO_VALUE         = 0x0010;     // Don't free Value in node when freeing Node
int const SLANG_NODE_PUSH_SCOPE       = 0x0020; 	// This node pushes a new scope underneath itself
int const SLANG_NODE_DONT_FREE        = 0x0040; 	// Don't free this node even if someone asks you to
int const SLANG_NODE_NAMED_CONST      = 0x0080; 	// This is a named constant, don't print with quotes


/*
**	Define parse flags
*/

SLANG_PARSE_FLAGS const SLANG_BIND_BLOCK            = 0x0001;		// Function expects trailing block of code
SLANG_PARSE_FLAGS const SLANG_NO_PARENS             = 0x0002;		// No parens (or arguments, except perhaps bound block)
SLANG_PARSE_FLAGS const SLANG_BIND_COLON            = 0x0004;		// Optional trailing : and block (for if then else)
SLANG_PARSE_FLAGS const SLANG_PARSE_TIME            = 0x0008;		// Call function at parse time
SLANG_PARSE_FLAGS const SLANG_DEPRECATED            = 0x0010;		// Do not include in any function help
SLANG_PARSE_FLAGS const SLANG_EVAL_ONCE             = 0x0020;		// Function returns constants when acting on constants
SLANG_PARSE_FLAGS const SLANG_CHECK_RETURN          = 0x0040;		// Might contain a return statement (supports SLANG_EVAL_HAS_RETURN)
SLANG_PARSE_FLAGS const SLANG_DATA_TYPE_CONSTRUCTOR = 0x0080;     // Is a datatype constructor
SLANG_PARSE_FLAGS const SLANG_BIND_COLON_ALWAYS     = 0x0100;		// trailing : and block (for if then else)
											        // if : is missing a block which evaluates to Null is bound
SLANG_PARSE_FLAGS const SLANG_EVAL_ONCE_ALWAYS      = 0x0200;		// Always replace with evaluated child
SLANG_PARSE_FLAGS const SLANG_FUNC_PUSH_SCOPE       = 0x0400;		// tags this func as pushing a new scope, translates to SLANG_NODE_PUSH_SCOPE on the nodes
SLANG_PARSE_FLAGS const SLANG_MEM_FUN               = 0x0800; // Is a member function


/*
**	Define variable flags
*/

int const SLANG_VARIABLE_RETAIN		  = 0x0001;		// Don't free value when freeing variable
int const SLANG_VARIABLE_PROTECT	  = 0x0002;		// Don't allow variable to be set/freed
int const SLANG_VARIABLE_REFERENCE	  = 0x0004;		// Always dereference when accessing
int const SLANG_VARIABLE_TEMPORARY	  = 0x0008;		// Do not bother doing garbage collection
int const SLANG_VARIABLE_NO_COPY	  = 0x0010;		// Don't copy when assigning, but free when done
int const SLANG_VARIABLE_DEBUG_READ	  = 0x0020;		// Break when accessing variable
int const SLANG_VARIABLE_DEBUG_WRITE  = 0x0040;		// Break when setting variable
int const SLANG_VARIABLE_FORMAL_PARAM = 0x0080;		// Formal Param, @@ must free, @ frees automatically
int const SLANG_VARIABLE_DESTROYED    = 0x0100; 	// Treat variable like it doesn't exist
int const SLANG_VARIABLE_FREE_NAME    = 0x0200; 	// Free name when variable is freed
int const SLANG_VARIABLE_LOCAL        = 0x0400; 	// A local variable

/*
** DefineClass flags
*/

unsigned const SLANG_DEFINE_CLASS_ABSTRACT 	= 0x0001;	// Abstract class
unsigned const SLANG_DEFINE_CLASS_INTERFACE	= 0x0002;	// Interface, not a real class at all
unsigned const SLANG_DEFINE_CLASS_SELF_DESC = 0x0004;	// Supports self-desc streaming


/*
**	Structure used to contain a variable
*/

struct SLANG_VARIABLE
{
	DT_VALUE
			*Value;

	char const*
			Name;

	HASH*
			HashTable;

	int		Flags;

	int		ReferenceCount;

};


/*
**	Structure used to contain a variable scope
*/

struct SLANG_VARIABLE_SCOPE
{
	HASH
			*VariableHash;

	SLANG_DATA_TYPE
			*UninitializedVarHook;

	inline bool RunUninitVarHook( char const* VarName, char const* ScopeName, SLANG_NODE* Root, SLANG_SCOPE* Scope, DT_VALUE* Value );
};


/*
**	Structure used to contain constants
*/

struct SLANG_CONSTANT
{
	char	*Name,
			*EnumName;			// Possibly NULL

	DT_VALUE
			ConstValue;

};

struct SLANG_ENUM
{
	char	*EnumName;

	HASH		// Hash table of SLANG_CONSTANTS
			*Members;

};


/*
**	Structure used to contain function information
*/

struct SLANG_FUNCTION_INFO
{
	char	*Name,
			*DllPath,
			*FuncName;

	SLANG_NODE_TYPE
			NodeType;

	SLANG_PARSE_FLAGS
			ParseFlags;

	SLANG_FUNC
			Func;

	void	*Handle;

};


/*
**	Structure used to hold error/position information
*/

struct SLANG_ERROR_INFO
{
    SLANG_ERROR_INFO() : ModuleName(0), ModuleType(0), BeginningLine(0),
			BeginningColumn(0), EndingLine(0), EndingColumn(0) {}
	const char
			*ModuleName;

	short	ModuleType,
			BeginningLine,
			BeginningColumn,		// Tabs are assumed to be one column
			EndingLine,
			EndingColumn;

};


/*
**	Union used to store DT_VALUEs in the parse tree
*/

union SLANG_NODE_VALUE_RESULTS
{
	double	Number;

	void	*Pointer;

	char	*DbgStr;		// Used in debugger

	struct
	{
		void	*Pointer;

		unsigned long
				Bucket;

	} NodeInfo;

};


/*
**	Structure used as a node on the parsed tree
*/

struct SLANG_NODE
{
	SLANG_NODE_TYPE
			Type;

	SLANG_FUNC
			FuncPtr;

	DT_DATA_TYPE
			ValueDataType;

	char	*StringValue;		// ComponentName or StringValue

	SLANG_NODE
			**Argv;

	void	*Extra;				// Application supplied

	SLANG_NODE_VALUE_RESULTS
			ValueData;

	SLANG_ERROR_INFO
			ErrorInfo;

	short	Argc;

	unsigned char
			Flags;

};

struct SLANG_RET
{
	int		Type;

	union
	{
		DT_VALUE
				Value;

		// FIX -- this should go into DtPointer
		struct
		{
			char	SecName[ SDB_SEC_NAME_SIZE ];

			SDB_VALUE_TYPE
					ValueType;

			int		Argc;

			DT_VALUE
					**Argv;

		} Eval;

	} Data;

};


/*
**	Debug function
*/

typedef int (*SLANG_DEBUG_FUNC)( void *Handle, SLANG_NODE *Root, SLANG_SCOPE *Scope );


/*
**	Define structures used to maintain scope
*/

/*
** Diddle structures are really private to sdb_eval.c (WBR -961018)
*/

struct SLANG_SCOPE_DIDDLE
{
	SDB_OBJECT
			*SecPtr;

	SDB_VALUE_TYPE
			ValueType;

	SDB_DIDDLE_ID
			DiddleID;

	int		Argc;

	DT_VALUE
			**Argv;

};

struct SLANG_SCOPE_DIDDLES
{
    SLANG_SCOPE_DIDDLES
			*Next;

	HASH
			*DiddleHash;

	int		DiddleDepth;
    int     TryDepth;

};

struct SLANG_SCOPE_FUNCTION
{
    SLANG_SCOPE_FUNCTION
			*Next;

	const char
			*FunctionName;

	SLANG_DATA_TYPE
			*SlangFunc;

	SLANG_ERROR_INFO
			*ErrorInfo;					// current eval location in func

	SLANG_VARIABLE
			**Locals;					// List of locals

	HASH
			*VariableHash;				// Hash for badly behaved functions

	SLANG_DATA_TYPE
			*UninitializedVarHook;		// Hook for uninitialized variables

	inline bool RunUninitVarHook( SLANG_NODE *Root, SLANG_SCOPE *Scope, DT_VALUE *Value );

};

struct SLANG_SCOPE
{
	SLANG_SCOPE_DIDDLES
			*Diddles;

	SLANG_SCOPE_FUNCTION
			*FunctionStack;

	int		DiddleDepth;
	int		TryDepth;

};


/*
**	Define structures used for expression context
*/

struct SLANG_CONTEXT
{
	SLANG_CONTEXT
			*Next;

	HASH
			*Hash;

	long	UpdateCount;

	SLANG_ABORT_FUNC
			AbortFunc;					// "Abort Function"

	void	*AbortHandle;				// "Abort Handle"

	int		AbortCheckGranularity;		// "Abort Check Granularity"

	int		Aborted;					// If aborted and abort function has not yet handled it

	SlangDebugContextStructure
			*DebugContext;				// Slang debugger context

};

struct SLANG_CONTEXT_ELEMENT
{
	char	*Name;

	void	*Value;

};

// The "Print Function" in the context is of this type (uses "Print Handle")
typedef int (*SLANG_PRINT_FUNCTION)( void *Handle, char const* String );


/*
**	Structure to hold default value and DataType info for a function parameter
*/

struct SLANG_FUNC_PARAM
{
	char	*Name;

	short	Index;				// Original parameter index ( array is sorted by name )

	unsigned
			PassByName : 1,		// if pass by name is allowed
			PassByReference : 1;// if is pass by reference

	DT_DATA_TYPE
			DataType;

	SLANG_NODE
			*DefaultExpression;

	// A place to store away the original string that was used as a
	// datatype constructor for this argument.

	const char
			*DtConstructorName;
};


/*
**	Structure used to hold a slang data type
*/

struct SLANG_DATA_TYPE
{
	int		ReferenceCount;

	SLANG_NODE
			*Expression;

	bool    HasReturn;
	short	RequiredPositionalArgs;	// Number of required positional arguments

	SLANG_FUNC_PARAM
			*Params;

	HASH
			*Locals;				// Hash of local variables

	char const** LocalNames;		// List of local names in order

	SDB_CHILD_LIST
			*ChildList;				// UFO stores a pointer to the VTI's ChildList after it parses the function

	SLANG_FUNC_PARAM
			**ParamsByName;			// Indices of params sorted by name

	SLANG_FUNC_PARAM
			*ParamByIndex( unsigned int Index )
	{
		return Params + Index;
	}

};


/*
**	hook used to free the dependency info which Ufo hangs onto a DT_SLANG
*/

typedef void (*DT_SLANG_FREE_HOOK)( DT_SLANG* );


/*
**	Structure used to make argument parsing easier
*/

struct SLANG_ARG_LIST
{
	const char
			*Name;

	DT_DATA_TYPE
			*DataType;

	const char
			*Description;

	int		Flags;

};

struct Mem_Fun_Comp
{
public:
	bool operator()( char const* str1, char const* str2 ) const { return strcasecmp( str1, str2 ) < 0; }
};

typedef CC_STL_MAP_WITH_COMP( char const*, SLANG_FUNCTION_INFO*, Mem_Fun_Comp ) SLANG_MEM_FUN_MAP;

/*
**	Global variables structure
*/

EXPORT_C_SECDB SLANG_ERROR_INFO
		SlangErrorInfo;			// Global error info

EXPORT_C_SECDB SLANG_CONTEXT
		*SlangContext;			// Global slang context

EXPORT_C_SECDB HASH
		*SlangVariableHash;		// Hash table for global variables

EXPORT_C_SECDB HASH
        *SlangRootHash;          // Roots and their memory

EXPORT_C_SECDB HASH
        *SlangFunctionHash;      // Functions

EXPORT_C_SECDB HASH
        *SlangConstantsHash;     // Constants

EXPORT_C_SECDB HASH
        *SlangEnumsHash;         // Enums

EXPORT_C_SECDB HASH
        *SlangVarScopeHash;      // Variable scopes

EXPORT_C_SECDB HASH
        *SlangNodeTypeHash;      // Node types

EXPORT_C_SECDB HASH
        *SlangNodesByName;       // NodeTypes by constant name

EXPORT_C_SECDB HASH
        *SlangNodesByType;       // NodeTypes by constant value


EXPORT_C_SECDB SLANG_SCOPE_DIDDLES
		*SlangStickyDiddles;	// Global sticky diddles list

EXPORT_C_SECDB unsigned
		SlangFunctionDepth;		// Depth of call stack

EXPORT_C_SECDB SLANG_EXPECT_NO_VALUE_STATE
		SlangExpectNoValueState;	// Enable checking for Loop blocks

EXPORT_C_SECDB int
		SlangCheckReturnEnabled;

EXPORT_C_SECDB SLANG_ATTACH_PROFILER
		SlangAttachProfiler;	// Give profiler a crack at parse trees before throwing them away


/*
**	Prototype functions
*/

EXPORT_C_SECDB void
		SlangInitialize(			void );

EXPORT_C_SECDB lexical_context
		*SlangCLexNew(				char const* File, char const* String );

EXPORT_C_SECDB SLANG_NODE
		*SlangParse(				char const* String, char const* ModuleName, int ModuleType ),
		*SlangParseWithScope(		char const* String, char const* ModuleName, int ModuleType, char const* PrivateScope ),
		*SlangParseFile(			char const* File, char const* ModuleName, int ModuleType ),
		*SlangParseCLex(			lexical_context* Ctx, char const* ModuleName, int ModuleType ),
		*SlangParseAttach(			SLANG_NODE* Root, char const* File, char const* String, char const* ModuleName, int ModuleType ),
		*SlangParseTreeCopy(		SLANG_NODE* Root );

SLANG_NODE
		*SlangParseTreeCopyInt(		SLANG_NODE *Node, char const* ModuleName, DT_SLANG *Func );

EXPORT_C_SECDB void
		SlangFree(					SLANG_NODE *Root );

EXPORT_C_SECDB int
		DtSlangExpressionReplace(	DT_VALUE *Value, SLANG_NODE *Expression, short RequiredPositionalArgs, bool HasReturn, DT_SLANG *Source );

EXPORT_C_SECDB void
		SlangRetFree(				SLANG_RET *Ret );

EXPORT_C_SECDB const char
		*SlangNodeNameFromType(		SLANG_NODE_TYPE NodeType );

EXPORT_C_SECDB int
		SlangParseTreeEqual(		SLANG_NODE const *NodeA, SLANG_NODE const *NodeB ),
		SlangParseTreeMemSize(		SLANG_NODE const *Root ),
		SlangFunctionRegister(		char const* Name, char const* DllPath, char const* FuncName, SLANG_FUNC Func, SLANG_PARSE_FLAGS ParseFlags ),
		SlangFunctionRegisterWithHandle(	char const* Name, char const* DllPath, char const* FuncName, SLANG_FUNC Func, SLANG_PARSE_FLAGS ParseFlags, void *Handle ),
		SlangNodeTypeRegister(		SLANG_NODE_TYPE NodeType, char const* DllPath, char const* FuncName, SLANG_FUNC Func, SLANG_PARSE_FLAGS ParseFlags ),
		SlangRegisterConstant(		char const* Name, double Value ),
		SlangRegisterConstantValue(	char const* Name, DT_VALUE *Value ),
		SlangRegisterEnum(			char const* EnumName, char const* Name, double Value ),
		SlangFunctionTableLoad(		char const* Path, char const* FileName, bool OkIfMissing ),
		SlangNodeFuncTableLoad(		char const* Path, char const* FileName );

EXPORT_C_SECDB DT_VALUE
		*SlangConstantLookupValue( 	char const* Name );

EXPORT_C_SECDB SLANG_RET_CODE
		SlangEvalError(				SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, char const* Format, ... ) ARGS_LIKE_PRINTF(5),
		SlangEvalWarn(				SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, int ErrCode, char const* Format, ... ) ARGS_LIKE_PRINTF(6),
		SlangEvalWarnV(				SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, int ErrCode, char const* Format, va_list Marker );

EXPORT_C_SECDB int
		SlangArgInfo(				SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, const SLANG_ARG_LIST *ArgList, int ArgCount ),
		SlangArgError(				SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, const SLANG_ARG_LIST *ArgList, int ArgCount ),
		SlangArgParse(				SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, const SLANG_ARG_LIST *ArgList, int ArgCount, SLANG_RET *Rets );

EXPORT_C_SECDB void
		SlangScopeDestroy(			SLANG_SCOPE *Scope ),
		SlangScopePushDiddles(		SLANG_SCOPE *Scope, SLANG_SCOPE_DIDDLES *ScopeDiddles ),
		SlangScopePopDiddles(		SLANG_SCOPE *Scope, int FreeDiddles ),
		SlangScopeLocalsToHash(		SLANG_SCOPE_FUNCTION *ScopeFuntion );

EXPORT_C_SECDB int
		SlangScopeFreeDiddles(		SLANG_SCOPE_DIDDLES *ScopeDiddles, SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE **Argv ),
		SlangScopeSetDiddle(		SLANG_SCOPE *Scope, SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE **Argv, DT_VALUE *Value),
		SlangScopePopFunction(		SLANG_SCOPE *Scope, bool FreeLocals = true ),
		SlangScopePushLocals(		SLANG_SCOPE *Scope, char const* Name, DT_SLANG *Function, SLANG_ERROR_INFO *CallerErrorInfo, SLANG_VARIABLE **Locals = NULL ),
		SlangUpdateScopeDiddles(    SDB_OBJECT *SecPtr ),
		SlangScopePushVariableHash(	SLANG_SCOPE *Scope, char const* FunctionName, SLANG_NODE *Root, SLANG_ERROR_INFO *CallerErrorInfo, HashStructure *VariableHash );

EXPORT_C_SECDB SDB_DIDDLE_ID
		SlangDiddlesSetDiddleWithFlags(	SLANG_SCOPE_DIDDLES* Diddles, SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE **Argv, DT_VALUE *Value,
										SDB_DIDDLE_ID DiddleID, int Flags, int bChangeOnly );

EXPORT_C_SECDB SLANG_SCOPE_DIDDLES
		*SlangScopeDiddlesNew( const char* sHashName = 0 );

EXPORT_C_SECDB bool
		SlangScopeDiddlesDelete(	SLANG_SCOPE_DIDDLES *ScopeDiddles );

EXPORT_C_SECDB SLANG_SCOPE
		*SlangScopeCreate(			void );


EXPORT_C_SECDB void
		SlangContextSet(			char const* Name, void *Value ),
		*SlangContextGet(			char const* Name ),
		SlangContextPush(			int DupeFlag ),
		SlangContextPop(			void );


EXPORT_C_SECDB SLANG_VARIABLE
		*SlangVariableAlloc(		int AllocValue ),
		*SlangVariableAllocLocal(	char const* Name, int Flags ),
		*SlangVariableSetByBucket( 	char const* Name, DT_VALUE *Value, SLANG_SCOPE_FUNCTION *Scope, int Flags, unsigned long Bucket ),
  		*SlangVariableGetByBucket(	char const* Name, SLANG_SCOPE_FUNCTION *Scope, unsigned long Bucket ),
  		*SlangVariableGetScopedByBucket( HASH *VarScope, char const* Name, unsigned long Bucket ),
  		*SlangVariableSetScopedByBucket( HASH *VarScope, char const* Name, DT_VALUE *Value, int Flags, unsigned long Bucket ),
		*SlangVariableSetLocal(		SLANG_SCOPE_FUNCTION *ScopeFunc, int Num, DT_VALUE *Value, int Flags ),
		*SlangEnumFirstVar(			SDB_ENUM_PTR *EnumPtr, HASH *HashTable ),
		*SlangEnumNextVar(			SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB int
		SlangVariableDestroyByName(	char const* Name, SLANG_SCOPE_FUNCTION *Scope ),
		SlangVariableDestroy(		SLANG_VARIABLE *Variable ),
		SlangVariableSetValue( 		SLANG_VARIABLE *Variable, DT_VALUE *Value, int Flags );

EXPORT_C_SECDB void
		SlangVariableFree(			SLANG_VARIABLE *Variable );

EXPORT_C_SECDB SLANG_VARIABLE
		*SlangVariableIncrRef(		SLANG_VARIABLE *Variable );

EXPORT_C_SECDB void
		SlangEnumVarClose(	  		SDB_ENUM_PTR EnumPtr );


EXPORT_C_SECDB SLANG_VARIABLE_SCOPE
		*SlangVariableScopeGet(		char const* Name, int AutoCreate );

EXPORT_C_SECDB int
		SlangVariableScopeDestroy(	SLANG_VARIABLE_SCOPE *VarScope ),
		SlangVariableHashDestroy(	HASH *VarScope );

EXPORT_C_SECDB SLANG_VARIABLE_SCOPE
		**SlangVariableScopes(		void );		// Result must be free()d


EXPORT_C_SECDB int
		SlangFunctionUsage(			char const* FunctionName ),
		SlangFunctionInfo(			char const* FunctionName, DT_VALUE *RetStructure );

EXPORT_C_SECDB SLANG_FUNC
		SlangFunctionBind(			SLANG_FUNCTION_INFO *FunctionInfo );

EXPORT_C_SECDB SLANG_FUNCTION_INFO
		*SlangFunctionLookup(		char const* FunctionName, unsigned long HashValue );

EXPORT_C_SECDB SLANG_MEM_FUN_MAP const*
		SlangMemFunMapLookup(		CC_NS(std,string) const& type );

EXPORT_C_SECDB char
		*SlangEnumFirstFunction(	SDB_ENUM_PTR *EnumPtr ),
		*SlangEnumNextFunction(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SlangEnumFunctionClose(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB int
		SlangNumConstants(			void );

EXPORT_C_SECDB SLANG_CONSTANT
		*SlangEnumFirstConstant(	SDB_ENUM_PTR *EnumPtr ),
		*SlangEnumNextConstant(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SlangEnumConstantClose(		SDB_ENUM_PTR EnumPtr );


EXPORT_C_SECDB SLANG_ENUM
		*SlangEnumFirstEnum(		SDB_ENUM_PTR *EnumPtr ),
		*SlangEnumNextEnum(			SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SlangEnumEnumClose(			SDB_ENUM_PTR EnumPtr );


EXPORT_C_SECDB const char
		*SlangModuleNameFromType(	int ModuleType );

EXPORT_C_SECDB SLANG_RET_CODE
		SlangXOptimize( 			SLANG_ARGS ),
		SlangXDebug(				SLANG_ARGS ),
		SlangXDelayLoad(			SLANG_ARGS );

EXPORT_C_SECDB int
		SlangCheckReturnRegister(   char const* FuncName ),	// FuncName *must* be the FuncInfo->Name ptr else it will not work
		SlangHasReturn(				SLANG_NODE *Root );

EXPORT_C_SECDB SLANG_RET_CODE
		SlangExpectNoValue(			SLANG_ARGS );

EXPORT_C_SECDB SLANG_FUNC_PARAM
		*SlangFuncParamGet(			char const* ParamName, DT_SLANG const* SdtSlang );

EXPORT_C_SECDB void
		SlangNodeReassignType(		SLANG_NODE *Root, SLANG_NODE_TYPE NewType ),
		SlangNodeStringReassign(	SLANG_NODE *Node, char *String ); 				// the node will end up owning String

EXPORT_C_SECDB SLANG_NODE
		*SlangNodeApply( 			DT_VALUE *Func, DT_VALUE *Args,	SLANG_ERROR_INFO *ErrorInfo );

EXPORT_C_SECDB SLANG_RET_CODE
		SlangApplyEach(				SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, DT_VALUE **EachArgs );


EXPORT_C_SECDB SLANG_NODE
		*g_CurrentSlangNode;
EXPORT_C_SECDB bool
		g_SecEditEditInProgress;

inline int SlangEvaluate( SLANG_NODE* Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET* Ret, SLANG_SCOPE* Scope )
{
	SLANG_NODE *savedCurrentNode = g_CurrentSlangNode;
	g_CurrentSlangNode = Root;
	int Result = ( *(Root)->FuncPtr )( Root, EvalFlag, Ret, Scope );
	g_CurrentSlangNode = savedCurrentNode;
	return Result;
}



/*
**	Define macros
*/

inline SLANG_VARIABLE* SlangVariableSet( char const* Name, DT_VALUE* Value, SLANG_SCOPE_FUNCTION* HashTable, int Flags )
{
    return SlangVariableSetByBucket( Name, Value, HashTable, Flags, HashStrHash( Name ));
}

inline SLANG_VARIABLE* SlangVariableSetScoped( HASH* VarScope, char const* Name, DT_VALUE* Value, int Flags )
{
    return SlangVariableSetScopedByBucket( VarScope, Name, Value, Flags, HashStrHash( Name ));
}

inline SLANG_VARIABLE* SlangVariableGet( char const* Name, SLANG_SCOPE_FUNCTION* Scope )
{
    return SlangVariableGetByBucket( Name, Scope, HashStrHash( Name ));
}

inline SLANG_VARIABLE* SlangVariableGetScoped( HASH* Scope, char const* Name )
{
    return SlangVariableGetScopedByBucket( Scope, Name, HashStrHash( Name ));
}

#define	SLANG_EVAL_ARG( ArgNum, EvalFlag, ArgRet )																	\
		{																											\
			SLANG_RET_CODE const RetCode = SlangEvaluate( Root->Argv[ (ArgNum) ], (EvalFlag), (ArgRet), Scope );	\
            if( RetCode == SLANG_EXCEPTION )																		\
            {																										\
                *Ret = *ArgRet;																						\
				return RetCode;																						\
            }																										\
            if( RetCode != SLANG_OK )																				\
                return RetCode;																						\
		}

inline void SLANG_RET_FREE( SLANG_RET* Ret )
{
    if( Ret->Type == SLANG_TYPE_VALUE )
	    DTM_FREE( &(Ret->Data.Value) );
	else if( Ret->Type == SLANG_VALUE_TYPE && Ret->Data.Eval.Argv )
	    SlangRetFree( Ret );
}

inline void SLANG_RET_EXCEPT_FREE( SLANG_RET* Ret )
{
    if( Ret->Type == SLANG_TYPE_STATIC_VALUE )
	    DTM_FREE( &(Ret->Data.Value) );
}

#define	SLANG_SET_ERROR( Node )														\
		{																			\
			if( SlangErrorInfo.ModuleName )											\
				free( (char *) SlangErrorInfo.ModuleName );							\
			SlangErrorInfo = (Node)->ErrorInfo;										\
			if( SlangErrorInfo.ModuleName )											\
				SlangErrorInfo.ModuleName = strdup( SlangErrorInfo.ModuleName );	\
		}

#define	SLANG_CHECK_ABORT( Root, Scope )																				\
		( SlangContext->AbortFunc																						\
		&& SlangContext->Aborted																						\
		&& ( SlangContext->Aborted = FALSE, (*SlangContext->AbortFunc)( SlangContext->AbortHandle, (Root), (Scope)) ) )


/*
**	Macro for number of arguments to a Slang function
*/

inline short SLANG_FUNC_ARITY( SLANG_NODE const* Root_ ) { return Root_->Argc - 1;}


/*
**	Position of first slang function argument in SLANG_NODE Argv
*/

inline void          SLANG_NODE_BUCKET_SET( SLANG_NODE* Node, unsigned long Bucket ) { Node->ValueData.NodeInfo.Bucket = Bucket;}
inline unsigned long SLANG_NODE_BUCKET( SLANG_NODE const* Node ) { return Node->ValueData.NodeInfo.Bucket;}

inline void           SLANG_NODE_VALUE_TYPE_SET( SLANG_NODE* Node, SDB_VALUE_TYPE ValueType ) { Node->ValueData.Pointer = ValueType;}
inline SDB_VALUE_TYPE SLANG_NODE_VALUE_TYPE( SLANG_NODE const* Node ) { return SDB_VALUE_TYPE(Node->ValueData.Pointer);}

inline bool SLANG_NODE_IS_CONSTANT( SLANG_NODE const* Node, DT_DATA_TYPE DataType ) { return ( Node->Type == SLANG_TYPE_CONSTANT && Node->ValueDataType == DataType );}
inline void* SLANG_NODE_GET_POINTER( SLANG_NODE const* Node, DT_DATA_TYPE DataType, void* Default ) { return Node->ValueDataType == DataType ? Node->ValueData.Pointer : Default;}
inline double SLANG_NODE_GET_NUMBER( SLANG_NODE const* Node, DT_DATA_TYPE DataType, double Default ) { return Node->ValueDataType == DataType ? Node->ValueData.Number : Default;}
inline void SLANG_NODE_GET_VALUE( SLANG_NODE const* Node, DT_VALUE* Dest ) { Dest->DataType = Node->ValueDataType; Dest->Data.Number = Node->ValueData.Number;}

inline void SLANG_NODE_SET_VALUE( SLANG_NODE* Node, DT_VALUE const* Src )  { Node->ValueDataType = Src->DataType; Node->ValueData.Number = Src->Data.Number;}
inline void SLANG_NODE_SET_NUMBER( SLANG_NODE* Node, DT_DATA_TYPE DataType, double Number ) { Node->ValueDataType = DataType; Node->ValueData.Number = Number;}
inline void SLANG_NODE_SET_POINTER( SLANG_NODE* Node, DT_DATA_TYPE DataType, void* Pointer ) { Node->ValueDataType = DataType; Node->ValueData.Pointer = Pointer;}

inline int  SLANG_NODE_INDEX( SLANG_NODE const* Node )          { return (intptr_t) Node->Argv;} // SW dies here if we use reinterpret cast due to incorrect const parsing
inline void SLANG_NODE_INDEX_SET( SLANG_NODE* Node, int Index ) { Node->Argv = reinterpret_cast<SLANG_NODE**>( Index ); }

inline bool SLANG_NODE_IS_EVAL_ONCE( SLANG_NODE const* Node )  { return (Node->Type == SLANG_TYPE_CONSTANT || Node->Type == SLANG_TYPE_EVAL_ONCE );}

inline void  SLANG_NODE_STRING_VALUE_SET( SLANG_NODE* Node, char* String ) { Node->StringValue = String;}
inline char* SLANG_NODE_STRING_VALUE( SLANG_NODE const* Node )             { return Node->StringValue;}

inline char const* MODULE_NAME_GET( SLANG_ERROR_INFO const& ErrorInfo_ ) { return ErrorInfo_.ModuleName;}
inline short       MODULE_TYPE_GET( SLANG_ERROR_INFO const& ErrorInfo_ ) { return ErrorInfo_.ModuleType;}


/*
**	Macros for argument handling
*/

#define	SLANG_ARG_COUNT					(sizeof(ArgList)/sizeof((ArgList)[0]))

#define	SLANG_ARG_VARS					SLANG_RET	Rets[ SLANG_ARG_COUNT ]

#define	SLANG_USAGE_LINES				static char const* UsageLines[]

#define	SLANG_PTR( ArgNum_ )			( Rets[ (ArgNum_) ].Data.Value.Data.Pointer )
#define	SLANG_STR( ArgNum_ )			( (char *) SLANG_PTR( ArgNum_ ))
#define	SLANG_NUM( ArgNum_ )			( Rets[ (ArgNum_) ].Data.Value.Data.Number )
#define	SLANG_INT( ArgNum_ )			( (int) SLANG_NUM( ArgNum_ ))
#define	SLANG_VAL( ArgNum_ )			( Rets[ (ArgNum_) ].Data.Value )
#define	SLANG_TYPE( ArgNum_ )			( Rets[ (ArgNum_) ].Data.Value.DataType )
#define	SLANG_LVALUE( ArgNum_ )			((DT_VALUE *)( Rets[ (ArgNum_) ].Data.Value.Data.Pointer ))
#define SLANG_BLOCK_RET					( Rets[ SLANG_ARG_COUNT - 1 ] )
#define	SLANG_BLOCK_EVAL( EvalFlag_ )	( SlangEvaluate( Root->Argv[ Root->Argc - 1 ], (EvalFlag_), &SLANG_BLOCK_RET, Scope ))
#define	SLANG_ARG_OMITTED( ArgNum_ )	( SLANG_VAL(ArgNum_).DataType == DtNull )

inline void SLANG_NO_VALUE_RET( SLANG_RET* Ret )
{
    Ret->Type = SLANG_TYPE_NO_VALUE;
}

inline void SLANG_NULL_RET( SLANG_RET* Ret)
{
    memset( Ret, 0, sizeof(*Ret) );
	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	DTM_INIT( &(Ret->Data.Value) );
}

#define	SLANG_ARG_PARSE()																						\
		{																										\
			int const RetCode = SlangArgParse( Root, EvalFlag, Ret, Scope, ArgList, SLANG_ARG_COUNT, Rets );	\
			if( RetCode != SLANG_OK )																			\
				return RetCode & ~SLANG_RC_ARG_PARSE_RET;														\
		}

// same as SLANG_ARG_PARSE but includes UsageLines in usage output.
//
#define	SLANG_ARG_PARSE_AND_USAGE()																				\
		{																										\
			int const RetCode = SlangArgParse( Root, EvalFlag, Ret, Scope, ArgList, SLANG_ARG_COUNT, Rets );	\
			if( RetCode != SLANG_OK )																			\
			{																									\
				if( Root->Argc == SLANG_ARGC_USAGE || ErrNum == SDB_ERR_SLANG_ARG )								\
				{																								\
					ErrAppend( "\n//" );																		\
					for( int Index = 0; UsageLines[ Index ] != NULL; ++Index )									\
					{																							\
						ErrAppend( "\n// %s", UsageLines[ Index ] );											\
					}																							\
				}																								\
				return RetCode;																					\
			}																									\
		}

// SLANG_ARG_PARSE_AND_USAGE_BLOCK() takes a code block as an argument, which can
// ErrAppend() additional text to become part of the usage message.
//
#define	SLANG_ARG_PARSE_AND_USAGE_BLOCK( Block_ )																\
		{																										\
			int const RetCode = SlangArgParse( Root, EvalFlag, Ret, Scope, ArgList, SLANG_ARG_COUNT, Rets );	\
			if( RetCode != SLANG_OK )																			\
			{																									\
				if( Root->Argc == SLANG_ARGC_USAGE || ErrNum == SDB_ERR_SLANG_ARG )								\
				{																								\
					Block_;																						\
				}																								\
				return RetCode;																					\
			}																									\
		}

#define	SLANG_ARG_FREE()													\
		{																	\
			for( unsigned ArgNum = 0; ArgNum < SLANG_ARG_COUNT; ++ArgNum )	\
				SLANG_RET_FREE( &Rets[ ArgNum ] );							\
		}

#define	SLANG_ARG_NONE()																			\
		{																							\
			if( Root->Argc != 0 )																	\
				return SlangEvalError( SLANG_ERROR_PARMS, "%s: No arguments", Root->StringValue );	\
		}


inline void SlangPrint( char const* Str )
{
	SLANG_PRINT_FUNCTION
			PrintFunc = (SLANG_PRINT_FUNCTION) SlangContextGet( "Print Function" );

	if( PrintFunc )
		PrintFunc( SlangContextGet( "Print Handle" ), Str );
	else
		puts( Str );
}

inline bool SLANG_VARIABLE_SCOPE::RunUninitVarHook( char const* VarName, char const* ScopeName, SLANG_NODE *Root, SLANG_SCOPE *Scope, DT_VALUE *Value )
{
	DT_VALUE
			Arg0,
			Arg1;

	if( !UninitializedVarHook )
		return false;

	if( !SlangScopePushLocals( Scope, "Uninitialized Var Hook", UninitializedVarHook, &Root->ErrorInfo ))
        return FALSE;
	Arg1.DataType = Arg0.DataType = DtString;
	Arg1.Data.Pointer = VarName;
	Arg0.Data.Pointer = ScopeName;
	SlangVariableSetLocal( Scope->FunctionStack, 0, &Arg0, SLANG_VARIABLE_RETAIN );
	SlangVariableSetLocal( Scope->FunctionStack, 1, &Arg1, SLANG_VARIABLE_RETAIN );

	SLANG_RET Ret;
	SLANG_RET_CODE RetCode = SlangEvaluate( UninitializedVarHook->Expression->Argv[ UninitializedVarHook->Expression->Argc - 1 ], SLANG_EVAL_RVALUE, &Ret, Scope );
	if( RetCode == SLANG_OK || RetCode == SLANG_RETURN )
	{
		RetCode = SLANG_OK;
		if( SLANG_TYPE_STATIC_VALUE == Ret.Type )
			DTM_ASSIGN( Value, &Ret.Data.Value );
		else if( SLANG_TYPE_VALUE == Ret.Type )
			*Value = Ret.Data.Value;
		else
		{
			SLANG_RET_FREE( &Ret );
			RetCode = SLANG_ERROR;
		}
	}
	else if (SLANG_EXCEPTION == RetCode)
	{
		// so that we dont leek the exception. This code seemed to be executing a hook for uninit vars
		// internal tp slang.
		// ErrMore("Slang Exception occured \n %s \n", SecDbExceptionToCStr(Ret));
		SLANG_RET_EXCEPT_FREE( &Ret );
	}
	SlangScopePopFunction( Scope );
	return RetCode == SLANG_OK;
}

inline bool SLANG_SCOPE_FUNCTION::RunUninitVarHook( SLANG_NODE *Root, SLANG_SCOPE *Scope, DT_VALUE *Value )
{
	DT_VALUE
			Arg0,
			Arg1;

	if( !UninitializedVarHook )
		return false;

	if( !SlangScopePushLocals( Scope, "Uninitialized Var Hook", UninitializedVarHook, &Root->ErrorInfo ))
        return FALSE;

	DTM_INIT( &Arg0 );
	Arg1.DataType = DtString;
	Arg1.Data.Pointer = SLANG_NODE_STRING_VALUE( Root );
	SlangVariableSetLocal( Scope->FunctionStack, 0, &Arg0, SLANG_VARIABLE_RETAIN );
	SlangVariableSetLocal( Scope->FunctionStack, 1, &Arg1, SLANG_VARIABLE_RETAIN );

	SLANG_RET Ret;
	SLANG_RET_CODE RetCode = SlangEvaluate( UninitializedVarHook->Expression->Argv[ UninitializedVarHook->Expression->Argc - 1 ], SLANG_EVAL_RVALUE, &Ret, Scope );
	if( RetCode == SLANG_OK || RetCode == SLANG_RETURN )
	{
		RetCode = SLANG_OK;
		if( SLANG_TYPE_STATIC_VALUE == Ret.Type )
			DTM_ASSIGN( Value, &Ret.Data.Value );
		else if( SLANG_TYPE_VALUE == Ret.Type )
			*Value = Ret.Data.Value;
		else
		{
			SLANG_RET_FREE( &Ret );
			RetCode = SLANG_ERROR;
		}
	}
	SlangScopePopFunction( Scope );
	return RetCode == SLANG_OK;
}


/*
** Hacky cplusplus versions of SLANG_XXX stuff. Ideally it should all be c++
*/

/*
** A SlangRet structure has a default constructor for NO_VALUE, and does a SLANG_RET_FREE in the destructor
*/

struct SLANG_RET_CC : public SLANG_RET
{
	SLANG_RET_CC()
	{
		Type = SLANG_TYPE_NO_VALUE;
	}
	~SLANG_RET_CC()
	{
		SLANG_RET_FREE( this );
	}

private:
	// make these private and undefined to prevent copying of this
	SLANG_RET_CC(SLANG_RET_CC const&);
    SLANG_RET_CC& operator=(SLANG_RET_CC const&);
};

// Now we can define Rets for the arg vars as SLANG_RET_CC, so no need to do SLANG_ARG_FREE anymore

#define	SLANG_ARG_VARS_CC			SLANG_RET_CC	Rets[ SLANG_ARG_COUNT ]

// stack of secdb slang scope. you can query the top.
//
// the idiom to push one is:
//
// {
//     SecDb_Slang_Scope guard( Scope, Root->ErrorInfo /* 0 if you do not know */ );
//
//     call into secdb
//
// }
//
// You can also do:
//
// DT_VALUE* res = SecDb_Slang_Scope( Scope, Root->ErrorInfo /* 0 if you do not know */ ), SecDbGetValueWithArgs( ... );
//
// note that given the broken temporary lifetime semantics of SW and VC6 you might wish to enclose the
// above statement in a block in order to guarantee correct stacking of the scopes (even though it is a stack
// so it is not terribly important)
//
// UFO uses the top scope to call a value function

struct EXPORT_CLASS_SECDB SecDb_Slang_Scope
{
    SecDb_Slang_Scope( SLANG_SCOPE* scope, SLANG_ERROR_INFO* caller_loc )
	{
		m_scopes.push( scope );
		if( scope->FunctionStack && caller_loc )
			scope->FunctionStack->ErrorInfo = caller_loc;
	}
    ~SecDb_Slang_Scope()
	{
		m_scopes.pop();
	}

    static SLANG_SCOPE* top()
	{
		return m_scopes.empty() ? 0 : m_scopes.top();
	}

private:
	static CC_STL_STACK( SLANG_SCOPE* ) m_scopes;
};

// hack to allow us to specialize addin args which need a DT_SLANG*
class dt_slang_addin_arg
{
public:
    dt_slang_addin_arg( DT_VALUE* val ) : m_value( val ) {}

	DT_VALUE* m_value;
};

#endif

