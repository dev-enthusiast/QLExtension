#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbnenum.c,v 1.43 2001/10/02 15:33:26 singhki Exp $"
/****************************************************************
**
**	SDBNENUM.C
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbnenum.c,v $
**	Revision 1.43  2001/10/02 15:33:26  singhki
**	move object,vt and vti into children. allows us to write better
**	iterators, literals take less space much cooler in general.
**
**	Revision 1.42  2001/09/27 20:54:11  singhki
**	switch to collision hash, forchildren 2x as fast
**	
**	Revision 1.41  2001/08/20 22:24:24  simpsk
**	various cleanups: added missing #includes, removed deprecated CC_ macros.
**	
**	Revision 1.40  2001/06/29 15:22:16  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.39  2001/03/16 18:05:59  singhki
**	initialize HashEntryPtr
**	
**	Revision 1.38  2001/03/14 18:33:30  singhki
**	Rewrite nodes to use STL style multiset instead of skip list
**	
**	Revision 1.37  2001/02/06 16:37:35  singhki
**	Ask each node for its ExpectedChildDb during ForChildren because if we
**	need to build the graph we need to keep track of which Db to build it
**	in.
**	
**	This all really sucks we should write generic graph traveral code
**	which can then be used by getvalue, forchildren and whoever else needs
**	to traverse the graph. This approach just leads to duplicated code.
**	
**	Revision 1.36  2001/01/30 01:26:59  singhki
**	add new error for DbChanged and use it in CollectChildren
**	
**	Revision 1.35  2001/01/30 01:16:16  singhki
**	Yet another fix for BuildChildren during CollectChildren which makes
**	it set the PrimaryDb to the correct Db in which it wants the children
**	to be built
**	
**	Revision 1.34  2001/01/29 23:32:18  singhki
**	warning fix
**	
**	Revision 1.33  2001/01/29 23:19:21  singhki
**	Respect pass errors while building children and throw an exception if db changes in order to capture the third return state
**	
**	Revision 1.32  2001/01/11 22:22:45  singhki
**	Return failures from CollectChildren. Also do not hide errors if
**	Parent is pass errors, has no bearing on CollectChild failure.
**	Fail early if Db is no longer in the DbSet after a buildchildren.
**	SecDbChildEnumFirst will retry in this case.
**	
**	Revision 1.31  2000/12/19 17:20:44  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.30  2000/11/02 11:31:03  shahia
**	Back out last rev, checked in by mistake
**	
**	Revision 1.28  2000/09/12 10:23:40  shahia
**	Bug Fix: F1 didn't show children when node was invalid
**	
**	Revision 1.27  2000/06/06 18:12:21  singhki
**	Do not build children unless we should Recurse on this node
**	
**	Revision 1.26  2000/04/24 11:04:25  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.25  2000/02/08 04:50:27  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.24  1999/12/22 19:33:46  singhki
**	move node enum stuff from sdbnfind.c
**	
**	Revision 1.23  1999/12/22 17:31:01  singhki
**	fix comments
**	
**	Revision 1.22  1999/11/18 18:51:19  singhki
**	arghh, make SDB_CHILD_ENUM_FILTER a struct because of OMNI, can you ****ing believe that?
**	
**	Revision 1.21  1999/11/17 02:37:51  singhki
**	make Match a bool to fix warning
**	
**	Revision 1.20  1999/11/17 01:04:13  singhki
**	Rewrite ChildEnum filters as classes with virtual functions in order to allow Slang to define custom filters
**	
**	Revision 1.19  1999/11/01 22:41:53  singhki
**	death to all link warnings
**	
**	Revision 1.18  1999/07/16 21:22:13  singhki
**	NodeGetValue() now is passed the Db so that it can ensure that it builds
**	the childrne in the correct Db.
**	Fixed VTI interactions with diddle scopes, VTI's now not only cause splits
**	but also are set on extended nodes.
**	
**	Revision 1.17  1999/06/15 23:12:43  hsuf
**	SDB_CHILD_INFO contains SDB_DBSET pointer, instead of DbSet name
**	
**	Revision 1.16  1999/06/15 20:49:44  hsuf
**	added DbSet to SDB_CHILD_LIST structure, so we can tell one node
**	from another, if they are from the same vt, and live in different
**	diddle scopes.
**	
**	Revision 1.15  1999/05/14 16:49:35  singhki
**	Set only the useful cacheflags on the node, saves 9 bits. Also fix bug in alias handling
**	
**	Revision 1.14  1999/04/16 23:53:20  singhki
**	Add NodeFlags in NodeFindChild based on Item flags set by SecDbChildAdd. Fixup noeval stuff to work better with each.
**	
**	Revision 1.13  1999/01/12 17:07:38  singhki
**	Match a terminal node even if the VT doesn't exist on the class
**	
**	Revision 1.12  1998/11/16 23:00:02  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.11.4.2  1998/11/05 02:53:17  singhki
**	factored c++ stuff into internal header files
**	
**	Revision 1.11.4.1  1998/11/04 19:04:43  singhki
**	use new C++ node api. Optimize ParentLink/Unlink, makes initial DP much much faster
**	
**	Revision 1.11  1998/04/15 13:33:37  gribbg
**	Add NodeLockAll/UnlockAll
**	
**	Revision 1.10  1998/01/02 21:33:15  rubenb
**	added GSCVSID
**	
**	Revision 1.9  1997/10/06 13:17:01  schlae
**	More Heap fns become GSHeap fns
**	
**	Revision 1.8  1997/08/05 02:27:07  lundek
**	BugFix: Exclude ~Cast, ~Size, etc. type nodes from enumeration
**
**	Revision 1.7  1997/04/30 01:00:13  lundek
**	Do not trust Childc for literal nodes
**
**	Revision 1.6  1997/01/10 21:59:20  gribbg
**	Vector Diddles
**
**	Revision 1.5.2.2  1996/12/20 21:17:00  lundek
**	Revert Node Scope stuff
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<heap.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secerror.h>
#include	<secdb.h>
#include	<sdb_enum.h>
#include    <sdbdbset.h>

#include	<exception>
CC_USING( std::exception );

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	<extension/collision_hash>

struct node_hash
{
	size_t operator()( SDB_NODE* n ) const { return (size_t) n; }
};

typedef COLLISION_HASH_SET_H( SDB_NODE*, node_hash ) NodeSet;

static HEAP
		*ChildInfoHeap = NULL;


/*
**	Define node enumeration structure
*/

