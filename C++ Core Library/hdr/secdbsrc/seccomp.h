/****************************************************************
**
**	SECCOMP.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/seccomp.h,v 1.6 2013/10/07 19:33:17 khodod Exp $
**
****************************************************************/

#if !defined( IN_SECCOMP_H )
#define IN_SECCOMP_H

typedef	int (*SC_SLANG)( SLANG_ARGS );

#define	SCM_ERR(Text)		{ if( Text ) ErrMore( "%s", Text ); RetCode = SLANG_ERROR; goto SCM_ERROR_LABEL; }

#define SCM_FUNC_TO_VARIABLE( Name, VariableHash, _Argc )			\
{																	\
	SLANG_NODE														\
			TmpNode;												\
																	\
	DT_VALUE														\
			TmpVal;													\
																	\
	SLANG_DATA_TYPE													\
			*SdtSlang;												\
																	\
	memset( &TmpNode, 0, sizeof( TmpNode ));						\
	TmpNode.Argc				= (_Argc);							\
	TmpNode.Argv 				= NodeSP - (_Argc);					\
	SdtSlang					= malloc( sizeof( *SdtSlang ));		\
	SdtSlang->ReferenceCount	= 1;								\
	SdtSlang->Expression		= SlangParseTreeCopy( &TmpNode );	\
	DTM_INIT( &TmpVal );											\
	TmpVal.DataType				= DtSlang;							\
	TmpVal.Data.Pointer			= SdtSlang;							\
	if( !SlangVariableSet( Name, &TmpVal, VariableHash, 0 ))		\
		SCM_ERR( NULL );											\
	NodeSP -= (_Argc);												\
}

#define SCM_SET_VARIABLE( Name, VariableHash )						\
{																	\
	SLANG_RET														\
			*VRet;													\
																	\
	VRet = *--CtxSP;												\
	if( !SlangVariableSet( Name, &VRet->Data.Value, VariableHash, 0 )) \
		SCM_ERR( NULL );											\
}

#define SCM_COMPONENT(RRet,Comp)									\
{																	\
	SLANG_RET														\
			*VRet;													\
																	\
	DT_VALUE														\
			*CompPtr,												\
			CompValue;												\
								 									\
	VRet = *--CtxSP;												\
	CompPtr = DtComponentGet( &VRet->Data.Value, (Comp), NULL );	\
	if( CompPtr )													\
	{																\
		(RRet)->Type = SLANG_TYPE_STATIC_VALUE;						\
		(RRet)->Data.Value = *CompPtr;								\
	}																\
	else															\
	{																\
		(RRet)->Type = SLANG_TYPE_VALUE;							\
		DTM_INIT( &CompValue );										\
		CompValue.DataType = DtString;								\
		CompValue.Data.Pointer = (Comp);							\
		if( !DtSubscriptGetCopy( &VRet->Data.Value, &CompValue, &(RRet)->Data.Value ))\
			SCM_ERR( NULL );										\
	}																\
}
			
#define SCM_SUBSCRIPT(RRet,Sub)										\
{																	\
	SLANG_RET														\
			*VRet;													\
																	\
	DT_VALUE														\
			*CompPtr,												\
			CompValue;												\
																	\
	VRet = *--CtxSP;												\
	CompPtr = DtSubscriptGet( &(VRet)->Data.Value, (Sub), NULL );	\
	if( CompPtr )													\
	{																\
		(RRet)->Type = SLANG_TYPE_STATIC_VALUE;						\
		(RRet)->Data.Value = *CompPtr;								\
	}																\
	else															\
	{																\
		(RRet)->Type = SLANG_TYPE_VALUE;							\
		DTM_INIT( &CompValue );										\
		CompValue.DataType = DtDouble;								\
		CompValue.Data.Number = (Sub);								\
		if( !DtSubscriptGetCopy( &(VRet)->Data.Value, &CompValue, &(RRet)->Data.Value ))\
			SCM_ERR( NULL );										\
	}																\
}

#define SCM_ALLOC( RRet, Dt )										\
{																	\
	(RRet)->Type = SLANG_TYPE_VALUE;								\
	if( !DTM_ALLOC( &(RRet)->Data.Value, Dt ))						\
		SCM_ERR( NULL );											\
}

