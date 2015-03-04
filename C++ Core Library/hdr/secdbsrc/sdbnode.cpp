#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbnode.cpp,v 1.32 2012/06/14 15:59:57 khodod Exp $"
/****************************************************************
**
**	sdbnode.cpp	- SDB_NODE implementation
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbnode.cpp,v $
**	Revision 1.32  2012/06/14 15:59:57  khodod
**	Remove statistics-generation dead code.
**	AWR: #177555
**
**	Revision 1.31  2004/11/29 23:48:12  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.30  2004/01/30 23:38:20  khodod
**	Fixed the comparators for large pointer values.
**	
**	Revision 1.29  2003/12/23 21:17:07  khodod
**	BugFix: Null out the VtiNode child of a deleted garbage node.
**	
**	Revision 1.28  2003/12/23 21:13:53  khodod
**	BugFix: Do not return 0 from GetTextI declared as returning a string.
**	
**	Revision 1.27  2001/11/27 23:23:41  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.25  2001/10/02 15:33:26  singhki
**	move object,vt and vti into children. allows us to write better
**	iterators, literals take less space much cooler in general.
**	
**	Revision 1.24  2001/09/24 18:11:50  singhki
**	report num pointers too
**	
**	Revision 1.23  2001/08/20 22:24:25  simpsk
**	various cleanups: added missing #includes, removed deprecated CC_ macros.
**	
**	Revision 1.22  2001/08/07 13:52:59  singhki
**	move stats out of the node
**	
**	Revision 1.21  2001/07/17 17:49:54  singhki
**	split children and parents into separate arrays in SDB_NODE. allows us
**	to build cycles because we can add parents while a node is
**	locked. code is much cleaner too.
**	
**	Revision 1.20  2001/06/21 19:34:26  singhki
**	Completely get rid of remove diddle message. Rewrite side effect
**	diddle maps to actually work. Use it to remove all side effect
**	diddles. fix side effect/phantom interaction with DiddleStackMove.
**	
**	Also remove all diddles during destroy to prevent leaks.
**	
**	Revision 1.19  2001/05/24 18:41:05  singhki
**	make sdb_async_results a pointer and initialize it in SDB_NODE::Initialize because
**	some crappy solaris platforms cannot initialize a semaphore during dll load
**	
**	Revision 1.18  2001/05/16 17:40:07  singhki
**	added rbtree stats for Gnu STL
**	
**	Revision 1.17  2001/04/19 23:24:05  singhki
**	added stats for rbtrees
**	
**	Revision 1.16  2001/03/20 17:17:38  singhki
**	Use InvalidateSafe() to prevent invalidation of set/diddled nodes in internal graph operations
**	
**	Revision 1.15  2001/03/14 18:33:48  singhki
**	Rewrite nodes to use STL style multiset instead of skip list
**	
**	Revision 1.14  2001/02/05 13:50:01  schlae
**	Don't trample interesting errors.
**	
**	Revision 1.13  2001/01/19 19:15:55  shahia
**	Got rid of use of SDB_NODE_CONTEXTs Clear method, OK to PRODVER
**	
**	Revision 1.12  2001/01/09 21:47:26  singhki
**	finally dependencies within a temporary diddle scope are now detected
**	and added to AutoChildren. This is only enabled if you set
**	SDB_AUTO_CHILD_LIST. Should be enabled by DiddleScopeUse in a VF.
**	
**	Since diddle scopes are never merged when diddles are removed, we must
**	compute this information in order to determine the dependencies of a
**	node in a temporary diddle scope. Unfortunately this is never
**	cached. We should look into a way to cache this. Possibly by adding a
**	Merged member to a DbSet...
**	
**	Revision 1.11  2001/01/08 20:10:45  singhki
**	Separate out tracking the previous node from actually collecting auto
**	children (prepare to collect temporary diddle scope children).
**	
**	Revision 1.10  2000/12/05 16:48:29  singhki
**	Added Subscribe capability to nodes
**	
**	Revision 1.9  2000/08/23 01:07:18  singhki
**	Remove destroyed nodes from skiplist/literal hash when they are
**	marked. This prevents them from being found and referenced during
**	delayed destruction thus making the graph point to freed nodes.
**	
**	Also perpetuate hideous hack where literals have an implicit reference
**	to the secptr by making the secptr get freed only when its ref count
**	is -1. This should really fixed
**	
**	Revision 1.8  2000/08/04 11:51:27  shahia
**	Bug Fix: for large memory leak of GsNodeValues for Literal Hash
**	
**	Revision 1.7  2000/05/26 23:58:50  vengrd
**	added GetDepth variable
**	
**	Revision 1.6  2000/05/17 01:53:51  singhki
**	do not append value if doing get
**	
**	Revision 1.5  2000/05/03 15:20:00  singhki
**	Fix ValueArray & Each interaction. Add Value Array Eval() to terminate
**	ValueArray expansion.
**	
**	Revision 1.4  2000/05/01 20:30:47  singhki
**	Use SlangScopeFunctionAlloc in PushVariableHash too
**	
**	Revision 1.3  2000/04/28 11:14:36  singhki
**	remove unused var and fix warnings
**	
**	Revision 1.2  2000/04/28 11:02:31  vengrd
**	Use GsString in GetText. Make secdb depend upon gsbase
**	
**	Revision 1.1  2000/04/24 11:04:27  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**
****************************************************************/

