#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_val.c,v 1.106 2004/11/29 23:48:12 khodod Exp $"
/****************************************************************
**
**	SDB_VAL.C	- Security database value types
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_val.c,v $
**	Revision 1.106  2004/11/29 23:48:12  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.105  2001/11/27 23:23:39  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.101  2001/10/02 15:33:26  singhki
**	move object,vt and vti into children. allows us to write better
**	iterators, literals take less space much cooler in general.
**	
**	Revision 1.100  2001/08/20 22:24:21  simpsk
**	various cleanups: added missing #includes, removed deprecated CC_ macros.
**	
**	Revision 1.99  2001/06/20 23:02:27  simpsk
**	need to use SecDbValueTypeRegisterClassOld.
**	
**	Revision 1.98  2001/06/19 17:26:56  simpsk
**	Create a GOB for an empty class.
**	
**	Revision 1.97  2001/05/16 17:40:48  singhki
**	removed dtnode.c, moved SecDb Node to x_secdb in slang
**	
**	Revision 1.96  2001/04/27 15:40:58  singhki
**	added SecDb Unstorable
**	
**	Revision 1.95  2001/03/23 23:26:39  singhki
**	Make ValueTypeMap just an STL map.
**	
**	Revision 1.94  2001/03/16 01:57:41  singhki
**	Add SDB_TRACK_NODE_WITH_ARGS which causes Create to invalidate the
**	special node Value Type Args every time a node with args is created.
**	
**	Add Value Type Args() VT to return the list of nodes with args.
**	
**	Revision 1.93  2001/03/15 16:41:00  simpsk
**	Minor cleaning.
**	
**	Revision 1.92  2001/03/09 16:50:20  goodfj
**	Added SecDbValueTypeCheck
**	
**	Revision 1.91  2001/03/09 16:33:57  goodfj
**	Prevent ValueTypes being registered as returning Null
**	
**	Revision 1.90  2001/03/09 15:22:00  goodfj
**	Better errors
**	
**	Revision 1.89  2001/02/28 18:59:01  singhki
**	ignore CHECK_SUPPORTED flag in FindTerminal. SpecialVts are now
**	generated entirely via the get value of Class Value Func.
**	
**	Revision 1.88  2001/02/28 14:15:51  shahia
**	take the header to the top of the file
**	
**	Revision 1.87  2001/02/25 18:28:13  vengrd
**	Revert build-breaking changes re: FOR_* macros.
**	
**	Revision 1.86  2001/02/23 23:56:32  simpsk
**	Use STL, not FOR* macros.
**	
**	Revision 1.85  2001/02/23 14:50:50  shahia
**	prevent atuo doc of MakeSecDbValueTypeMap until autodoc is fixed
**	
**	Revision 1.84  2001/02/22 12:32:22  shahia
**	changed ValueTypeMap implementation to use STL map
**	
**	Revision 1.83  2001/01/30 20:55:42  shahia
**	stop crash when Implemented Interfaces is called on non gob objects
**	
**	Revision 1.82  2000/12/28 21:27:45  singhki
**	Use an STL map for ImplementedInterfaces. Nicer than a HASH
**	
**	Revision 1.81  2000/11/24 18:40:48  singhki
**	Added Class member to VTI, support lots of components on DtValueTypeInfo
**	
**	Revision 1.80  2000/11/03 22:19:25  singhki
**	Honor CLASS_STATIC flag
**	
**	Revision 1.79  2000/10/20 20:52:37  singhki
**	Check if a VT defined as CLASS_STATIC is ever defined as INSTANCE
**	
**	Revision 1.78  2000/09/15 10:40:45  averir
**	Added SecDbValueTypeCount function
**	
**	Revision 1.77  2000/08/25 23:13:17  singhki
**	BaseDb may be NULL
**	
**	Revision 1.76  2000/04/24 11:04:24  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.75  2000/04/12 00:47:17  singhki
**	remove Security Description from SecDb Base Interface since it is a pain in the a**
**	
**	Revision 1.74  2000/04/10 20:33:10  fradkv
**	SecDb Base Interface is now automatically inherited by any class.
**	The interface mirrows SecDb Base Class
**	
**	Revision 1.73  2000/04/07 17:29:37  vengrd
**	Skunk interest rate merge/checkin
**	
**	Revision 1.72  2000/03/30 22:38:41  fradkv
**	Interfaces and abstract classes are introduced
**	
**	Revision 1.71  2000/02/10 22:32:50  singhki
**	Child ResultTypes are identified by a bit. Pass seperate flag to mark
**	child as terminal/intermediate
**	
**	Revision 1.70  2000/02/08 04:50:26  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.69  2000/02/01 22:54:18  singhki
**	Replace ValueTypeMap in SecPtr with the SDB_CLASS_INFO for the class
**	from which we should get VTIs. Class still points to the original
**	SDB_CLASS_INFO to be used for the ObjectFunc
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<date.h>
#include	<mempool.h>
#include	<hash.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secerror.h>
#include	<sectrace.h>
#include	<secindex.h>
#include	<sdb_int.h>
#include	<sdb_meta.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<sectrans.h>
#include	<datatype.h>
#include	<secdt.h>

#include    <map>
#include    <ccstl.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>


/*
**	Prototype functions
*/

