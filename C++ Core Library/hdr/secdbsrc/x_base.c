#define GSCVSID "$Header: /home/cvs/src/secdb/src/x_base.c,v 1.73 2001/11/27 23:23:47 procmon Exp $"
/****************************************************************
**
**	X_BASE.C	- Slang base types and related functions
**
**	$Log: x_base.c,v $
**	Revision 1.73  2001/11/27 23:23:47  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.68  2001/09/10 22:21:07  singhki
**	better error handling for ds->array through SecDbDiddleScopeToArray
**	
**	Revision 1.67  2001/07/24 21:21:40  shahia
**	fix mem leak from SlanxVariable
**	
**	Revision 1.66  2001/06/29 15:22:17  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.65  2001/05/03 00:58:09  singhki
**	only initialize Ret if success
**	
**	Revision 1.64  2001/04/04 19:50:17  singhki
**	added SecDbClearLRUCache for debugging
**	
**	Revision 1.63  2001/03/26 19:10:45  shahia
**	init slang returns to avoid core dump
**	
**	Revision 1.62  2001/03/22 14:40:32  singhki
**	oops, missed one initialize of Ret->Type
**	
**	Revision 1.61  2001/03/21 15:50:07  singhki
**	do not initialize ret struct unless we return properly
**	
**	Revision 1.60  2001/03/13 23:43:56  simpsk
**	Cleanups: scope variables, use guardians to manage SLANG_RET's.
**	
**	Revision 1.59  2001/01/16 19:17:31  schlae
**	Reset FuncPtr to SlangXDelayLoad if the load fails.
**	
**	Revision 1.58  2000/12/05 10:46:32  shahia
**	Implementing Exceptions in slang
**
**	Revision 1.57  2000/11/02 11:38:56  shahia
**	Backout last re, checked in by mistake
**
**	Revision 1.55  2000/06/21 15:24:08  singhki
**	Cache funcinfo for DelayLoad so we don't need to look it up again
**
**	Revision 1.54  2000/06/19 16:19:37  singhki
**	Delay loading of dll implementing SlangX funcs until the function
**	actually needs to be executed
**
**	Revision 1.53  2000/05/01 21:35:27  vengrd
**	Hack to improve speed of pointer array subscripting on NT where ftol is dog slow
**
**	Revision 1.52  2000/04/24 11:04:29  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**
**	Revision 1.51  2000/04/07 23:09:50  singhki
**	Added SecDbStreamByNameDefault to control SecPtr streaming by name
**
**	Revision 1.50  2000/04/07 17:29:39  vengrd
**	Skunk interest rate merge/checkin
**
**	Revision 1.49  2000/02/08 04:50:29  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**
**	Revision 1.48  1999/12/30 13:28:04  singhki
**	Add UninitializedVarHook
**
**	Revision 1.47  1999/11/01 22:41:55  singhki
**	death to all link warnings
**
**	Revision 1.46  1999/10/22 21:45:39  singhki
**	Do not access Node->Value directly but use member functions
**
**	Revision 1.45  1999/10/12 16:10:51  singhki
**	use new better HashStats API. Add SecDbCacheStatistics
**
**	Revision 1.44  1999/10/07 22:48:45  gribbg
**	Add Multi-dimension subscripting
**
**	Revision 1.43  1999/09/01 15:29:37  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**
**	Revision 1.42  1999/06/21 03:03:40  gribbg
**	Type casts for new C++
**
**	Revision 1.41  1999/04/20 14:29:56  singhki
**	Move the meat of ApplyDiddleState into DiddleScopeFromArray
**
**	Revision 1.40  1999/04/20 02:14:05  singhki
**	Added unstream from array to diddle state.
**
**	Revision 1.39  1999/03/26 14:45:42  singhki
**	Better node statistics
**
**	Revision 1.38  1999/03/12 22:14:37  singhki
**	Added SlangXSecDbNodeStatistics
**
**	Revision 1.37  1999/03/03 16:43:11  singhki
**	Slang Functions now use a local variable stack and their variables
**	are represented as an index into this stack.
**	Scoped variable nodes in the parse tree are replaced by references to
**	the variable itself.
**	Diddle Scopes inside a Slang Scope are now allocated on demand.
**
**	Revision 1.36  1999/02/23 04:34:03  lundek
**	New value type syntax obj.valuetype
**
**	Revision 1.35  1999/01/26 17:31:47  singhki
**	Rename DiddleList to DiddleListSecdb in order for safe prodver
**
**	Revision 1.34  1998/12/07 23:22:40  singhki
**	Add flag to DiddleList() to get args of diddled nodes
**
**	Revision 1.33  1998/11/16 23:00:07  singhki
**	DiddleColors: Merge from branch
**
**	Revision 1.32.4.2  1998/11/05 02:53:22  singhki
**	factored c++ stuff into internal header files
**
**	Revision 1.32.4.1  1998/11/05 01:01:27  singhki
**	TempRev: hacks to support OS/2 IBM compiler in C++ mode
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<malloc.h>
#include	<date.h>
#include	<secdb.h>
#include	<mempool.h>
#include	<hash.h>
#include	<secexpr.h>
#include	<secerror.h>
#include	<sdb_x.h>
#include	<dtptr.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secdt.h>
#include	<sdb_int.h>
#include	<secdbcc.h>

EXPORT_C_SECDB SLANG_RET_CODE
		SlangXDiddleListSecDb( SLANG_ARGS ),
		SlangXSecDbNodeStatistics( SLANG_ARGS ),
		SlangXSecDbApplyDiddleState( SLANG_ARGS ),
		SlangXSecDbCacheStatistics( SLANG_ARGS ),
		SlangXSecDbStreamByNameDefault( SLANG_ARGS ),
		SlangXSecDbClearLRUCache( SLANG_ARGS ),
		SlangXSecDbDiddleScopeToArray( SLANG_ARGS );


/****************************************************************
**	Routine: SlangXConstantNode
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Return a constant
****************************************************************/

