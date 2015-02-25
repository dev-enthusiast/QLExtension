/****************************************************************
**
**	gob.h
**
**	$Header: /home/cvs/src/gob/src/gob.h,v 1.147 2001/11/27 22:31:24 procmon Exp $
**
****************************************************************/

#ifndef IN_GOB_H
#define IN_GOB_H

#include	<stddef.h>
#if !defined( IN_SECOBJ_H)
#include	<secobj.h>
#endif
#if !defined( IN_GOB_BASE_H )
#include	<gob_base.h>
#endif

/*
**	Forward declare structures
*/

union SDB_M_DATA;
struct SDB_CHILD_REF;


/*
**	Parameters of GobSecurityFromStructure
*/

#define		GOB_FLAG_IGNORE_WARNINGS 	0x0002	// If not set, fail if there are any validation warnings
#define		GOB_FLAG_IGNORE_ERRORS		0x0004	// If not set, fail if there are any validation errors
#define		GOB_FLAG_DONT_COMMIT		0x0008	// If not set, don't commit to the database
#define		GOB_FLAG_COERCE_TYPES		0X0010  // If set, then do type coercion when setting values

enum GOB_IF_EXISTS
{
	GOB_IF_EXISTS_FAIL,			// Leave existing object untouched
	GOB_IF_EXISTS_UPDATE,		// Set values of existing object
	GOB_IF_EXISTS_REPLACE,		// Delete existing object and recreate
	GOB_IF_EXISTS_TRY_UPDATE	// Try UPDATE, but if it fails do REPLACE

};

// Error constant
#define		ERR_GOB_VALIDATION		8192

/*
**	Constants for GOB_ARG::* for projects which do not compile in C++ mode 
**	These *MUST* be the same as the constants in gobarg.h
*/

#define	GOB_ARG_PASS_ERRORS			1
#define	GOB_ARG_DYNAMIC				2

/*
**	Helper macros for CREATE_CHILD_LIST:
**
**	Assumed Variables:
**
**		SecPtr			The current security
**
**		ValueTypeInfo	The SDB_VALUE_TYPE_INFO *
**
**		Status			An int status variable initialized to TRUE
**
**		ValueMap		An array of GOB_VALUE_MAPs of which the ValueName
**						macro parameters are indices
**
**		TempValue		An DT_VALUE with TempValue.Data containing the results
**
**	Assumed Type:
**
**		CHILD_DATA		The type of the structure of containing the child data for the value
**	
**	
**	Key:
**		GOB_CHILD_tA	Child is just a default value for an argument
**		GOB_CHILD_t1	Child with one arg, e.g., Spot( Cross( Self ), Expiration Date( Self ))
**		GOB_CHILD_t2	Child with two args, e.g., Volatility( Cross( Self ), Expiration Date( Self ), Spot Date( Self ))
**		GOB_CHILD_tI	Indirect child, e.g., (Volatility Method( Self ))( Self )
**		GOB_CHILD_tPE	Pass Errors, e.g., Spot( Children( Self )) where Children( Self ) may not all exist
*/

// Literal children, e.g. Pricing Date( "Security Database" )
#define	GOB_CHILD_L(  	ValueName, ResultsTag, ObjectName )						( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, NULL ))
#define	GOB_CHILD_LA(	ValueName, ResultsTag, ObjectName )						( Status = Status && GobChildLiteral( &MsgData->GetChildList, TRUE,  ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, NULL ))
#define	GOB_CHILD_L1( 	ValueName, ResultsTag, ObjectName, Arg1_ )				( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), NULL ))
#define	GOB_CHILD_L2( 	ValueName, ResultsTag, ObjectName, Arg1_, Arg2_ )		( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), NULL ))
#define	GOB_CHILD_L3( 	ValueName, ResultsTag, ObjectName, Arg1_, Arg2_, Arg3_ )	( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), (Arg3_), NULL ))
#define	GOB_CHILD_L4( 	ValueName, ResultsTag, ObjectName, Arg1_, Arg2_, Arg3_, Arg4_ )		( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), (Arg3_), (Arg4_), NULL ))
#define	GOB_CHILD_L5( 	ValueName, ResultsTag, ObjectName, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_ )		( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), NULL ))
#define	GOB_CHILD_L7( 	ValueName, ResultsTag, ObjectName, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_, Arg7_ )	( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), (Arg6_), (Arg7_), NULL ))
#define	GOB_CHILD_L8( 	ValueName, ResultsTag, ObjectName, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_, Arg7_, Arg8_ )		( Status = Status && GobChildLiteral( &MsgData->GetChildList, FALSE, ValueName, ObjectName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), (Arg6_), (Arg7_), (Arg8_), NULL ))