#define SCM_TRUTH(Ret)					(((Ret)->Type == SLANG_TYPE_VALUE || (Ret)->Type == SLANG_TYPE_STATIC_VALUE ) && DT_OP( DT_MSG_TRUTH, NULL, &(Ret)->Data.Value, NULL ))
#define SCM_DT_OP(Op,RRet,ARet,BRet)	DT_OP( Op, (RRet) ? &(RRet)->Data.Value : NULL, &(ARet)->Data.Value, (BRet) ? &(BRet)->Data.Value : NULL )

#define SCM_PUSH_CTX(PRet)			*CtxSP++ = PRet
#define SCM_PUSH_SLANG(Func)		{ memset( *NodeSP, 0, sizeof( **NodeSP )); (*NodeSP)->Type = SLANG_RESERVED_FUNC;	(*NodeSP)->FuncPtr = (Func); 			(*NodeSP)->Extra = CtxSP;			NodeSP++; }
#define SCM_PUSH_STRING(Str)		{ memset( *NodeSP, 0, sizeof( **NodeSP )); (*NodeSP)->Type = SLANG_TYPE_STRING;		(*NodeSP)->FuncPtr = SlangXString; 		(*NodeSP)->StringValue = (Str);		NodeSP++; }
#define SCM_PUSH_DOUBLE(Num)		{ memset( *NodeSP, 0, sizeof( **NodeSP )); (*NodeSP)->Type = SLANG_TYPE_DOUBLE;		(*NodeSP)->FuncPtr = SlangXDouble; 		(*NodeSP)->DoubleValue = (Num);		NodeSP++; }
#define SCM_PUSH_VARNODE(Name)		{ memset( *NodeSP, 0, sizeof( **NodeSP )); (*NodeSP)->Type = SLANG_TYPE_VARIABLE;	(*NodeSP)->FuncPtr = SlangXVariable; 	(*NodeSP)->StringValue = (Name);	NodeSP++; }
#define SCM_PUSH_PRET(PRet,Name)	{ memset( *NodeSP, 0, sizeof( **NodeSP )); (*NodeSP)->Type = SLANG_RESERVED_FUNC;	(*NodeSP)->FuncPtr = SlangXScPRet;		(*NodeSP)->Extra = (PRet);			(*NodeSP)->StringValue = (Name);	NodeSP++; }

#define SCM_PUSH_VARIABLE( VRet, VarName, VariableHash )			\
{																	\
	SLANG_VARIABLE													\
			*Variable;												\
																	\
	Variable = SlangVariableGet( VarName, VariableHash );		  	\
	if( Variable )													\
	{																\
		(VRet)->Type = SLANG_TYPE_STATIC_VALUE;						\
		(VRet)->Data.Value = *Variable->Value;						\
		SCM_PUSH_CTX( VRet );										\
	}																\
	else															\
	{																\
		(VRet)->Data.Value.DataType = NULL;							\
		SCM_PUSH_CTX( NULL );										\
	}																\
}

#define SCM_CALL(CallRet,FuncPtr,_Argc,Ctxc)						\
{																	\
	SLANG_RET														\
			*pRet,													\
			FakeRet;												\
	SLANG_NODE														\
			TmpNode;												\
																	\
	TmpNode.Argc = (_Argc);											\
	TmpNode.Argv = NodeSP - (_Argc);								\
		  															\
	pRet = (CallRet) ? (CallRet) : &FakeRet;						\
	RetCode = (*(FuncPtr))( &TmpNode, SLANG_EVAL_RVALUE, pRet, Scope );\
	if( pRet == &FakeRet )	 										\
		SLANG_RET_FREE( &FakeRet );									\
	if( SLANG_OK != RetCode )										\
		goto SCM_ERROR_LABEL;										\
	NodeSP -= (_Argc);												\
	CtxSP  -= (Ctxc);												\
}

#define SCM_CALL_TOP(CallRet,Ctxc)									\
{																	\
	SLANG_RET														\
			*pRet,													\
			FakeRet;												\
	SLANG_NODE														\
			*TmpNode;												\
			 														\
	TmpNode = *(--NodeSP); 											\
	pRet = (CallRet) ? (CallRet) : &FakeRet;						\
	RetCode = (*TmpNode->FuncPtr)( TmpNode, SLANG_EVAL_RVALUE, pRet, Scope );\
	if( pRet == &FakeRet )											\
		SLANG_RET_FREE( &FakeRet );									\
	if( SLANG_OK != RetCode )										\
		goto SCM_ERROR_LABEL;										\
	CtxSP  -= (Ctxc);												\
}

