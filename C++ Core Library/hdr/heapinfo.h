/* $Header: /home/cvs/src/kool_ade/src/heapinfo.h,v 1.13 2012/05/31 17:48:26 e19351 Exp $ */
/****************************************************************
**
**	HEAPINFO.H	- Heap information structures and functions
**				  Actual functions are in smrtheap project
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_HEAPINFO_H )
#define IN_HEAPINFO_H

typedef struct HeapInfoStructure
{
	long	UsedBytes,
			UsedNodes;

	unsigned int
			UsedMaxBlock;

	void	*UsedHighPtr;

	long	FreeBytes,
			FreeNodes,
			MemAvailable;

	unsigned int
			FreeMaxBlock;

	void	*FreeHighPtr;

	char	*ErrorText;

} HEAP_INFO;


/*
**	Global variable for indicating semantics for HeapMemSize
*/

typedef enum
{
	MEM_SIZE_BYTES_ALLOCATED,		// Report bytes allocated -- default
	MEM_SIZE_BYTES_USED				// Report bytes used for allocation

} MEM_SIZE_TYPE;

DLLEXPORT MEM_SIZE_TYPE
		GSHeapInfoMemSizeType;


/*
**	Function prototypes
*/

DLLEXPORT long
		GSHeapDebug(		void ),
		GSHeapInfo(		HEAP_INFO *HeapInfo ),
		GSHeapMemSize(	const void *Ptr ),
		GSHeapSet(		int Value ),
		GSHeapUsed(		void );

DLLEXPORT void
		*GSHeapFindSize(	void *Start, size_t Size ),
		GSHeapGetSizes(	int *RetArray, int RetArraySize );

#endif