// Reflexive children, e.g., Price( Self )
#define	GOB_CHILD_R(  	ValueName, ResultsTag )									( Status = Status && GobChildSelf( &MsgData->GetChildList, FALSE, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , NULL ))
#define	GOB_CHILD_RA( 	ValueName, ResultsTag )									( Status = Status && GobChildSelf( &MsgData->GetChildList, TRUE,  ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , NULL ))
#define GOB_CHILD_A1(	ValueName, ResultsTag, Arg1_ )							( Status = Status && GobChildSelf( &MsgData->GetChildList, TRUE,  ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , (Arg1_), NULL ))
#define	GOB_CHILD_R1( 	ValueName, ResultsTag, Arg1_ )							( Status = Status && GobChildSelf( &MsgData->GetChildList, FALSE, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , (Arg1_), NULL ))
#define	GOB_CHILD_R2( 	ValueName, ResultsTag, Arg1_, Arg2_ )					( Status = Status && GobChildSelf( &MsgData->GetChildList, FALSE, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , (Arg1_), (Arg2_), NULL ))
#define	GOB_CHILD_R3( 	ValueName, ResultsTag, Arg1_, Arg2_, Arg3_ )            ( Status = Status && GobChildSelf( &MsgData->GetChildList, FALSE, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , (Arg1_), (Arg2_), (Arg3_), NULL ))
#define	GOB_CHILD_R4( 	ValueName, ResultsTag, Arg1_, Arg2_, Arg3_, Arg4_ )    	( Status = Status && GobChildSelf( &MsgData->GetChildList, FALSE, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , (Arg1_), (Arg2_), (Arg3_), (Arg4_), NULL ))
#define	GOB_CHILD_R5( 	ValueName, ResultsTag, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_ )		( Status = Status && GobChildSelf( &MsgData->GetChildList, FALSE, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), NULL ))
#define	GOB_CHILD_R6( 	ValueName, ResultsTag, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_ )		( Status = Status && GobChildSelf( &MsgData->GetChildList, FALSE, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), (Arg6_), NULL ))
#define	GOB_CHILD_RI( 	ValueName, ResultsTag )									( Status = Status && GobChildSelfIndirect( &MsgData->GetChildList, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, NULL ))
#define	GOB_CHILD_RI1(	ValueName, ResultsTag, Arg1_ )		 					( Status = Status && GobChildSelfIndirect( &MsgData->GetChildList, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), NULL ))
#define	GOB_CHILD_RI2(	ValueName, ResultsTag, Arg1_, Arg2_ )					( Status = Status && GobChildSelfIndirect( &MsgData->GetChildList, ValueName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), NULL ))
#define	GOB_CHILD_A(  	ValueName, ResultsTag )									GOB_CHILD_RA(ValueName,ResultsTag)

// Value children, e.g., Spot( Cross( Self ))
#define	GOB_CHILD_V(  	ValueName, ResultsTag, ObjectValName )					( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, FALSE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , NULL ))
#define	GOB_CHILD_VA( 	ValueName, ResultsTag, ObjectValName )					( Status = Status && GobChildIntermediate( &MsgData->GetChildList, TRUE,  FALSE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , NULL ))
#define	GOB_CHILD_V1( 	ValueName, ResultsTag, ObjectValName, Arg1_ )						( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, FALSE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (Arg1_), NULL ))
#define	GOB_CHILD_V2( 	ValueName, ResultsTag, ObjectValName, Arg1_, Arg2_ )				( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, FALSE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (Arg1_), (Arg2_), NULL ))
#define	GOB_CHILD_V3( 	ValueName, ResultsTag, ObjectValName, Arg1_, Arg2_, Arg3_) 			( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, FALSE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (Arg1_), (Arg2_), (Arg3_ ), NULL ))
#define	GOB_CHILD_V4( 	ValueName, ResultsTag, ObjectValName, Arg1_, Arg2_, Arg3_, Arg4_) 	( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, FALSE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (Arg1_), (Arg2_), (Arg3_ ), (Arg4_), NULL ))
#define	GOB_CHILD_V5(   ValueName, ResultsTag, ObjectValName, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_ ) 	( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, FALSE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (Arg1_), (Arg2_), (Arg3_ ), (Arg4_), (Arg5_ ), NULL ))
#define	GOB_CHILD_VPE(	ValueName, ResultsTag, ObjectValName )					( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, TRUE,  ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag , NULL ))
#define GOB_CHILD_VPE1( ValueName, ResultsTag, ObjectValName, Arg1_ )                           ( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, TRUE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), NULL ))
#define	GOB_CHILD_VPE2( ValueName, ResultsTag, ObjectValName, Arg1_, Arg2_ )					( Status = Status && GobChildIntermediate( &MsgData->GetChildList, FALSE, TRUE, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (Arg1_), (Arg2_), NULL ))
#define	GOB_CHILD_VI( 	ValueName, ResultsTag, ObjectValName )					( Status = Status && GobChildIntermediateIndirect( &MsgData->GetChildList, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, NULL ))
#define	GOB_CHILD_VI1(	ValueName, ResultsTag, ObjectValName, Arg1_ )			( Status = Status && GobChildIntermediateIndirect( &MsgData->GetChildList, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), NULL ))
#define	GOB_CHILD_VI2(	ValueName, ResultsTag, ObjectValName, Arg1_, Arg2_ )	( Status = Status && GobChildIntermediateIndirect( &MsgData->GetChildList, ValueName, ObjectValName, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, (Arg1_), (Arg2_), NULL ))

