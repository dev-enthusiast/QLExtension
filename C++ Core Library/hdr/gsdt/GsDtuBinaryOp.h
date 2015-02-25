/****************************************************************
**
**	GSDTUBINARYOP.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtuBinaryOp.h,v 1.8 2001/11/27 22:44:01 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTUBINARYOP_H )
#define IN_GSDT_GSDTUBINARYOP_H

#include <assert.h>
#include <gsdt/base.h>
#include <gsdt/op.h>
#include <gsdt/GsDtuBinaryFunc.h>
#include <gsdt/GsDtTypeMap.h>

CC_BEGIN_NAMESPACE( Gs )


////////////////////////////////////////////////////////////////////////
//
// The purpose of the following macros are to enable us to easily
// expose C++ operators to slang or any other wrapper environment we
// might create.
//
// They would be prettier if implemented with templates, but that
// would require creating a function templated on some classes
// that did not take any args of those classes.  Our current crop
// of compilers are pretty hopeless when it comes to doing this.
//
////////////////////////////////////////////////////////////////////////



/****************************************************************
**	Routine: GsDtuBinaryAssignOp
**	Returns: GsStatus
**	Action : Binary op on two GsDt-wrapped types.
****************************************************************/

#define GS_DTU_BINARY_ASSIGN_OP( LHT, RHT )													\
GsStatus GsDtuBinaryAssignOp ## LHT ## RHT (												\
	GsDt 		&DtLhs,																		\
	GSDT_BINOP 	Op,																			\
	const GsDt 	&DtRhs																		\
)																							\
{																							\
	assert( DtLhs.type() == GsDtTypeMap<LHT>::GsDtType::staticType() );						\
	assert( DtRhs.type() == GsDtTypeMap<RHT>::GsDtType::staticType() );						\
																							\
	LHT																						\
			&Lhs = GsDtTypeMap<LHT>::GsDtType::Cast( &DtLhs )->data();						\
																							\
	const RHT																				\
			&Rhs = GsDtTypeMap<RHT>::GsDtType::Cast( &DtRhs )->data();						\
																							\
	switch( Op )																			\
	{																						\
		case GSDTBO_ADD:																	\
			Lhs += Rhs;																		\
			break;																			\
																							\
		case GSDTBO_SUBTRACT:																\
			Lhs -= Rhs;																		\
			break;																			\
																							\
		case GSDTBO_MULTIPLY:																\
			Lhs *= Rhs;																		\
			break;																			\
																							\
		case GSDTBO_DIVIDE:																	\
			Lhs /= Rhs;																		\
			break;																			\
																							\
		default:																			\
			return GsErr( ERR_UNSUPPORTED_OPERATION, "AssignOp(%s %s %s), @",				\
						  DtLhs.typeName(), GsDtuBinOpToString( Op ), DtRhs.typeName() );	\
	}																						\
																							\
	return GS_OK;																			\
}

#define GS_DTU_BINARY_ASSIGN_OP_INITIALIZE( LHT, RHT )																								\
	new GsDtuBinaryFunc( GsDtTypeMap<LHT>::GsDtType::staticType(), GsDtTypeMap<RHT>::GsDtType::staticType(), GsDtuBinaryAssignOp ## LHT ## RHT )



/****************************************************************
**	Routine: GsDtuBinaryAssignOp
**	Returns: GsStatus
**	Action : Binary op on two GsDt types.
****************************************************************/

#define GS_DTU_BINARY_ASSIGN_OP_GSDT( LHT, RHT )											\
GsStatus GsDtuBinaryAssignOp ## LHT ## RHT (												\
	GsDt 		&DtLhs,																		\
	GSDT_BINOP 	Op,																			\
	const GsDt	&DtRhs																		\
)																							\
{																							\
	assert( DtLhs.type() == LHT::staticType() );											\
	assert( DtRhs.type() == RHT::staticType() );											\
																							\
	LHT																						\
			&Lhs = const_cast< LHT & >( LHT::Cast( DtLhs ) );								\
																							\
	const RHT																				\
			&Rhs = RHT::Cast( DtRhs );														\
																							\
	switch( Op )																			\
	{																						\
		case GSDTBO_ADD:																	\
			Lhs += Rhs;																		\
			break;																			\
																							\
		case GSDTBO_SUBTRACT:																\
			Lhs -= Rhs;																		\
			break;																			\
																							\
		case GSDTBO_MULTIPLY:																\
			Lhs *= Rhs;																		\
			break;																			\
																							\
		case GSDTBO_DIVIDE:																	\
			Lhs /= Rhs;																		\
			break;																			\
																							\
		default:																			\
			return GsErr( ERR_UNSUPPORTED_OPERATION, "AssignOp(%s %s %s), @",				\
						  DtLhs.typeName(), GsDtuBinOpToString( Op ), DtRhs.typeName() );	\
	}																						\
																							\
	return GS_OK;																			\
}