SLANG_RET_CODE SlangXConstantNode( SLANG_ARGS )
{
	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	SLANG_NODE_GET_VALUE( Root, &Ret->Data.Value );

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXNull
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : converts node to a SLANG_TYPE_CONSTANT containing a null
****************************************************************/

SLANG_RET_CODE SlangXNull( SLANG_ARGS )
{
	SLANG_ARG_NONE();

	if( EvalFlag == SLANG_EVAL_PARSE_TIME )
	{
		SlangNodeStringReassign( Root, NULL );

		Root->Type = SLANG_TYPE_CONSTANT;
		// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
		Root->FuncPtr = (SLANG_FUNC) SlangXConstantNode;
		Root->ValueDataType = DtNull;
		Root->ValueData.Number = 0;
	}
	else
	{
		Ret->Type = SLANG_TYPE_STATIC_VALUE;
		DTM_INIT( &Ret->Data.Value );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXDebug
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Invoke the debugger function found in the context
****************************************************************/

SLANG_RET_CODE SlangXDebug( SLANG_ARGS )
{
	static SLANG_DEBUG_FUNC
			DebugFunction;

	static void
			*DebugHandle;

	static long
			UpdateCount;


	if( Root->Argc != 0 )
		return SlangEvalError( SLANG_ERROR_PARMS,
							   "Debug: No arguments.\n"
							   "       Set Slang breakpoint." );

	if( UpdateCount != SlangContext->UpdateCount )
	{
		UpdateCount		= SlangContext->UpdateCount;
		DebugFunction	= (SLANG_DEBUG_FUNC) SlangContextGet( "Debug Function" );
		DebugHandle		= SlangContextGet( "Debug Handle" );
	}

	SLANG_NULL_RET( Ret );

	if( DebugFunction )
	{
		Scope->FunctionStack->ErrorInfo = &Root->ErrorInfo;
		return( (*DebugFunction)( DebugHandle, Root, Scope ));
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXVariable
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Return a pointer to a variable, or the value of a
**			 variable depending upon the eval flag
****************************************************************/

SLANG_RET_CODE SlangXVariable( SLANG_ARGS )
{
	SLANG_VARIABLE
			*Variable;

	DT_VALUE
			TmpValue;

	int		RetVal;

	SLANG_NULL_RET(Ret);


	if( Root->Type == SLANG_TYPE_LOCAL_VARIABLE )
	{
		if( Scope->FunctionStack && Scope->FunctionStack->Locals )
		{
			if( !( Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root )]))
			{
				if( Scope->FunctionStack->RunUninitVarHook( Root, Scope, &TmpValue ))
				{
					Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root )] =
							SlangVariableAllocLocal( Scope->FunctionStack->SlangFunc->LocalNames[ SLANG_NODE_BUCKET( Root )], 0 );
					SlangVariableSetValue( Variable, &TmpValue, SLANG_VARIABLE_NO_COPY );
				}
				else if( SLANG_EVAL_LVALUE_CREATE == EvalFlag || SLANG_EVAL_LVALUE_NEWREF == EvalFlag )
				{
					if( !( Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root )] =
						   SlangVariableAllocLocal( Scope->FunctionStack->SlangFunc->LocalNames[ SLANG_NODE_BUCKET( Root )],
													SLANG_EVAL_LVALUE_NEWREF == EvalFlag ? SLANG_VARIABLE_REFERENCE : 0 )))
						return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
					SlangVariableIncrRef( Variable );

					Ret->Type = SLANG_TYPE_VALUE;
					DtPointerVariable( &Ret->Data.Value, Variable );
					return SLANG_OK;
				}
				else
				{
					return SlangEvalError( SLANG_ERROR_PARMS, "Access of uninitialized variable '%s'", SLANG_NODE_STRING_VALUE( Root ));
				}
			}
		}
		else
		{
			if( !( Variable = SlangVariableGet( SLANG_NODE_STRING_VALUE( Root ), Scope->FunctionStack )))
			{
				if( Scope->FunctionStack->RunUninitVarHook( Root, Scope, &TmpValue ))
				{
					if( !( Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root ), &TmpValue, Scope->FunctionStack,
																SLANG_VARIABLE_NO_COPY, HashStrHash( SLANG_NODE_STRING_VALUE( Root )))))
						return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
				}
				else if( SLANG_EVAL_LVALUE_CREATE == EvalFlag || SLANG_EVAL_LVALUE_NEWREF == EvalFlag )
				{
					if( !( Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root ), NULL, Scope->FunctionStack,
																SLANG_EVAL_LVALUE_NEWREF == EvalFlag ? SLANG_VARIABLE_REFERENCE : 0,
																HashStrHash( SLANG_NODE_STRING_VALUE( Root )))))
						return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );

					Ret->Type = SLANG_TYPE_VALUE;
					DtPointerVariable( &Ret->Data.Value, Variable );
					return SLANG_OK;
				}
				else
				    return SlangEvalError( SLANG_ERROR_PARMS, "Access of uninitialized variable '%s'", SLANG_NODE_STRING_VALUE( Root ));
			}
		}
	}
	else if( !(	Variable = SlangVariableGetByBucket( SLANG_NODE_STRING_VALUE( Root ), Scope->FunctionStack, SLANG_NODE_BUCKET( Root ))) && Scope->FunctionStack->RunUninitVarHook( Root, Scope, &TmpValue ))
		Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root ), &TmpValue, Scope->FunctionStack,
											 SLANG_VARIABLE_NO_COPY, SLANG_NODE_BUCKET( Root ));

	if( SLANG_EVAL_RVALUE == EvalFlag )
	{
		if( !Variable || ( Variable->Flags & SLANG_VARIABLE_DESTROYED ))
			return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s'", Root->StringValue );
		if( Variable->Flags & SLANG_VARIABLE_DEBUG_READ )
		{
			RetVal = SlangXDebug( Root, EvalFlag, Ret, Scope );
			if( RetVal != SLANG_OK )
				return( RetVal );
		}
		if( Variable->Flags & SLANG_VARIABLE_REFERENCE )
		{
			if( DT_OP( DT_MSG_DEREFERENCE, &TmpValue, Variable->Value, NULL ))
			{
				Ret->Type = SLANG_TYPE_STATIC_VALUE;
				Ret->Data.Value = *(DT_VALUE *)TmpValue.Data.Pointer;
				return( SLANG_OK );
			}

			if( DT_OP( DT_MSG_DEREFERENCE_VALUE, &Ret->Data.Value, Variable->Value, NULL ))
			{
				Ret->Type = SLANG_TYPE_VALUE;
				return ( SLANG_OK );
			}
			else
				return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
		}
		else
		{
			Ret->Type = SLANG_TYPE_STATIC_VALUE;
			Ret->Data.Value = *(Variable->Value);
		}
	}
	else
	{
		if( !Variable || (Variable->Flags & SLANG_VARIABLE_DESTROYED ))
		{
			if( SLANG_EVAL_LVALUE_CREATE == EvalFlag || SLANG_EVAL_LVALUE_NEWREF == EvalFlag )
			{
				if( Variable )
					SlangVariableSetValue( Variable, NULL, SLANG_EVAL_LVALUE_NEWREF == EvalFlag ? SLANG_VARIABLE_REFERENCE : 0 );
				else
				{
					Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root ), NULL, Scope->FunctionStack, SLANG_EVAL_LVALUE_NEWREF == EvalFlag ? SLANG_VARIABLE_REFERENCE : 0, SLANG_NODE_BUCKET( Root ));
					if( !Variable )
					{
						// ErrMore( "Unable to create variable" );
						return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
					}
				}
			}
			else
			    return SlangEvalError( SLANG_ERROR_PARMS, "Access of uninitialized variable '%s'", SLANG_NODE_STRING_VALUE( Root ));
		}
		else
		{
			if( Variable->Flags & SLANG_VARIABLE_DEBUG_WRITE )
			{
				RetVal = SlangXDebug( Root, EvalFlag, Ret, Scope );
				if( RetVal != SLANG_OK )
					return( RetVal );
			}
			if( Variable->Flags & SLANG_VARIABLE_REFERENCE )
			{
				if( !Variable->Value || Variable->Value->DataType != DtPointer )
					return SlangEvalError( Root, EvalFlag, Ret, Scope, "Trying to dereference a non-pointer '%s'", Root->StringValue );
				if( SLANG_EVAL_LVALUE_NEWREF == EvalFlag )
				{
					Err( SDB_ERR_OBJECT_IN_USE, "'%s' variable is in use", Variable->Name );
					return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
				}
				Ret->Type = SLANG_TYPE_STATIC_VALUE;
				Ret->Data.Value = *(Variable->Value);
				return( SLANG_OK );
			}
			else if( SLANG_EVAL_LVALUE_NEWREF == EvalFlag )
				Variable->Flags |= SLANG_VARIABLE_REFERENCE;
		}

		Ret->Type = SLANG_TYPE_VALUE;
		DtPointerVariable( &Ret->Data.Value, Variable );
	}
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXSubscript
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Handle array subscripting
****************************************************************/