#define SCM_CTX_ARG(t,ArgVar)			(ArgVar) = *(t **)(CtxPtr++)


#define SCM_LOCAL(NodeDepth,CtxDepth)								\
	int		RetCode	= SLANG_OK;										\
																	\
	void	**CtxPtr = (void **)Root->Extra;						\
																	\
	SLANG_NODE														\
			NodeStack[ max(1,NodeDepth) ],							\
			*NodeArgv[ max(1,NodeDepth) ],							\
			**NodeSP = NodeArgv;									\
																	\
	void	*CtxStack[ max(1,CtxDepth) ],							\
			**CtxSP = CtxStack;
	
#define SCM_PRET_INIT					{ SLANG_TYPE_STATIC_VALUE }
#define SCM_STATIC_INIT					{ SLANG_TYPE_STATIC_VALUE, {{ NULL }}}
#define SCM_STATIC_INITIALIZED(PRet)	( (PRet)->Data.Value.DataType != NULL )
#define SCM_PREAMBLE(NodeDepth)										\
{																	\
	int		i;														\
																	\
																	\
	if( EvalFlag != SLANG_EVAL_RVALUE )								\
		return( SlangEvalError( SLANG_ERROR_PARMS, "Compiler assumed it was an rvalue" ));\
	for( i = 0; i < (NodeDepth); i++ )								\
		NodeArgv[i] = &NodeStack[i];								\
	CtxPtr;															\
}


int SlangXString(		SLANG_ARGS ),
	SlangXDouble(		SLANG_ARGS ),
	SlangXVariable(		SLANG_ARGS ),
	SlangXScPRet(		SLANG_ARGS ),
	ScSecViewErrorFunc(	void *ErrorHandle, SLANG_ARGS );
	
	
	
// FIX -- move to a .c
#include	<dtptr.h>
/****************************************************************
**	Routine: SlangXScPRet
**	Returns: Evaluation return code (SLANG_ERROR, SLANG_OK ...)
**	Action : Return Ret pointed to by Extra
****************************************************************/

int SlangXScPRet( SLANG_ARGS )
{
	SLANG_VARIABLE
			*Variable;
			
	SLANG_RET
			*PRet	= Root->Extra;


	Ret->Type = SLANG_TYPE_STATIC_VALUE;
	if( SLANG_EVAL_RVALUE == EvalFlag )
	{
		if( !PRet || !PRet->Data.Value.DataType )
			return( SlangEvalError( SLANG_ERROR_PARMS, "Access of an uninitialized variable '%s'", Root->StringValue ? Root->StringValue : "<no name>" ));
		Ret->Data.Value = PRet->Data.Value;
	}
	else 
	{
		if(( SLANG_EVAL_LVALUE_CREATE == EvalFlag || SLANG_EVAL_LVALUE_NEWREF == EvalFlag ) && Root->StringValue )
		{
			if( !PRet->Data.Value.DataType )
				PRet->Data.Value.DataType = DtNull;
			Variable = SlangVariableSet( Root->StringValue, &PRet->Data.Value, Scope->Variables->VariableHash, SLANG_VARIABLE_RETAIN | ( SLANG_EVAL_LVALUE_NEWREF == EvalFlag ? SLANG_VARIABLE_REFERENCE : 0 ));
		}
		else
			return SlangEvalError( Root, EvalFlag, Ret, Scope, "Access of an uninitialized variable '%s'", Root->StringValue ? Root->StringValue : "<no name>" );
		Ret->Type = SLANG_TYPE_VALUE;
		DtPointerVariable( &Ret->Data.Value, Variable );
	}
	return SLANG_OK;
}



/****************************************************************
**	Routine: ScSecViewErrorFunc
**	Returns: SLANG_ABORT
**	Action : Displays message
****************************************************************/

int ScSecViewErrorFunc(
	void		*ErrorHandle,
	SLANG_ARGS
)
{
	ErrMsg( ERR_LEVEL_ERROR, "%s", ErrGetString());
	return( SLANG_ABORT );
}

#endif