struct ENUM_CONTEXT
{
	SDB_OBJECT
			*SecPtr;

	int		EnumFlags;

	SDB_NODE::nodes_iterator
			iter;

	HASH_ENTRY_PTR
			HashEntryPtr;

	bool first_time;

	ENUM_CONTEXT( SDB_OBJECT *sec_ptr, int enum_flags )
	: SecPtr( sec_ptr ), EnumFlags( enum_flags ), HashEntryPtr( 0 ), first_time( true )
	{
		SDB_NODE tmp_node;
		memset( &tmp_node, 0, sizeof( tmp_node ) );
	}

	void reset_iterator()
	{
		if( SecPtr )
		{
			DT_VALUE TmpVal;
			DTM_INIT( &TmpVal );
			TmpVal.DataType		= DtSecurity;
			TmpVal.Data.Pointer	= SecPtr;

			// First check if it is there, if not, no need to create it
			if( !SDB_NODE::HaveLiteral( &TmpVal ))
				iter = SDB_NODE::nodes().end();
			else
			{
				SDB_NODE tmp_node;
				SDB_NODE* children[3];
				memset( &tmp_node, 0, sizeof( tmp_node ));
				children[0] = SDB_NODE::FindLiteral( &TmpVal, SecDbRootDb );
				children[1] = 0;
				children[2] = 0;
				tmp_node.m_children = children;
				iter = SDB_NODE::nodes().lower_bound( &tmp_node );
			}
		}
		else
			iter = SDB_NODE::nodes().begin();
		first_time = false;
	}
	~ENUM_CONTEXT()
	{}
};

struct db_changed : exception
{
	db_changed( SDB_NODE *Node ) : m_node( Node ) {}