SLANG_RET_CODE SlangXSubscript( SLANG_ARGS )
{
	SLANG_RET_CC
			TmpRet1,
			TmpRets[ SLANG_SUBSCRIPT_MAX_ARGS ];

	DT_VALUE
			*ParentValue,
			TmpValue,
			*IndexValues[ SLANG_SUBSCRIPT_MAX_ARGS ],
			*SubValue;

	DT_POINTER_ARRAY
			PtrArray;


/*
**	Get the arguments (var,index)
*/

	SLANG_EVAL_ARG( 0, SLANG_EVAL_LVALUE, &TmpRet1 );
	{
	    for( int ArgNum = 1; ArgNum < Root->Argc; ++ArgNum )
		{
		    int const RetCode = SlangEvaluate( Root->Argv[ ArgNum ], SLANG_EVAL_RVALUE, &TmpRets[ ArgNum - 1], Scope );
			if ( RetCode == SLANG_EXCEPTION )
				*Ret = TmpRets[ArgNum];  // return the exception value

			if( RetCode != SLANG_OK )
				return RetCode;
		}
	}


/*
**	Evaluate (or not...)
*/

	switch( TmpRet1.Type )
	{
		case SLANG_TYPE_VALUE:
		case SLANG_TYPE_STATIC_VALUE:
			ParentValue = &TmpRet1.Data.Value;
			break;

		case SLANG_VALUE_TYPE:
			SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, &TmpRet1 );
			ParentValue = &TmpRet1.Data.Value;
			EvalFlag	= SLANG_EVAL_RVALUE;
			break;

		default:
			Err( ERR_INVALID_ARGUMENTS, "Can't subscript left value" );
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
	}

	bool WantRealValue;

	switch( EvalFlag )
	{
		case SLANG_EVAL_RVALUE:
		case SLANG_EVAL_NO_VALUE:
			WantRealValue = true;
			break;

		default:
			WantRealValue = ParentValue->DataType != DtPointer;
			break;
	}

	if( Root->Argc == 2 )
		SubValue = &(TmpRets[0].Data.Value);
	else
	{
		SubValue = &TmpValue;
		TmpValue.DataType = DtPointerArray;
		TmpValue.Data.Pointer = &PtrArray;
		PtrArray.Size = Root->Argc-1;
		PtrArray.Element = IndexValues;
		for( int ArgNum = 1; ArgNum < Root->Argc; ++ArgNum )
			IndexValues[ ArgNum-1 ] = &TmpRets[ ArgNum-1 ].Data.Value;
	}
	
	bool const Success = ( WantRealValue
						   ? DT_OP( DT_MSG_SUBSCRIPT_VALUE, &Ret->Data.Value, ParentValue, SubValue )
						   : DtPointerSubscript( &Ret->Data.Value, ParentValue, SubValue ) );

	if( !Success )
	{
		DT_VALUE
				StrVal;

		StrVal.DataType = DtString;
		ErrPush();
		bool const GotString = DTM_TO( &StrVal, SubValue );
		ErrPop();
		if( GotString )
		{
			ErrMore( "[%s]", (char*) DT_VALUE_TO_POINTER( &StrVal ));
			DTM_FREE( &StrVal );
		}
		else
			ErrMore( "[Can't convert to string]" );
		return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
	}

	Ret->Type = SLANG_TYPE_VALUE;
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXStar
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Handle array subscripting
****************************************************************/

