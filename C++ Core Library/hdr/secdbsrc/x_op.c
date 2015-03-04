#define GSCVSID "$Header: /home/cvs/src/secdb/src/x_op.c,v 1.127 2013/10/08 21:08:06 khodod Exp $"
/****************************************************************
**
**	X_OP.C 	- Expression evaluator operators
**
**	$Log: x_op.c,v $
**	Revision 1.127  2013/10/08 21:08:06  khodod
**	Make sure to set SLANG_RET value to a valid value only on success.
**	AWR: #315795
**
**	Revision 1.126  2011/10/20 15:45:43  khodod
**	Slight clean-up of the code to allow implicit casts of numeric types
**	to String/GsDt.
**	AWR: #247083
**
**	Revision 1.125  2011/10/04 21:46:56  c07426
**	Allows "string OP double" to succeed, by casting
**	double to string.(Uncommented code)
**	AWR #: 247083
**
**	Revision 1.123  2010/08/20 13:54:53  e19351
**	Optimized DtString assignment to preserve buffer during block printing.
**
**	Revision 1.122  2001/11/27 23:23:48  procmon
**	Reverted to CPS_SPLIT.
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<hash.h>
#include	<secexpr.h>
#include	<secloop.h>
#include	<sdb_x.h>
#include	<dtptr.h>
#include	<secerror.h>
#include	<err.h>
#include	<secdb/sdb_exception.h>
#include    <secdb/LtSecDbObj.h>
#include    <gsdt/GsDtString.h>
#include    <gsdt/GsDtGeneric.h>
#include    <gsdt/dttypes.h>
#include    <gsdt/GsDtToDtValue.h>
#include    <gsdt/x_gsdt.h>
#include    <gsdt/Enums.h>

CC_USING_NAMESPACE( Gs );

static int ValueMethod(
	SLANG_NODE		*Root,
	SLANG_EVAL_FLAG	EvalFlag,
	SLANG_RET		*Ret,
	SLANG_SCOPE		*Scope,
	SDB_VALUE_TYPE	ValueMethod
);

EXPORT_C_SECDB int SlangXVoidBlock( SLANG_ARGS );
EXPORT_C_SECDB int SlangXDebugBreak( SLANG_ARGS );


/****************************************************************
**	Routine: OpAssignWithGsDtConversion
**	Returns: 
**	Action : 
****************************************************************/

static int OpAssignWithGsDtConversion(
	int OperatorMsg,
	DT_VALUE *Value,
	DT_VALUE *RhsIn	
)
{
	int 	Success = FALSE;

	DT_VALUE
			*Rhs = RhsIn;

	DT_VALUE
			ConvertedRhs;

	DTM_INIT( &ConvertedRhs );

	if( ( Value->DataType != RhsIn->DataType ) && ( DtGsDt == RhsIn->DataType ) )
	{
		// Convert the GsDt if we will get the same type as Value has.
		if( Value->DataType == GsDtEquivDtValue( *(const GsDt *)DT_VALUE_TO_POINTER( RhsIn ) ) )
		{
			GsDtToDtValue( *(const GsDt *)DT_VALUE_TO_POINTER( RhsIn ), &ConvertedRhs, GSDT_TO_DTV_FAIL_WITH_DTGSDT );

			Rhs = &ConvertedRhs;
		}		
	}

	Success = DT_OP( OperatorMsg, NULL, Value, Rhs );

	DTM_FREE( &ConvertedRhs );
	
	return Success;
}


/****************************************************************
**	Routine: SlangXAssignVar
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Assignment operators
****************************************************************/

int SlangXAssignVar( SLANG_ARGS )
{
	SLANG_RET
			TmpRet2;

	DT_VALUE
			TmpValue1,
			TmpValue2;

	int		Success = FALSE;

	SLANG_RET_CODE
			RetCode;

	SLANG_VARIABLE
			*Variable;

	if( Root->Argv[ 0 ]->Type == SLANG_TYPE_LOCAL_VARIABLE )
	{
		if( Scope->FunctionStack && Scope->FunctionStack->Locals )
		{
			if( !( Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root->Argv[0] )]))
			{
				if( Scope->FunctionStack->RunUninitVarHook( Root->Argv[ 0 ], Scope, &TmpValue1 ))
				{
					Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root->Argv[ 0 ] )] =
							SlangVariableAllocLocal( Scope->FunctionStack->SlangFunc->LocalNames[ SLANG_NODE_BUCKET( Root->Argv[0] )], 0 );
					SlangVariableSetValue( Variable, &TmpValue1, SLANG_VARIABLE_NO_COPY );
				}
				else
				{
					if( !( Variable = Scope->FunctionStack->Locals[ SLANG_NODE_BUCKET( Root->Argv[0] )] =
						   SlangVariableAllocLocal( Scope->FunctionStack->SlangFunc->LocalNames[ SLANG_NODE_BUCKET( Root->Argv[0] )], 0 )))
						return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
					SlangVariableIncrRef( Variable );
				}
			}
		}
		else
		{
			if( !( Variable = SlangVariableGet( SLANG_NODE_STRING_VALUE( Root->Argv[0] ), Scope->FunctionStack )))
			{
				if( Scope->FunctionStack->RunUninitVarHook( Root->Argv[0], Scope, &TmpValue1 ))
				{
					if( !( Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root->Argv[0] ), &TmpValue1, Scope->FunctionStack,
																SLANG_VARIABLE_NO_COPY, HashStrHash( SLANG_NODE_STRING_VALUE( Root->Argv[0] )))))
						return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
				}
				else
				{
					if( !( Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root->Argv[0] ), NULL, Scope->FunctionStack,
																0,
																HashStrHash( SLANG_NODE_STRING_VALUE( Root->Argv[0] )))))
						return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
				}
			}
		}
	}
	else if( Root->Argv[0]->Type != SLANG_TYPE_VARIABLE )
		return SlangEvalError( Root->Argv[0], EvalFlag, Ret, Scope, "Uh oh: LHS of assignment is no longer a variable" );
	else if( !(	Variable = SlangVariableGetByBucket( SLANG_NODE_STRING_VALUE( Root->Argv[0] ), Scope->FunctionStack, SLANG_NODE_BUCKET( Root->Argv[0] ))) && Scope->FunctionStack->RunUninitVarHook( Root->Argv[0], Scope, &TmpValue1 ))
		Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root->Argv[0] ), &TmpValue1, Scope->FunctionStack,
											 SLANG_VARIABLE_NO_COPY, SLANG_NODE_BUCKET( Root->Argv[0] ));

	if( !Variable && !( Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root->Argv[0] ), NULL, Scope->FunctionStack, 0, SLANG_NODE_BUCKET( Root->Argv[0] ))))
	{
		ErrMore( "Error creating variable for assignment" );
		return SlangEvalError( Root->Argv[0], EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
	}

	if( Variable->Flags & SLANG_VARIABLE_DESTROYED )
		SlangVariableSetValue( Variable, NULL, 0 );

	// Evaluate the Right Hand Side to get a real value

	SLANG_NULL_RET( &TmpRet2 );	// NULL out value in case some badly behaved function doesn't do TRT
	if( SLANG_OK != ( RetCode = SlangEvaluate( Root->Argv[ 1 ], SLANG_EVAL_RVALUE, &TmpRet2, Scope )))
	{
		if( SLANG_EXIT == RetCode  || SLANG_EXCEPTION == RetCode )
		{
			Ret->Type		= TmpRet2.Type;
			Ret->Data.Value	= TmpRet2.Data.Value;
		}
		else
			*Ret = TmpRet2;
		return RetCode;
	}

	switch( Root->Type )
	{
		case SLANG_ASSIGN			:
			if( Variable->Flags & SLANG_VARIABLE_PROTECT )
				Success = Err( SDB_ERR_OBJECT_IN_USE, "'%s' variable is in use", Variable->Name );
			else if( Variable->Flags & SLANG_VARIABLE_REFERENCE )
			{
				if( TmpRet2.Type == SLANG_TYPE_STATIC_VALUE )
				{
					Success = DTM_ASSIGN( &TmpValue1, &TmpRet2.Data.Value ) && DtPointerAssign( Variable->Value, &TmpValue1 );
				}
				else
				{
					TmpRet2.Type = SLANG_TYPE_STATIC_VALUE; // steal value
					Success = DtPointerAssign( Variable->Value, &TmpRet2.Data.Value );
				}
			}
			else
			{
				if( TmpRet2.Type == SLANG_TYPE_STATIC_VALUE )
				{
					if( !DTM_ASSIGN( &TmpValue1, &TmpRet2.Data.Value ) )
					{
						Success = FALSE;
						break;
					}
				}
				else
				{
					TmpRet2.Type = SLANG_TYPE_STATIC_VALUE; // steal value
					TmpValue1 = TmpRet2.Data.Value;
					DTM_INIT( &TmpRet2.Data.Value );
				}

				// A special case optimization (logically equivalent) for PrintToObject.
				// This can be removed, it will just prevent block printing from buffering
				// output if the string is assigned to inside the block.
				if( Variable->Value->DataType == DtString && TmpValue1.DataType == DtString ) {
					TmpValue2 = *(Variable->Value);

					Success = DTM_ASSIGN( Variable->Value, &TmpValue1 );
					if( Success )
					{
						// Free old data from a different address, preserving the buffer
						DTM_FREE( &TmpValue2 );
						DTM_INIT( &TmpValue2 );
					} 
					else 
						*(Variable->Value) = TmpValue2;

					DTM_FREE( &TmpValue1 );
					DTM_INIT( &TmpValue1 );
				}
				else
					DTM_REPLACE( Variable->Value, &TmpValue1, Success );
			}
			break;

		case SLANG_ADD_ASSIGN		:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_ADD,      Variable->Value, &TmpRet2.Data.Value ); break;
		case SLANG_SUBTRACT_ASSIGN	:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_SUBTRACT, Variable->Value, &TmpRet2.Data.Value ); break;
		case SLANG_MULTIPLY_ASSIGN	:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_MULTIPLY, Variable->Value, &TmpRet2.Data.Value ); break;
		case SLANG_DIVIDE_ASSIGN	:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_DIVIDE,   Variable->Value, &TmpRet2.Data.Value ); break;
		case SLANG_AND_ASSIGN		:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_AND,      Variable->Value, &TmpRet2.Data.Value ); break;
	}

	// No longer need TmpRet2
	SLANG_RET_FREE( &TmpRet2 );
	if( !Success )
		goto EXIT;