static int
		ValFuncClassValueFunc(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncDateCreated(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncDbUpdated(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncDiddleScope(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncImplementedInterfaces(	SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncLastTransaction(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncObjectValueFunc(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncSecurityData(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncSecurityName(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncSecurityType(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncTimeModified(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncTranLogInfo(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncUpdateCount(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncValueFunc(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncVersion(					SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	);

int		SecDbValFuncValueTypeArgs(		SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	);


// FIX - deprecated
static int
		ValFuncDbCreated(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	),
		ValFuncTimeCreated(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo	);


/*
**	Special value types which every object gets
*/

static SDB_VALUE_TYPE_INFO
		ValueTableSecDbBase[] =
		{
			{	"Class Value Func",		&DtValueTypeInfo,ValFuncClassValueFunc,			NULL,	SDB_CALCULATED | SDB_SET_RETAINED | TRADE_ALLOW_OVERRIDE_VT | SDB_CLASS_STATIC },
			{	"Date Created",			&DtDate,		ValFuncDateCreated,				NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{	"Database Created",		&DtString,		ValFuncDbCreated,				NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},	// FIX- Deprecated
			{	"Database Updated",		&DtString,		ValFuncDbUpdated,				NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{	"Diddle Scope",			&DtDiddleScope, ValFuncDiddleScope,				NULL,	SDB_CALCULATED							},
			{	"Implemented Interfaces",&DtStructure,	ValFuncImplementedInterfaces,	NULL,	SDB_CALCULATED | SDB_CLASS_STATIC		},
			{	"Last Transaction",		&DtDouble,		ValFuncLastTransaction,			NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{	"Object Value Func",	&DtValueTypeInfo,ValFuncObjectValueFunc,		NULL,	SDB_CALCULATED | SDB_SET_RETAINED | TRADE_ALLOW_OVERRIDE_VT },
			{	"Security Data",		&DtBinary,		ValFuncSecurityData,			NULL,	SDB_CALCULATED | SDB_HIDDEN | SDB_EXPORT },
			{	"Security Description",	&DtString,		ValFuncSecurityName,			NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{	"Security Name",		&DtString,		ValFuncSecurityName,			NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{	"Security Type",		&DtString,		ValFuncSecurityType,			NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{	"Time Created",			&DtTime,		ValFuncTimeCreated,				NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{	"Time Modified",		&DtTime,		ValFuncTimeModified,			NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
            {	"Transaction Log Info",	&DtStructure,	ValFuncTranLogInfo,				NULL,	SDB_CALCULATED							},
			{	"Update Count",			&DtDouble,		ValFuncUpdateCount,				NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT	},
			{ 	"Value Func",			&DtValueTypeInfo,ValFuncValueFunc,				NULL,	SDB_CALCULATED | SDB_SET_RETAINED | TRADE_ALLOW_OVERRIDE_VT },
			{   "Value Type Args",		&DtEach,        SecDbValFuncValueTypeArgs,      NULL,   SDB_CALCULATED },
			{	"Version",				&DtStructure,	ValFuncVersion,					NULL,	SDB_STATIC | SDB_HIDDEN | SDB_EXPORT,	},
			{	NULL }
		};

/*
**	ValueTable for SecDb Base Interface
*/

static SDB_VALUE_TYPE_INFO
		ValueTableSecDbBaseInterface[] =
		{
			{	"Class Value Func",		&DtValueTypeInfo,	SecDbValueFuncDefault, 		NULL,	SDB_CLASS_STATIC },
			{	"Date Created",			&DtDate,			SecDbValueFuncDefault },
			{	"Database Created",		&DtString,			SecDbValueFuncDefault },
			{	"Database Updated",		&DtString,			SecDbValueFuncDefault },
			{	"Diddle Scope",			&DtDiddleScope, 	SecDbValueFuncDefault },
			{	"Implemented Interfaces",&DtStructure,		SecDbValueFuncDefault,		NULL,	SDB_CLASS_STATIC },
			{	"Last Transaction",		&DtDouble,			SecDbValueFuncDefault },
			{	"Object Value Func",	&DtValueTypeInfo,	SecDbValueFuncDefault },
			{	"Security Data",		&DtBinary,			SecDbValueFuncDefault },
//			{	"Security Description",	&DtString,			SecDbValueFuncDefault },
			{	"Security Name",		&DtString,			SecDbValueFuncDefault },
			{	"Security Type",		&DtString,			SecDbValueFuncDefault },
			{	"Time Created",			&DtTime,			SecDbValueFuncDefault },
			{	"Time Modified",		&DtTime,			SecDbValueFuncDefault },
            {	"Transaction Log Info",	&DtStructure,		SecDbValueFuncDefault },
			{	"Update Count",			&DtDouble,			SecDbValueFuncDefault },
			{ 	"Value Func",			&DtValueTypeInfo,	SecDbValueFuncDefault },
			{   "Value Type Args",		&DtEach,        	SecDbValueFuncDefault },
			{	"Version",				&DtStructure,		SecDbValueFuncDefault },
			{	NULL }
		};

static SDB_VALUE_TYPE_INFO
		ValueTableSecDbUnStorable[] =
		{
			{	NULL }
		};

static SDB_VALUE_TYPE_INFO
		ValueTableSecDbEmptyClass[] =
		{
			{	NULL }
		};

extern SDB_VALUE_TYPE						// Pre-defined value types:
		SecDbValueDateCreated,
		SecDbValueDbUpdated,
		SecDbValueLastTransaction,
		SecDbValueSecurityDesc,
		SecDbValueUpdateCount;				//	 Update count of object
		
// FIX - deprecated
extern SDB_VALUE_TYPE
		SecDbValueDbCreated,
		SecDbValueTimeCreated;

DLLEXPORT int
		SecDbObjFuncCast( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ),
		SecDbObjFuncSubscript( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ),
		DbObjFuncSecDbUnStorable( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ),
		DbObjFuncSecDbEmptyClass( SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData );

enum Vt_static_states
{
	VTNotDefined,
	VTDefinedStatic,
	VTDefinedInstance
};

const char *
		VTDefinedToName[] = 
{
	"NotDefined",
	"Static",
	"Instance"
};

typedef CC_STL_MAP( SDB_VALUE_TYPE_ID, Vt_static_states ) Class_static_vt_states;

static Class_static_vt_states class_static_vts;



/****************************************************************
**	Routine: ClassStaticVtCheck
**	Returns: TRUE/Err
**	Action : Checks if VT is class static and if it is not been
**           previously registered as per instance
****************************************************************/

static bool ClassStaticVtCheck(
	SDB_VALUE_TYPE_INFO	*VTI
)
{
	Vt_static_states Expected;

	if( !( VTI->ValueFlags & SDB_CLASS_STATIC ))
		Expected = VTDefinedInstance;
	else
		Expected = VTDefinedStatic;

	Vt_static_states& state = class_static_vts[ VTI->ValueType->ID ];
	if( state == VTNotDefined )
	{
		if( Expected == VTDefinedStatic )
			VTI->ValueType->ClassStaticVt = TRUE;
		state = Expected;
		return true;
	}
	if( state != Expected )
		return Err( ERR_NOT_CONSISTENT, "VT '%s' already defined as '%s', trying to redefine as '%s'", VTI->ValueType->Name, VTDefinedToName[ state ], VTDefinedToName[ Expected ] );

	return true;
}



/****************************************************************
**	Routine: SecDbValueTypeRegisterClassOld
**	Returns: TRUE	- Value type info registered without error
**			 FALSE	- Error registering value type info
**	Action : Register a table of value types for a security class
**			 whack in special Vts
**			 Deprecated, special Vts should be inherited
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeRegisterClassOld"

int SecDbValueTypeRegisterClassOld(
	SDB_CLASS_INFO		*Class,			// Class to register under
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type info to register
)
{
	static SDB_VALUE_TYPE_INFO
			*SpecialVts = NULL;

	SDB_VALUE_TYPE_INFO
			*SpecVti;

	char	Prefix[ SDB_SEC_NAME_SIZE ];

	if( !SpecialVts )
	{
		SpecialVts = new SDB_VALUE_TYPE_INFO[ sizeof( ValueTableSecDbBase ) / sizeof( SDB_VALUE_TYPE_INFO )];
		memcpy( SpecialVts, ValueTableSecDbBase, sizeof( ValueTableSecDbBase ));
	}

/*
**	Allocate the default value map for the class
*/

	if( !( Class->ValueTypeMap = new SecDbValueTypeMap ) )
		return Err( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );


/*
**	Setup the default object function behavior
*/

	for( SpecVti = SpecialVts; SpecVti->Name; ++SpecVti )
	{
		if( !SpecVti->ValueType )
			SpecVti->ValueType = SecDbValueTypeFromName( SpecVti->Name, *SpecVti->DataType );
		if( !SpecVti->ValueType )
			return FALSE;

		if( !ClassStaticVtCheck( SpecVti ))
			return FALSE;

		(*Class->ValueTypeMap)[ SpecVti->ValueType->ID ] = SpecVti;
	}


/*
**	Fill in the map with the class specific value types
*/

	for( ;ValueTypeInfo->Name; ValueTypeInfo++ )
	{
		ValueTypeInfo->Class = Class;
		if( ! ValueTypeInfo->ValueType )
			ValueTypeInfo->ValueType = SecDbValueTypeFromName( ValueTypeInfo->Name, *(ValueTypeInfo->DataType) );
		if( ! ValueTypeInfo->ValueType )
			return FALSE;

		if( !ClassStaticVtCheck( ValueTypeInfo ))
			return FALSE;

		(*Class->ValueTypeMap)[ ValueTypeInfo->ValueType->ID ] = ValueTypeInfo;
	}

	// Construct the name for the Class Security
	sprintf( Prefix, "~%lx:", (long) Class );
	return Class->GenerateClassSec( Prefix );
}



/****************************************************************
**	Routine: SecDbValueTypeRegisterClass
**	Returns: TRUE	- Value type info registered without error
**			 FALSE	- Error registering value type info
**	Action : Register a table of value types for a security class
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeRegisterClass"

int SecDbValueTypeRegisterClass(
	SDB_CLASS_INFO		*Class,			// Class to register under
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type info to register
)
{
	char	Prefix[ SDB_SEC_NAME_SIZE ];

/*
**	FIX: remove this getout as soon as possible
*/
	static int
			AllowNullValueTypeDataType = !stricmp( "true", SecDbConfigurationGet( "SECDB_ALLOW_NULL_VALUETYPE_DATATYPE", NULL, NULL, "" ) );

/*
**	Allocate the default value map for the class
*/
	Class->ValueTypeMap = new SecDbValueTypeMap;


/*
**	Fill in the map with the class specific value types
*/

	for( ;ValueTypeInfo->Name; ValueTypeInfo++ )
	{
		ValueTypeInfo->Class = Class;
		if( ! ValueTypeInfo->ValueType )
		{
			if( !AllowNullValueTypeDataType && !(*ValueTypeInfo->DataType) )
				return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to register %s (datatype missing)", ValueTypeInfo->Name ? ValueTypeInfo->Name : "unknown vt" );
			ValueTypeInfo->ValueType = SecDbValueTypeFromName( ValueTypeInfo->Name, *(ValueTypeInfo->DataType) );
		}
		if( ! ValueTypeInfo->ValueType )
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to register %s", ValueTypeInfo->Name ? ValueTypeInfo->Name : "unknown vt" );

		if( !ClassStaticVtCheck( ValueTypeInfo ))
			return ErrMore( ARGCHECK_FUNCTION_NAME );

		(*Class->ValueTypeMap)[ ValueTypeInfo->ValueType->ID ] = ValueTypeInfo;
	}

	// Construct the name for the Class Security
	sprintf( Prefix, "~%lx:", (long) Class );
	return Class->GenerateClassSec( Prefix );
}



/****************************************************************
**	Routine: SecDbObjectSdbClassSet
**	Returns: TRUE	- Value type info registered without error
**			 FALSE	- Error registering value type info
**	Action : Register a table of value types for a security class
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbObjectSdbClassSet"

int SecDbObjectSdbClassSet(
	SDB_OBJECT			*SecPtr,		// Object to get registered
	SDB_CLASS_INFO		*NewClass		// New class to register
)
{
	SDB_VALUE_TYPE_INFO
			*Vti;
			
	int		Token = 0;

	SDB_NODE
			*Node;
			
	SDB_ENUM_PTR
			EnumPtr;
	
	
	if( SecPtr->SdbClass == NewClass )
		return TRUE;

	// Assign the new class
	SecPtr->SdbClass = NewClass;
		
/*
**	For each node in current cache:
**		if not in new map
**			discard
**		else if not SDB_CACHE_SET
**			invalidate
*/

	SDB_NODE::Destroy( NULL, &Token );
	for( Node = SecDbNodeEnumFirst( SecPtr, SDB_NODE_ENUM_TERMINALS, &EnumPtr ); Node; Node = SecDbNodeEnumNext( EnumPtr ))
	{
		Vti = Node->VtiGet();
		if( Vti && !NewClass->LookupVti( Vti->ValueType->ID ) )
			SDB_NODE::Destroy( Node, &Token );
		else if( !( Node->CacheFlagsGet() & ( SDB_CACHE_SET | SDB_CACHE_DIDDLE )))
		{
			Node->Invalidate();
			Node->InvalidateChildren();
		}
		if( Node->DbSetGet()->BaseDb() && Node->VtiNodeGet() && Node->VtiNodeGet()->ObjectGet() != Node->ObjectGet() ) // Reassign VTI
			Node->VtiNodeSet( SDB_NODE::FindDefaultVtiNode( Node->DbSetGet()->BaseDb(), Node->ObjectGet(), Node->ValueTypeGet(), SDB_NODE::CREATE ));
	}
	SecDbNodeEnumClose( EnumPtr );
	SDB_NODE::Destroy( NULL, &Token );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbValueTypeFromName
**	Returns: Value type
**	Summary: Retrieve a value type from a value type name
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeFromName"

SDB_VALUE_TYPE SecDbValueTypeFromName(
	const char		*ValueName,		// Value name to convert into value type
	DT_DATA_TYPE	DataType		// Optional data type of the value type
)
{
	static int next_value_type_id = 0;

	SDB_VALUE_TYPE
			ValueType;

/*
**	FIX: remove this getout as soon as possible
*/
	static int
			AllowNullValueTypeDataType = !stricmp( "true", SecDbConfigurationGet( "SECDB_ALLOW_NULL_VALUETYPE_DATATYPE", NULL, NULL, "" ) );


	if( !SecDbValueTypeCheck( ValueName, DataType ) )
		return (SDB_VALUE_TYPE) ErrMoreN( ARGCHECK_FUNCTION_NAME ": @ - '%s' is invalid", ValueName );

	if( DataType && !AllowNullValueTypeDataType && DataType == DtNull && ValueName )
		return (SDB_VALUE_TYPE) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Can't register '%s' as '%s'", ValueName, DataType->Name );

	ValueType = (SDB_VALUE_TYPE) HashLookup( SecDbValueTypeHash, ValueName );
	if( ValueType )
	{
		if( DataType )
		{
			if( ValueType->DataType && ValueType->DataType != DataType && ValueType->DataType != DtNull )
				return (SDB_VALUE_TYPE) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Can't change '%s'\nfrom '%s' to '%s'",
						ValueType->Name, ValueType->DataType->Name, DataType->Name );
			ValueType->DataType = DataType;
		}
		return ValueType;
	}

	if( strlen( ValueName ) >= SDB_VALUE_NAME_SIZE )
		return (SDB_VALUE_TYPE) ErrN( ERR_OVERFLOW, "ValueTypeFromName( %s ), name too long", ValueName );

	ValueType = (SDB_VALUE_TYPE_STRUCTURE *) malloc( sizeof( SDB_VALUE_TYPE_STRUCTURE ));

	if( ValueType )
	{
		ValueType->ID = next_value_type_id++;
		ValueType->NoEval = FALSE;
		ValueType->ClassStaticVt = FALSE;
		ValueType->IsSpecialVt = FALSE;
		ValueType->SpecialVt = NULL;
		strcpy( ValueType->Name, ValueName );
		if( DataType )
			ValueType->DataType = DataType;
		else
			ValueType->DataType = DtNull;
		HashInsert( SecDbValueTypeHash, ValueType->Name, ValueType );
		return ValueType;
	}

	return (SDB_VALUE_TYPE) ErrN( ERR_MEMORY, ARGCHECK_FUNCTION_NAME ": @" );
}



/****************************************************************
**	Routine: SecDbValueNameFromType
**	Returns: Name of specified value type
**			 "NULL" if ValueType is NULL
**	Summary: Retrieve the name of a value type
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueNameFromType"

const char *SecDbValueNameFromType(
	SDB_VALUE_TYPE	ValueType		// Value type to lookup
)
{
	return ValueType ? ValueType->Name : "NULL";
}



/****************************************************************
**	Routine: SecDbValueTypeCount
**	Returns: size_t
**	Summary: Returns count of 'known'/loaded value types
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeCount"

size_t SecDbValueTypeCount()
{
	return (size_t) SecDbValueTypeHash->KeyCount;
}


/****************************************************************
**	Routine: SecDbValueTypeEnumFirst
**	Returns: First ValueType info
**			 NULL if no ValueTypes
**	Summary: Start the enumeration of value types for a security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeEnumFirst"

SDB_VALUE_TYPE_INFO *SecDbValueTypeEnumFirst(
	SDB_OBJECT		*SecPtr,	// Security to enumerate, or NULL for all
	SDB_ENUM_PTR	*EnumPtr	// Pointer to enumeration pointer
)
{
	SDB_VALUE_TYPE_INFO
			*ValueTypeInfo;

	HASH	*Hash;

	HASH_ENTRY_PTR
			Ptr;


/*
**	Allocate the enumeration structures
*/

	*EnumPtr = (SDB_ENUM_STRUCT *) malloc( sizeof( SDB_ENUM_STRUCT ));
	Hash = HashCreate( "ValueTypeEnum", NULL, NULL, 0, MemPoolCreate( ));


/*
**	Special case of the security name being NULL, return ALL value types
*/

	if( !SecPtr )
	{
		HASH_FOR( Ptr, SecDbValueTypeHash )
		{
			ValueTypeInfo = (SDB_VALUE_TYPE_INFO *) MemPoolCalloc( Hash->MemPool, 1, sizeof( *ValueTypeInfo ));
			ValueTypeInfo->ValueType = (SDB_VALUE_TYPE) HashValue( Ptr );
			HashInsert( Hash, (HASH_KEY) ValueTypeInfo->ValueType, (HASH_VALUE) ValueTypeInfo );
		}
	}


/*
**	Case of a specific security
*/

	else
	{
		const SDB_VALUE_TYPE_INFO* Vti;
		FOREACH_CLASS_VTI_BEGIN(SecPtr->SdbClass->ValueTypeMap, Vti)

			HashInsert( Hash, (HASH_KEY) Vti->ValueType, (HASH_VALUE) Vti );

		FOREACH_CLASS_VTI_END();
	}


/*
**	Get the first ValueType from the linked list
*/

	(*EnumPtr)->Hash	= Hash;
	(*EnumPtr)->Count	= Hash->KeyCount;
	(*EnumPtr)->Ptr		= HashFirst( Hash );
	if( HashEnd( (*EnumPtr)->Ptr ))
		return NULL;
	return (SDB_VALUE_TYPE_INFO *) HashValue( (*EnumPtr)->Ptr );
}



/****************************************************************
**	Routine: SecDbValueTypeEnumReset
**	Returns: First ValueType info
**			 NULL if no more ValueTypes
**	Summary: Return to the first value type info for a security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeEnumReset"

SDB_VALUE_TYPE_INFO *SecDbValueTypeEnumReset(
	SDB_ENUM_PTR	EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashFirst( EnumPtr->Hash );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;
	return (SDB_VALUE_TYPE_INFO *) HashValue( EnumPtr->Ptr );
}



/****************************************************************
**	Routine: SecDbValueTypeEnumNext
**	Returns: Next ValueType info
**			 NULL if no more ValueTypes
**	Summary: Get the next value type info for a security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeEnumNext"

SDB_VALUE_TYPE_INFO *SecDbValueTypeEnumNext(
	SDB_ENUM_PTR	EnumPtr		// Enumeration pointer
)
{
	EnumPtr->Ptr = HashNext( EnumPtr->Hash, EnumPtr->Ptr );
	if( HashEnd( EnumPtr->Ptr ))
		return NULL;
	return (SDB_VALUE_TYPE_INFO *) HashValue( EnumPtr->Ptr );
}



/****************************************************************
**	Routine: SecDbValueTypeEnumClose
**	Returns: Nothing
**	Summary: End the enumeration of value types
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeEnumClose"

void SecDbValueTypeEnumClose(
	SDB_ENUM_PTR	EnumPtr		// Enumeration pointer
)
{
	if( EnumPtr )
	{
		MemPoolDestroy( EnumPtr->Hash->MemPool );
		free( EnumPtr );
	}
}



/****************************************************************
**	Routine: SecDbValueFlags
**	Returns: Value flags associated with 'SecType' and 'ValueType'
**			 0 if no flags or 'SecType' does not support 'ValueType'
**	Summary: Get the value flags for a security type and value type
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueFlags"

SDB_VALUE_FLAGS SecDbValueFlags(
	SDB_SEC_TYPE	SecType,		// Security class
	SDB_VALUE_TYPE	ValueType		// Value type to lookup
)
{
	SDB_CLASS_INFO
			*ClassInfo = SecDbClassInfoFromType( SecType );

	SDB_VALUE_TYPE_INFO
			*Vti;


	if( ClassInfo && ClassInfo->ValueTypeMap && ( Vti = ClassInfo->LookupVti( ValueType->ID )))
		return Vti->ValueFlags;
	else
		return 0;
}



/*
**	Standard value function code for pulling things out of secptr
*/

#define VAL_FUNC_GET_FROM_SECPTR( Data_, Value_ )														\
{																										\
	if( SDB_MSG_SET_VALUE == Msg )																		\
		return FALSE;																					\
	else if( SDB_MSG_GET_VALUE == Msg )																	\
	{																									\
		MsgData->GetValue.Value->Data.Data_ = Value_;													\
		MsgData->GetValue.CacheFlags = SDB_CACHE_PASS_THROUGH;											\
	}																									\
	else if( SDB_MSG_GET_CHILD_LIST == Msg )															\
	{																									\
		DT_VALUE																						\
				ValueTypeVal;																			\
																										\
		SDB_CHILD_LIST_ITEM																				\
				Item;																					\
																										\
		DTM_INIT( &ValueTypeVal );																		\
		ValueTypeVal.DataType       = DtValueType;														\
		ValueTypeVal.Data.Pointer   = SecDbValueSecurityData;											\
		Item.ValueType.Element		= MsgData->GetChildList.LiteralAdd( &ValueTypeVal, DtValueType );	\
		Item.ValueType.Type         = SDB_CHILD_LIST_LITERAL;											\
		Item.Result.Type			= SDB_CHILD_LIST_TERMINAL;											\
		Item.Name      		      	= "Security Data";													\
		if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )					\
			return FALSE;																				\
		return TRUE;																					\
	}																									\
	return TRUE;																						\
}



/****************************************************************
**	Routine: InvalidateSelf
**	Returns: TRUE
**	Action : Invalidates the node representing this value
****************************************************************/

inline int InvalidateSelf(
	SDB_OBJECT			*SecPtr,
	SDB_VALUE_TYPE_INFO	*Vti
)
{
	// FIX- If you diddle one of these nodes they will split and hence not be invalidated
	// Ideally we should call SecDbInvalidate with a flag which says don't create the node if it doesn't exist
	// cause if we naively call it then all these nodes will in instantiated as invalid when the SecPtr is loaded
	SDB_NODE *Node = SDB_NODE::FindTerminalInt( SecPtr, Vti->ValueType, 0, NULL, SDB_NODE::DO_NOT_CREATE, SecPtr->Db, SDB_NODE::SHARE );
	if( Node )
		Node->Invalidate();
	return TRUE;
}



/****************************************************************
**	Routine: ValFuncSecurityData
**	Returns: Always TRUE
**	Action : Invalidates when the object is loaded
****************************************************************/

static int ValFuncSecurityData(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	switch( Msg )
	{
		case SDB_MSG_SET_VALUE:
			return FALSE;

			// FIX- For now we always invalidate these even if it is a Reload and the object has not changed
			// this is because Rename uses this in order to invalidate the name, and I just need to figure out
			// the right set of messages to get invalidation
		case SDB_MSG_LOAD:
			return InvalidateSelf( SecPtr, ValueTypeInfo ); // if ( !MsgData->Load.Reload || MsgData->Load.Changed ))

		case SDB_MSG_GET_VALUE:
			// FIX- this should be the bstream of the security and all instreams should depend upon it
			DTM_ALLOC( MsgData->GetValue.Value, DtBinary );
			return TRUE;
	}
	return TRUE;
}



/****************************************************************
**	Routine: ValFuncSecurityName
**	Returns: Always TRUE
**	Action : Return the security name
****************************************************************/

static int ValFuncSecurityName(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	if( SDB_MSG_RENAME == Msg )
		return InvalidateSelf( SecPtr, ValueTypeInfo );
	VAL_FUNC_GET_FROM_SECPTR( Pointer, SecPtr->SecData.Name )
}



/****************************************************************
**	Routine: ValFuncSecurityType
**	Returns: Always TRUE
**	Action : Returns the security type
****************************************************************/

static int ValFuncSecurityType(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	SDB_CLASS_INFO
			*ClassInfo = NULL;			// Dummy initializer to quiet gcc warning

	if( SDB_MSG_SET_VALUE == Msg )
	{
		const char 
				*ClassName = (const char *) MsgData->SetValue.Value->Data.Pointer;
		SDB_SEC_TYPE 
				Type;
		if ( !(Type = SecDbClassTypeFromName( ClassName )))
			return FALSE;
		return SecDbReclassify( SecPtr, Type );
	}

	if( SecPtr && !( ClassInfo = SecDbClassInfoFromType( SecPtr->Class->Type )))
		return FALSE;
	VAL_FUNC_GET_FROM_SECPTR( Pointer, ClassInfo->Name )
}



/****************************************************************
**	Routine: ValFuncTimeCreated
**	Returns: Always TRUE
**	Action : Returns the time a security was created
****************************************************************/

static int ValFuncTimeCreated(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	VAL_FUNC_GET_FROM_SECPTR( Number, (double) DATE_TO_TIME( (DATE) SecPtr->SecData.DateCreated ))
}



/****************************************************************
**	Routine: ValFuncTimeModified
**	Returns: Always TRUE
**	Action : Returns the time a security was modified
****************************************************************/

static int ValFuncTimeModified(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	VAL_FUNC_GET_FROM_SECPTR( Number, (double) SecPtr->SecData.TimeUpdated )
}



/****************************************************************
**	Routine: ValFuncDbCreated
**	Returns: Always TRUE
**	Action : Returns the name of the database that created the object
****************************************************************/

static int ValFuncDbCreated(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	VAL_FUNC_GET_FROM_SECPTR( Pointer, "" )
}



/****************************************************************
**	Routine: ValFuncDbUpdated
**	Returns: Always TRUE
**	Action : Returns the name of the database that last updated the object
****************************************************************/

static int ValFuncDbUpdated(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	SDB_DB_INFO
			DbInfo;


	VAL_FUNC_GET_FROM_SECPTR( Pointer, SecPtr->SecData.DbIDUpdated && SecDbDbInfoFromID( SecPtr->SecData.DbIDUpdated, &DbInfo ) ? DbInfo.DbName : "" )
}



/****************************************************************
**	Routine: ValFuncUpdateCount
**	Returns: Always TRUE
**	Action : Returns the update count of the object
****************************************************************/

static int ValFuncUpdateCount(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	VAL_FUNC_GET_FROM_SECPTR( Number, (double) SecPtr->SecData.UpdateCount )
}



/****************************************************************
**	Routine: ValFuncDateCreated
**	Returns: Always TRUE
**	Action : Returns the creation date of the object
****************************************************************/

static int ValFuncDateCreated(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	VAL_FUNC_GET_FROM_SECPTR( Number, (double) SecPtr->SecData.DateCreated )
}



/****************************************************************
**	Routine: ValFuncLastTransaction
**	Returns: Always TRUE
**	Action : Returns the last transaction of the object
****************************************************************/

static int ValFuncLastTransaction(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	VAL_FUNC_GET_FROM_SECPTR( Number, (double) SecPtr->SecData.LastTransaction )
}



/****************************************************************
**	Routine: ValFuncVersion
**	Returns: Always TRUE
**	Action : Returns the time a security was created
****************************************************************/

static int ValFuncVersion(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of operation
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	int		Status;
	
	SDB_DISK_VERSION
			Version;


	if( SDB_MSG_SET_VALUE == Msg )
		return FALSE;
	else if( SDB_MSG_GET_VALUE == Msg )
	{
		Status = DTM_ALLOC( MsgData->GetValue.Value, DtStructure );
		Status = Status && SecDbDiskInfoGetVersion( &SecPtr->SecData, &Version );
		Status = Status && DtComponentSetNumber( MsgData->GetValue.Value, "ObjectVersion",	DtDouble,	Version.Object );
		Status = Status && DtComponentSetNumber( MsgData->GetValue.Value, "StreamVersion",	DtDouble,	Version.Stream );
		if( !Status )
			return FALSE;
	}
	else if( SDB_MSG_GET_CHILD_LIST == Msg )
	{
		DT_VALUE
				ValueTypeVal;

		SDB_CHILD_LIST_ITEM
				Item;

		DTM_INIT( &ValueTypeVal );
		ValueTypeVal.DataType       = DtValueType;
		ValueTypeVal.Data.Pointer   = SecDbValueSecurityData;
		Item.ValueType.Element		= MsgData->GetChildList.LiteralAdd( &ValueTypeVal, DtValueType );
		Item.ValueType.Type         = SDB_CHILD_LIST_LITERAL;
		Item.Result.Type			= SDB_CHILD_LIST_TERMINAL;
		Item.Name      		      	= "Security Data";
		if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
			return FALSE;
		return TRUE;
	}
	return TRUE;
}


/****************************************************************
**	Routine:	ValFuncValueFunc
**	Returns:	TRUE/Err
**	Action:		
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "ValFuncValueFunc"

static int ValFuncValueFunc(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	DT_VALUE
			*Args,
			*Result;

	SDB_VALUE_TYPE
			ValueType;

	DT_VALUE_TYPE_INFO
			*Vti;

		
	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Gets the Vti for a Value Type" );
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:
		{
			SDB_CHILD_LIST_ITEM
					Item;

			Item.Result.Type	= SDB_CHILD_LIST_ELLIPSIS;
			Item.Name			= "...";
			if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				return FALSE;
			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
			Args = MsgData->GetValue.ChildValues[ 0 ];
			if( Args->DataType != DtArray )
				return Err( ERR_UNKNOWN, ARGCHECK_FUNCTION_NAME ": A horrible error has occurred in NodeGetValue, ... isn't an array" );

			Result = MsgData->GetValue.Value;
			DTM_ALLOC( Result, DtValueTypeInfo );
			Vti = (DT_VALUE_TYPE_INFO *) Result->Data.Pointer;
			Vti->Free = FALSE;	// Vti will be owned by the class

			if( DtSize( Args ) == 0 )
				ValueType = SecDbValueValueFunc;
			else if( !(ValueType = (SDB_VALUE_TYPE)DtSubscriptGetPointer( Args, 0, DtValueType, NULL )))
				return Err( ERR_INVALID_ARGUMENTS, "Value Func(): Expected Arg0 of type Value Type" );

			Vti->Vti = SecPtr->SdbClass->LookupVti( ValueType->ID );
			return TRUE;
	}

	return TRUE;
}



/****************************************************************
**	Routine:	ValFuncClassValueFunc
**	Returns:	TRUE/Err
**	Action:		
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "ValFuncClassValueFunc"

static int ValFuncClassValueFunc(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Gets the class Vti for a Value Type" );
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:
		{
			SDB_CHILD_LIST_ITEM
					Item;

			DT_VALUE
					TmpValue;

			TmpValue.DataType = DtValueType;
			TmpValue.Data.Pointer = SecDbValueClassValueFunc;

			Item.Result.Type	= SDB_CHILD_LIST_ARGUMENT;
			Item.Security.Type	= SDB_CHILD_LIST_CONSTANT;
			Item.Security.Element=MsgData->GetChildList.LiteralAdd( &TmpValue, DtValueType );
			Item.Name			= "ValueType";
			if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				return FALSE;

			ValueTypeInfo->ValueType->ClassStaticVt = TRUE;
			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
		{
			DT_VALUE
					*Result;

			DT_VALUE_TYPE_INFO
					*Vti;
		
			SDB_VALUE_TYPE
					ValueType = (SDB_VALUE_TYPE) MsgData->GetValue.ChildValues[ 0 ]->Data.Pointer;

			const SDB_VALUE_TYPE_INFO *pClassVti = SecPtr->SdbClass->LookupVti( ValueType->ID );

			if( !pClassVti )
			{
				if( ClassSpecialVtGen( SecPtr, ValueType ) )
					pClassVti = SecPtr->SdbClass->LookupVti( ValueType->ID );
				else
					return Err( ERR_UNSUPPORTED_OPERATION, "@ Class '%s' does not support VT '%s'", SecPtr->Class->Name, ValueType->Name );
			}

			Result = MsgData->GetValue.Value;
			DTM_ALLOC( Result, DtValueTypeInfo );
			Vti = (DT_VALUE_TYPE_INFO *) Result->Data.Pointer;
			Vti->Free = FALSE;	// Vti will be owned by the class
			Vti->Vti = const_cast<SDB_VALUE_TYPE_INFO*>( pClassVti );
			return TRUE;
		}		
	}

	return TRUE;
}



/****************************************************************
**	Routine:	ValFuncObjectValueFunc
**	Returns:	TRUE/Err
**	Action:		
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "ValFuncObjectValueFunc"

static int ValFuncObjectValueFunc(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Gets the Vti for a Value Type on an object" );
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:
		{
			DT_VALUE TmpValue;
			TmpValue.DataType = DtValueType;
			TmpValue.Data.Pointer = SecDbValueObjectValueFunc;

			{
			    SDB_CHILD_LIST_ITEM Item;
				Item.Result.Type	= SDB_CHILD_LIST_ARGUMENT;
				Item.Security.Type	= SDB_CHILD_LIST_CONSTANT;
				Item.Security.Element=MsgData->GetChildList.LiteralAdd( &TmpValue, DtValueType );
				Item.Name			= "ValueType";
				if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				    return FALSE;
			}

			{
			    SDB_CHILD_LIST_ITEM Item;
				Item.Name			   = "Class VTI";
				Item.Result.Type	   = SDB_CHILD_LIST_TERMINAL;

				TmpValue.Data.Pointer  = SecDbValueClassValueFunc;
				Item.ValueType.Type    = SDB_CHILD_LIST_LITERAL;
				Item.ValueType.Element = MsgData->GetChildList.LiteralAdd( &TmpValue, DtValueType );

				SDB_CHILD_COMPONENT ItemArg;
				ItemArg.Type    = SDB_CHILD_LIST_ARGUMENT;
				ItemArg.Element = 0;

				Item.Argc			   = 1;
				Item.Argv			   = &ItemArg;

				if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				    return FALSE;
			}
			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
			DTM_ASSIGN( MsgData->GetValue.Value, MsgData->GetValue.ChildValues[ 1 ] );
			return TRUE;
	}

	return TRUE;
}



/****************************************************************
**	Routine:	ValFuncDiddleScope
**	Returns:	TRUE/Err
**	Action:		
****************************************************************/

static int ValFuncDiddleScope(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Constructs a new diddle scope" );
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:
		{
			SDB_CHILD_LIST_ITEM
					Item;

			Item.Result.Type	= SDB_CHILD_LIST_ELLIPSIS;
			Item.Name			= "...";
			if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				return FALSE;
			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
			if( DiddleScopeNestNew( MsgData->GetValue.Db, MsgData->GetValue.Value ))
				return TRUE;
			else
				return FALSE;
	}

	return TRUE;
}



/****************************************************************
**	Routine:	ValFuncTranLogInfo
**	Returns:	TRUE or FALSE indicating success or failure, resp.
**
**	Value Type:	Transaction Log Info( obj )
**
**	Children:	Last Transaction( obj )
**
**	Action:		
****************************************************************/

static int ValFuncTranLogInfo(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					 Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	SDB_TRANS_HEADER
			Header;

	DT_VALUE
			EmptyRet,
		   *Ret;

	SDB_TRANS_ID
			LastTrans;

	int		ReturnEmptyValue;


	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "transaction log header of Last Transaction( obj )" );
			return TRUE;

		case SDB_MSG_VALIDATE:
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:
		{
			DT_VALUE
					ValueTypeVal;

			SDB_CHILD_LIST_ITEM
					Item;

			DTM_INIT( &ValueTypeVal );
			ValueTypeVal.DataType       = DtValueType;
			ValueTypeVal.Data.Pointer   = SecDbValueTypeFromName( "Last Transaction", NULL );
			Item.ValueType.Element		= MsgData->GetChildList.LiteralAdd( &ValueTypeVal, DtValueType );
			Item.ValueType.Type         = SDB_CHILD_LIST_LITERAL;
			Item.Result.Type			= SDB_CHILD_LIST_TERMINAL;
			Item.Name      		      	= "LastTransaction";
			if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				return FALSE;
			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
			if( getenv( "SECDB_TRANS_HDR_DISABLE" ))	// don't get trans header
				ReturnEmptyValue = TRUE;
			else if( MsgData->GetValue.ChildValues == 0 )
				ReturnEmptyValue = TRUE;
			else
			{
				Ret       = NULL;
				LastTrans = (SDB_TRANS_ID) MsgData->GetValue.ChildValues[ 0 ]->Data.Number;
				if( SecDbTransLogHeader( SecDbRootDb, LastTrans, &Header ))
					Ret = SecDbTransHeaderToStructure( &Header );
	
				if( !Ret )								// trans header not found
					ReturnEmptyValue = TRUE;
				else
				{
					MsgData->GetValue.Value->Data.Pointer = Ret->Data.Pointer;
					free( Ret );						// this was malloc'd, must free here
					ReturnEmptyValue = FALSE;
				}
			}

			if( ReturnEmptyValue )
			{
				if( !DTM_ALLOC( &EmptyRet, ValueTypeInfo->ValueType->DataType ))
					return FALSE;
				MsgData->GetValue.Value->Data.Pointer = EmptyRet.Data.Pointer;
			}
			return TRUE;
	}
	return TRUE;
}



/****************************************************************
**	Routine: DbObjFuncSecDbBase
**	Returns: TRUE/FALSE
**	Action : SecDb Base Class Object
**			 Virtual Base Class, should never instantiate
**			 an instance of it
****************************************************************/

int DbObjFuncSecDbBase(
	SDB_OBJECT	*SecPtr,
	int			Msg,
	SDB_M_DATA	*MsgData
)
{
	switch( Msg )
	{
		case SDB_MSG_START:
		{
			MsgData->Start.Class->ValueTable = ValueTableSecDbBase;
			if( !SecDbValueTypeRegisterClass( MsgData->Start.Class, ValueTableSecDbBase ))
				return ErrMore( "Registering Class Value Types for Class %s", MsgData->Start.Class->Name );

			return TRUE;
		}

		default:
			return SecDbDefaultObjFunc( SecPtr, Msg, MsgData );
	}
}



/****************************************************************
**	Routine: DbObjFuncSecDbBaseInterface
**	Returns: TRUE/FALSE
**	Action : SecDb Base Class Interface
****************************************************************/

int DbObjFuncSecDbBaseInterface(
	SDB_OBJECT	*SecPtr,
	int			Msg,
	SDB_M_DATA	*MsgData
)
{
	switch( Msg )
	{
		case SDB_MSG_START:
			MsgData->Start.Class->ValueTable = ValueTableSecDbBaseInterface;
			MsgData->Start.Class->Interface = 1;

			if( !SecDbValueTypeRegisterClass( MsgData->Start.Class, ValueTableSecDbBaseInterface ))
				return ErrMore( "Registering Class Value Types for Interface %s", MsgData->Start.Class->Name );
			return TRUE;

		default:
			return SecDbDefaultObjFunc( SecPtr, Msg, MsgData );
	}
}



/****************************************************************
**	Routine:	ValueFuncCast
**	Returns:	TRUE or FALSE indicating success or failure, resp.
**
**	Value Type:	<CastDt>( "~Cast", CastFrom )
**
****************************************************************/

static int ValueFuncCast(
	SDB_OBJECT			*SecPtr,
	int					Msg,
	SDB_M_DATA			*MsgData,
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo
)
{
	switch( Msg )
	{
		case SDB_MSG_GET_CHILD_LIST:
		{
			SDB_CHILD_LIST_ITEM
					Item;

			DT_VALUE
					TmpVal;

			DTM_INIT( &TmpVal );
			Item.Security.Type		= SDB_CHILD_LIST_CONSTANT;
			Item.Security.Element	= MsgData->GetChildList.LiteralAdd( &TmpVal, NULL );
			Item.Name				= "CastFrom";
			Item.Result.Type		= SDB_CHILD_LIST_ARGUMENT;
			if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				return FALSE;

			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
			return( DTM_TO( MsgData->GetValue.Value, MsgData->GetValue.ChildValues[ 0 ] ));
	}
	return Err( ERR_UNSUPPORTED_OPERATION, "%s( %s )", ValueTypeInfo->ValueType->Name, SecPtr->SecData.Name );
}



/****************************************************************
**	Routine:	ValFuncCastClassValueFunc
**	Returns:	TRUE/Err
**	Action:		
****************************************************************/

static int ValFuncCastClassValueFunc(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	if( Msg == SDB_MSG_GET_VALUE )
	{
		SDB_VALUE_TYPE
				ValueType = (SDB_VALUE_TYPE) MsgData->GetValue.ChildValues[ 0 ]->Data.Pointer;

		if( !SecPtr->SdbClass->LookupVti( ValueType->ID ))
		{
			SDB_VALUE_TYPE_INFO
					*Vti;

			if( !ERR_CALLOC( Vti, SDB_VALUE_TYPE_INFO, 1, sizeof( *Vti )))
				return FALSE;
			Vti->Name		= ValueType->Name;
			Vti->Func		= ValueFuncCast;
			Vti->ValueFlags	= SDB_CALCULATED;
			Vti->ValueType	= ValueType;
			(*SecPtr->SdbClass->ValueTypeMap)[ ValueType->ID ] = Vti;
		}
	}
	return ValFuncClassValueFunc( SecPtr, Msg, MsgData, ValueTypeInfo );
}



/****************************************************************
**	Routine: SecDbObjFuncCast
**	Returns: TRUE/FALSE
**	Action : SecDb Cast Object
****************************************************************/

int SecDbObjFuncCast(
	SDB_OBJECT	*SecPtr,
	int			Msg,
	SDB_M_DATA	*MsgData
)
{
	static SDB_VALUE_TYPE_INFO
			*ValueTable;

	switch( Msg )
	{
		case SDB_MSG_START:
		{
			SDB_VALUE_TYPE_INFO
					*Vti;

			ValueTable = new SDB_VALUE_TYPE_INFO[ sizeof( ValueTableSecDbBase ) / sizeof( SDB_VALUE_TYPE_INFO )];
			memcpy( ValueTable, ValueTableSecDbBase, sizeof( ValueTableSecDbBase ));

			for( Vti = ValueTable; Vti->Func != ValFuncClassValueFunc; ++Vti );
			Vti->Func = ValFuncCastClassValueFunc;

			MsgData->Start.Class->ValueTable = ValueTable;
			if( !SecDbValueTypeRegisterClass( MsgData->Start.Class, ValueTable ))
				return ErrMore( "Registering Class Value Types for Class %s", MsgData->Start.Class->Name );
			return TRUE;
		}

		default:
			return SecDbDefaultObjFunc( SecPtr, Msg, MsgData );
	}
}



/****************************************************************
**	Routine:	ValueFuncSubscript
**	Returns:	TRUE or FALSE indicating success or failure, resp.
**
**	Value Type:	<RetDt>( "~Subscript", Array, Index )
**
****************************************************************/

static int ValueFuncSubscript(
	SDB_OBJECT			*SecPtr,
	int					Msg,
	SDB_M_DATA			*MsgData,
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo
)
{
	switch( Msg )
	{
		case SDB_MSG_GET_CHILD_LIST:
		{
			SDB_CHILD_LIST_ITEM
					Item;

			DT_VALUE
					TmpVal;

			DTM_INIT( &TmpVal );
			Item.Security.Type		= SDB_CHILD_LIST_CONSTANT;
			Item.Security.Element	= MsgData->GetChildList.LiteralAdd( &TmpVal, NULL );
			Item.Name				= "Array";
			Item.Result.Type		= SDB_CHILD_LIST_ARGUMENT;
			if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				return FALSE;

			Item.Name				= "Subscript";
			if( MsgData->GetChildList.ChildAdd( &Item, SDB_CHILD_LIST_TERMINAL ) == -1 )
				return FALSE;

			return TRUE;
		}

		case SDB_MSG_GET_VALUE:
			return( DtSubscriptGetCopy( MsgData->GetValue.ChildValues[ 0 ], MsgData->GetValue.ChildValues[ 1 ], MsgData->GetValue.Value ));
	}
	return Err( ERR_UNSUPPORTED_OPERATION, "%s( %s )", ValueTypeInfo->ValueType->Name, SecPtr->SecData.Name );
}



/****************************************************************
**	Routine:	ValFuncSubscriptClassValueFunc
**	Returns:	TRUE/Err
**	Action:		
****************************************************************/

static int ValFuncSubscriptClassValueFunc(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	if( Msg == SDB_MSG_GET_VALUE )
	{
		SDB_VALUE_TYPE
				ValueType = (SDB_VALUE_TYPE) MsgData->GetValue.ChildValues[ 0 ]->Data.Pointer;

		if( !SecPtr->SdbClass->LookupVti( ValueType->ID ))
		{
			SDB_VALUE_TYPE_INFO
					*Vti;

			if( !ERR_CALLOC( Vti, SDB_VALUE_TYPE_INFO, 1, sizeof( *Vti )))
				return FALSE;
			Vti->Name		= ValueType->Name;
			Vti->Func		= ValueFuncSubscript;
			Vti->ValueFlags	= SDB_CALCULATED;
			Vti->ValueType	= ValueType;
			(*SecPtr->SdbClass->ValueTypeMap)[ ValueType->ID ] = Vti;
		}
	}
	return ValFuncClassValueFunc( SecPtr, Msg, MsgData, ValueTypeInfo );
}



/****************************************************************
**	Routine: SecDbObjFuncSubscript
**	Returns: TRUE/FALSE
**	Action : SecDb Subscript Object
****************************************************************/

int SecDbObjFuncSubscript(
	SDB_OBJECT	*SecPtr,
	int			Msg,
	SDB_M_DATA	*MsgData
)
{
	static SDB_VALUE_TYPE_INFO
			*ValueTable;

	switch( Msg )
	{
		case SDB_MSG_START:
		{
			SDB_VALUE_TYPE_INFO
					*Vti;

			ValueTable = new SDB_VALUE_TYPE_INFO[ sizeof( ValueTableSecDbBase ) / sizeof( SDB_VALUE_TYPE_INFO )];
			memcpy( ValueTable, ValueTableSecDbBase, sizeof( ValueTableSecDbBase ));

			for( Vti = ValueTable; Vti->Func != ValFuncClassValueFunc; ++Vti );
			Vti->Func = ValFuncSubscriptClassValueFunc;

			MsgData->Start.Class->ValueTable = ValueTable;
			if( !SecDbValueTypeRegisterClass( MsgData->Start.Class, ValueTable ))
				return ErrMore( "Registering Class Value Types for Class %s", MsgData->Start.Class->Name );
			return TRUE;
		}

		default:
			return SecDbDefaultObjFunc( SecPtr, Msg, MsgData );
	}
}



/***************************************************************
**	Routine:	ValFuncImplementedInterfaces
**	Returns:	TRUE/Err
**	Action:		
****************************************************************/

static int ValFuncImplementedInterfaces(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Gets the implemented interfaces for a class" );
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:
			return TRUE;

		case SDB_MSG_GET_VALUE:
		{
			DT_VALUE
					*Result;


			Result = MsgData->GetValue.Value;
			DTM_ALLOC( Result, DtStructure );
			
			if (SecPtr->SdbClass->ImplementedInterfaces)
			{
				SecDbInterfacesMap::iterator iter;
				for( iter = SecPtr->SdbClass->ImplementedInterfaces->begin();
					iter != SecPtr->SdbClass->ImplementedInterfaces->end(); ++iter )
					DtComponentSetPointer( Result, (*iter).first->Name, DtString, (*iter).second );
			}
			return TRUE;
		}		
	}

	return TRUE;
}

/****************************************************************
**	Routine:	SecDbValueFuncDefault
**	Returns:	TRUE
**
**	Default value func which does nothing
**
**	Action:		
****************************************************************/

int SecDbValueFuncDefault(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	return TRUE;
}



/****************************************************************
**	Routine: SecDbValFuncValueTypeArgs
**	Returns: 
**	Action : 
****************************************************************/

int SecDbValFuncValueTypeArgs(
	SDB_OBJECT			*SecPtr,		// Pointer to security to operate on
	int					Msg,			// Type of security
	SDB_M_DATA			*MsgData,		// Data used for operation
	SDB_VALUE_TYPE_INFO	*ValueTypeInfo	// Value type information
)
{
	switch( Msg )
	{
		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Returns VtApply list for all nodes with args of this value type" );
			return TRUE;

		case SDB_MSG_GET_CHILD_LIST:	// no Child list, implicitly invalidated via Create/Destroy
		{
			SDB_CHILD_LIST_ITEM item;
			DT_VALUE tmp;

			DTM_INIT( &tmp );

			item.Result.Type	= SDB_CHILD_LIST_ARGUMENT;
			item.Security.Type	= SDB_CHILD_LIST_CONSTANT;
			item.Security.Element=MsgData->GetChildList.LiteralAdd( &tmp, DtNull );
			item.Name			= "ValueType";
			return MsgData->GetChildList.ChildAdd( &item, SDB_CHILD_LIST_TERMINAL ) != -1;
		}

		case SDB_MSG_GET_VALUE:
		{
			if( !DtSecDbNode )
				DtFromName( "SecDb Node" );

			struct res_guard
			{
				res_guard( DT_VALUE* val ) : m_val( val ) {}
				~res_guard() { if( m_val ) DTM_FREE( m_val ); }

				DT_VALUE* release() { DT_VALUE* val = m_val; m_val = 0; return val; }

			private:
				DT_VALUE* m_val;
			};

			SDB_NODE* node = MsgData->GetValue.Node;

			SDB_NODE tmp_node;
			memset( &tmp_node, 0, sizeof( tmp_node ));

			SDB_NODE* children[3];
			children[0] = node->ObjectGet();
			children[1] = node->ArgGet( 0 );
			children[2] = 0;
			tmp_node.m_children = children;

			DT_VALUE res;
			DTM_ALLOC( &res, DtArray );
			res_guard _res( &res );

			for( SDB_NODE::nodes_iterator it = SDB_NODE::nodes().lower_bound( &tmp_node ); it != SDB_NODE::nodes().end(); ++it )
			{
				if( (*it)->ValueTypeGet() != node->ArgGet( 0 ) || (*it)->ObjectGet() != node->ObjectGet() )
					break;

				SDB_NODE* node1 = (*it)->FindAgain( SDB_NODE::DO_NOT_CREATE, node->DbSetGet()->BaseDb(), SDB_NODE::SHARE );
				if( node1 )
				{
					DtAppendPointer( &res, DtSecDbNode, node1 );
					node->AddAutoChild( node1 );
				}
			}
			res.DataType = DtEach;		// FIX- sleazy
			*MsgData->GetValue.Value = *_res.release();
			return TRUE;
		}		
	}

	return TRUE;
}


/****************************************************************
**	Routine: DbObjFuncSecDbUnStorable
**	Returns: TRUE/FALSE
**	Action : SecDb UnStorable Interface
****************************************************************/

int DbObjFuncSecDbUnStorable(
	SDB_OBJECT	*SecPtr,
	int			Msg,
	SDB_M_DATA	*MsgData
)
{
	switch( Msg )
	{
		case SDB_MSG_START:
			MsgData->Start.Class->ValueTable = ValueTableSecDbUnStorable;
			MsgData->Start.Class->Interface = 1;

			if( !SecDbValueTypeRegisterClass( MsgData->Start.Class, ValueTableSecDbUnStorable ))
				return ErrMore( "Registering Class Value Types for Interface %s", MsgData->Start.Class->Name );
			return TRUE;

		default:
			return SecDbDefaultObjFunc( SecPtr, Msg, MsgData );
	}
}

/****************************************************************
**	Routine: DbObjFuncSecDbEmptyClass
**	Returns: TRUE/FALSE
**	Action : SecDb Empty Class
****************************************************************/

int DbObjFuncSecDbEmptyClass(
	SDB_OBJECT	*SecPtr,
	int			Msg,
	SDB_M_DATA	*MsgData
)
{
	switch( Msg )
	{
		case SDB_MSG_START:
			MsgData->Start.Class->ValueTable = ValueTableSecDbEmptyClass;

			if( !SecDbValueTypeRegisterClassOld( MsgData->Start.Class, ValueTableSecDbEmptyClass ))
				return ErrMore( "Registering Class Value Types for class %s", MsgData->Start.Class->Name );
			return TRUE;

		default:
			return SecDbDefaultObjFunc( SecPtr, Msg, MsgData );
	}
}
