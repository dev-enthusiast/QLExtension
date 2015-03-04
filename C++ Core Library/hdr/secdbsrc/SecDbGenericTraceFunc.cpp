#define GSCVSID "$Header: /home/cvs/src/secdb/src/SecDbGenericTraceFunc.cpp,v 1.14 2004/11/29 23:48:11 khodod Exp $"
/****************************************************************
**
**	SecDbGenericTraceFunc.cpp	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: SecDbGenericTraceFunc.cpp,v $
**	Revision 1.14  2004/11/29 23:48:11  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.13  2004/01/30 15:42:10  khodod
**	Use snprintf to guard against stack corruption.
**	
**	Revision 1.12  2003/12/23 19:27:57  khodod
**	BugFix: Use the thread-safe time api routines (iBug #9277).
**	
**	Revision 1.11  2001/11/27 23:23:31  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.9  2001/10/09 20:07:06  singhki
**	revert to min/max because roguewave is gone
**	
**	Revision 1.8  2001/10/02 18:44:31  singhki
**	rw and other fixes for NT, need to define own min/max grrr, fix should remove it when rw issues are sorted
**	
**	Revision 1.7  2001/09/19 19:11:41  singhki
**	do not allow args if purple during compile. also add trace
**	
**	Revision 1.6  2001/08/30 19:45:19  nauntm
**	added function headers
**	
**	Revision 1.5  2001/08/15 15:45:38  simpsk
**	remove deprecated #includes.
**	
**	Revision 1.4  2001/07/06 20:47:59  singhki
**	use increment guard to make sure increment level is always updated
**	
**	Revision 1.3  2001/07/05 14:19:16  shahia
**	keep linux compiler happy, removed warnings
**	
**	Revision 1.2  2001/07/05 13:47:53  shahia
**	remove typename left from templated version attempt
**	
**	Revision 1.1  2001/07/05 13:20:45  shahia
**	Generalized SecDb trace func and moved it to secdb
**	
**
****************************************************************/

#define     BUILDING_SECDB

#include	<portable.h>

#include	<secdb/SecDbTraceStack.h>
#include	<secdb/SecDbGenericTraceFunc.h>

#include	<kool_ade.h>
#include	<hash.h>
#include	<secdb.h>
#include	<secexpr.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secdrv.h>

#include    <algorithm>
#include    <ccstl.h>

#include    <sstream>
CC_USING( std::ostringstream );

#include    <string>
CC_USING( std::string );

#include	<stdio.h>
#include	<stdarg.h>
#include	<stdlib.h>
#include	<string.h>

static SecDbTraceStack	*pSecDbTraceStack = 0;

static HASH
		*TraceBreakpoints = NULL,
		*TraceBreakExcludes = NULL;

static TRACE_BREAK *TraceBreakMatch(
	int			Message,
	SDB_NODE	*Node,
	SDB_OBJECT	*Object = NULL
);

static string to_string( DT_VALUE* val )
{
	if( !val )
		return "<null>";
	dt_value_var str_val;
	str_val->DataType = DtString;
	if( !DTM_TO( str_val.get(), val ) )
	{
		DTM_INIT( str_val.get() );		// who knows what junk is in str_val..
		string res = "<unknown ";
		res += val->DataType->Name;
		res += ">";
		return res;
	}
	return static_cast<char const*>( str_val->Data.Pointer );
}


/****************************************************************
**	Routine: TraceBreakHash
**	Returns: Hash key
**	Action : hashes a trace break
****************************************************************/

static DT_HASH_CODE TraceBreakHash(
	const void	*Key
)
{
	TRACE_BREAK
			*Breakpoint = (TRACE_BREAK *) Key; // FIX- shouldn't cast away const

	DT_HASH_CODE
			Hash;

	SDB_NODE
			**Args = Breakpoint->m_Args;
	Breakpoint->m_Args = NULL;

	Hash = DtHashQuick( (unsigned char *) Breakpoint, sizeof( *Breakpoint ), 0 );
	if( Args )
	{
		Hash = DtHashQuick( (unsigned char *) Args, sizeof( *Args ) * Breakpoint->m_Argc, Hash );
		Breakpoint->m_Args = Args;
	}
	return Hash;
}



/****************************************************************
**	Routine: TraceBreakCompare
**	Returns: TRUE/FALSE
**	Action : compares two trace breakpoints
****************************************************************/

static int TraceBreakCompare(
	const void *Key1,
	const void *Key2
)
{
	const TRACE_BREAK
			*Break1 = (const TRACE_BREAK *) Key1,
			*Break2 = (const TRACE_BREAK *) Key2;

	return !( Break1->m_Object == Break2->m_Object && Break1->m_ValueType == Break2->m_ValueType
			  && Break1->m_Message == Break2->m_Message && Break1->m_Argc == Break2->m_Argc
			  && memcmp( Break1->m_Args, Break2->m_Args, Break1->m_Argc * sizeof( *Break1->m_Args )) == 0 );
}



/****************************************************************
**	Routine: TraceBreakMatchInternal
**	Returns: TRACE_BREAK * / NULL
**	Action : returns a trace breakpoint which matches the node
**			 NULL if none found
****************************************************************/

