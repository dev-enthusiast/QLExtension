#define GSCVSID "$Header: /home/cvs/src/secdb/src/x_inc.c,v 1.47 2001/11/27 23:23:48 procmon Exp $"
/****************************************************************
**
**	X_INC.C		- Slang include function
**
**	$Log: x_inc.c,v $
**	Revision 1.47  2001/11/27 23:23:48  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.45  2001/06/27 21:50:45  singhki
**	little error msg change for KMW
**	
**	Revision 1.44  2001/01/25 13:34:48  shahia
**	Do ErrMore before slang exception is deleted
**	
**	Revision 1.43  2000/12/05 10:46:33  shahia
**	Implementing Exceptions in slang
**	
**	Revision 1.42  2000/07/03 14:08:50  goodfj
**	Use SecDbSourceDbGein Link()
**	
**	Revision 1.41  2000/06/07 14:05:29  goodfj
**	Fix includes
**	
**	Revision 1.40  1999/12/30 13:28:04  singhki
**	Add UninitializedVarHook
**	
**	Revision 1.39  1999/10/28 17:55:28  singhki
**	don't do getbyname is script is already smartlinked
**	
**	Revision 1.38  1999/10/28 17:49:38  singhki
**	Allow any kind of constant as first arg
**	
**	Revision 1.37  1999/10/22 16:15:06  singhki
**	Support constants of any type, not just doubles
**	
**	Revision 1.36  1999/09/01 15:29:37  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.35  1999/08/07 01:40:17  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.34  1999/07/30 21:15:37  singhki
**	Undo SmartLinkEnable if parsing or evaluation fails
**	
**	Revision 1.33  1999/03/26 14:37:57  singhki
**	Use SlangVariableGetScoped instead of explicit HashLookup
**	
**	Revision 1.32  1999/03/03 16:43:12  singhki
**	Slang Functions now use a local variable stack and their variables
**	are represented as an index into this stack.
**	Scoped variable nodes in the parse tree are replaced by references to
**	the variable itself.
**	Diddle Scopes inside a Slang Scope are now allocated on demand.
**	
**	Revision 1.31  1999/02/15 12:10:20  schlae
**	SlangXLinkLog()
**	
****************************************************************/

#define BUILDING_SECDB
#define INCL_FILEIO
#include	<portable.h>
#include	<hash.h>
#include	<secexpr.h>
#include	<err.h>
#include	<sdb_x.h>
#include	<secdb/sdb_exception.h>

// scope used by Link and SmartLinkEnable (not obsolete SmartLink)
#define SMART_LINK_SCOPE	"SmartLinkEnable"

// hook used to give profiler a crack at parse trees before throwing them away
SLANG_ATTACH_PROFILER
		SlangAttachProfiler = NULL;



/****************************************************************
**	Routine: SlangXLink
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Link another slang script at parse time
**
**  Strategy: In the absence of the SmartLink flag, the script is
**  parsed, and the resulting block is attached at the location of the
**	link command.
**
**  If script has the SmartLink flag set, then it is
**  evaluated at parse time the first time it is seen
**  (a flag in the SmartLink scope is set to prevent subsequent
**  evaluations).  At run time, such a link command simply
**  returns null (it is represented in the parse tree as an empty
**  block).
**
****************************************************************/

