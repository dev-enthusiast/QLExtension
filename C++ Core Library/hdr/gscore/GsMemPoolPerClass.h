/****************************************************************
**
**	GSMEMPOOLPERCLASS.H	-
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsMemPoolPerClass.h,v 1.5 2001/11/27 22:41:06 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSBASE_GSMEMPOOLPERCLASS_H )
#define IN_GSBASE_GSMEMPOOLPERCLASS_H

#include <heap.h>


#ifdef CC_DELETE_PLACEMENT

// For compilers like VC 6.0 that have a placemente delete.

#define GS_MEMPOOL_PER_CLASS_DECLARE_DELETE_PLACEMENT 	static void operator delete( void *Mem, void *Place  );

#define GS_MEMPOOL_PER_CLASS_DEFINE_DELETE_PLACEMENT( Class )	\
void Class::operator delete(									\
	void *Mem,													\
	void *Place													\
)																\
{																\
}

#else

// For compilers unlike VC 6.0 that don't have a placemente delete.

#define GS_MEMPOOL_PER_CLASS_DECLARE_DELETE_PLACEMENT
#define GS_MEMPOOL_PER_CLASS_DEFINE_DELETE_PLACEMENT( Class )

#endif // CC_DELETE_PLACEMENT



#define GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS				\
															\
public:														\
															\
	static void *operator new( size_t Size );				\
	static void *operator new( size_t Size, void *Place );	\
	static void operator delete( void *Mem );				\
	GS_MEMPOOL_PER_CLASS_DECLARE_DELETE_PLACEMENT			\
															\
private:													\
															\
	static HEAP												\
			*s_Heap;


#define GS_MEMPOOL_PER_CLASS_DEFINE_MEMBERS( Class )												\
																									\
void * Class::operator new(																			\
	size_t Size																						\
)																									\
{																									\
	if( sizeof( Class ) != Size )																	\
		GSX_THROW( GsXInvalidArgument, 																\
				   "Per class operator new() called with request for size != sizeof( class )" );	\
																									\
	return GSHeapMalloc( s_Heap );																	\
}																									\
																									\
																									\
void * Class::operator new(																			\
	size_t Size,																					\
	void *Place																						\
)																									\
{																									\
	return Place;																					\
}																									\
																									\
																									\
void Class::operator delete(																		\
	void *Mem																						\
)																									\
{																									\
	GSHeapFree( s_Heap, Mem );																		\
}																									\
																									\
GS_MEMPOOL_PER_CLASS_DEFINE_DELETE_PLACEMENT( Class )												\
																									\
HEAP																								\
		*Class::s_Heap = GSHeapCreate( #Class " class heap",  sizeof( Class ), 64 );



#endif

