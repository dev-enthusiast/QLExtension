/****************************************************************
**
**	SECOBJ.H	- Security master object header file
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secobj.h,v 1.87 2014/11/19 19:50:36 c01713 Exp $
**
****************************************************************/

#ifndef IN_SECOBJ_H
#define IN_SECOBJ_H

#if !defined( IN_BSTREAM_H )
#include <bstream.h>
#endif
#if !defined( IN_SDB_BASE_H )
#include <sdb_base.h>
#endif
#if !defined( IN_SECDB_SDBCHILD_H )
#include <sdbchild.h>
#endif
#include <ccstl.h>
#include <vector>

#include <iosfwd> // for std::ostream

/*
**	Define object method messages
*/

#define	SDB_MSG_START						0x0001
#define	SDB_MSG_END							0x0002
#define	SDB_MSG_LOAD						0x0003
#define	SDB_MSG_UNLOAD						0x0004
#define	SDB_MSG_NEW							0x0005
#define	SDB_MSG_ADD							0x0006
#define	SDB_MSG_UPDATE						0x0007
#define	SDB_MSG_DELETE						0x0008
#define	SDB_MSG_VALIDATE					0x0009
#define	SDB_MSG_GET_VALUE					0x000A
#define	SDB_MSG_SET_VALUE					0x000B
#define	SDB_MSG_GET_CHILD_LIST				0x000C
#define	SDB_MSG_DUMP_OBJ_DATA				0x000E
#define	SDB_MSG_DIDDLE_SET					0x000F
#define	SDB_MSG_DIDDLE_CHANGED				0x0010
#define	SDB_MSG_DIDDLE_REMOVED				0x0011
#define	SDB_MSG_RENAME						0x0012
#define	SDB_MSG_LOAD_VERSION				0x0014
#define	SDB_MSG_DESCRIPTION					0x0015
#define	SDB_MSG_MEM							0x0016		// Same as ADD but with no side-effects
#define	SDB_MSG_INCREMENTAL					0x0017
#define	SDB_MSG_INVALIDATE					0x0018
#define	SDB_MSG_MEM_USED					0x0019
#define	SDB_MSG_VTI_FREE					0x001A
#define SDB_MSG_VTI_INHERIT					0x001B
#define SDB_MSG_GET_DYNAMIC_CHILD_LIST		0x001C
	
#define	SDB_MSG_ROLL_BACK_NEW				0x001F
#define	SDB_MSG_ROLL_BACK_ADD				0x0020
#define	SDB_MSG_ROLL_BACK_UPDATE			0x0021
#define	SDB_MSG_ROLL_BACK_DELETE			0x0022
#define	SDB_MSG_ROLL_BACK_RENAME			0x0024
#define	SDB_MSG_ROLL_BACK_INCREMENTAL		0x0025

#define	SDB_MSG_IS_VALUE_TYPE_IMPLEMENTED	0x0026
#define SDB_MSG_GET_VALUE_TYPE_DATA			0x0027

// Message sent to security object on sucessful
// completion of events.
#define	SDB_MSG_POST_NEW             		0x0028
#define	SDB_MSG_POST_ADD    		        0x0029
#define	SDB_MSG_POST_UPDATE 		        0x002A
#define	SDB_MSG_POST_DELETE 		        0x002B
#define	SDB_MSG_POST_RENAME 		        0x002C

#define	SDB_MSG_USER						0x1000


/*
**	Child list related structures
*/

struct SDB_CHILD_ITEM_REF;
struct SDB_CHILD_REF;

union SDB_CHILD_REF_CAST_DATA
{
	SDB_CHILD_LIST_ITEM
			*Intermediate;

	int		ArgNum;

};

struct SDB_CHILD_REF_CAST
{
	DT_DATA_TYPE
			DataType;

	SDB_CHILD_REF_CAST_DATA
			Ref;

};

union SDB_CHILD_REF_DATA
{
	DT_VALUE	*Literal;

	SDB_CHILD_REF_CAST
				Cast;

