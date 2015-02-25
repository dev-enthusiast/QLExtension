/****************************************************************
**
**	slangfnc.h	- Application hooks to call slang functions
**
**	Copyright 1995-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/slang/src/slangfnc.h,v 1.12 2000/04/11 17:22:49 demeor Exp $
**
****************************************************************/

#if !defined( IN_SLANGFNC_H )
#define IN_SLANGFNC_H


/*
**	Structure used to hold application slang function call data
*/

#if !defined( IN_SECEXPR_H )
#include <secexpr.h>
#endif

typedef struct SlangAppFuncStructure
{
	SLANG_DATA_TYPE
			*SdtSlang;

	SLANG_SCOPE
			*Scope;

	SLANG_NODE
			*CallerRoot;

	DT_VALUE
			Function;

	DT_DATA_TYPE
			ReturnDataType;

} SLANG_APP_FUNC;

/*
**	Structure for context of wrapper fn for call to SlangFunctionCall
*/

typedef struct SlangNGenericFuncContext
{
	SLANG_APP_FUNC
			AppFunc;

	DT_VALUE
			ArgArray,
			*ArgPtr;

	int		RetCode;

} SLANG_N_GENERIC_FUNC_CONTEXT;

typedef struct SlangNGeneric2DFuncContext
{
	SLANG_APP_FUNC
			AppFunc;

	DT_VALUE
			ArgArray,
			*Arg1Ptr,
			*Arg2Ptr;

	int		RetCode;

} SLANG_N_GENERIC_FUNC_2D_CONTEXT;

typedef struct SlangNGeneric3DFuncContext
{
	SLANG_APP_FUNC
			AppFunc;

	DT_VALUE
			ArgArray,
			*Arg1Ptr,
			*Arg2Ptr,
			*Arg3Ptr;

	int		RetCode;

} SLANG_N_GENERIC_FUNC_3D_CONTEXT;

typedef struct SlangNGenericVectorFuncContext
{
	SLANG_APP_FUNC
			AppFunc;

	DT_VALUE
			ArgArray,
			*ArgPtr;

	int		RetCode;

} SLANG_N_GENERIC_FUNC_VECTOR_CONTEXT;

typedef struct SlangNGenericParam1VectorFuncContext
{
	SLANG_APP_FUNC
			AppFunc;

	DT_VALUE
			ArgArray,
			*ArgParamPtr,
			*ArgVectorPtr;

	int		RetCode;

} SLANG_N_GENERIC_FUNC_PARAM1_VECTOR_CONTEXT;


/*
**	Prototype functions
*/

DLLEXPORT int
		SlangAppFunctionPush(		SLANG_APP_FUNC *AppFunc, const char *FunctionName, SLANG_NODE *CallerRoot, DT_VALUE *Function, DT_DATA_TYPE ReturnDataType, SLANG_SCOPE *Scope ),
		SlangAppFunctionCall(		SLANG_APP_FUNC *AppFunc, DT_VALUE *ArgArray, SLANG_RET *Ret ),
		SlangAppFunctionCallAsArray(SLANG_APP_FUNC *AppFunc, SLANG_RET *Ret, int ArgFlags, int NumArgs, DT_VALUE **Args );

DLLEXPORT void
		SlangAppFunctionPop(		SLANG_APP_FUNC *AppFunc );


DLLEXPORT int
		SlangAppFuncNGeneric( 						double x, 			void *Context, double *Result ),
		SlangAppFuncNGeneric2D(						double x, double y, void *Context, double *Result ),
		SlangAppFuncNGeneric3D(						double x, double y, double z, void *Context, double *Result ),
		SlangAppFuncNGenericParam1Vector( double p, double *x,			void *Context, double *Result ),
		SlangAppFuncNGenericVector(					double *x,			void *Context, double *Result );

#endif