	SDB_NODE *node() const { return m_node; }

  private:
	SDB_NODE *m_node;
};


/*
**	Local prototypes
*/

static int CollectChild(
	SDB_NODE				*Node,
	SDB_CHILD_ENUM_FILTER	*Filter,
	NodeSet&				Visited,
	HASH					*Results,
	SDB_DB					*Db
);

static int CollectChildren(
	SDB_NODE				*Parent,
	SDB_CHILD_ENUM_FILTER	*Filter,
	NodeSet&				Visited,
	HASH					*Results,
	SDB_DB					*Db
);



/****************************************************************
**	Routine: NodeChildHash
**	Returns: Bucket number
**	Action : Combines SecPtr and ValueType pointers
****************************************************************/

static HASH_BUCKET NodeChildHash(
	HASH_KEY	Key
)
{
	SDB_CHILD_INFO
			*Node = (SDB_CHILD_INFO *) Key;


	return( (HASH_BUCKET) (( (unsigned long) Node->SecPtr << 5 ) + ( (unsigned long) Node->ValueType >> 5 )));
}



/****************************************************************
**	Routine: NodeChildCompare
**	Returns: Zero for equal; non-zero for not equal
**	Action : Compares the two nodes
****************************************************************/

static int NodeChildCompare(
	HASH_KEY	Key1,
	HASH_KEY	Key2
)
{
	SDB_CHILD_INFO
			*Node1 = (SDB_CHILD_INFO *) Key1,
			*Node2 = (SDB_CHILD_INFO *) Key2;

	int		ArgNum;

	DT_VALUE
			Result;


	if( Node1->SecPtr != Node2->SecPtr
			|| Node1->ValueType != Node2->ValueType
			|| Node1->Argc != Node2->Argc
		    || Node1->DbSet != Node2->DbSet )
		return( 1 );

	for( ArgNum = 0; ArgNum < Node1->Argc; ArgNum++ )
		if( !DT_OP( DT_MSG_EQUAL, &Result, &Node1->Argv[ ArgNum ], &Node2->Argv[ ArgNum ] )
				|| Result.Data.Number == 0.0 )
			return( 1 );

	return( 0 );
}



/****************************************************************
**	Routine: CollectChild
**	Returns: TRUE or FALSE
**	Action : Looks for match on given child, etc.
****************************************************************/

static int CollectChild(
	SDB_NODE				*Node,		// Node to collect
	SDB_CHILD_ENUM_FILTER	*Filter,	// Child Enum Filter
	NodeSet&				Visited,	// Hash of all the nodes that have gone through CollectChild already in this enumeration
	HASH					*Results,	// Hash of matching nodes so far
	SDB_DB					*Db
)
{
	int		ArgNum,
			Recurse,
			Terminal,
			NonTerminal;

	bool	Match;

	SDB_NODE
			*ArgNode;

	SDB_CHILD_INFO
			*ChildInfo;

	SDB_NODE_CONTEXT
			SubCtx;


/*
**	Only visit any branch once
*/

	if( !Visited.insert( Node ).second )
		return TRUE;


/*
**	Check for match
*/

	Terminal	= Node->IsTerminal() && ( (SDB_OBJECT *) Node->ObjectGet()->m_Value.Data.Pointer )->Class != NULL; // Class != NULL test for ~Cast, ~Size, etc. type terminal-looking nodes
	NonTerminal = !Terminal && SDB_NODE::IsNode( Node ) && !Node->IsLiteral();

	Match = Terminal ? Filter->Match( Node ) : false;

	if( Match )
	{
		ChildInfo = (SDB_CHILD_INFO *) GSHeapMalloc( ChildInfoHeap );
		if( !ChildInfo )
			return( FALSE );

		ChildInfo->Node			= Node;
		ChildInfo->SecPtr		= (SDB_OBJECT *) Node->ObjectGet()->AccessValue()->Data.Pointer;
		ChildInfo->ValueType	= (SDB_VALUE_TYPE) Node->ValueTypeGet()->AccessValue()->Data.Pointer;
		ChildInfo->Argc			= Node->ArgcGet();
		ChildInfo->DbSet        = Node->DbSetGet();

		if( Node->ArgcGet() )
		{
			if( !ERR_CALLOC( ChildInfo->Argv, DT_VALUE, ChildInfo->Argc, sizeof( *ChildInfo->Argv )))
				return( FALSE );
			for( ArgNum = 0; ArgNum < ChildInfo->Argc; ArgNum++ )
			{
				ArgNode = Node->ArgGet( ArgNum );
				if( !ArgNode->ValidGet() )
				{
					memset( &SubCtx, 0, sizeof( SubCtx ));
					if( !ArgNode->GetValue( &SubCtx, Db ))
						return( FALSE );
				}
				if( !DTM_ASSIGN( &ChildInfo->Argv[ ArgNum ], ArgNode->AccessValue()))
					return( FALSE );

			}
		}
		HashInsert( Results, ChildInfo, (HASH_VALUE)( (long) Match + 1 ));
	}


/*
**	Check for further recursion
*/

	Recurse = NonTerminal || ( Terminal && Filter->Recurse( Node, Match ));

	return Recurse ? CollectChildren( Node, Filter, Visited, Results, Db ) : TRUE;
}



