#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_did.c,v 1.98 2001/09/27 14:40:39 singhki Exp $"
/****************************************************************
**
**	SDB_DID.C	- SecDb diddle functions
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_did.c,v $
**	Revision 1.98  2001/09/27 14:40:39  singhki
**	remove unused var
**
**	Revision 1.97  2001/08/14 17:12:15  singhki
**	if SDB_SET_NO_MESSAGE then free OrigValue in PlaceDiddle
**	
**	Revision 1.96  2001/07/18 19:59:12  simpsk
**	auto_ptr_array moved to extension.
**	
**	Revision 1.95  2001/07/16 08:56:27  goodfj
**	Warnfix: Remove unused variable
**	
**	Revision 1.94  2001/06/21 19:34:23  singhki
**	Completely get rid of remove diddle message. Rewrite side effect
**	diddle maps to actually work. Use it to remove all side effect
**	diddles. fix side effect/phantom interaction with DiddleStackMove.
**	
**	Also remove all diddles during destroy to prevent leaks.
**	
**	Revision 1.93  2001/04/04 20:07:35  singhki
**	fixes for handling of PrevDiddleID in cases where the next diddle is
**	the fake set value on the diddle stack.
**	
**	Revision 1.92  2001/04/04 16:40:05  singhki
**	do not use 0 for PrevDiddleId since that is a valid diddle id. Also return SDB_DIDDLE_ID instead of int
**	
**	Revision 1.91  2001/03/28 17:37:23  singhki
**	DiddleStackMoveTo must take off a NotMyDiddle in the case that a node
**	in a base diddle scopes was diddled before trying to move the diddle
**	stack.
**	
**	Revision 1.90  2001/03/20 17:18:31  singhki
**	add tracing for notmydiddle and diddlestackmove
**	
**	Revision 1.89  2001/03/14 18:33:22  singhki
**	Rewrite nodes to use STL style multiset instead of skip list
**	
**	Revision 1.88  2001/03/06 20:17:26  singhki
**	side effect diddles may be phantom in their own right
**	
**	Revision 1.87  2001/02/28 01:49:05  singhki
**	In case we consider a diddled node as the best node ensure that we
**	have also considered the node in the PhysicalDb else we will get
**	incorrect sharing.
**	
**	Old way of creating the node during the set diddle did not work in all
**	cases because that node could get co-opted for another Db/destroyed by
**	LRU cache.
**	
**	Revision 1.86  2000/11/27 13:59:27  singhki
**	Free OldValue if diddle succeeds. Also use TargetValue to get old value (works for vector contexts too)
**	
**	Revision 1.85  2000/11/24 18:04:54  shahia
**	Bug Fix: Failed Change diddle resulted in Eval not being able to pop the diddle
**	
**	Revision 1.84  2000/10/11 20:15:58  singhki
**	don't copy valid bit if it is a fake one set by GetValue to indicate
**	that it is checking for cycle detection. FIX- GetValue shouldn't
**	overload this bit since it has a GetValueLock
**	
**	Revision 1.83  2000/08/25 23:09:08  singhki
**	BaseDb may be NULL
**	
**	Revision 1.82  2000/07/31 17:00:02  goodfj
**	Allow diddles to be rejected by valuefunction
**	
**	Revision 1.81  2000/05/17 03:46:02  singhki
**	Prevent DiddleStackMove from invalidating a node if it is a phantom diddle and engaged in a getvalue
**	
**	Revision 1.80  2000/04/28 11:14:19  singhki
**	remove unused var
**	
**	Revision 1.79  2000/04/27 11:57:07  vengrd
**	Make ChangeDiddle a member of SDB_NODE. Get rid of SdbNodeStructure
**	
**	Revision 1.78  2000/04/24 11:04:21  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.77  2000/04/12 19:32:38  singhki
**	Add SideEffectDiddleIterator to get side effect diddles of a phantom diddle
**	
**	Revision 1.76  2000/04/12 19:22:18  singhki
**	export DiddleChange so SecDbSetValue can use it
**	
**	Revision 1.75  2000/02/08 04:50:24  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
****************************************************************/

#define INCL_ALLOCA
#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<heapinfo.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<sdb_int.h>
#include	<secerror.h>
#include	<sdbdbset.h>
#include	<vector>
#include	<secdbcc.h>
#include	<extension/auto_ptr_array.h>
#include	<memory>

CC_USING( std::auto_ptr );
CC_USING( extension::auto_ptr_array );


/*
**	Macros
*/

