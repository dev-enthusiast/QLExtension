#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbninv.c,v 1.71 2004/02/02 17:24:09 khodod Exp $"
/****************************************************************
**
**	SDBNINV.C
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbninv.c,v $
**	Revision 1.71  2004/02/02 17:24:09  khodod
**	Revert the fix for now.
**
**	Revision 1.69  2001/11/27 23:23:41  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.67  2001/08/02 21:10:44  hsuf
**	Fix: When Invalidating an SDB_RELEASE* node, make sure we set its
**	ValidLink flag to false.  If the node remains ValidLink==true, secdb may try to
**	invalidate this node again through another link path.  If we have a diamond
**	shaped node graph (where every node is a SDB_RELEASE_VALUE_BY_DESCENDENT),
**	we may invalidate the top node more than once, unnecessarily.
**	
**	Revision 1.66  2001/03/20 17:18:56  singhki
**	tracing cleanups using cool trace_scope
**	
**	Revision 1.65  2001/01/03 23:26:01  simpsk
**	Fixing splitting of auto children.
**	
**	Revision 1.64  2000/12/05 16:48:29  singhki
**	Added Subscribe capability to nodes
**	
**	Revision 1.63  2000/07/05 23:31:29  nauntm
**	fix for passerrors: partial child lists added (KIS)
**	
**	Revision 1.62  2000/04/24 11:04:27  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.61  2000/02/16 21:14:20  singhki
**	oops, need SetOrDiddleLock in order for Destroy/LRUCache to work
**	correctly. What was I thinking!
**	
**	Revision 1.60  2000/02/15 21:11:03  singhki
**	free value before recursive invalidate, no need for SetLock during
**	Invalidate or InvalidateChildren anymore.
**	
**	Revision 1.59  2000/02/08 04:50:28  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.58  2000/01/25 22:35:23  singhki
**	Mark cycle detected/val changed/invalidating while calc as fatal errors, and do not allow PassErrors to hide them
**	
**	Revision 1.57  1999/12/22 17:30:22  singhki
**	fix comments
**	
**	Revision 1.56  1999/11/17 23:42:08  davidal
**	Added two new attributes for VT as follows:
**	- SDB_RELEASE_VALUE;
**	- SDB_RELEASE_VALUE_WHEN_PARENTS_VALID.
**	Once set for the VT, the former will cause an immediate release of the node's value after it gets consumed
**	by a parent; the latter will do the same after *all* the parents are valid.
**	By default, both the attributes are set to FALSE; however, defining the env variable
**	SECDB_RELEASE_VALUE_WHEN_PARENTS_VALID as TRUE will set SDB_RELEASE_VALUE_WHEN_PARENTS_VALID for all
**	non-terminal nodes.
**	
**	Revision 1.55  1999/10/22 21:45:39  singhki
**	Do not access Node->Value directly but use member functions
**	
**	Revision 1.54  1999/10/18 18:50:06  singhki
**	added BUILDING_SECDB macro
**	
**	Revision 1.53  1999/10/13 19:16:52  singhki
**	remove extraneous variable decl
**	
**	Revision 1.52  1999/09/13 15:17:53  singhki
**	Add DestroyLiteral arg to NodeDestroyCache in order to control destruction of literals during detach
**	
**	Revision 1.51  1999/09/07 22:57:59  singhki
**	Fix up secptr freeing some more
**	
**	Revision 1.50  1999/09/01 01:55:15  singhki
**	Get rid of SDB_CHILD_REFs, split intermediates and terminals into seperate
**	arrays in SDB_CHILD_LIST. Rationalize NoEval stuff, auto mark args/ints as
**	NoEval if they are never evaluated.
**	Color child links as purple so that we don't have to find the VTI in order
**	to correctly unlink them.
**	Uncolor arg/int which aren't purple so that they won't invalidate the tree.
**	
**	Revision 1.49  1999/08/07 01:40:14  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.48  1999/07/20 16:41:33  singhki
**	use macros for TRACE_FUNC
**	
**	Revision 1.47  1999/07/15 23:27:38  singhki
**	hack: always success in InvalidateChildren in we have duplicate children
**	
**	Revision 1.46  1999/06/03 01:17:33  singhki
**	remove hacky use of Locked, instead remember Destroyed state in order to decide whether Invalidate frees the node or not
**	
**	Revision 1.45  1999/06/02 17:10:52  singhki
**	Take care not to **** up ref counted of ref counted literals on unlink. Also fix little bug where invalidatechildren thinks node is locked when it is being destroyed
**	
**	Revision 1.44  1999/05/25 16:24:18  singhki
**	Lock node in destroy to prevent Invalidate from freeing it
**	
**	Revision 1.43  1999/05/20 23:20:54  singhki
**	Add nodes with 0 parents to an LRU list on the node's object
**	The last n nodes of this list get thrown away when it reaches
**	MaxUnparentedNodes which is set on a class basis
**	
**	Revision 1.42  1999/04/29 21:32:54  singhki
**	If VTI is null for a terminal, then unlink children as both purple & non purple. Slower but work.
**	Typically we will have a VTI so we can do the right thing. Precompute child array bounds in iteration.
**	
**	Revision 1.41  1999/04/21 16:07:24  singhki
**	Stylistic changes in response to code review
**	
**	Revision 1.40  1999/04/16 23:53:21  singhki
**	Add NodeFlags in NodeFindChild based on Item flags set by SecDbChildAdd. Fixup noeval stuff to work better with each.
**	
**	Revision 1.39  1999/04/14 16:48:31  singhki
**	Track sideffects of phantom diddles in order to be able to correctly stream out the diddle state
**	
**	Revision 1.38  1999/04/06 22:50:37  singhki
**	Support parented literals of which SecPtrs are the only one.
**	All other literals are refcounted.
**	This allows proper cleanup of nodes using the SecPtr as args.
**	Tweak node parent hash table.
**	
**	Revision 1.37  1999/03/22 21:26:35  singhki
**	Terminal nodes now add all the intermediates as purple children.
**	A purple child has a purple parent link back to the parent, and invalidating a purple child invalidates the child list of the parent.
**	Arguments are now resolved to literals. Each() arguments invalidate correctly and do not require the object or vt to be an each.
**	Alias nodes are gone. Non terminals are now unnecessary as the object, vt and args of a node are now literals.
**	
**	Revision 1.36  1999/02/16 20:18:20  singhki
**	Fix UMR
**	
**	Revision 1.35  1999/01/29 20:16:22  singhki
**	BugFix: If some Arg and Child are the same node, then InvalidateChildren
**	loses the parentlink which screws up destroy/invalidate, etc.
**	Added DuplicateChildren bit on Parent to detect this state, InvalidateChildren
**	now relinks Args if this bit is set and then clears it. Still need to resolve
**	duplicate problems related to Object, ValueType & VTI of node.
**	
**	Revision 1.34  1999/01/29 00:05:34  singhki
**	Check for NULL child before ParentUnlink
**	
**	Revision 1.33  1999/01/20 21:52:58  singhki
**	Fail ParentLink/Unlink if node is locked
**	
**	Revision 1.32  1998/12/09 01:28:29  singhki
**	Use ERR_OFF/ON instead of ErrPush/Pop
**	
**	Revision 1.31  1998/11/27 19:54:51  singhki
**	minor bug fixes in NodeSplitChildren & DbSet code, better trace for node split
**	
**	Revision 1.30  1998/11/18 17:42:53  singhki
**	Add NodeIsValid & remove default args from NodeFind... to force explicit spec of Db & Share
**	
**	Revision 1.29  1998/11/16 23:00:04  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.28.2.2  1998/11/05 02:53:20  singhki
**	factored c++ stuff into internal header files
**	
**	Revision 1.28.2.1  1998/11/04 19:04:46  singhki
**	use new C++ node api. Optimize ParentLink/Unlink, makes initial DP much much faster
**	
**	Revision 1.28  1998/10/01 15:08:56  gribbg
**	Add SDB_NODE_LOCKALL_ENABLE
**	
**	Revision 1.27  1998/08/11 17:27:31  singhki
**	Don't free value on invalidate if node is locked 'cause it crashes
**	
**	Revision 1.26  1998/06/26 16:24:53  lundek
**	BugFix: do not infinitely recurse on ValidLink cycle
**	
**	Revision 1.25  1998/06/04 19:38:59  lundek
**	Restore 1.23
**	
**	Revision 1.23  1998/06/03 15:49:01  wene
**	Removed PassErrors restriction for invalidating nodes with ValidLink
**	
**	Revision 1.22  1998/05/13 20:59:08  brownb
**	fix compiler warning
**	
**	Revision 1.21  1998/04/15 13:33:40  gribbg
**	Add NodeLockAll/UnlockAll
**	
**	Revision 1.20  1998/03/25 20:31:43  gribbg
**	More little optimizaions
**	
**	Revision 1.19  1998/03/25 17:39:33  gribbg
**	Optimize things a little bit
**	
**	Revision 1.18  1998/01/02 21:33:21  rubenb
**	added GSCVSID
**	
**	Revision 1.17  1997/12/01 20:55:58  singhki
**	Added VTI to nodes and Ellipsis Arg type
**	
**	Revision 1.16  1997/11/03 16:35:40  lundek
**	casts
**	
**	Revision 1.15  1997/06/06 04:39:45  lundek
**	Streamline DtEach handling
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<hash.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<sectrace.h>
#include	<secerror.h>


/*
**	Define constants
*/