// Arbitrary children, e.g., Spot( Cross( Self, Pricing Date( Self )), Arg1 )
#define	GOB_CHILD_ANY(	ResultsTag, pDataType, RealChild )						( Status = Status && GobChildAny( &MsgData->GetChildList, FALSE, FALSE, (pDataType), offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (RealChild) ))
#define	GOB_CHILD_ANYA(	ResultsTag, pDataType, RealChild )						( Status = Status && GobChildAny( &MsgData->GetChildList, TRUE,  FALSE, (pDataType), offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (RealChild) ))
#define	GOB_CHILD_ANYE(	ResultsTag, pDataType, RealChild )						( Status = Status && GobChildAny( &MsgData->GetChildList, FALSE, TRUE,  (pDataType), offsetof( CHILD_DATA, ResultsTag), #ResultsTag , (RealChild) ))

// Constant children, e.g., Date( "3May59" )
#define	GOB_CHILD_CN(	ResultsTag, NumericConst, DataType )					( Status = Status && GobChildConstant( &MsgData->GetChildList, FALSE, NumericConst, NULL, &DataType, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag ))
#define	GOB_CHILD_CNA( 	ResultsTag, NumericConst, DataType )					( Status = Status && GobChildConstant( &MsgData->GetChildList, TRUE,  NumericConst, NULL, &DataType, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag ))
#define	GOB_CHILD_CP(  	ResultsTag, PointerConst, DataType )					( Status = Status && GobChildConstant( &MsgData->GetChildList, FALSE, 0, PointerConst, &DataType, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag ))
#define	GOB_CHILD_CPA( 	ResultsTag, PointerConst, DataType )					( Status = Status && GobChildConstant( &MsgData->GetChildList, TRUE,  0, PointerConst, &DataType, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag ))

#define	GOB_CHILD_ELLIPSIS( ResultsTag )										( Status = Status && GobChildEllipsis( &MsgData->GetChildList, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag ))
#define	GOB_CHILD_ELLIPSIS_SIZE( ResultsTag )									( Status = Status && GobChildEllipsisSize( &MsgData->GetChildList, offsetof( CHILD_DATA, ResultsTag ), #ResultsTag ))