#define	TRACE_FUNC( Node, Msg )													\
	(void) ( !SecDbTraceFunc || (SecDbTraceFunc)( SDB_TRACE_##Msg##_DIDDLE, Node ))

inline void Trace( SDB_NODE* Node, SDB_TRACE_MSG Msg, void* arg )
{
	if( SecDbTraceFunc )
		SecDbTraceFunc( Msg, Node, arg );
}

/*
**	Variables
*/

static int
		NodeDiddleIDs = 1;

/****************************************************************
**	Routine: SDB_NODE::DiddleHeaderGet
**	Returns: SDB_DIDDLE_HEADER * / NULL
**	Action : gets the diddle header for a node if diddled
****************************************************************/

SDB_DIDDLE_HEADER *SDB_NODE::DiddleHeaderGet(
)
{
	return DiddleGet() ? (SDB_DIDDLE_HEADER *) HashLookup( s_DiddleHash, this ) : NULL;
}



/****************************************************************
**	Routine: SDB_NODE::DiddleFind
**	Returns: SDB_DIDDLE *
**	Action : Diddle structure for a given diddle
****************************************************************/

SDB_DIDDLE *SDB_NODE::DiddleFind(
	SDB_DIDDLE_ID	DiddleID
)
{
	SDB_DIDDLE_HEADER
			*DiddleHeader = DiddleHeaderGet();

	if( !DiddleHeader )
		return NULL;

	SDB_DIDDLE
			*Diddle = DiddleHeader->DiddleList;

	for( ; Diddle; Diddle = Diddle->Next )
		if( Diddle->DiddleID == DiddleID )
			return Diddle;

	return NULL;
}



/****************************************************************
**	Routine: SDB_NODE::VectorIndexPush
**	Returns: TRUE or FALSE
**	Action : Allocates and pushes a new NodeVectorContext
****************************************************************/

bool SDB_NODE::VectorIndexPush()
{
	SDB_VECTOR_CONTEXT
			*New;


	if( !ERR_CALLOC( New, SDB_VECTOR_CONTEXT, 1, sizeof( *New )))
		return( FALSE );

	New->InUse	= FALSE;
	New->Next	= s_VectorContext;
	s_VectorContext = New;
	return( TRUE );
}



/****************************************************************
**	Routine: SDB_NODE::VectorIndexPop
**	Returns: Nothing
**	Action : Pops top NodeVectorContext
****************************************************************/

void SDB_NODE::VectorIndexPop()
{
	SDB_VECTOR_CONTEXT
			*Old;

	if( s_VectorContext )
	{
		Old = s_VectorContext;
		s_VectorContext = Old->Next;
		free( Old );
	}
}



/****************************************************************
**	Routine: SendDiddleMsg
**	Returns: TRUE or FALSE
**	Action : Sends message to value function (if ValueArray to
**			 each element recursively)
****************************************************************/

static bool SendDiddleMsg(
	SDB_OBJECT			*SecPtr,
	SDB_VALUE_TYPE_INFO	*Vti,
	int					DiddleMsg,
	SDB_M_DIDDLE		*DiddleMsgData,
	int					Phantomness		// Phantom flag must match this (unless -1 in which case check 2nd-nth against 1st)
)
{
	DT_VALUE
			*VecVal;

	int		Size;

	SDB_M_DATA
			MsgData;

	DT_DATA_TYPE
			DiddleDataType = DiddleMsgData->Value->DataType;


	if( DiddleDataType == DtValueArray )
	{
		VecVal = DiddleMsgData->Value;

		if( SDB_NODE::VectorContext()->InUse )
			return Err( ERR_UNSUPPORTED_OPERATION, "SendDiddleMsg, VectorContext is InUse" ), false;
		SDB_NODE::VectorContext()->InUse = TRUE;
		SDB_NODE::VectorContext()->Size  = DtSize( VecVal );

		// Send msg to each component
		Size 	= SDB_NODE::VectorContext()->Size;
		for( SDB_NODE::VectorContext()->Index = 0; SDB_NODE::VectorContext()->Index < Size; SDB_NODE::VectorContext()->Index++ )
		{
			DiddleMsgData->Value = DtSubscriptGet( VecVal, SDB_NODE::VectorContext()->Index, NULL );
			if( !DiddleMsgData->Value )
				return false;

			if( !SendDiddleMsg( SecPtr, Vti, DiddleMsg, DiddleMsgData, Phantomness ))
				return false;

			if( -1 == Phantomness )
				Phantomness = DiddleMsgData->Phantom;
			else if( (unsigned) Phantomness != DiddleMsgData->Phantom )
				return Err( ERR_NOT_CONSISTENT, "All elements of a vector diddle must be either all Phantom or all !Phantom" ), false;
		}

		SDB_NODE::VectorContext()->InUse = FALSE;
	}
	else if( Vti )
	{
		int		DiddleSuccess;


		// Only allow the proper datatype unless the datatype is DtAny
		if( *Vti->DataType == DtAny )
		{
			// Allow anything but DtAny
			if(	DiddleDataType == DtAny )
				return Err( ERR_INVALID_ARGUMENTS, "'%s' expects anything but Any",
							Vti->Name ), false;
		}
		else if( *Vti->DataType != DiddleDataType )
			return Err( ERR_INVALID_ARGUMENTS, "'%s' expects a '%s' not a '%s'",
						Vti->Name, (*Vti->DataType)->Name, DiddleDataType->Name ), false;

		MsgData.Diddle			= *DiddleMsgData;
		MsgData.Diddle.Phantom	= FALSE;

		// Send the message to the object (note it cannot refuse)
		DiddleSuccess = SDB_MESSAGE_VT_TRACE( SecPtr, DiddleMsg, &MsgData, Vti );

		if( !DiddleSuccess )
			return ErrMore( "Failed to set diddle on %s( %s )", Vti->Name, SecPtr->SecData.Name );

		DiddleMsgData->Phantom = MsgData.Diddle.Phantom;
	}
	return true;
}



/****************************************************************
**	Routine: SDB_NODE::PlaceDiddle
**	Returns: TRUE or FALSE
**	Action : Places the diddle value in the right place in the
**			 Diddle->Value and calls top level of SendDiddleMsg.
**			 Also pushes and pops a new vector context if
**			 diddling to a value array.
****************************************************************/

bool SDB_NODE::PlaceDiddle(
	SDB_DIDDLE		*Diddle,
	SDB_OBJECT		*SecPtr,
	DT_VALUE		*Value,			// Pre-allocated value, i.e., a NO_COPY value, but not NO_FREE
	int				DiddleMsg,
	SDB_M_DIDDLE	*DiddleMsgData
)
{
	DT_VALUE
			OrgValue,
			*TargetValue,
			TmpValue;

	bool	Status;


/*
**	Figure out where value should go (and put it there)
*/

	// target value to change, on the diddle stack
	TargetValue = &Diddle->Value;
	if( s_VectorContext )
	{
		if( TargetValue->DataType == DtNull )
		{
			// Value Array not allocated yet, so allocate it
			DTM_INIT( &TmpValue );
			TmpValue.DataType		= DtDouble;
			TmpValue.Data.Number	= s_VectorContext->Size;
			TargetValue->DataType	= DtValueArray;
			if( !DTM_TO( TargetValue, &TmpValue ))
				return false;

		}
		TargetValue = DtSubscriptGet( TargetValue, s_VectorContext->Index, NULL );
		if( !TargetValue )
			return false;

		Diddle->CacheFlags = 0;
	}

	// save the old value in the diddle stack, hanging from s_DiddleHash (or inside vector context)
	OrgValue = *TargetValue;

	// set the diddle stack's value to the new value
	*TargetValue = *Value;


/*
**	Send the message to the value function
*/

	if(	DiddleMsgData->Flags & SDB_SET_NO_MESSAGE )
	{
		DTM_FREE( &OrgValue );
		return true;
	}

	DiddleMsgData->Value 	= TargetValue;
	SetOrDiddleLockSet( true );
	{
		if( Value->DataType == DtValueArray )
		{
			Status = VectorIndexPush();
			Diddle->VectorContext = s_VectorContext;
			Status = Status && SendDiddleMsg( SecPtr, VtiGet(), DiddleMsg, DiddleMsgData, -1 );
			VectorIndexPop();
		}
		else
			Status = SendDiddleMsg( SecPtr, VtiGet(), DiddleMsg, DiddleMsgData, -1 );
	}
	SetOrDiddleLockSet( false );

	if( !Status )
	{
		// change diddle failed, restore the diddle stacks value to the original value
		if( SDB_MSG_DIDDLE_CHANGED == DiddleMsg )
			*TargetValue = OrgValue;
		else
		{
			// else set diddle failed so we have no target value
			DTM_INIT( TargetValue );
			DTM_FREE( &OrgValue );		// FIX this is probably unnecessary in the case of set diddle
		}
	}
	else
		DTM_FREE( &OrgValue );

	return Status;
}



/****************************************************************
**	Routine: SDB_NODE::DiddleToNode
**	Returns: Nothing
**	Action : Transfers diddle value to node
****************************************************************/

void SDB_NODE::DiddleToNode(
	SDB_DIDDLE	*Diddle
)
{
	if( Diddle->Phantom )
	{
		TRACE_FUNC( this, PHANTOM );
		CacheFlagsSet( 0 );
	}
	else
	{
		CacheFlagsSet( SDB_CACHE_DIDDLE | SDB_CACHE_NO_FREE );
		m_Value = Diddle->Value;
		ValidSet( true );
	}
}



/****************************************************************
**	Routine: SDB_NODE::DiddleSet
**	Returns: TRUE/Err()
**	Action :
****************************************************************/

SDB_DIDDLE_ID SDB_NODE::DiddleSet(
	SDB_OBJECT		*SecPtr,
	DT_VALUE		*Value,
	SDB_M_DIDDLE	*DiddleMsgData,
	SDB_DIDDLE_ID	&PrevDiddleID
)
{
	SDB_DIDDLE_HEADER
			*DiddleHeader = NULL;

	SDB_DIDDLE
			*Diddle	= NULL;

	int		CopiesNeeded;

	DT_VALUE
			*OrigValue,
			SizeValue,
			TempValue;


	DTM_INIT( &TempValue );


/*
**	Check write cycles
*/

	if( SetOrDiddleLockGet() )
	{
		Err( SDB_ERR_OBJECT_IN_USE, "Trying to diddle a value which is already being set or diddled" );
		goto ERROR_EXIT;
	}
	if( CacheFlagsGet() & SDB_CACHE_SET )
	{
		SDB_VALUE_TYPE_INFO
				*Vti = VtiGet();

		if( Vti && ( Vti->ValueFlags & SDB_IN_STREAM ))
			SecPtr->Modified = TRUE;
	}


/*
**	Need to store original value in case we are placing a value array
*/

	if( s_VectorContext && ValidGet() )
		OrigValue = &m_Value;
	else
		OrigValue = NULL;


/*
**	Find the appropriate Diddle structure in diddle list for this node
*/

	DiddleHeader = (SDB_DIDDLE_HEADER *) HashLookup( s_DiddleHash, this );
	if(( DiddleHeader == NULL ) != !DiddleGet() )
	{
		Err( ERR_NOT_CONSISTENT, "@: DiddleHeader" );
		goto ERROR_EXIT;
	}

	if( !DiddleHeader )
	{
		if( !ERR_CALLOC( DiddleHeader, SDB_DIDDLE_HEADER, 1, sizeof( *DiddleHeader )))
			goto ERROR_EXIT;

		DiddleHeader->Node = this;
		if( !HashInsert( s_DiddleHash, this, DiddleHeader ))
			goto ERROR_EXIT;
		DiddleSet( true );

		// Last Diddle structure in DiddleList is the original value of the node

		if( !ERR_CALLOC( DiddleHeader->DiddleList, SDB_DIDDLE, 1, sizeof( *Diddle )))
			goto ERROR_EXIT;

		// Will only be able to restore pre-diddled value if it had no children to invalidate it
		if( ValidGet() && ( CacheFlagsGet() & SDB_CACHE_SET ))
		{
			DiddleHeader->DiddleList->SetValue		= TRUE;
			DiddleHeader->DiddleList->Value			= m_Value;
			DiddleHeader->DiddleList->CacheFlags	= CacheFlagsGet();
			DTM_INIT( &m_Value );
			if( OrigValue )
				OrigValue = &DiddleHeader->DiddleList->Value;
		}
		else
		{
			DTM_INIT( &DiddleHeader->DiddleList->Value );
		}
		DiddleHeader->DiddleList->DiddleID = SDB_DIDDLE_ID_SET_VALUE;
	}


/*
**	CASE 1: Working on current diddle
*/

	if( s_VectorContext && s_VectorContext == DiddleHeader->DiddleList->VectorContext )
	{
		Diddle = DiddleHeader->DiddleList;

		// Take care of ref count on Value (since Value Array must own exactly one copy)
		CopiesNeeded = !( DiddleMsgData->Flags & SDB_CACHE_NO_COPY ) + !!( DiddleMsgData->Flags & SDB_CACHE_NO_FREE );
		if( CopiesNeeded )
		{
			if( CopiesNeeded > 1 )
			{
				Err( ERR_UNSUPPORTED_OPERATION, "SDB_CACHE_NO_COPY + SDB_CACHE_NO_FREE would cause a memory leak" );
				goto ERROR_EXIT;
			}
			if( !DTM_ASSIGN( &TempValue, Value ))
				goto ERROR_EXIT;
			Value = &TempValue;
		}

		if( !PlaceDiddle( Diddle, SecPtr, Value, SDB_MSG_DIDDLE_SET, DiddleMsgData ))
			goto ERROR_EXIT;
	}


/*
**	CASE 2: New diddle
*/

	else
	{
		// Putting on a NotMyDiddle, figure out whether if it is blocked or not
		if( DiddleMsgData->NotMyDiddle )
		{
			// If the source node wasn't diddled, then this node must not be diddled either
			if( PrevDiddleID == SDB_DIDDLE_ID_ERROR )
			{
				if( DiddleHeader->DiddleList->Next )
				{
				  NO_DIDDLE:
					DTM_FREE( &TempValue );
					return SDB_DIDDLE_ID_ERROR;
				}
			}
			// Else if the source node was diddled, then we only replace a NotMyDiddle
			// with that exact diddle id
			else if( !DiddleHeader->DiddleList->NotMyDiddle
					 || DiddleHeader->DiddleList->DiddleID != PrevDiddleID )
				goto NO_DIDDLE;	
			// OK to put on this NotMyDiddle
		}

		if( !ERR_CALLOC( Diddle, SDB_DIDDLE, 1, sizeof( *Diddle )))
			goto ERROR_EXIT;

		DiddleCount++;
		DTM_INIT( &Diddle->Value );
		Diddle->VectorContext = s_VectorContext;

		// This belongs in PlaceDiddle, but OrigValue is lost by then.
		if( OrigValue )
		{
			DTM_INIT( &SizeValue );
			SizeValue.DataType		= DtDouble;
			SizeValue.Data.Number	= s_VectorContext->Size;
			Diddle->Value.DataType	= DtValueArray;
			if( !DTM_TO( &Diddle->Value, &SizeValue ))
				goto ERROR_EXIT;
			if( !DtSubscriptSet( &Diddle->Value, s_VectorContext->Size-1, OrigValue ))
				goto ERROR_EXIT;
		}

		Diddle->Next = DiddleHeader->DiddleList;
		DiddleHeader->DiddleList = Diddle;

		Diddle->DiddleID = DiddleMsgData->DiddleID;

		// Take care of ref count on Value
		Diddle->CacheFlags = DiddleMsgData->Flags;
		if( !( DiddleMsgData->Flags & SDB_CACHE_NO_COPY ))
		{
			if( !DTM_ASSIGN( &TempValue, Value ))
				goto ERROR_EXIT;
			Value = &TempValue;
		}

		// if we are putting on a notmydiddle, and it is a phantom diddle, we may be currently doing a getvalue
		// in this case we need not do an invalidate. This is in order to allow us to do a SplitChildren() with a create
		// arg in buildchildren. If we did not do this, we would be forced to descend the graph twice.
		if( DiddleMsgData->NotMyDiddle && ( DiddleMsgData->Flags & SDB_SET_NO_MESSAGE ) && DiddleMsgData->Phantom
			&& GetValueLockGet() );
		else if( !Invalidate() )
			goto ERROR_EXIT;

		if( !PlaceDiddle( Diddle, SecPtr, Value, SDB_MSG_DIDDLE_SET, DiddleMsgData ))
			goto ERROR_EXIT;

		Diddle->Phantom	= DiddleMsgData->Phantom;
		Diddle->NotMyDiddle = DiddleMsgData->NotMyDiddle;

		if( ( PrevDiddleID = Diddle->Next->DiddleID ) == SDB_DIDDLE_ID_SET_VALUE )
			PrevDiddleID = SDB_DIDDLE_ID_ERROR;	// if next is the fake set value on the diddle stack, then there was no diddle

		// If previous diddle wasn't my diddle, then toss it out
		if( Diddle->Next->NotMyDiddle )
		{
			SDB_DIDDLE
					*OldDiddle = Diddle->Next;

			Diddle->Next = OldDiddle->Next;
			if( !( OldDiddle->CacheFlags & SDB_CACHE_NO_FREE ))
				DTM_FREE( &OldDiddle->Value );
			free( OldDiddle );
			--DiddleCount;
			SecDbFree( SecPtr );
		}
	}

	DiddleToNode( Diddle );

	// Hold onto the SecPtr so that it doesn't get thrown away
	SecDbIncrementReference( SecPtr );
	return Diddle->DiddleID;

ERROR_EXIT:
	DTM_FREE( &TempValue );
	if( Diddle )
	{
		if( !( Diddle->CacheFlags & SDB_CACHE_NO_FREE ))
			DTM_FREE( &Diddle->Value );
		if( DiddleHeader && DiddleHeader->DiddleList == Diddle )
		{
			DiddleHeader->DiddleList = Diddle->Next;
			if( !DiddleHeader->DiddleList->Next )
			{
				free( DiddleHeader->DiddleList );
				HashDelete( s_DiddleHash, this );
				free( DiddleHeader );
				DiddleSet( false );
			}
		}
		free( Diddle );
	}
	Error( "SetDiddle" );
	TRACE_FUNC( this, ERROR );
	return SDB_DIDDLE_ID_ERROR;
}



/****************************************************************
**	Routine: SDB_NODE::DiddleRemove
**	Returns: TRUE or FALSE
**	Action : Finds diddle and removes it
****************************************************************/

bool SDB_NODE::DiddleRemove(
	SDB_OBJECT		*SecPtr,
	SDB_M_DIDDLE	*DiddleMsgData
)
{
	SDB_DIDDLE
			*Diddle,
			**pDiddle,
			*NextDiddle;

	SDB_DIDDLE_HEADER
			*DiddleHeader;

	
	bool	side_effect_status = true;


	if( !DiddleGet() )
	{
		SDB_VALUE_TYPE_INFO
				*Vti = VtiGet();

		Err( ERR_INVALID_ARGUMENTS, "RemoveDiddle: %s( %s ) has no diddles", Vti ? Vti->Name : "Unknown Vt", SecPtr->SecData.Name );
		goto ERROR_EXIT;
	}
	if( !( DiddleHeader = (SDB_DIDDLE_HEADER *) HashLookup( s_DiddleHash, this )))
	{
		ErrMore( "RemoveDiddle: Software error" );
		goto ERROR_EXIT;
	}


/*
**	Find diddle
*/

	for( pDiddle = &DiddleHeader->DiddleList; (*pDiddle)->Next && (*pDiddle)->DiddleID != DiddleMsgData->DiddleID; pDiddle = &(*pDiddle)->Next );
	Diddle 		= *pDiddle;
	NextDiddle	= Diddle->Next;
	if( !NextDiddle )
	{
		Err( ERR_NOT_FOUND, "RemoveDiddle - diddle @" );
		goto ERROR_EXIT;
	}
	if( Diddle->NotMyDiddle && !DiddleMsgData->NotMyDiddle )
	{
		Err( ERR_UNSUPPORTED_OPERATION, "RemoveDiddle - cannot remove a NotMyDiddle" );
		goto ERROR_EXIT;
	}
	*pDiddle	= NextDiddle;

	DiddleMsgData->Phantom = Diddle->Phantom;

	if( NextDiddle == DiddleHeader->DiddleList )
	{
		if( !Invalidate() )
			goto ERROR_EXIT;

		// Restore previous diddle
		if( NextDiddle->Next )
			DiddleToNode( NextDiddle );

		// Or prior-to-diddle value
		else
		{
            CacheFlagsSet ( NextDiddle->CacheFlags & SDB_NODE_CACHE_FLAGS_MASK );
            m_Value       = NextDiddle->Value;
            ValidSet      ( NextDiddle->SetValue );
            DiddleSet     ( false );

			free( NextDiddle );
			HashDelete( s_DiddleHash, this );
			free( DiddleHeader );
		}
	}
	else
		TRACE_FUNC( this, BURIED );

	// first remove any side effect diddles if any
	{
		SideEffectDiddleMap::iterator side_effect_it = s_side_effect_diddles.find( DiddleMsgData->DiddleID );
		if( side_effect_it != s_side_effect_diddles.end() )
		{
			if( side_effect_it->second.first != this ) // if removing diddle from sideeffect node before root, then remove the sideeffect node
				side_effect_it->second.second->erase( this );
			else							// is the root, so remove diddles from all sideeffect nodes first
			{
				auto_ptr<NodeSet> side_effects( side_effect_it->second.second );
				s_side_effect_diddles.erase( side_effect_it );
				
				for( NodeSet::iterator it = side_effects->begin(); it != side_effects->end(); ++it )
					side_effect_status = (*it)->RemoveDiddle( DiddleMsgData->DiddleID ) && side_effect_status;
			}
		}
	}

	if( !( Diddle->CacheFlags & SDB_CACHE_NO_FREE ))
		DTM_FREE( &Diddle->Value );
	free( Diddle );
	DiddleCount--;
	SecDbFree( SecPtr );

	return side_effect_status;


ERROR_EXIT:
	TRACE_FUNC( this, ERROR );
	return( FALSE );
}



/****************************************************************
**	Routine: SDB_NODE::DiddleChange
**	Returns: true/Err
**	Action : Changes a diddle by DiddleID
****************************************************************/

bool SDB_NODE::DiddleChange(
	SDB_OBJECT		*SecPtr,
	DT_VALUE		*Value,
	SDB_M_DIDDLE	*DiddleMsgData,
	int				OkIfNotFound,
	SDB_DIDDLE_ID	&PrevDiddleID
)
{
	SDB_DIDDLE
			*Diddle,
			**pDiddle;

	SDB_DIDDLE_HEADER
			*DiddleHeader;

	DT_VALUE
			TempValue;

	int		CopiesNeeded;


	DTM_INIT( &TempValue );

	if( !DiddleGet() )
	{
		if( OkIfNotFound )
			return TRUE;

		SDB_VALUE_TYPE_INFO
				*Vti = VtiGet();

		Err( ERR_INVALID_ARGUMENTS, "ChangeDiddle: %s( %s ) has no diddles", Vti ? Vti->Name : "Unknown VT", SecPtr->SecData.Name );
		goto ERROR_EXIT;
	}
	if( !( DiddleHeader = (SDB_DIDDLE_HEADER *) HashLookup( s_DiddleHash, this )))
	{
		ErrMore( "ChangeDiddle: Software error" );
		goto ERROR_EXIT;
	}


/*
**	If a set under request, go to bottom of stack and replace value
*/

	if( SDB_DIDDLE_ID_SET_VALUE == DiddleMsgData->DiddleID )
	{
		if(	!( DiddleMsgData->Flags & SDB_SET_NO_MESSAGE ))
		{
			Err( ERR_INVALID_ARGUMENTS, "SDB_DIDDLE_ID_SET_VALUE does not support value function messaging" );
			goto ERROR_EXIT;
		}
		for( Diddle = DiddleHeader->DiddleList; Diddle->Next; Diddle = Diddle->Next );

		if( !( Diddle->CacheFlags & SDB_CACHE_NO_FREE ))
			DTM_FREE( &Diddle->Value );
		DTM_INIT( &Diddle->Value );

		if( !( DiddleMsgData->Flags & ( SDB_CACHE_TOUCH | SDB_CACHE_NO_COPY )))
		{
			if( !DTM_ASSIGN( &Diddle->Value, Value ))
				goto ERROR_EXIT;
		}
		else
			Diddle->Value = *Value;

		Diddle->SetValue	= TRUE;
		Diddle->CacheFlags	= DiddleMsgData->Flags;
		return( TRUE );
	}


/*
**	Find diddle and bring to front of diddle list
*/

	pDiddle = &DiddleHeader->DiddleList;

	// Only change a NotMyDiddle if the existing top is a NotMyDiddle with PrevDiddleID
	// else no need to do this diddle
	if( DiddleMsgData->NotMyDiddle )
	{
		if( !(*pDiddle)->NotMyDiddle || (*pDiddle)->DiddleID != PrevDiddleID )
			return TRUE;
		(*pDiddle)->DiddleID = DiddleMsgData->DiddleID;
		(*pDiddle)->Phantom = DiddleMsgData->Phantom;
	}
	// now only replace a diddle if it has the same diddle id
	if( !DiddleMsgData->NotMyDiddle || !(*pDiddle)->NotMyDiddle )
	{
		for( ; (*pDiddle)->Next && (*pDiddle)->DiddleID != DiddleMsgData->DiddleID; pDiddle = &(*pDiddle)->Next );

		// check to see if we found the diddle
		if( (*pDiddle)->DiddleID != DiddleMsgData->DiddleID )
		{
			if( OkIfNotFound )
				return TRUE;

			Err( ERR_INVALID_ARGUMENTS, "Trying to change a diddle which doesn't exist" );
			goto ERROR_EXIT;
		}
		// if prev diddle ID was the one for the fake set value on the diddle stack, then its as if there was no diddle
		if( ( PrevDiddleID = DiddleHeader->DiddleList->DiddleID ) == SDB_DIDDLE_ID_SET_VALUE )
			PrevDiddleID = SDB_DIDDLE_ID_ERROR;
	}
	Diddle = *pDiddle;

	*pDiddle = Diddle->Next;
	Diddle->Next = DiddleHeader->DiddleList;
	DiddleHeader->DiddleList = Diddle;


/*
**	Replace the value
*/

	// Take care of ref count on Value (since PlaceDiddle must own exactly one copy)
	CopiesNeeded = !( DiddleMsgData->Flags & SDB_CACHE_NO_COPY ) + !!( DiddleMsgData->Flags & SDB_CACHE_NO_FREE );
	if( CopiesNeeded )
	{
		while( CopiesNeeded-- > 0 )
			if( !DTM_ASSIGN( &TempValue, Value ))
				goto ERROR_EXIT;
		Value = &TempValue;
	}
	DiddleMsgData->Flags &= SDB_SET_NO_MESSAGE;

	if( !Invalidate() )
		goto ERROR_EXIT;

	if( !PlaceDiddle( Diddle, SecPtr, Value, SDB_MSG_DIDDLE_CHANGED, DiddleMsgData ))
		goto ERROR_EXIT;

	DiddleToNode( Diddle );
	return( TRUE );


ERROR_EXIT:
	DTM_FREE( &TempValue );
	Error( "ChangeDiddle" );
	TRACE_FUNC( this, ERROR );
	return( FALSE );
}



/****************************************************************
**	Routine: SecDbSetDiddle
**	Returns: DiddleID
**	Summary: Temporarily override a value for a security
****************************************************************/

SDB_DIDDLE_ID SecDbSetDiddle(
	SDB_OBJECT		*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE	ValueType,		// ValueType to change
	DT_VALUE		*Value,			// New value for diddle
	SDB_SET_FLAGS	Flags,			// Message flags
	SDB_DIDDLE_ID	DiddleID		// DiddleID (value or SDB_DIDDLE_ID_NEW)
)
{
	return SecDbSetDiddleWithArgs( SecPtr, ValueType, 0, NULL, Value, Flags, DiddleID );
}



/****************************************************************
**	Routine: SecDbChangeDiddle
**	Returns: TRUE	- Diddle changed without error
**			 FALSE	- Error changing diddle
**	Summary: Change specified diddle
****************************************************************/

SDB_DIDDLE_ID SecDbChangeDiddle(
	SDB_OBJECT		*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE	ValueType,		// ValueType to change
	DT_VALUE		*Value,			// New value for diddle
	SDB_SET_FLAGS	Flags,			// Message flags
	SDB_DIDDLE_ID	DiddleID		// DiddleID (from SecDbSetDiddle)
)
{
	return SecDbChangeDiddleWithArgs( SecPtr, ValueType, 0, NULL, Value, Flags, DiddleID );
}



/****************************************************************
**	Routine: SecDbRemoveDiddle
**	Returns: TRUE	- DiddleGet(s) removed without error
**			 FALSE	- Couldn't find diddle to remove
**	Summary: Remove one or many diddles from a security
****************************************************************/

bool SecDbRemoveDiddle(
	SDB_OBJECT		*SecPtr,	// Pointer to security
	SDB_VALUE_TYPE	ValueType,	// Value type or NULL for all diddles
	SDB_DIDDLE_ID	DiddleID	// Id of diddle(s) to remove
)
{
	SDB_NODE
			*pNode;

	SDB_ENUM_PTR
			EnumPtr;

	if( ValueType )
		return SecDbRemoveDiddleWithArgs( SecPtr, ValueType, 0, NULL, DiddleID );


/*
**	If ValueType is NULL, free all Diddles on node
*/

	for( pNode = SecDbNodeEnumFirst( SecPtr, SDB_NODE_ENUM_TERMINALS, &EnumPtr );
		 pNode;
		 pNode = SecDbNodeEnumNext( EnumPtr ))
	{
		while( pNode->DiddleGet() )
			pNode->RemoveDiddle( pNode->DiddleHeaderGet()->DiddleList->DiddleID );
	}
	SecDbNodeEnumClose( EnumPtr );
	return true;
}



/****************************************************************
**	Routine: SDB_NODE::WhiteoutDiddle
**	Returns: SDB_DIDDLE_ID
**	Action : adds a whiteout diddle to node and all sideeffects
****************************************************************/

SDB_DIDDLE_ID SDB_NODE::WhiteoutDiddle(
)
{
	SDB_DIDDLE_HEADER* header = this->DiddleHeaderGet();
	if( !header || !header->DiddleList )
		return Err( ERR_INVALID_ARGUMENTS, "%s: node is not diddled, cannot whiteout", this->GetText(0).c_str() );

	CC_STL_VECTOR(SDB_NODE*) nodes;

	bool is_phantom = false;

	SideEffectDiddleMap::const_iterator side_effect_it = s_side_effect_diddles.find( header->DiddleList->DiddleID );
	if( side_effect_it != s_side_effect_diddles.end() && side_effect_it->second.first->FindAgain( SDB_NODE::CREATE, SecDbRootDb, SDB_NODE::DO_NOT_SHARE ) == this )
	{
		is_phantom = true;

		for( NodeSet::iterator it = side_effect_it->second.second->begin(); it != side_effect_it->second.second->end(); ++it )
		{
			SDB_NODE* node1 = (*it)->FindAgain( SDB_NODE::DO_NOT_CREATE, SecDbRootDb, SDB_NODE::SHARE );
			if( !node1->IsTerminal() )
				return Err( ERR_INVALID_ARGUMENTS, "One of side effect nodes is not a terminal" );
			if( node1 )
				nodes.push_back( node1 );
		}
	}
	SDB_DIDDLE_ID NewId;
	{
		dt_value_var null_value( this->VtiGet()->ValueType->DataType );

		NewId = this->SetDiddle( null_value.get(), SDB_SET_NO_MESSAGE | SDB_DIDDLE_IS_PHANTOM, SDB_DIDDLE_ID_NEW );
		if( NewId == SDB_DIDDLE_ID_ERROR )
		{
			ErrMore( "could not place whiteout diddle" );
			return SDB_DIDDLE_ID_ERROR;
		}
	}

	CC_STL_VECTOR(SDB_NODE*)::iterator it;

	if( is_phantom )
		SDB_NODE::MarkPhantomDiddle( NewId, this );

	SDB_DIDDLE_ID ret = NewId;
	try
	{
		for( it = nodes.begin(); it != nodes.end(); ++it )
		{
			dt_value_var null_value( (*it)->VtiGet()->ValueType->DataType );

			if( (*it)->SetDiddle( null_value.get(), SDB_SET_NO_MESSAGE | SDB_DIDDLE_IS_PHANTOM, NewId ) == SDB_DIDDLE_ID_ERROR )
				throw false;
		}
	}
	catch( bool& )
	{
		// take off any whiteouts we put on
		ErrPush();
		for( CC_STL_VECTOR(SDB_NODE*)::iterator it1 = nodes.begin(); it1 != it; ++it1 )
			(*it1)->RemoveDiddle( NewId );
		this->RemoveDiddle( NewId );
		ErrPop();
		ErrMore( "whiteout sideffect diddle failed" );
		ret = SDB_DIDDLE_ID_ERROR;
	}

	if( is_phantom )
		SDB_NODE::UnMarkPhantomDiddle( NewId );

	return ret;
}



/****************************************************************
**	Routine: SDB_NODE::SetDiddle
**	Returns: DIDDLE_ID
**	Action : sets a diddle on a node
****************************************************************/

SDB_DIDDLE_ID SDB_NODE::SetDiddle(
	DT_VALUE*       Value,
	SDB_SET_FLAGS	Flags,
	SDB_DIDDLE_ID	DiddleID
)
{
	SDB_M_DIDDLE
			DiddleMsgData;

	SDB_DIDDLE_ID
			PrevDiddleID = SDB_DIDDLE_ID_ERROR; 

	if( !this->IsTerminal() || this->ObjectGet()->AccessValue()->DataType != DtSecurity )
		return Err( ERR_INVALID_ARGUMENTS, "Node %s is not a terminal, cannot diddle", SDB_NODE::IsNode( this ) ? this->GetText( 0 ).c_str() : "<null>" );

	if( DiddleID == SDB_DIDDLE_ID_WHITEOUT )
	{
		if( !this->DiddleGet() )
		{
			Err( ERR_INVALID_ARGUMENTS, "Node %s is not diddled, cannot whiteout", this->GetText( 0 ).c_str() );
			return SDB_DIDDLE_ID_ERROR;
		}

		return this->WhiteoutDiddle();
	}

	SDB_OBJECT* SecPtr = (SDB_OBJECT*) this->ObjectGet()->AccessValue()->Data.Pointer;

	int Argc = this->ArgcGet();

	auto_ptr_array<DT_VALUE*> args( new DT_VALUE*[ Argc ] );
	{
		for( int i = 0; i < Argc; ++i )
			args[i] = this->ArgGet( i )->AccessValue();
	}
	DT_VALUE** Argv = args.get();

	memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));

	if( DiddleID == SDB_DIDDLE_ID_NEW )
		DiddleID = NodeDiddleIDs++;

	if( DiddleID == SDB_DIDDLE_ID_NEW_PHANTOM )
	{
		DiddleID = NodeDiddleIDs++;
		DiddleMsgData.Phantom = TRUE;
	}

	if( Flags & SDB_DIDDLE_IS_PHANTOM )
		DiddleMsgData.Phantom = TRUE;

	SDB_NODE* root_phantom;
	if( !( root_phantom = SDB_NODE::IsPhantomDiddle( DiddleID )))
		SDB_NODE::MarkPhantomDiddle( DiddleID, this );

	sdb_node_trace_scope<SDB_TRACE_SET_DIDDLE, SDB_TRACE_DONE_DIDDLE > trace( this );

	DiddleMsgData.Argc		= Argc;
	DiddleMsgData.Argv		= Argv;
	DiddleMsgData.Flags		= Flags;
	DiddleMsgData.DiddleID	= DiddleID;
	DiddleID = this->DiddleSet( SecPtr, Value, &DiddleMsgData, PrevDiddleID );

	if( !root_phantom )
		SDB_NODE::UnMarkPhantomDiddle( DiddleID );

	if( DiddleID != SDB_DIDDLE_ID_ERROR )
	{
		int Phantom = DiddleMsgData.Phantom;

		DT_VALUE
				TmpValue;

		if( root_phantom )					// add this diddle as a sideeffect
			this->AddAsSideEffect( DiddleID, root_phantom );

		if( Phantom )
			DTM_INIT( &TmpValue );
		else
			TmpValue = *Value;

		Value = &TmpValue;
		Flags &= ~( SDB_CACHE_NO_COPY | SDB_CACHE_NO_FREE );

		for( SDB_NODE::nodes_iterator it = SDB_NODE::nodes().lower_bound( this ); it != SDB_NODE::nodes().end() && equal( this, *it ); ++it )
		{
			SDB_NODE *Node = *it;
			if( !Node->DbSetGet()->BaseDb() ) // node seems ****ed up, we'll just ignore it
				continue;				// and hope someone fixes it

			if( Node == this ) // oh no, not this one again
				continue;

			SDB_DBSET_BEST *Best = SDB_DBSET_BEST::Lookup( Node->DbSetGet(), this->DbSetGet()->BaseDb() );
			if( !Best->m_Shareable )
				continue;

			sdb_node_trace_scope< SDB_TRACE_SET_DIDDLE, SDB_TRACE_DONE_DIDDLE > trace1( Node );

			memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
			DiddleMsgData.Argc		= Argc;
			DiddleMsgData.Argv		= Argv;
			DiddleMsgData.Flags		= Flags | SDB_SET_NO_MESSAGE;
			DiddleMsgData.DiddleID	= DiddleID;
			DiddleMsgData.NotMyDiddle = 1;
			DiddleMsgData.Phantom	= Phantom;

			// Since this is a NotMyDiddle, DiddleSet will ensure that it only does
			// the diddle if the PrevDiddleID matches
			Node->DiddleSet( SecPtr, Value, &DiddleMsgData, PrevDiddleID );
		}
	}
	return DiddleID;
}