int SlangXLink( SLANG_ARGS )
{
	SDB_OBJECT
			*SecPtr;

	SLANG_NODE
			*Expression;

	DT_VALUE
			*ScriptVal,
			TmpValue;

	char	*ScriptExpression,
			*ScriptName,
			*UnqualifiedName;

	SLANG_VARIABLE_SCOPE
			*VarScope;

	SLANG_VARIABLE
			*VarSmartLink;

	SLANG_RET
			TmpRet1;

	int
			RetCode;

	SDB_DB	*SourceDb;



	if( SLANG_EVAL_PARSE_TIME != EvalFlag )
	{
		/*
		**	Run time
		*/
		if( Root->Argc == 2 )
		{
			SLANG_EVAL_ARG( 1, SLANG_EVAL_RVALUE, Ret );
		}
		else
		{
			// SmartLink case
			SLANG_NULL_RET( Ret );
		}
		return SLANG_OK;
	}

	if( Root->Argc != 1 )
	{
		Err( ERR_UNKNOWN, "%s: ( ScriptName )", SLANG_NODE_STRING_VALUE( Root ));
		return SLANG_ERROR;
	}
	if( !( ScriptName = (char *) SLANG_NODE_GET_POINTER( Root->Argv[ 0 ], DtString, NULL )))
	{
		Err( ERR_UNKNOWN, "Argument must be a string constant" );
		return SLANG_ERROR;
	}

	VarScope = SlangVariableScopeGet( SMART_LINK_SCOPE, TRUE );
	if( !VarScope )
	{
		ErrMore( "Link( %s )", ScriptName );
		return SLANG_ERROR;
	}

	// If we've already smartlinked, do nothing.
	if( !( UnqualifiedName = strrchr( ScriptName, SDB_SECNAME_DELIMITER )))
		UnqualifiedName = ScriptName;
	else
		++UnqualifiedName;

	if(   ( VarSmartLink = SlangVariableGetScoped( VarScope->VariableHash, UnqualifiedName ))
	   && VarSmartLink->Value && VarSmartLink->Value->Data.Number == 1 )
		return SLANG_OK;

	if( !(SourceDb = SecDbSourceDbGet()))
	{
		ErrMore( "Link( %s ): Failed to get source db", ScriptName );
		return SLANG_ERROR;
	}

	if( !(SecPtr = SecDbGetByName( ScriptName, SourceDb, 0 )))
	{
		ErrMore( "Link( %s )", ScriptName );
		return SLANG_ERROR;
	}

	// Get and parse the expression.
	ScriptVal = SecDbGetValue( SecPtr, SecDbValueTypeFromName( "Expression", NULL ));
	if( ScriptVal )
		ScriptExpression = (char *) ScriptVal->Data.Pointer;
	else
	{
		SecDbFree( SecPtr );
		ErrMore( "Link( %s )", Root->Argv[ 0 ]->StringValue );
		return SLANG_ERROR;
	}

	Expression = SlangParseAttach( Root, NULL, ScriptExpression, ScriptName, SLANG_MODULE_OBJECT );

	if( !Expression )
	{
		ErrMore( "Link( %s ) Parsing failed @(%d,%d) in '%s'",
				ScriptName,
				SlangErrorInfo.BeginningLine,
				SlangErrorInfo.BeginningColumn,
				SlangErrorInfo.ModuleName );
		RetCode = SLANG_ERROR;
		goto ErrorReturn;
	}

	if( SlangVariableGetScoped( VarScope->VariableHash, UnqualifiedName ))
	{
		/*
		** SmartLinkEnable was parsed.  We evaluate immediately now.
		*/

		/*
		** We use the PARSE_TIME scope to evaluate this
		** script.  Except in weird circumstances (e.g. inside
		** an Exec), this should be the global scope.
		** Scripts with the SmartLink bit set typically only
		** place things in named scopes anyway.  We throw away
		** the return result of the link.
		** If profiling is enabled, attach profiler first.
		*/

		int		FreeParseTree = SlangAttachProfiler ? !SlangAttachProfiler( Expression, TRUE ) : TRUE;
		RetCode = SlangEvaluate( Expression, SLANG_EVAL_RVALUE, &TmpRet1, Scope );
		if(SLANG_EXCEPTION == RetCode)
		{
			*Ret = TmpRet1;
		}
		else 
		{
			SLANG_RET_FREE( &TmpRet1 );
		}

		if( RetCode != SLANG_OK )
		{
			ErrMore( "Link( %s )", ScriptName );
			goto ErrorReturn;
		}

		/*
		** Script is attached, but we don't want to execute it at
		** run time.  So we decrement Argc, and free its parse tree
		*/
		--Root->Argc;
		if( FreeParseTree )
			SlangFree( Root->Argv[ Root->Argc ]);
	}

	SecDbFree( SecPtr );
	return SLANG_OK;

  ErrorReturn:
	// First undo any smart link enable
	if( SlangVariableGetScoped( VarScope->VariableHash, UnqualifiedName ))
	{
		TmpValue.DataType		= DtDouble;
		TmpValue.Data.Number	= 0;
		SlangVariableSetScoped( VarScope->VariableHash, UnqualifiedName, &TmpValue, FALSE );
	}
	SecDbFree( SecPtr );
	return RetCode;
}