SLANG_RET_CODE SlangXStar( SLANG_ARGS )
{
	DT_VALUE
			TmpValue;


	SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, Ret );

	if( Ret->Data.Value.DataType != DtPointer )
	{
		SLANG_RET_FREE( Ret );
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "Cannot dereference a non-pointer" );
	}

	if( SLANG_EVAL_RVALUE == EvalFlag || SLANG_EVAL_NO_VALUE == EvalFlag )
	{
		if( !DT_OP( DT_MSG_DEREFERENCE_VALUE, &TmpValue, &Ret->Data.Value, NULL ))
		{
			SLANG_RET_FREE( Ret );
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
		}
		SLANG_RET_FREE( Ret );
		Ret->Type = SLANG_TYPE_VALUE;
		Ret->Data.Value = TmpValue;
	}
	return( SLANG_OK );
}



/****************************************************************
**	Routine: SlangXVariableIndex
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for Variable[ n ]
****************************************************************/

SLANG_RET_CODE SlangXVariableIndex( SLANG_ARGS )
{
	SLANG_VARIABLE
			*Variable;

	DT_VALUE
			*Value,
			Subscript,
			Ptr;

	char	*VarName = (char *) Root->ValueData.Pointer;

	SLANG_RET_CODE
			RetCode = SLANG_OK;


	if( Scope->FunctionStack->Locals )
	{
		Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root )];
		if( !Variable && Scope->FunctionStack->RunUninitVarHook( Root, Scope, &Ptr ))
		{
			Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root )] = SlangVariableAllocLocal( Scope->FunctionStack->SlangFunc->LocalNames[ SLANG_NODE_BUCKET( Root )], 0 );
			if( Variable )
				SlangVariableSetValue( Variable, &Ptr, SLANG_VARIABLE_NO_COPY );
		}
	}
	else
	{
		Variable = SlangVariableGet( VarName, Scope->FunctionStack );
		if( !Variable && Scope->FunctionStack->RunUninitVarHook( Root, Scope, &Ptr ))
			Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root ), &Ptr, Scope->FunctionStack,
												 SLANG_VARIABLE_NO_COPY, SLANG_NODE_BUCKET( Root ));
	}
	if( !Variable )
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of uninitialized variable '%s'", VarName);

	if( SLANG_EVAL_RVALUE == EvalFlag )
	{
		// Sleazy hack to pass an int in the Pointer part of a
		// DtError.  The reason for this is that NT takes
		// absofukinlutely forever to cast a double back to an int.

		DT_VALUE
				ErrIndex,
				Element;

		DT_POINTER_TO_VALUE( &ErrIndex, (void *)SLANG_NODE_INDEX( Root ), DtError );

		if( DT_OP( DT_MSG_SUBSCRIPT, &Element, Variable->Value, &ErrIndex ) && ( Value = (DT_VALUE*) Element.Data.Pointer ) )
		{
			Ret->Type 		= SLANG_TYPE_STATIC_VALUE;
			Ret->Data.Value = *Value;
		}
		else
		{
			DT_NUMBER_TO_VALUE( &Subscript, SLANG_NODE_INDEX( Root ), DtDouble );
			if( DtSubscriptGetCopy( Variable->Value, &Subscript, &Ret->Data.Value ))
				Ret->Type = SLANG_TYPE_STATIC_VALUE;
			else
				return SlangEvalError( SLANG_ERROR_PARMS, "Unable to evaluate subscript %s[ %d ]", VarName, SLANG_NODE_INDEX( Root ));
		}
	}
	else if( SLANG_EVAL_LVALUE == EvalFlag || SLANG_EVAL_LVALUE_CREATE == EvalFlag )
	{
		Ret->Type = SLANG_TYPE_VALUE;

		// Same sleaze as above.

		DT_POINTER_TO_VALUE( &Subscript, (void *)SLANG_NODE_INDEX( Root ), DtError );
		DTM_INIT( &Ptr );
		if( !DtPointerVariable( &Ptr, Variable ) || !DtPointerSubscript( &Ret->Data.Value, &Ptr, &Subscript ))
			RetCode = SLANG_ERROR;
		DTM_FREE( &Ptr );
	}

	return RetCode;
}



