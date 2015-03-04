#define GSCVSID "$Header: /home/cvs/src/secdb/src/x_exception.c,v 1.19 2014/06/09 21:56:46 khodod Exp $"
/****************************************************************
**
**	x_exception.cpp 	- Exception Addin Funcs
**
**	$Log: x_exception.c,v $
**	Revision 1.19  2014/06/09 21:56:46  khodod
**	Correctly go to the Slang debugger, if not in any enclosing try block
**	(pulled in from the pre-split changes that had never been deployed).
**	AWR: #340553
**
**	Revision 1.16  2001/10/30 14:34:50  shahia
**	fixed debugablity of exceptions and also the format of the red boxes
**
*****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<secexpr.h>
#include	<sdb_x.h>
#include	<secdb/sdb_exception.h>

#include 	<gsdt/x_gsdt.h>
#include 	<gsdt/GsDtString.h>
#include 	<gsdt/GsDtExceptionInfo.h>

#include 	<gscore/err.h>
#include 	<gscore/GsException.h>

#include	<dtptr.h>

#include    <exception>
CC_USING( std::exception );


CC_USING_NAMESPACE( Gs );

static SLANG_RET_CODE SlangErrorToException(
	void			*Handle,
	SLANG_NODE		*Root,
	SLANG_EVAL_FLAG	EvalFlag,
	SLANG_RET		*Ret,
	SLANG_SCOPE		*Scope,
	int				ErrCode
);


CC_STL_STACK(void*) TryBlockGuard::s_OldErrorFuncs;

TryBlockGuard::TryBlockGuard(SLANG_SCOPE* pScope): m_pScope(pScope)    
{
	++m_pScope->TryDepth;
	m_OldErrorFunc = SlangContextGet( "Error Function" ); 
	if ( m_OldErrorFunc != SlangErrorToException )
	{
		SlangContextSet( "Error Function", (void *)(&SlangErrorToException) );
		s_OldErrorFuncs.push( m_OldErrorFunc) ;
	}
}

TryBlockGuard::~TryBlockGuard()	
{
	--m_pScope->TryDepth;
	if ( m_OldErrorFunc != SlangErrorToException )
	{
		SlangContextSet( "Error Function", m_OldErrorFunc ); 
		s_OldErrorFuncs.pop( ) ;
	}
}

void TryBlockGuard::RestoreLastErrorFunc()
{
	if ( !s_OldErrorFuncs.empty() )
	{
		SlangContextSet( "Error Function", s_OldErrorFuncs.top() );
	}
}


static SLANG_RET_CODE SlangErrorToException(
	void			*Handle,
	SLANG_NODE		*Root,
	SLANG_EVAL_FLAG	EvalFlag,
	SLANG_RET		*Ret,
	SLANG_SCOPE		*Scope,
	int				ErrCode
)
{
	// turn errors to an exception with unknown source
	GsDtString* pGsDtString = new GsDtString( "SLANG_ERROR" );
	

	SecDbSetRetToException(Ret, Root, pGsDtString, ERR_INVALID_ARGUMENTS, "", 0, ErrGetString() );

	return SLANG_EXCEPTION;
}

/****************************************************************
**	Routine: SlangXTry
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Try block and then catch exception 
****************************************************************/