/*
**	Deal with return values.  If EvalFlag is SLANG_EVAL_NO_VALUE, don't need
**	to return a value.
*/

	RetCode = SLANG_OK;
	if( EvalFlag == SLANG_EVAL_NO_VALUE )
	{
		SLANG_NO_VALUE_RET( Ret );
	}
	else
	{
		if( !( Variable->Flags & SLANG_VARIABLE_REFERENCE ))
		{
			Ret->Type = SLANG_TYPE_STATIC_VALUE;
			// FIX- This makes no lifetime guarantee, it's a miracle it works.
			// but I'm just copying what Assign used to do. Can't make it slower :-)
			Ret->Data.Value = *Variable->Value;
		}
		else
		{
			if( DT_OP( DT_MSG_DEREFERENCE, &TmpValue1, Variable->Value, NULL ))
			{
				Ret->Type = SLANG_TYPE_STATIC_VALUE;
				Ret->Data.Value = *(DT_VALUE *)TmpValue1.Data.Pointer;
			}
			// Otherwise try to make a copy
			else if( DT_OP( DT_MSG_DEREFERENCE_VALUE, &Ret->Data.Value, Variable->Value, NULL ))
				Ret->Type = SLANG_TYPE_VALUE;

			// Otherwise the datatype is refusing to cough up what it just set, so assume NULL
			else
				SLANG_NULL_RET( Ret );
		}
	}

EXIT:
	if( !Success )
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	return RetCode;
}



/****************************************************************
**	Routine: SlangXAssign
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Assignment operators
****************************************************************/

int SlangXAssign( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1,
			TmpRet2;

	DT_VALUE
			*TmpValue,
			TmpValue1,
			TmpValue2,
			*Value2,
			*Value;

	SDB_OBJECT
			*SecPtr = NULL;

	int		Success = FALSE,
			FreeValue = FALSE;

	SLANG_RET_CODE
			RetCode;

	if( EvalFlag == SLANG_EVAL_PARSE_TIME )
	{
		if( Root->Argv[0]->Type == SLANG_TYPE_VARIABLE )
			Root->FuncPtr = SlangXAssignVar;
		return SLANG_OK;
	}

/*
**	Evaluate the Left Hand Side to get a Value Type or
**	a pointer (possibly creating a variable as needed)
*/
	DTM_INIT( &TmpRet1.Data.Value );
	SLANG_EVAL_ARG( 0, SLANG_ASSIGN == Root->Type ? SLANG_EVAL_LVALUE_CREATE : SLANG_EVAL_LVALUE, &TmpRet1 );


/*
**	Evaluate the Right Hand Side to get a real value
*/

	DTM_INIT( &TmpRet2.Data.Value );
	if( SLANG_OK != ( RetCode = SlangEvaluate( Root->Argv[ 1 ], SLANG_EVAL_RVALUE, &TmpRet2, Scope )))
	{
		SLANG_RET_FREE( &TmpRet1 );
		if( SLANG_EXIT == RetCode || SLANG_EXCEPTION == RetCode )
		{
			Ret->Type		= TmpRet2.Type;
			Ret->Data.Value	= TmpRet2.Data.Value;
		}
		else
			*Ret = TmpRet2;
		return RetCode;
	}


/*
**	Think about the LHS to figure out what to do
**	At the end of this section value points to DT_VALUE
**	containing a DtPointer to the target or a copy of
**	the value from SecDb.  In either case we own
**	Value locally (so must be freed when done).
**	We also still own TmpRet1 and TmpRet2.
*/

	if(( TmpRet1.Type == SLANG_TYPE_VALUE || TmpRet1.Type == SLANG_TYPE_STATIC_VALUE ) && TmpRet1.Data.Value.DataType == DtPointer )
	{
		// Steal Value from TmpRet1, so make TmpRet1 empty
		FreeValue		= TmpRet1.Type == SLANG_TYPE_VALUE;
		Value			= &TmpRet1.Data.Value;
		TmpRet1.Type	= SLANG_TYPE_STATIC_VALUE;
	}
	else if( TmpRet1.Type == SLANG_VALUE_TYPE )
	{
		// Get copy of current value from SecDb
		if( !(SecPtr = SecDbGetByName( TmpRet1.Data.Eval.SecName, SecDbRootDb, 0 )))
		{
			SLANG_RET_FREE( &TmpRet1 );
			SLANG_RET_FREE( &TmpRet2 );
			return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
		}

		if( Root->Type != SLANG_ASSIGN )
		{
			SecDb_Slang_Scope guard( Scope, &Root->ErrorInfo );

			if( !( TmpValue = SecDbGetValueWithArgs( SecPtr, TmpRet1.Data.Eval.ValueType, TmpRet1.Data.Eval.Argc, TmpRet1.Data.Eval.Argv, NULL )))
			{
				SLANG_RET_FREE( &TmpRet1 );
				SLANG_RET_FREE( &TmpRet2 );
				return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
			}
			DTM_ASSIGN( &TmpValue1, TmpValue );
		}
		else
			DTM_INIT( &TmpValue1 );

		FreeValue = TRUE;
		Value = &TmpValue1;
	}
	else
	{
		SLANG_RET_FREE( &TmpRet1 );
		SLANG_RET_FREE( &TmpRet2 );
		return SlangEvalError( SLANG_ERROR_PARMS, "Assignment: First argument can't be assigned to" );
	}


/*
**	Perform the operation (plain assignment, or something like +=)
**	Do not change ownership of Value, TmpRet1, or TmpRet2.
*/

	switch( Root->Type )
	{
		case SLANG_ASSIGN			:
			if( TmpRet1.Type == SLANG_VALUE_TYPE )
			{
				DTM_FREE( Value );
				Success = DTM_ASSIGN( Value, &TmpRet2.Data.Value );
			}
			else
			{
				Success = TRUE;
				if( TmpRet2.Type == SLANG_TYPE_STATIC_VALUE )
				{
					Success = DTM_ASSIGN( &TmpValue2, &TmpRet2.Data.Value );
					Value2 = &TmpValue2;
				}
				else
				{
					// Steal Value2 from TmpRet2, so make TmpRet2 empty
					Value2 = &TmpRet2.Data.Value;
					TmpRet2.Type = SLANG_TYPE_STATIC_VALUE;
				}

				// Value2 is always freed, even if DtPointerAssign fails
				Success = Success && DtPointerAssign( Value, Value2 );
			}
			break;

		case SLANG_ADD_ASSIGN		:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_ADD,      Value, &TmpRet2.Data.Value ); break;
		case SLANG_SUBTRACT_ASSIGN	:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_SUBTRACT, Value, &TmpRet2.Data.Value ); break;
		case SLANG_MULTIPLY_ASSIGN	:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_MULTIPLY, Value, &TmpRet2.Data.Value ); break;
		case SLANG_DIVIDE_ASSIGN	:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_DIVIDE,   Value, &TmpRet2.Data.Value ); break;
		case SLANG_AND_ASSIGN		:   Success = OpAssignWithGsDtConversion( DT_MSG_ASSIGN_AND,      Value, &TmpRet2.Data.Value ); break;

	}
	// No longer need TmpRet2
	SLANG_RET_FREE( &TmpRet2 );
	if( !Success )
		goto EXIT;