/****************************************************************
**	Routine: SlangXEvalOnce
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for EvalOnce
****************************************************************/

SLANG_RET_CODE SlangXEvalOnce( SLANG_ARGS )
{
	DT_VALUE
			TmpValue;

	int		RetCode;


	if( EvalFlag == SLANG_EVAL_PARSE_TIME )
	{
		if( Root->Argc != 1 )
			return SLANG_ERROR;

		Root->Type = SLANG_TYPE_EVAL_ONCE;

		SlangNodeStringReassign( Root, NULL );
		Root->ValueDataType = NULL;		// force eval time evaluation to evaluate it

		return SLANG_OK;
	}

	if( Root->ValueDataType != NULL )
	{
		// We have already evaluated this once, just return the cached value

		Ret->Type = SLANG_TYPE_STATIC_VALUE;
		SLANG_NODE_GET_VALUE( Root, &Ret->Data.Value );
		return SLANG_OK;
	}

	// Never been evaluated, evaluate as an RVALUE, cache the value and return it
	if( (RetCode = SlangEvaluate( Root->Argv[ 0 ], SLANG_EVAL_RVALUE, Ret, Scope )) != SLANG_OK )
	{
		if( RetCode != SLANG_EXCEPTION && RetCode != SLANG_ERROR && RetCode != SLANG_ABORT )
			return SlangEvalError( SLANG_ERROR_PARMS, "EvalOnce: Expression threw unexpected signal '%d'", RetCode );
		return RetCode;
	}

	SlangNodeReassignType( Root, SLANG_TYPE_EVAL_ONCE );

	if( Ret->Type == SLANG_TYPE_VALUE )
	{
		SLANG_NODE_SET_VALUE( Root, &Ret->Data.Value );
		Ret->Type = SLANG_TYPE_STATIC_VALUE;
	}
	else
	{
		DTM_ASSIGN( &TmpValue, &Ret->Data.Value );
		SLANG_NODE_SET_VALUE( Root, &TmpValue );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXDiddleListSecDb
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action :
****************************************************************/

SLANG_RET_CODE SlangXDiddleListSecDb( SLANG_ARGS )
{
	static SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Security",	&DtSecurity,	"Security with diddles; NULL for all", SLANG_ARG_NULLS | SLANG_ARG_OPTIONAL		},
				{ "IncludeArgs",&DtDouble,		"If args should be included",		   SLANG_ARG_OPTIONAL },
			};

	SLANG_ARG_VARS_CC;

	SDB_OBJECT
			*SecPtr;

	bool	IncludeArgs = false;


	SLANG_ARG_PARSE();
	if( SLANG_ARG_OMITTED(0) )
		SecPtr = NULL;
	else
		SecPtr = (SDB_OBJECT *) SLANG_PTR(0);

	if( !SLANG_ARG_OMITTED( 1 ))
		IncludeArgs = (bool) SLANG_INT( 1 );

	Ret->Type = SLANG_TYPE_VALUE;
	if( !SDB_NODE::DiddleList( &Ret->Data.Value, SecPtr, IncludeArgs ))
		DTM_INIT( &Ret->Data.Value );

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXSecDbNodeStatistics
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for SecDbNodeStatistics
****************************************************************/

SLANG_RET_CODE SlangXSecDbNodeStatistics( SLANG_ARGS )
{
	if( Root->Argc != 0 )
		return SlangEvalError( SLANG_ERROR_PARMS, "%s(): Returns node statistics. Takes no args", SLANG_NODE_STRING_VALUE( Root ));

	Ret->Type = SLANG_TYPE_VALUE;

	if( !SDB_NODE::Statistics( &Ret->Data.Value ))
		DTM_INIT( &Ret->Data.Value );

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXSecDbCacheStatistics
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Print Cache statistics for a Db
****************************************************************/

SLANG_RET_CODE SlangXSecDbCacheStatistics( SLANG_ARGS )
{
	DT_VALUE
			*Value,
			Stats;

	SDB_DB *Db = DbToPhysicalDb( SecDbRootDb );
	if( !Db )
		return SlangEvalError( SLANG_ERROR_PARMS, "Error: %s is not a physical Db", SecDbRootDb ? SecDbRootDb->FullDbName.c_str() : "(null)" );

	Ret->Type = SLANG_TYPE_VALUE;
	Value = &Ret->Data.Value;

	DTM_ALLOC( Value, DtStructure );

	DtHashStatsToStruct( SecDbRootDb->CacheHash, &Stats );
	DtComponentReplacePointer( Value, "Cache", DtStructure, Stats.Data.Pointer );

	DtHashStatsToStruct( SecDbRootDb->PreCacheHash, &Stats );
	DtComponentReplacePointer( Value, "PreCache", DtStructure, Stats.Data.Pointer );

	DtHashStatsToStruct( SecDbRootDb->PageTableHash, &Stats );
	DtComponentReplacePointer( Value, "PageTable", DtStructure, Stats.Data.Pointer );

	return SLANG_OK;
}



// FIX- This should be taken out of the slang wrapper
/****************************************************************
**	Routine: SlangXSecDbApplyDiddleState
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for SecDbApplyDiddleState
****************************************************************/

SLANG_RET_CODE SlangXSecDbApplyDiddleState( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "DiddleState",&DtArray,	"Diddle State" }
			};

	SLANG_ARG_VARS;


	SLANG_ARG_PARSE();

	if( !DiddleScopeFromArray( (DT_ARRAY *) SLANG_VAL( 0 ).Data.Pointer, Scope ))
	{
		SLANG_ARG_FREE();
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	}
	SLANG_ARG_FREE();

	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	Ret->Data.Value.DataType	= DtDouble;
	Ret->Data.Value.Data.Number = 1;

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXSecDbStreamByNameDefault
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for SecDbStreamByNameDefault
****************************************************************/

SLANG_RET_CODE SlangXSecDbStreamByNameDefault( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "StreamByName",&DtDouble,	"Whether SecPtrs should be streamed by Name or by Instream", SLANG_ARG_OPTIONAL }
			};

	SLANG_ARG_VARS_CC;


	SLANG_ARG_PARSE();

	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	Ret->Data.Value.DataType = DtDouble;
	Ret->Data.Value.Data.Number = SecDbStreamByNameDefault;

	if( !SLANG_ARG_OMITTED( 0 ))
		SecDbStreamByNameDefault = SLANG_INT( 0 );

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXDelayLoad
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Used to delay loading dlls until the slang func is
**		     actually called
****************************************************************/

SLANG_RET_CODE SlangXDelayLoad( SLANG_ARGS )
{
	const char
			*Name = SLANG_NODE_STRING_VALUE( Root );

	SLANG_FUNCTION_INFO
			*Info = (SLANG_FUNCTION_INFO *) Root->ValueData.Pointer;

	if( !Info )
		return SlangEvalError( SLANG_ERROR_PARMS, "DelayLoad: Function info for '%s' has mysteriously disappeared", Name );

	if( !( Root->FuncPtr = SlangFunctionBind( Info ) ))
	{
		ErrMore( "DelayLoad: Cannot bind SlangX function to implementation" );
		Root->FuncPtr = SlangXDelayLoad;
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	}
	return SlangEvaluate( Root, EvalFlag, Ret, Scope );
}



/****************************************************************
**	Routine: SlangXSecDbClearLRUCache
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for SecDbClearLRUCache
****************************************************************/

SLANG_RET_CODE SlangXSecDbClearLRUCache( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Security",	NULL, "Security for which we should destroy unparented nodes" },
			};

	SLANG_ARG_VARS_CC;


	SLANG_ARG_PARSE();

	SdbObjectPtr sec;

	if( SLANG_TYPE(0) == DtString )
		sec = SecDbGetByName( SLANG_STR(0), SecDbRootDb, 0 );
	else if( SLANG_TYPE(0) == DtSecurity )
		sec = (SDB_OBJECT*) SLANG_PTR(0);
	else
		Err( ERR_INVALID_ARGUMENTS, "First arg must be security or a string" );

	if( sec.val() == 0 )
	{
		ErrMore( "arg is not a valid security" );
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	}

	int num_destroyed = 0;
	int token = 0;
	SDB_NODE::Destroy( NULL, &token );
	while( sec->NumCachedNodes > 0 )
	{
		SDB_NODE* head = sec->NodeCache;
		head->LRUCacheByObjectDelete();
		SDB_NODE::Destroy( head, &token );
		++num_destroyed;
	}
	SDB_NODE::Destroy( NULL, &token );

	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value.DataType	= DtDouble;
	Ret->Data.Value.Data.Number = num_destroyed;

	return SLANG_OK;
}


/****************************************************************
**	Routine: SlangXSecDbDiddleScopeToArray
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for SecDbDiddleScopeToArray
****************************************************************/

SLANG_RET_CODE SlangXSecDbDiddleScopeToArray( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Diddle Scope",	&DtDiddleScope,	"Diddle scope which is to be converted to an array" }
			};

	SLANG_ARG_VARS_CC;

	SLANG_ARG_PARSE();

	DT_ARRAY* res = SecDbDiddleScopeToArray( (DT_DIDDLE_SCOPE*) SLANG_PTR( 0 ) );
	if( !res )
	{
		ErrMore( "Diddle Scope to Array failed!" );
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	}
	Ret->Type = SLANG_TYPE_VALUE;
	Ret->Data.Value.DataType = DtArray;
	Ret->Data.Value.Data.Pointer = res;
	return SLANG_OK;
}