#define BUILDING_SECDB
#include <portable.h>
#include <secnode.h>
#include <secdb/async_result.h>

#ifdef SDB_NODE_NO_INLINE
#include "sdbn_inl.h"
#endif

#include <string>
CC_USING( std::string );

//////////////////////////////////////////////////////////////////
// Static Members of SDB_NODE
//////////////////////////////////////////////////////////////////

HASH	*SDB_NODE::s_InvParentsHash = 0;
bool	SDB_NODE::s_ReleaseValueWhenParentsValidDefault = false;

SDB_NODE
		*SDB_NODE::s_CurrentAutoChildListNode = NULL,
		*SDB_NODE::s_ValueArrayEvalVT = NULL;

bool	SDB_NODE::s_CollectAutoChildren = false;

int
		SDB_NODE::s_GetDepth = 0;

SDB_VECTOR_CONTEXT
		*SDB_NODE::s_VectorContext = NULL;

HEAP	*SDB_NODE::s_NodeHeap = NULL;

HASH	*SDB_NODE::s_DiddleHash = NULL,
		*SDB_NODE::s_LiteralHash = NULL,
		*SDB_NODE::s_PhantomDiddleHash = NULL;

SDB_NODE::AutoChildrenMap
		SDB_NODE::s_AutoChildrenMap;

SDB_NODE::SideEffectDiddleMap
		SDB_NODE::s_side_effect_diddles;

SDB_NODE::nodes_map
		SDB_NODE::s_nodes;

SDB_OBJECT
		SDB_NODE::s_NonTerminalObject;

SDB_NODE::NodeSet*
		SDB_NODE::s_changedNodes = 0;

int		SDB_NODE::s_Parentc[ SDB_NODE::NODE_MAX_PARENTC + 1 ] =
{
	0,									// 0
	1,									// 1
	2,									// 2
	3,									// 3
	4,									// 4
	6,									// 5
	8,									// 6

	1 << 4,								// 7 is SDB_NODE::MinParentIndexForHash so the following *must*
										// be powers of 2

	1 << 5,								// 8
	1 << 6,								// 9
	1 << 7,								// 10
	1 << 8,								// 11
	1 << 9, 							// 12
	1 << 10,							// 13
	1 << 11,							// 14
	1 << 12,							// 15
	1 << 13,							// 16
	1 << 14,							// 17
	1 << 15,							// 18
	1 << 16,							// 19
	1 << 17,							// 20
	1 << 18,							// 21
	1 << 19,							// 22
	1 << 20,							// 23
	1 << 21,							// 24
	1 << 22,							// 25
	1 << 23,							// 26
	1 << 24,							// 27
	1 << 25,							// 28
	1 << 26,							// 29
	1 << 27,							// 30
	1 << 28								// 31
};

bool					SDB_NODE::s_stats_enabled = false;
SDB_NODE::stats_map_t	SDB_NODE::s_stats_map;


/*
**	Constants
*/

#define	NODE_BLOCK_SIZE	10920


/****************************************************************
**	Routine: InvParentsHashInit
**	Returns: TRUE/FALSE
**	Action : Creates an empty Hash object (s_InvParentsHash)
****************************************************************/

int SDB_NODE::InvParentsHashInit()
{
	if( s_InvParentsHash )
		return TRUE;

	s_InvParentsHash = HashCreate( InvParentsHashName(), NULL, NULL, 0, NULL );
	return s_InvParentsHash ? TRUE : FALSE;
}

/****************************************************************
**	Routine: InvParentsHashDestroy
**	Returns: nothing
**	Action : Destroys s_InvParentsHash
****************************************************************/

void SDB_NODE::InvParentsHashDestroy()
{
	HashDestroy( s_InvParentsHash );
}

/****************************************************************
**	Routine: InvParentsHashSet
**	Returns: nothing
**	Action : Inserts Child in the Hash (s_InvParentsHash) if
**           it's not already there and sets its value to Num.
****************************************************************/

void SDB_NODE::InvParentsHashSet( SDB_NODE *Child, int Num )
{
	// assert( Child );
	HASH_ENTRY_PTR	ep = HashLookupPtr( s_InvParentsHash, (HASH_KEY) Child );
	if( !ep )
		HashInsert( s_InvParentsHash, (HASH_KEY) Child, (HASH_VALUE) Num );
	else
		ep->Value = (HASH_VALUE) Num;

	if( SecDbTraceFunc )
		SecDbTraceFunc( SDB_TRACE_NUM_INV_PARENTS_SET, Child, Child, Num );
}

