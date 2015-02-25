/****************************************************************
**
**	SECTRACE.H	- SecDb run-time trace API
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sectrace.h,v 1.34 2001/11/27 23:23:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECTRACE_H )
#define IN_SECTRACE_H

#if !defined( IN_SDB_BASE_H )
#	include <sdb_base.h>
#endif
#if !defined( IN_SECOBJ_H )
#	include <secobj.h>
#endif

/*
**	Tree tracing
*/


enum SDB_TRACE_MSG
{
	// Start/End messages, never sent by SecDb; provided as user messages
	SDB_TRACE_START,
	SDB_TRACE_END,
	
	// Error messages
	SDB_TRACE_ERROR_NODE_CTX,			// (*TraceFunc)( Msg, Node, Context )
	SDB_TRACE_ERROR_NODE,				// (*TraceFunc)( Msg, Node )
	SDB_TRACE_ERROR_OBJECT,				// (*TraceFunc)( Msg, SecPtr, Msg, MsgData )
	SDB_TRACE_ERROR_VALUE,				// (*TraceFunc)( Msg, SecPtr, Msg, MsgData, Vti )
	SDB_TRACE_ERROR,					// (*TraceFunc)( Msg )
	
	// GetValue messages: (*TraceFunc)( Msg, Node, Ctx )
	SDB_TRACE_GET,						// NodeGetValue called 					( also pass Db )
	SDB_TRACE_VALID,					// Node already valid
	SDB_TRACE_CALC,						// Node calculated
	SDB_TRACE_EVAL_END,					// Node done evaluating itself
	SDB_TRACE_BUILD_CHILDREN,			// Node getting children attached		( also pass Db )
	SDB_TRACE_DONE_BUILD_CHILDREN,		// Node done getting children attached
	
	// Diddle messages: (*TraceFunc)( Msg, Node )
	SDB_TRACE_SET_DIDDLE,				// SecDbSetDiddle called
	SDB_TRACE_CHANGE_DIDDLE,			// SecDbChangeDiddle called
	SDB_TRACE_REMOVE_DIDDLE,			// SecDbRemoveDiddle called
	SDB_TRACE_PHANTOM_DIDDLE,			// Set or changed diddled is a phantom
	SDB_TRACE_BURIED_DIDDLE,			// Removed diddle was not at the top
	SDB_TRACE_DONE_DIDDLE,				// Diddle operation done

	// SetValue message: (*TraceFunc)( Msg, Node )
	SDB_TRACE_SET,						// SecDbSetValue called
	SDB_TRACE_DONE_SET,					// Set value has replaced current value
	
	// Invalidation messages: (*TraceFunc)( Msg, Node )
	SDB_TRACE_INVALIDATE,				// NodeInvalidate called
	SDB_TRACE_DONE_INVALIDATE,			// NodeInvalidate finished
	SDB_TRACE_INVALIDATE_CHILDREN,		// Object or ValueType calculation invalidated
	SDB_TRACE_DONE_INVALIDATE_CHILDREN,	// Childlist invalidation done
	
	// Low level stuff: (*TraceFunc)( Msg, Node )
	SDB_TRACE_NEW,						// New node being created
	SDB_TRACE_DONE_NEW,					// Done with node creation
	SDB_TRACE_DESTROY,					// Node about to be destroyed
	SDB_TRACE_DESTROY_DELETE,			// Node marked for destroy
	SDB_TRACE_DESTROY_AFFECT,			// Node marked as affected by destroy

	// Diddle color stuff: (*TraceFunc)( Msg, Node, Db )
	SDB_TRACE_NODE_REMOVE_DB,			// Removed Db From Node
	SDB_TRACE_NODE_SDB_DBSET_EXTEND,	// Extend DbSet of node with Db

	SDB_TRACE_NODE_SPLIT,				// NodeSplitChildren on Db: Params (Msg, Node, Db )
	SDB_TRACE_NODE_SPLIT_CHILD,			// Node Split because of Child: Params (Msg, Child )
	SDB_TRACE_NODE_SPLIT_EXTEND,		// Node Split finished and extended: Params (Msg, Node )
	SDB_TRACE_NODE_SPLIT_DONE,			// Node split done (some child was split): (Msg, Node)
	
	// Linking: (*TraceFunc)( Msg, Child, Parent )
	SDB_TRACE_LINK,						// Node is the child node; (SDB_NODE *)SecPtr is the parent node
	SDB_TRACE_UNLINK,					// Node is the child node; (SDB_NODE *)SecPtr is the parent node

	// Misc object messages: (*TraceFunc)( Msg, SecPtr, ObjectMsg, SDB_M_DATA * )
	SDB_TRACE_OBJECT_MSG,				// message sent to an object
	SDB_TRACE_DONE_OBJECT_MSG,			// object function return success
	
