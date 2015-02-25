/* $Header: /home/cvs/src/ufo/src/ufo.h,v 1.61 2001/07/30 22:25:09 simpsk Exp $ */
/****************************************************************
**
**	UFO.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.61 $
**
****************************************************************/

#ifndef IN_UFO_H
#define IN_UFO_H

#include <stdlib.h>

#if !defined( IN_GOB_H )
#	include	<gob.h>
#endif

#if !defined( IN_CFUNC_H )
#	include	<x_cfunc.h>				// For the SEC_C_FUNC
#endif

#include    <datatype.h> // for DtComponentGet, DTM_FREE

#include	<secdb.h>
#include    <secdrv.h>
#include    <secexpr.h>

/*
**  Forward declarations
*/

struct SLANG_ERROR_INFO;
struct SLANG_RET;
struct HashStructure;
struct SLANG_DATA_TYPE;

/*
**	Global variables
*/

DLLEXPORT DT_DATA_TYPE
		DtCFunc;

DLLEXPORT SLANG_ERROR_INFO
		*UfoErrorInfo;

// ValueTypeInfo Structure components - defined in slang
char const* const VTI_NAME = "Value Name";	    // Name of Value Type
char const* const VTI_TYPE = "DataType";		// Data Type of Value Type
char const* const VTI_DATA = "ValueData";		// Extra ValueData
char const* const VTI_FLAG = "Flags";			// SDB_FLAGS
char const* const VTI_CHLD = "Child List Func"; // Child List Func
char const* const VTI_RSLT = "Results";		    // Results

//	- Message handlers
char const* const VTI_GET	  = "GET";			// SDB_MSG_GET 				Handler
char const* const VTI_SET	  = "SET";			// SDB_MSG_SET 				Handler
char const* const VTI_STR	  = "STR";			// SDB_MSG_START 			Handler
char const* const VTI_NEW     = "NEW";			// SDB_MSG_NEW				Handler
char const* const VTI_VAL     = "VAL";			// SDB_MSG_VALIDATE 		Handler
char const* const VTI_LOAD    = "LOAD";			// SDB_MSG_LOAD	  	 		Handler
char const* const VTI_DID_SET = "DID_SET";		// SDB_MSG_DIDDLE_SET
char const* const VTI_DID_DEL = "DID_DEL";		// SDB_MSG_DIDDLE_CHANGED
char const* const VTI_DID_MOD = "DID_MOD";		// SDB_MSG_DIDDLE_REMOVED
char const* const VTI_DEF     = "DEF";			// All messages

char const* const UFO_OBJ_FUNCTIONS = "Object Functions"; // Object functions entry

// Error constants
int const UFO_ERR       = 7371;
int const UFO_PARSE_ERR = 7372;


// Value function Macros
inline int UFO_CALL_MSGFUNC(DT_VALUE* MsgFunc, SDB_OBJECT* SecPtr, int Msg, SDB_M_DATA* MsgData, SDB_VALUE_TYPE_INFO* ValueTypeInfo)
{ return (( SDB_VALUE_FUNC ) (((SEC_C_FUNC*)( MsgFunc->Data.Pointer ))->FuncPtr))( SecPtr, Msg, MsgData, ValueTypeInfo );}

inline DT_VALUE* UFO_DID_SET_FUNC( DT_VALUE* VTIStruct ) { return DtComponentGet( VTIStruct, VTI_DID_SET, NULL );}
inline DT_VALUE* UFO_DID_MOD_FUNC( DT_VALUE* VTIStruct ) { return DtComponentGet( VTIStruct, VTI_DID_MOD, NULL );}
inline DT_VALUE* UFO_DID_DEL_FUNC( DT_VALUE* VTIStruct ) { return DtComponentGet( VTIStruct, VTI_DID_DEL, NULL );}
inline DT_VALUE* UFO_LOAD_FUNC( DT_VALUE* VTIStruct )    { return DtComponentGet( VTIStruct, VTI_LOAD,    NULL );}
inline DT_VALUE* UFO_GET_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_GET,     NULL );}
inline DT_VALUE* UFO_STR_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_STR,     NULL );}
inline DT_VALUE* UFO_SET_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_SET,     NULL );}
inline DT_VALUE* UFO_VAL_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_VAL,     NULL );}
inline DT_VALUE* UFO_NEW_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_NEW,     NULL );}
inline DT_VALUE* UFO_DEF_FUNC( DT_VALUE* VTIStruct )     { return DtComponentGet( VTIStruct, VTI_DEF,     NULL );}
inline DT_VALUE* UFO_CHLD_FUNC( DT_VALUE* VTIStruct )    { return DtComponentGet( VTIStruct, VTI_CHLD,    NULL );}


