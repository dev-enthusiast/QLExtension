/****************************************************************
**
**	HEAP.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/heap.h,v 1.15 2004/01/22 12:12:30 mahonp Exp $
**
****************************************************************/

#ifndef IN_HEAP_H
#define IN_HEAP_H

/*
**	Define constants
*/

#define	HEAP_DEFAULT_BLOCK_ELEMENTS		500


/*
**	Structure used to maintain the heap
*/

struct HEAP_ELEMENT
{
	HEAP_ELEMENT
			*Next;

};

struct HEAP_BLOCK
{
	size_t	Size;

	HEAP_BLOCK
			*Next;

};

struct HEAP
{
	const char
			*Name;

	size_t	BlockSize,
			ElementSize,
			ElementAllocSize;

	HEAP_ELEMENT
			*FreeElementList;

	HEAP_BLOCK
			*BlockList;

	unsigned char
			*BlockPtr,
			*BlockEnd;

};


/*
**	Structure used to obtain statistics about the heap
*/

struct HEAP_STATS
{
	long	BlockCount,

			TotalElementCount,
			TotalMemory,

			FreeElementCount,
			FreeMemory,

			UsedElementCount,
			UsedMemory;

};


/*
**	Typedef for HeapWalk callback function.  A zero return value terminates
**	walk.
*/

typedef int (HEAP_WALK_FUNC)( void *Element, void *FuncArg );


/*
**	Prototype functions
*/

DLLEXPORT HEAP
		*GSHeapCreate(	const char *Name, size_t ElementSize, size_t BlockElements );

DLLEXPORT void
		GSHeapDestroy(	HEAP *Heap ),
		GSHeapFree(		HEAP *Heap, void *Element );

DLLEXPORT int
		GSHeapWalk( HEAP *Heap, HEAP_WALK_FUNC *Func, void *FuncArg );

DLLEXPORT void
		GSHeapStatistics(	HEAP *Heap, HEAP_STATS *HeapStats );

DLLEXPORT void
		*GSHeapMalloc(	HEAP *Heap ),
		*GSHeapCalloc(	HEAP *Heap );

#endif
