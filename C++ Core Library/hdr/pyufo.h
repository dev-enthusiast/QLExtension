/* $Header: /home/cvs/src/pyglue/src/pyufo.h,v 1.3 2011/11/07 19:09:09 e63100 Exp $ */
/****************************************************************
**
**	PYUFO.H
**
**	Copyright 2011, Constellation Energy Group, Inc.
**
**	$Log: pyufo.h,v $
**	Revision 1.3  2011/11/07 19:09:09  e63100
**	Set PythonName on vti
**
**	Revision 1.2  2011/09/15 20:21:23  khodod
**	Eliminate some gcc warnings.
**	AWR: #237078
**
**	Revision 1.1  2011/08/03 18:40:43  e63100
**	initial commit
**
**
****************************************************************/

#if !defined( IN_PYUFO_H )
#define IN_PYUFO_H

#include <pyglue/base.h>

EXPORT_C_PYGLUE int
		PyUfoObjFunc( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData );


EXPORT_C_PYGLUE int 
		DefinePythonClass( char *Class,	char *ModuleName );

// Remove...
// ValueTypeInfo Structure components - defined in slang
#define	VTI_NAME	"Value Name"	// Name of Value Type
#define	VTI_TYPE	"DataType"		// Data Type of Value Type
#define	VTI_DATA	"ValueData"		// Extra ValueData
#define	VTI_FLAG	"Flags"			// SDB_FLAGS
#define VTI_CHLD	"Child Data"	// Child Data info
#define VTI_RSLT	"Results"		// Results

// Remove...
// UFO_DEPENDENCY_INFO.Flags
#define UFO_DEP_CACHE_SET			0x0001	// SDB_CACHE_SET_FLAG
#define UFO_DEP_VALUEDATA_BINARY	0x0002	// ValueData converted to 'C' binary form ( where and rename)
#define UFO_DEP_IN_STREAM_CHILDREN	0x0004	// InStream ala Implied Name
#define UFO_DEP_PASS_ERRORS			0x0008	// GOB_CHILD_PASS_ERRORS
#define UFO_DEP_OWN_VTI				0x0010	// Free VTI when freeing depinfo
#define UFO_DEP_PHANTOM_DIDDLES		0x0020	// diddles maybe phantom

#define NoMethod NULL

typedef struct PYUFODependencyInfoStruct
{
	int		Flags;			// 'Special' Value Flags, currently used only for setting SDB_CACHE_SET on
							// MsgData->GetValue.CacheFlags

	DT_VALUE
			VTI,			// Original VTI struct
			
			*PythonName,	// Name of corresponding Python attribute

			*DidSetFunc,	// SDB_MSG_DIDDLE_SET 	  function
			*DidModFunc,	// SDB_MSG_DIDDLE_CHANGED function
			*DidDelFunc,	// SDB_MSG_DIDDLE_REMOVED function

			*LoadFunc,		// SDB_LOAD_MSG 		  function
			*ChldFunc,		// SDB_GET_CHILD_LIST_MSG function	  // FOR INTERNAL USE ONLY
			*GetFunc,		// SDB_GET_MSG 			  function
			*NewFunc,		// SDB_NEW_MSG 			  function
			*StrFunc,		// SDB_START_MSG 		  function
			*SetFunc,		// SDB_SET_MSG 			  function
			*ValFunc,		// SDB_VALIDATE_MSG 	  function
			*DefFunc;		// Default 				  function

	DT_DATA_TYPE
			Type;

	char	*Description;

	int ArgCount; // FIX

} PYUFO_DEPENDENCY_INFO;

#define PYUFO_DEPENDENCY_INFO_FREE( DepInfo )			\
{													\
	if( DepInfo->Flags & UFO_DEP_OWN_VTI )			\
		DTM_FREE( &(DepInfo)->VTI );			   	\
	free( (DepInfo) );								\
}

#define INHERIT_MASK	 	    ( GOB_INHERIT_CLASS_FLAG		\
								| GOB_INHERIT_VT				\
								| GOB_DISINHERIT_VT				\
								| GOB_IMPLEMENT_INTERFACE_FLAG	\
								| GOB_DISINHERIT_INTERFACE_FLAG )


//	- Message handlers
#define	VTI_GET		"GET"			// SDB_MSG_GET 				Handler
#define	VTI_SET		"SET"			// SDB_MSG_SET 				Handler
#define	VTI_STR		"STR"			// SDB_MSG_START 			Handler
#define	VTI_NEW		"NEW"			// SDB_MSG_NEW				Handler
#define	VTI_VAL		"VAL"			// SDB_MSG_VALIDATE 		Handler
#define	VTI_LOAD	"LOAD"			// SDB_MSG_LOAD	  	 		Handler
#define	VTI_DID_SET	"DID_SET"		// SDB_MSG_DIDDLE_SET
#define	VTI_DID_DEL	"DID_DEL"		// SDB_MSG_DIDDLE_CHANGED
#define	VTI_DID_MOD	"DID_MOD"		// SDB_MSG_DIDDLE_REMOVED
#define	VTI_DEF		"DEF"			// All messages

#define UFO_CALL_MSGFUNC (( SDB_VALUE_FUNC ) ((( SEC_C_FUNC * )( MsgFunc->Data.Pointer ))->FuncPtr))( SecPtr, Msg, MsgData, ValueTypeInfo )

inline DT_VALUE* PYUFO_DID_SET_FUNC( DT_VALUE* VTIStruct ) { return DtComponentGet( VTIStruct, VTI_DID_SET, NULL );}
inline DT_VALUE* PYUFO_DID_MOD_FUNC( DT_VALUE* VTIStruct ) { return DtComponentGet( VTIStruct, VTI_DID_MOD, NULL );}
inline DT_VALUE* PYUFO_DID_DEL_FUNC( DT_VALUE* VTIStruct ) { return DtComponentGet( VTIStruct, VTI_DID_DEL, NULL );}
inline DT_VALUE* PYUFO_LOAD_FUNC( DT_VALUE* VTIStruct )    { return DtComponentGet( VTIStruct, VTI_LOAD,    NULL );}
inline DT_VALUE* PYUFO_GET_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_GET,     NULL );}
inline DT_VALUE* PYUFO_STR_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_STR,     NULL );}
inline DT_VALUE* PYUFO_SET_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_SET,     NULL );}
inline DT_VALUE* PYUFO_VAL_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_VAL,     NULL );}
inline DT_VALUE* PYUFO_NEW_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_NEW,     NULL );}
inline DT_VALUE* PYUFO_DEF_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_DEF,     NULL );}
inline DT_VALUE* PYUFO_CHLD_FUNC( DT_VALUE* VTIStruct )    { return DtComponentGet( VTIStruct, VTI_CHLD,    NULL );}

#endif
