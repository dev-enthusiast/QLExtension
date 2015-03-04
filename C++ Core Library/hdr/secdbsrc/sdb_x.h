/****************************************************************
**
**	SDB_X.H	- Slang prototypes
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdb_x.h,v 1.33 2013/08/13 05:30:08 khodod Exp $
**
****************************************************************/

#if !defined( IN_SDB_X_H )
#define IN_SDB_X_H


/*
**	Max args to SlangXSubscript
*/

#define SLANG_SUBSCRIPT_MAX_ARGS			20



/*
**	Prototype functions
*/

EXPORT_CPP_SECDB SLANG_RET_CODE
		SlangXTry(				SLANG_ARGS );

EXPORT_C_SECDB SLANG_RET_CODE
		SlangXAssign( 			SLANG_ARGS ),
		SlangXOperator(			SLANG_ARGS ),
		SlangXLogicalOr(		SLANG_ARGS ),
		SlangXLogicalAnd(		SLANG_ARGS ),
		SlangXUnaryNot(			SLANG_ARGS ),
		SlangXUnaryMinus(		SLANG_ARGS ),

		SlangXPreIncrement(		SLANG_ARGS ),
		SlangXPreDecrement(		SLANG_ARGS ),
		SlangXPostIncrement(	SLANG_ARGS ),
		SlangXPostDecrement(	SLANG_ARGS ),

		SlangXValueMethod(		SLANG_ARGS ),
		SlangXValueMethodFunc(	SLANG_ARGS ),
		SlangXDataTypeCreator(	SLANG_ARGS ),

		SlangXIf(				SLANG_ARGS ),
		SlangXThrow(			SLANG_ARGS ),
		SlangXBlock(			SLANG_ARGS ),
		SlangXGlobal(			SLANG_ARGS ),
		SlangXDoGlobal(			SLANG_ARGS ),
		SlangXSticky(			SLANG_ARGS ),
		SlangXStickyDiddle(		SLANG_ARGS ),
		SlangXUseDatabase(		SLANG_ARGS ),
		SlangXWhile(			SLANG_ARGS ),

		SlangXLink(				SLANG_ARGS ),
		SlangXSmartLinkEnable(	SLANG_ARGS ),
		SlangXLinkFile(			SLANG_ARGS ),
		SlangXLinkLog(			SLANG_ARGS ),
		SlangXConstant(			SLANG_ARGS ),
		SlangXEnum(				SLANG_ARGS ),

		SlangXArrayInit(		SLANG_ARGS ),

		SlangXScopeOperator(	SLANG_ARGS ),

		SlangXColonAssign(		SLANG_ARGS ),
		SlangXDollar( 			SLANG_ARGS ),
                

		SlangXSymbolStyleTsdb(	SLANG_ARGS );


// From x_base.c
EXPORT_C_SECDB SLANG_RET_CODE
		SlangXComponent(		SLANG_ARGS ),
		SlangXConstantNode(		SLANG_ARGS ),
		SlangXEvalOnce(			SLANG_ARGS ),
		SlangXNull(				SLANG_ARGS ),
		SlangXStar(				SLANG_ARGS ),
		SlangXSubscript(		SLANG_ARGS ),
		SlangXVariable(			SLANG_ARGS ),
		SlangXVariableIndex(	SLANG_ARGS );

EXPORT_C_SECDB SLANG_RET_CODE
		SlangXGsDt(				SLANG_ARGS ),
		SlangXGsDtCast(			SLANG_ARGS ),
		SlangXGsDtToDtValue(	SLANG_ARGS ),
		SlangXGsDtTypeOf(		SLANG_ARGS ),
		SlangXGsDtTypes(		SLANG_ARGS ),
		SlangXBinaryAtOp(		SLANG_ARGS ),
		SlangXDataTypeOf(		SLANG_ARGS );

#endif