// UFO_DEPENDENCY_INFO.Flags
int const UFO_DEP_CACHE_SET          = 0x0001;	// SDB_CACHE_SET_FLAG
int const UFO_DEP_VALUEDATA_BINARY   = 0x0002;	// ValueData converted to 'C' binary form ( where and rename)
int const UFO_DEP_IN_STREAM_CHILDREN = 0x0004;	// InStream ala Implied Name
int const UFO_DEP_PASS_ERRORS        = 0x0008;	// GOB_CHILD_PASS_ERRORS
int const UFO_DEP_OWN_VTI            = 0x0010;	// Free VTI when freeing depinfo
int const UFO_DEP_PHANTOM_DIDDLES    = 0x0020;	// diddles maybe phantom



// Slang Setup Macros

#define UFO_DEF_PARMS		 			memset( Params, 0, MAX_PARAMS * sizeof(DT_VALUE *));		\
										SecValue.DataType = DtSecurity;								\
										SecValue.Data.Pointer = SecPtr;								\
										VTIValue.DataType = DtStructure;							\
									    VTIValue.Data.Pointer = DepInfo->VTI.Data.Pointer;			\
									    Params[ 0 ] = &SecValue; Params[ 1 ] = &VTIValue; Args = 2


#define UFO_VAL_PARMS					TxtValue.DataType = DtNull;					   		      	  	  \
										Params[ Args++ ] = &TxtValue;

#define UFO_SET_PARMS					Params[ Args++ ] =  MsgData->SetValue.OldValue; 				  \
										Params[ Args++ ] =  MsgData->SetValue.Value;					  \
										FlagsValue.DataType = DtDouble;									  \
										FlagsValue.Data.Number = MsgData->SetValue.Flags;				  \
										Params[ Args++ ] =  &FlagsValue

#define UFO_SET_DID_PARMS				Params[ Args++ ] = MsgData->Diddle.Value; UfoDiddleId = MsgData->Diddle.DiddleID;				\
										Params[ Args++ ] = &FlagsValue;																	\
										FlagsValue.DataType = DtDouble;																	\
										FlagsValue.Data.Number = Msg == SDB_MSG_DIDDLE_CHANGED ? 1 : 0;									\
										if( MsgFunc && MsgFunc->DataType == DtSlang														\
											&& (SlangDataType = (SLANG_DATA_TYPE *) MsgFunc->Data.Pointer)->Expression->Argc == 6 )		\
											ParamToFree = Params[ Args++ ] = UfoSetVfArgs( MsgData->Diddle.Argc, MsgData->Diddle.Argv )

#define UFO_VAL_PARMS_FREE				DTM_FREE( &TxtValue )


// Cache set and Implied name hacks
char const* const UFO_CACHE_SET_STRING = "UFO_CACHE_SET";
inline bool UFO_CACHE_SET_ON_GET( SLANG_NODE const* Root )
{ return SLANG_NODE_IS_CONSTANT( Root, DtDouble ) && SLANG_NODE_STRING_VALUE( Root ) && !stricmp( SLANG_NODE_STRING_VALUE( Root ), UFO_CACHE_SET_STRING );}

char const* const UFO_IN_STREAM_STRING = "UFO_IN_STREAM_CHILDREN";
inline bool UFO_IN_STREAM_CHILDREN( SLANG_NODE const* Root )
{ return SLANG_NODE_IS_CONSTANT( Root, DtDouble ) && SLANG_NODE_STRING_VALUE( Root ) && !stricmp( SLANG_NODE_STRING_VALUE( Root ), UFO_IN_STREAM_STRING );}

char const* const UFO_PASS_ERRORS_STRING = "UFO_PASS_ERRORS";
inline bool UFO_PASS_ERRORS_CHILDREN( SLANG_NODE const* Root )
{ return SLANG_NODE_IS_CONSTANT( Root, DtDouble ) && SLANG_NODE_STRING_VALUE( Root ) && !stricmp( SLANG_NODE_STRING_VALUE( Root ), UFO_PASS_ERRORS_STRING );}

// magic constant which tells UFO that the diddle is a phantom diddle
int const UFO_DIDDLE_PHANTOM = 2;