/****************************************************************
**	Routine: SecDbSetDiddleWithArgs
**	Returns: DiddleID
**	Summary: Temporarily override a value for a security
****************************************************************/

SDB_DIDDLE_ID SecDbSetDiddleWithArgs(
	SDB_OBJECT		*SecPtr,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	DT_VALUE		**Argv,
	DT_VALUE		*Value,
	SDB_SET_FLAGS	Flags,
	SDB_DIDDLE_ID	DiddleID
)
{
	SDB_NODE
			*NodeToDiddle;

	// Create or split Node to diddle
	NodeToDiddle = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::DO_NOT_CHECK_SUPPORTED, SecDbRootDb, SDB_NODE::DO_NOT_SHARE );
	if( !SDB_NODE::IsNode( NodeToDiddle ) )
		return ErrMore( "SetDiddle" );

	return NodeToDiddle->SetDiddle( Value, Flags, DiddleID );
}



/****************************************************************
**	Routine: SDB_NODE::RemoveDiddle
**	Returns: true/false
**	Action : Removes specified diddle
****************************************************************/

bool SDB_NODE::RemoveDiddle(
	SDB_DIDDLE_ID	DiddleID
)
{
	SDB_NODE
			*Node;

	SDB_M_DIDDLE
			DiddleMsgData;

	bool	Status = true,
			HaveDiddlesToRemove = false;

	SDB_DBSET_BEST
			*BestDbSet = NULL;

	SDB_DIDDLE
			*NewDiddle = NULL;			// New Diddle to put on

	SDB_DIDDLE_ID
			PrevDiddleID;

	DT_VALUE
			TmpValue,
			*Value = NULL;				// quiet bogus gcc uninitialized var warning


	if( !this->IsTerminal() || this->ObjectGet()->AccessValue()->DataType != DtSecurity )
		return Err( ERR_INVALID_ARGUMENTS, "Node %s is not a terminal, cannot remove diddle", SDB_NODE::IsNode( this ) ? this->GetText( 0 ).c_str() : "<null>" );

	SdbObjectPtr sec_ptr( (SDB_OBJECT*) this->ObjectGet()->AccessValue()->Data.Pointer ); // make sure the SecPtr doesn't go away until we are done

	int Argc = this->ArgcGet();

	auto_ptr_array<DT_VALUE*> args( new DT_VALUE*[ Argc ] );
	{
		for( int i = 0; i < Argc; ++i )
			args[i] = this->ArgGet( i )->AccessValue();
	}
	DT_VALUE** Argv = args.get();

	Node = this;				// For Trace messages to work

	sdb_node_trace_scope<SDB_TRACE_REMOVE_DIDDLE, SDB_TRACE_DONE_DIDDLE> trace_guard( this );

	memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
	DiddleMsgData.Argc		= Argc;
	DiddleMsgData.Argv		= Argv;
	DiddleMsgData.DiddleID	= DiddleID;

	if( !Node->DiddleRemove( sec_ptr.val(), &DiddleMsgData ) )
		return false;

	if( !Node->DiddleGet() )
	{
		// We may need to put a diddle back on this node
		// so iterate over all nodes and look for the best one to use
		SDB_NODE::nodes_iterator_range range = SDB_NODE::nodes().equal_range( this );
		for( SDB_NODE::nodes_iterator it = range.first; it != range.second; ++it )
		{
			Node = *it;
			// ignore the original node and undiddled nodes
			if( Node == this || !Node->DiddleGet() || !Node->DbSetGet()->BaseDb() )
				continue;

			SDB_DIDDLE_HEADER
					*DiddleHeader = Node->DiddleHeaderGet();

			SDB_DBSET_BEST *Best = SDB_DBSET_BEST::Lookup( Node->DbSetGet(), this->DbSetGet()->BaseDb() );
			if( !Best )
				continue;

			// The BestPosition test ensures that we only use dbsets whose elements are wholly contained in Db
			// (the dbset best logic adds Db->DbDepth * SubpathSize to the position in case it found a Subpath of a DbSetElem inside Db)
			if( DbSetBestGreater( Best, BestDbSet ) && Best->m_BestPosition < this->DbSetGet()->BaseDb()->DbDepth && DiddleHeader->DiddleList->DiddleID != DiddleID )
			{
				BestDbSet = Best;
				NewDiddle = DiddleHeader->DiddleList;
			}
			if( DiddleHeader->DiddleList->DiddleID == DiddleID )
				HaveDiddlesToRemove = true;
		}
	}
	else
	{
		// Need to diddle all extended diddles to the newly uncovered one
		NewDiddle = this->DiddleHeaderGet()->DiddleList;
		HaveDiddlesToRemove = true;		// just set this to true
	}

	// Ok, we need to transfer the diddle from this node over
	if( !this->DiddleGet() && NewDiddle )
	{
		Node = this;

		if( NewDiddle->Phantom )
		{
			DTM_INIT( &TmpValue );
			Value = &TmpValue;
		}
		else
			Value = &NewDiddle->Value;

		sdb_node_trace_scope<SDB_TRACE_REMOVE_DIDDLE, SDB_TRACE_DONE_DIDDLE> trace1( Node );

		memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
		DiddleMsgData.Argc		= Argc;
		DiddleMsgData.Argv		= Argv;
		DiddleMsgData.Flags		= SDB_SET_NO_MESSAGE;
		DiddleMsgData.DiddleID	= NewDiddle->DiddleID;
		DiddleMsgData.NotMyDiddle = 1;
		DiddleMsgData.Phantom	= NewDiddle->Phantom;
		PrevDiddleID			= SDB_DIDDLE_ID_ERROR;

		Node->DiddleSet( sec_ptr.val(), Value, &DiddleMsgData, PrevDiddleID );
	}
	else if( NewDiddle )
	{
		if( NewDiddle->Phantom )
		{
			DTM_INIT( &TmpValue );
			Value = &TmpValue;
		}
		else
			Value = &NewDiddle->Value;
	}

	// other nodes were diddled to this node
	// If NewDiddle then diddle them again, else go and remove their diddles
	if( HaveDiddlesToRemove )
	{
		PrevDiddleID = DiddleID;
		SDB_NODE::nodes_iterator_range range = SDB_NODE::nodes().equal_range( this );
		for( SDB_NODE::nodes_iterator it = range.first; it != range.second; ++it )
		{
			Node = *it;
			if( Node != this && Node->DiddleGet() && Node->DiddleHeaderGet()->DiddleList->DiddleID == DiddleID )
			{
				// If no new diddle, then simply take it off
				if( !NewDiddle )
				{
					sdb_node_trace_scope<SDB_TRACE_REMOVE_DIDDLE, SDB_TRACE_DONE_DIDDLE> trace2( Node );

					memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
					DiddleMsgData.Argc		= Argc;
					DiddleMsgData.Argv		= Argv;
					DiddleMsgData.DiddleID	= DiddleID;
					DiddleMsgData.NotMyDiddle = 1;

					Status = Node->DiddleRemove( sec_ptr.val(), &DiddleMsgData ) && Status;
				}
				else					// replacing diddle
				{
					sdb_node_trace_scope<SDB_TRACE_SET_DIDDLE, SDB_TRACE_DONE_DIDDLE> trace3( Node );

					memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
					DiddleMsgData.Argc		= Argc;
					DiddleMsgData.Argv		= Argv;
					DiddleMsgData.Flags		= SDB_SET_NO_MESSAGE;
					DiddleMsgData.DiddleID	= NewDiddle->DiddleID;
					DiddleMsgData.NotMyDiddle = 1;
					DiddleMsgData.Phantom	= NewDiddle->Phantom;

					Node->DiddleSet( sec_ptr.val(), Value, &DiddleMsgData, PrevDiddleID );
				}
			}
		}
	}

	return Status;
}