/****************************************************************
**	Routine: SlangXSmartLinkEnable
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Mark a smart link script as smart linkable, by putting
**			 an entry in the SmartLink scope at parse time.
**
****************************************************************/

int SlangXSmartLinkEnable( SLANG_ARGS )
{
	DT_VALUE
			TmpValue;

	SLANG_VARIABLE_SCOPE
			*VarScope;

	if( Root->Argc != 0 )
	{
		Err( ERR_UNKNOWN, "%s()\n"
			"    Mark containing script as SmartLinkable",
			Root->StringValue );
		return SLANG_ERROR;
	}

	if( SLANG_EVAL_PARSE_TIME != EvalFlag )
	{
		/*
		**	Run time
		*/
		SLANG_NULL_RET( Ret );
		return SLANG_OK;
	}

	VarScope = SlangVariableScopeGet( SMART_LINK_SCOPE, TRUE );
	if( !VarScope )
	{
		ErrMore( "SmartLinkEnable()" );
		return SLANG_ERROR;
	}

	DTM_INIT( &TmpValue );
	TmpValue.DataType	 = DtDouble;
	TmpValue.Data.Number = 1;
	/*
	** This seems like the best place to get the name
	** of the script I am linking...
	*/
	SlangVariableSetScoped( VarScope->VariableHash, Root->ErrorInfo.ModuleName, &TmpValue, FALSE );
	return SLANG_OK;
}

/****************************************************************
**	Routine: SlangXLinkFile
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Link another slang file at parse time
****************************************************************/

