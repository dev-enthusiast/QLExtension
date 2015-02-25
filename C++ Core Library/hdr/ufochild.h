/****************************************************************
**
**	UFOCHILD.H	- Ufo child parsing functions
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ufo/src/ufochild.h,v 1.6 2001/07/30 22:25:11 simpsk Exp $
**
****************************************************************/

#ifndef IN_UFO_UFOCHILD_H
#define IN_UFO_UFOCHILD_H

#include <secexpr.h>
#include <gobarg.h>

// To define your children, implement the SDB_MSG_GET_CHILD_LIST as:
//
// {
// 	struct CHILD_DATA
// 	{
// 		double	RateDate;
//
// 		SDB_OBJECT
// 				*PmFactory;
//
// 		DT_ARRAY
// 				*PMVtApply;
//
// 		double	ModelValue;
//
// 	} *ChildData;
//
// 	//...
//	
// 	case SDB_MSG_GET_CHILD_LIST:
// 		// First add all the args
// 		GOB_ADD_ARG( RateDate, &DtDate, "Date()" );
// 		GOB_ADD_ARG( PmFactory, &DtSecurity, "Default PM Factory( Self )" );
// 		GOB_ADD_ARG( PmVtApply, &DtVtApply, "Default PM VtApply( Self )" );
//
// 		// Now the children
// 		GOB_ADD_CHILD( ModelValue, &DtDouble, "Model Value VT( Self )( PM Object( Self, RateDate, PmFactory, PmVtApply ) )" );
// 		return TRUE;

#define GOB_ADD_ARG( ResultsTag, DataType, DefaultValue )	( Status = Status && GobAddChild( &MsgData->GetChildList, (DataType), offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, false, true, (DefaultValue) ))

#define GOB_ADD_CHILD( ResultsTag, DataType, DefaultValue )	( Status = Status && GobAddChild( &MsgData->GetChildList, (DataType), offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, false, false, (DefaultValue) ))

#define GOB_ADD_CHILD_PE( ResultsTag, DataType, DefaultValue )	( Status = Status && GobAddChild( &MsgData->GetChildList, (DataType), offsetof( CHILD_DATA, ResultsTag ), #ResultsTag, true, false, (DefaultValue) ))
 
// UfoValueTypeParse contexts

enum UfoValueTypeParseContext
{
	UFO_SECURITY_CONTEXT,
	UFO_VALUE_TYPE_CONTEXT,
	UFO_ARG_CONTEXT,
	UFO_DB_CONTEXT,
	UFO_TOP_LEVEL_CONTEXT
};

DLLEXPORT GOB_ARG
		*UfoValueTypeParse( SLANG_NODE *Root, DT_SLANG const *GetFunc, SDB_VALUE_TYPE_INFO *Vti, UfoValueTypeParseContext Context );

DLLEXPORT int
		GobAddChild( SDB_M_GET_CHILD_LIST *ChildList, DT_DATA_TYPE *pDataType, int Offset, const char *Name, bool PassErrors, bool IsArg, const char *ChildString );

DLLEXPORT GOB_ARG
		*GobArgFromString( SDB_M_GET_CHILD_LIST *ChildList, const char *ChildString );

#endif 
