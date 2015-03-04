#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_setv.c,v 1.55 2001/07/23 21:17:26 vengrd Exp $"
/****************************************************************
**
**	SDB_SETV.C	- SecDbSetValue and friends
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_setv.c,v $
**	Revision 1.55  2001/07/23 21:17:26  vengrd
**	Permit sets of GsDt types that map correctly to actual types of VTs.
**
**	Revision 1.54  2001/03/14 18:33:26  singhki
**	Rewrite nodes to use STL style multiset instead of skip list
**	
**	Revision 1.53  2001/01/23 23:43:15  simpsk
**	Fixes for VC5(6).
**	
**	Revision 1.52  2000/09/27 18:29:42  shahia
**	Bug Fix: DefineClass create last err even when it succeeded
**	
**	Revision 1.51  2000/08/25 23:13:17  singhki
**	BaseDb may be NULL
**	
**	Revision 1.50  2000/04/24 11:04:24  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.49  2000/04/12 15:44:27  singhki
**	don't ignore diddled nodes in set but use DiddleChange with
**	SET_UNDER_DIDDLE to set the bottom of the diddle stack
**	
**	Revision 1.48  2000/04/12 03:47:35  singhki
**	do not set value of split node if it is diddled. Also clean up log
**	
**	Revision 1.47  2000/02/29 00:43:05  singhki
**	If node is marked as destroyed during SetValue, free it
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secerror.h>
#include	<sdb_int.h>
#include	<sdbdbset.h>
#include	<gsdt/GsDtToDtValue.h>
#include	<gsdt/x_gsdt.h>

CC_USING_NAMESPACE( Gs );


/*
**	Macros
*/