/****************************************************************
**	Routine: SecDbRemoveDiddleWithArgs
**	Returns: TRUE or FALSE
**	Action : Removes specified diddle
****************************************************************/

bool SecDbRemoveDiddleWithArgs(
	SDB_OBJECT		*SecPtr,
	SDB_VALUE_TYPE	ValueType,
	int				Argc,
	DT_VALUE		**Argv,
	SDB_DIDDLE_ID	DiddleID
)
{
	SDB_NODE
			*NodeToRemove;

	// share nodes during NodeFind() because the DbSet of the Node may have been extended after the diddle
	NodeToRemove = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::DO_NOT_CHECK_SUPPORTED, SecDbRootDb, SDB_NODE::SHARE );
	if( !SDB_NODE::IsNode( NodeToRemove ) )
		return ErrMore( "RemoveDiddle" );

	return NodeToRemove->RemoveDiddle( DiddleID );
}



/****************************************************************
**	Routine: SDB_NODE::ChangeDiddle
**	Returns: TRUE/Err
**	Action : Change specified diddle
****************************************************************/

SDB_DIDDLE_ID SDB_NODE::ChangeDiddle(
	DT_VALUE		*Value,
	SDB_SET_FLAGS	Flags,
	SDB_DIDDLE_ID	DiddleID
)
{
	SDB_NODE
			*Node;

	SDB_M_DIDDLE
			DiddleMsgData;

	int		Status = TRUE;

	SDB_DIDDLE_ID
			PrevDiddleID = SDB_DIDDLE_ID_ERROR;

	int		ArgNum;

	if( !IsTerminalQuick() )
		return Err( ERR_INVALID_ARGUMENTS, "ChangeDiddle(): Node isn't a terminal" );

	// FIX- this is kind of silly that we reget the secptr/valuetype/args from the node
	// we should probably change the message to get a list of nodes as args and node dt_value *s
	SDB_OBJECT
			*SecPtr = (SDB_OBJECT *) ObjectGet()->AccessValue()->Data.Pointer;

	DT_VALUE
			**Argv = ArgcGet() ? ( (DT_VALUE **) alloca( sizeof( SDB_NODE * ) * ArgcGet() )) : 0;

	for( ArgNum = ArgcGet() - 1; ArgNum >= 0; --ArgNum )
		Argv[ ArgNum ] = ArgGet( ArgNum )->AccessValue();

	TRACE_FUNC( this, CHANGE );

	memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
	DiddleMsgData.Argc		= ArgcGet();
	DiddleMsgData.Argv		= Argv;
	DiddleMsgData.Flags		= Flags;
	DiddleMsgData.DiddleID	= DiddleID;

	if( !DiddleChange( SecPtr, Value, &DiddleMsgData, FALSE, PrevDiddleID ))
		Status = FALSE;

	TRACE_FUNC( this, DONE );

	if( !Status )
		return FALSE;

	int Phantom = DiddleMsgData.Phantom;

	DT_VALUE
			TmpValue;

	if( Phantom )
		DTM_INIT( &TmpValue );
	else
		TmpValue = *Value;
	Value = &TmpValue;

	// Force all diddles to copy and free the value
	Flags &= ~( SDB_CACHE_NO_COPY | SDB_CACHE_NO_FREE );

	// Change diddle for all matching nodes
	// we expect it to fail for a bunch of them
	nodes_iterator_range range = nodes().equal_range( this );
	for( nodes_iterator it = range.first; it != range.second; ++it )
	{
		Node = *it;
		if( Node == this || !Node->DbSetGet()->BaseDb() )		// Already done
			continue;

		// If diddling the set value, then we must do it for all nodes
		if( DiddleID != SDB_DIDDLE_ID_SET_VALUE )
		{
			SDB_DBSET_BEST *Best = SDB_DBSET_BEST::Lookup( Node->DbSetGet(), DbSetGet()->BaseDb() );

			// If not shareable then we never put a diddle on this node, so don't
			// even attempt to change it.
			// DiddleChange always replaces a NotMyDiddle with another NotMyDiddle
			// so it would be disastrous to attempt to change all diddles without doing this check
			if( !Best->m_Shareable )
				continue;
		}

		TRACE_FUNC( Node, CHANGE );

		memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
		DiddleMsgData.Argc		= ArgcGet();
		DiddleMsgData.Argv		= Argv;
		DiddleMsgData.Flags		= Flags | SDB_SET_NO_MESSAGE;
		DiddleMsgData.DiddleID	= DiddleID;
		DiddleMsgData.Phantom	= Phantom;
		DiddleMsgData.NotMyDiddle = 1;

		// PrevDiddleID will ensure that we never change diddles that we didn't do
		Status = Node->DiddleChange( SecPtr, Value, &DiddleMsgData, TRUE, PrevDiddleID ) && Status;

		TRACE_FUNC( Node, DONE );
	}
	return Status;	
}