#define	MAX_ARGS_IN_ERRMSG	3
// #define FULL_VALUE_TEXT


/*
**	Macros
*/

#define	TRACE_FUNC( Msg, Node_ )											\
	(void) ( !SecDbTraceFunc || (SecDbTraceFunc)( SDB_TRACE_##Msg, (Node_) ))


/*
**	Variables
*/

#if defined( SECNODE_DBUG )

HASH	*CacheDebugHash	= NULL,
		*ListDebugHash	= NULL;

#endif



/****************************************************************
**	Routine: SDB_NODE::Invalidate
**	Returns: TRUE	- Success
**			 FALSE	- Invalidated a locked node
**	Action : Frees value and recursively invalidates
****************************************************************/

bool SDB_NODE::Invalidate()
{
	bool	SvValidLink,
			SvDestroyed,
			SvLock;


	if( this && ( ValidGet() || ValidLinkGet() ))
	{
		if( NodeLockers )
			return Err( SDB_ERR_OBJECT_IN_USE, "Invalidating node while all nodes locked by '%s'", NodeLockers->Name );

		// Prevent infinite recursion for validlink cycles
		SvValidLink = ValidLinkGet();
		ValidLinkSet( false );

		// Use the SetOrDiddleLock to prevent the node from being freed in Destroy or put on the LRUCache
		// which might realloc the ParentList which would screw us
		SvLock = SetOrDiddleLockGet();
		SetOrDiddleLockSet( true );

		sdb_node_trace_scope< SDB_TRACE_INVALIDATE, SDB_TRACE_DONE_INVALIDATE > tracer( this );

		// Throw away the value
		if( ValidGet() )
			FreeValue();				// This also invalidates the node
		else if( IsReservedValidValue() )
		{
			m_Value.DataType = 0;		// clear the state for a 'ReleaseValue*' node which is the parent of
										// a 'ReleaseValueWhenParentsValid' node

			SvValidLink = false;        // Once a sdbRelease* node is invalidated explicitly, it is no longer
			                            // ValidLink node.  If we do not clear this flag, then a complex graph
			                            // of sdbRelease* nodes (e.g. Gexa Tableau Cell nodes) may be invalidated
			                            // many thousand times unnecessarily.
		}

		if( SubscribedGet() )			// If subscribed to this node, record that it was changed
			s_changedNodes->insert( this );

		SvDestroyed = DestroyedGet();

		// A parent which has Node as it's VTI will need the VTI in order to unlink it's children
		// so we first invalidate all our parents
		SDB_NODE
				**ParentMax = ParentGetAddr( ParentsMax() );

		for( SDB_NODE **ParentPtr = ParentGetAddr( 0 ); ParentPtr < ParentMax; ++ParentPtr )
		{
			if( IsNode( *ParentPtr ))
			{
				SDB_NODE
						*Parent = SDB_NODE_PARENT_PTR( *ParentPtr ).Node();

				// If purple parent, then invalidate it's children
				if( SDB_NODE_PARENT_PTR( *ParentPtr ).IsPurple() )
					Parent->InvalidateChildren();

				if(( Parent->ValidGet() || Parent->ValidLinkGet() ) && !( Parent->CacheFlagsGet() & ( SDB_CACHE_DIDDLE | SDB_CACHE_SET )))
					Parent->Invalidate();
			}
		}

/*
**	Check for moribundity and cycles
*/

		// NodeInvalidate may be reentered if ValidLink after a Node has
		// been marked by destroy. Therefore we only destroy the node if
		// the Destroy state has changed during any recursion which happened above
		if( DestroyedGet() && !SvDestroyed )
		{
			Free( this );
			return true;
		}
		
		ValidLinkSet( SvValidLink );
		SetOrDiddleLockSet( SvLock );
		if( GetValueLockGet() )
		{
			Err( SDB_ERR_INVALIDATING_LOCKED_VALUE, "@ - Fatal error. Note this may also cause a memory leak." );
			TRACE_FUNC( ERROR_NODE, this );
			return false;
		}
	}
	return true;
}



/****************************************************************
**	Routine: SDB_NODE::InvalidateChildrenInt
**	Returns: TRUE/FALSE
**	Action : Undo child/parent links
****************************************************************/

int SDB_NODE::InvalidateChildrenInt(
)
{
	ChildrenValidSet( false );

	if( ChildcGet() == 0 )
		return TRUE;

	int		Success = TRUE;

	SDB_NODE
			**ChildPtr = ChildGetAddr( ChildcGet() - 1 ),
			**ChildPtrBound = ChildGetAddr( 0 );

	bool SvDestroyed = DestroyedGet();
	bool SvLock = SetOrDiddleLockGet();
	SetOrDiddleLockSet( true );

	for( ; ChildPtr >= ChildPtrBound; --ChildPtr )
		if( *ChildPtr )
		{
			if( SDB_NODE_IS_PURPLE_CHILD( *ChildPtr ))
				Success = SDB_NODE_FROM_PURPLE_CHILD( *ChildPtr )->ParentUnlink( SDB_NODE_PURPLE_PARENT( this )) && Success;
			else
				Success = (*ChildPtr)->ParentUnlink( this ) && Success;
			*ChildPtr = NULL;
		}

	// FIX- This is still wrong if the VTI node happens to be the same as one of the arguments
	// but that is an extremely unlikely nasty event so I am sure it will happen :-)
	if( DuplicateChildrenGet() )
	{
		for( ChildPtr = ChildGetAddr( 0 ) - 1; ChildPtr >= ArgGetAddr( 0 ); --ChildPtr )
			if( *ChildPtr )
			{
				SDB_NODE *Child = SDB_NODE_FROM_PURPLE_CHILD( *ChildPtr );
				if( Child->RefCountedParents() )
					continue;

				if( SDB_NODE_IS_PURPLE_CHILD( *ChildPtr ))
					Child->ParentLink( SDB_NODE_PURPLE_PARENT( this ));
				else
					Child->ParentLink( this );
			}
		DuplicateChildrenSet( false );
		// FIX- If DuplicateChildren, then we assume some Unlink operations failed because of the duplicates
		// however the failure might have been due to something more sinister, so this should really get fixed
		Success = TRUE;
	}

	if( DestroyedGet() && !SvDestroyed )	// Node was destroyed recursively, but not freed, free it
		Free( this );
	else
		SetOrDiddleLockSet( SvLock );

	return Success;
}



/****************************************************************
**	Routine: SDB_NODE::InvalidateChildren
**	Returns: TRUE/Err
**	Action : Removes all the children pointers from Parent
****************************************************************/

bool SDB_NODE::InvalidateChildren(
)
{
	sdb_node_trace_scope< SDB_TRACE_INVALIDATE_CHILDREN, SDB_TRACE_DONE_INVALIDATE_CHILDREN > tracer( this );

	if( GetValueLockGet() )
	{
		Error( "InvalidateChildren during GetValue" );
		TRACE_FUNC( ERROR_NODE, this );
		return false;
	}
	if( ChildrenValidGet() )
	{
		ChildrenValidSet( false );
		MakeFirstChildNonNull();
	}

	return true;
}



/****************************************************************
**	Routine: NodeDestroyCache
**	Returns: Nothing
**	Action : Free all the nodes for given SecPtr
****************************************************************/

void NodeDestroyCache(
	SDB_OBJECT	*SecPtr,
	int			DestroyLiteral
)
{
	int		Token = 0;

	TRACE_FUNC( DESTROY_CACHE, SecPtr );

	SecDbRemoveDiddle( SecPtr, NULL, 0 );

	// Find the security literal
	DT_VALUE TmpVal;
	TmpVal.DataType = DtSecurity;
	TmpVal.Data.Pointer = SecPtr;
	SDB_NODE *Literal = SDB_NODE::FindLiteral( &TmpVal, SecDbRootDb );

	// It should have all the parent pointers, so just destroy it
	SDB_NODE::Destroy( NULL, &Token );
	SDB_NODE::Destroy( Literal, NULL );
	// Note that we may not want to destroy the literal here because that would free the SecPtr
	// and remove it from the CacheHash. Detach doesn't necessarily want to do this..
	if( !DestroyLiteral )
		SDB_NODE::Destroy( Literal, SDB_NODE_DESTROY_UNDO );
	SDB_NODE::Destroy( NULL, &Token );

	TRACE_FUNC( DONE_DESTROY_CACHE, SecPtr );
}



/****************************************************************
**	Routine: NodeLockAll
**	Returns: TRUE/Err()
**	Action : Lock all nodes to prevent invalidation
**			 Used by ForChildren to prevent nodes from going away
****************************************************************/

int NodeLockAll(
	const char	*LockerName		// Must be static constant
)
{
	if( getenv( "SECDB_NODE_LOCKALL_ENABLE" ))
	{
		SDB_NODE_LOCKER
				*Lock = (SDB_NODE_LOCKER *) calloc( 1, sizeof( *Lock ));
	
	
		Lock->Name  = LockerName;
		Lock->Next  = NodeLockers;
		NodeLockers	= Lock;
	}
	return TRUE;
}



/****************************************************************
**	Routine: NodeUnlockAll
**	Returns: TRUE/Err()
**	Action : Pop a lock off the top of the lock stack
****************************************************************/

int NodeUnlockAll(
	const char	*LockerName		// Must be static constant
)
{
	if( getenv( "SECDB_NODE_LOCKALL_ENABLE" ))
	{
		SDB_NODE_LOCKER
				*Lock = NodeLockers;
	
	
		if( !Lock )
			Err( ERR_NOT_CONSISTENT, "NodeUnlockAll( %s ), nothing on stack", LockerName );
		else if( 0 != stricmp( Lock->Name, LockerName ))
			Err( ERR_NOT_CONSISTENT, "NodeUnlockAll( %s ), top of stack is '%s'", LockerName, Lock->Name );
		else
		{
			NodeLockers = Lock->Next;
			free( Lock );
			return TRUE;
		}
	
		ErrMsg( ERR_LEVEL_ERROR, "%s", ErrGetString() );
		return FALSE;
	}
	return TRUE;
}

