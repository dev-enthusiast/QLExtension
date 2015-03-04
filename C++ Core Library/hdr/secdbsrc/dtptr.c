#define GSCVSID "$Header: /home/cvs/src/secdb/src/dtptr.c,v 1.41 2013/10/07 19:33:17 khodod Exp $"
/****************************************************************
**
**	DTPTR.C		Pointer datatype
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: dtptr.c,v $
**	Revision 1.41  2013/10/07 19:33:17  khodod
**	Address obvious cases of format-string vulnerabilities.
**	#315536
**
**	Revision 1.40  2001/11/27 23:23:32  procmon
**	Reverted to CPS_SPLIT.
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<heap.h>
#include	<heapinfo.h>
#include	<hash.h>
#include	<datatype.h>
#include	<dterr.h>
#include	<secdb.h>
#include	<secindex.h>
#include	<sdb_int.h>
#include	<secexpr.h>
#include	<secerror.h>
#include	<dtptr.h>


#define MAX_DEREFERENCE_DEPTH		64
#define	ERR_REFERENCE_CYCLE			25321

/*
**	Internal prototype
*/

static int CollectGarbage(		SLANG_VARIABLE *Variable, HASH *RefHash );


/****************************************************************
**	Routine: DtPointerAssign
**	Returns: TRUE or FALSE
**	Action : *Ptr = Value
****************************************************************/

int DtPointerAssign(
	DT_VALUE	*PtrValue,	// Lvalue to be assigned to
	DT_VALUE	*Value		// Value to be assigned to *Ptr
)
{
	SLANG_VARIABLE
			*Variable;

	DT_POINTER
			*Ptr;

	int		Status;


	if( PtrValue->DataType != DtPointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@, Assignment to non-pointer (%s)", PtrValue->DataType->Name ));
	Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );

	if( PtrNull == Ptr->Type )
		return( Err( ERR_UNSUPPORTED_OPERATION, "Assignment of a null pointer" ));

	Variable = Ptr->Variable;
	switch( Ptr->Type )
	{
		case PtrVariable:
			if( Variable->Flags & SLANG_VARIABLE_PROTECT )
				return( Err( SDB_ERR_OBJECT_IN_USE, "'%s' variable is in use", Variable->Name ));
			DTM_REPLACE( Variable->Value, Value, Status )
			return( Status );

		case PtrComponent:
			return DtComponentReplace( Variable->Value, &Ptr->Element, Value );

		case PtrSubscript:
			return DtSubscriptReplace( Variable->Value, &Ptr->Element, Value );

		case PtrNull:
			break;
	}

	return( FALSE );
}



/****************************************************************
**	Routine: DtPointerDestroy
**	Returns: TRUE or FALSE
**	Action : Destroy the referenced object:
**				PtrVariable		call SlangVariableDestroy
**				PtrComponent	send DT_MSG_COMPONENT_DESTROY
**				PtrComponent	send DT_MSG_SUBSCRIPT_DESTROY
****************************************************************/

int DtPointerDestroy(
	DT_VALUE	*PtrValue	// Lvalue to be destroyed
)
{
	SLANG_VARIABLE
			*Variable;

	DT_POINTER
			*Ptr;


	if( PtrValue->DataType != DtPointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@, Attempt to destroy non-pointer (%s)", PtrValue->DataType->Name ));
	Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );

	if( PtrNull == Ptr->Type )
		return( TRUE );

	Variable = Ptr->Variable;
	if( Variable->Flags & SLANG_VARIABLE_PROTECT )
		return( Err( SDB_ERR_OBJECT_IN_USE, "'%s' variable is in use", Variable->Name ));

	switch( Ptr->Type )
	{
		case PtrVariable:
			return SlangVariableDestroy( Variable );

		case PtrComponent:
			return( DT_OP( DT_MSG_COMPONENT_DESTROY, NULL, Variable->Value, &Ptr->Element ));

		case PtrSubscript:
			return( DT_OP( DT_MSG_SUBSCRIPT_DESTROY, NULL, Variable->Value, &Ptr->Element ));

		case PtrNull:
			break;
	}
	return( FALSE );
}



/****************************************************************
**	Routine: DtPointerName
**	Returns: Nothing
**	Action : Fills name of pointer into given buffer
****************************************************************/

