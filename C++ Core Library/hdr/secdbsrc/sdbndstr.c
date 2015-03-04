#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbndstr.c,v 1.55 2013/08/23 16:55:31 khodod Exp $"
// uncomment to get verbose destroy tracing
// #define VERBOSE_DESTROY
/****************************************************************
**
**	sdbndstr.c	- destroy SDB_NODEs and check graph sanity
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbndstr.c,v $
**	Revision 1.55  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.54  2012/05/22 16:15:55  e19351
**	Fixed vc6 break introduced in last commit.
**	AWR: #177555
**
**	Revision 1.53  2012/05/21 16:43:10  e19351
**	Removed some deprecated stuff for old compilers.
**	ARW: #177555
**
**	Revision 1.52  2012/05/21 16:18:31  e19351
**	Fixed invalid iterator usage. Slight cleanup.
**	AWR: #177555
**
**	Revision 1.51  2004/01/30 23:38:19  khodod
**	Fixed the comparators for large pointer values.
**
****************************************************************/

#define BUILDING_SECDB
#include <portable.h>
#include <skipliko.h>
#include <kool_ade.h>
#include <hash.h>
#include <datatype.h>
#include <secdb.h>
#include <secnode.h>
#include <secnodei.h>
#include <secobj.h>

#include <ccstl.h>
#include <vector>
#include <algorithm>


/*
**	Node stack type
*/

struct NodeDestroyInfo
{
	NodeDestroyInfo( SDB_NODE* Node, bool Undo )
	  : m_Node( Node ),
		m_Undo( Undo )
    {}

	SDB_NODE* m_Node;
	bool	  m_Undo;
};

typedef std::vector< SDB_NODE* > DestroyStack;

typedef std::vector< NodeDestroyInfo > DelayedDestroyStack;

enum DestroyState
{
	StateDestroyInactive,
	StateDestroyMark,
	StateDestroyInProgress
};

/*
** Define Trace only if verbose destroy
*/

#ifdef VERBOSE_DESTROY
#	define TRACE_FUNC( Tag, Node1, Node2 )		if( SecDbTraceFunc ) SecDbTraceFunc( SDB_TRACE_DESTROY_##Tag, Node1, Node2 )
#else
#	define TRACE_FUNC( Tag, Node1, Node2 )
#endif

/*
**	Define values for NodeDestroyCollect/Commit
*/

int const NODE_DESTROY_TOKEN = 0xBADF00D;

/*
** Mark a node as affected by destruction
*/

inline void MARK( SDB_NODE* const Node, DestroyStack* const Stack )
{
	// FIX- should be more efficient, Lookup & Insert should be combined
	// Not sufficient to check if the node is in the hash, it might be marked for destroy by a DestroyCommit
	// which is in the destroy phase and hence the hash table is empty
	if( SDB_NODE::IsNode( Node ) && !Node->IsLiteral() && !Node->DestroyFlagsGet() )
	{
		Node->DestroyFlagsSet( SDB_NODE::DESTROY_AFFECT );
		Stack->push_back( Node );
		TRACE_FUNC( AFFECT, Node, NULL );
	}
}

inline bool MARKED_DELETE_QUICK( SDB_NODE* const Node )
{
	return Node->DestroyFlagsGet() & SDB_NODE::DESTROYED;
}

inline bool MARKED_DELETE_SAFE( SDB_NODE* const Node )
{
	return SDB_NODE::IsNode( Node ) && MARKED_DELETE_QUICK( Node );
}

/*
**	private variables - put these in an anonymous namespace once the compilers get updated
*/


static DestroyStack*        g_DestroyCollect      = 0;                    // stack used to collect nodes to destroy.
static DestroyState         g_DestroyState        = StateDestroyInactive; // Current destroy state
static DelayedDestroyStack* g_DelayedDestroyStack = 0;                    // Stack used to delay destroy messages when a destroy is in progress
static bool                 g_DiagMode            = false;                // Diag mode for destroy

void mark_node( SDB_NODE * node ) { MARK( node, g_DestroyCollect ); }