	// Misc value func messages: (*TraceFunc)( Msg, SecPtr, ObjectMsg, SDB_M_DATA *, Vti )
	SDB_TRACE_VALUE_MSG,				// message sent to an object
	SDB_TRACE_DONE_VALUE_MSG,			// object function return success
	
	// Object related but no msg to object: (*TraceFunc)( Msg, SecPtr )
	SDB_TRACE_DESTROY_CACHE,			// NodeDestroyCache called
	SDB_TRACE_DONE_DESTROY_CACHE,		// NodeDestroyCache done
	SDB_TRACE_GET_OBJECT,				// SecPtr->ReferenceCount++
	SDB_TRACE_FREE_OBJECT,				// SecPtr->ReferenceCount--
	SDB_TRACE_RENAME_OBJECT,			// SecPtr, OldName
	
	// ValidLink message: (*TraceFunc)( Msg, Node )
	SDB_TRACE_VALID_LINK,				// MarkValidLink called
	SDB_TRACE_DONE_VALID_LINK,			// MarkValidLink done

	// ReleaseValue tracing: (*TraceFunc)( Msg, Child, Parent )
	SDB_TRACE_NUM_INV_PARENTS_INCREMENT,
	SDB_TRACE_NUM_INV_PARENTS_DECREMENT,
	SDB_TRACE_NUM_INV_PARENTS_SET,		// ( Msg, Child, Parent, Num )
	SDB_TRACE_RELEASE_VALUE,
	SDB_TRACE_RELEASE_DESCENDENT_VALUES,
	SDB_TRACE_DONE_RELEASE_DESCENDENT_VALUES,

	SDB_TRACE_DESTROY_UNDO,
	SDB_TRACE_DESTROY_START,
	SDB_TRACE_DESTROY_END,
	SDB_TRACE_DESTROY_AFFECTED_BY,		// ( Msg, Node, Child )

	SDB_TRACE_PLACE_NOT_MY_DIDDLE,		// ( Msg, Node, Src )
	SDB_TRACE_DIDDLE_STACK_MOVE,		// ( Msg, Node, Src )

	SDB_TRACE_SCHEDULE_ASYNC_GET,		// ( Msg, Node )
	SDB_TRACE_APPLY_ASYNC_RESULT,		// ( Msg, Node )

	SDB_TRACE_STR_TO_SEC_CONVERSION,    // ( Msg, Child, Parent )
	SDB_TRACE_STR_TO_SEC_USED,    		// ( Msg, Child, Parent )

	SDB_TRACE_DESTROY_DIDDLE_SCOPE,		// ( Msg, Db )
	SDB_TRACE_TRANSFORM_DBSET,			// ( Msg, Old, New )

	SDB_TRACE_COMPILED_GET_CHILD,		// ( Msg, ChildStr, Val )
	SDB_TRACE_COMPILED_CALC,			// ( Msg, Vti, Val )

	SDB_TRACE_LAST_MESSAGE_NUM,

	// Aliases
	SDB_TRACE_ERROR_GET					= SDB_TRACE_ERROR_NODE_CTX,
	SDB_TRACE_ERROR_DIDDLE				= SDB_TRACE_ERROR_NODE,
	SDB_TRACE_ERROR_SET					= SDB_TRACE_ERROR_NODE,
	SDB_TRACE_ERROR_OBJECT_MSG			= SDB_TRACE_ERROR_OBJECT

};

typedef int (*SDB_TRACE_FUNC)( SDB_TRACE_MSG Msg, ... );


/*
**	Global trace function
*/

EXPORT_C_SECDB SDB_TRACE_FUNC
		SecDbTraceFunc,
		SecDbTraceAbortFunc;			// Function used to abort getvalue

EXPORT_C_SECDB int
		SecDbTraceAbortInit;			// Abort initialized?


/*
**	Macros
*/

#define	SDB_MESSAGE( SecPtr_, Msg_, Data_)	( SecDbTraceFunc ? SecDbMessageTrace( (SecPtr_), (Msg_), (Data_) ) : (*(SecPtr_)->Class->FuncPtr)( (SecPtr_), (Msg_), (Data_) ))
#define SDB_MESSAGE_VT_TRACE( SecPtr_, Msg_, Data_, Vti_ )	(SecDbTraceFunc ? SecDbMessageTraceVt( (SecPtr_), (Msg_), (Data_), (Vti_) ) : (*(Vti_)->Func)( (SecPtr_), (Msg_), (Data_), (Vti_) ))


/*
**	Prototypes
*/

EXPORT_C_SECDB int
		SecDbMessageTrace(				SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData ),
		SecDbMessageTraceVt(			SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *Vti ),
		SecDbTraceAbortGet(				SDB_TRACE_MSG Msg, ... );

#endif