int SlangXLinkFile( SLANG_ARGS )
{
	SLANG_NODE
			*Expression;

	char	*FileName,		// Passed in file name
			*Buffer;		// Constructed file name (allocated)


/*
**	Parse time
*/

	if( SLANG_EVAL_PARSE_TIME == EvalFlag )
	{
		if( Root->Argc != 1 )
		{
			Err( ERR_UNKNOWN,
					"LinkFile: ( FileName )\n"
					"    FileName is found along CONFIG_PATH" );
			return SLANG_ERROR;
		}
		if( !( FileName = (char *) SLANG_NODE_GET_POINTER( Root->Argv[ 0 ], DtString, NULL )))
		{
			Err( ERR_UNKNOWN, "Argument must be a string constant" );
			return SLANG_ERROR;
		}

		// Look for the file in SecDbPath (if the file isn't found)
		if( !ERR_CALLOC( Buffer, char, 1, 1 + FILENAME_MAX ))
			return SLANG_ERROR;
		if( !KASearchPath( "", FileName, Buffer, FILENAME_MAX )
				&& !KASearchPath( SecDbPath, FileName, Buffer, FILENAME_MAX ))
		{
			Err( ERR_FILE_NOT_FOUND, "LinkFile( %s ) Can't find '%s' or '%s'", FileName, Buffer, FileName );
			free( Buffer );
			return SLANG_ERROR;
		}

		// Parse the expression
		Expression = SlangParseAttach(
				Root,
				Buffer,
				NULL,
				FileName,
				SLANG_MODULE_OBJECT );
		if( !Expression )
		{
			ErrMore( "LinkFile( %s ) Parsing failed @(%d,%d) in '%s'",
					Buffer,
					SlangErrorInfo.BeginningLine,
					SlangErrorInfo.BeginningColumn,
					SlangErrorInfo.ModuleName );
			free( Buffer );
			return SLANG_ERROR;
		}

		free( Buffer );
		return SLANG_OK;
	}


/*
**	Run time
*/

	SLANG_EVAL_ARG( 1, SLANG_EVAL_RVALUE, Ret );
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXConstant
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Define a constant at parse time
****************************************************************/

int SlangXConstant( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1;
	SLANG_RET_CODE RetCode = SLANG_OK;


/*
**	Parse time
*/

	if( SLANG_EVAL_PARSE_TIME == EvalFlag )
	{
		if( Root->Argc != 2 )
		{
			Err( ERR_UNKNOWN, "%s: ( Name, Value )", Root->StringValue );
			return SLANG_ERROR;
		}

		// First arg must be a variable or a named constant
		if( !SLANG_NODE_STRING_VALUE( Root->Argv[ 0 ] )
			|| ( SLANG_TYPE_VARIABLE != Root->Argv[ 0 ]->Type
				 && Root->Argv[ 0 ]->Type != SLANG_TYPE_CONSTANT ))
		{
			Err( ERR_UNKNOWN, "Constant: Arg 0 (Name) must be a variable/named constant" );
			return SLANG_ERROR;
		}

		// Evaluate second arg, value of constant
		if( SLANG_OK != (RetCode = SlangEvaluate( Root->Argv[ 1 ], SLANG_EVAL_RVALUE, &TmpRet1, Scope )))
		{
			// At Parse time We cannot deal with exceptions
			if (SLANG_EXCEPTION == RetCode)
			{
				Err(ERR_UNKNOWN, "Constant: Arg 1 results in exception \n %s \n", SecDbExceptionToCStr(TmpRet1));
				SLANG_RET_EXCEPT_FREE(&TmpRet1);	
				return SLANG_ERROR;
			}
			else
			{
				ErrMore( "Constant: Arg 1 cannot evaluate" );
				return SLANG_ERROR;
			}
		}
		if( TmpRet1.Type == SLANG_TYPE_STATIC_VALUE )
		{
			DT_VALUE
					TmpValue;

			DTM_ASSIGN( &TmpValue, &TmpRet1.Data.Value );
			SlangRegisterConstantValue( Root->Argv[ 0 ]->StringValue, &TmpValue );
		}
		else if( TmpRet1.Type == SLANG_TYPE_VALUE )
			SlangRegisterConstantValue( Root->Argv[ 0 ]->StringValue, &TmpRet1.Data.Value );
		else
		{
			Err( ERR_UNKNOWN, "Constant: Arg 1 isn't a value" );
			return SLANG_ERROR;
		}
		// Note: No need to free TmpRet1 because we have given the constant's value away

		return SLANG_OK;
	}


/*
**	Run time
*/

	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	DTM_INIT( &Ret->Data.Value );
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXEnum
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Define enumerated constants at parse time
****************************************************************/

int SlangXEnum( SLANG_ARGS )
{
/*
**	Parse time
*/

	if( SLANG_EVAL_PARSE_TIME == EvalFlag )
	{
		int		ArgNum;


		// Check the argument count
		if( Root->Argc < 1 )
		{
			Err( ERR_UNKNOWN, "%s: ( Const1 [, ... ConstN ] )", Root->StringValue );
			return SLANG_ERROR;
		}

		// Point to the block
		Root = Root->Argv[ 0 ];

		// Go through each constant in the block
		for( ArgNum = 0; ArgNum < Root->Argc; ArgNum++ )
		{
			if( SLANG_NODE_STRING_VALUE( Root->Argv[ ArgNum ])
					&& (SLANG_TYPE_VARIABLE == Root->Argv[ ArgNum ]->Type
							|| SLANG_NODE_IS_CONSTANT( Root->Argv[ ArgNum ], DtString )
							|| SLANG_NODE_IS_CONSTANT( Root->Argv[ ArgNum ], DtDouble )))
			{
				SlangRegisterConstant( SLANG_NODE_STRING_VALUE( Root->Argv[ ArgNum ]), (double) ArgNum );
			}
			else
			{
				Err( ERR_UNKNOWN, "Define: Arg %d must be a string constant", ArgNum );
				return SLANG_ERROR;
			}
		}

		return SLANG_OK;
	}


/*
**	Run time
*/

	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	DTM_INIT( &Ret->Data.Value );
	return SLANG_OK;
}

/****************************************************************
**	Routine: SlangXLinkLog
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Returns a structure showing all scripts linked so far.
****************************************************************/

int SlangXLinkLog( SLANG_ARGS )
{
	extern DT_VALUE SlangLinkLog;

	SLANG_ARG_NONE();

	Ret->Type = SLANG_TYPE_VALUE;
	DTM_ASSIGN( &Ret->Data.Value, &SlangLinkLog );

	return SLANG_OK;
}