void mark_node_parent( SDB_NODE * node ) { MARK( SDB_NODE_FROM_PURPLE_PARENT( node ), g_DestroyCollect ); }

void mark_node_child( SDB_NODE * node ) { MARK( SDB_NODE_FROM_PURPLE_CHILD( node ), g_DestroyCollect ); }

/****************************************************************
**	Routine: CheckSanity
**	Returns:
**	Action :
****************************************************************/

static void CheckSanity( void )
{
	HASH* BadHash = SecDbNodeSanityCheck();
	if( !BadHash )
	    return;

	Err( ERR_NOT_CONSISTENT, "Errors in cache!" );

	// This message first in case subsequent NodeText calls crash
	SecDbErrorMessage( ErrGetString());

	{
	    HASH_ENTRY_PTR HashEnumPtr;
		HASH_FOR( HashEnumPtr, BadHash )
		    if( SDB_NODE_INVALID_PTR == (SDB_NODE *) HashValue( HashEnumPtr ))
			{
			    SDB_NODE* const Node = (SDB_NODE *) HashKey( HashEnumPtr );
				if( SDB_NODE::IsNode( Node ))
				    ErrMore( "%-8p %s", Node, Node->GetText( true ).c_str() );
			}
	}
	ErrMore( "\nNodes with bad pointers:" );

	{
	    HASH_ENTRY_PTR HashEnumPtr;
		HASH_FOR( HashEnumPtr, BadHash )
		    if( SDB_NODE_CHILD_DISCONNECTED == (SDB_NODE *) HashValue( HashEnumPtr ))
			{
			    SDB_NODE* const Node = (SDB_NODE *) HashKey( HashEnumPtr );
				if( SDB_NODE::IsNode( Node ))
				    ErrMore( "%-8p %s", Node, Node->GetText( true ).c_str() );
			}
	}
	ErrMore( "\nNodes with disconnected children:" );

	{
	    HASH_ENTRY_PTR HashEnumPtr;
		HASH_FOR( HashEnumPtr, BadHash )
		    if( SDB_NODE_PARENT_DISCONNECTED == (SDB_NODE *) HashValue( HashEnumPtr ))
			{
				SDB_NODE* const Node = (SDB_NODE *) HashKey( HashEnumPtr );
				if( SDB_NODE::IsNode( Node ))
				    ErrMore( "%-8p %s", Node, Node->GetText( true ).c_str() );
			}
	}
	ErrMore( "\nNodes with disconnected parents:" );

	SecDbErrorMessage( ErrGetString());
}



/****************************************************************
**	Routine: SDB_NODE::DestroyCommit
**	Returns: Nothing
**	Action : Make all the deletions marked in Collect
****************************************************************/