static TRACE_BREAK *TraceBreakMatchInternal(
	int			Message,
	SDB_NODE	*Node,
	SDB_OBJECT	*Object,
	HASH*		TraceBreakpoints
)
{
	TRACE_BREAK
			Break( Node, Message, Object ),
			BreakNoMessage( Node, -1, Object ),
			*Ptr;

	if( !TraceBreakpoints || !TraceBreakpoints->KeyCount ) // quick check
		return NULL;

	if(( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &Break ))
	   || ( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &BreakNoMessage )))
		return Ptr;

	// Look for breakpoint without the args
	if( Break.m_Args )
	{
		BreakNoMessage.m_Args = Break.m_Args = NULL;
		BreakNoMessage.m_Argc = Break.m_Argc = 0;		
		if(( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &Break ))
		   || ( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &BreakNoMessage )))
			return Ptr;
	}

	// squirell away value type to use again
	SDB_NODE
			*ValueType = Break.m_ValueType;

	// Look for breakpoint only on the object
	BreakNoMessage.m_ValueType = Break.m_ValueType = NULL;
	if(( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &Break ))
	   || ( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &BreakNoMessage )))
		return Ptr;

	// Look for a breakpoint only on the VT
	BreakNoMessage.m_Object = Break.m_Object = NULL;
	BreakNoMessage.m_ValueType = Break.m_ValueType = ValueType;
	if(( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &Break ))
	   || ( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &BreakNoMessage )))
		return Ptr;

	// Look for a breakpoint only on the message
	BreakNoMessage.m_ValueType = Break.m_ValueType = NULL;
	if(( Ptr = (TRACE_BREAK *) HashLookup( TraceBreakpoints, &Break )))
		return Ptr;

	return NULL;
}



/****************************************************************
**	Routine: TraceBreakMatch
**	Returns: TRACE_BREAK* / NULL
**	Action : returns a trace breakpoint which matches the node
****************************************************************/

static TRACE_BREAK* TraceBreakMatch(
	int			Message,
	SDB_NODE	*Node,
	SDB_OBJECT	*Object
)
{
	TRACE_BREAK* break_point = TraceBreakMatchInternal( Message, Node, Object, TraceBreakpoints );
	return !break_point || TraceBreakMatchInternal( Message, Node, Object, TraceBreakExcludes ) ? 0 : break_point;
}



/****************************************************************
**	Routine: SecDbTraceBreak
**	Returns: TRUE or FALSE
**	Action : Set or remove breakpoint on:
**				- Any node on given value type,
**				- Any node on given object, or,
**				- Any node on both value type and object
****************************************************************/

int SecDbTraceBreak(
	SDB_OBJECT		*Object,
	DT_ARRAY		*VtApply,
	int				Message,
	int				On,
	bool			exclude				// if we should exclude nodes which match this breakpoint
)
{
	DT_VALUE
			TempVal;

	TRACE_BREAK
			*Breakpoint;


	if( !TraceBreakpoints )
	{
		TraceBreakpoints = HashCreate( "TraceBreakpoints", TraceBreakHash, TraceBreakCompare, 0, NULL );
		if( !TraceBreakpoints )
			return( FALSE );
	}
	if( !TraceBreakExcludes )
	{
		TraceBreakExcludes = HashCreate( "TraceBreakExcludes", TraceBreakHash, TraceBreakCompare, 0, NULL );
		if( !TraceBreakExcludes )
			return( FALSE );
	}

	Breakpoint 			  = new TRACE_BREAK;
	Breakpoint->m_Message = Message;
	if( Object )
	{
		TempVal.DataType		= DtSecurity;
		TempVal.Data.Pointer	= Object;
		if( !( Breakpoint->m_Object = SDB_NODE::FindLiteral( &TempVal, SecDbRootDb )))
			return FALSE;
	}
		
	if( VtApply && VtApply->Size > 0 )
	{
		if( !( Breakpoint->m_ValueType = SDB_NODE::FindLiteral( &VtApply->Element[ 0 ], SecDbRootDb )))
			return FALSE;

		if( VtApply->Size > 1 )
		{
			Breakpoint->m_Argc = VtApply->Size - 1;
			Breakpoint->m_Args = new SDB_NODE *[ VtApply->Size - 1 ];

			for( int Arg = 0; Arg < VtApply->Size - 1; ++Arg )
				if( !( Breakpoint->m_Args[ Arg ] = SDB_NODE::FindLiteral( &VtApply->Element[ Arg + 1 ], SecDbRootDb )))
					return FALSE;
		}
	}

	HASH* hash = exclude ? TraceBreakExcludes : TraceBreakpoints;
	if( On )
		HashInsert( hash, Breakpoint, Breakpoint );
	else
	{
		TRACE_BREAK
				*PrevBreakpoint = (TRACE_BREAK *) HashLookup( hash, Breakpoint );
		HashDelete( hash, Breakpoint );

		if( PrevBreakpoint )
		{
			delete PrevBreakpoint->m_Args;
			delete PrevBreakpoint;
		}
		delete Breakpoint->m_Args;
		delete Breakpoint;
	}
	return( TRUE );
}


/****************************************************************
**	Routine: VectorAddressString
**	Returns: TRUE
**	Action : 
****************************************************************/

static int VectorAddressString(
	char	*Buffer
)
{
	if( SDB_NODE::VectorContext() && SDB_NODE::VectorContext()->InUse )
		sprintf( Buffer, "{%d}", SDB_NODE::VectorContext()->Index );
	else
		*Buffer = '\0';
	return TRUE;
}




/****************************************************************
**	Routine: SecDbSetTraceStack
**	Returns: 
**	Action : 
**			 
****************************************************************/

SecDbTraceStack* SecDbSetTraceStack( SecDbTraceStack	*pTraceStack )
{
	SecDbTraceStack	*pCurrTraceStack = pSecDbTraceStack;
	pSecDbTraceStack = pTraceStack;
	return pCurrTraceStack;
}

/****************************************************************
**	Routine: SecDbGetTraceStack
**	Returns: 
**	Action : 
****************************************************************/

SecDbTraceStack* SecDbGetTraceStack( )
{
	return pSecDbTraceStack;
}