static void DtPointerName(
	DT_POINTER	*Ptr,
	char		*Buffer,
	int			BufSize
)
{
	DT_VALUE
			StrVal;

	int		Sz;


	BufSize--;
	if( PtrNull == Ptr->Type )
		sprintf( Buffer, "%.*s", BufSize, "(null)" );
	else
	{
		if( BufSize > 2 && Ptr->Variable->HashTable && Ptr->Variable->HashTable->Title )
		{
			Sz = sprintf( Buffer, "%.*s::", BufSize - 2, Ptr->Variable->HashTable->Title );
			Buffer += Sz;
			BufSize -= Sz;
		}
		Sz = sprintf( Buffer, "%.*s", BufSize, Ptr->Variable->Name );
		Buffer += Sz;
		BufSize -= Sz;

		if( PtrVariable != Ptr->Type )
		{
			StrVal.DataType = DtString;
			if( DTM_TO( &StrVal, &Ptr->Element ) )
			{
				if( PtrSubscript == Ptr->Type && BufSize > 4 ) // if 1 < BufSize <= 4 use the . case below
					sprintf( Buffer, "[ %.*s ]", BufSize - 4, (char *) DT_VALUE_TO_POINTER( &StrVal ));
				else if( BufSize > 1 )
					sprintf( Buffer, ".%.*s", BufSize - 1, (char *) DT_VALUE_TO_POINTER( &StrVal ));
				DTM_FREE( &StrVal );
			}
		}
	}
}



/****************************************************************
**	Routine: DtPointerVariable
**	Returns: TRUE or FALSE
**	Action : Constructs Variable-type DtPointer
****************************************************************/

int DtPointerVariable(
	DT_VALUE			*PtrValue,		// Value to be initialized with pointer data
	SLANG_VARIABLE		*Variable		// Variable pointed to
)
{
	DT_POINTER
			*Ptr;


	if( !DTM_ALLOC( PtrValue, DtPointer ))
		return( FALSE );

	Ptr				= (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );
	Ptr->Type		= PtrVariable;
	Ptr->Variable	= SlangVariableIncrRef( Variable );
	return( TRUE );
}



/****************************************************************
**	Routine: DtPointerElement
**	Returns: TRUE or FALSE
**	Action : Constructs Component-type or Subscript-type DtPointer
****************************************************************/

static int DtPointerElement(
	DT_VALUE		*PtrValue,		// Value to be initialized with pointer data
	DT_VALUE		*ParentPtr,		// Variable-type DtPointer to reference
	DT_VALUE		*Element,		// Component/subscript value
	DT_POINTER_TYPE	Type
)
{
	DT_POINTER
			*Ptr;


	if( !DTM_ASSIGN( PtrValue, ParentPtr ))
		return( FALSE );

	Ptr	= (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );


/*
**	If the pointed to value is also a subscript/component then a temporary
**	variable (not in any variable scope) must be created.  E.g., a.foo.bar
**	must create a temporary variable called "a.foo" which points to the
**	parent DtPointer.  Since the variable is not referenced by any variable
**	hash, it gets freed when the returned DtPointer is freed.
*/

	if( Ptr->Type == PtrSubscript || Ptr->Type == PtrComponent )
	{
		SLANG_VARIABLE
				*TmpVariable;


		if( !( TmpVariable = SlangVariableAlloc( TRUE )))
			return( FALSE );

#if 0
		DtPointerName( Ptr, TmpVariable->Name, sizeof( TmpVariable->Name ));
#else
		TmpVariable->Name = "...";
#endif
		*TmpVariable->Value	= *PtrValue;
		TmpVariable->Flags |= SLANG_VARIABLE_TEMPORARY;

		if( !DtPointerVariable( PtrValue, TmpVariable ))
		{
			SlangVariableFree( TmpVariable );
			return( FALSE );
		}
		Ptr	= (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );
	}


/*
**	Assign the element
*/

	if( !DTM_ASSIGN( &Ptr->Element, Element ))
	{
		DTM_FREE( PtrValue );
		return( FALSE );
	};
	Ptr->Type = Type;
	return( TRUE );
}



/****************************************************************
**	Routine: DtPointerSubscript
**	Returns: TRUE or FALSE
**	Action : Constructs Subscript-type DtPointer
****************************************************************/