/*
**	Deal with return values.  If EvalFlag is SLANG_EVAL_NO_VALUE, don't need
**	to return a value.
*/

	RetCode = SLANG_OK;
	if( TmpRet1.Type == SLANG_VALUE_TYPE )
	{
		if( !SlangScopeSetDiddle( Scope, SecPtr, TmpRet1.Data.Eval.ValueType, TmpRet1.Data.Eval.Argc, TmpRet1.Data.Eval.Argv, Value))
		{
			Success = FALSE;
			goto EXIT;
		}

		if( EvalFlag == SLANG_EVAL_NO_VALUE )
		{
			SLANG_NO_VALUE_RET( Ret );
		}
		else
		{
			// Assume that Value must be identical to what we would now get from a SecDbGetValue
			Ret->Type = SLANG_TYPE_VALUE;
			Ret->Data.Value = *Value;
			DTM_INIT( Value );
		}
	}
	else	// TmpRet1 is a pointer
		if( EvalFlag == SLANG_EVAL_NO_VALUE )
		{
			SLANG_NO_VALUE_RET( Ret );
		}
		else
		{
			// Try to get a static pointer (no guarantees about lifetime unfortunately)
			if( DT_OP( DT_MSG_DEREFERENCE, &TmpValue1, Value, NULL ))
			{
				Ret->Type = SLANG_TYPE_STATIC_VALUE;
				Ret->Data.Value = *(DT_VALUE *)TmpValue1.Data.Pointer;
			}
			// Otherwise try to make a copy
			else if( DT_OP( DT_MSG_DEREFERENCE_VALUE, &Ret->Data.Value, Value, NULL ))
				Ret->Type = SLANG_TYPE_VALUE;

			// Otherwise the datatype is refusing to cough up what it just set, so assume NULL
			else
				SLANG_NULL_RET( Ret );
		}


EXIT:
	SecDbFree( SecPtr );
	if( FreeValue )
		DTM_FREE( Value );
	SLANG_RET_FREE( &TmpRet1 );
	if( !Success && SLANG_EXCEPTION != RetCode)
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	return RetCode;
}



/****************************************************************
**	Routine: SlangDtOperator
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Helper for SlangXOperator
****************************************************************/