// Arguments for GOB_CHILD_t1 and GOB_CHILD_t2
#define GOB_ARG_SELF_OBJ()														GobArgA( -1, FALSE )
#define GOB_ARG_SELF_VT()														GobArgA( -2, FALSE )
#define GOB_ARG_A(		ArgNumber, UseDefaultIfMissing )						GobArgA( (ArgNumber), (UseDefaultIfMissing) )
#define GOB_ARG_ANY(	ValueRef, ObjectRef )									GobArgAny( (ValueRef), (ObjectRef), NULL )
#define GOB_ARG_ANY1(	ValueRef, ObjectRef, Arg1_ )							GobArgAny( (ValueRef), (ObjectRef), (Arg1_), NULL )
#define GOB_ARG_ANY2(	ValueRef, ObjectRef, Arg1_, Arg2_ )						GobArgAny( (ValueRef), (ObjectRef), (Arg1_), (Arg2_), NULL )
#define GOB_ARG_ANY3(	ValueRef, ObjectRef, Arg1_, Arg2_, Arg3_ )				GobArgAny( (ValueRef), (ObjectRef), (Arg1_), (Arg2_), (Arg3_), NULL )
#define GOB_ARG_ANY4(	ValueRef, ObjectRef, Arg1_, Arg2_, Arg3_, Arg4_ )		GobArgAny( (ValueRef), (ObjectRef), (Arg1_), (Arg2_), (Arg3_), (Arg4_), NULL )
#define GOB_ARG_ANY5(	ValueRef, ObjectRef, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_ )		GobArgAny( (ValueRef), (ObjectRef), (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), NULL )
#define GOB_ARG_ANY6(	ValueRef, ObjectRef, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_ )		GobArgAny( (ValueRef), (ObjectRef), (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), (Arg6_), NULL )
#define GOB_ARG_ANY7(	ValueRef, ObjectRef, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_, Arg7_ )		GobArgAny( (ValueRef), (ObjectRef), (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), (Arg6_), (Arg7_), NULL )
#define GOB_ARG_ANY8(	ValueRef, ObjectRef, Arg1_, Arg2_, Arg3_, Arg4_, Arg5_, Arg6_, Arg7_, Arg8_ )		GobArgAny( (ValueRef), (ObjectRef), (Arg1_), (Arg2_), (Arg3_), (Arg4_), (Arg5_), (Arg6_), (Arg7_), (Arg8_), NULL )
#define GOB_ARG_EACH(	AnyRef )												GobArgCast( (AnyRef), DtEach )
#define GOB_ARG_CAST(	AnyRef, DataType )										GobArgCast( (AnyRef), (DataType) )
#define GOB_ARG_CAST_NO_EVAL( AnyRef, DataType )								GobArgCastNoEval( (AnyRef), (DataType) )
#define GOB_ARG_CN( 	NumericConst, DataType )								GobArgCNum( (DataType), (NumericConst) )
#define GOB_ARG_CP( 	PointerConst, DataType ) 								GobArgCPtr( (DataType), (PointerConst), FALSE )
#define GOB_ARG_COPY_CP(PointerConst, DataType ) 								GobArgCPtr( (DataType), (PointerConst), TRUE )
#define GOB_ARG_CStr( 	StringConst )											GobArgCStr( (StringConst), FALSE )
#define GOB_ARG_CVt( 	VtConst )												GobArgCVt( (VtConst) )
#define GOB_ARG_L(  	ValueName, ObjName )									GobArgL( (ValueName), (ObjName), NULL )
#define GOB_ARG_L1( 	ValueName, ObjName, Arg1_ )		   						GobArgL( (ValueName), (ObjName), (Arg1_), NULL )
#define GOB_ARG_L2( 	ValueName, ObjName, Arg1_, Arg2_ ) 						GobArgL( (ValueName), (ObjName), (Arg1_), (Arg2_), NULL )
#define GOB_ARG_L3( 	ValueName, ObjName, Arg1_, Arg2_, Arg3_ ) 		        GobArgL( (ValueName), (ObjName), (Arg1_), (Arg2_), (Arg3_), NULL )
#define GOB_ARG_L4( 	ValueName, ObjName, Arg1_, Arg2_, Arg3_, Arg4_ ) 		GobArgL( (ValueName), (ObjName), (Arg1_), (Arg2_), (Arg3_), (Arg4_), NULL )
#define GOB_ARG_R(  	ValueName )												GobArgL( (ValueName), NULL, NULL )
#define GOB_ARG_R1( 	ValueName, Arg1_ )										GobArgL( (ValueName), NULL, (Arg1_), NULL )
#define GOB_ARG_R2( 	ValueName, Arg1_, Arg2_ )								GobArgL( (ValueName), NULL, (Arg1_), (Arg2_), NULL )
#define GOB_ARG_R3( 	ValueName, Arg1_, Arg2_, Arg3_ )						GobArgL( (ValueName), NULL, (Arg1_), (Arg2_), (Arg3_), NULL )
#define GOB_ARG_R4( 	ValueName, Arg1_, Arg2_, Arg3_, Arg4_ )					GobArgL( (ValueName), NULL, (Arg1_), (Arg2_), (Arg3_), (Arg4_), NULL )
#define GOB_ARG_V(  	ValueName, ObjValName )									GobArgV( (ValueName), (ObjValName), NULL )
#define GOB_ARG_V1( 	ValueName, ObjValName, Arg1_ )							GobArgV( (ValueName), (ObjValName), (Arg1_), NULL )
#define GOB_ARG_V2( 	ValueName, ObjValName, Arg1_, Arg2_ )					GobArgV( (ValueName), (ObjValName), (Arg1_), (Arg2_), NULL )
#define GOB_ARG_NULL()															GOB_ARG_CN( 0, DtNull )
#define GOB_ARG_NO_EVAL(Arg_ )													GobArgNoEval( (Arg_), FALSE )
#define GOB_ARG_NO_EVAL_CONTAGIOUS(Arg_)										GobArgNoEval( (Arg_), TRUE )
#define	GOB_ARG_ELLIPSIS()														GobArgEllipsis()
#define	GOB_ARG_ELLIPSIS_SIZE()													GobArgEllipsisSize()
#define GOB_ARG_DIDDLE_SCOPE()													GobArgDiddleScope()