void SDB_NODE::DestroyCommit(
	void* pCollect			// collected nodes.
)
{
	DestroyStack* const Collect = static_cast<DestroyStack*>( pCollect );

	if( !Collect || g_DestroyState != StateDestroyInactive )
		return;

	static bool FirstTime = true;

	if( FirstTime )
	{
		ErrPush();
		char const* DiagEnvVar = SecDbConfigurationGet( "SECDB_DIAGNOSTIC_MODE", NULL, NULL, "FALSE" );
		ErrPop();
		g_DiagMode  = ( stricmp( DiagEnvVar, "TRUE" ) == 0 );
		FirstTime = false;
	}

	if( g_DiagMode )
		CheckSanity();

	g_DestroyState = StateDestroyMark;

/*
**	See if any affected nodes need to be deleted based on other deletes.
**	This needs to be run iteratively until Collect has the transitive
**	closure.  (FIX - could be done with single pass if we could color each
**	of the links as being either: <ok to delete>, or <causes parent to be
**	deleted>.  We choose not to do this for now since we believe the deletion
**	chains will be pretty short and the coloring would be pretty ugly.)
*/

	int DeleteCount;
	do
	{
		DeleteCount = 0;
		// Stack may be added to by Destroy, which would invalidate iterators.
		for( size_t index = 0; index < (size_t) Collect->size(); ++index )
		{
			SDB_NODE* const Node = ( *Collect )[ index ];

			if( DESTROY_AFFECT == Node->DestroyFlagsGet() )
			{
				bool DeleteMe = MARKED_DELETE_SAFE( Node->ObjectGet() )    ||
						MARKED_DELETE_SAFE( Node->ValueTypeGet() ) ||
						MARKED_DELETE_SAFE( Node->VtiNodeGet() );
				for( int Num = 0; !DeleteMe && Num < Node->ArgcGet(); ++Num )
				{
					SDB_NODE* const Arg = Node->ArgGet( Num );
					DeleteMe = MARKED_DELETE_SAFE( Arg );
				}
				if( DeleteMe )
				{
					++DeleteCount;
					Destroy( Node, NULL );
				}
			}
		}
	} while( DeleteCount );


/*
**	Trace destroyed nodes before actually unlinking their arguments
**	because if the argument is a ref counted literal it might get destroyed
**	upon the parent unlink
*/

	if( SecDbTraceFunc )
		for( DestroyStack::iterator NodeIter = Collect->begin(); NodeIter < Collect->end(); ++NodeIter )
			if( DESTROYED & (*NodeIter)->DestroyFlagsGet() )
				SecDbTraceFunc( SDB_TRACE_DESTROY, *NodeIter );

	// Destroy is in progress, clear the destroy stack
	g_DestroyCollect = 0;
	g_DestroyState = StateDestroyInProgress;

/*
**	Repair merely affected nodes.
*/

	for( DestroyStack::iterator RepairIter = Collect->begin(); RepairIter < Collect->end(); ++RepairIter )
	{
		SDB_NODE* const Node = *RepairIter;

		if( DESTROY_AFFECT == Node->DestroyFlagsGet() )
		{
			// XXX - Check?
			{
				NodeSet* const AutoChildren = Node->AutoChildrenGet(); // move this into if() when compilers get updated
				if( AutoChildren )
				{
				    for( NodeSet::iterator iter = AutoChildren->begin(); iter != AutoChildren->end(); )
					{
					    if( MARKED_DELETE_QUICK( (*iter) ))
						{
							Node->ChildrenValidSet( false );
							AutoChildren->erase( iter++ );
						}
						else
						  ++iter;
					}
				}
			}

			if( Node->ChildcGet() > 0 && ( Node->ChildrenValidGet() || Node->ChildGet( 0 ) ))
			{
				bool Invalidated = false;

				SDB_NODE** const ChildPtrBound = Node->ChildGetAddr( Node->ChildcGet() );
				SDB_NODE**       ChildPtr      = Node->ChildGetAddr( 0 );

				for( ; ChildPtr < ChildPtrBound; ++ChildPtr )
				{
					SDB_NODE* const Child = SDB_NODE_FROM_PURPLE_CHILD( *ChildPtr );

					if( Child && MARKED_DELETE_QUICK( Child ))
					{
						TRACE_FUNC( AFFECTED_BY, Node, Child );
						Node->ChildrenValidSet( false );
						*ChildPtr = 0;
						Invalidated = true;
					}
				}

				if( Invalidated )
				{
					Node->InvalidateSafe();

					// Do not invalidate children because one of them may pass invalidation
					// up to us
					Node->MakeFirstChildNonNull();
				}
			}
		}
		else if( DESTROYED & Node->DestroyFlagsGet() ) // Undo all parent links to this node
		{
			// Unlink children

			// XXX - Check?
		    {
			    NodeSet* const AutoChildren = Node->AutoChildrenGet();
				if( AutoChildren )
				{
					for( NodeSet::iterator iter = AutoChildren->begin(); iter != AutoChildren->end(); ++iter )
					    (*iter)->ParentUnlink( Node );

					Node->DeleteAutoChildList();
				}
			}

			if( Node->ChildcGet() > 0 && ( Node->ChildrenValidGet() || Node->ChildGet( 0 ) ))
			{
			    SDB_NODE** const ChildPtrBound = Node->ChildGetAddr( 0 );
				SDB_NODE**       ChildPtr      = Node->ChildGetAddr( Node->ChildcGet() - 1 );

				for( ; ChildPtr >= ChildPtrBound; --ChildPtr )
				{
					if( *ChildPtr )
					{
						if( SDB_NODE_IS_PURPLE_CHILD( *ChildPtr ))
							SDB_NODE_FROM_PURPLE_CHILD( *ChildPtr )->ParentUnlink( SDB_NODE_PURPLE_PARENT( Node ));
						else
							(*ChildPtr)->ParentUnlink( Node );
					}
				}
			}

			if( !Node->IsLiteral() )
			{
				// Unlink the implied children of the node
				Node->ObjectGet()->ParentUnlink( SDB_NODE_PURPLE_PARENT( Node ));
				Node->ValueTypeGet()->ParentUnlink( SDB_NODE_PURPLE_PARENT( Node ));

				if( Node->VtiNodeGet() )
					Node->VtiNodeGet()->ParentUnlink( SDB_NODE_PURPLE_PARENT( Node ));

				// Unlink args
				SDB_NODE** const ChildPtrBound = Node->ArgGetAddr( Node->ArgcGet() );
				SDB_NODE**       ChildPtr      = Node->ArgGetAddr( 0 );

				for( ; ChildPtr < ChildPtrBound; ++ChildPtr )
				{
					if( SDB_NODE_IS_PURPLE_CHILD( *ChildPtr ))
						SDB_NODE_FROM_PURPLE_CHILD( *ChildPtr )->ParentUnlink( SDB_NODE_PURPLE_PARENT( Node ));
					else
						(*ChildPtr)->ParentUnlink( Node );
				}
			}

			if( Node->OnLRUCacheGet() && !Node->SetOrDiddleLockGet() )
				Node->LRUCacheByObjectDelete();
		}
	}

/*
**	Actually do the deletions
*/


    for( DestroyStack::iterator DeleteIter = Collect->begin(); DeleteIter < Collect->end(); ++DeleteIter )
	{
	    SDB_NODE* const Node = *DeleteIter;
		if( Node->DestroyFlagsGet() != DESTROYED )
		{
			Node->DestroyFlagsSet( 0 ); // clear destroy flags
			continue;
		}

		// get here if Node->DestroyFlagsGet() == DESTROYED

		// FIX - Should assert that refcount of ref counted literals is 0 before deleting them
		if( Node->IsLiteral() )
		{
		    // Node is already deleted from LiteralHash just free it
		    FreeLiteral( Node );
			continue;
		}
			
		// get here if Non-literal

		// first find the node
		nodes_iterator it = nodes().lower_bound( Node );
		while( it != nodes().end() && equal( *it, Node ) && *it != Node ) 
			++it;

		// now erase it
		if( it != nodes().end() && *it == Node )
			nodes().erase( it );
		else 
			Err( ERR_UNKNOWN, "Internal error: Failed to find the node being deleted in the node map!" );
		if( !Node->SetOrDiddleLockGet() )
		{
			Free( Node );
			continue;
		}

		// get here if SetOrDiddleLockGet() == true

		// Some NodeInvalidate/NodeLRUCacheByObjectInsert will free the node when it is done
		// because we have marked it as Destroyed and it's saved Destroyed state
		// will be different
		if( !Node->OnLRUCacheGet() )
		{
		    memset( Node->m_children + 3, 0, sizeof( *Node->m_children ) * ( Node->child_list_size() - 3 ) );
		    memset( Node->m_parents, 0, sizeof( *Node->m_parents ) * Node->parent_list_size());
		}
	}

	if( g_DiagMode )
		CheckSanity();

	// Mark state as inactive
	delete Collect;
	g_DestroyState = StateDestroyInactive;

	// If we got recursive destroys, execute them all now
	if( g_DelayedDestroyStack )
	{
		DelayedDestroyStack* const Stack = g_DelayedDestroyStack;

		g_DelayedDestroyStack = 0;

		int Token = 0;

		Destroy( NULL, &Token );
		for( DelayedDestroyStack::iterator iter = Stack->begin(); iter < Stack->end(); ++iter )
			Destroy( iter->m_Node, iter->m_Undo ? SDB_NODE_DESTROY_UNDO : &Token );

		delete Stack;

		Destroy( NULL, &Token );
	}
}