/****************************************************************
**	Routine: SDB_NODE::VtiNodeSet
**	Returns: void
**	Action : Sets the VTI node and updates parent links
****************************************************************/

void SDB_NODE::VtiNodeSet(
	SDB_NODE	*Vti
)
{
	SDB_NODE_CONTEXT
			Ctx;

	SDB_NODE* const vti_node = VtiNodeGet();
	if( vti_node == Vti )
		return;

	// If Node is valid, we may be setting the VTI in response to a split/forced create in the base
	// scope. In this case if the node's VTI will not change, so we don't invalidate it.
	// Really one should force the user to specify the VTI nodes before hand, but this is assumes too
	// much knowledge about the underlying graph, and this seems an acceptable and safe compromise.
	if( !( ValidGet() && vti_node->GetValue( &Ctx, vti_node->DbSetGet()->Db() )
		   && Vti->GetValue( &Ctx, Vti->DbSetGet()->Db() )
		   && vti_node->AccessValue()->Data.Pointer == Vti->AccessValue()->Data.Pointer ))
	{
		InvalidateSafe();
		if( ChildrenValidGet() )
			InvalidateChildren();
	}

	Vti->ParentLink( SDB_NODE_PURPLE_PARENT( this ));
	if( vti_node )
		vti_node->ParentUnlink( SDB_NODE_PURPLE_PARENT( this ));

	VtiNodeGetRef() = SDB_NODE_PURPLE_CHILD( Vti );
}

/****************************************************************
**	Routine: SDB_NODE::VectorContextAccessValue
**	Returns: Get correct vector context sub value
**	Action :
****************************************************************/

DT_VALUE *SDB_NODE::VectorContextAccessValue(
	DT_VALUE	*SubValue
)
{
	if( SubValue && SubValue->DataType == DtValueArray )
	{
		DT_VALUE_ARRAY
				*ValueArray = (DT_VALUE_ARRAY *) SubValue->Data.Pointer;

		int		Index = s_VectorContext->Index;

		if( Index >= 0 && Index <= ValueArray->Size )
		{
			SubValue = &ValueArray->Element[ Index ];
			if( SubValue->DataType == DtNull )
				SubValue = &ValueArray->Element[ Index > 0 ? Index - 1 : 0 ];
		}
		else
		{
			Err( ERR_INVALID_ARGUMENTS, "VectorContextAccessValue, vector index[%d] out of range [0..%d]",
				 Index, ValueArray->Size-1 );
			return NULL;
		}
	}
	return SubValue;
}


/****************************************************************
**	Routine: SDB_NODE::FreeLiteral
**	Returns: void
**	Action : Frees a literal node
****************************************************************/

void SDB_NODE::FreeLiteral(
	SDB_NODE	*Literal
)
{
	if( !Literal )
		return;

	if( s_stats_enabled )
		s_stats_map.erase( Literal );

	// If freed as a result of ref count -> 0 then DestroyFlags will be 0 so we need to delete it from the literal hash
	if( Literal->DestroyFlagsGet() != DESTROYED )
		HashDelete( s_LiteralHash, (HASH_KEY) &Literal->m_Value );
	if( Literal->m_Value.DataType == DtSecurity )
	{
		SDB_OBJECT *Obj = (SDB_OBJECT *) Literal->m_Value.Data.Pointer;
		--Obj->ReferenceCount;
		Obj->Free();
	}
	else if( Literal->m_Value.DataType == DtDiddleScope )
		free( (void *) Literal->m_Value.Data.Pointer );
	else if( !( Literal->CacheFlagsGet() & SDB_CACHE_NO_FREE ))
		DTM_FREE( &Literal->m_Value );

	if( Literal->RefCountedParents() )
		Literal->m_parents = NULL;

	Free( Literal );
	return;
}

/****************************************************************
**	Routine: SDB_NODE::InsertHead
**	Returns: void
**	Action : Inserts node at head
****************************************************************/

void SDB_NODE::InsertHead(
	SDB_OBJECT	*Object
)
{
	NODE_LRU_POINTERS
			*Curr = LruPointers(),
			*Prev,
			*Next;

	++Object->NumCachedNodes;
	// Special case, list is empty
	if( !Object->NodeCache )
	{
		Curr->Next = this;
		Curr->Prev = this;
		Object->NodeCache = this;
		return;
	}
	Next = Object->NodeCache->LruPointers();
	Prev = Next->Prev->LruPointers();

	Curr->Prev = Next->Prev;
	Curr->Next = Object->NodeCache;

	Next->Prev = this;
	Prev->Next = this;
	Object->NodeCache = this;
}



/****************************************************************
**	Routine: SDB_NODE::RemoveNode
**	Returns: SDB_NODE *
**	Action : Removes a node from the linked list and returns it
****************************************************************/

