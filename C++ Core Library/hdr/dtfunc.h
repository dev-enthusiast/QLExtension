/* $Header: /home/cvs/src/slang/src/dtfunc.h,v 1.6 2001/11/27 23:27:13 procmon Exp $ */
/****************************************************************
**
**	dtfunc.h	- Datatype member function structs and funcs\
**				  See src/slang/src/x_func.c for implementation
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_DTFUNC_H )
#define IN_DTFUNC_H


typedef struct DtFuncArgsStruct
{
	SLANG_NODE
			*Root;		// Argv[0]		- Object
						// StringValue	- member function name
						// Argv[1-]		- function args

	int		EvalFlag;	// Input

	SLANG_RET
			*Ret;		// Output - result goes here

	SLANG_SCOPE
			*Scope;

	int		RetCode;	// Output - SlangEval return code

	// Use DtFuncArgsFillIn to fill in these values
	char	*FuncName;

	DT_VALUE
			Args;		// As a DtArray

} DT_FUNC_ARGS;


DLLEXPORT DT_FUNC_ARGS
		*DtFuncArgsNew( SLANG_ARGS );

DLLEXPORT void
		DtFuncArgsDelete( DT_FUNC_ARGS *Args );

DLLEXPORT int
		DtFuncArgsFillIn( DT_FUNC_ARGS *Args);

DLLEXPORT int
		DtFuncArgsFillInAndRet( DT_FUNC_ARGS *Args, SLANG_RET *Ret);


#endif

