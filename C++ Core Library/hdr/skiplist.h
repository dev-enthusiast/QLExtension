/* $Header: /home/cvs/src/skiplist/src/skiplist.h,v 1.11 2004/11/10 22:19:33 khodod Exp $ */
/****************************************************************
**
**	SKIPLIST.H	- Skip List API
**
**	$Revision: 1.11 $
**
****************************************************************/

#if !defined( IN_SKIPLIST_H )
#define IN_SKIPLIST_H

/*
**	Define constants
*/

#define	SKIP_LIST_DUPE_ALLOW			1
#define	SKIP_LIST_DUPE_REJECT			2
#define	SKIP_LIST_DUPE_OVERWRITE		3
										
#define	SKIP_LIST_DEFAULT_MAX_LEVEL		15
#define	SKIP_LIST_DEFAULT_BLOCK_SIZE	4000

#define	SKIP_LIST_ERR_NODE_ORDER		1
#define	SKIP_LIST_ERR_NODE_COUNT		2


/*
**	Function pointers
*/

typedef int					(*SKIP_LIST_CMP)( const void *Key1, const void *Key2 );
typedef	void				(*SKIP_LIST_FREE_VALUE)( void *Value );
#define SKIP_LIST_STRICMP	((SKIP_LIST_CMP)stricmp)


/*
**	Structure used to hold heap block for node allocation
*/

typedef struct SkipListHeapBlockStructure
{
	struct SkipListHeapBlockStructure
			*Next;

	int		Size;

} SKIP_LIST_HEAP_BLOCK;


/*
**	Structure to hold a node in the list
*/

typedef struct SkipListNodeStructure
{
    void	*Key,
    		*Value;

    struct SkipListNodeStructure
    		*Forward[1];  	// variable sized array of forward pointers

} SKIP_LIST_NODE;


/*
**	Structure to hold skip list statistics
*/

typedef struct SkipListStatsStructure
{
	long	Levels,			// Levels allocated in NodesUsed & NodesFree
			TotalNodesUsed,	// Total nodes in use
			TotalNodesFree;	// Total nodes on free lists

	long	*NodesUsed,		// [ Levels ] - Nodes in use by level
			*NodesFree;		// [ Levels ] - Nodes free by level

	long	BlockMemory;	// Total memory in NodeHeapList

	long	MaxLength,		// Greatest length to any node
			TotalLength;	// Sum of all lengths to each node

} SKIP_LIST_STATS;


/*
**	Structure to hold a skip list 
*/

typedef struct SkipListStructure
{
	int		Level,			// 1 more than the highest level in the list
			MaxLevel,		// Maximum number of levels
			HeapBlockSize;	// Allocation size for each heap block

	SKIP_LIST_NODE
	   		*Header,		// Pointer to header
	   		**Update,		// Table used for updates
	   		**FreeNodes;   	// Table of free nodes (used by heap functions)

	SKIP_LIST_HEAP_BLOCK
			*NodeHeapList;	// List of allocated blocks

	unsigned char
			*NodeHeapPtr,	// Current pointer into heap block
			*NodeHeapEnd;	// Ending location of heap block

	SKIP_LIST_CMP
			pfCompare;		// Pointer to compare function

	long	Count;			// Number of nodes in the skip list

	SKIP_LIST_FREE_VALUE 	// Optional func to call to automatically free
			pfFreeValue; 	// values

	int		DupeFlags;		// Flags indicating what to do with dupes

	void	*NullValue;		// Value to return if key not found

	SKIP_LIST_STATS			// Allocated/refreshed by SkipListStats
			*Stats;

} SKIP_LIST;


/*
**	Macros
*/

#define	SkipListFirst(SkipList)		((SkipList)->Header->Forward[0])
#define	SkipListNext(SkipListNode)	((SkipListNode)?(SkipListNode)->Forward[0]:NULL)
#define	SkipListEnd(SkipListNode)	(!(SkipListNode))
#define	SkipListValue(SkipListNode)	((SkipListNode)?(SkipListNode)->Value:NULL)
#define	SkipListKey(SkipListNode)	((SkipListNode)?(SkipListNode)->Key:NULL)

#define	SKIP_LIST_FOR(sln,sl)		for( (sln) = SkipListFirst( sl ); !SkipListEnd( sln ); (sln) = SkipListNext( sln ))


/*
**	Prototype functions
*/

DLLEXPORT SKIP_LIST
		*SkipListCreate(	SKIP_LIST_CMP pfCompare, int DupeFlags );

DLLEXPORT void
		SkipListDestroy(	SKIP_LIST *SkipList );

DLLEXPORT int
		SkipListInsert(		SKIP_LIST *SkipList, void *Key, void *Value ),
		SkipListDelete(		SKIP_LIST *SkipList, void *Key ),
		SkipListDeleteNode(	SKIP_LIST *SkipList, const void *Key, const void *KeyToDelete, const void *ValueToDelete );

DLLEXPORT SKIP_LIST_NODE
		*SkipListFind(		SKIP_LIST *SkipList, const void *Key ),
		*SkipListFindGE(	SKIP_LIST *SkipList, const void *Key );

DLLEXPORT void
		*SkipListGetEqual(	SKIP_LIST *SkipList, const void *Key, void **KeyPtr ),
		*SkipListGetFirst(	SKIP_LIST *SkipList, void **KeyPtr ),
		*SkipListGetLast(	SKIP_LIST *SkipList, void **KeyPtr ),
		*SkipListGetGreater(SKIP_LIST *SkipList, const void *Key, void **KeyPtr ),
		*SkipListGetGE(		SKIP_LIST *SkipList, const void *Key, void **KeyPtr ),
		*SkipListGetLess(	SKIP_LIST *SkipList, const void *Key, void **KeyPtr ),
		*SkipListGetLE(		SKIP_LIST *SkipList, const void *Key, void **KeyPtr );

DLLEXPORT long
		SkipListStats(		SKIP_LIST *SkipList );

DLLEXPORT int
		SkipListValidate(	SKIP_LIST *SkipList );

#endif