#define	GOB_CHILD_LIST_CHECK									if( !Status || !GobCheckChildList( &ValueTypeInfo->ChildList, sizeof( CHILD_DATA ))) return( FALSE )
#define	GOB_CHILD_LIST_PASS_ERRORS								if( Status ) ValueTypeInfo->ChildList.PassErrors = TRUE


/*
**	Helper macros for GET_VALUE:
*/

#define		GOB_CHILD_DATA0		( MsgData->GetValue.ChildData )
#define		GOB_CHILD_DATA		( (CHILD_DATA *) GOB_CHILD_DATA0 )

#define		GOB_ASSIGN														\
\
				TempValue.DataType = MsgData->GetValue.Value->DataType;		\
				DTM_ASSIGN( MsgData->GetValue.Value, &TempValue )

#define		GOB_SET_POINTER(P)	MsgData->GetValue.Value->Data.Pointer = P

#define		GOB_SET_NUMBER(N)	MsgData->GetValue.Value->Data.Number = N


/*
**	Standard local variables for a value type function
*/

#define		GOB_LOCAL_NO_TEMP												\
\
				CHILD_DATA													\
						*ChildData;											\
																			\
				int		Status	= TRUE

#define		GOB_LOCAL														\
\
				GOB_LOCAL_NO_TEMP;											\
																			\
				DT_VALUE													\
						TempValue


/*
**	Value type function arguments (from secdb.h)
*/

#define 	GOB_VALUE_FUNC_ARGS	 											\
\
				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo

/*
** For passing them on down to another function.
*/
#define     GOB_VALUE_FUNC_ARG_NAMES 	\
SecPtr, Msg, MsgData, ValueTypeInfo

/*
**	Inheritance
*/

#define		GOB_INHERIT_CLASS(Class)					{ Class, NULL, NULL, NULL,		GOB_INHERIT_CLASS_FLAG,                  SDB_CHILD_LIST_NULL, NULL, NULL, NULL, NULL }
#define		GOB_INHERIT_INTERFACE(Class)				{ Class, NULL, NULL, NULL,		GOB_INHERIT_CLASS_FLAG,                  SDB_CHILD_LIST_NULL, NULL, NULL, NULL, NULL }
#define		GOB_IMPLEMENT_INTERFACE(Class, Interface)	{ Class, NULL, NULL, Interface, GOB_IMPLEMENT_INTERFACE_FLAG,			 SDB_CHILD_LIST_NULL, NULL, NULL, NULL, NULL }
#define		GOB_DISINHERIT_INTERFACE(Class, Interface)	{ Class, NULL, NULL, Interface, GOB_DISINHERIT_INTERFACE_FLAG,           SDB_CHILD_LIST_NULL, NULL, NULL, NULL, NULL }
#define		GOB_INHERIT_VALUE(Class,Value)				{ Class, NULL, NULL, Value,		GOB_INHERIT_CLASS_FLAG | GOB_INHERIT_VT, SDB_CHILD_LIST_NULL, NULL, NULL, NULL, NULL }
#define		GOB_INHERIT_AS(Class,Value,NewName)			GOB_INHERIT_VALUE( Class, Value ";" NewName )

#define		GOB_INHERIT									NULL, NULL, NULL, GOB_INHERIT_VT,    SDB_CHILD_LIST_NULL, NULL, NULL, NULL, NULL
#define		GOB_DISINHERIT								NULL, NULL, NULL, GOB_DISINHERIT_VT, SDB_CHILD_LIST_NULL, NULL, NULL, NULL, NULL


/*
**	Misc. SecDb additions
*/

#define		GOB_MAX_MEM_SIZE		(32767 - sizeof( SDB_DISK ))