SDB_NODE *SDB_NODE::RemoveNode(
	SDB_OBJECT	*Object
)
{
	NODE_LRU_POINTERS
			*Curr = LruPointers(),
			*Prev,
			*Next;

	--Object->NumCachedNodes;
	// Special case, list is one element
	if( Curr->Next == this )
	{
		Object->NodeCache = NULL;
		return this;
	}

	Next = Curr->Next->LruPointers();
	Prev = Curr->Prev->LruPointers();
	Next->Prev = Curr->Prev;
	Prev->Next = Curr->Next;

	// special case, removing head
	if( this == Object->NodeCache )
		Object->NodeCache = Curr->Next;

	return this;
}



/****************************************************************
**	Routine: SDB_NODE::LRUCacheByObjectDelete
**	Returns: void
**	Action : deletes a node from the LRUcache by object
****************************************************************/

void SDB_NODE::LRUCacheByObjectDelete(
)
{
	SDB_OBJECT
			*NodeObject;

	DT_VALUE
			*ObjectValue = ObjectGet()->AccessValue();

	if( !ObjectValue || ObjectValue->DataType != DtSecurity )
		NodeObject = &s_NonTerminalObject;
	else
		NodeObject = (SDB_OBJECT *) ObjectValue->Data.Pointer;

	RemoveNode( NodeObject );
	TakeOffLRUCache();
}



/****************************************************************
**	Routine: SDB_NODE::LRUCacheByObjectInsert
**	Returns: void
**	Action : Inserts node into the LRUCache by object
****************************************************************/

void SDB_NODE::LRUCacheByObjectInsert(
)
{
	static int
			DontUseLRUCache = -1;

	if( DontUseLRUCache == -1)
	{
		ERR_OFF();
		DontUseLRUCache = (stricmp( SecDbConfigurationGet( "SECDB_USE_LRU_CACHE", NULL, NULL, "True" ), "True" ) != 0);
		ERR_ON();
	}
	if( DontUseLRUCache )
		return;

	if( !CandidateForLRUCache() || !ObjectGet()->IsLiteral() || OnLRUCacheGet() )
		return;

	DT_VALUE
			*ObjectValue = ObjectGet()->AccessValue();
	SDB_OBJECT
			*Object;

	if( ObjectValue->DataType != DtSecurity )
		Object = &s_NonTerminalObject;
	else if( !( Object = (SDB_OBJECT *) ObjectValue->Data.Pointer ))
		return;

	// mark the node as being on the LRUCache in order to prevent
	// recursive cache insertions during the next phase from attempting
	// to put it on the cache again
	PutOnLRUCache();
	SetOrDiddleLockSet( true );			// mark as locked so that we won't try taking it off the LRU cache if it does get destroyed

	// Flush the cache (if necessary) before putting the node on it
	// because it may cause recursive flushes which would throw out the node
	if( Object->NumCachedNodes >= Object->Class->MaxUnparentedNodes )
	{
		int		Token = 0;

		Destroy( NULL, &Token );
		for( int i = Object->Class->GCBlockSize; i > 0 && Object->NodeCache; --i )
		{
			bool	ConsiderDestroyVti;

			SDB_NODE
					*GarbageNode = Object->NodeCache->LruPointers()->Prev->RemoveNode( Object );
			GarbageNode->OnLRUCacheSet( false );

			// check to see if node we are throwing away has come in use
			if( !GarbageNode->CandidateForLRUCache()
				|| ( ( ConsiderDestroyVti = 
					   GarbageNode->VtiNodeGet() && (((SDB_VALUE_TYPE) GarbageNode->VtiNodeGet()->ValueTypeGet()->m_Value.Data.Pointer)->ID == SecDbValueValueFunc->ID ))
					 && !GarbageNode->VtiNodeGet()->CandidateForLRUCache() ) )
				GarbageNode->TakeOffLRUCache();
			else
			{
				// Do fast version of TakeOffLRUCache because
				// we are going to destroy the node
				GarbageNode->OnLRUCacheSet( false );
				if( ConsiderDestroyVti && GarbageNode->VtiNodeGet()->ParentsNumCalc() == 1 )
				{
					Destroy( GarbageNode->VtiNodeGet(), NULL );
					GarbageNode->VtiNodeGetRef() = 0;
				}
				Destroy( GarbageNode, NULL );
			}
		}
		Destroy( NULL, &Token );
	}

	SetOrDiddleLockSet( false );
	if( DestroyedGet() )
		Free( this );
	else
		// Now that the cache is clean, insert the node in the cache
		InsertHead( Object );
}


/****************************************************************
**	Routine: SDB_NODE::Initialize
**	Returns: 
**	Action : 
****************************************************************/