/****************************************************************
**	Routine: SecDbChangeDiddleWithArgs
**	Returns: TRUE	- Diddle changed without error
**			 FALSE	- Error changing diddle
**	Summary: Change specified diddle
****************************************************************/

SDB_DIDDLE_ID SecDbChangeDiddleWithArgs(
	SDB_OBJECT		*SecPtr,		// Pointer to security
	SDB_VALUE_TYPE	ValueType,		// ValueType to change
	int				Argc,			// Number of arguments
	DT_VALUE		**Argv,			// Argument values
	DT_VALUE		*Value,			// New value for diddle
	SDB_SET_FLAGS	Flags,			// Message flags
	SDB_DIDDLE_ID	DiddleID		// DiddleID (from SecDbSetDiddle)
)
{
	// share nodes during NodeFind() because the DbSet of the Node may have been extended after the diddle
	SDB_NODE *NodeToDiddle = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::DO_NOT_CHECK_SUPPORTED, SecDbRootDb, SDB_NODE::SHARE );
	if( !NodeToDiddle )
		return ErrMore( "ChangeDiddle" );

	return NodeToDiddle->ChangeDiddle( Value, Flags, DiddleID );
}



/****************************************************************
**	Routine: SDB_NODE::DiddleStackMoveTo
**	Returns: TRUE/Err
**	Action : Moves the diddle stack from the Src to the Dest node
**			 and puts the top diddle as a NotMyDiddle onto the
**			 source node
****************************************************************/