#define     GOB_ARRAY_CHILDREN /* artifact of dynamic numbers, pointers, strings removal */

#define		GOB_VALUE_TYPE_INFO(SecPtr,ValueType)							\
\
				( SecPtr->ValueTypeInfoTable && (ValueType) ? SecPtr->ValueTypeInfoTable[ (ValueType)->ID ] : NULL )

struct GOB_VT_CHILD_INFO
{
	char	*ValueName,
			*ChildName;

};


struct GOB_VT_COMPONENT_INFO
{
	char	*ValueName,
			*Component;

};

struct GOB_VT_SORT_TABLE_INFO
{
	char	*ValueName,
			**SortOrder;

};


/*
**	Gob value type argument support (opaque structure, only in gobchild.c)
*/

struct GOB_ARG;


/*
**	Define special ValueData structure for GobValueFuncElement
*/

struct GOB_ELEMENT
{
	const char
			*ValueTypeName;		// Name of array value type

	int		ElementNumber;		// Number of element

	const void
			*DefaultValue;		// Acts like value data for normal thing

	SDB_VALUE_TYPE
			ValueType;			// Filled in first time used

};


/*
**	Define special ValueData structure for GobValueFuncImpliedName
*/

struct GOB_CD_IMPLIED_NAME
{
	DT_VALUE_RESULTS
			ValueDump;			// All SDB_IN_STREAM values get written here

};


/*
**	Define special ValueData structure for Arrays
**		(used by GobValueFuncDefault)
*/

struct GOB_ARRAY
{
	char	*ElementTypeName;	// NULL = any type

	int		MinLength,
			MaxLength;			// Set them the same for fixed length

	SDB_VALUE_TYPE
			ElementValueType;	// Filled in first time used

};


/*
**	Stuff to support GobValueFuncWhere
*/

struct GOB_WHERE_INFO
{
							// FirstElement			Others
	char	*ChildValue,	// Value to get			Value to be substituted
			*MyValue;		// Child to get from	with this value of myself

	// These values managed by GobValueFuncWhere

	SDB_VALUE_TYPE
			ChildValueType,	// Filled in by GobValueFuncWhere
			MyValueType;	// ditto

	int		Used;			// TRUE if replacement has been done

};


/*
**	Data structure for initializing Methods structures
*/

struct GOB_HASH_PAIR
{
	char	*Key,
			*Value;
		
};


/*
**	Macros that used to be functions
*/

#define GobCacheValue( SecPtr, ValueName, Value )	SecDbSetValue( (SecPtr), SecDbValueTypeFromName( (ValueName), NULL ), (Value), SDB_SET_NO_MESSAGE )
#define GobClearRetainedValue( SecPtr, ValueName )	SecDbInvalidate( (SecPtr), SecDbValueTypeFromName( (ValueName), NULL ), 0, NULL )
#define GobGetCacheFlags( SecPtr, Type )			SecDbGetFlags( (SecPtr), (Type), 0, NULL )
#define GobRemoveCachedValue( SecPtr, ValueName )	SecDbInvalidate( (SecPtr), SecDbValueTypeFromName( (ValueName), NULL ), 0, NULL )
#define GobSetValue( SecPtr, ValueName, Value )		SecDbSetValue( (SecPtr), SecDbValueTypeFromName( (ValueName), NULL ), (Value), 0 )
#define GobSetValueByType( SecPtr, Type, Value )	SecDbSetValue( (SecPtr), (Type), (Value), 0 )
#define GobSetValueCoerce( SecPtr, ValueName, Value )	SecDbSetValue( (SecPtr), SecDbValueTypeFromName( (ValueName), NULL ), (Value), SDB_SET_COERCE_TYPES )

#define GobSetPointer( SecPtr, ValueTypeName, Pointer )	GobSetPointerWithFlags( (SecPtr), (ValueTypeName), (Pointer), 0 )
#define GobSetString( SecPtr, ValueTypeName, Pointer )	GobSetStringWithFlags( (SecPtr), (ValueTypeName), (Pointer), 0 )


/*
**	Declare function prototypes
*/

// GOBOF.C

EXPORT_C_GOB int
		GobObjFunc					( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ),
		GobObjFuncAlias				( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ), 
		GobObjFuncSameValueTable	( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ), 
		GobObjFuncHandleRename		( SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, int CompareInstreams ),
		GobObjFuncLoadVersion		( SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *VersionTable ),
		GobMemoryUsage				( DT_VALUE *ResultsStructure );


// GOBOPS.C