bool SDB_NODE::Initialize(
)
{
/*
**	Heaps, hashes, skiplists
*/

	if( !( s_NodeHeap = GSHeapCreate( "NodeHeap", sizeof( SDB_NODE ), NODE_BLOCK_SIZE ))
		|| !( s_LiteralHash = HashCreate( "LiteralHash", LiteralHashFunc, LiteralHashCmp, 1000, NULL ))
		|| !( s_DiddleHash = HashCreate( "NodeDiddleHash", NULL, NULL, 0, NULL ))
		|| !( s_PhantomDiddleHash = HashCreate( "PhantomDiddleHash", NULL, NULL, 0, NULL ))
		|| !  SDB_NODE::InvParentsHashInit())
		return false;

	sdb_async_results = new Sdb_Async_Results; // initialize async result queue

	s_ReleaseValueWhenParentsValidDefault = stricmp( SecDbConfigurationGet( "SECDB_RELEASE_VALUE_WHEN_PARENTS_VALID", 0, 0, "FALSE" ), "True" ) == 0;

	s_changedNodes = new NodeSet;

	s_stats_enabled = SecDbConfigurationGet( "SECDB_NODE_PERFORMANCE_STATS", 0, 0, 0 ) != 0;

	return true;
}


/****************************************************************
**	Routine: SDB_NODE::InitializeMore
**	Returns: true/false
**	Action : Called after all secdb tables have been loaded
****************************************************************/

bool SDB_NODE::InitializeMore(
)
{
	memset( &s_NonTerminalObject, 0, sizeof( s_NonTerminalObject ));
	strcpy( s_NonTerminalObject.SecData.Name, "~NonTerminal" );
	s_NonTerminalObject.ReferenceCount = 1;
	s_NonTerminalObject.SdbClass = s_NonTerminalObject.Class = SecDbClassInfoLookup( SecDbClassTypeFromName( "SecDb Base Class" ));

	s_ValueArrayEvalVT = LinkLiteral( DtValueType, SecDbValueTypeFromName( "Value Array Eval", DtArray ));

	return true;
}


/****************************************************************
**	Routine: SDB_NODE::LiteralHashFunc
**	Returns: Hash for literal node
**	Action : hash function for a literal value
****************************************************************/

HASH_BUCKET SDB_NODE::LiteralHashFunc(
	HASH_KEY	Key
)
{
	DT_VALUE
			*Value = (DT_VALUE *) Key,
			TmpVal;

	DT_HASH_CODE
			Hash = 0;

	TmpVal.Data.Pointer = &Hash;
	if( !DT_OP( DT_MSG_HASH_QUICK, &TmpVal, Value, NULL ))
	{
		Hash = 0;
		if( !DtHashPortable( Value, &Hash )) // if even this fails, then in desperation
		{								// convert the value to a string and hash that
			TmpVal.DataType = DtString;
			DTM_TO( &TmpVal, Value );
			Hash = DtHashQuick( (unsigned char *) TmpVal.Data.Pointer, strlen( (char *) TmpVal.Data.Pointer ), 0 );
			DTM_FREE( &TmpVal );
		}
	}
	return (HASH_BUCKET) Hash;
}



/****************************************************************
**	Routine: SDB_NODE::LiteralHashCmp
**	Returns: 0 if equal, 1 otherwise
**	Action : KeyA == KeyB
****************************************************************/

int SDB_NODE::LiteralHashCmp(
	HASH_KEY	KeyA,
	HASH_KEY	KeyB
)
{
	DT_VALUE
			*ValueA	= (DT_VALUE *) KeyA,
			*ValueB	= (DT_VALUE *) KeyB,
			Equal;


	if( ValueA->DataType == ValueB->DataType )
	{
		if( ValueA->DataType->Flags & DT_FLAG_REFERENCE )
			return( ValueA->Data.Pointer != ValueB->Data.Pointer );

		if( ValueA->DataType->Flags & DT_FLAG_NUMERIC )
		{
			if( ValueA->DataType == DtNull )
				return( 0 );
			return( ValueA->Data.Number != ValueB->Data.Number );
		}

		if( DT_OP( DT_MSG_EQUAL, &Equal, ValueA, ValueB ) && Equal.Data.Number != 0 )
			return( 0 );
	}

	else if ((ValueA->DataType == DtGsNodeValues) && (ValueB->DataType == DtArray))
	{
		if( DT_OP( DT_MSG_EQUAL, &Equal, ValueA, ValueB ) && Equal.Data.Number != 0 )
			return( 0 );
	}
		 
	else if ((ValueA->DataType == DtArray) && (ValueB->DataType == DtGsNodeValues))
	{
		if( DT_OP( DT_MSG_EQUAL, &Equal, ValueB, ValueA ) && Equal.Data.Number != 0 )
			return( 0 );
	}
	return( 1 );
}



/****************************************************************
**	Routine: SDB_NODE::DiddleIdCompare
**	Returns: <0, 0, >0 ala strcmp
**	Action : compares two diddle ids for the skip list ordering
****************************************************************/

int SDB_NODE::DiddleIdCompare(
	const void	*Key1,
	const void	*Key2
)
{
	return (SDB_DIDDLE_ID) Key1 - (SDB_DIDDLE_ID) Key2;
}