/****************************************************************
**	Routine: SecDbGenericTraceFunc
**	Returns: TRUE
**	Action : Generic trace function, template arg represents SecDbTraceStack
**			 
****************************************************************/

int SecDbGenericTraceFunc(
	SDB_TRACE_MSG	Msg,
	...
)
{
	va_list	Marker;

	// va_list Marker;
	
	SDB_NODE
			*Node,
			*Parent;
			
	SDB_OBJECT
			*SecPtr;

	SDB_VALUE_TYPE_INFO
			*Vti;

	SecDbTraceStack::value_type
			TrCtx;

	struct inc_guard
	{
		inc_guard() : m_inc( false ) {}
		~inc_guard()
		{
			SecDbTraceStack::value_type ctx = pSecDbTraceStack->Top();
			if( m_inc && ctx )
				ctx->SetLevel( ctx->GetLevel() + 1 );
		}

		void operator=( bool val ) { m_inc = val; }

	private:
		bool m_inc;

	} Increment;

	int		Decrement		= FALSE,
			ObjMsg;
			

	char	
			TimeBuf[26],
			Buffer[ 512 ],
			UnkMsg[ 64 ],
			VecAddr[ 64 ],
			VtiMsg[ SDB_CLASS_NAME_SIZE + SDB_VALUE_NAME_SIZE + 5 ],
			DbMsg[ SDB_FULL_SEC_NAME_SIZE + 20 + 250 ];

	const char
			*Tag			= NULL;

	string
			NodeMsg;

	void	*DisplayPtr		= NULL,
			*DisplayPtr2	= NULL;

	SDB_DB	*Db;

	TRACE_BREAK
			*Breakpoint		= NULL;
	


#define NODEMSG( Text_, Value_ )				\
	do {										\
		NodeMsg = Node->GetText( (Value_) );	\
		Tag = (Text_);							\
		DisplayPtr = Node;						\
		VectorAddressString( VecAddr );			\
	}											\
	while( 0 )

#define NODEMSGDB( Text_, Value_ )																				\
	do																											\
	{																											\
		NodeMsg = Node->GetText( (Value_) );																	\
		sprintf( DbMsg, "%s: %s ", (Text_), ( Db = Node->DbSetGet()->BaseDb() ) ? Db->FullDbName.c_str() : "(null)" );	\
		Tag = DbMsg;																							\
		DisplayPtr = Node;																						\
		VectorAddressString( VecAddr );																			\
	}																											\
	while( 0 )

#define SECPTRMSG( Text_ )								\
	do													\
	{													\
		NodeMsg = SecPtr ? SecPtr->SecData.Name : "";	\
		Tag = (Text_);									\
		DisplayPtr2 = SecPtr;							\
	}													\
	while( 0 )

#define OBJECTMSG( Msg_ )			\
	case SDB_MSG_##Msg_: SECPTRMSG( #Msg_ " Object" ); break

#define VTIMSG( Text_ )																															\
	do																																			\
	{																																			\
		sprintf( VtiMsg, "%s( %s )", Vti && Vti->ValueType ? Vti->ValueType->Name : "", SecPtr && SecPtr->Class ? SecPtr->Class->Name : "" );	\
		NodeMsg = VtiMsg;																														\
		Tag = (Text_);																															\
		DisplayPtr2 = NULL;																														\
	}																																			\
	while( 0 )

#define	VALUEMSG( Msg_ )			\
	case SDB_MSG_##Msg_: VTIMSG( #Msg_ " Value" ); break

	if( pSecDbTraceStack->IsFull() &&  ( SDB_TRACE_END != Msg ) )
	{
		return( TRUE );
	}

	TrCtx = pSecDbTraceStack->Top();		

	// don't crash if the state machine is out of sync
	if ( ( SDB_TRACE_START != Msg ) && !TrCtx )
		return TRUE;

/*
**	Check for ESC key occasionally
*/

	if( pSecDbTraceStack->size() && SDB_TRACE_END != Msg )
	{
		if( TrCtx->GetAborted() )
			return( FALSE );
		
		TrCtx->CheckForEsc();

		if( TrCtx->GetSuspended() )
			return( TRUE );
	}
	

	*VecAddr = '\0';
	switch( Msg )
	{
	
		// Start/End messages, never sent by SecDb; provided as user messages
		case SDB_TRACE_START:
			if ( pSecDbTraceStack->IsFull() )
				return( TRUE );
			TrCtx = pSecDbTraceStack->PushNewContext();

			TrCtx->Init(	
					1,					// Level,
					FALSE,				// Aborted,
					FALSE,				// Suspended,
					pSecDbTraceStack->GetVerbose(),		// Verbose,
					pSecDbTraceStack->GetRefresh(),		// TraceRefresh
					pSecDbTraceStack->GetStep(),	// Step,
					Now()				// StartTime
					);

			
			TrCtx->LastAbortCheck		= time( NULL );

			sprintf( Buffer, "SecViewTrace started %s", ( gs_ctime_r( &TrCtx->LastAbortCheck, TimeBuf ) ? TimeBuf : "<bad time>" ) );
			TrCtx->AppendText( Buffer );

			return( TRUE );

		case SDB_TRACE_END:
		{
			TrCtx->End();
			pSecDbTraceStack->PopContext();
			TrCtx = pSecDbTraceStack->Top();
			if( TrCtx && TrCtx->GetLevel() <= TrCtx->GetStep() )
			{
				return TrCtx->OnBreak() ;
			}
			return( TRUE );
		}

		
		// Error messages
		case SDB_TRACE_ERROR_NODE_CTX:
		case SDB_TRACE_ERROR_NODE:
		case SDB_TRACE_ERROR_OBJECT:
		case SDB_TRACE_ERROR_VALUE:
		case SDB_TRACE_ERROR:

			TrCtx->AppendText( "{\n" );
			TrCtx->AppendText( ErrGetString() );
			TrCtx->AppendText( "}\n" );

			Tag = "Error reported to trace";
			Breakpoint = pSecDbTraceStack->GetStopOnErr() || ( ErrNum >= SDB_NODE_FIRST_FATAL_ERROR && ErrNum <= SDB_NODE_LAST_FATAL_ERROR ) ? pSecDbTraceStack->GetTraceBreakErr() : NULL;
			break;

		// Compiled messages:
		case SDB_TRACE_COMPILED_CALC:
		{
			va_start( Marker, Msg );
			SDB_VALUE_TYPE_INFO* vti = va_arg( Marker, SDB_VALUE_TYPE_INFO * );
			DT_VALUE* val = va_arg( Marker, DT_VALUE* );
			va_end( Marker );

			NodeMsg = vti->ValueType->Name;
			NodeMsg += " = ";
			NodeMsg += to_string( val );
			if( NodeMsg.size() > 200 )
				NodeMsg.replace( 200, NodeMsg.size() - 200, "..." );

			Tag = "Calc'd";
			DisplayPtr2 = val;
			break;
		}
		case SDB_TRACE_COMPILED_GET_CHILD:
		{
			va_start( Marker, Msg );
			char const* child_str = va_arg( Marker, char const* );
			DT_VALUE* val = va_arg( Marker, DT_VALUE* );
			va_end( Marker );

			NodeMsg = child_str ? child_str : "";
			NodeMsg += " = ";
			NodeMsg += to_string( val );
			if( NodeMsg.size() > 200 )
				NodeMsg.replace( 200, NodeMsg.size() - 200, "..." );

			Tag = "GetCompiled";
			DisplayPtr2 = val;
			break;
		}
			
		// GetValue messages: (*TraceFunc)( Msg, Node, Ctx )
		case SDB_TRACE_VALID:
		case SDB_TRACE_CALC:
		case SDB_TRACE_EVAL_END:
		case SDB_TRACE_DONE_BUILD_CHILDREN:
		
		// Diddle messages: (*TraceFunc)( Msg, Node )
		case SDB_TRACE_SET_DIDDLE:
		case SDB_TRACE_CHANGE_DIDDLE:
		case SDB_TRACE_REMOVE_DIDDLE:
		case SDB_TRACE_PHANTOM_DIDDLE:
		case SDB_TRACE_BURIED_DIDDLE:
		case SDB_TRACE_DONE_DIDDLE:

		// SetValue message: (*TraceFunc)( Msg, Node )
		case SDB_TRACE_SET:
		case SDB_TRACE_DONE_SET:
	
		// ValidLink message: (*TraceFunc)( Msg, Node )
		case SDB_TRACE_VALID_LINK:
		case SDB_TRACE_DONE_VALID_LINK:

		// Invalidation messages: (*TraceFunc)( Msg, Node )
		case SDB_TRACE_INVALIDATE:
		case SDB_TRACE_DONE_INVALIDATE:
		case SDB_TRACE_INVALIDATE_CHILDREN:
		case SDB_TRACE_DONE_INVALIDATE_CHILDREN:
	
		// Low level stuff: (*TraceFunc)( Msg, Node )
		case SDB_TRACE_NEW:
		case SDB_TRACE_DONE_NEW:
		case SDB_TRACE_DESTROY:
		case SDB_TRACE_DESTROY_AFFECT:
		case SDB_TRACE_DESTROY_DELETE:
		case SDB_TRACE_DESTROY_UNDO:

		// Async messages
		case SDB_TRACE_SCHEDULE_ASYNC_GET:
		case SDB_TRACE_APPLY_ASYNC_RESULT:

		// NodeSplit by child: (*TraceFunc)( Msg, Node )
		case SDB_TRACE_NODE_SPLIT_CHILD:

			va_start( Marker, Msg );
			Node = va_arg( Marker, SDB_NODE * );
			va_end( Marker );
			
			Breakpoint = TraceBreakMatch( Msg, Node );
			
			switch( Msg )
			{
				case SDB_TRACE_VALID:
					NODEMSG( "Valid ", TRUE );
					Decrement = TRUE;
					break;

				case SDB_TRACE_CALC:
					NODEMSG( "Calc'd", TRUE );
					Decrement = TRUE;
					break;

				case SDB_TRACE_EVAL_END:
					Decrement = TRUE;
					break;

				case SDB_TRACE_SET_DIDDLE:		// SecDbSetDiddle called
					NODEMSG( "SetDiddle", FALSE );
					Increment = TRUE;
					break;

				case SDB_TRACE_CHANGE_DIDDLE:	// SecDbChangeDiddle called
					NODEMSG( "ChangeDiddle", FALSE );
					Increment = TRUE;
					break;

				case SDB_TRACE_REMOVE_DIDDLE:	// SecDbRemoveDiddle called
					NODEMSG( "RemoveDiddle", FALSE );
					Increment = TRUE;
					break;

				case SDB_TRACE_PHANTOM_DIDDLE:	// Set or changed diddled is a phantom
					Increment = TRUE;
					Decrement = TRUE;
					NODEMSG( "Phantom", FALSE );
					break;

				case SDB_TRACE_BURIED_DIDDLE:	// Removed diddle was not at the top
					Increment = TRUE;
					Decrement = TRUE;
					NODEMSG( "Buried", FALSE );
					break;

				case SDB_TRACE_SET:				// SecDbSetValue called
					Increment = TRUE;
					NODEMSG( "SetVal", FALSE );
					break;

				case SDB_TRACE_INVALIDATE:		// NodeInvalidate called (only Node->Cache filled in)
					Increment = TRUE;
					NODEMSG( "Invalidate", TRUE );
					break;

				case SDB_TRACE_INVALIDATE_CHILDREN:
					Increment = TRUE;
					NODEMSG( "InvalidateChildren", TRUE );
					break;

				case SDB_TRACE_NEW:
					if( TrCtx->GetVerbose() )
					{
						NODEMSG( "Alloc ", FALSE );
						Increment = TRUE;
					}
					break;

				case SDB_TRACE_DESTROY_AFFECT:
					if( TrCtx->GetVerbose() )
						NODEMSGDB( "DestroyAffect ", FALSE );
					break;

				case SDB_TRACE_DESTROY_DELETE:
					if( TrCtx->GetVerbose() )
						NODEMSGDB( "DestroyDelete ", FALSE );
					break;

				case SDB_TRACE_DESTROY_UNDO:
					if( TrCtx->GetVerbose() )
						NODEMSGDB( "DestroyUndo ", FALSE );
					break;

				case SDB_TRACE_DESTROY:
					if( TrCtx->GetVerbose() )
						NODEMSGDB( "Destroy ", FALSE );
					break;

				case SDB_TRACE_VALID_LINK:
					if( TrCtx->GetVerbose() )
					{
						NODEMSG( "ValidLink", FALSE );
						Increment = TRUE;
					}
					break;

				case SDB_TRACE_DONE_BUILD_CHILDREN:
				case SDB_TRACE_DONE_NEW:
				case SDB_TRACE_DONE_VALID_LINK:
					if( TrCtx->GetVerbose() )
						Decrement = TRUE;
					break;

				case SDB_TRACE_DONE_SET:
				case SDB_TRACE_DONE_DIDDLE:
				case SDB_TRACE_DONE_INVALIDATE:
				case SDB_TRACE_DONE_INVALIDATE_CHILDREN:
					Decrement = TRUE;
					break;

				case SDB_TRACE_NODE_SPLIT_CHILD:
					if( TrCtx->GetVerbose() )
					{
						NODEMSG( "NodeSplitByChild ", FALSE );
					}
					break;

				case SDB_TRACE_SCHEDULE_ASYNC_GET:
					NODEMSG( "AsyncGet ", FALSE );
					break;

				case SDB_TRACE_APPLY_ASYNC_RESULT:
					NODEMSG( "AsyncResult ", TRUE );
					break;

				default:
					break;
			}
			break;

		// Marking a node for destroy: (*TraceFunc)( Msg, NodeToDestroy )
			va_start( Marker, Msg );
			Node = va_arg( Marker, SDB_NODE * );
			if( TrCtx->GetVerbose() )
				NODEMSG( "MarkDestroy ", FALSE );
			break;

		// GetValue messages: (*TraceFunc)( Msg, Node, Ctx, Db )
		case SDB_TRACE_BUILD_CHILDREN:
			if( !TrCtx->GetVerbose() )
				break;

		case SDB_TRACE_GET:
		{
			SDB_DB	*Db;

			va_start( Marker, Msg );
			Node = va_arg( Marker, SDB_NODE * );
			(void) va_arg( Marker, SDB_NODE_CONTEXT * );
			Db = va_arg( Marker, SDB_DB * );
			va_end( Marker );

			Breakpoint = TraceBreakMatch( Msg, Node );

			switch( Msg )
			{
				case SDB_TRACE_BUILD_CHILDREN:
					sprintf( DbMsg, "BuildChildren( %s )", Db->FullDbName.c_str() );
					Increment = TRUE;
					break;

				case SDB_TRACE_GET:
					sprintf( DbMsg, "GetVal( %s )", Db->FullDbName.c_str() );
					Increment = TRUE;
					break;

				default:
					break;				// handle all enums to appease gcc
			}
			Tag = DbMsg;
			NODEMSG( Tag, FALSE );
			break;
		}

		// Diddle color stuff: (*TraceFunc)( Msg, Node, Db )
		case SDB_TRACE_NODE_SPLIT:
		case SDB_TRACE_NODE_SPLIT_EXTEND:
		case SDB_TRACE_NODE_SPLIT_DONE:
		case SDB_TRACE_NODE_REMOVE_DB:
		case SDB_TRACE_NODE_SDB_DBSET_EXTEND:
		{
			SDB_DB	*Db;

			if( !TrCtx->GetVerbose() )
				break;

			va_start( Marker, Msg );
			Node = va_arg( Marker, SDB_NODE * );
			Db = va_arg( Marker, SDB_DB * );
			va_end( Marker );

			Breakpoint = TraceBreakMatch( Msg, Node );

			switch( Msg )
			{
				case SDB_TRACE_NODE_SPLIT_EXTEND:
					sprintf( DbMsg, "NodeSplitExtend( %s )", Db->FullDbName.c_str() );
					break;

				case SDB_TRACE_NODE_SPLIT_DONE:
					sprintf( DbMsg, "NodeSplitDone( %s )", Db->FullDbName.c_str() );
					Decrement = TRUE;
					break;

				case SDB_TRACE_NODE_SPLIT:
					sprintf( DbMsg, "NodeSplitChildren( %s )", Db->FullDbName.c_str() );
					Increment = TRUE;
					break;

				case SDB_TRACE_NODE_REMOVE_DB:
					sprintf( DbMsg, "DbSetRemove( %s )", Db->FullDbName.c_str() );
					break;

				case SDB_TRACE_NODE_SDB_DBSET_EXTEND:
					sprintf( DbMsg, "DbSetExtend( %s )", Db->FullDbName.c_str() );
					break;

				default:				// handle all enums to appease compiler
					break;
			}
			Tag = DbMsg;
			NODEMSG( Tag, FALSE );
			break;
		}

		case SDB_TRACE_DESTROY_DIDDLE_SCOPE:
		{
			SDB_DB	*Db;

			if( !TrCtx->GetVerbose() )
				break;

			va_start( Marker, Msg );
			Db = va_arg( Marker, SDB_DB * );
			va_end( Marker );
			
			sprintf( DbMsg, "DestroyDiddleScope %s", Db->FullDbName.c_str() );
			Tag = DbMsg;
			break;
		}

		case SDB_TRACE_TRANSFORM_DBSET:
		{
			if( !TrCtx->GetVerbose() )
				break;

			va_start( Marker, Msg );
			SDB_DBSET *dbset_old = va_arg( Marker, SDB_DBSET * );
			SDB_DBSET *dbset_new = va_arg( Marker, SDB_DBSET * );
			va_end( Marker );
			
			ostringstream str;
			str << "Transform " << dbset_old->toString() << " -> " << dbset_new->toString();
			sprintf( DbMsg, "%.*s", (int)sizeof( DbMsg ) - 1, str.str().c_str() );
			Tag = DbMsg;
			break;
		}

		case SDB_TRACE_STR_TO_SEC_CONVERSION:
		case SDB_TRACE_STR_TO_SEC_USED:
		{
			if( !TrCtx->GetVerbose() )
				break;

			va_start( Marker, Msg );
			Node = va_arg( Marker, SDB_NODE * );
			Parent = va_arg( Marker, SDB_NODE * );
			va_end( Marker );

			Breakpoint = TraceBreakMatch( Msg, Node );
			DisplayPtr2 = Parent;

			switch( Msg )
			{
				case SDB_TRACE_STR_TO_SEC_CONVERSION:
			    	sprintf( DbMsg, "StringToSecurityConversion by Parent:" );
					break;

				case SDB_TRACE_STR_TO_SEC_USED:
			    	sprintf( DbMsg, "StringToSecurityUsed:" );
					break;
				default:
					break;
			}

			Tag = DbMsg;
			NODEMSG( Tag, FALSE );
			break;
		}

		case SDB_TRACE_NUM_INV_PARENTS_DECREMENT:
		case SDB_TRACE_NUM_INV_PARENTS_INCREMENT:
		case SDB_TRACE_NUM_INV_PARENTS_SET:
		case SDB_TRACE_RELEASE_VALUE:
		case SDB_TRACE_RELEASE_DESCENDENT_VALUES:
		case SDB_TRACE_DONE_RELEASE_DESCENDENT_VALUES:
		{
			int Num = 0;				// dummy initializer to appease compiler

			if( !TrCtx->GetVerbose() )
				break;

			va_start( Marker, Msg );
			Node = va_arg( Marker, SDB_NODE * );
			Parent = va_arg( Marker, SDB_NODE * );
			if( Msg != SDB_TRACE_RELEASE_VALUE )
				Num = va_arg( Marker, int );
			va_end( Marker );

			Breakpoint = TraceBreakMatch( Msg, Node );
			DisplayPtr2 = Parent;

			switch( Msg )
			{
				case SDB_TRACE_NUM_INV_PARENTS_SET:
					sprintf( DbMsg, "NumInvParentsSet( %d )", Num );
					break;

				case SDB_TRACE_NUM_INV_PARENTS_INCREMENT:
					sprintf( DbMsg, "NumInvParentsInc( %d )", Num );
					break;

				case SDB_TRACE_NUM_INV_PARENTS_DECREMENT:
					sprintf( DbMsg, "NumInvParentsDec( %d )", Num );
					break;

				case SDB_TRACE_RELEASE_VALUE:
					sprintf( DbMsg, "ReleaseValue" );
					break;

				case SDB_TRACE_RELEASE_DESCENDENT_VALUES:
					sprintf( DbMsg, "ReleaseDescendentValues" );
					break;

				case SDB_TRACE_DONE_RELEASE_DESCENDENT_VALUES:
					sprintf( DbMsg, "ReleaseDescendentValuesDone" );
					break;

				default:				// handle all enums to appease compiler
					break;
			}
			Tag = DbMsg;
			NODEMSG( Tag, FALSE );
			break;
		}

		// Linking: (*TraceFunc)( Msg, Child, Parent )		
		case SDB_TRACE_DIDDLE_STACK_MOVE:
		case SDB_TRACE_PLACE_NOT_MY_DIDDLE:
		case SDB_TRACE_LINK:
		case SDB_TRACE_UNLINK:
			if( TrCtx->GetVerbose() )
			{
				va_start( Marker, Msg );
				Node 	= va_arg( Marker, SDB_NODE * );
				Parent	= va_arg( Marker, SDB_NODE * );
				va_end( Marker );
			
				Breakpoint = TraceBreakMatch( Msg, Node );

				switch( Msg )
				{
					case SDB_TRACE_LINK:
						NODEMSG( "Link ", FALSE );
						DisplayPtr2 = Parent;
						break;

					case SDB_TRACE_UNLINK:
						NODEMSG( "Unlink ", FALSE );
						DisplayPtr2 = Parent;
						break;

					case SDB_TRACE_DIDDLE_STACK_MOVE:
						NODEMSG( "DiddleStackMoveFrom ", FALSE );
						DisplayPtr2 = Parent;
						break;

					case SDB_TRACE_PLACE_NOT_MY_DIDDLE:
						NODEMSG( "PlaceNotMyDiddleFrom ", FALSE );
						DisplayPtr2 = Parent;
						break;

					default:
						break;
				}
			}
			break;
			
		// Object messages
		case SDB_TRACE_DESTROY_CACHE:
			va_start( Marker, Msg );
			SecPtr = va_arg( Marker, SDB_OBJECT * );
			va_end( Marker );
			
			Increment = TRUE;
			SECPTRMSG( "DestroyCache" );
			break;
			
		case SDB_TRACE_GET_OBJECT:
		case SDB_TRACE_FREE_OBJECT:
			if( TrCtx->GetVerbose() )
			{
				va_start( Marker, Msg );
				SecPtr = va_arg( Marker, SDB_OBJECT * );
				va_end( Marker );
			
				if( SecPtr )
				{
					Breakpoint = TraceBreakMatch( Msg, NULL, SecPtr );

					NodeMsg = SecPtr->SecData.Name;
				}
				else
					NodeMsg = "";

				if( SDB_TRACE_GET_OBJECT == Msg )
					sprintf( UnkMsg, "GET Object (to: %d)", SecPtr->ReferenceCount );
				else
					sprintf( UnkMsg, "FREE Object (from: %d)", SecPtr->ReferenceCount );
				Tag = UnkMsg;
				DisplayPtr2 = SecPtr;
			}
			break;
			
		case SDB_TRACE_OBJECT_MSG:
			va_start( Marker, Msg );
			SecPtr	= va_arg( Marker, SDB_OBJECT * );
			ObjMsg	= va_arg( Marker, int );
			va_end( Marker );
			
			Increment = TRUE;
			switch( ObjMsg )
			{
				case SDB_MSG_START:
					NodeMsg = SecPtr ? SecPtr->Class->Name : "";
					Tag = "START Class";
					DisplayPtr2 = SecPtr ? SecPtr->Class : NULL;
					break;
				
				OBJECTMSG( END						);
				OBJECTMSG( LOAD						);
				OBJECTMSG( UNLOAD					);
				OBJECTMSG( NEW						);
				OBJECTMSG( ADD						);
				OBJECTMSG( UPDATE					);
				OBJECTMSG( DELETE					);
				OBJECTMSG( VALIDATE					);
				OBJECTMSG( GET_VALUE				);
				OBJECTMSG( SET_VALUE				);
				OBJECTMSG( GET_CHILD_LIST			);
				OBJECTMSG( DUMP_OBJ_DATA			);
				OBJECTMSG( DIDDLE_SET				);
				OBJECTMSG( DIDDLE_CHANGED			);
				OBJECTMSG( DIDDLE_REMOVED			);
				OBJECTMSG( RENAME					);
				OBJECTMSG( LOAD_VERSION				);
				OBJECTMSG( DESCRIPTION				);
				OBJECTMSG( MEM						);
				OBJECTMSG( INCREMENTAL				);
				OBJECTMSG( INVALIDATE				);
				OBJECTMSG( MEM_USED					);
				OBJECTMSG( VTI_FREE					);
				OBJECTMSG( VTI_INHERIT				);
				OBJECTMSG( ROLL_BACK_ADD			);
				OBJECTMSG( ROLL_BACK_UPDATE			);
				OBJECTMSG( ROLL_BACK_DELETE			);
				OBJECTMSG( ROLL_BACK_RENAME			);
				OBJECTMSG( ROLL_BACK_INCREMENTAL	);
				
				default:
					NodeMsg = SecPtr ? SecPtr->SecData.Name : "";
					sprintf( UnkMsg, "<ObjectMsg#%04x>", ObjMsg );
					Tag = UnkMsg;
					DisplayPtr2 = SecPtr;
					break;
			}
			break;

		case SDB_TRACE_DONE_DESTROY_CACHE:		// NodeDestroyCache done
		case SDB_TRACE_DONE_OBJECT_MSG:
			Decrement = TRUE;
			break;

		case SDB_TRACE_VALUE_MSG:
			if( TrCtx->GetVerbose() )
			{
				va_start( Marker, Msg );
				SecPtr	= va_arg( Marker, SDB_OBJECT * );
				ObjMsg	= va_arg( Marker, int );
				(void) va_arg( Marker, SDB_M_DATA * );
				Vti		= va_arg( Marker, SDB_VALUE_TYPE_INFO * );
				va_end( Marker );

				Increment = TRUE;
				switch( ObjMsg )
				{
					VALUEMSG( START					);
					VALUEMSG( END					);
					VALUEMSG( LOAD					);
					VALUEMSG( UNLOAD				);
					VALUEMSG( NEW					);
					VALUEMSG( ADD					);
					VALUEMSG( UPDATE				);
					VALUEMSG( DELETE				);
					VALUEMSG( VALIDATE				);
					VALUEMSG( GET_VALUE				);
					VALUEMSG( SET_VALUE				);
					VALUEMSG( GET_CHILD_LIST		);
					VALUEMSG( DUMP_OBJ_DATA			);
					VALUEMSG( DIDDLE_SET			);
					VALUEMSG( DIDDLE_CHANGED		);
					VALUEMSG( DIDDLE_REMOVED		);
					VALUEMSG( RENAME				);
					VALUEMSG( LOAD_VERSION			);
					VALUEMSG( DESCRIPTION			);
					VALUEMSG( MEM					);
					VALUEMSG( INCREMENTAL			);
					VALUEMSG( INVALIDATE			);
					VALUEMSG( MEM_USED				);
					VALUEMSG( VTI_FREE				);
					VALUEMSG( VTI_INHERIT			);
					VALUEMSG( ROLL_BACK_ADD			);
					VALUEMSG( ROLL_BACK_UPDATE		);
					VALUEMSG( ROLL_BACK_DELETE		);
					VALUEMSG( ROLL_BACK_RENAME		);
					VALUEMSG( ROLL_BACK_INCREMENTAL	);

					default:
						sprintf( UnkMsg, "<ValueMsg#%d>", ObjMsg );
						VTIMSG( UnkMsg );
						break;
				}
			}
			break;

		case SDB_TRACE_DONE_VALUE_MSG:
			if( TrCtx->GetVerbose() )
				Decrement = TRUE;
			break;

		case SDB_TRACE_DESTROY_START:
			if( TrCtx->GetVerbose() )
			{
				Tag = "DestroyStart";
				Increment = TRUE;
			}
			break;
			
		case SDB_TRACE_DESTROY_END:
			if( TrCtx->GetVerbose() )
			{
				Tag = "DestroyEnd";
				Decrement = TRUE;
			}
			break;

		case SDB_TRACE_DESTROY_AFFECTED_BY:
			if( TrCtx->GetVerbose() )
			{
				va_start( Marker, Msg );
				Node 		= va_arg( Marker, SDB_NODE * );
				DisplayPtr2	= va_arg( Marker, SDB_NODE * );
				va_end( Marker );
			
				NODEMSG( "DestroyAffectedBy ", FALSE );
			}
			break;
			
		default:
			return( TRUE );
	}


	if( Decrement )
			TrCtx->SetLevel ( max( TrCtx->GetLevel() - 1 , 1 ) );

	if( !NodeMsg.empty() || Tag )
	{
		TrCtx->AppendPointerInfo( DisplayPtr );
		if( TrCtx->GetVerbose() )
		{
			sprintf( Buffer, "%6.2f", Now() - TrCtx->GetStartTime() );
			TrCtx->AppendText( Buffer );
		}
		sprintf( Buffer, "[%2d]%*s", TrCtx->GetLevel(), TrCtx->GetLevel() * 2, "" );
		TrCtx->AppendText( Buffer );
		if( *VecAddr )
			TrCtx->AppendText( VecAddr );
		if( Tag )
		{
			TrCtx->AppendText( Tag );
		}
		if( TrCtx->GetVerbose() && DisplayPtr2 )
		{
			sprintf( Buffer, " %p ", DisplayPtr2 );
			TrCtx->AppendText( Buffer );
		}
		if( !NodeMsg.empty() )
		{
			snprintf( Buffer, sizeof( Buffer ), ": %.256s", NodeMsg.c_str() );
			Buffer[ sizeof( Buffer ) - 1 ] = 0;
			TrCtx->AppendText( Buffer );
		}
		TrCtx->AppendText( "\n" );
		if( TrCtx->GetLevel() <= TrCtx->GetStep() || Breakpoint )
		{
			return TrCtx->OnBreak();
		}
	}

	return( TRUE );
}