SLANG_RET_CODE SlangXTry( SLANG_ARGS )
{
	
	int	RetCode = SLANG_OK;
	SLANG_RET	ExceptVarRet;
	GsDtExceptionInfo	ExceptVar;


	if( Root->Argc != 3 )
		return SlangEvalError( Root, EvalFlag, Ret, Scope,
					"Try( ExeptionVar )\n"
					"    { TRY_BLOCK }\n"
					":\n"
					"    { CATCH_BLOCK };" );


	// Init Return vals
	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	DTM_INIT(&Ret->Data.Value);


	// do we need to return anyting
	SLANG_EVAL_FLAG
			BlockEvalFlag;

	switch( EvalFlag )
	{
		case SLANG_EVAL_HAS_RETURN:
			return SlangHasReturn( Root->Argv[1] ) && SlangHasReturn( Root->Argv[2] );

		case SLANG_EVAL_NO_VALUE:
			BlockEvalFlag = SLANG_EVAL_NO_VALUE;
			break;

		default:
			BlockEvalFlag = SLANG_EVAL_RVALUE;
	}

	// eval and create the exception var (1st arg) if required
	SLANG_EVAL_ARG(0, SLANG_EVAL_LVALUE_CREATE, &ExceptVarRet);

	
	try
	{
		// eval the 2nd arg with secview error func disabled
		{
			TryBlockGuard _TryBlockGuard ( Scope );
			RetCode = SlangEvaluate( Root->Argv[1], BlockEvalFlag, Ret, Scope );
		}
		const GsDtExceptionInfo* pGsDtExceptionInfo = 0;
		if (	RetCode == SLANG_EXCEPTION   &&
				Ret->Data.Value.DataType == DtGsDt &&
				(pGsDtExceptionInfo = SecDbGetException(Ret->Data.Value))
			)

		{
			DT_VALUE DtVal;
			DtVal.DataType = DtGsDt;
			DtVal.Data.Pointer = pGsDtExceptionInfo;	
			DtPointerAssign( &ExceptVarRet.Data.Value, &DtVal);
			return SlangEvaluate( Root->Argv[2], BlockEvalFlag, Ret, Scope );
		}
		// This condition shouldn't occur as SlangErrorToException would turn erros to exceptions
		// it is only here for completeness
		else if (	RetCode == SLANG_ERROR )
		{
			// turn errors to an exception with unknown source
			DT_VALUE DtVal;
			GsDtString* pGsDtString = new GsDtString( "SLANG_ERROR" );
			GsDtExceptionInfo* pGsDtExceptionInfo = new GsDtExceptionInfo(
						ERR_INVALID_ARGUMENTS,
						Root->ErrorInfo.ModuleName,
						Root->ErrorInfo.BeginningLine,
						Root->ErrorInfo.EndingLine,
						Root->ErrorInfo.BeginningColumn,
						Root->ErrorInfo.EndingColumn,
						"",
						0,
						pGsDtString,
						ErrGetString() 
						);
			DtVal.DataType = DtGsDt;
			DtVal.Data.Pointer = pGsDtExceptionInfo;
			DtPointerAssign( &ExceptVarRet.Data.Value, &DtVal);
			return SlangEvaluate( Root->Argv[2], BlockEvalFlag, Ret, Scope );
		}
		
	}
	
	catch( GsException const& E )
	{
		ErrMsg( ERR_LEVEL_ERROR, "@ Unexpected GsException Exception: caught exception '%s' at File %s, Line %d", E.what(), E.getFile() , E.getLine() );
    }
	
	catch( exception const& E )
	{
		ErrMsg( ERR_LEVEL_ERROR, "@ Unexpected exception Exception: caught exception '%s'", E.what());

	}
	catch(...)
	{
		ErrMsg( ERR_LEVEL_ERROR, "@ Unexpected, unknown, C++ Exception: caught exception");
        throw;

	}

	return RetCode;
}


/****************************************************************
**	Routine: SlangXThrow
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Throw exceptions in  slang
****************************************************************/

SLANG_RET_CODE SlangXThrow( SLANG_ARGS )
{
    if( EvalFlag == SLANG_EVAL_HAS_RETURN )
	    return TRUE;

    static const SLANG_ARG_LIST
    ArgList[] =
	{
		{	"Object",		NULL,	"Object to Throw/Exception to Rethrow", 0 },
		{	"ErrorCode",	&DtDouble,	"Error constant, defualt _ERR_UNKNOWN", SLANG_ARG_NULLS | SLANG_ARG_OPTIONAL }

	};

	SLANG_ARG_VARS;

	int ErrorCode;

	SLANG_ARG_PARSE();

	if( SLANG_ARG_OMITTED(1) )
		ErrorCode = ERR_UNKNOWN;
	else
		ErrorCode = (int) SLANG_NUM(1);

    // If we are not in any enclosing try block, go to the debugger.
    if ( Scope->TryDepth <= 0 )
	{
		TryBlockGuard::RestoreLastErrorFunc();
		GsDt *pGsDt = GsDtFromDtValue(&SLANG_VAL(0));
		if(!pGsDt) 
		{
			Err( ERR_INVALID_ARGUMENTS, "Argument is not a GsDt Compatible Data type");
			return SlangEvalError( SLANG_ERROR_PARMS, "Incorrect arg" );
		}

		GsDtExceptionInfo ExceptionInfo(
							ErrorCode,
							Root->ErrorInfo.ModuleName,
							Root->ErrorInfo.BeginningLine,
							Root->ErrorInfo.EndingLine,
							Root->ErrorInfo.BeginningColumn,
							Root->ErrorInfo.EndingColumn,
							"", // CPPFileName
							0,  // CPPLine
						    pGsDt	
							);

        return( SlangEvalError( SLANG_ERROR_PARMS, "Throw will not be caught by any try block" ) );
	}

	const GsDtExceptionInfo* pGsDtExceptionInfo = SecDbGetException(SLANG_VAL(0));
	if (pGsDtExceptionInfo)
	{
		// if rethrowing an exception then throw a copy
		// note: it is important to throw a copy, otherwise
		// top level will destroy uncaught exceptions and the var
		// in try would be pointing to freed mem
		SecDbInitExceptionRet(*Ret, static_cast<GsDtExceptionInfo*>( pGsDtExceptionInfo->CopyDeep() ) );
	}
	else
	{
		
		GsDt *pGsDt = GsDtFromDtValue(&SLANG_VAL(0));
		if(!pGsDt) 
		{
			Err( ERR_INVALID_ARGUMENTS, "Argument is not a GsDt Compatible Data type");
			return SlangEvalError( SLANG_ERROR_PARMS, "Incorrect arg" );
		}

		SecDbSetRetToException(Ret, Root, pGsDt, ErrorCode);

		// also set the the global slang error so that any app using slang_evaluate
		// and doesn't cater specially for Exceptions, reports correct error
		Err(ErrorCode, " ");
		//SLANG_SET_ERROR( Root );
		
		SLANG_ARG_FREE();
	}


	return SLANG_EXCEPTION;
}