/****************************************************************
**	Routine: SDB_NODE::Compare
**	Returns: <, >, == 0 ala strcmp
**	Action : Compares two nodes
**				1) Node->Object
**				2) Node->ValueType
**				3) Node->Argc
**			    4) Node->Argv[i] for each i from 0 to Node->Argc
**			 Comparisons are all simple pointer comparisons since
**			 each node is uniquely represented.
****************************************************************/

int SDB_NODE::Compare(
	const void	*KeyA,
	const void	*KeyB
)
{
	SDB_NODE
			*NodeA	= (SDB_NODE *) KeyA,
			*NodeB	= (SDB_NODE *) KeyB;

	int		ArgNum;

	long	r;

	//
	// Assumes (unsigned long) Ptr is isomorphic to (void *) Ptr,
	// which seems a better assumption than being able to compare any two pointers.
	//
	// Enumeration assumes NULL < non-NULL and that sort if first by Object

#define CMP(ptr1, ptr2)                                      \
        {                                                    \
            unsigned long val1 = (unsigned long) ptr1;       \
            unsigned long val2 = (unsigned long) ptr2;       \
                                                             \
            if( val1 < val2 ) return -1;			         \
            else if( val1 > val2 ) return 1;	             \
        }                                         

    CMP( NodeA->ObjectGet(), NodeB->ObjectGet() );
    CMP( NodeA->ValueTypeGet(), NodeB->ValueTypeGet() );

    if( ( r = NodeA->ArgcGet() - NodeB->ArgcGet() ) )
		return (int) r;
	else for( ArgNum = 0; ArgNum < NodeA->ArgcGet(); ArgNum++ )
        CMP( NodeA->ArgGet( ArgNum ), NodeB->ArgGet( ArgNum ) );
  
#undef CMP
	return 0; //( DB_SET::Compare( (HASH_KEY) NodeA->DbSet, (HASH_KEY) NodeB->DbSet ));
}

/****************************************************************
**	Routine: SDB_NODE::Statistics
**	Returns: true/Err
**	Action : returns various  statistics about the data structures
****************************************************************/

bool SDB_NODE::Statistics(
	DT_VALUE	*Result
)
{
	DT_VALUE
			TmpVal,
			nodes_stats,
			LiteralHashStats;

	size_t	num_pointers = 0;

	DTM_ALLOC( Result, DtStructure );
	DTM_ALLOC( &nodes_stats, DtStructure );

	TmpVal.DataType = DtString;
	TmpVal.Data.Pointer = (void *) "MapStatistics";
	DtComponentReplace( Result, &TmpVal, &nodes_stats );

	DtComponentSetNumber( Result, "NumNodes",			DtDouble,	nodes().size() );

	SDB_ENUM_PTR
			EnumPtr;

	int		NumTerminals = 0,
			NumNonTerminals = 0;

	for( SDB_NODE *Node = SecDbNodeEnumFirst( NULL, SDB_NODE_ENUM_TERMINALS | SDB_NODE_ENUM_NON_TERMINALS, &EnumPtr );
		 Node;
		 Node = SecDbNodeEnumNext( EnumPtr ))
	{
		if( Node->IsTerminal() )
			++NumTerminals;
		else
			++NumNonTerminals;
		num_pointers += Node->child_list_size() + Node->parent_list_size();
	}
	SecDbNodeEnumClose( EnumPtr );

	DtComponentSetNumber( Result, "NumTerminals",		DtDouble,	NumTerminals );
	DtComponentSetNumber( Result, "NumNonTerminals",	DtDouble,	NumNonTerminals );
	DtComponentSetNumber( Result, "NumLiterals",		DtDouble,	s_LiteralHash->KeyCount );
	DtComponentSetNumber( Result, "NumPointers",		DtDouble,	num_pointers );

	// Output node literal hash statistics
	HASH_ENTRY_PTR
			HashPtr	= DtHashStatsToStruct( s_LiteralHash, &LiteralHashStats );
	DtComponentSetPointer( Result, "LiteralHash", DtStructure, LiteralHashStats.Data.Pointer );

	DTM_ALLOC( &TmpVal, DtArray );
	if( HashPtr )
	{
		while( HashPtr->Status != HASH_EMPTY )
		{
			if( HashPtr->Status == HASH_IN_USE )
			{
				DT_VALUE
						*Value = (DT_VALUE *) HashKey( HashPtr );

				DT_VALUE
						Elem;

				DTM_ALLOC( &Elem, DtStructure );
				// Special check for Security because it may be in the literal hash table yet in the dead pool
				// this would cause a bogus reference so we just insert it as a string
				if( Value->DataType == DtSecurity )
					DtComponentSetPointer( &Elem, "Key", DtString, ((SDB_OBJECT *) Result->Data.Pointer)->SecData.Name );
				else
					DtComponentSet( &Elem, "Key", Value );
				DtComponentSetNumber( &Elem, "Hash", DtDouble, HashPtr->Bucket );
				DtAppendNoCopy( &TmpVal, &Elem );
			}
		    ++HashPtr;
			if( HashPtr > s_LiteralHash->LastBucket )
				HashPtr = s_LiteralHash->Table;
		}
	}
	DT_COMPONENT_GIVE( Result, "LongestChain", &TmpVal );
	return true;
}