bool SDB_NODE::DiddleStackMoveTo(
	SDB_NODE	*NodeDest
)
{
	SDB_DIDDLE_HEADER
			*DiddleHeader;

	SDB_DIDDLE
			*DiddlePtr;

	int		Arg;

	SDB_OBJECT
			*SecPtr;

	DT_VALUE
			*ObjectValue;

	SDB_M_DIDDLE
			DiddleMsgData;

	SDB_DIDDLE_ID
			PrevDiddleID;

	Trace( this, SDB_TRACE_DIDDLE_STACK_MOVE, NodeDest );

	if( !NodeDest->ObjectGet()->ValidGet() || !( ObjectValue = NodeDest->ObjectGet()->AccessValue() )
		|| ObjectValue->DataType != DtSecurity )
		return Err( ERR_INVALID_ARGUMENTS, "DiddleStackMove(): Node isn't a terminal" ), false;

	SecPtr = (SDB_OBJECT *) ObjectValue->Data.Pointer;

	if( !( DiddleHeader = DiddleHeaderGet() ))
		return Err( ERR_NOT_CONSISTENT, "Node has no diddles to move" ), false;
	DiddlePtr = DiddleHeader->DiddleList;

	// Make a list of the args to the node because the diddle messages want them
	DT_VALUE
			**Argv = ArgcGet() ? (DT_VALUE **) alloca( sizeof( DT_VALUE * ) * ArgcGet() ) : NULL;

	for( Arg = 0; Arg < ArgcGet(); ++Arg )
		Argv[ Arg ] = ArgGet( Arg )->AccessValue();

	SDB_DIDDLE_HEADER* target_hdr = (SDB_DIDDLE_HEADER*) HashLookup( s_DiddleHash, NodeDest );
	if( target_hdr )
	{
		if( !target_hdr->DiddleList->NotMyDiddle )
			return Err( ERR_NOT_CONSISTENT, "Node to move diddle stack onto is already diddled" );

		// Else take off the notmydiddle so that we can move the diddle stack onto it
		sdb_node_trace_scope< SDB_TRACE_REMOVE_DIDDLE, SDB_TRACE_DONE_DIDDLE > tracer( NodeDest );

		SDB_M_DIDDLE diddle_msg;
		memset( &diddle_msg, 0, sizeof( diddle_msg ));
        diddle_msg.Argc        = ArgcGet(); // This is the same as NodeDest->ArgcGet()
        diddle_msg.Argv        = Argv;
		diddle_msg.Flags       = SDB_SET_NO_MESSAGE;
        diddle_msg.DiddleID    = target_hdr->DiddleList->DiddleID;
        diddle_msg.NotMyDiddle = 1;

		if( !NodeDest->DiddleRemove( SecPtr, &diddle_msg ) )
			return false;

		if( NodeDest->DiddleGet() || ( target_hdr = (SDB_DIDDLE_HEADER*) HashLookup( s_DiddleHash, NodeDest ) ) )
			return Err( ERR_NOT_CONSISTENT, "Removed NotMyDiddle from target but it still has a diddle!" );
	}

	// FIX- This seems really dangerous, it assumes lots of knowledge about diddles and the node layout
	// might want to think of a cleaner way to do it
    DiddleHeader->Node      = NodeDest;
    HashInsert              ( s_DiddleHash, NodeDest, DiddleHeader );
    NodeDest->CacheFlagsSet ( CacheFlagsGet()                      );
    NodeDest->m_Value       = m_Value;
    NodeDest->DiddleSet     ( DiddleGet()                          );
	// FIX- HACK GetValue overloads the Valid bit for cycle detection. It should not do so
	// and this hack should be removed
    NodeDest->ValidSet      ( GetValueLockGet() ? false : ValidGet() );

	CacheFlagsSet  			( CacheFlagsGet() | SDB_CACHE_NO_FREE  );
	DiddleSet				( false );
	HashDelete( s_DiddleHash, this );

	{
		for( SDB_DIDDLE* diddle = DiddleHeader->DiddleList; diddle; diddle = diddle->Next )
		{
			SideEffectDiddleMap::iterator side_effect_it = s_side_effect_diddles.find( diddle->DiddleID );
			if( side_effect_it == s_side_effect_diddles.end() )
				continue;				// no phantom/sideeffect diddles

			if( side_effect_it->second.first == this )
				side_effect_it->second.first = NodeDest; // root phantom diddle has moved;
			else						// side effect may have moved
			{
				NodeSet& side_effects = *side_effect_it->second.second;
				NodeSet::iterator it = side_effects.find( this );
				if( it != side_effects.end() ) // side effect has moved
				{
					side_effects.erase( this );
					side_effects.insert( NodeDest );
				}
			}
		}
	}

	memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
	DiddleMsgData.Argc		= ArgcGet();
	DiddleMsgData.Argv		= Argv;
	DiddleMsgData.Flags		= SDB_SET_NO_MESSAGE;
	DiddleMsgData.DiddleID	= DiddlePtr->DiddleID;
	DiddleMsgData.Phantom	= DiddlePtr->Phantom;
	DiddleMsgData.NotMyDiddle = 1;
	PrevDiddleID			= SDB_DIDDLE_ID_ERROR;

	if( !DiddleSet( SecPtr, &DiddlePtr->Value, &DiddleMsgData, PrevDiddleID ))
		return false;

	// Now check to see if there is a SetValue under the diddle stack and copy it across too
	while( DiddlePtr->Next )
		DiddlePtr = DiddlePtr->Next;

	if( DiddlePtr->SetValue )
	{
		memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
		DiddleMsgData.Argc		= ArgcGet();
		DiddleMsgData.Argv		= Argv;
		DiddleMsgData.Flags		= SDB_SET_UNDER_DIDDLE | SDB_SET_NO_MESSAGE;
		DiddleMsgData.DiddleID	= SDB_DIDDLE_ID_SET_VALUE;

		if( !NodeDest->DiddleChange( SecPtr, &DiddlePtr->Value, &DiddleMsgData, TRUE, PrevDiddleID ))
			return false;
	}

	return true;
}