	int			ArgNum;

	SDB_CHILD_LIST_ITEM
				*Intermediate;
				
};

struct SDB_CHILD_REF
{
	unsigned short
			Type,
			NoEval;
			
	SDB_CHILD_REF_DATA
			Ref;
				
};

struct SDB_CHILD_ITEM_REF
{
	SDB_CHILD_REF
			Security,
			ValueType,
			Db;
			
	int		Argc;
	
	SDB_CHILD_REF
			*Argv;

	bool	PassErrors,			// If set, return NULL or HUGE_VAL for missing values
			Dynamic;			// If set, return SecList values as SecDb Dynamic * datatype
};

/*
**	Define substructures of the security message data structure
*/

struct SDB_M_START
{
	SDB_CLASS_INFO
			*Class;

};

struct SDB_M_END
{
	SDB_CLASS_INFO
			*Class;

};

struct SDB_M_LOAD
{
	int		Reload,
			Changed;

	BSTREAM	*Stream;

	int		CurrentVersion;

	SDB_DISK_VERSION
			Version;

};

struct SDB_M_NEW
{
//	void	*Mem;
	int		Obsolete;

};

struct SDB_M_ADD
{
	BSTREAM	*Stream;
	
	SDB_DISK_VERSION
			Version;
	
};

struct SDB_M_UPDATE
{
	SDB_STORED_OBJECT
			CurObject;		// Current values in db for object (input to class func)
			
	BSTREAM	*Stream;		// Updated mem-stream for object (output of class func)

	SDB_DISK_VERSION
			Version;
	
	int		NothingChanged;

};

struct SDB_M_DUMP_OBJ_DATA
{
	void	*Handle;

	int		(*Print)( void *Handle, char *Format, ... );

};

struct SDB_M_GET_VALUE
{
	int		Argc;
	
	DT_VALUE
			*Value;

	void	*ChildData;
	
	DT_VALUE
			**ChildValues;

	int		Childc;

	SDB_SET_FLAGS
			CacheFlags;

	SDB_DB	*Db;

	SDB_NODE*
			Node;
};

struct SDB_M_DIDDLE
{
	DT_VALUE
			*Value;

	unsigned
			Phantom:1,
			NotMyDiddle:1;

	SDB_DIDDLE_ID
			DiddleID;
			
	int		Argc;
	
	DT_VALUE
			**Argv;
	
	SDB_SET_FLAGS
			Flags;

};

struct SDB_M_SET_VALUE
{
	DT_VALUE
			*Value,
			*OldValue;

	SDB_SET_FLAGS
			Flags;
	
	int		Argc;
	
	DT_VALUE
			**Argv;

};

struct SDB_M_RENAME
{
	int		CreateNewName;                     // true: ask security to return an implied name
	int     CacheOnlyInferredName;              // true: do not check with disk when creating implied name

	char	NewName[ SDB_SEC_NAME_SIZE ];

	char	*NewNamePtr;

};

struct SDB_M_VALIDATE
{
	SDB_VALIDATE
			*List;

	int		Count;
			
	SDB_VALIDATE
			*Base;

};

struct SDB_M_INVALIDATE
{
	int		Argc;
	
	DT_VALUE
			**Argv;

};

struct SDB_M_MEM_USED
{
	SDB_CLASS_INFO
			*Class;

	int		Size;
	
	DT_VALUE
			Detail;

};

struct SDB_M_DYNAMIC_CHILD_LIST
{
	SDB_NODE* node;

	CC_STL_VECTOR(SDB_NODE*)* children;
};

/*
**	Define structure used to pass messages to security objects
*/

union SDB_M_DATA
{
	SDB_M_START
			Start;

	SDB_M_END
			End;

	SDB_M_LOAD
			Load;

	SDB_M_NEW
			New;

	SDB_M_ADD
			Add,
			Mem;

	SDB_M_UPDATE
			Update;

	SDB_M_DUMP_OBJ_DATA
			DumpObjData;

	SDB_M_GET_VALUE
			GetValue;