EXPORT_C_GOB void
	 	GobValidateCross			( const char *Ccy1Ccy2, double Value, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType );

EXPORT_C_GOB int
		GobGetValue					( SDB_OBJECT *SecPtr, const char *ValueTypeName, DT_VALUE *Value ),
		GobGetValueByType			( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE Type, DT_VALUE *Value );

EXPORT_C_GOB double
		GobGetDouble				( SDB_OBJECT *SecPtr, const char *ValueTypeName );

EXPORT_C_GOB const void
		*GobGetPointer				( SDB_OBJECT *SecPtr, const char *ValueTypeName, const void *Default );

EXPORT_C_GOB char
		*GobGetString				( SDB_OBJECT *SecPtr, const char *ValueTypeName );

EXPORT_C_GOB int
		GobCacheDouble				( SDB_OBJECT *SecPtr, const char *ValueTypeName, double Number		),
		GobCacheString				( SDB_OBJECT *SecPtr, const char *ValueTypeName, const char *String	),
		GobSetDouble				( SDB_OBJECT *SecPtr, const char *ValueTypeName, double Number		),
		GobSetPointerWithFlags		( SDB_OBJECT *SecPtr, const char *ValueTypeName, const void *Pointer, int Flags ),
		GobSetStringWithFlags		( SDB_OBJECT *SecPtr, const char *ValueTypeName, const char *String, int Flags );
		
EXPORT_C_GOB SDB_OBJECT
		*GobFillSecPtr				( SDB_OBJECT *SecPtr, const char *SecName );

EXPORT_C_GOB int
		GobValueTypeDescription		( SDB_OBJECT *SecPtr, const char *ValueName, SDB_VALUE_TYPE_INFO *Vti, DT_VALUE *Ret );


// GOBCHILD.C

EXPORT_C_GOB GOB_ARG
		*GobArgA					( int ArgNumber, int UseDefault ),
		*GobArgSelfObj				( void ),
		*GobArgSelfVt				( void ),
		*GobArgAny					( GOB_ARG *ValueRef, GOB_ARG *ObjectRef, ... ),
		*GobArgAnyList				( GOB_ARG *ValueRef, GOB_ARG *ObjectRef, GOB_ARG *ArgList ),
		*GobArgCast					( GOB_ARG *AnyRef, DT_DATA_TYPE DataType ),
		*GobArgCastNoEval			( GOB_ARG *AnyRef, DT_DATA_TYPE DataType ),
		*GobArgCNum					( DT_DATA_TYPE DataType, double Number ),
		*GobArgCPtr					( DT_DATA_TYPE DataType, const void *Pointer, int Copy ),
		*GobArgCStr					( const char *String, int Copy ),
		*GobArgCVt					( SDB_VALUE_TYPE ValueType ),
		*GobArgL					( const char *ValueName, const char *ObjName, ... ),
		*GobArgV					( const char *ValueName, const char *ObjValName, ... /* GOB_ARG* (end with NULL) */ ),
		*GobArgEllipsis				( void ),
		*GobArgEllipsisSize			( void ),
		*GobArgDiddleScope			( void );


/*
**	GobArgError is returned on error, not NULL.
**	This allows proper cleanup.
*/

EXPORT_C_GOB GOB_ARG
		*GobArgError;


/*
**	Unfortunately we have to define these C wrappers for C++ members in order to easily export
**	to OS/2
*/

EXPORT_C_GOB void
		GobArgDelete				( GOB_ARG * ),
		GobArgAppend				( GOB_ARG *List, GOB_ARG *Arg ),
		GobArgDbRefSet				( GOB_ARG *Arg, GOB_ARG *Db );

EXPORT_C_GOB GOB_ARG
		*GobArgArgs					( GOB_ARG * ),
		*GobArgNextArg				( GOB_ARG * ),
		*GobArgNoEval				( GOB_ARG *Arg, int Contagious );

EXPORT_C_GOB int
		GobArgToChildRef			( GOB_ARG *Arg, SDB_M_GET_CHILD_LIST *ChildList, SDB_CHILD_COMPONENT *ChildComp, SDB_CHILD_ITEM_TYPE OuterType, const char *Name, int Flags );