static int SlangDtOperator(
	int RootType,
	SLANG_RET *Ret,
	DT_VALUE	*TmpDt1,
	DT_VALUE	*TmpDt2
)
{
	int		Success = FALSE;

	switch( RootType )
	{
		case SLANG_ADD				:	Success = DT_OP( DT_MSG_ADD, 				&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_SUBTRACT			:	Success = DT_OP( DT_MSG_SUBTRACT,			&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_MULTIPLY			:	Success = DT_OP( DT_MSG_MULTIPLY,			&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_DIVIDE			:	Success = DT_OP( DT_MSG_DIVIDE,				&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_EQUAL			:	Success = DT_OP( DT_MSG_EQUAL,				&Ret->Data.Value, TmpDt1, TmpDt2 )
											   || DT_OP( DT_MSG_EQUAL,				&Ret->Data.Value, TmpDt2, TmpDt1 ); break;
		case SLANG_NOT_EQUAL		:	Success = DT_OP( DT_MSG_NOT_EQUAL,			&Ret->Data.Value, TmpDt1, TmpDt2 )
											   || DT_OP( DT_MSG_NOT_EQUAL,			&Ret->Data.Value, TmpDt2, TmpDt1 ); break;
		case SLANG_COMPARE			:	Success = DT_OP( DT_MSG_COMPARE,			&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_GREATER_THAN		:	Success = DT_OP( DT_MSG_GREATER_THAN,		&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_LESS_THAN		:	Success = DT_OP( DT_MSG_LESS_THAN,			&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_GREATER_OR_EQUAL	:	Success = DT_OP( DT_MSG_GREATER_OR_EQUAL,	&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
		case SLANG_LESS_OR_EQUAL	:	Success = DT_OP( DT_MSG_LESS_OR_EQUAL,		&Ret->Data.Value, TmpDt1, TmpDt2 );	break;
	}

	if( !Success )
	{
		switch( RootType )
		{
			case SLANG_EQUAL:		// If datatype can't compare, always FALSE!
				DTM_INIT( &Ret->Data.Value );
				Ret->Data.Value.Data.Number = (double) FALSE;
				Ret->Data.Value.DataType	= DtDouble;
				Success = TRUE;
				break;

			case SLANG_NOT_EQUAL:	// If datatype can't compare, always TRUE!
				DTM_INIT( &Ret->Data.Value );
				Ret->Data.Value.Data.Number = (double) TRUE;
				Ret->Data.Value.DataType	= DtDouble;
				Success = TRUE;
				break;

			case SLANG_COMPARE:
				Success = DT_OP( DT_MSG_COMPARE, &Ret->Data.Value, TmpDt2, TmpDt1 );
				if( Success )
					Ret->Data.Value.Data.Number *= -1.0;
				break;

			// Try the inverse (arg2 vs. arg1) Pay close attention to the inversion!!!
			case SLANG_GREATER_THAN		:	Success = DT_OP( DT_MSG_LESS_THAN,			&Ret->Data.Value, TmpDt2, TmpDt1 );	break;
			case SLANG_LESS_THAN		:	Success = DT_OP( DT_MSG_GREATER_THAN,		&Ret->Data.Value, TmpDt2, TmpDt1 );	break;
			case SLANG_GREATER_OR_EQUAL	:	Success = DT_OP( DT_MSG_LESS_OR_EQUAL,		&Ret->Data.Value, TmpDt2, TmpDt1 );	break;
			case SLANG_LESS_OR_EQUAL	:	Success = DT_OP( DT_MSG_GREATER_OR_EQUAL,	&Ret->Data.Value, TmpDt2, TmpDt1 );	break;

			default:
				break;
		}

		// If the rhs is a GsDt try to cast lhs to a GsDt
		// and then do the op.  This allows things like double + GsDt
		// to work properly by casting the double to a GsDt first.
		// Check types first to prevent recursion on repeated failure.

		if(    !Success && ( TmpDt1->DataType != TmpDt2->DataType ) 
			&& ( ( CC_NS(Gs,DtGsDt) == TmpDt2->DataType ) || ( DtString == TmpDt2->DataType ) ) )
		{
			DT_VALUE
					LhsWithRhsType;

			DTM_ALLOC( &LhsWithRhsType, TmpDt2->DataType );

			if( DTM_TO( &LhsWithRhsType, TmpDt1 ) )
				Success = SlangDtOperator( RootType, Ret, &LhsWithRhsType, TmpDt2 );
		}
		
		// Same principle as in the implicit-cast logic above, 
		// but for the GsDt/String OP NumericType combination.

		else if(    !Success && ( TmpDt1->DataType != TmpDt2->DataType ) 
					&& ( ( CC_NS(Gs,DtGsDt) == TmpDt1->DataType ) || ( DtString == TmpDt1->DataType ) )
                    && ( TmpDt2->DataType->Flags & DT_FLAG_NUMERIC ) )
		{
			DT_VALUE
					RhsWithLhsType;

			DTM_ALLOC( &RhsWithLhsType, TmpDt1->DataType );

			if( DTM_TO( &RhsWithLhsType, TmpDt2 ) )
				Success = SlangDtOperator( RootType, Ret, TmpDt1, &RhsWithLhsType );
		} 

	}

	return Success;
}



/****************************************************************
**	Routine: SlangXOperator
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Mathematical operators, compare functions
****************************************************************/

int SlangXOperator( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1,
			TmpRet2;

	int		Success = FALSE;

	SLANG_RET_CODE
			RetCode;

	if( SLANG_OK != (RetCode = SlangEvaluate( Root->Argv[ 0 ], SLANG_EVAL_RVALUE, &TmpRet1, Scope )))
	{
		if( SLANG_EXIT == RetCode || SLANG_EXCEPTION == RetCode )
		{
			Ret->Type		= TmpRet1.Type;
			Ret->Data.Value	= TmpRet1.Data.Value;
		}
		return( RetCode );
	}
	if( SLANG_OK != (RetCode = SlangEvaluate( Root->Argv[ 1 ], SLANG_EVAL_RVALUE, &TmpRet2, Scope )))
	{
		SLANG_RET_FREE( &TmpRet1 );
		if( SLANG_EXIT == RetCode || SLANG_EXCEPTION == RetCode )
		{
			Ret->Type		= TmpRet2.Type;
			Ret->Data.Value	= TmpRet2.Data.Value;
		}
		return( RetCode );
	}

	Ret->Type = SLANG_TYPE_VALUE;

	Success = SlangDtOperator( Root->Type, Ret, &TmpRet1.Data.Value, &TmpRet2.Data.Value );

	SLANG_RET_FREE( &TmpRet1 );
	SLANG_RET_FREE( &TmpRet2 );

	if( !Success )
	{
		DT_VALUE NullValue;
		DTM_INIT( &NullValue );
		Ret->Data.Value = NullValue;
		return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXLogicalOr
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Logical Or operator
****************************************************************/

int SlangXLogicalOr( SLANG_ARGS )
{
	int		Success;


/*
**	Analyze the first part
*/

	SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, Ret );
	Success = DT_OP( DT_MSG_TRUTH, NULL, &Ret->Data.Value, NULL );
	SLANG_RET_FREE( Ret );


/*
**	If needed, analyze the second part
*/

	if( !Success )
	{
		SLANG_EVAL_ARG( 1, SLANG_EVAL_RVALUE, Ret );
		Success = DT_OP( DT_MSG_TRUTH, NULL, &Ret->Data.Value, NULL );
		SLANG_RET_FREE( Ret );
	}

	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	DTM_INIT( &Ret->Data.Value );
	Ret->Data.Value.Data.Number = (double) Success;
	Ret->Data.Value.DataType	= DtDouble;

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXLogicalAnd
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Logical And operator
****************************************************************/

int SlangXLogicalAnd( SLANG_ARGS )
{
	int		Success;


/*
**	Analyze the first part
*/

	SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, Ret );
	Success = DT_OP( DT_MSG_TRUTH, NULL, &Ret->Data.Value, NULL );
	SLANG_RET_FREE( Ret );


/*
**	If needed, analyze the second part
*/

	if( Success )
	{
		SLANG_EVAL_ARG( 1, SLANG_EVAL_RVALUE, Ret );
		Success = DT_OP( DT_MSG_TRUTH, NULL, &Ret->Data.Value, NULL );
		SLANG_RET_FREE( Ret );
	}

	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	DTM_INIT( &Ret->Data.Value );
	Ret->Data.Value.Data.Number = (double) Success;
	Ret->Data.Value.DataType	= DtDouble;

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXUnaryNot
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Unary not operator
****************************************************************/

int SlangXUnaryNot( SLANG_ARGS )
{
	int		Success;


	SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, Ret );
	Success = DT_OP( DT_MSG_TRUTH, NULL, &Ret->Data.Value, NULL );
	SLANG_RET_FREE( Ret );

	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	DTM_INIT( &Ret->Data.Value );
	Ret->Data.Value.DataType	= DtDouble;
	Ret->Data.Value.Data.Number	= (double) !Success;

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXUnaryMinus
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Unary minus operator
****************************************************************/

int SlangXUnaryMinus( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1;

	int		Success;


	SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, &TmpRet1 );
	Ret->Type = SLANG_TYPE_VALUE;
	Success = DT_OP( DT_MSG_UNARY_MINUS, &Ret->Data.Value, &TmpRet1.Data.Value, NULL );
	SLANG_RET_FREE( &TmpRet1 );
	if( !Success )
		return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXHandleAnyment
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Handle ++Var, --Var, Var--, & Var++
****************************************************************/

int SlangXHandleAnyment(
	SLANG_ARGS,
	int		Message,
	int		Prefix
)
{
	SLANG_RET
			TmpRet;

	int		Status;

	DT_VALUE
			ValuePointer;


	SLANG_EVAL_ARG( 0, SLANG_EVAL_LVALUE, &TmpRet );

	if( TmpRet.Type == SLANG_VALUE_TYPE )
	{
		// FIX
		SLANG_RET_FREE( &TmpRet );
		return SlangEvalError( SLANG_ERROR_PARMS, "%s: Not supported for value methods",
				Message == DT_MSG_INCREMENT ? "++" : "--" );
	}

	if( EvalFlag == SLANG_EVAL_NO_VALUE )
	{
		SLANG_NO_VALUE_RET( Ret );

		// If we don't want the value, we don't care about Prefix vs Postfix
		Status = DT_OP( DT_MSG_DEREFERENCE, &ValuePointer, &TmpRet.Data.Value, NULL );
		Status = Status && DT_OP( Message, NULL, (DT_VALUE *) ValuePointer.Data.Pointer, NULL );
	}
	else
	{
		Status = DT_OP( DT_MSG_DEREFERENCE, &ValuePointer, &TmpRet.Data.Value, NULL );
		if( Prefix )
			Status = Status && DT_OP( Message, NULL, (DT_VALUE *) ValuePointer.Data.Pointer, NULL );

		Ret->Type = SLANG_TYPE_VALUE;
		Status = Status && DT_OP( DT_MSG_DEREFERENCE_VALUE, &Ret->Data.Value, &TmpRet.Data.Value, NULL );

		if( !Prefix )
			Status = Status && DT_OP( Message, NULL, (DT_VALUE *) ValuePointer.Data.Pointer, NULL );
	}
	SLANG_RET_FREE( &TmpRet );

	if( !Status )
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXPreIncrement
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : ++Var
****************************************************************/

int SlangXPreIncrement( SLANG_ARGS )
{
	return SlangXHandleAnyment( Root, EvalFlag, Ret, Scope, DT_MSG_INCREMENT, TRUE );
}



/****************************************************************
**	Routine: SlangXPreDecrement
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : --Var
****************************************************************/

int SlangXPreDecrement( SLANG_ARGS )
{
	return SlangXHandleAnyment( Root, EvalFlag, Ret, Scope, DT_MSG_DECREMENT, TRUE );
}



/****************************************************************
**	Routine: SlangXPostIncrement
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Var++
****************************************************************/

int SlangXPostIncrement( SLANG_ARGS )
{
	return SlangXHandleAnyment( Root, EvalFlag, Ret, Scope, DT_MSG_INCREMENT, FALSE );
}



/****************************************************************
**	Routine: SlangXPostDecrement
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Var--
****************************************************************/

int SlangXPostDecrement( SLANG_ARGS )
{
	return SlangXHandleAnyment( Root, EvalFlag, Ret, Scope, DT_MSG_DECREMENT, FALSE );
}



/****************************************************************
**	Routine: SlangXIf
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Conditional evaluation
****************************************************************/

int SlangXIf( SLANG_ARGS )
{
	int		Condition;

	SLANG_RET_CODE
			RetCode;

	SLANG_EVAL_FLAG
			BlockEvalFlag;


	switch( EvalFlag )
	{
		case SLANG_EVAL_HAS_RETURN:
			if( Root->Argc == 3 )
				return SlangHasReturn( Root->Argv[1] ) && SlangHasReturn( Root->Argv[2] );
			else
				return FALSE;

		case SLANG_EVAL_NO_VALUE:
			BlockEvalFlag = SLANG_EVAL_NO_VALUE;
			break;

		default:
			BlockEvalFlag = SLANG_EVAL_RVALUE;
	}

	if( Root->Argc < 2 || Root->Argc > 3 )
		return SlangEvalError( Root, EvalFlag, Ret, Scope,
					"If( Condition )\n"
					"    { TRUE_BLOCK }\n"
					":\n"
					"    { OPTIONAL_FALSE_BLOCK };" );

	SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, Ret );
	Condition = DT_OP( DT_MSG_TRUTH, NULL, &Ret->Data.Value, NULL );
	SLANG_RET_FREE( Ret );

	if( Condition )
		RetCode = SlangEvaluate( Root->Argv[1], BlockEvalFlag, Ret, Scope );
	else if( Root->Argc > 2 )
		RetCode = SlangEvaluate( Root->Argv[2], BlockEvalFlag, Ret, Scope );
	else if( EvalFlag == SLANG_EVAL_NO_VALUE )
	{
		SLANG_NO_VALUE_RET( Ret );
		RetCode = SLANG_OK;
	}
	else
	{
		SLANG_NULL_RET( Ret );
		RetCode = SLANG_OK;
	}
	return RetCode;
}



/****************************************************************
**	Routine: SlangXWhile
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Conditional loop execution
****************************************************************/

int SlangXWhile( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1;

	SLANG_RET_CODE
			CondRetCode;

	int		WhileCondition;

	SLANG_LOOP_VARS


	SLANG_LOOP_FUNC_INIT

	if( Root->Argc != 2 )
		return SlangEvalError( SLANG_ERROR_PARMS, "While( Condition ) {BLOCK};" );


	SLANG_LOOP_LOOP_INIT
	for(;;)
	{
		SLANG_LOOP_CHECK_ABORT


/*
**	Evaluate the conditional expression
*/

		if( SLANG_OK != (CondRetCode = SlangEvaluate( Root->Argv[ 0 ], SLANG_EVAL_RVALUE, &TmpRet1, Scope )))
		{
			SLANG_LOOP_RET_FREE
			if( SLANG_EXIT == CondRetCode || SLANG_EXCEPTION == CondRetCode)
			{
				Ret->Type		= TmpRet1.Type;
				Ret->Data.Value	= TmpRet1.Data.Value;
			}
			return CondRetCode;
		}
		WhileCondition = DT_OP( DT_MSG_TRUTH, NULL, &TmpRet1.Data.Value, NULL );
		SLANG_RET_FREE( &TmpRet1 );
		if( !WhileCondition )
		{
			LoopRetCode = SLANG_OK;
			goto SlangLoopExit;
		}


/*
**	Evaluate block if the conditional was TRUE
*/

		SLANG_LOOP_BLOCK_EVAL
	}

SlangLoopExit:
	return LoopRetCode;
}



/****************************************************************
**	Routine: SlangXVoidBlock
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Evaluate the bound block, but don't care about the
**			 resultant value
****************************************************************/

int SlangXVoidBlock( SLANG_ARGS )
{
	static SLANG_ARG_LIST
			ArgList[] =
			{
				{ 	"Block",	NULL,	"Block to evaluate", 	SLANG_ARG_BLOCK		}
			};

	SLANG_ARG_VARS;

	SLANG_RET_CODE
			RetCode;


	SLANG_ARG_PARSE();
	RetCode = SLANG_BLOCK_EVAL( SLANG_EVAL_NO_VALUE );

	*Ret = SLANG_BLOCK_RET;
	return RetCode;
}



/****************************************************************
**	Routine: SlangXBlock
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Evaluate expressions within a block
****************************************************************/

int SlangXBlock( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1;

	int		ArgNum;

	SLANG_RET_CODE
			RetCode;

	SLANG_EVAL_FLAG
			EvalKind;


	switch( EvalFlag )
	{
		case SLANG_EVAL_HAS_RETURN:
			if( Root->Argc <= 0 )
				return FALSE;
			return SlangHasReturn( Root->Argv[ Root->Argc-1 ] );

		case SLANG_EVAL_NO_VALUE:
			EvalKind = SLANG_EVAL_NO_VALUE;
			break;

		default:
			EvalKind = SLANG_EVAL_RVALUE;
	}

	SLANG_NULL_RET( Ret );
	SLANG_NULL_RET( &TmpRet1 );

/*
	if( EvalFlag == SLANG_EVAL_NO_VALUE )
		EvalKind = SLANG_EVAL_NO_VALUE;
	else if( ArgNum < Root->Argc-1 )
	{
		if( Root->Argv[ ArgNum ] is a break or a continue )
			EvalKind = SLANG_EVAL_RVALUE;
		else
			EvalKind = SLANG_EVAL_NO_VALUE;
	}
	else
		EvalKind = SLANG_EVAL_RVALUE;
*/

	for( ArgNum = 0; ArgNum < Root->Argc; ArgNum++ )
	{
		RetCode = SlangEvaluate( Root->Argv[ ArgNum ], EvalKind, &TmpRet1, Scope );
		switch( RetCode )
		{
			case SLANG_OK:
				SLANG_RET_FREE( Ret );
				Ret->Type		= TmpRet1.Type;
				if( Ret->Type != SLANG_TYPE_NO_VALUE )
					Ret->Data.Value = TmpRet1.Data.Value;
				break;

			case SLANG_ABORT:
			case SLANG_ERROR:
				SLANG_RET_FREE( Ret );
				return RetCode;

			case SLANG_BACKUP:
				ArgNum -= 2;
				if( ArgNum < -1 )
					return RetCode;
				break;

			case SLANG_EXCEPTION:
			case SLANG_RETURN:
			case SLANG_EXIT:
			default:
				SLANG_RET_FREE( Ret );
				Ret->Type		= TmpRet1.Type;
				Ret->Data.Value = TmpRet1.Data.Value;
				return RetCode;
		}
	}

	if( EvalFlag == SLANG_EVAL_NO_VALUE )
	{
		SLANG_RET_FREE( Ret );
		SLANG_NO_VALUE_RET( Ret );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: GetValueFromSecList
**	Returns: TRUE or FALSE
**	Action : Calls SecDbGetValueWithArgs for each SecPtr and
**			 assigns result to corresponding component of RetValue
****************************************************************/

static int GetValueFromSecList(
	DT_VALUE		*RetValue,
	DT_VALUE		*SecList,
	DT_VALUE		*Default,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	DT_VALUE		**Argv
)
{
	DT_VALUE
			*TempValue,
			Enum,
			ElemValue,
			NewValue;

	SDB_OBJECT
			*SecPtr;


/*
**	Make copy to preserve structure
*/

	if( DtSecurityList == SecList->DataType )
	{
		RetValue->DataType = DtArray;
		if( !DTM_TO( RetValue, SecList ))
			return FALSE;
	}
	else if( !DTM_ASSIGN( RetValue, SecList ))
		return FALSE;

	DTM_FOR( &Enum, SecList )
	{
		TempValue = NULL;
		if( !DtSubscriptGetCopy( SecList, &Enum, &ElemValue ))
			return FALSE;
		if( ElemValue.DataType == DtSecurity )
		{
			SecPtr = (SDB_OBJECT *) ElemValue.Data.Pointer;
			if( SecPtr )
				TempValue = SecDbGetValueWithArgs( SecPtr, ValueType, Argc, Argv, NULL );
		}
		DTM_FREE( &ElemValue );

		if( !TempValue )
			TempValue = Default;

		if( !DTM_ASSIGN( &NewValue, TempValue ))
			return FALSE;
		if( !DtSubscriptReplace( RetValue, &Enum, &NewValue ))
			return FALSE;
	}
	return TRUE;
}



/****************************************************************
**
**	Routine: SlangXComponent
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Handle data type components (structure elements)
**
****************************************************************/

SLANG_RET_CODE SlangXComponent( SLANG_ARGS )
{
	SLANG_RET
			TmpRet1;

	DT_VALUE
			*ParentValue,
			VariableDeref,
			*VariableVal = NULL,
			TmpValue1;

	int		Success,
			WantRealValue;

    SLANG_NO_VALUE_RET( Ret ); // Initialize to be sure.

/*
**	Get the arguments (var,index)
*/

	SLANG_EVAL_ARG( 0, SLANG_EVAL_LVALUE, &TmpRet1 );

	DTM_INIT( &TmpValue1 );
	TmpValue1.DataType		= DtString;
	TmpValue1.Data.Pointer	= Root->StringValue;

/*
**	Evaluate (or not...)
*/

	switch( TmpRet1.Type )
	{
		case SLANG_TYPE_VALUE:
		case SLANG_TYPE_STATIC_VALUE:
			ParentValue = &TmpRet1.Data.Value;

			ERR_OFF();
			if( DT_OP( DT_MSG_DEREFERENCE, &VariableDeref, ParentValue, NULL ))
				VariableVal = (DT_VALUE *) VariableDeref.Data.Pointer;
			ERR_ON();

			// Intercept SecDb things which would not normally support component:  String, Security, Security List
			if( VariableVal && (
					VariableVal->DataType == DtString ||
					VariableVal->DataType == DtSecurity ||
					VariableVal->DataType == DtSecurityList ))
			{
				SLANG_RET_FREE( &TmpRet1 );
				return ValueMethod( Root, EvalFlag, Ret, Scope, SecDbValueTypeFromName( Root->StringValue, NULL ));
			}

			break;

		case SLANG_VALUE_TYPE:
			SLANG_RET_FREE( &TmpRet1 );
			// return ValueMethod( Root, EvalFlag, Ret, Scope, SecDbValueTypeFromName( Root->StringValue, NULL ));
			SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, &TmpRet1 );
			ParentValue = &TmpRet1.Data.Value;
			EvalFlag	= SLANG_EVAL_RVALUE;
			break;

		default:
			SLANG_RET_FREE( &TmpRet1 );
			return SlangEvalError( Root, EvalFlag, Ret, Scope, "Can't get a component from left value" );
	}

	switch( EvalFlag )
	{
		case SLANG_EVAL_RVALUE:
		case SLANG_EVAL_NO_VALUE:
			WantRealValue = TRUE;
			break;

		default:
			WantRealValue = ParentValue->DataType != DtPointer;
			break;
	}

	if( WantRealValue )
	{
		if( VariableVal )
			ParentValue = VariableVal;
		Success = DT_OP( DT_MSG_COMPONENT_VALUE, &Ret->Data.Value, ParentValue, &TmpValue1 );
	}
	else
		Success = DtPointerComponent( &Ret->Data.Value, ParentValue, &TmpValue1 );

	SLANG_RET_FREE( &TmpRet1 );
	if( !Success )
	{
		ErrMore( "'.%s'", Root->StringValue );
		return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
	}

	Ret->Type = SLANG_TYPE_VALUE; // We are certain to have a valid component value here.
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXValueMethod
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Perform a value method on a security
****************************************************************/

int SlangXValueMethod( SLANG_ARGS )
{
/*
**	Must have at least one argument (security name)
*/

	if( Root->Argc < 1 )
		return SlangEvalError( SLANG_ERROR_PARMS, "Value Type usage: %s( SecName [, Arg1, ... ] )", SLANG_NODE_STRING_VALUE( Root ));

	return ValueMethod( Root, EvalFlag, Ret, Scope, SLANG_NODE_VALUE_TYPE( Root ));
}


/****************************************************************
**	Routine: SlangXValueMethodFunc
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Perform a value method on a security
****************************************************************/

int SlangXValueMethodFunc( SLANG_ARGS )
{
/*
**	Must have at least two arguments (valuetype name & security name)
*/

	if( Root->Argc < 2 )
		return SlangEvalError( SLANG_ERROR_PARMS, "Usage: %s( ValueType, SecName [, Arg1, ... ] )", Root->StringValue );

	return ValueMethod( Root, EvalFlag, Ret, Scope, NULL );
}



/****************************************************************
**	Routine: ValueMethod
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Perform a value method on a security
****************************************************************/

static int ValueMethod(
	SLANG_NODE		*Root,
	SLANG_EVAL_FLAG	EvalFlag,
	SLANG_RET		*Ret,
	SLANG_SCOPE		*Scope,
	SDB_VALUE_TYPE	ValueMethod
)
{
	SDB_OBJECT
			*SecPtr;

	DT_VALUE
			*TmpValue,
			**Argv,
			SecList,
			TmpList,
			NullValue;

	SLANG_RET
			TmpRet1,
			TmpRet2,
			TmpRetArg,
			*Rets;

	int		Argc,
			ArgNum,
			RetCode,
			Status,
			ArgIndex;
	RetCode = SLANG_OK;


	if( NULL == ValueMethod )	// Get ValueMethod as first arg
	{
		ArgIndex	= 2;
		SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, &TmpRet2 );

		if( DtValueType == TmpRet2.Data.Value.DataType )
			ValueMethod = (SDB_VALUE_TYPE) TmpRet2.Data.Value.Data.Pointer;
		else if( DtString == TmpRet2.Data.Value.DataType )
			ValueMethod	= SecDbValueTypeFromName( (char *) TmpRet2.Data.Value.Data.Pointer, NULL );
		else
		{
	   		SLANG_RET_FREE( &TmpRet2 );
			return SlangEvalError( SLANG_ERROR_PARMS, "Value Method must be String or Value Type" );
		}

	   	SLANG_RET_FREE( &TmpRet2 );

		if( !ValueMethod )
			return SlangEvalError( SLANG_ERROR_PARMS, "Value Method must be a valid name" );
	}
	else
		ArgIndex	= 1;

/*
**	Must have at least one/two argument (security name/valuemethod)
**	This should be checked further up, for meaningful error return
*/

	if( Root->Argc < ArgIndex )
		return SlangEvalError( SLANG_ERROR_PARMS, "Incorrect number of args" );


/*
**	Base arg is the security name (or security itself)
*/

	SLANG_NULL_RET( Ret );
	SecPtr = NULL;
	Status = TRUE;
	DTM_INIT( &SecList );

	SLANG_EVAL_ARG( ArgIndex - 1, SLANG_EVAL_RVALUE, &TmpRet1 );

	if( TmpRet1.Data.Value.DataType == DtString )
	{
		if( strlen( (char *) TmpRet1.Data.Value.Data.Pointer ) >= SDB_SEC_NAME_SIZE )
			Status = Err( ERR_INVALID_ARGUMENTS, "Value type: Security name '%s' exceeds %d characters", (char *) TmpRet1.Data.Value.Data.Pointer, SDB_SEC_NAME_SIZE - 1 );
		else
			strcpy( Ret->Data.Eval.SecName, (char *) TmpRet1.Data.Value.Data.Pointer );
	}

	else if( TmpRet1.Data.Value.DataType == DtSecurity )
	{
		SecPtr = (SDB_OBJECT *) TmpRet1.Data.Value.Data.Pointer;
		if( SecPtr )
		{
			SecDbIncrementReference( SecPtr );
			strcpy( Ret->Data.Eval.SecName, SecPtr->SecData.Name );
		}
		else
			Status = Err( ERR_INVALID_ARGUMENTS, "Value type: '%s' First parameter is not a valid security", Root->StringValue );
	}
	else if( TmpRet1.Data.Value.DataType == CC_NS( Gs, DtGsDt ) )
	{
		const CC_NS( Gs, GsDt* ) Cast
				= static_cast< const CC_NS( Gs, GsDt * ) >( TmpRet1.Data.Value.Data.Pointer );

		if( Cast )
		{
			const CC_NS( Gs, GsDtString* ) GsDtStr 
					= CC_NS( Gs, GsDtString::Cast )( Cast );

			if( GsDtStr )
			{
				CC_NS( Gs, GsString ) Str = GsDtStr->data();

				if( Str.length() >= SDB_SEC_NAME_SIZE )
					Status = Err( ERR_INVALID_ARGUMENTS, "Value type: Security name '%s' exceeds %d characters", (char *) TmpRet1.Data.Value.Data.Pointer, SDB_SEC_NAME_SIZE - 1 );
				else
					strcpy( Ret->Data.Eval.SecName, Str.c_str() );
			}
			else
			{
				const CC_NS( Gs, LtSecDbObj::GsDtType * ) GsDtSec 
						= CC_NS( Gs, LtSecDbObj::GsDtType::Cast )( Cast );

				if( GsDtSec )
				{
					SecPtr = (SDB_OBJECT *) GsDtSec->data().ptr();

					if( SecPtr )
					{
						SecDbIncrementReference( SecPtr );
						strcpy( Ret->Data.Eval.SecName, SecPtr->SecData.Name );
					}
					else
						Status = Err( ERR_INVALID_ARGUMENTS, "@: Security must be of type Security, Security List, String, GsDtString, Array, LtSecDbObj or Structure"  );
				}
				else
					Status = Err( ERR_INVALID_ARGUMENTS, "@: Security must be of type Security, Security List, String, GsDtString, Array, LtSecDbObj or Structure"  );
			}
		}
		else
			Status = Err( ERR_INVALID_ARGUMENTS, "@: Security must be of type Security, Security List, String, GsDtString, Array, LtSecDbObj or Structure"  );
	}
	else if( SLANG_EVAL_RVALUE == EvalFlag || SLANG_EVAL_NO_VALUE == EvalFlag )
	{
		if( TmpRet1.Data.Value.DataType == DtSecurityList )
			Status = DTM_ASSIGN( &SecList, &TmpRet1.Data.Value );

		else if( TmpRet1.Data.Value.DataType == DtArray || TmpRet1.Data.Value.DataType == DtStructure )
		{
			DTM_INIT( &TmpList );
			SecList.DataType	= TmpRet1.Data.Value.DataType;

			Status = SecDbGetMany( SecDbRootDb, 0, &TmpRet1.Data.Value, &TmpList, TRUE );
			Status = Status && DTM_TO( &SecList, &TmpList );

			DTM_FREE( &TmpList );
		}
		else
			Status = Err( ERR_INVALID_ARGUMENTS, "@: Security must be of type Security, Security List, String, Array, or Structure" );
	}
	else
		Status = Err( ERR_INVALID_ARGUMENTS, "@: Security must be of type Security or String" );

	SLANG_RET_FREE( &TmpRet1 );
	if( !Status )
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );


/*
**	Evaluate if requested
*/

	Ret->Data.Eval.ValueType = ValueMethod;
	if( SLANG_EVAL_RVALUE == EvalFlag || SLANG_EVAL_NO_VALUE == EvalFlag )
	{
		if( !SecPtr && SecList.DataType == DtNull )
		{
			SecPtr = SecDbGetByName( Ret->Data.Eval.SecName, SecDbRootDb, 0 );
			if( !SecPtr )
				return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
		}

		if( Root->Argc > ArgIndex )
		{
			Argc = Root->Argc - ArgIndex;
			if(	   !ERR_MALLOC( Argv, DT_VALUE*, Argc * sizeof( *Argv ))
				|| !ERR_MALLOC( Rets, SLANG_RET, Argc * sizeof( *Rets )))
				return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );

			for( ArgNum = 0; ArgNum < Argc; ArgNum++ )
			{
				RetCode = SlangEvaluate( Root->Argv[ ArgNum + ArgIndex ], SLANG_EVAL_RVALUE, &Rets[ ArgNum ], Scope );
				if( SLANG_OK == RetCode )
				{
					Argv[ ArgNum ] = &SLANG_VAL( ArgNum );
					continue;
				}

				if (SLANG_EXCEPTION == RetCode)
					*Ret = Rets[ ArgNum ]; /* on exception store the exception val in Ret */
				if( SLANG_RETURN != RetCode )
					ArgNum--; /* Don't free current arg */

				if( SLANG_ABORT != RetCode  && SLANG_EXCEPTION != RetCode)
					RetCode = SLANG_ERROR;

				for( ; ArgNum >= 0; ArgNum-- )
					SLANG_RET_FREE( &Rets[ ArgNum ] );
				free( Argv );
				free( Rets );
				SecDbFree( SecPtr );
				return RetCode;
			}
		}
		else
		{
			Argc = 0;
			Argv = NULL;
			Rets = NULL;
		}

		if( SecPtr )
		{
			SecDb_Slang_Scope guard( Scope, &Root->ErrorInfo );

			TmpValue = SecDbGetValueWithArgs( SecPtr, Ret->Data.Eval.ValueType, Argc, Argv, NULL );
			if( TmpValue )
			{
				DTM_ASSIGN( &Ret->Data.Value, TmpValue );
			}
			else
				DTM_INIT( &Ret->Data.Value );
			SecDbFree( SecPtr );
			Status = TRUE;
		}
		else
		{
			SecDb_Slang_Scope guard( Scope, &Root->ErrorInfo );

			DTM_INIT( &NullValue );

			Status = GetValueFromSecList( &Ret->Data.Value, &SecList, &NullValue, Ret->Data.Eval.ValueType, Argc, Argv );

			DTM_FREE( &NullValue );
			DTM_FREE( &SecList );
		}

		for( ArgNum = 0; ArgNum < Argc; ArgNum++ )
			SLANG_RET_FREE( &Rets[ ArgNum ] );
		free( Rets );
		free( Argv );
		if( !Status )
			return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );

		if( SLANG_EVAL_NO_VALUE == EvalFlag)
		{
			// Admittedly a bit silly to get the value and then throw it away, but it seems like the easiest way
			// to deal with GetMany and other complex get values. And if you are silly enough to evaluate a VT for it's
			// sideeffects only, then well you deserve it :-)
			DTM_FREE( &Ret->Data.Value );
			Ret->Type = SLANG_TYPE_NO_VALUE;
		}
		else
			Ret->Type = SLANG_TYPE_VALUE;
	}
	else if( SLANG_EVAL_LVALUE == EvalFlag || SLANG_EVAL_LVALUE_CREATE == EvalFlag )
	{
		if( SecPtr )
			SecDbFree( SecPtr );
		if( Root->Argc > ArgIndex )
		{
			Argc = Root->Argc - ArgIndex;
			if(	!ERR_CALLOC( Argv, DT_VALUE*, Argc, sizeof( *Argv )))
				return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );

			for( ArgNum = 0; ArgNum < Argc; ArgNum++ )
			{
				RetCode = SlangEvaluate( Root->Argv[ ArgNum + ArgIndex ], SLANG_EVAL_RVALUE, &TmpRetArg, Scope );
				if( SLANG_OK == RetCode )
				{
					if(	!ERR_MALLOC( Argv[ArgNum], DT_VALUE, sizeof( *Argv[0] )))
						return SlangEvalError( Root, EvalFlag, Ret, Scope, "Out of memory and a bunch just leaked" );
					DTM_ASSIGN( Argv[ ArgNum ], &TmpRetArg.Data.Value );
					SLANG_RET_FREE( &TmpRetArg );
					continue;
				}

				// FIX-Need to free if SLANG_BREAK as well?
				if( SLANG_RETURN == RetCode )
					SLANG_RET_FREE( &TmpRetArg );

				if (SLANG_EXCEPTION == RetCode)
					*Ret = TmpRetArg; /* on exception store the exception val in Ret */
				else if( SLANG_ABORT != RetCode )
					RetCode = SLANG_ERROR;
				// Don't free this argument, but free all others
				while( --ArgNum >= 0 )
				{
					DTM_FREE( Argv[ ArgNum ] );
					free( Argv[ ArgNum ] );
				}
				free( Argv );
				return RetCode;
			}

			Ret->Data.Eval.Argc	= Argc;
			Ret->Data.Eval.Argv	= Argv;
		}
		else
		{
			Ret->Data.Eval.Argc	= 0;
			Ret->Data.Eval.Argv	= NULL;
		}
		Ret->Type = SLANG_VALUE_TYPE;

	}
	else
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "ValueType: Unknown EvalFlag (%d)", EvalFlag );

	return RetCode;
}



/****************************************************************
**	Routine: SlangXDataTypeCreator
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Create a new instance of a data type
****************************************************************/

int SlangXDataTypeCreator( SLANG_ARGS )
{
	DT_VALUE
			TmpValue;

	int		Success;

	DT_DATA_TYPE
			DataType;


	if( Root->Type == SLANG_RESERVED_FUNC )
	{
		// someone who didn't know about datatypes created this node, so lookup the datatype
		DataType = DtFromName( SLANG_NODE_STRING_VALUE( Root ));
		if( !DataType )
			return SlangEvalError( SLANG_ERROR_PARMS, "DataTypeConstructor: No such datatype '%s'",
								   SLANG_NODE_STRING_VALUE( Root ));
	}
	else
		DataType = Root->ValueDataType;
	if( !DataType->Func )
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s: Unknown data type", DataType->Name );


/*
**	If there are no arguments, create a new instance of the datatype
*/

	if( Root->Argc == 0 )
	{
		if( !(DTM_ALLOC( &Ret->Data.Value, DataType )))
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );
		Ret->Type = SLANG_TYPE_VALUE;
	}


/*
**	If there is one argument, convert it into the new datatype
*/

	else if( 1 == Root->Argc )
	{
		SLANG_EVAL_ARG( 0, SLANG_EVAL_RVALUE, Ret );
		TmpValue.DataType = DataType;
		Success = DTM_TO( &TmpValue, &Ret->Data.Value );
		SLANG_RET_FREE( Ret );
		if( !Success )
			return SlangEvalError( Root, EvalFlag, Ret, Scope, ERR_STR_DONT_CHANGE );

		Ret->Data.Value = TmpValue;
		Ret->Type = SLANG_TYPE_VALUE;
	}


/*
**	Wrong number of arguments
*/

	else
	{
		Err( SDB_ERR_SLANG_EVAL, "DataTypeConstructor for %s\nUsage: %s( [ Arg ] )", DataType->Name, DataType->Name );
		for( DT_HELP *Help = DataType->Help; Help && Help->HelpText; ++Help )
		{
			if( Help->Type )
				ErrMore( "%s: %s\n", Help->Type, Help->HelpText );
			else
				ErrMore( "%s\n", Help->HelpText );
		}
		return SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );
	}
	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXArrayInit
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Creates a new array
****************************************************************/

int SlangXArrayInit( SLANG_ARGS )
{
	SLANG_RET
			TmpRet;

	int		ArgNum,
			RetCode;


/*
**	Create and fill up an array
*/

	Ret->Type = SLANG_TYPE_VALUE;
	DTM_ALLOC( &Ret->Data.Value, DtArray );

	for( ArgNum = 0; ArgNum < Root->Argc; ArgNum++ )
	{
		RetCode = SlangEvaluate( Root->Argv[ ArgNum ], SLANG_EVAL_RVALUE, &TmpRet, Scope );
		if( SLANG_OK != RetCode )
		{
			SLANG_RET_FREE( Ret );
			if (SLANG_EXCEPTION == RetCode)
			{
				*Ret = TmpRet;
				return (RetCode);
			}
			else if( SLANG_ERROR != RetCode )
				SlangEvalError( Root, EvalFlag, Ret, Scope, "Control statements invalid in array initialization" );
			return( SLANG_ERROR );
		}
		DT_OP( DT_MSG_ASSIGN_AND, NULL, &Ret->Data.Value, &TmpRet.Data.Value );
		SLANG_RET_FREE( &TmpRet );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXGlobal
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Reference a global variable
****************************************************************/

int SlangXGlobal( SLANG_ARGS )
{
	SLANG_VARIABLE
			*Variable;

	int		RetVal;

	unsigned long
			Bucket;


	if( Root->Argc != 1 )
	{
	  Usage:
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "Global( GlobalVariable )" );
	}

	if( SLANG_TYPE_VARIABLE == Root->Argv[ 0 ]->Type )
		Bucket = SLANG_NODE_BUCKET( Root->Argv[ 0 ]);
	else if( SLANG_TYPE_LOCAL_VARIABLE == Root->Argv[ 0 ]->Type )
		Bucket = HashStrHash( SLANG_NODE_STRING_VALUE( Root->Argv[ 0 ]));
	else
		goto Usage;

	Variable = SlangVariableGetByBucket( SLANG_NODE_STRING_VALUE( Root->Argv[ 0 ]), NULL, Bucket );

	if( SLANG_EVAL_RVALUE == EvalFlag )
	{
		if( !Variable )
			return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s'", Root->Argv[ 0 ]->StringValue );
		if( Variable->Flags & SLANG_VARIABLE_DEBUG_READ )
		{
			RetVal = SlangXDebug( Root, EvalFlag, Ret, Scope );
			if( RetVal != SLANG_OK )
				return( RetVal );
		}
		Ret->Type = SLANG_TYPE_STATIC_VALUE;
		Ret->Data.Value = *(Variable->Value);
	}
	else
	{
		if( !Variable )
		{
			if( SLANG_EVAL_LVALUE_CREATE == EvalFlag )
				Variable = SlangVariableSetByBucket( SLANG_NODE_STRING_VALUE( Root->Argv[ 0 ]), NULL, NULL, FALSE, Bucket );
			else
				return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s'", Root->Argv[ 0 ]->StringValue );
		}

		if( Variable->Flags & SLANG_VARIABLE_DEBUG_WRITE )
		{
			RetVal = SlangXDebug( Root, EvalFlag, Ret, Scope );
			if( RetVal != SLANG_OK )
				return( RetVal );
		}
		Ret->Type = SLANG_TYPE_VALUE;
		DtPointerVariable( &Ret->Data.Value, Variable );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXDoGlobal
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Switch into global context
****************************************************************/

int SlangXDoGlobal( SLANG_ARGS )
{
	if( Root->Argc < 1 )
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s\n{\n    BLOCK\n};", Root->StringValue );

	HASH *VariableHash = Scope->FunctionStack->VariableHash;
	Scope->FunctionStack->VariableHash = SlangVariableHash;

	SLANG_VARIABLE **Locals = Scope->FunctionStack->Locals;
	Scope->FunctionStack->Locals = NULL;

	int const RetCode = SlangEvaluate( Root->Argv[ 0 ], SLANG_EVAL_RVALUE, Ret, Scope );

	Scope->FunctionStack->VariableHash = VariableHash;
	Scope->FunctionStack->Locals = Locals;

	return RetCode;
}



/****************************************************************
**	Routine: SlangXSticky
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Reference a sticky variable
****************************************************************/

int SlangXSticky( SLANG_ARGS )
{
	SLANG_VARIABLE
			*Variable;

	unsigned long
			Bucket;

	if( Root->Argc != 1 )
	{
	  Usage:
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s: ( StickyVariable )", SLANG_NODE_STRING_VALUE( Root ));
	}

	if( SLANG_TYPE_VARIABLE == Root->Argv[ 0 ]->Type )
		Bucket = SLANG_NODE_BUCKET( Root->Argv[ 0 ]);
	else if( SLANG_TYPE_LOCAL_VARIABLE == Root->Argv[ 0 ]->Type )
		Bucket = HashStrHash( Root->Argv[ 0 ]);
	else
		goto Usage;

	Variable = SlangVariableGetScopedByBucket( SlangVariableScopeGet( "Sticky", TRUE )->VariableHash, SLANG_NODE_STRING_VALUE( Root->Argv[ 0 ]), Bucket );

	if( SLANG_EVAL_RVALUE == EvalFlag )
	{
		if( !Variable )
			return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s'", Root->Argv[ 0 ]->StringValue );
		Ret->Type = SLANG_TYPE_STATIC_VALUE;
		Ret->Data.Value = *(Variable->Value);
	}
	else
	{
		if( !Variable )
		{
			if( SLANG_EVAL_LVALUE_CREATE == EvalFlag )
				Variable = SlangVariableSetScopedByBucket( SlangVariableScopeGet( "Sticky", TRUE )->VariableHash, SLANG_NODE_STRING_VALUE( Root->Argv[ 0 ]), NULL, FALSE, Bucket );
			else
				return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s'", Root->Argv[ 0 ]->StringValue );
		}

		Ret->Type = SLANG_TYPE_VALUE;
		DtPointerVariable( &Ret->Data.Value, Variable );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXStickyDiddle
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Evaluate all expressions using the sticky-scope for
**			 any of the diddles
****************************************************************/

int SlangXStickyDiddle( SLANG_ARGS )
{
	static int
			InStickyDiddle = 0,
			AllowNestedStickyDiddles = -1;

	if( Root->Argc != 1 )
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "%s\n{\n    BLOCK\n};", Root->StringValue );

	if( AllowNestedStickyDiddles == -1 )
		AllowNestedStickyDiddles = stricmp( SecDbConfigurationGet( "SECDB_ALLOW_NESTED_STICKY_DIDDLES", NULL, NULL, "false" ), "true" ) == 0;

	if( InStickyDiddle && !AllowNestedStickyDiddles )
		return SlangEvalError( Root, EvalFlag, Ret, Scope, "Cannot nest sticky diddle scopes. Already in StickyDiddle\n" );

	++InStickyDiddle;
	SlangScopePushDiddles( Scope, SlangStickyDiddles );
	int const RetCode = SlangEvaluate( Root->Argv[ 0 ], SLANG_EVAL_RVALUE, Ret, Scope );
	SlangScopePopDiddles( Scope, FALSE );
	--InStickyDiddle;

	return RetCode;
}



/****************************************************************
**	Routine: SlangXScopeOperator
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Reference a scoped variable
****************************************************************/

int SlangXScopeOperator( SLANG_ARGS )
{
	const char
			*VarName;

	unsigned long
			VarBucket;

	SLANG_VARIABLE_SCOPE
			*VariableScope;

	SLANG_VARIABLE
			*Variable;

	int		RetVal;


	if( Root->Type == SLANG_TYPE_SCOPED_VARIABLE )
	{
		Variable = (SLANG_VARIABLE *) SLANG_NODE_BUCKET( Root->Argv[ 1 ]);
		VarName = Variable->Name;

		VariableScope = (SLANG_VARIABLE_SCOPE *) Root->StringValue;
		VarBucket = 0;					// Fake initializer in order to quite uninitialized warning
	}
	else
	{
		DT_VALUE
				TmpValue;

		VarName 	= SLANG_NODE_STRING_VALUE( Root->Argv[ 1 ]);
		VarBucket	= SLANG_NODE_BUCKET( Root->Argv[ 1 ]);

		if( !(VariableScope = (SLANG_VARIABLE_SCOPE *) Root->StringValue))
			return SlangEvalError( Root, EvalFlag, Ret, Scope, "Operator::, Internal error: Variable Scope is NULL" );
		if( !( Variable = SlangVariableGetScopedByBucket( VariableScope->VariableHash, VarName, VarBucket )) && VariableScope->RunUninitVarHook( VarName, VariableScope->VariableHash->Title, Root, Scope, &TmpValue ))
			Variable = SlangVariableSetScopedByBucket( VariableScope->VariableHash, VarName, &TmpValue, SLANG_VARIABLE_NO_COPY, VarBucket );
	}

	if( SLANG_EVAL_RVALUE == EvalFlag )
	{
		if( !Variable || ( Variable->Flags & SLANG_VARIABLE_DESTROYED ))
			return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s::%s'", VariableScope->VariableHash->Title, VarName );
		if( Variable->Flags & SLANG_VARIABLE_DEBUG_READ )
		{
			RetVal = SlangXDebug( Root, EvalFlag, Ret, Scope );
			if( RetVal != SLANG_OK )
				return( RetVal );
		}
		Ret->Type = SLANG_TYPE_STATIC_VALUE;
		Ret->Data.Value = *(Variable->Value);
	}
	else
	{
		if( !Variable )
		{
			if( SLANG_EVAL_LVALUE_CREATE == EvalFlag )
				Variable = SlangVariableSetScopedByBucket( VariableScope->VariableHash, VarName, NULL, FALSE, VarBucket );
			else
				return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s'", VarName );
		}

		if( ( Variable->Flags & SLANG_VARIABLE_DESTROYED ) && SLANG_EVAL_LVALUE_CREATE == EvalFlag )
			SlangVariableSetValue( Variable, NULL, 0 );

		if( Variable->Flags & SLANG_VARIABLE_DEBUG_WRITE )
		{
			RetVal = SlangXDebug( Root, EvalFlag, Ret, Scope );
			if( RetVal != SLANG_OK )
				return( RetVal );
		}
		Ret->Type = SLANG_TYPE_VALUE;
		DtPointerVariable( &Ret->Data.Value, Variable );
	}

	if( Variable && Root->Type == SLANG_SCOPE_OPERATOR )
	{
		Root->Type = SLANG_TYPE_SCOPED_VARIABLE;
		SLANG_NODE_BUCKET_SET( Root->Argv[ 1 ], (unsigned long) Variable );
		SlangVariableIncrRef( Variable );
	}

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXDebugBreak
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for DebugBreak
****************************************************************/

int SlangXDebugBreak( SLANG_ARGS )
{
	static const SLANG_ARG_LIST
			ArgList[] =
			{
				{ "Variable",		NULL,		"Variable to watch",				SLANG_ARG_VARIABLE					 },
				{ "BreakOnRead",	&DtDouble,	"Break when read (Def. TRUE)",		SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS },
				{ "BreakOnWrite",	&DtDouble,	"Break when written (Def. TRUE)",	SLANG_ARG_OPTIONAL | SLANG_ARG_NULLS },
			};

	SLANG_ARG_VARS;

	int		NewFlags = 0;

	DT_VALUE
			*PtrValue;

	DT_POINTER
			*Ptr;

	SLANG_VARIABLE
			*Variable;


	SLANG_ARG_PARSE();

	if( SLANG_ARG_OMITTED(1) || SLANG_INT(1) )
		NewFlags |= SLANG_VARIABLE_DEBUG_READ;
	if( SLANG_ARG_OMITTED(2) || SLANG_INT(2) )
		NewFlags |= SLANG_VARIABLE_DEBUG_WRITE;

	PtrValue = &SLANG_VAL(0);
	Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );
	Variable = Ptr->Variable;
	Variable->Flags ^= Variable->Flags & ( SLANG_VARIABLE_DEBUG_READ | SLANG_VARIABLE_DEBUG_WRITE );
	Variable->Flags |= NewFlags;

	Ret->Type = SLANG_TYPE_VALUE;
	DTM_INIT( &Ret->Data.Value );
	Ret->Data.Value.DataType	= DtDouble;
	Ret->Data.Value.Data.Number = TRUE;

	SLANG_ARG_FREE();

	return SLANG_OK;
}



/****************************************************************
**	Routine: SlangXColonAssign
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for ColonAssign
****************************************************************/

SLANG_RET_CODE SlangXColonAssign( SLANG_ARGS )
{
	return SlangEvalError( SLANG_ERROR_PARMS, "Illegal use of :=" );
}



/****************************************************************
**	Routine: SlangXDollar
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for Dollar
****************************************************************/

SLANG_RET_CODE SlangXDollar( SLANG_ARGS )
{
	return SlangEvalError( SLANG_ERROR_PARMS, "$ is reserved for use within spreadsheet formulas only." );
}



/****************************************************************
**	Routine: SlangXSymbolStyleTsdb
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Wrapper for SymbolStyleTsdb
****************************************************************/

SLANG_RET_CODE SlangXSymbolStyleTsdb( SLANG_ARGS )
{
	if( Root->Argc != 1 )
		return SlangEvalError( Root, EvalFlag, Ret, Scope,
					"SymbolStyleTsdb\n"
					"    { BLOCK }\n"
					);


	return SlangEvaluate( Root->Argv[0], EvalFlag, Ret, Scope );
}


