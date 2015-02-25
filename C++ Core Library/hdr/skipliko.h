/* $Header: /home/cvs/src/skiplist/src/skipliko.h,v 1.6 1999/10/18 17:48:35 singhki Exp $ */
/****************************************************************
**
**	SKIPLIKO.H	- Skip List with Keys only
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_SKIPLIKO_H )
#define IN_SKIPLIKO_H

/*
**	Define constants
*/

#define	SKIP_LIKO_DUPE_ALLOW			1
#define	SKIP_LIKO_DUPE_REJECT			2
#define	SKIP_LIKO_DUPE_OVERWRITE		3
										
#define	SKIP_LIKO_DEFAULT_MAX_LEVEL		15
#define	SKIP_LIKO_DEFAULT_BLOCK_SIZE	4000

#define	SKIP_LIKO_ERR_NODE_ORDER		1
#define	SKIP_LIKO_ERR_NODE_COUNT		2


/*
**	Function pointers
*/

typedef int					(*SKIP_LIKO_CMP)( const void *Key1, const void *Key2 );
#define SKIP_LIKO_STRICMP	((SKIP_LIKO_CMP)stricmp)


/*
**	Structure used to hold heap block for node allocation
*/

typedef struct SkipLikoHeapBlockStructure
{
	struct SkipLikoHeapBlockStructure
			*Next;

	int		Size;

} SKIP_LIKO_HEAP_BLOCK;


/*
**	Structure to hold a node in the list
*/

typedef struct SkipLikoNodeStructure
{
    void	*Key;

    struct SkipLikoNodeStructure
    		*Forward[1];  	// variable sized array of forward pointers

} SKIP_LIKO_NODE;


/*
**	Structure to hold skip list statistics
*/

typedef struct SkipLikoStatsStructure
{
	long	Levels,			// Levels allocated in NodesUsed & NodesFree
			TotalNodesUsed,	// Total nodes in use
			TotalNodesFree;	// Total nodes on free lists

	long	*NodesUsed,		// [ Levels ] - Nodes in use by level
			*NodesFree;		// [ Levels ] - Nodes free by level

	long	BlockMemory;	// Total memory in NodeHeapList

	long	MaxLength,		// Greatest length to any node
			TotalLength;	// Sum of all lengths to each node

} SKIP_LIKO_STATS;


/*
**	Structure to hold a skip list 
*/

typedef struct SkipLikoStructure
{
	int		Level,			// 1 more than the highest level in the list
			MaxLevel,		// Maximum number of levels
			HeapBlockSize;	// Allocation size for each heap block

	SKIP_LIKO_NODE
	   		*Header,		// Pointer to header
	   		**Update,		// Table used for updates
			
	   		**FreeNodes;   	// Table of free nodes (used by heap functions)

	SKIP_LIKO_HEAP_BLOCK
			*NodeHeapList;	// List of allocated blocks

	unsigned char
			*NodeHeapPtr,	// Current pointer into heap block
			*NodeHeapEnd;	// Ending location of heap block

	SKIP_LIKO_CMP
			pfCompare;		// Pointer to compare function

	long	Count;			// Number of nodes in the skip list

	int		DupeFlags;		// Flags indicating what to do with dupes

	void	*NullValue;		// Value to return if key not found

	SKIP_LIKO_STATS			// Allocated/refreshed by SkipLikoStats
			*Stats;

	int		UpdateCount;	// every update increments this

} SKIP_LIKO;


/*
**	Macros
*/

#define	SkipLikoFirst(SkipLiko)		((SkipLiko)->Header->Forward[0])
#define	SkipLikoNext(SkipLikoNode)	((SkipLikoNode)?(SkipLikoNode)->Forward[0]:NULL)
#define	SkipLikoEnd(SkipLikoNode)	(!(SkipLikoNode))
#define	SkipLikoKey(SkipLikoNode)	((SkipLikoNode)?(SkipLikoNode)->Key:NULL)

#define	SKIP_LIKO_FOR(sln,sl)		for( (sln) = SkipLikoFirst( sl ); !SkipLikoEnd( sln ); (sln) = SkipLikoNext( sln ))


/*
**	Prototype functions
*/

DLLEXPORT SKIP_LIKO
		*SkipLikoCreate(	SKIP_LIKO_CMP pfCompare, int DupeFlags );

DLLEXPORT void
		SkipLikoDestroy(	SKIP_LIKO *SkipLiko );

DLLEXPORT int
		SkipLikoInsert(		SKIP_LIKO *SkipLiko, void *Key, int UseUpdate ),
		SkipLikoDelete(		SKIP_LIKO *SkipLiko, void *Key ),
		SkipLikoDeleteNode(	SKIP_LIKO *SkipLiko, const void *Key, const void *KeyToDelete );

DLLEXPORT SKIP_LIKO_NODE
		*SkipLikoFind(		SKIP_LIKO *SkipLiko, const void *Key ),
		*SkipLikoFindGE(	SKIP_LIKO *SkipLiko, const void *Key, int Update );

DLLEXPORT void
		*SkipLikoGetEqual(	SKIP_LIKO *SkipLiko, const void *Key ),
		*SkipLikoGetFirst(	SKIP_LIKO *SkipLiko ),
		*SkipLikoGetLast(	SKIP_LIKO *SkipLiko ),
		*SkipLikoGetGreater(SKIP_LIKO *SkipLiko, const void *Key ),
		*SkipLikoGetGE(		SKIP_LIKO *SkipLiko, const void *Key ),
		*SkipLikoGetLess(	SKIP_LIKO *SkipLiko, const void *Key ),
		*SkipLikoGetLE(		SKIP_LIKO *SkipLiko, const void *Key );

DLLEXPORT long
		SkipLikoStats(		SKIP_LIKO *SkipLiko );

DLLEXPORT int
		SkipLikoValidate(	SKIP_LIKO *SkipLiko );

#endif