/****************************************************************
**	Routine: SDB_NODE::PlaceNotMyDiddleFrom
**	Returns: TRUE/Err
**	Action : Puts a NotMyDiddle on the dest node
****************************************************************/

bool SDB_NODE::PlaceNotMyDiddleFrom(
	SDB_NODE	*NodeSrc
)
{
	SDB_DIDDLE_HEADER
			*DiddleHeader;

	SDB_DIDDLE
			*DiddlePtr;

	int		Arg;

	SDB_OBJECT
			*SecPtr;

	DT_VALUE
			*ObjectValue;

	SDB_M_DIDDLE
			DiddleMsgData;

	SDB_DIDDLE_ID
			PrevDiddleID;


	Trace( this, SDB_TRACE_PLACE_NOT_MY_DIDDLE, NodeSrc );

	if( !ObjectGet()->ValidGet() || !( ObjectValue = ObjectGet()->AccessValue() )
		|| ObjectValue->DataType != DtSecurity )
		return Err( ERR_INVALID_ARGUMENTS, "DiddleStackCopy(): Node isn't a terminal" ), false;

	SecPtr = (SDB_OBJECT *) ObjectValue->Data.Pointer;

	if( !( DiddleHeader = (SDB_DIDDLE_HEADER *) HashLookup( s_DiddleHash, NodeSrc )))
		return Err( ERR_NOT_CONSISTENT, "Node has no diddles to copy" ), false;

	DiddlePtr = DiddleHeader->DiddleList;

	DT_VALUE
			**Argv = ArgcGet() ? (DT_VALUE **) alloca( sizeof( DT_VALUE * ) * ArgcGet() ) : NULL;

	for( Arg = 0; Arg < ArgcGet(); ++Arg )
		Argv[ Arg ] = ArgGet( Arg )->AccessValue();

	memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
	DiddleMsgData.Argc		= ArgcGet();
	DiddleMsgData.Argv		= Argv;
	DiddleMsgData.Flags		= SDB_SET_NO_MESSAGE;
	DiddleMsgData.DiddleID	= DiddlePtr->DiddleID;
	DiddleMsgData.Phantom	= DiddlePtr->Phantom;
	DiddleMsgData.NotMyDiddle = 1;
	PrevDiddleID			= SDB_DIDDLE_ID_ERROR;

	if( !DiddleSet( SecPtr, &DiddlePtr->Value, &DiddleMsgData, PrevDiddleID ))
		return false;

	// Now check to see if there is a SetValue under the diddle stack and copy it across too
	while( DiddlePtr->Next )
		DiddlePtr = DiddlePtr->Next;

	if( DiddlePtr->SetValue )
	{
		memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
		DiddleMsgData.Argc		= ArgcGet();
		DiddleMsgData.Argv		= Argv;
		DiddleMsgData.Flags		= SDB_SET_UNDER_DIDDLE | SDB_SET_NO_MESSAGE;
		DiddleMsgData.DiddleID	= SDB_DIDDLE_ID_SET_VALUE;

		if( !DiddleChange( SecPtr, &DiddlePtr->Value, &DiddleMsgData, TRUE, PrevDiddleID ))
			return false;
	}

	return true;
}