#define GS_DTU_BINARY_ASSIGN_OP_GSDT_INITIALIZE( LHT, RHT )											\
	new GsDtuBinaryFunc( LHT::staticType(), RHT::staticType(), GsDtuBinaryAssignOp ## LHT ## RHT )




/****************************************************************
**	Routine: GsDtuBinaryAssignOp
**	Returns: GsStatus
**	Action : Binary op on a GsDt and a non-gsdt.
****************************************************************/

#define GS_DTU_BINARY_ASSIGN_OP_LHT_GSDT( LHT, RHT )										\
GsStatus GsDtuBinaryAssignOp ## LHT ## RHT (												\
	GsDt 		&DtLhs,																		\
	GSDT_BINOP 	Op,																			\
	const GsDt	&DtRhs																		\
)																							\
{																							\
	assert( DtLhs.type() == LHT::staticType() );											\
	assert( DtRhs.type() == GsDtTypeMap<RHT>::GsDtType::staticType() );						\
																							\
	LHT																						\
			&Lhs = const_cast< LHT & >( LHT::Cast( DtLhs ) );								\
																							\
	const RHT																				\
			&Rhs = GsDtTypeMap<RHT>::GsDtType::Cast( &DtRhs )->data();						\
																							\
	switch( Op )																			\
	{																						\
		case GSDTBO_ADD:																	\
			Lhs += Rhs;																		\
			break;																			\
																							\
		case GSDTBO_SUBTRACT:																\
			Lhs -= Rhs;																		\
			break;																			\
																							\
		case GSDTBO_MULTIPLY:																\
			Lhs *= Rhs;																		\
			break;																			\
																							\
		case GSDTBO_DIVIDE:																	\
			Lhs /= Rhs;																		\
			break;																			\
																							\
		default:																			\
			return GsErr( ERR_UNSUPPORTED_OPERATION, "AssignOp(%s %s %s), @",				\
						  DtLhs.typeName(), GsDtuBinOpToString( Op ), DtRhs.typeName() );	\
	}																						\
																							\
	return GS_OK;																			\
}

#define GS_DTU_BINARY_ASSIGN_OP_LHT_GSDT_INITIALIZE( LHT, RHT )																\
	new GsDtuBinaryFunc( LHT::staticType(), GsDtTypeMap<RHT>::GsDtType::staticType(), GsDtuBinaryAssignOp ## LHT ## RHT )



// A binary op that returns an arbitrary type.

#define GS_DTU_BINARY_OP( REST, LHT, RHT )													\
GsDt *GsDtuBinaryOp ## LHT ## RHT (															\
	const GsDt	&DtLhs,																		\
	GSDT_BINOP	Op,																			\
	const GsDt	&DtRhs																		\
)																							\
{																							\
	assert( DtLhs.type() == GsDtTypeMap< LHT >::GsDtType::staticType() );					\
	assert( DtRhs.type() == GsDtTypeMap< RHT >::GsDtType::staticType() );					\
																							\
	LHT																				\
			Lhs = GsDtTypeMap< LHT >::GsDtType::Cast( &DtLhs )->data();					\
																							\
	RHT																				\
			Rhs = GsDtTypeMap< RHT >::GsDtType::Cast( &DtRhs )->data();					\
																							\
	REST	Result;																			\
																							\
	switch( Op )																			\
	{																						\
		case GSDTBO_ADD:																	\
			Result = Lhs + Rhs;																\
			break;																			\
																							\
		case GSDTBO_SUBTRACT:																\
			Result = Lhs - Rhs;																\
			break;																			\
																							\
		case GSDTBO_MULTIPLY:																\
			Result = Lhs * Rhs;																\
			break;																			\
																							\
		case GSDTBO_DIVIDE:																	\
			Result = Lhs / Rhs;																\
			break;																			\
																							\
		default:																			\
			GsErr( ERR_UNSUPPORTED_OPERATION, "Op(%s %s %s), @",							\
				   DtLhs.typeName(), GsDtuBinOpToString( Op ), DtRhs.typeName() );			\
			return NULL;																	\
	}																						\
																							\
																							\
	return new GsDtTypeMap< REST >::GsDtType( Result );										\
}

#define GS_DTU_BINARY_OP_INITIALIZE( LHT, RHT )							\
	new GsDtuBinaryFunc( GsDtTypeMap< LHT >::GsDtType::staticType(),	\
						 GsDtTypeMap< RHT >::GsDtType::staticType(),	\
						 NULL, GsDtuBinaryOp ## LHT ## RHT )




CC_END_NAMESPACE

#endif 