/****************************************************************
**	Routine: SDB_NODE::DiddleList
**	Returns: true/Err
**	Action : returns a list of all extant diddles
****************************************************************/

bool SDB_NODE::DiddleList(
	DT_VALUE	*Result,
	SDB_OBJECT	*SecPtr,
	bool		IncludeArgs
)
{
	SDB_OBJECT
			*DiddleSecPtr;

	DT_VALUE
			DiddleList,
			DiddleItem,
			TmpValue;

	HASH_ENTRY_PTR
			HashPtr;

	SDB_DIDDLE_HEADER
			*DiddleHeader;

	SDB_DIDDLE
			*Diddle;

	DTM_ALLOC( Result, DtArray );
	DTM_ALLOC( &TmpValue, DtStructure );

	if( s_DiddleHash )
		HASH_FOR( HashPtr, s_DiddleHash )
		{
			DiddleHeader = (SDB_DIDDLE_HEADER *) HashValue( HashPtr );
			DiddleSecPtr = (SDB_OBJECT *) DiddleHeader->Node->ObjectGet()->AccessValue()->Data.Pointer;
			if( !SecPtr || SecPtr == DiddleSecPtr )
			{
				DTM_ALLOC( &DiddleList, DtArray );
				DTM_ALLOC( &DiddleItem, DtStructure );
				for( Diddle = DiddleHeader->DiddleList; Diddle; Diddle = Diddle->Next )
				{
					DtComponentSet( &DiddleItem, "Value", &Diddle->Value );
					DtComponentSetNumber( &DiddleItem, "Phantom",		DtDouble, Diddle->Phantom		);
					DtComponentSetNumber( &DiddleItem, "SetValue",		DtDouble, Diddle->SetValue		);
					DtComponentSetNumber( &DiddleItem, "DiddleID",		DtDouble, Diddle->DiddleID		);
					DtComponentSetNumber( &DiddleItem, "CacheFlags",	DtDouble, Diddle->CacheFlags	);
					DtAppend( &DiddleList, &DiddleItem );
				}
				DtComponentSet( &TmpValue, "DiddleList", &DiddleList );
				DTM_FREE( &DiddleItem );
				DTM_FREE( &DiddleList );

				DtComponentSetPointer(	&TmpValue, "Object",	DtSecurity,		DiddleSecPtr );
				DtComponentSetPointer(	&TmpValue, "ValueType",	DtValueType, 	DiddleHeader->Node->ValueTypeGet()->AccessValue()->Data.Pointer );
				if( DiddleHeader->Node->ValidGet() )
					DtComponentSet(			&TmpValue, "Value",	DiddleHeader->Node->AccessValue() );
				else
					DtComponentSetPointer(	&TmpValue, "Value", DtNull, NULL );
				if( IncludeArgs )
				{
					DT_VALUE
							Args;

					int		ArgNum;

					DTM_ALLOC( &Args, DtArray );
					for( ArgNum = 0; ArgNum < DiddleHeader->Node->ArgcGet(); ++ArgNum )
					{
						if( DiddleHeader->Node->ArgGet( ArgNum )->ValidGet() )
							DtAppend( &Args, DiddleHeader->Node->ArgGet( ArgNum )->AccessValue() );
						else
							DtAppendPointer( &Args, DtNull, NULL );
					}
					DtComponentSet( &TmpValue, "Args", &Args );
					DTM_FREE( &Args );
				}
				DtAppend( Result, &TmpValue );
			}
		}
	DTM_FREE( &TmpValue );
	return true;
}

/****************************************************************
**	Routine: AppendTrunc
**	Returns: true if truncated / false otherwise
**	Action : Appends string but only if the size doesn't become too big
****************************************************************/

bool AppendTrunc(
	string			&Str,
	const string	&Suffix
)
{
	const unsigned
			MaxSize = 1024;

	if( Str.size() >= MaxSize )
	{
		Str.append( " ..." );
		return true;
	}
	if( Str.size() + Suffix.size() < MaxSize )
	{
		Str += Suffix;
		return false;
	}

	Str.append( Suffix.c_str(), MaxSize - Str.size() );
	Str.append( " ..." );
	return true;
}

/****************************************************************
**	Routine: AppendTrunc
**	Returns: true if truncated / false otherwise
**	Action : Appends string but only if the size doesn't become too big
****************************************************************/

bool AppendTrunc(
	string          	&Str,
	const char			*Suffix
)
{
	const unsigned
			MaxSize = 1024;

	if( Str.size() >= MaxSize )
	{
		Str.append( " ..." );
		return true;
	}
	if( Str.size() + strlen( Suffix ) < MaxSize )
	{
		Str += Suffix;
		return false;
	}

	Str.append( Suffix, MaxSize - Str.size() );
	Str.append( " ..." );
	return true;
}

