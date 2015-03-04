#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbninit.c,v 1.93 2012/06/18 20:11:13 khodod Exp $"
/****************************************************************
**
**	SDBNINIT.C
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbninit.c,v $
**	Revision 1.93  2012/06/18 20:11:13  khodod
**	Roll back the HASH_QUICK implementation for DtEach for now.
**	AWR: #NA
**
**	Revision 1.87  2004/11/29 23:48:12  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<bstream.h>
#include	<heap.h>
#include	<heapinfo.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<sectrace.h>
#include	<sdb_int.h>
#include	<sdbdbset.h>
#include    <dterr.h>
#include    <gscore/GsException.h>
#include    <secdb/GsNodeValues.h>

#include    <sstream>
CC_USING( std::ostringstream );

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

/*
**	Variables
*/

DT_DATA_TYPE
		DtValueType,
		DtPassThrough,
		DtValueArray,
		DtEach,
		DtNodeFlags,
		DtValueTypeInfo,
		DtEllipsis,
		DtVtApply,
		DtGsNodeValues;

long
		SecDbGetValueDepth = 0;

SDB_NODE_LOCKER
		*NodeLockers = NULL;


/****************************************************************
**	Routine: DtFuncEllipsis
**	Returns:
**	Action :
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncEllipsis"

static int DtFuncEllipsis(
	int			Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size = sizeof( a->Data.Pointer );
			a->DataType->Flags = DT_FLAG_POINTER | DT_FLAG_EVAL_ONCE_CONSTRUCTOR;
			return TRUE;

		case DT_MSG_ALLOCATE:
			r->DataType = DtEllipsis;
			r->Data.Pointer = NULL;
			return TRUE;

		case DT_MSG_TO:
			if( r->DataType == a->DataType )
				r->Data.Pointer = a->Data.Pointer;
			else if( r->DataType == DtString )
				r->Data.Pointer = strdup( "..." );
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "Can't convert Ellipsis to '%s'", r->DataType->Name );
			return TRUE;
	}
	return DtFuncNull( Msg, r, a, b );
}


/****************************************************************
**	Routine: DtValueTypeInfoFromVti
**	Returns: 
**	Action : 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtValueTypeInfoFromVti"

int DtValueTypeInfoFromVti(
	DT_VALUE			*DtVti,
	SDB_VALUE_TYPE_INFO *Vti
)
{
	if( !DtVti )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": DtVti missing" );

	DTM_ALLOC( DtVti, DtValueTypeInfo );

	DT_VALUE_TYPE_INFO
			*A = (DT_VALUE_TYPE_INFO *) DT_VALUE_TO_POINTER( DtVti );

	A->Free	= FALSE;
	A->Vti	= Vti;

	return TRUE;
}


/****************************************************************
**	Routine: DtFuncValueTypeInfo
**	Returns:
**	Action :
****************************************************************/