/****************************************************************
**	Routine: TraceStackGen
**	Returns: void
**	Action : generates a track stack by walking up the tree to
**			 locked parents
****************************************************************/

static void TraceStackGen(
	SDB_NODE		*Node,
	SecDbTraceContext	*TrCtx
)
{
	char	Buffer[ 512 ];

	SDB_NODE
			*LockedParent = NULL;

	for( int i = 0; i < Node->ParentsMax(); ++i )
	{
		SDB_NODE
				*Parent = Node->ParentGet( i );

		if( SDB_NODE::IsNode( Parent ) && Parent->GetValueLockGet() )
		{
			LockedParent = Parent;
			break;
		}
	}

	if( LockedParent )
	{
		TraceStackGen( LockedParent, TrCtx );
		TrCtx->SetLevel( TrCtx->GetLevel() + 1 );
	}
	else
		TrCtx->SetLevel( 1 );

	sprintf( Buffer, "->%-10p ", Node );
	TrCtx->AppendText( Buffer );

	sprintf( Buffer, "[%2d]%*s", TrCtx->GetLevel(), TrCtx->GetLevel() * 2, "" );
	TrCtx->AppendText(  Buffer );
	TrCtx->AppendText( "GetVal: " );
	TrCtx->AppendText( Node->GetText( FALSE ).c_str() );
	TrCtx->AppendText( "\n" );
}