#define	TRACE_FUNC( Msg )													\
	(void) ( !SecDbTraceFunc || (SecDbTraceFunc)( SDB_TRACE_##Msg, pNode ))



/****************************************************************
**	Routine: SecDbSetValue
**	Returns: TRUE	- Value set without error
**			 FALSE	- Error setting value
**	Summary: Set a value on a security
****************************************************************/

int SecDbSetValue(
	SDB_OBJECT		*SecPtr,	// Security Object whose value is to be set
	SDB_VALUE_TYPE	ValueType,	// Value type to be set
	DT_VALUE		*Value,		// Value to set
	SDB_SET_FLAGS	Flags		// SDB_SET_FLAGS
)
{
	return( SecDbSetValueWithArgs( SecPtr, ValueType, 0, NULL, Value, Flags ));
}



/****************************************************************
**	Routine: SecDbSetValueWithArgs
**	Returns: TRUE or FALSE
**	Summary: Set a value on a security
**	Action : Generally caches the given value for the given node.
**			 Specifically:
**	
**				Action					Unless flag set
**				------					---------------
**				Call object function	SDB_SET_NO_MESSAGE
**	
**				Check for retention		SDB_SET_RETAINED_FLAG (on vti)
**	
**				Free old value			SDB_CACHE_NO_FREE (on orig value)
**										or SDB_CACHE_TOUCH
**
**				Copy new value			SDB_CACHE_NO_COPY or SDB_CACHE_TOUCH
**	
****************************************************************/

int SecDbSetValueWithArgs(
	SDB_OBJECT		*SecPtr,	// Security Object whose value is to be set
	SDB_VALUE_TYPE	ValueType,	// Value type to be set
	int				Argc,		// Number of arguments to value type
	DT_VALUE		**Argv,		// Argument values
	DT_VALUE		*Value,		// Value to set to
	SDB_SET_FLAGS	Flags		// SDB_SET_FLAGS
)
{
	static SDB_VALUE_TYPE_INFO StaticVti =
	{
		"Static VTI", &DtAny
	};

	int		Status	= TRUE;

	bool	SvDestroy;

	SDB_NODE
			*pNode,
			OldCache;
			
	SDB_M_DATA
			MsgData;
			
	SDB_VALUE_TYPE_INFO
			*Vti;

	DT_VALUE
			CoercedValue;
	
/*
**	Get the node
*/

	pNode = SDB_NODE::FindTerminalAndArgs( SecPtr, ValueType, Argc, Argv, SDB_NODE::CHECK_SUPPORTED, SecPtr->Db, SDB_NODE::DO_NOT_SHARE );
	if( !pNode || !Value )
		return( ErrMore( "SetValue" ));
	else
		ErrClear();

	TRACE_FUNC( SET );

	// In rare cases could be trying to set node to its own value--just succeed trivially (subsequent code would crash)
	// however if SDB_CACHE_TOUCH is set, then we do the set anwyays
	if( &pNode->m_Value == Value && !( Flags & SDB_CACHE_TOUCH ))
	{
		TRACE_FUNC( DONE_SET );
		return TRUE;
	}

/*
**	Illegal to set a locked or diddled value (just for protection against oneself)
*/

	if( pNode->DiddleGet() )
	{
		SDB_SET_FLAGS
				Mask = SDB_SET_UNDER_DIDDLE | SDB_SET_NO_MESSAGE;
				
		if(( Flags & Mask ) == Mask )
			return( SecDbChangeDiddleWithArgs( SecPtr, ValueType, Argc, Argv, Value, Flags, SDB_DIDDLE_ID_SET_VALUE ));

		Status = Err( ERR_UNSUPPORTED_OPERATION, "@ - cannot set a diddled value" );
	}
	else if( pNode->GetValueLockGet() )
		Status = Err( SDB_ERR_OBJECT_IN_USE, "Trying to set a value which is being calculated" );
	else if( pNode->SetOrDiddleLockGet() )
		Status = Err( SDB_ERR_OBJECT_IN_USE, "Trying to set a value which is already being set or diddled" );

	if( Flags & SDB_SET_DONT_LOOKUP_VTI )
		Vti = &StaticVti;
	else
		Vti = pNode->VtiGet();
	
	DTM_INIT( &CoercedValue );

	// Type checking ignored by DtAny
	if( !( Flags & SDB_SET_DONT_LOOKUP_VTI ) && Vti && Vti->ValueType->DataType != DtAny )
		if( Vti->ValueType->DataType != Value->DataType )
		{
			if( ( Flags & SDB_SET_COERCE_TYPES ) && !( Vti->ValueType->DataType->Flags & ( DT_FLAG_SUBSCRIPTS | DT_FLAG_COMPONENTS | DT_FLAG_REFERENCE ) ) )
			{
				DTM_ALLOC( &CoercedValue, Vti->ValueType->DataType );

				if( DTM_TO( &CoercedValue, Value ) )
					Value = &CoercedValue;
				else
					Status = ErrMore( "'%s' expects a '%s'.  Unable to coerce from '%s'", Vti->ValueType->Name, Vti->ValueType->DataType->Name, Value->DataType->Name );
			}
			else if( ( DtGsDt != Value->DataType ) || ( Vti->ValueType->DataType != GsDtEquivDtValue( *( (GsDt *)DT_VALUE_TO_POINTER( Value ) ) ) ) )				
				Status = Err( ERR_INVALID_ARGUMENTS, "'%s' expects a '%s' not a '%s'", Vti->ValueType->Name, Vti->ValueType->DataType->Name, Value->DataType->Name );
		}

	if( !Status || !Vti )
	{
		pNode->Error( "SetValue" );
		TRACE_FUNC( DONE_SET );
		TRACE_FUNC( ERROR_SET );
		DTM_FREE( &CoercedValue );
		return FALSE;
	}

	// Mark object as modified if we are changing an in-stream
	if( Vti->ValueFlags & SDB_IN_STREAM )
		SecPtr->Modified = TRUE;
	

/*
**	Invalidate this node and any descendants.
**
**	For non-retained values, the assumption is that the value func will do
**	all the work, i.e., presumably passes through the set value to the set
**	retained values.
*/

	OldCache = *pNode;
	if( !OldCache.ValidGet() )
		DTM_INIT( &OldCache.m_Value );
		
	if( Vti->ValueFlags & SDB_SET_RETAINED_FLAG )
		Flags |= SDB_CACHE_SET;

	SvDestroy = pNode->DestroyedGet();

	if( Flags & SDB_CACHE_SET )
	{
		if( pNode->ValidGet() )
		{
			// Flag value so that invalidation does not free it
			// because if we subsequently fail, then we want to restore it
			pNode->CacheFlagsSet( pNode->CacheFlagsGet() | SDB_CACHE_NO_FREE );
		}

		Status = pNode->Invalidate();	// If we subsequently fail, we have merely thrown away some would-be valid calculated nodes--oh well

		// Fake up the new value in the cache

		pNode->m_Value = *Value;
		pNode->ValidSet( true );
		pNode->CacheFlagsSet( SDB_CACHE_SET | SDB_CACHE_NO_FREE );
	}


/*
**	Send the message to the object
*/

	if( Status && !( Flags & SDB_SET_NO_MESSAGE ))
	{
		MsgData.SetValue.Argc			= Argc;
		MsgData.SetValue.Argv			= Argv;
		MsgData.SetValue.Value			= Value;
		MsgData.SetValue.OldValue		= &OldCache.m_Value;
		MsgData.SetValue.Flags			= Flags;
		pNode->SetOrDiddleLockSet( true );
		Status = SDB_MESSAGE_VT_TRACE( SecPtr, SDB_MSG_SET_VALUE, &MsgData, Vti );
		pNode->SetOrDiddleLockSet( false );

		if( pNode->DestroyedGet() && !SvDestroy )
		{
			if( Flags & SDB_CACHE_SET )
			{
				// ok, we might have to free the OldCache
				Flags |= SDB_CACHE_NO_COPY;// pretend this is set so we don't make a copy of the value as we are throwing away the node
			}
			Flags &= ~SDB_CACHE_TOUCH;	// so we won't keep the OldValue
		}
	}
	
	
/*
**	Make the node correct
*/

	if( Status && ( Flags & SDB_CACHE_SET ))
	{
		// Copy new value
        if( Flags & SDB_CACHE_TOUCH )
            pNode->m_Value = OldCache.m_Value;
		else if( !( Flags & SDB_CACHE_NO_COPY ))
			Status = DTM_ASSIGN( &pNode->m_Value, Value );
			
		if( Status )
		{
			// Set flags
			pNode->CacheFlagsSet( Flags & SDB_NODE_CACHE_FLAGS_MASK );

			// Free old value
			if( !( OldCache.CacheFlagsGet() & SDB_CACHE_NO_FREE ) && !( Flags & SDB_CACHE_TOUCH ))
				DTM_FREE( &OldCache.m_Value );
		}
	}


/*
**	Free old value and return
*/

	if( !Status )
	{
		// FIX - Ideally the object should get a SET_VALUE_BACKOUT message or something

		if( Flags & SDB_CACHE_SET )
		{
			// Value in pNode will not be freed because CACHE_NO_FREE is set
			pNode->Invalidate();

			// Now restore Value and Cache info from OldCache in order to free them
			pNode->m_Value = OldCache.m_Value;
			pNode->CacheFlagsSet( OldCache.CacheFlagsGet() );
			pNode->ValidSet( OldCache.ValidGet() );

			if( pNode->ValidGet() && !( pNode->CacheFlagsGet() & SDB_CACHE_SET ))
				pNode->Invalidate();
		}

		pNode->Error( "SetValue" );
		TRACE_FUNC( DONE_SET );
		TRACE_FUNC( ERROR_SET );
		DTM_FREE( &CoercedValue );

		if( pNode->DestroyedGet() && !SvDestroy )
			SDB_NODE::Free( pNode );

		return FALSE;
	}
	else if( Flags & SDB_CACHE_SET )	// go make other nodes split off of this one correct
	{
		SDB_NODE
				*Node;

		SDB_DBSET_BEST
				*Best;

		for( SDB_NODE::nodes_iterator it = SDB_NODE::nodes().lower_bound( pNode ); it != SDB_NODE::nodes().end() && equal( pNode, *it ); ++it )
		{
			Node = *it;
			if( Node != pNode && Node->DbSetGet()->BaseDb() && ( Best = SDB_DBSET_BEST::Lookup( Node->DbSetGet(), pNode->DbSetGet()->BaseDb() )) && Best->m_Shareable )
			{
				SDB_VALUE_TYPE_INFO
						*NodeVti = Node->VtiGet();

				// If we couldn't get the VTI or the node isn't set retained, ignore the set
				if( !NodeVti || !( NodeVti->ValueFlags & SDB_SET_RETAINED_FLAG ) ||
					( NodeVti->ValueType->DataType != DtAny && NodeVti->ValueType->DataType != Value->DataType ))
					continue;

				if( Node->DiddleGet() )
				{
					SDB_M_DIDDLE
							DiddleMsgData;

					SDB_DIDDLE_ID
							PrevDiddleId;

					memset( &DiddleMsgData, 0, sizeof( DiddleMsgData ));
					DiddleMsgData.Flags		= SDB_SET_UNDER_DIDDLE | SDB_SET_NO_MESSAGE;
					DiddleMsgData.DiddleID	= SDB_DIDDLE_ID_SET_VALUE;
					Node->DiddleChange( SecPtr, Value, &DiddleMsgData, TRUE, PrevDiddleId );
				}
				else
				{
					Node->Invalidate();	// invalidate Node (frees it's value too)

					// Set Node->Value
					DTM_ASSIGN( &Node->m_Value, Value );
					Node->CacheFlagsSet( SDB_CACHE_SET );
					Node->ValidSet( true );
				}
			}
		}
	}
	TRACE_FUNC( DONE_SET );
	DTM_FREE( &CoercedValue );

	if( pNode->DestroyedGet() && !SvDestroy )
		SDB_NODE::Free( pNode );

	return TRUE;
}