/****************************************************************
**	Routine: CollectChildren
**	Returns: TRUE or FALSE
**	Action : Recursively collects the selected child nodes
**			 (building them when necessary)
****************************************************************/

static int CollectChildren(
	SDB_NODE				*Parent,
	SDB_CHILD_ENUM_FILTER	*Filter,
	NodeSet&				Visited,
	HASH					*Results,
	SDB_DB					*Db
)
{
	int		ChildNum,
			ArgNum;

	SDB_NODE_CONTEXT
			Ctx;


	if( !SDB_NODE::IsNode( Parent ) || Parent->IsLiteral() )
		return( Err( ERR_INVALID_ARGUMENTS, "@: CollectChildren, Invalid or literal node" ));

/*
**	Object is a "child"
*/
	if( !SDB_NODE::IsNode( Parent->ObjectGet() ) )
		return( Err( ERR_INVALID_ARGUMENTS, "@: CollectChildren, Invalid Parent Object" ));

	if( !Parent->ObjectGet()->ValidGet() )
	{
		memset( &Ctx, 0, sizeof( Ctx ));
		if( !Parent->ObjectGet()->GetValue( &Ctx, Db ))
			return( FALSE );
	}
	if( !CollectChild( Parent->ObjectGet(), Filter, Visited, Results, Db ))
		return( FALSE );


/*
**	ValueType is a "child"
*/
	if( !SDB_NODE::IsNode( Parent->ValueTypeGet() ) )
		return( Err( ERR_INVALID_ARGUMENTS, "@: CollectChildren, Invalid Parent ValueType" ));

	if( !Parent->ValueTypeGet()->ValidGet() )
	{
		memset( &Ctx, 0, sizeof( Ctx ));
		if( !Parent->ValueTypeGet()->GetValue( &Ctx, Db ))
			return( FALSE );
	}
	if( !CollectChild( Parent->ValueTypeGet(), Filter, Visited, Results, Db ))
		return( FALSE );


	SDB_DB *ChildDb = Parent->ExpectedChildDb( Db );

/*
**	Arguments are each a "child"
*/

	for( ArgNum = 0; ArgNum < Parent->ArgcGet(); ArgNum++ )
		if( !CollectChild( Parent->ArgGet( ArgNum ), Filter, Visited, Results, ChildDb ) && !Parent->PassErrorsGet() )
			return( FALSE );


/*
**	And, of course, children are each a "child"
*/

	if( !Parent->ChildrenValidGet() )
	{
		// Check for recursion here because building children for a diddled node
		// may cause the graph to get in a cycle when you depend upon a diddle scope
		if( !Filter->Recurse( Parent, true ))
			return TRUE;

		// Children are invalid, so force PrimaryDb to be the one we want in case
		// BuildChildren removes the one we are interested in
		if( Parent->DbSetGet()->Db() != Db )
			Parent->DbSetSet( SDB_DBSET_SET_DB_PRIMARY( Parent->DbSetGet(), Db ) );

		SDB_DBSET *OldDbSet = Parent->DbSetGet();
		if( !Parent->BuildChildren( &Ctx ) )
			return FALSE;

		// If BuildChildren cacues us to loose the RootDb from the DbSet, then fail
		// NodeChildren will do the retry
		if( Parent->DbSetGet() != OldDbSet && !Parent->DbSetGet()->Member( Db ) )
		{
			// FIX Add DbChanged error
			Err( SDB_ERR_NODE_DB_CHANGED, "DbSet no longer contains %s while building children", Db->FullDbName.c_str() );

			if( SecDbTraceFunc )
				SecDbTraceFunc( SDB_TRACE_ERROR_NODE );

			throw db_changed( Parent );
		}
	}

	if( !Parent->IsLiteral() )
		for( ChildNum = 0; ChildNum < Parent->ChildcGet(); ChildNum++ )
			if( !CollectChild( Parent->ChildGet( ChildNum ), Filter, Visited, Results, ChildDb ) && !Parent->PassErrorsGet() )
				return( FALSE );

	return( TRUE );
}