/****************************************************************
**	Routine: TraceOnErr
**	Returns: 
**	Action : 
****************************************************************/

int SecDbGenericTraceOnErrFunc(
	SDB_TRACE_MSG	Msg,
	...
)
{
	va_list	Marker;

	switch( Msg )
	{
		case SDB_TRACE_START:
		case SDB_TRACE_END:
			return SecDbGenericTraceFunc(Msg);

		case SDB_TRACE_ERROR_NODE_CTX:
		case SDB_TRACE_ERROR_NODE:
		{
			SecDbTraceStack::value_type
					TrCtx = 0;

			
			SDB_NODE
					*Node = 0;

			if( pSecDbTraceStack->IsFull() )
			{
				return( TRUE );
			}
			// don't crash if func is used wrongly, i.e no SDB_TRACE_START before tracing
			TrCtx = pSecDbTraceStack->Top();

			if ( ! TrCtx )
				return FALSE;
			
			TrCtx->AppendText( "{\n" );
			TrCtx->AppendText( ErrGetString() );
			TrCtx->AppendText("\n}\n" );

			va_start( Marker, Msg );
			Node = va_arg( Marker, SDB_NODE * );
			va_end( Marker );

			TraceStackGen( Node, TrCtx );
			
			SecDbTraceFunc = SecDbGenericTraceOnErrFunc;

			return TRUE;
		}

		default:
			return TRUE;
	}
}

