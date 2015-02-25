/****************************************************************
**
**	secloop.h	- Macros for Slang Loop functions
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secloop.h,v 1.4 1998/10/16 19:19:05 gribbg Exp $
**
****************************************************************/

#if !defined( IN_SECLOOP_H )
#define IN_SECLOOP_H



/*
**	Use these macros when creating a slang loop as follows:
**
**	SLANG_RET_CODE SlangXWhatever( SLANG_ARGS )
**	{
**		SLANG_LOOP_VARS
**
**		SLANG_LOOP_FUNC_INIT
**
**		SLANG_LOOP_LOOP_INIT
**		while( <something loopy> )
**		{
**			SLANG_LOOP_CHECK_ABORT
**
**			// Other stuff as desired
**			if( <exceptional condition> )
**			{
**				SLANG_LOOP_RET_FREE
**				Ret->Value = <Whatever>;
**				return SLANG_OK;
**			}
**
**
**			SLANG_LOOP_BLOCK_EVAL
**		}
**	SlangLoopExit:
**
**		return LoopRetCode;
**	}
**
**
**	Optionally, if the loop never needs the value of the block (all new loop
**	functions should behave this way), rather than using SLANG_LOOP_FUNC_INIT,
**	just use
**		LoopEvalFlag = SLANG_EVAL_NO_VALUE;
*/

#define SLANG_LOOP_VARS					\
	SLANG_RET_CODE		LoopRetCode;	\
	SLANG_EVAL_FLAG     LoopEvalFlag;

#define SLANG_LOOP_FUNC_INIT	\
	if( EvalFlag == SLANG_EVAL_NO_VALUE )	\
		LoopEvalFlag = SLANG_EVAL_NO_VALUE;	\
	else									\
		LoopEvalFlag = SLANG_EVAL_RVALUE;	\
	DEBUG_EXPECT_NO_VALUE		// If enabled, check that block is not looking for value

#define SLANG_LOOP_LOOP_INIT	\
	{							\
		LoopRetCode = SLANG_OK;	\
		SLANG_NULL_RET( Ret );	\
	}

#define SLANG_LOOP_RET_FREE						\
	{											\
			SLANG_RET_FREE( Ret );				\
	}

#define SLANG_LOOP_CHECK_ABORT				\
	if( SLANG_CHECK_ABORT( Root, Scope ))	\
	{										\
		LoopRetCode = SLANG_ABORT;			\
		SLANG_RET_FREE( Ret );				\
		goto SlangLoopExit;					\
	}

#define SLANG_LOOP_BLOCK_EVAL				\
	SLANG_LOOP_RET_FREE						\
	LoopRetCode = SlangEvaluate( Root->Argv[ Root->Argc-1 ],	\
								 LoopEvalFlag, Ret, Scope );	\
	switch( LoopRetCode )					\
	{										\
		case SLANG_CONTINUE:				\
			LoopRetCode = SLANG_OK;			\
		case SLANG_OK:						\
			break;							\
											\
		case SLANG_BREAK:					\
			LoopRetCode = SLANG_OK;			\
		default:							\
			goto SlangLoopExit;				\
	}

#endif