static int DtFuncValueTypeInfo(
	int			Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	enum
	{
		VTI_NAME = 0,
		VTI_DATA_TYPE,
		VTI_FLAGS,
		VTI_VALUE_TYPE,
		VTI_PARENT,
		VTI_CLASS,
		VTI_DESCRIPTION,
		VTI_DATA,
		VTI_CHILD_LIST_INFO,
		VTI_LAST_SUBSCRIPT
	};

	static DT_HELP
			Help[] =
	{
		{ "From:String",	"Convert Strings of the form 'ValueType;Class' to the VTI defined for that Class" },
		{ "To:String",		"Return the description of the VTI to which we are bound" },
		{ "To:Structure",	"Return { Name, DataType } for the VTI. Should be more descriptive" },
		{ NULL,				"Value Type Info() is the datatype which SecDb uses to store all information about a ValueType for a class.\n"
				  "You can construct it from a structure by calling the function ValueTypeInfoFromStruct(), which takes a structure in the format"
				  "of a UFO value table element." },
		{ NULL,				NULL }
	};

	static const char *VTISubscripts[] =
	{
		"Name",
		"DataType",
		"Flags",
		"ValueType",
		"Parent",
		"Class",
		"Description",
		"Data",
		"ChildListInfo",
		NULL
	};

	static HASH
			*SubscriptNames;

	DT_INFO	*DataTypeInfo;

	DT_VALUE_TYPE_INFO
			*A,
			*B;




	if( a )
		A = (DT_VALUE_TYPE_INFO*) a->Data.Pointer;
	else
		A = NULL;
	if( b )
		B = (DT_VALUE_TYPE_INFO*) b->Data.Pointer;
	else
		B = NULL;

	switch( Msg )
	{
		case DT_MSG_START:
		{
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_REFERENCE | DT_FLAG_COMPONENTS;
			a->DataType->Help	= Help;

			SubscriptNames = HashCreate( "VTI Subscript Names Hash", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );

			for( const char **Subscript = VTISubscripts; *Subscript; ++Subscript )
				HashInsert( SubscriptNames, (HASH_KEY) *Subscript, (HASH_VALUE) ( Subscript - VTISubscripts + 1 ));

			return TRUE;
		}

		case DT_MSG_ALLOCATE:
			r->DataType = DtValueTypeInfo;
			A = (DT_VALUE_TYPE_INFO*) calloc( 1, sizeof( DT_VALUE_TYPE_INFO ));
			A->Vti = NULL;
			A->Free = TRUE;
			A->Refcount = 1;
			r->Data.Pointer = A;
			return TRUE;

		case DT_MSG_FREE:
			if( --A->Refcount == 0 )
			{
				if( A->Free )
				{
					SecDbVtiDelete( A->Vti );
					free( A->Vti );			 	// SecDbVtiDelete only frees Vti's for ~value types
				}
				free( A );
			}
			a->Data.Pointer = NULL;
			return TRUE;

		case DT_MSG_INFO:
			DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags = DT_FLAG_POINTER | DT_FLAG_REFERENCE;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;

		case DT_MSG_ASSIGN:
			++A->Refcount;
			r->DataType = a->DataType;
			r->Data.Pointer = A;
			return TRUE;

		case DT_MSG_TRUTH:
			return a->Data.Pointer != NULL;

		case DT_MSG_EQUAL:
			r->DataType = DtDouble;
			r->Data.Number = (double)( a->Data.Pointer == b->Data.Pointer );
			return TRUE;

		case DT_MSG_NOT_EQUAL:
			r->DataType = DtDouble;
			r->Data.Number = (double)( a->Data.Pointer != b->Data.Pointer );
			return TRUE;

		case DT_MSG_TO:
			if( r->DataType == a->DataType )
			{
				++A->Refcount;
				r->Data.Pointer = A;
			}
			else if( r->DataType == DtString )
			{
				if( A->Vti )
				{
					r->Data.Pointer = StrPaste( A->Vti->Name, "( ", A->Vti->Class ? A->Vti->Class->Name : "<unknown class>", " )", NULL );
				}
				else
					r->Data.Pointer = strdup( "" );
			}
			else if( r->DataType == DtStructure )
			{
				DTM_ALLOC( r, DtStructure );
				if( A && A->Vti )
				{
					DT_VALUE
							Enum,
							Value;

					DTM_FOR( &Enum, a )
					{
						if (DtSubscriptGetCopy( a, &Enum, &Value ))
							DtComponentReplace( r, &Enum, &Value );
					}
				}
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", a->DataType->Name, r->DataType->Name );
			return TRUE;

		case DT_MSG_FROM:
			if( b->DataType == a->DataType )
			{
				++B->Refcount;
				a->Data.Pointer = B;
			}
			else if( b->DataType == DtString )
			{
				char	*Str = strdup( (char *) b->Data.Pointer ),
						*ValueTypeName = strtok( Str, ";" ),
						*ClassName = strtok( NULL, ";" );

				if( !ValueTypeName || !ClassName )
				{
					free( Str );
					return Err( ERR_INVALID_ARGUMENTS, "Expected ValueType;ClassName, not '%s'", (char *) b->Data.Pointer );
				}

				SDB_VALUE_TYPE
						ValueType = SecDbValueTypeFromName( ValueTypeName, NULL );

				if( !ValueType )
				{
					free( Str );
					return ErrMore( "Invalid value type in '%s'", (char *) b->Data.Pointer );
				}

				SDB_SEC_TYPE
						ClassType = SecDbClassTypeFromName( ClassName );

				free( Str );

				if( !ClassType )
					return ErrMore( "Invalid class in '%s'", (char *) b->Data.Pointer );

				SDB_CLASS_INFO
						*ClassInfo = SecDbClassInfoFromType( ClassType );

				if( !ClassInfo )
					return ErrMore( "Invalid class in '%s'", (char *) b->Data.Pointer );

				SDB_VALUE_TYPE_INFO* Vti = ClassInfo->LookupVti( ValueType->ID );
				if( !Vti )
					return Err( ERR_UNSUPPORTED_OPERATION, "Class '%s' does not support '%s'", ClassInfo->Name, ValueType->Name );
				if( !DtValueTypeInfoFromVti( a, Vti ) )
					return ErrMore( "Failed to make VTI" );
				return TRUE;
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", b->DataType->Name, a->DataType->Name );
			return TRUE;

		case DT_MSG_HIGH_LIMIT:
		case DT_MSG_LOW_LIMIT:
			return Err( ERR_UNSUPPORTED_OPERATION, "@ - %s", DtMsgToString( Msg ) );

		case DT_MSG_SUBSCRIPT_NEXT:
			if( r->DataType == DtNull )
			{
				r->DataType = DtString;
				r->Data.Pointer = VTISubscripts[0];
			}
			else
				r->Data.Pointer = VTISubscripts[ (long) HashLookup( SubscriptNames, r->Data.Pointer ) ];
			if( !r->Data.Pointer )
			{
				r->DataType = DtNull;
				return FALSE;
			}
			return TRUE;

		case DT_MSG_SUBSCRIPT_LAST:
			r->DataType = DtString;
			r->Data.Pointer = VTISubscripts[ VTI_LAST_SUBSCRIPT - 1 ];
			return TRUE;

		case DT_MSG_COMPONENT_VALUE:
		case DT_MSG_SUBSCRIPT_VALUE:
		{		
			if( b->DataType != DtString )
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a String" );

			int Subscript = (long) HashLookup( SubscriptNames, b->Data.Pointer );
			if( !Subscript || !A || !A->Vti )
				return Err( DT_ERR_COMPONENT_NOT_EXIST, "@: %s[ '%s' ]", a->DataType->Name, (char *) b->Data.Pointer );

			switch( Subscript - 1 )
			{
				case VTI_NAME:
					r->DataType = DtString;
					r->Data.Pointer = strdup( A->Vti->Name );
					break;

				case VTI_DATA_TYPE:
					if( A->Vti->DataType )
						DTM_ALLOC( r, *A->Vti->DataType );
					else
						DTM_INIT( r );
					break;

				case VTI_FLAGS:
					r->DataType = DtDouble;
					r->Data.Number = A->Vti->ValueFlags;
					break;

				case VTI_VALUE_TYPE:
					r->DataType = DtValueType;
					r->Data.Pointer = A->Vti->ValueType;
					break;

				case VTI_PARENT:
					if( A->Vti->Parent )
					{
						DTM_ALLOC( r, DtValueTypeInfo );

						DT_VALUE_TYPE_INFO *R = (DT_VALUE_TYPE_INFO *) r->Data.Pointer;
						R->Vti = A->Vti->Parent;
						R->Free = FALSE;
					}
					else
						DTM_INIT( r );
					break;

				case VTI_CLASS:
					if( A->Vti->Class )
					{
						r->DataType = DtString;
						r->Data.Pointer = strdup( A->Vti->Class->Name );
					}
					else
						DTM_INIT( r );
					break;

				case VTI_DESCRIPTION:
				{
					SDB_M_DATA
							MsgData;

					MsgData.Description = NULL;
					if ( !SDB_MESSAGE_VT_TRACE( NULL, SDB_MSG_DESCRIPTION, &MsgData, A->Vti ))
						return ErrMore( "ValueTypeInfo Description failed" );

					if( !( r->Data.Pointer = MsgData.Description) )
						r->Data.Pointer = strdup( A->Vti->Name );
					r->DataType = DtString;
					break;
				}

				case VTI_DATA:
				{
					SDB_M_DATA
							MsgData;

					DTM_INIT( &MsgData.ValueTypeData );
					if( !SDB_MESSAGE_VT_TRACE( NULL, SDB_MSG_GET_VALUE_TYPE_DATA, &MsgData, A->Vti ))
						return ErrMore( "ValueTypeInfo GetValueData failed" );

					*r = MsgData.ValueTypeData;
					break;
				}

				case VTI_CHILD_LIST_INFO:
				{
					if( !A->Vti->Class )
						return Err( ERR_UNKNOWN, "VTI has no parent class" );

					dt_value_var ret;
					if( !SecDbChildListInfo( A->Vti->Class->ClassSec, A->Vti->ValueType, A->Vti, ret.get() ) )
						return ErrMore( "VTI.ChildListInfo" );

					*r = ret.release();
					break;
				}
			}
			return TRUE;
		}					

	}
	return Err( ERR_UNSUPPORTED_OPERATION, "%s: @ %s", a->DataType->Name, DtMsgToString( Msg ) );
}



/****************************************************************
**	Routine: DtFuncValueType
**	Returns: TRUE or FALSE
**	Action : Value type datatype func
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncValueType"

static int DtFuncValueType(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	DT_INFO
			*DataTypeInfo;

	BSTREAM	*BStream;


	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_REFERENCE | DT_FLAG_VALUE_TYPE;
			return TRUE;

		case DT_MSG_INFO:
			DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags = DT_FLAG_POINTER | DT_FLAG_REFERENCE;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;

		case DT_MSG_ASSIGN:
			*r = *a;
			return TRUE;

		case DT_MSG_TRUTH:
			return a->Data.Pointer != NULL;

		case DT_MSG_EQUAL:
			r->DataType = DtDouble;
			r->Data.Number = (double)( a->Data.Pointer == b->Data.Pointer );
			return TRUE;

		case DT_MSG_NOT_EQUAL:
			r->DataType = DtDouble;
			r->Data.Number = (double)( a->Data.Pointer != b->Data.Pointer );
			return TRUE;

		case DT_MSG_TO:
		{
			SDB_VALUE_TYPE
					ValueType;


			ValueType = (SDB_VALUE_TYPE) a->Data.Pointer;
			if( r->DataType == a->DataType )
				r->Data.Pointer = ValueType;
			else if( r->DataType == DtString )
				if( ValueType )
					r->Data.Pointer = strdup( ValueType->Name );
				else
					r->Data.Pointer = strdup( "" );
			else if( r->DataType == DtStructure )
			{
				DTM_ALLOC( r, DtStructure );
				if( ValueType )
				{
					DtComponentSetPointer( r, "Name",     DtString, ValueType->Name );
					DtComponentSetPointer( r, "DataType", DtString, ValueType->DataType->Name );
					DtComponentSetNumber(  r, "ID",       DtDouble, ValueType->ID );
				}
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", a->DataType->Name, r->DataType->Name );
			return TRUE;
		}

		case DT_MSG_FROM:
			if( b->DataType == a->DataType )
				a->Data.Pointer = b->Data.Pointer;
			else if( b->DataType == DtString )
				a->Data.Pointer = SecDbValueTypeFromName( (char *) b->Data.Pointer, NULL );
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", b->DataType->Name, a->DataType->Name );
			return TRUE;

		case DT_MSG_FORMAT:
		{
			DT_VALUE
					StringVal;

			int		Status;

			StringVal.DataType = DtString;
			Status = DTM_TO( &StringVal, a );
			if( Status )
			{
				Status = DT_OP( Msg, r, &StringVal, b );
				DTM_FREE( &StringVal );
			}
			return( Status );
		}

		case DT_MSG_TO_STREAM:
			BStream = (BSTREAM *) r->Data.Pointer;
			BStreamPutString( BStream, (const char *)((SDB_VALUE_TYPE)a->Data.Pointer)->Name );
			if( BStream->Overflow )
				return Err( ERR_OVERFLOW, "@ - Streaming %s", a->DataType->Name );
			return TRUE;

		case DT_MSG_FROM_STREAM:
			BStream = (BSTREAM *) r->Data.Pointer;
			a->Data.Pointer = SecDbValueTypeFromName( BStreamGetStringPtr( BStream ), NULL );
			if( !a->Data.Pointer || BStream->Overflow )
				return Err( ERR_UNDERFLOW, "@ - Unstreaming %s", a->DataType->Name );
			return TRUE;

		case DT_MSG_HASH_QUICK:
		{
			unsigned long
					Hash1 = (unsigned long) a->Data.Pointer,
					Hash2 = (unsigned long) DtValueType,
					Hash3 = *(DT_HASH_CODE *) r->Data.Pointer;

			DT_HASH_MIX( Hash1, Hash2, Hash3 );
			*(DT_HASH_CODE *) r->Data.Pointer = Hash3;
			return TRUE;
		}
	}
	return DtFuncNull( Msg, r, a, b );
}

/****************************************************************
**	Routine: DtFuncNodeFlags
**	Returns: TRUE or FALSE
**	Action : Node modifiers
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncNodeFlags"

static int DtFuncNodeFlags(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size = sizeof( a->Data.Number );
			return TRUE;

		case DT_MSG_INFO:
			r->Data.Pointer = calloc( 1, sizeof( DT_INFO ));
			return TRUE;

		case DT_MSG_ASSIGN:
			*r = *a;
			return TRUE;

		case DT_MSG_FREE:
			return TRUE;

		case DT_MSG_SIZE:
		case DT_MSG_MEMSIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= (double) 0;
			return TRUE;

		case DT_MSG_TRUTH:
			return (DT_NODE_FLAGS) a->Data.Number != 0;

		case DT_MSG_EQUAL:
			r->DataType = DtDouble;
			r->Data.Number = (double)( a->Data.Number == b->Data.Number && a->DataType == b->DataType );
			return TRUE;

		case DT_MSG_NOT_EQUAL:
			r->DataType = DtDouble;
			r->Data.Number = (double)( a->Data.Number != b->Data.Number || a->DataType != b->DataType );
			return TRUE;

		case DT_MSG_FROM:
			if( b->DataType == DtDouble )
			{
				a->DataType = DtNodeFlags;
				a->Data.Number = b->Data.Number;
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ cannot convert %s to %s", b->DataType->Name, a->DataType->Name );
			return TRUE;

		case DT_MSG_TO:
		{
			DT_NODE_FLAGS
					NodeFlags = (DT_NODE_FLAGS) a->Data.Number;


			if( r->DataType == a->DataType )
				*r = *a;
			else if( r->DataType == DtString )
			{
				ostringstream stream;

				const char
						*Delim = "";

				if( NodeFlags & SDB_PASS_ERRORS )
				{
					stream << Delim << "Pass Errors";
					Delim = " | ";
				}
				if( NodeFlags & SDB_ARRAY_RETURN )
				{
					stream << Delim << "Array Return";
					Delim = " | ";
				}
				if( NodeFlags & SDB_PREPEND_ARGS )
				{
					stream << Delim << "Prepend Args";
					Delim = " | ";
				}
				r->Data.Pointer = strdup( stream.str().c_str() );
			}
			else if( r->DataType == DtStructure )
			{
				DTM_ALLOC( r, DtStructure );
				DtComponentSetNumber( r, "Pass Errors",  DtDouble, (double) SDB_PASS_ERRORS == ( NodeFlags & SDB_PASS_ERRORS ));
				DtComponentSetNumber( r, "Array Return", DtDouble, (double) SDB_ARRAY_RETURN == ( NodeFlags & SDB_ARRAY_RETURN ));
				DtComponentSetNumber( r, "Prepend Args", DtDouble, (double) SDB_PREPEND_ARGS == ( NodeFlags & SDB_PREPEND_ARGS ));
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - Can't convert %s to a %s", a->DataType->Name, r->DataType->Name );
			return TRUE;
		}
	}
	return DtFuncNull( Msg, r, a, b );
}



/****************************************************************
**	Routine: DtFuncPassThrough
**	Returns: TRUE or FALSE
**	Action : Pass through datatype (Data.Pointer points to the real value)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncPassThrough"

static int DtFuncPassThrough(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size = 0;
			return TRUE;

		case DT_MSG_INFO:
			r->Data.Pointer = calloc( 1, sizeof( DT_INFO ));
			return TRUE;

		case DT_MSG_ASSIGN:
			return( Err( ERR_UNSUPPORTED_OPERATION, "@" ));

		case DT_MSG_FREE:
			return TRUE;

		case DT_MSG_SIZE:
		case DT_MSG_MEMSIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= (double) 0;
			return TRUE;

		case DT_MSG_FROM:
			if( b->DataType->Flags & DT_FLAG_NUMERIC )
				*a = *b;	// a little dicey perhaps--changing the datatype of a...
			else
				a->Data.Pointer = b;
			return( TRUE );
	}
	if( !a || !a->Data.Pointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@ - No pass through pointer" ));
	return( DT_OP( Msg, r, (DT_VALUE *) a->Data.Pointer, b ));
}



/****************************************************************
**	Routine: DtFuncValueArray
**	Returns: TRUE or FALSE
**	Action : Value Array datatype (== array)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncValueArray"

static int DtFuncValueArray(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	int		Status,
			ChangeB,
			ChangeRBefore,
			ChangeRAfter;


	ChangeB = b && b->DataType == DtValueArray;

	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size 	= sizeof( a->Data.Pointer );
			a->DataType->Flags 	= DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS | DT_FLAG_VALUE_TYPE | DT_FLAG_SDB_OBJECT;
			return( TRUE );

		case DT_MSG_TO:
			ChangeRBefore	=
			ChangeRAfter	= r->DataType == DtValueArray;
			break;

		case DT_MSG_ALLOCATE:
		case DT_MSG_ADD:
		case DT_MSG_SUBTRACT:
		case DT_MSG_MULTIPLY:
		case DT_MSG_DIVIDE:
		case DT_MSG_AND:
		case DT_MSG_SHIFT:
		case DT_MSG_UNION:
		case DT_MSG_ASSIGN:
		case DT_MSG_UNARY_MINUS:
		case DT_MSG_INCREMENT:
		case DT_MSG_DECREMENT:
		case DT_MSG_INVERT:
			ChangeRBefore	= FALSE;
			ChangeRAfter	= TRUE;
			break;

		default:
			ChangeRBefore	= FALSE;
			ChangeRAfter	= FALSE;
			break;
	}

	if( a )
		a->DataType = DtArray;
	if( ChangeB )
		b->DataType = DtArray;
	if( ChangeRBefore )
		r->DataType = DtArray;

	Status = (*DtArray->Func)( Msg, r, a, b );

	if( a )
		a->DataType = DtValueArray;
	if( ChangeB )
		b->DataType = DtValueArray;
	if( ChangeRAfter && Status && r->DataType == DtArray )
		r->DataType = DtValueArray;

	if( !Status )
		ErrMore( "Value Array" );
	return( Status );
}



/****************************************************************
**	Routine: DtFuncEach
**	Returns: TRUE or FALSE
**	Action : Each datatype (== array)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncEach"

static int DtFuncEach(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	int		Status,
			ChangeA,
			ChangeB,
			ChangeRBefore,
			ChangeRAfter;

	ChangeA = !!a;
	ChangeB = b && b->DataType == DtEach;

	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size 	= sizeof( a->Data.Pointer );
			a->DataType->Flags 	= DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS | DT_FLAG_VALUE_TYPE | DT_FLAG_SDB_OBJECT;
			return( TRUE );

		case DT_MSG_TO:
			ChangeRBefore	=
			ChangeRAfter	= r->DataType == DtEach;
			break;

		case DT_MSG_ALLOCATE:
		case DT_MSG_ADD:
		case DT_MSG_SUBTRACT:
		case DT_MSG_MULTIPLY:
		case DT_MSG_DIVIDE:
		case DT_MSG_AND:
		case DT_MSG_SHIFT:
		case DT_MSG_UNION:
		case DT_MSG_ASSIGN:
		case DT_MSG_UNARY_MINUS:
		case DT_MSG_INCREMENT:
		case DT_MSG_DECREMENT:
		case DT_MSG_INVERT:
			ChangeRBefore	= FALSE;
			ChangeRAfter	= TRUE;
			break;

		case DT_MSG_HASH_QUICK:
			ChangeA 		= FALSE;
			ChangeRBefore	= FALSE;
			ChangeRAfter	= FALSE;
			break;

		default:
			ChangeRBefore	= FALSE;
			ChangeRAfter	= FALSE;
			break;
	}

	if( ChangeA )
		a->DataType = DtArray;
	if( ChangeB )
		b->DataType = DtArray;
	if( ChangeRBefore )
		r->DataType = DtArray;

	Status = (*DtArray->Func)( Msg, r, a, b );

	if( a )
		a->DataType = DtEach;
	if( ChangeB )
		b->DataType = DtEach;
	if( ChangeRAfter && Status && r->DataType == DtArray )
		r->DataType = DtEach;

	if( !Status )
		ErrMore( "Each" );
	return( Status );
}


/****************************************************************
**	Routine: DtFuncGsNodeValues
**	Returns: TRUE or FALSE
**	Action : DtGsNodeValues datatype
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncGsNodeValues"

static int DtFuncGsNodeValues(
	int Msg,
	DT_VALUE* r,
	DT_VALUE* a,
	DT_VALUE* b
)
{
	GsNodeValues* NodeValues = NULL;

	if( a )
		NodeValues =static_cast< GsNodeValues* >( const_cast< void *>( a->Data.Pointer ) );

	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS;
			return TRUE;
			
		case DT_MSG_ALLOCATE:
			r->DataType = DtGsNodeValues;
			r->Data.Pointer = 0;
			return TRUE;
			
		case DT_MSG_FREE:
			delete NodeValues;
			a->Data.Pointer = NULL;
			return TRUE;

		case DT_MSG_ASSIGN:
			r->DataType = DtArray;
			return DT_OP( DT_MSG_TO, r, a, NULL );

		case DT_MSG_TRUTH:
			return TRUE;

		case DT_MSG_SIZE:
			r->DataType 	= DtDouble;
			r->Data.Number	= NodeValues ? NodeValues->size() : 0;
			return TRUE;


		case DT_MSG_EQUAL:
		case DT_MSG_NOT_EQUAL:
		{
			r->DataType = DtDouble;
			if ( ( a->DataType != b->DataType ) && ( b->DataType != DtArray) )
				r->Data.Number = 0.0;
			else if ( a->DataType == b->DataType ) // RHs is GSNodeValues
			{
				GsNodeValues
						*Lhs = NodeValues,
						*Rhs = static_cast< GsNodeValues*>( const_cast< void* >( b->Data.Pointer ) );


				if( Lhs == Rhs )
					r->Data.Number = 1.0;
				else if( !Lhs || !Rhs || Lhs->size() != Rhs->size() )
					r->Data.Number = 0.0;
				else
				{
					r->Data.Number = 1.0;
					DT_VALUE
							Same;
					for( int Elem = 0; Elem < Lhs->size(); Elem++ )
						if( !DT_OP( DT_MSG_EQUAL, &Same, (*Lhs)[ Elem ], (*Rhs)[ Elem ] ) || !Same.Data.Number )
						{
							r->Data.Number = 0.0;
							break;
						}
				}
			}
			else if ( b->DataType == DtArray ) // RHs is DtArray
			{
				GsNodeValues
						*Lhs = NodeValues;
				DT_ARRAY
						*Rhs = (DT_ARRAY *) b->Data.Pointer;


				if( !Lhs || !Rhs || Lhs->size() != Rhs->Size )
					r->Data.Number = 0.0;
				else
				{
					r->Data.Number = 1.0;
					DT_VALUE
							Same;
					for( int Elem = 0; Elem < Lhs->size(); Elem++ )
					{
						// is an element is GsNodeValues, make sure it uses its comparison
						if((*Lhs)[ Elem ]->DataType == DtGsNodeValues ) {
							if( !DT_OP( DT_MSG_EQUAL, &Same, (*Lhs)[ Elem ], &Rhs->Element[ Elem ] ) || !Same.Data.Number )
							{
								r->Data.Number = 0.0;
								break;
							}
						}
						else
						{
							if( !DT_OP( DT_MSG_EQUAL, &Same, &Rhs->Element[ Elem ], (*Lhs)[ Elem ]  ) || !Same.Data.Number ) 
							{
								r->Data.Number = 0.0;
								break;
							}
						}
					}
				}
			}
			if( DT_MSG_NOT_EQUAL == Msg )
				r->Data.Number = (r->Data.Number ? 0.0 : 1.0);
			return TRUE;
		}

		case DT_MSG_TO:
		{
			if( r->DataType == DtArray )
			{
				DTM_ALLOC( r, DtArray );
				if( NodeValues )
				{
					int Size = NodeValues->size();
					for( int Elem = 0; Elem < Size; Elem++ )
						DtAppend( r, (*NodeValues)[ Elem ] );

					return TRUE;
				}
			}
			else if ( r->DataType == DtSecurityList )
			{
				DT_SECURITY_LIST* SecList = (DT_SECURITY_LIST*)( r->Data.Pointer = 
																 calloc(1, sizeof( DT_SECURITY_LIST) ) );

				SecList->Size = NodeValues ? NodeValues->size() : 0;
				SecList->SecPtrs = (SDB_OBJECT **) calloc( SecList->Size, sizeof( *SecList->SecPtrs ) );

				for( int Elem = 0; Elem < SecList->Size; Elem++ )
				{
					DT_VALUE Security;
					Security.DataType = DtSecurity;
					if( DTM_TO( &Security, (*NodeValues)[ Elem ] ) )
						SecList->SecPtrs[ Elem ] = static_cast< SDB_OBJECT* >( const_cast< void* >( Security.Data.Pointer ) );
					else
						SecList->SecPtrs[ Elem ] = NULL;
				}
				return TRUE;
			}
			else // Since this used to be an ARRAY let just turn it into one first
			{
				DT_VALUE ArrayVal;
				ArrayVal.DataType = DtArray;
				if( DTM_TO( &ArrayVal, a ) )
				{
					int Success = DTM_TO( r, &ArrayVal );
					DTM_FREE( &ArrayVal );
					
					if( Success )
						return Success;
					else
						return Err( ERR_UNSUPPORTED_OPERATION, "@" );
				}
			}
			return Err( ERR_UNSUPPORTED_OPERATION, "@" );
		}

	case DT_MSG_SUBSCRIPT:
		{
			int Elem = 0;
			if( b->DataType == DtDouble )
				Elem = static_cast< int >( b->Data.Number );
			else
				return Err( ERR_INVALID_ARGUMENTS, "@ - Subscript must be a double not a %s", b->DataType->Name );
			

			DT_VALUE* ChildValue = (*NodeValues)[ Elem ];

			if( ChildValue )
			{
				r->Data.Pointer = ChildValue;
				return TRUE;
			}
			else
			{
				DT_VALUE NullValue;
				DTM_INIT( &NullValue );
				r->Data.Pointer = &NullValue;
				return TRUE;
			}
		}


		case DT_MSG_SUBSCRIPT_REPLACE:
			return Err( ERR_UNSUPPORTED_OPERATION, 
						"Node GsNodeValues: Subscript Replace not supported, elements are read only." );


		case DT_MSG_HASH_PORTABLE:
		{
			DT_HASH_CODE
					Hash = *(DT_HASH_CODE *)r->Data.Pointer;


			int Status = TRUE;

			if( NodeValues )
			{
				for( int Elem = 0; Elem < NodeValues->size(); Elem++ )
					if( !DtHashPortable( (*NodeValues)[ Elem ], &Hash ))
					{
						Status = ErrMore( "Could not hash array element #%d", Elem );
						break;
					}
				if( Status )
					*(DT_HASH_CODE *)r->Data.Pointer = Hash;
			}

			return Status;
		}

		case DT_MSG_HASH_QUICK:
		{
			DT_HASH_CODE
					CurHash = *(DT_HASH_CODE *) r->Data.Pointer;

			int Status = TRUE;

			// Always hash in datatype so that a single element array does not hash
			// to the same place as the value of the single element.
			{
				unsigned long
						Hash1 = (unsigned long) DtArray,
						Hash2 = 0;

				DT_HASH_MIX( Hash1, Hash2, CurHash );
			}

			if( NodeValues )
			{
				DT_VALUE
						CurHashVal;

				size_t
						ElemIncr;

				CurHashVal.Data.Pointer = &CurHash;

				ElemIncr = ( NodeValues->size() + 7 ) >> 3;

				for( int Elem = 0; Elem < NodeValues->size(); Elem += ElemIncr )
				{
					DT_HASH_CODE
							SavedHash = CurHash;

					if( !DT_OP( DT_MSG_HASH_QUICK, &CurHashVal, (*NodeValues)[ Elem ], NULL )
						&& (CurHash = SavedHash, !DtHashPortable( (*NodeValues)[ Elem ], &CurHash )))
					{
						Status = ErrMore( "Could not hash array element #%d", Elem );
						break;
					}
				}

				if( Status )
					*(DT_HASH_CODE *) r->Data.Pointer = CurHash;
			}
			return Status;
		}

		default: // Convert to DT_ARRAY and call message for it.
		{
			if( !a )
				return Err( ERR_UNSUPPORTED_OPERATION, "@ - %s", DtMsgToString( Msg ) );

			DT_VALUE ArrayValA;
			ArrayValA.DataType = DtArray;
			if( DTM_TO( &ArrayValA, a ) )
			{
				int Success;
				if( b && b->DataType == DtGsNodeValues )
				{
					DT_VALUE
							ArrayValB;
					ArrayValB.DataType = DtArray;
					if( DTM_TO( &ArrayValB, b ))
					{
						Success = DT_OP( Msg, r, &ArrayValA, &ArrayValB );
						DTM_FREE( &ArrayValB );
					}
					else
						Success = FALSE;
				}
				else
					Success = DT_OP( Msg, r, &ArrayValA, b );
				DTM_FREE( &ArrayValA );
				return Success;
			}
		}
	}

	return FALSE;
} 


/****************************************************************
**	Routine: DtFuncVtApply
**	Returns: TRUE or FALSE
**	Action : VtApply datatype (== array)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "DtFuncVtApply"

static int DtFuncVtApply(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	int		Status,
			ChangeB,
			ChangeRBefore,
			ChangeRAfter;


	ChangeB = b && b->DataType == DtVtApply;

	switch( Msg )
	{
		case DT_MSG_START:
			a->DataType->Size 	= sizeof( a->Data.Pointer );
			a->DataType->Flags 	= DT_FLAG_POINTER | DT_FLAG_SUBSCRIPTS | DT_FLAG_VALUE_TYPE;
			return( TRUE );

		case DT_MSG_TO:
			ChangeRBefore	=
			ChangeRAfter	= r->DataType == DtVtApply;
			break;

		case DT_MSG_ALLOCATE:
		case DT_MSG_ADD:
		case DT_MSG_SUBTRACT:
		case DT_MSG_MULTIPLY:
		case DT_MSG_DIVIDE:
		case DT_MSG_AND:
		case DT_MSG_SHIFT:
		case DT_MSG_UNION:
		case DT_MSG_ASSIGN:
		case DT_MSG_UNARY_MINUS:
		case DT_MSG_INCREMENT:
		case DT_MSG_DECREMENT:
		case DT_MSG_INVERT:
			ChangeRBefore	= FALSE;
			ChangeRAfter	= TRUE;
			break;

		default:
			ChangeRBefore	= FALSE;
			ChangeRAfter	= FALSE;
			break;
	}

	if( a )
		a->DataType = DtArray;
	if( ChangeB )
		b->DataType = DtArray;
	if( ChangeRBefore )
		r->DataType = DtArray;

	Status = (*DtArray->Func)( Msg, r, a, b );

	if( a )
		a->DataType = DtVtApply;
	if( ChangeB )
		b->DataType = DtVtApply;
	if( ChangeRAfter && Status && r->DataType == DtArray )
		r->DataType = DtVtApply;

	if( !Status )
		ErrMore( "VtApply" );
	return( Status );
}





/****************************************************************
**	Routine: NodeInitialize
**	Returns: TRUE or FALSE
**	Action : Does all the start-time initialization for Node API
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "NodeInitialize"

int NodeInitialize( void )
{
#if 0
/*
**	Various assertions
*/

#define SDB_NODE_SIZE 32
	if( sizeof( SDB_NODE ) != SDB_NODE_SIZE )
		return Err( ERR_NOT_CONSISTENT, "NodeInitialize, @\n"
					"sizeof( SDB_NODE ) = %d, expected %d",
					sizeof( SDB_NODE ), SDB_NODE_SIZE );
#endif

	if( sizeof( SDB_NODE * ) != sizeof( unsigned long ))
		return Err( ERR_UNSUPPORTED_OPERATION, "SecDb will only work is sizeof( SDB_NODE * ) == sizeof( unsigned long ), sorry!" );


	if( !SDB_NODE::Initialize() )
		return FALSE;

/*
**	Placeholder datatypes
*/

	DtValueType			= DtRegister( "Value Type",				"", "", DtFuncValueType, 		59, NULL, NULL );
	DtPassThrough		= DtRegister( "SecDb Pass Through",		"", "", DtFuncPassThrough, 		62, NULL, NULL  );
	DtValueArray		= DtRegister( "Value Array",			"", "", DtFuncValueArray,		69, NULL, NULL  );
	DtEach				= DtRegister( "Each",					"", "", DtFuncEach,				70, NULL, NULL  );
	DtNodeFlags			= DtRegister( "SecDb Node Flags",		"", "", DtFuncNodeFlags,		71, NULL, NULL  );
	DtValueTypeInfo		= DtRegister( "Value Type Info",		"",	"",	DtFuncValueTypeInfo,	82, NULL, NULL  );
	DtEllipsis			= DtRegister( "Ellipsis",				"", "", DtFuncEllipsis,			83, NULL, NULL  );
	DtVtApply			= DtRegister( "VtApply",				"", "", DtFuncVtApply,			90, NULL, NULL	);
	DtGsNodeValues      = DtRegister( "GsNodeValues",           "", "", DtFuncGsNodeValues,		107, NULL, NULL	);

	return DtValueType && 
	  DtEach && DtNodeFlags && DtValueTypeInfo && DtEllipsis && DtVtApply && DtGsNodeValues;
}