	SDB_M_GET_CHILD_LIST
			GetChildList;

	SDB_M_DIDDLE
			Diddle;

	SDB_M_SET_VALUE
			SetValue;

	SDB_M_RENAME
			Rename;

	SDB_M_VALIDATE
			Validate;
			
	char	*Description;

	SDB_M_INVALIDATE
			Invalidate;
			
	SDB_M_MEM_USED
			MemUsed;

	int		IsValueTypeImplemented;

	DT_VALUE
			ValueTypeData;

	SDB_M_DYNAMIC_CHILD_LIST
			DynamicChildList;
};


/*
**	Prototype functions
*/

EXPORT_C_SECDB BSTREAM
		*SecDbBStreamCreateDefault(		void ),
		*SecDbBStreamCreate(			void *Mem, size_t MemSize );

EXPORT_C_SECDB void
		SecDbBStreamDestroy(			BSTREAM *Stream, int FreeData );

EXPORT_C_SECDB int
		SecDbMsgMem(					SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData );

EXPORT_C_SECDB void
		SecDbValidateAdd(				SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, int Flags, const char *Text );

EXPORT_C_SECDB int
		SecDbDefaultObjFunc(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData );

EXPORT_C_SECDB int
		SecDbValueTypeRegisterClass(	SDB_CLASS_INFO *Class, SDB_VALUE_TYPE_INFO *ValueTypeInfo ),
		SecDbValueTypeRegisterClassOld(	SDB_CLASS_INFO *Class, SDB_VALUE_TYPE_INFO *ValueTypeInfo ),
		SecDbObjectSdbClassSet(			SDB_OBJECT *SecPtr, SDB_CLASS_INFO *Class );
		
EXPORT_C_SECDB int
		SecDbClassRegister(				SDB_SEC_TYPE Type, const char *DllPath, const char *FuncName, const char *Name, const char *Argument ),
		SecDbClassUnregister(			const char *ClassName );

EXPORT_C_SECDB bool
        SecDbChildPullValue(			SDB_VALUE_TYPE_INFO *ValueTypeInfo, SDB_CHILD_COMPONENT *Child, int ByteOffset, DT_DATA_TYPE *DataType );

EXPORT_C_SECDB int
		SecDbSendGetChildList(			SDB_OBJECT *Object, SDB_VALUE_TYPE_INFO *Vti );

EXPORT_C_SECDB void
	    SecDbChildDescription(			CC_NS(std,ostream&), SDB_CHILD_LIST_ITEM *Item, SDB_CHILD_LIST *ChildList );

EXPORT_C_SECDB int
		SecDbChildListInfo(				SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, SDB_VALUE_TYPE_INFO *Vti, DT_VALUE *RetStruct );

EXPORT_C_SECDB int 
		SecDbValueTypeUsageInfo( SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, DT_VALUE* pDTResult );

EXPORT_C_SECDB void
		SecDbVtiInherit(				SDB_VALUE_TYPE_INFO *VtiChild, SDB_VALUE_TYPE_INFO *VtiParent );

EXPORT_C_SECDB int
		SecDbValueFuncDefault(			SDB_OBJECT *SecPtr, int MsgType, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo );

EXPORT_C_SECDB bool
		SecDbIsClassProtected( 			SDB_SEC_TYPE ClassType );

/*
**	Macro for sending value type messages
*/

#define	SDB_MESSAGE_VT( SecPtr_, Msg_, Data_, Vti_ )	( (Vti_) == NULL ? TRUE : (*(Vti_)->Func)( (SecPtr_), (Msg_), (Data_), (Vti_) ))


/*
**	send SDB_MSG_GET_CHILD_LIST if necessary
*/

#ifdef __cplusplus

inline int SecDbMsgGetChildList( SDB_OBJECT *Object, SDB_VALUE_TYPE_INFO *Vti )
{
	return !Vti->ChildList.ChildListValid ? SecDbSendGetChildList( Object, Vti ) : TRUE;
}

#endif

#endif