/****************************************************************
**	Routine: SDB_NODE::GetText
**	Returns: Number of bytes written to Buffer
**	Action : Writes a description of the given node in the buffer
****************************************************************/

string SDB_NODE::GetTextI(
	int				AppendValue
)
{
	int		ArgNum;

	DT_VALUE
			*Value,
			StringVal;

	string
			Str;

	bool	Truncated = false;

	if( !IsNode( this ))
		return Str;

	Str.reserve( 40 );

	if( DiddleGet() )
		Str += '+';

	if( IsLiteralQuick() )				// Literal
	{
		StringVal.DataType = DtString;
		if( !DTM_TO( &StringVal, &m_Value ))
		{
			Str += "???";
			return Str;
		}
		Str += (char *) StringVal.Data.Pointer;
		DTM_FREE( &StringVal );
		return Str;
	}

	if( !ValueTypeGet()->IsLiteral() )
		Str += '(';
	Truncated = Truncated || AppendTrunc( Str, ValueTypeGet()->GetTextI( FALSE ));
	if( !ValueTypeGet()->IsLiteral() )
		Str += ')';
	Str += "( ";
	Truncated = Truncated || AppendTrunc( Str, ObjectGet()->GetTextI( FALSE ));

	if( ArgcGet() && !Truncated )
		for( ArgNum = 0; ArgNum < ArgcGet() && !Truncated; ArgNum++ )
		{
			Str += ", ";
			Truncated = AppendTrunc( Str, ArgGet( ArgNum )->GetTextI( FALSE ));
		}

	if( Str[ Str.size() - 1 ] == ')' )
		Str += ')';
	else
		Str += " )";

	if( AppendValue )
	{
		Str += " = ";
		if( ValidGet() && !GetValueLockGet() ) // GetValue falsely marks a node as valid during the get
		{
			Value = &m_Value;
			if( Value->DataType == DtString )
				AppendTrunc( Str, (char *) Value->Data.Pointer );
			else if( Value->DataType->Flags & DT_FLAG_NUMERIC )
			{
				StringVal.DataType = DtString;
				if( DTM_TO( &StringVal, Value ))
				{
					AppendTrunc( Str, (char *) StringVal.Data.Pointer );
					DTM_FREE( &StringVal );
				}
			}
			else
			{
				Str += '<';
				Str += Value->DataType->Name;
				Str += '>';
			}
		}
		else
			Str += "<invalid>";
	}
	return Str;
}



/****************************************************************
**	Routine: SDB_NODE::GetText
**	Returns: First arg if successful
**	Action : Writes a description of the given node in the buffer
****************************************************************/

string SDB_NODE::GetText(
	int				AppendValue
)
{
	ERR_OFF();
	string str = GetTextI( AppendValue );
	ERR_ON();

	// change newlines into slashes
	for( int pos = 0; ( pos = str.find( '\n', pos )) != -1; )
		str[ pos ] = '/';

	return( str );
}


/****************************************************************
**	Routine: SDB_NODE::Error
**	Returns: FALSE
**	Action : Sets ErrMore with Text( Nodedesc )
****************************************************************/

int SDB_NODE::Error(
	const char	*Tag
)
{
	return( ErrMore( "%s: %s", Tag, GetText( FALSE ).c_str() ));
}


/****************************************************************
**	Routine: FreeNodev
**	Returns: TRUE
**	Action : Frees nodev array for each node
****************************************************************/

static int FreeNodev(
	 void	*Element,
	 void	* //FuncArg
)
{
 	SDB_NODE
            *Node = (SDB_NODE *) Element;


    if( Node->ValidGet() && !( Node->CacheFlagsGet() & SDB_CACHE_NO_FREE ))
        DTM_FREE( &Node->m_Value );
    free( Node->m_children );
    free( Node->m_parents );
 	return( TRUE );
}


/****************************************************************
**	Routine: SDB_NODE::Cleanup
**	Returns: 
**	Action : 
****************************************************************/

void SDB_NODE::Cleanup(
)
{
	GSHeapWalk( s_NodeHeap, FreeNodev, NULL );
	GSHeapDestroy( s_NodeHeap );
	HashDestroy( s_LiteralHash );
	//SkipLikoDestroy( s_NodeSkipLiko );
	if( s_DiddleHash )
	{
		HASH_ENTRY_PTR
				HashPtr;

		HASH_FOR( HashPtr, s_DiddleHash )
		{
			SDB_DIDDLE_HEADER
					*DiddleHeader = (SDB_DIDDLE_HEADER *) HashValue( HashPtr );

			SDB_DIDDLE
					*Diddle;
			while(( Diddle = DiddleHeader->DiddleList ))
			{
				DiddleHeader->DiddleList = Diddle->Next;
				free( Diddle );
			}
			free( DiddleHeader );
		}
		HashDestroy( s_DiddleHash );
		s_DiddleHash = NULL;
	}
	SDB_NODE::InvParentsHashDestroy();
}