EXPORT_C_GOB int
		GobChildAny					( SDB_M_GET_CHILD_LIST *ChildList, int IsArg, int PassErrors, DT_DATA_TYPE *pDataType, int Offset, const char *Name, GOB_ARG *RealChild ),
		GobChildConstant			( SDB_M_GET_CHILD_LIST *ChildList, int IsArg, double NumericConst, const void *OrPointerConst, DT_DATA_TYPE *DataTypePointer, int Offset, const char *Name ),
		GobChildLiteral				( SDB_M_GET_CHILD_LIST *ChildList, int IsArg, const char *ValueName, const char *ObjectName, int Offset, const char *Name, ... ),
		GobChildIntermediate		( SDB_M_GET_CHILD_LIST *ChildList, int IsArg, int PassErrors, const char *ValueName, const char *ObjectValueName, int Offset, const char *Name, ... ),
		GobChildSelf				( SDB_M_GET_CHILD_LIST *ChildList, int IsArg, const char *ValueName, int Offset, const char *Name, ... ),
		GobChildSelfIndirect		( SDB_M_GET_CHILD_LIST *ChildList, const char *ValueName, int Offset, const char *Name, ... ),
		GobChildIntermediateIndirect( SDB_M_GET_CHILD_LIST *ChildList, const char *ValueName, const char *ObjectValueName, int Offset, const char *Name, ... ),
		GobChildEllipsis			( SDB_M_GET_CHILD_LIST *ChildList, int Offset, const char *Name ),
		GobChildEllipsisSize		( SDB_M_GET_CHILD_LIST *ChildList, int Offset, const char *Name );

EXPORT_C_GOB int
		GobCheckChildList			( SDB_CHILD_LIST *ChildList, unsigned int ChildDataSize );
		
// GOBMUSH.C

EXPORT_C_GOB unsigned long
		GobMushInStreamValues		( SDB_OBJECT *SecPtr, DT_VALUE_RESULTS *ValueResults ),
        GobMushTypedInStreamValues  ( SDB_OBJECT *SecPtr, DT_VALUE_RESULTS *ValueResults, DT_DATA_TYPE *Types),
		GobMushStringToLong 		( char *String );

EXPORT_C_GOB char
		*GobMushString				( char *Buffer, unsigned long MushValue, int BufferSize ),
		*GobSigOut					( char *Buffer, double Number, int Width ),
		*GobSigOutWithDecimal		( char *Buffer, double Number, int Width ),
		*GobSigOutWithMagLetter		( char *Buffer, double Number, int Width );


// GOBVFDEF.C

EXPORT_C_GOB int
		GobValueFuncDefault			( GOB_VALUE_FUNC_ARGS );


// GOBVF.C and GOBVFVAL.C

EXPORT_C_GOB int
		GobValueFuncAlias			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncArrayOfVTs		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncBoolean			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncChild			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncChildDynamic	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncChildL			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncChildPassThrough( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncChildV			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncChildVDiddle	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncComponent		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncCompositeElement( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncConstant		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncCurrentDate		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncCurSecName		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncCurvePoint		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncDollarCross		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncDollarPrice		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncDoNothing		( GOB_VALUE_FUNC_ARGS ), 
		GobValueFuncDouble			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncEditInfo		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncEditObj			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncElement			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncEmptyStruct		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncExpirationMonth	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncExtractComponent( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncEqual			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncFakeDate		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncFutureDate		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncImpliedName		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncIndirect		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncIndirectVtInvoke( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncInStreams(		  GOB_VALUE_FUNC_ARGS ),
		GobValueFuncInt				( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncInt32			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncInverse			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncLiteral			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncLookupElem		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncMarketDate  	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncMaxAbsValueOne  ( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncMethodStruct	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncNonEmptyString	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncNonNegDouble	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncPastDate		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncPercent			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncPercentage		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncPositionDate	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncPositiveDouble	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncPricingDate		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncRename			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncRequired		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncSecName			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncSelf			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncSize			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncSortTable		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncStrTab			( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncStructureArray	( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncUniqueID		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncValueStruct		( GOB_VALUE_FUNC_ARGS ),
		GobValueFuncWhere			( GOB_VALUE_FUNC_ARGS );

EXPORT_C_GOB int
		GobValidateSecName			( SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, char *ClassName, int OkayEmpty );


// GOBUNTIL.C

EXPORT_C_GOB int
		GobValueFuncCalcUntilSet	( GOB_VALUE_FUNC_ARGS );


// GOBADD.C

EXPORT_C_GOB SDB_OBJECT
		*GobSecurityFromStructure(	SDB_DB *Db, DT_VALUE *ValuesStruct, GOB_IF_EXISTS IfExists, int Flags );

#endif