// Child Flags
char const* const UFO_CHILD_PREFIX = "ChildData";		// Struct Name of dependencies that
char const* const UFO_CHILD        = "1";		 		// Dummy number for child hash table


// Where clause struct elements
char const* const UFO_WHERE_OBJECT	   = "_UFO_Object";    // Where.Object
char const* const UFO_WHERE_VALUE_TYPE = "_UFO_ValueType"; // Where.ValueType

												 //  ( used during hash create )
// Childdata string constants
int const MAX_STRING_EXPRESSION_SIZE = 100;			 // Max size of value type substitutions
int const MAX_PARAMS                 = 6;			 //	Current MAX size of slang parameter lists (+1)


// Misc return code
int const VERSION_NOT_FOUND			 = -1;			 // Version not detected


/*
**	Structure Definitions
*/

struct UFO_DEPENDENCY_INFO
{
	int		Flags;			// 'Special' Value Flags, currently used only for setting SDB_CACHE_SET on
							// MsgData->GetValue.CacheFlags

	DT_VALUE
			VTI,			// Original VTI struct

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

};


/*
** info about each phantom diddle we put on
*/

struct UFO_DIDDLE
{
	char	SecName[ SDB_SEC_NAME_SIZE ];

	SDB_DB	*Db;

	SDB_VALUE_TYPE
			ValueType;

	int		Argc;

	DT_VALUE
			**Argv;

};


/*
** list of phantom diddles by diddle ids
*/

struct UFO_DIDDLE_LIST
{
	UFO_DIDDLE
			Diddle;

	UFO_DIDDLE_LIST
			*Next;
};


/*
**	macros
*/

inline void UFO_DEPENDENCY_INFO_FREE( UFO_DEPENDENCY_INFO* DepInfo )
{
    if( DepInfo->Flags & UFO_DEP_OWN_VTI )
	    DTM_FREE( &(DepInfo)->VTI );
	free( DepInfo );
}

inline void UFO_DIDDLE_FREE( UFO_DIDDLE* Diddle )
{
    for( ; --Diddle->Argc >= 0; )
	{
		DTM_FREE( Diddle->Argv[ Diddle->Argc ]);
		free( Diddle->Argv[ Diddle->Argc ]);
	}
	free( Diddle->Argv );
	Diddle->Argv = NULL;
}


/*
**	Prototype functions
*/


// ufo.c
DLLEXPORT int
		UfoStart( 						SDB_CLASS_INFO *ClassInfo, DT_VALUE *ValueTableValue, int Version ),
		UfoObjFunc(						SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ),
		UFOValueFuncWhere(	 			GOB_VALUE_FUNC_ARGS	),
		UFOValueFuncRename(	 			GOB_VALUE_FUNC_ARGS	),
		UfoValueFunc(					GOB_VALUE_FUNC_ARGS ),
		UfoBreakpointToggle(			SDB_VALUE_TYPE_INFO *Vti, int Msg ),
		UfoBreakpointQuery(				SDB_VALUE_TYPE_INFO *Vti, int Msg );

int		UfoValueFuncInStreamChildren(	GOB_VALUE_FUNC_ARGS	);

extern HashStructure
		*UfoPhantomDiddles;

// ufochild.c
int		UfoChildParseSlang(				UFO_DEPENDENCY_INFO *DepInfo, SDB_M_GET_CHILD_LIST *ChildList );

int		UfoChildInStreams(				SDB_CLASS_INFO *Class, SDB_M_GET_CHILD_LIST *ChildList );

// ufodid.c

int		UfoDiddlesReplace(				UFO_DEPENDENCY_INFO *DepInfo, SDB_VALUE_TYPE_INFO *Vti );

extern SDB_DIDDLE_ID
		UfoDiddleId;

extern SDB_DIDDLE_FUNC
		UfoDiddleFunc;

// ufostart.c
SDB_VALUE_TYPE_INFO
		*UfoValueTypeInfo(				DT_VALUE *ArrayValue, char *ClassName );

int     UfoStartFromModule( 			SDB_CLASS_INFO *Class );

void	UfoValueTableFreeOnError(		DT_VALUE *ValueTable, SDB_VALUE_TYPE_INFO *Table, int FreeDepInfo );

int		UfoInitialize(					void );


// ufoeval.c
int		UfoVfEvaluate(	DT_VALUE **Params, SLANG_DATA_TYPE *SlangFunc, SLANG_RET *Ret, const char *ValueType, DT_VALUE *ChildData, int Args );

#endif