/****************************************************************
**	Routine: SDB_NODE::Destroy
**	Returns: Nothing
**	Action : Marks node for deletion and possibly commits all the
**			 deletions collected so far.  Idea is to delay
**			 committing a series of deletions and any related
**			 recursive deletions to keep graph inconsistency time
**			 local and only process side-effects once.
**
**			 If pToken is NULL, then Node is the only node caller
**			 knows should be deleted.  If pToken is non-NULL then
**			 caller is destroying multiple nodes, terminally passing
**			 Node as NULL.
**
**	Example: 
**			 // Single deletion
**			 {	
**				SDB_NODE::Destroy( Node, NULL );
**			 }
**	
**			 // Multiple deletions:
**			 {
**				int		Token = 0;
**
**				// For each Node in deletion list
**					SDB_NODE::Destroy( Node, &Token );
**
**				// Terminate
**				SDB_NODE::Destroy( NULL, &Token );
**			 }
**	
**			 // Recursive deletions I want to group:
**			 {
**				int		Token = 0;
**
**				SDB_NODE::Destroy( NULL, &Token );
**
**				// Code that may call SDB_NODE::Destroy
**
**				SDB_NODE::Destroy( NULL, &Token );
**			 }
**	
****************************************************************/

void SDB_NODE::Destroy(
	SDB_NODE	*Node,		// Node to delete or NULL to indicate end of multiple deletions
	int			*pToken		// NULL: single node; Non-NULL: multiple nodes to collect and delete; NodeDestroyUndo: undo destroy
)
{
	// If destroy is in progress, record the node and return immediately
	// it will be executed when the destroy finishes
	if( g_DestroyState == StateDestroyInProgress )
	{
		if( !g_DelayedDestroyStack )
			g_DelayedDestroyStack = new DelayedDestroyStack;

		if( SDB_NODE::IsNode( Node ))
			g_DelayedDestroyStack->push_back( NodeDestroyInfo( Node, pToken == SDB_NODE_DESTROY_UNDO ));
		return;
	}


/*
**	If this is the last of multiple calls at the top level, commit
**	the deletions.  Top level is indicated by holding the token.
*/

	if( !Node )
	{
		if( !pToken || pToken == SDB_NODE_DESTROY_UNDO )
			return;

		if( g_DestroyCollect )
		{
			if( *pToken == NODE_DESTROY_TOKEN )
			{
				DestroyCommit( g_DestroyCollect );
				*pToken = 0;
				if( SecDbTraceFunc )
					SecDbTraceFunc( SDB_TRACE_DESTROY_END );
			}
			return;
		}
	}


/*
**	If first call at top level, create the collection hash and assign
**	the token.
*/

	if( !g_DestroyCollect )
	{
		if( pToken == SDB_NODE_DESTROY_UNDO )
			return;

  		if( !pToken )
  		{
  			// Single deletion at top level -- turn it into a single element collection
		    int OneShotToken = 0;
  			SDB_NODE::Destroy( Node, &OneShotToken );
  			SDB_NODE::Destroy( NULL, &OneShotToken );
  			return;
  		}
  		g_DestroyCollect = new DestroyStack;
  		*pToken = NODE_DESTROY_TOKEN;

		if( SecDbTraceFunc )
			SecDbTraceFunc( SDB_TRACE_DESTROY_START );
  	}

	if( pToken == SDB_NODE_DESTROY_UNDO )
	{
		if( !Node->IsLiteral() )
		{
			Err( ERR_LEVEL_ERROR, "SDB_NODE::Destroy() Cannot Undo destroy of a NonLiteral. I'll probably be crashing soon." );
			return;
		}
		TRACE_FUNC( UNDO, Node, NULL );
		if( MARKED_DELETE_QUICK( Node ))
			Node->DestroyFlagsSet( SDB_NODE::DESTROY_FAKE );
		HashInsert( s_LiteralHash, (HASH_KEY) &Node->m_Value, Node );
		if( Node->m_Value.DataType == DtSecurity )
		{
			SDB_OBJECT* const SecPtr = static_cast<SDB_OBJECT*>( const_cast<void*>( Node->m_Value.Data.Pointer ));
			if( SecPtr->Db )
				HashInsert( SecPtr->Db->CacheHash, SecPtr->SecData.Name, SecPtr );
		}
		return;
	}

/*
**	Mark this node as deleted and free
*/

	if( !IsNode( Node ) || Node->DestroyFlagsGet() == SDB_NODE::DESTROYED )
		return;

	if( SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_DESTROY_DELETE, Node, NULL );

	bool const is_literal = Node->IsLiteralQuick();
	if( !is_literal )
	{
		while( Node->DiddleGet() )		// node is diddled, forcibly remove diddles
		{
			SDB_DIDDLE* diddle = Node->DiddleHeaderGet()->DiddleList;

			SDB_M_DIDDLE msg_data;
			memset( &msg_data, 0, sizeof( msg_data ));
			msg_data.DiddleID = diddle->DiddleID;
			msg_data.NotMyDiddle = diddle->NotMyDiddle;

			Node->DiddleRemove( (SDB_OBJECT*) Node->ObjectGet()->AccessValue()->Data.Pointer, &msg_data );

			// FIX FIX if diddle fails then what do we do?
			if( Node->DiddleGet() && Node->DiddleHeaderGet()->DiddleList == diddle )
				break;
		}

		static SDB_DBSET* NullDbSet = SDB_DBSET();

		// Invalidate node
		// But do not free literal values since the value itself defines it in NodeLiteralHash--will also be done later by DestroyCommit
		if( Node->ValidGet() )
			Node->Invalidate();

		// Clear out the DbSet of the node so that it will never be found again
		// We don't delete it from the skip list even though we'd like to because this will screw up iterators
		Node->DbSetSet( NullDbSet );
	}
	else
	{
		// Delete Literals from the literal hash because they might end up being found again
		// We can't null out their DbSet because FindLiteral doesn't work that way. Also we don't want to lose
		// the DbSet because Security Literal deletion is undone for some complicated reason I forget
		HashDelete( s_LiteralHash, (HASH_KEY) &Node->m_Value );
		if( Node->m_Value.DataType == DtSecurity )
		{
			SDB_OBJECT* const SecPtr = static_cast<SDB_OBJECT*>( const_cast<void*>( Node->m_Value.Data.Pointer ));
			if( SecPtr->Db )
				HashDelete( SecPtr->Db->CacheHash, SecPtr->SecData.Name );
		}
	}

	// Take Node off any LRUCache because it is being destroyed
	if( Node->OnLRUCacheGet() )
		Node->LRUCacheByObjectDelete();

	// Node may have been marked affect during invalidation
	// so be careful not to multiply insert it on the stack
	if( !Node->DestroyFlagsGet() )
		g_DestroyCollect->push_back( Node );
	Node->DestroyFlagsSet( SDB_NODE::DESTROYED );


/*
**	Mark all the affected nodes
*/

	// Parents
	if( Node->ParentsMax() > 0 )
	    std::for_each( Node->ParentGetAddr( 0 ), Node->ParentGetAddr( Node->ParentsMax() ),
				  mark_node_parent   );

	// Object, ValueType
	if( !is_literal )
	{
		MARK( Node->ObjectGet(), g_DestroyCollect );
		MARK( Node->ValueTypeGet(), g_DestroyCollect );

		// Vti
		MARK( Node->VtiNodeGet(), g_DestroyCollect );

		// Args
		if( Node->ArgcGet() > 0 )
			std::for_each( Node->ArgGetAddr( 0 ), Node->ArgGetAddr( Node->ArgcGet() ),
					  mark_node_child   );

		// Children
		if( Node->ChildcGet() > 0 && ( Node->ChildrenValidGet() || Node->ChildGet( 0 ) ))
			std::for_each( Node->ChildGetAddr( 0 ), Node->ChildGetAddr( Node->ChildcGet() ),
					  mark_node_child   );

		// Implicit Chilren
		{
			NodeSet* const AutoChildren = Node->AutoChildrenGet(); // place inside if() when compilers get updated
			if( AutoChildren )
			{
				if( !Node->IsLiteral() )
				{
					std::for_each( AutoChildren->begin(), AutoChildren->end(), mark_node );
				}
				Node->DeleteAutoChildList();
			}
		}
	}
}



