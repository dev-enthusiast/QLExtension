/****************************************************************
**
**	MEMPOOL.H	- Header file for memory pool functions
**
**	$Header: /home/cvs/src/kool_ade/src/mempool.h,v 1.13 2001/11/27 22:49:07 procmon Exp $
**
****************************************************************/

#ifndef IN_MEMPOOL_H
#define IN_MEMPOOL_H


/*
**	Define structure for tracking memory
*/

typedef struct MemPoolElemStruct
{
	struct MemPoolElemStruct
			*Prev,
			*Next;

	size_t	Size;

#if defined( MICROSOFT )
	// Only 16-bit MS compiler has huge concept
	char	Style;
#else
	long	Filler;	// Make sure struct is double-aligned
#endif

} MEM_POOL_ELEM;


/* 
**	Styles
*/

#define		MEMPOOL_FAR		0
#define		MEMPOOL_HUGE	1

/*
**	Define structure for memory pools
*/

typedef struct MemPoolStructure
{
	MEM_POOL_ELEM
			Head,
			Tail;

	long	Size;

	unsigned int
			MallocCount,
			CallocCount,
			HallocCount,
			FreeCount;

} MEM_POOL;


/*
**	Prototype functions
*/

DLLEXPORT MEM_POOL
		*MemPoolCreate(		void );

DLLEXPORT void
		MemPoolDestroy(		MEM_POOL *MemPool ),
		MemPoolFree( 		MEM_POOL *MemPool, void *Ptr ),
		MemPoolFreeAll(		MEM_POOL *MemPool ),
		MemPoolDump(		MEM_POOL *MemPool );

DLLEXPORT void
		*MemPoolCalloc(		MEM_POOL *MemPool, size_t Count, size_t Size),
		*MemPoolMalloc(		MEM_POOL *MemPool, size_t Size ),
		*MemPoolRealloc(	MEM_POOL *MemPool, void *Ptr, size_t Size );

DLLEXPORT void _huge
		*MemPoolHalloc(		MEM_POOL *MemPool, long Count, size_t Size );

DLLEXPORT char
		*MemPoolStrDup(		MEM_POOL *MemPool, const char *Source );

// no need to export as all methods are inline
class MemPoolGuard
{
public:
    MemPoolGuard() : m_pool(MemPoolCreate()) {}
	~MemPoolGuard() { MemPoolDestroy(m_pool); }

	void* Malloc( size_t size )               { return MemPoolMalloc( m_pool, size );}
	void* Calloc( size_t count, size_t size ) { return MemPoolCalloc( m_pool, count, size );}
	void* Realloc( void* ptr, size_t size )   { return MemPoolRealloc( m_pool, ptr, size );}

	char* StrDup( char const* source ) { return MemPoolStrDup( m_pool, source );}

	void Free( void* ptr ) { MemPoolFree( m_pool, ptr );}
	void FreeAll()         { MemPoolFreeAll( m_pool );}

	void Dump() { MemPoolDump( m_pool );}

	MEM_POOL*       operator->()       { return m_pool;}
	MEM_POOL const* operator->() const { return m_pool;}

	MEM_POOL&       operator*()       { return *m_pool;}
	MEM_POOL const& operator*() const { return *m_pool;}

	MEM_POOL* get() { return m_pool; }

private:
// undefined to prevent copying
	MemPoolGuard(MemPoolGuard const&);
	MemPoolGuard& operator=(MemPoolGuard const&);

	MEM_POOL* const m_pool;
};

#endif