/****************************************************************
**	Routine: NodeChildren
**	Returns: TRUE or FALSE
**	Action : Keeps track of children while doing (possibly
**			 modified) GetValue
****************************************************************/

int NodeChildren(
	SDB_NODE				*Node,
	HASH					**Hash,
	SDB_CHILD_ENUM_FILTER	*Filter,
	SDB_DB					*Db
)
{
	int		Status;

	NodeSet Visited( 0, SDB_NODE_INVALID_PTR );

	if( !ChildInfoHeap )
		ChildInfoHeap = GSHeapCreate( "ChildInfoHeap", sizeof( SDB_CHILD_INFO ), 500 );

	if( !*Hash )
	{
		*Hash = HashCreate( "NodeChildren", NodeChildHash, NodeChildCompare, 0, NULL );
		if( !*Hash )
			return( FALSE );
	}

	// Fix: build first level children, CollectChildern now only builds children
	// of the whole tree, ( when SDB_ENUM_TREE is set )
	SDB_NODE_CONTEXT Ctx;
	memset( &Ctx, 0, sizeof( Ctx ));
	if( !Node->ChildrenValidGet() && !Node->BuildChildren( &Ctx ))
		return( FALSE );
	if( !Node->DbSetGet()->Member( Db ) )
		return FALSE;					// BuildChildren lost the Db we are interested in

	try
	{
		Status = CollectChildren( Node, Filter, Visited, *Hash, Db );
	}
	catch( db_changed& )
	{
		Status = FALSE;
	}

	return( Status );
}



/****************************************************************
**	Routine: NodeChildrenHashDestroy
**	Returns: Nothing
**	Action : Deletes the hash table and its contents from a
**			 prior NodeChildren call
****************************************************************/

void NodeChildrenHashDestroy(
	HASH	*Hash
)
{
	HASH_ENTRY_PTR
			Ptr;

	SDB_CHILD_INFO
			*ChildInfo;

	int		ArgNum;


	HASH_FOR( Ptr, Hash )
	{
		ChildInfo = (SDB_CHILD_INFO *) HashKey( Ptr );
		if( ChildInfo->Argc )
		{
			for( ArgNum = 0; ArgNum < ChildInfo->Argc; ArgNum++ )
				DTM_FREE( &ChildInfo->Argv[ ArgNum ] );
			free( ChildInfo->Argv );
		}
		GSHeapFree( ChildInfoHeap, ChildInfo );
	}
	HashDestroy( Hash );
}



/****************************************************************
**	Routine: EnumNext
**	Returns: Next node in iteration
**	Action :
****************************************************************/