/****************************************************************
**	Routine: SecDbNodeSanityCheck
**	Returns: Hash table of bad nodes; NULL for no cache errors
**			 Hash table keys are the bad nodes addresses, hash
**			 table values are one of:
**				INVALID_PTR			Node references non-existant
**									nodes
**				CHILD_DISCONNECTED	A child does not have this
**									node in parent list
**				PARENT_DISCONNECTED	A parent does not have this
**									node as Object, ValueType,
**									arg, or child
**	Action : Walks through all the nodes and verifies all the
**			 links.  Changes all bad pointers to SDB_NODE_INVALID_PTR.
****************************************************************/

HASH* SecDbNodeSanityCheck( void )
{
	int const EnumFlags = SDB_NODE_ENUM_TERMINALS | SDB_NODE_ENUM_NON_TERMINALS | SDB_NODE_ENUM_LITERALS;

/*
**	Create hash table for bad nodes
*/

    HASH* const BadHash = HashCreate( "BadHash", NULL, NULL, 0, NULL );

/*
**	Check for bad pointers, i.e., node pointers that do not point to nodes (at least any more)
*/

#define CHECK_BAD( Node_, Addr_, AddrLval_ )														\
{																									\
	if( (Addr_) != SDB_NODE_INVALID_PTR && (Addr_) != HashLookup( TmpHash, (HASH_KEY) (Addr_) ))	\
	{																								\
		HashInsert( BadHash, (HASH_KEY) (Node_), (HASH_VALUE) SDB_NODE_INVALID_PTR );				\
		(AddrLval_) = SDB_NODE_INVALID_PTR;															\
	}																								\
}


    {
	    HASH*        TmpHash = HashCreate( "TmpHash", NULL, NULL, 1000, NULL );
		SDB_ENUM_PTR EnumPtr;
		{
		    for( SDB_NODE* Node = SecDbNodeEnumFirst( NULL, EnumFlags, &EnumPtr ); Node; Node = SecDbNodeEnumNext( EnumPtr ))
			    HashInsert( TmpHash, (HASH_KEY) Node, (HASH_VALUE) Node );
		}
		SecDbNodeEnumClose( EnumPtr );
		{
		    for( SDB_NODE* Node = SecDbNodeEnumFirst( NULL, EnumFlags, &EnumPtr ); Node; Node = SecDbNodeEnumNext( EnumPtr ))
			{
			    CHECK_BAD( Node, Node->ObjectGet(), Node->ObjectGetRef() );
				CHECK_BAD( Node, Node->ValueTypeGet(), Node->ValueTypeGetRef() );

				{
				    for( int Num = 0; Num < Node->ArgcGet(); ++Num )
					    CHECK_BAD( Node, Node->ArgGet( Num ), *Node->ArgGetAddr( Num ));
				}
				CHECK_BAD( Node, Node->VtiNodeGet(), Node->VtiNodeGetRef() );
				{
				    for( int Num = 0; Num < Node->ChildcGet(); ++Num )
					    CHECK_BAD( Node, Node->ChildGet( Num ), *Node->ChildGetAddr( Num ));
				}
				{
				    for( int Num = 0; Num < Node->ParentsMax(); ++Num )
					    CHECK_BAD( Node, Node->ParentGet( Num ).PtrValue(), *Node->ParentGetAddr( Num ));
				}
			}
		}
		SecDbNodeEnumClose( EnumPtr );
		HashDestroy( TmpHash );
	}

#undef CHECK_BAD

/*
**	Check for disconnected children, i.e., children who do not have this node in its parent list
*/


// This is a little too lax, it doesn't bother enforcing the fact
// that a purple child link must correspond to a purple parent link
#define CHECK_CHILD( Node_, Addr_ )																														\
{																																						\
	SDB_NODE* const ChildAddr = SDB_NODE_FROM_PURPLE_CHILD( (Addr_) );																					\
	bool Found = false;																																	\
																																						\
	if( SDB_NODE::IsNode( ChildAddr ) && !ChildAddr->RefCountedParents() && !HashLookup( BadHash, (HASH_KEY) (Node_) ))									\
	{																																					\
		Found = SDB_NODE_IS_PURPLE_CHILD( (Addr_) ) ? ChildAddr->ParentLookup( (Node_) ) : ChildAddr->ParentLookup( SDB_NODE_PURPLE_PARENT( (Node_) ));	\
 		if( !Found )																																	\
			HashInsert( BadHash, (HASH_KEY) (Node_), (HASH_VALUE) SDB_NODE_CHILD_DISCONNECTED );														\
	}																																					\
}


    {
	    SDB_ENUM_PTR EnumPtr;
		for( SDB_NODE* Node = SecDbNodeEnumFirst( NULL, EnumFlags, &EnumPtr ); Node; Node = SecDbNodeEnumNext( EnumPtr ))
		{
		    CHECK_CHILD( Node, SDB_NODE_PURPLE_CHILD( Node->ObjectGet() ));
			CHECK_CHILD( Node, SDB_NODE_PURPLE_CHILD( Node->ValueTypeGet() ));
			{
			    for( int Num = 0; Num < Node->ArgcGet(); ++Num )
				    CHECK_CHILD( Node, Node->ArgGet( Num ));
			}
			{
			    for( int Num = 0; Num < Node->ChildcGet(); ++Num )
				    CHECK_CHILD( Node, Node->ChildGet( Num ));
			}
			CHECK_CHILD( Node, SDB_NODE_PURPLE_CHILD( Node->VtiNodeGet() ));
		}
		SecDbNodeEnumClose( EnumPtr );
	}

#undef CHECK_CHILD

/*
**	Check for disconnected parents, i.e., parents that do not actually reference this node
*/

#define CHECK_PARENT( Node_, Addr_ )																								\
{																																	\
	SDB_NODE* const ParentAddr = (Addr_).Node();																					\
	bool Found = false;																												\
																																	\
	if( SDB_NODE::IsNode( ParentAddr ) && !HashLookup( BadHash, (HASH_KEY) (Node_) ))												\
	{																																\
		Found = ParentAddr->ObjectGet() == (Node_) || ParentAddr->ValueTypeGet() == (Node_) || ParentAddr->VtiNodeGet() == (Node_);	\
		{																															\
		    for( int ElemNum = 0; !Found && ElemNum < ParentAddr->ArgcGet(); ++ElemNum )											\
		        Found = SDB_NODE_FROM_PURPLE_CHILD( ParentAddr->ArgGet( ElemNum )) == (Node_);										\
		}																															\
        {																															\
            for( int ElemNum = 0; !Found && ElemNum < ParentAddr->ChildcGet(); ++ElemNum )											\
			    Found = SDB_NODE_FROM_PURPLE_CHILD( ParentAddr->ChildGet( ElemNum )) == (Node_);									\
 		}																															\
        if( !Found )																												\
			HashInsert( BadHash, (HASH_KEY) (Node_), (HASH_VALUE) SDB_NODE_PARENT_DISCONNECTED );									\
	}																																\
}


    {
	    SDB_ENUM_PTR EnumPtr;
		for( SDB_NODE* Node = SecDbNodeEnumFirst( NULL, EnumFlags, &EnumPtr ); Node; Node = SecDbNodeEnumNext( EnumPtr ))
		    for( int Num = 0; Num < Node->ParentsMax(); ++Num )
			    CHECK_PARENT( Node, Node->ParentGet( Num ) );
		SecDbNodeEnumClose( EnumPtr );
	}

#undef CHECK_PARENT

/*
**	Return hash of errors found
*/

	if( BadHash->KeyCount == 0 )
	{
		HashDestroy( BadHash );
		return 0;
	}
	return BadHash;
}