int DtPointerSubscript(
	DT_VALUE	*PtrValue,		// Value to be initialized with pointer data
	DT_VALUE	*ParentPtr,		// Variable-type DtPointer to subscript
	DT_VALUE	*Subscript		// Subscript value
)
{
	if( ParentPtr->DataType != DtPointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@, Attempt to subscript non-pointer (%s)", ParentPtr->DataType->Name ));

	return( DtPointerElement( PtrValue, ParentPtr, Subscript, PtrSubscript ));
}



/****************************************************************
**	Routine: DtPointerComponent
**	Returns: TRUE or FALSE
**	Action : Constructs Component-type DtPointer
****************************************************************/

int DtPointerComponent(
	DT_VALUE	*PtrValue,		// Value to be initialized with pointer data
	DT_VALUE	*ParentPtr,		// Variable-type DtPointer to reference
	DT_VALUE	*Component		// Component value
)
{
	if( ParentPtr->DataType != DtPointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@, Attempt to reference component of non-pointer (%s)", ParentPtr->DataType->Name ));

	return( DtPointerElement( PtrValue, ParentPtr, Component, PtrComponent ));
}



/****************************************************************
**	Routine: DtPointerChangeElement
**	Returns: TRUE or FALSE
**	Action : Modifies a DtPointer
****************************************************************/

int DtPointerChangeElement(
	DT_VALUE			*PtrValue,		// Value to be initialized with pointer data
	DT_VALUE			*Element		// Element of variable pointed to
)
{
	DT_POINTER
			*Ptr;


	if( PtrValue->DataType != DtPointer )
		return( Err( ERR_INVALID_ARGUMENTS, "@: DtPointerChangeElement: First arg must be a DtPointer" ));

	Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );
	DTM_FREE( &Ptr->Element );
	return( DTM_ASSIGN( &Ptr->Element, Element ));
}



/****************************************************************
**	Routine: DtPointerProtect
**	Returns: TRUE or FALSE
**	Action : Sets protection bit on pointed-to variable
****************************************************************/

int DtPointerProtect(
	DT_VALUE	*PtrValue		// Value pointing to variable to protect
)
{
	DT_POINTER
			*Ptr;


	if( PtrValue->DataType != DtPointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@, Attempt to protect non-pointer (%s)", PtrValue->DataType->Name ));
	Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );

	if( PtrVariable != Ptr->Type )
		return( Err( ERR_UNSUPPORTED_OPERATION, "DtPointerProtect: only supported on pointers to variables" ));

 	Ptr->Variable->Flags |= SLANG_VARIABLE_PROTECT;
 	return( TRUE );
}



/****************************************************************
**	Routine: DtPointerUnprotect
**	Returns: TRUE or FALSE
**	Action : Clears protection bit on pointed-to variable
****************************************************************/

int DtPointerUnprotect(
	DT_VALUE	*PtrValue		// Value pointing to variable to protect
)
{
	DT_POINTER
			*Ptr;


	if( PtrValue->DataType != DtPointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@, Attempt to unprotect non-pointer (%s)", PtrValue->DataType->Name ));
	Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );

	if( PtrVariable != Ptr->Type )
		return( Err( ERR_UNSUPPORTED_OPERATION, "DtPointerUnprotect: only supported on pointers to variables" ));

 	Ptr->Variable->Flags &= ~SLANG_VARIABLE_PROTECT;
 	return( TRUE );
}



/****************************************************************
**	Routine: DtPointerUnreference
**	Returns: TRUE or FALSE
**	Action : Clears reference bit on pointed-to variable
****************************************************************/

int DtPointerUnreference(
	DT_VALUE	*PtrValue		// Value pointing to variable to protect
)
{
	DT_POINTER
			*Ptr;


	if( PtrValue->DataType != DtPointer )
		return( Err( ERR_UNSUPPORTED_OPERATION, "@, Attempt to unreference non-pointer (%s)", PtrValue->DataType->Name ));
	Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( PtrValue );

	if( PtrVariable != Ptr->Type )
		return( Err( ERR_UNSUPPORTED_OPERATION, "DtPointerUnreference: only supported on pointers to variables" ));

 	Ptr->Variable->Flags &= ~SLANG_VARIABLE_REFERENCE;
 	return( TRUE );
}



/****************************************************************
**	Routine: DtFuncPointer
**	Returns: TRUE	- Operation worked
**			 FALSE	- Operation failed
**	Action : Handle operations for pointer data type
****************************************************************/

int DtFuncPointer(
	int	   		Msg,
	DT_VALUE	*r,
	DT_VALUE	*a,
	DT_VALUE	*b
)
{
	static int
			DerefLevel = 0;

	DT_INFO	*DataTypeInfo;

	int		TypeMsg,
			Equal,
			Status;

	DT_POINTER
			*PtrA,
			*PtrB,
			*PtrR;

	DT_VALUE
			TmpValue,
			ElementPointer;

	static HEAP
			*HeapPtr;


	switch( Msg )
	{
		case DT_MSG_START:
			ErrRegister( ERR_REFERENCE_CYCLE, "Reference cycle detected" );
			a->DataType->Size	= sizeof( a->Data.Pointer );
			a->DataType->Flags	= DT_FLAG_POINTER | DT_FLAG_COMPONENTS | DT_FLAG_SUBSCRIPTS;
			HeapPtr = GSHeapCreate( "Pointer Heap", sizeof( DT_POINTER ),  500 );
			return( TRUE );

		case DT_MSG_ALLOCATE:
			if( !( PtrA = (DT_POINTER *) GSHeapCalloc( HeapPtr )))
				return( Err( ERR_MEMORY, "@" ));
			DTM_INIT( &PtrA->Element );
			r->Data.Pointer = PtrA;

			return( TRUE );

		case DT_MSG_FREE:
			PtrA = (DT_POINTER *) a->Data.Pointer;
			if( PtrA->Variable )
				SlangVariableFree( PtrA->Variable );
			DTM_FREE( &PtrA->Element );
			GSHeapFree( HeapPtr, PtrA );
			return( TRUE );

		case DT_MSG_ASSIGN:		// r = a
			PtrA = (DT_POINTER *) a->Data.Pointer;
			if( !DtPointerVariable( r, PtrA->Variable ))
				return( FALSE );
			if( PtrVariable == PtrA->Type )
				return( TRUE );
			PtrR = (DT_POINTER *) r->Data.Pointer;
			if( !DTM_ASSIGN( &PtrR->Element, &PtrA->Element ))
			{
				DTM_FREE( r );
				return( FALSE );
			}
			PtrR->Type = PtrA->Type;
			return( TRUE );

		case DT_MSG_COMPONENT:
		case DT_MSG_SUBSCRIPT:
			if( DT_OP( DT_MSG_DEREFERENCE, &ElementPointer, a, NULL ))
				return( DT_OP( Msg, r, (DT_VALUE *) ElementPointer.Data.Pointer, b ));
			else
			{
				// indicate an error by setting result to null
				memset(r, 0, sizeof(DT_VALUE));
				return( FALSE );
			}

		case DT_MSG_SIZE:
		case DT_MSG_COMPONENT_VALUE:
		case DT_MSG_SUBSCRIPT_VALUE:
		case DT_MSG_ASSIGN_ADD:
		case DT_MSG_ASSIGN_SUBTRACT:
		case DT_MSG_ASSIGN_MULTIPLY:
		case DT_MSG_ASSIGN_DIVIDE:
		case DT_MSG_ASSIGN_AND:
		case DT_MSG_COMPONENT_REPLACE:
		case DT_MSG_SUBSCRIPT_REPLACE:
		case DT_MSG_COMPONENT_DESTROY:
		case DT_MSG_SUBSCRIPT_DESTROY:

			if( DerefLevel >= MAX_DEREFERENCE_DEPTH )
				return( Err( ERR_OVERFLOW, "Maximum dereference depth (%d) exceeded--possible cycle", MAX_DEREFERENCE_DEPTH ));
			DerefLevel++;

			// First attempt to operate on pointer to variable/element
			if( DT_OP( DT_MSG_DEREFERENCE, &ElementPointer, a, NULL ))
			{
				Status = DT_OP( Msg, r, (DT_VALUE *) ElementPointer.Data.Pointer, b );
				DerefLevel--;
				return( Status );
			}

			// Otherwise do the operation to a copy and reassign the result to
			// variable/component.
			//
			// If DEREFERENCE_VALUE returns a REFERENCE datatype, assume there
			// is no need to try to assign it back to DtPointer 'a'.  This is
			// necessary to support the Ole operations such as:
			//     Application.ToolBars[[ 1 ]].Visible = 1;
			// Without the REFERENCE check, the above code must be written as:
			//     K = Application.ToolBars[[ 1 ]]; K.Visible = 1;
			if( DT_OP( DT_MSG_DEREFERENCE_VALUE, &TmpValue, a, NULL ))
			{
				Status = DT_OP( Msg, r, &TmpValue, b );
				if(    Status
					&& DT_MSG_SUBSCRIPT_VALUE != Msg
					&& DT_MSG_COMPONENT_VALUE != Msg
					&& !(TmpValue.DataType->Flags & DT_FLAG_REFERENCE)
				)
					Status = DtPointerAssign( a, &TmpValue );
				else
					DTM_FREE( &TmpValue );
			}
			else
				Status = FALSE;

			// Create component/subscript for MSG_ASSIGN_* messages if it does not already exist
			if( !Status && DT_MSG_ASSIGN & Msg )
			{
				PtrA = (DT_POINTER *) a->Data.Pointer;
				DTM_INIT( &TmpValue );
				switch( PtrA->Type )
				{
					case PtrComponent:
						if( ErrNum == DT_ERR_COMPONENT_NOT_EXIST || ErrNum == DT_ERR_SUBSCRIPT_OUT_OF_RANGE )
						{
							// Initialize to 0.0, since we disallowed ops on Null + Double
							if( DtDouble == b->DataType )
								DT_NUMBER_TO_VALUE( &TmpValue, 0.0, DtDouble );
							Status = DtComponentReplace( PtrA->Variable->Value, &PtrA->Element, &TmpValue );
							if( Status )
								Status = DT_OP( Msg, r, a, b );
						}
						break;

					case PtrSubscript:
						if( ErrNum == DT_ERR_COMPONENT_NOT_EXIST || ErrNum == DT_ERR_SUBSCRIPT_OUT_OF_RANGE )
						{
							// Initialize to 0.0, since we disallowed ops on Null + Double
							if( DtDouble == b->DataType )
								DT_NUMBER_TO_VALUE( &TmpValue, 0.0, DtDouble );
							Status = DtSubscriptReplace( PtrA->Variable->Value, &PtrA->Element, &TmpValue );
							if( Status )
								Status = DT_OP( Msg, r, a, b );
						}
						break;

					case PtrVariable:
					case PtrNull:
						break;
				}
			}

			DerefLevel--;
			return( Status );

		case DT_MSG_DEREFERENCE_VALUE:
		case DT_MSG_DEREFERENCE:
			PtrA = (DT_POINTER *) a->Data.Pointer;
			if( PtrNull == PtrA->Type )
				return( Err( ERR_UNSUPPORTED_OPERATION, "Cannot dereference a null pointer" ));
			if( PtrVariable == PtrA->Type )
			{
				if( DT_MSG_DEREFERENCE == Msg )
				{
					r->Data.Pointer = PtrA->Variable->Value;
					return( TRUE );
				}
				else
					return( DTM_ASSIGN( r, PtrA->Variable->Value ));
			}

			if( PtrSubscript == PtrA->Type )
				if( DT_MSG_DEREFERENCE == Msg )
					TypeMsg = DT_MSG_SUBSCRIPT;
				else
					TypeMsg = DT_MSG_SUBSCRIPT_VALUE;
			else
				if( DT_MSG_DEREFERENCE == Msg )
					TypeMsg = DT_MSG_COMPONENT;
				else
					TypeMsg = DT_MSG_COMPONENT_VALUE;

			return( DT_OP( TypeMsg, r, PtrA->Variable->Value, &PtrA->Element ));

		case DT_MSG_EQUAL:
		case DT_MSG_NOT_EQUAL:
			PtrA	= (DT_POINTER *) a->Data.Pointer;
			if( b->DataType != a->DataType )
				return( FALSE );
			PtrB	= (DT_POINTER *) b->Data.Pointer;

			Equal = PtrA->Type == PtrB->Type;

			if( Equal && PtrNull != PtrA->Type )
			{
				Equal = PtrA->Variable == PtrB->Variable;
				if( Equal )
				{
					if( !DT_OP( DT_MSG_EQUAL, r, &PtrA->Element, &PtrB->Element ))
						return( FALSE );
					Equal = (int) r->Data.Number;
				}
				if( Equal )
				{
					if( !DT_OP( DT_MSG_EQUAL, r, PtrA->Variable->Value, PtrB->Variable->Value ))
						return( FALSE );
					Equal = (int) r->Data.Number;
				}
			}

			r->DataType = DtDouble;
			r->Data.Number = (double) ( Equal == ( DT_MSG_EQUAL == Msg ));
			return( TRUE );

		case DT_MSG_TRUTH:
			return TRUE;

		case DT_MSG_TO:
			PtrA = (DT_POINTER *) a->Data.Pointer;
			if( r->DataType == DtString )
			{
				char	*pBuf,
						Buffer[ 128 ];


				pBuf = Buffer;
				*pBuf++ = '&';
				DtPointerName( PtrA, pBuf, sizeof( Buffer ) - 1 );
				DT_POINTER_TO_VALUE( r, strdup( Buffer ), DtString )
			}
			else
				return Err( ERR_UNSUPPORTED_OPERATION, "@" );
			return TRUE;

		case DT_MSG_FROM:
			return Err( ERR_UNSUPPORTED_OPERATION, "@" );

		case DT_MSG_MEMSIZE:
			PtrA = (DT_POINTER *) a->Data.Pointer;
			if( PtrComponent == PtrA->Type || PtrSubscript == PtrA->Type )
			{
				if( !DT_OP( Msg, r, &PtrA->Element, NULL ))
					return( FALSE );
			}
			else
			{
				r->DataType = DtDouble;
				r->Data.Number  = 0;
			}
			r->Data.Number += GSHeapMemSize( PtrA );
			return TRUE;

		case DT_MSG_INFO:
			DataTypeInfo = (DT_INFO *) calloc( 1, sizeof( DT_INFO ));
			DataTypeInfo->Flags	= DT_FLAG_POINTER | DT_FLAG_COMPONENTS | DT_FLAG_SUBSCRIPTS;
			r->Data.Pointer = DataTypeInfo;
			return TRUE;

		case DT_MSG_INCREMENT:
		case DT_MSG_DECREMENT:
		case DT_MSG_ADD:
		case DT_MSG_SUBTRACT:
			// FIX -- behave like c on subscript types

		case DT_MSG_TO_STREAM:
		case DT_MSG_FROM_STREAM:
		default:
			return Err( ERR_UNSUPPORTED_OPERATION, "Pointer, @" );
	}
}



/****************************************************************
**	Routine: CollectComponentGarbage
**	Returns: Nothing
**	Action : Recursively looks for all variable references
**			 internally and records each (deletable) reference in
**			 the given hash.
****************************************************************/

static int CollectComponentGarbage(
	DT_VALUE	*ParentValue,
	HASH		*RefHash
)
{
	DT_VALUE
			*Value,
			ValuePtr,
			Enum;

	DT_POINTER
			*Ptr;


	if( -1 == (long) HashLookup( RefHash, ParentValue ))
//		return Err( ERR_REFERENCE_CYCLE, "@" );
		return TRUE;		// FIX: Will let cycles live unmolested for now.
	HashInsert( RefHash, (HASH_KEY) ParentValue, (HASH_VALUE)(long) -1 );

  	DTM_FOR( &Enum, ParentValue )
  	{
  		if( !DT_OP( DT_MSG_SUBSCRIPT, &ValuePtr, ParentValue, &Enum ))
		{
			DTM_FOR_BREAK( &Enum, ParentValue )
			break;
		}

  		Value = (DT_VALUE *) ValuePtr.Data.Pointer;

		if( DtPointer == Value->DataType )
		{
			Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( Value );
			if( !CollectGarbage( Ptr->Variable, RefHash ))
				goto FAIL;
		}
		else if( Value->DataType->Flags & ( DT_FLAG_SUBSCRIPTS | DT_FLAG_COMPONENTS ))
			if( !CollectComponentGarbage( Value, RefHash ))
				goto FAIL;
	}
	HashDelete( RefHash, ParentValue );
	return TRUE;

FAIL:
	{
		DT_VALUE	StringVal;

		DTM_INIT( &StringVal );
		StringVal.DataType = DtString;
		if( DTM_TO( &StringVal, &Enum ))
		{
			ErrMore( "[ %s ]", (char*)StringVal.Data.Pointer );
			DTM_FREE( &StringVal );
		}
		DTM_FOR_BREAK( &Enum, ParentValue )
		HashDelete( RefHash, ParentValue );
		return FALSE;
	}
}



/****************************************************************
**	Routine: CollectGarbage
**	Returns: Nothing
**	Action : Recursively looks for all variable references
**			 internally and records each (deletable) reference in
**			 the given hash.
****************************************************************/

static int CollectGarbage(
	SLANG_VARIABLE	*Variable,
	HASH			*RefHash
)
{
	long	RefsFound;

	DT_VALUE
			*Value;

	DT_POINTER
			*Ptr;


/*
**	If the variable has already been visited, just note that another
**	internal reference has been found, else traverse its contents.
*/

	RefsFound = (long) HashLookup( RefHash, Variable );
	HashInsert( RefHash, Variable, (HASH_VALUE)( RefsFound + 1 ));
	if( RefsFound )
		return TRUE;


/*
**	If RETAIN then the value can not be freed and therefore it
**	is not valid to traverse its contents.
*/

	if( Variable->Flags & SLANG_VARIABLE_RETAIN )
		return TRUE;


	if( !( Value = Variable->Value ))
		return TRUE;

	if( DtPointer == Value->DataType )
	{
		Ptr = (DT_POINTER *) DT_VALUE_TO_POINTER( Value );
		if( !CollectGarbage( Ptr->Variable, RefHash ))
		{
			ErrMore( "%s", Ptr->Variable->Name );
			ErrMore( "%s", Variable->Name );
			return FALSE;
		}
	}
	else if( Value->DataType->Flags & ( DT_FLAG_SUBSCRIPTS | DT_FLAG_COMPONENTS ))
		if( !CollectComponentGarbage( Value, RefHash ))
			return ErrMore( "%s", Variable->Name );

	return TRUE;
}



/****************************************************************
**	Routine: DtPointerCollectGarbage
**	Returns: Nothing
**	Action : Cleans up the variable if all its references are
**			 internal, i.e., following the variable and all its
**			 components/subscripts finds all the references to
**			 the given variable.
****************************************************************/

void DtPointerCollectGarbage(
	SLANG_VARIABLE	*Variable
)
{
	DT_VALUE
			*Value;

	HASH	*RefHash;

	int		Garbage;

	SLANG_VARIABLE
			*EachVar;

	HASH_ENTRY_PTR
			HashEnum;


/*
**	Some quick (some redundant) checks here to see if it is worth the trouble
*/

	if( !( Value = Variable->Value ))
		return;
	if( DtPointer != Value->DataType && !( Value->DataType->Flags & ( DT_FLAG_SUBSCRIPTS | DT_FLAG_COMPONENTS )))
		return;
	if( Variable->Flags & SLANG_VARIABLE_RETAIN )
		return;


/*
**	Create the hash of visited variables
*/

	RefHash = HashCreate( "RefHash", NULL, NULL, 0, NULL );


/*
**	See if we have garbage.
*/

	if( !CollectGarbage( Variable, RefHash ))
		SecDbErrorMessage( ErrGetString() );

	Variable->ReferenceCount++;		// Since it was already decremented before DtPointerCollectGarbage was called.
	Garbage = TRUE;
	HASH_FOR( HashEnum, RefHash )
	{
		EachVar = (SLANG_VARIABLE *) HashKey( HashEnum );
		if( (HASH_VALUE)(long) EachVar->ReferenceCount != HashValue( HashEnum ))
		{
			Garbage = FALSE;
			break;
		}
	}
	Variable->ReferenceCount--;


/*
**	If we have garbage, set the RETAIN bit since the last reference
**	will go away during the DTM_FREE of its value so we need to prevent
**	SlangVariableFree from trying to do the DTM_FREE which is already in
**	progress.  Similarly, set the HashTable to note taht the garbage
**	collection is in progress.
*/

	if( Garbage )
	{
		Variable->Flags		|= SLANG_VARIABLE_RETAIN;
		Variable->HashTable  = RefHash;

		DTM_FREE( Value );
		GSHeapFree( DtValueHeap, Value );
	}


/*
**	Cleanup
*/

	HashDestroy( RefHash );
}