static SDB_NODE *EnumNext(
	ENUM_CONTEXT	*Ctx
)
{
	SDB_NODE
			*Node;

	DT_VALUE
			TmpVal;


/*
**	Terminal nodes first
*/

	if( Ctx->EnumFlags & SDB_NODE_ENUM_TERMINALS )
	{
		if( Ctx->first_time )
			Ctx->reset_iterator();

		while( Ctx->iter != SDB_NODE::nodes().end() )
		{
			Node = *Ctx->iter;
			++Ctx->iter;

			if( Node->IsTerminalQuick() )
			{
				if( !Ctx->SecPtr || Node->ObjectGet()->m_Value.Data.Pointer == (void *) Ctx->SecPtr )
					return( Node );
				break;
			}
		}
		Ctx->EnumFlags &= ~SDB_NODE_ENUM_TERMINALS;
		Ctx->first_time = true;
	}


/*
**	Non-terminal nodes next
*/

	if( Ctx->EnumFlags & SDB_NODE_ENUM_NON_TERMINALS )
	{
		if( Ctx->first_time )
			Ctx->reset_iterator();

		while( Ctx->iter != SDB_NODE::nodes().end() )
		{
			Node = *Ctx->iter;
			++Ctx->iter;

			if( !Ctx->SecPtr || ( Node->ObjectGet()->ValidGet() && Node->ObjectGet()->m_Value.Data.Pointer != (void*) Ctx->SecPtr ) )
			{
				if( !Node->IsTerminalQuick() )
					return Node;
			}
			else
				break;
		}
		Ctx->EnumFlags &= ~SDB_NODE_ENUM_NON_TERMINALS;
	}


/*
**	Literal nodes last
*/

	if( Ctx->EnumFlags & SDB_NODE_ENUM_LITERALS )
	{
		if( Ctx->SecPtr )			// Only one literal to find
		{
			Ctx->EnumFlags &= ~SDB_NODE_ENUM_LITERALS;

			DTM_INIT( &TmpVal );
			TmpVal.DataType		= DtSecurity;
			TmpVal.Data.Pointer	= Ctx->SecPtr;
			return( SDB_NODE::FindLiteral( &TmpVal, SecDbRootDb ));
		}

		if( !Ctx->HashEntryPtr )	// First time
			Ctx->HashEntryPtr = HashFirst( SDB_NODE::LiteralHash() );
		else
			Ctx->HashEntryPtr = HashNext( SDB_NODE::LiteralHash(), Ctx->HashEntryPtr );

		if( Ctx->HashEntryPtr )
			return( (SDB_NODE *) HashValue( Ctx->HashEntryPtr ));

		Ctx->EnumFlags &= ~SDB_NODE_ENUM_LITERALS;
	}

	return( NULL );
}



/****************************************************************
**	Routine: SecDbNodeEnumFirst
**	Returns: First selected node from cache
**	Action : Starts iteration through all the selected nodes in
**			 the cache
****************************************************************/

SDB_NODE *SecDbNodeEnumFirst(
	SDB_OBJECT		*SecPtr,		// If non-NULL, select only terminal nodes for this security
	int				EnumFlags,
	SDB_ENUM_PTR	*EnumPtr
)
{
	ENUM_CONTEXT
			*EnumCtx;


	if( !ERR_CALLOC( *EnumPtr, SDB_ENUM_STRUCT, 1, sizeof( **EnumPtr )))
		return( NULL );

	EnumCtx = new ENUM_CONTEXT( SecPtr, EnumFlags );

	(*EnumPtr)->Extra 	= EnumCtx;

	return( EnumNext( (ENUM_CONTEXT *) (*EnumPtr)->Extra ));
}



/****************************************************************
**	Routine: SecDbNodeEnumClose
**	Returns: Nothing
**	Action : Closes an iteration through the nodes in the cache
****************************************************************/

void SecDbNodeEnumClose(
	SDB_ENUM_PTR	EnumPtr
)
{
	ENUM_CONTEXT* Ctx = (ENUM_CONTEXT*) EnumPtr->Extra;
	delete Ctx;
	free( EnumPtr );
}



/****************************************************************
**	Routine: SecDbNodeEnumNext
**	Returns: Next node in iteration
**	Action :
****************************************************************/

SDB_NODE *SecDbNodeEnumNext(
	SDB_ENUM_PTR	EnumPtr
)
{
	return( EnumNext( (ENUM_CONTEXT *) EnumPtr->Extra ));
}
