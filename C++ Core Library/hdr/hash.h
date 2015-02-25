/****************************************************************
**
**	hash.h - Generic hash table API
**
**	$Header: /home/cvs/src/kool_ade/src/hash.h,v 1.23 2004/11/29 19:36:54 khodod Exp $
**
****************************************************************/

#ifndef IN_HASH_H
#define IN_HASH_H

#include <port_int.h> // for uint32_t
#include <ka_base.h>


/*
**	Define constants
*/

#if !defined( ULONG_MAX )
#	include	<limits.h>
#endif
#define	HASH_MAX_BUCKET				UINT32_MAX

#define HASH_EXTEND_RATIO			0.7f
#define HASH_TARGET_RATIO			0.5f


/*
**	Hash entry status codes
*/

enum HashStatusEnum
{
	HASH_EMPTY = 0,
	HASH_DELETED,
	HASH_IN_USE
};


/*
**	Define types
*/

typedef const void *	HASH_KEY;
typedef const void *	HASH_VALUE;
typedef uint32_t		HASH_BUCKET;
typedef uint32_t		HASH_COUNT;

// Forward declare
struct MemPoolStructure;


/*
**	Typedefs for functions used by hash tables
*/


/*
**	Hash function that must return a bucket number
**	between 0 and HASH_MAX_BUCKET given any key.  May be NULL,
**	in which case the default (pretty good) hash
**	is used.  If you're hashing strings, you can use
**	HashStrHash (which casts the HASH_KEY into a
**	char *).
*/

typedef HASH_BUCKET
		(*HASH_FUNC)( HASH_KEY Key );


/*
**	Comparison function that must return zero or 
**	non-zero if key1 is ==,!=, respectively, to key2.
**	May be NULL, in which case the == operator is 
**	used.  If you're hashing strings, you can use 
**	HashStrcmp or HashStricmp.
*/

typedef int
		(*HASH_CMP)( HASH_KEY Key1, HASH_KEY Key2 );
		

/*
**	Print function for HashStat
*/

typedef int 
		(*HASH_PRINT_FUNC)( void *Handle, const char *Format, ... );


/*
**	Individual entry within a hash table
*/

struct HASH_ENTRY
{
	char 	Status;

	HASH_KEY
			Key;

	HASH_VALUE
			Value;
				
	HASH_BUCKET
			Bucket;

};
typedef HASH_ENTRY* HASH_ENTRY_PTR;


/*
**	Define hash table structure
*/

typedef struct HashStructure
{
	const char
			*Title;			// Purely for use by caller.  Just so you can tell
							// different tables apart if you want.

	HASH_ENTRY_PTR
			Table;			// For hash use only.

	HASH_FUNC
			Hash;			// Hash function that must return a bucket number
							// between 0 and size-1 given any key.  May be NULL,
							// in which case the default (pretty good) hash
							// is used.  If you're hashing strings, you can use
							// HashStrHash.

	HASH_CMP
			Cmp;			// Comparison function that must return zero or
							// non-zero if key1 is ==,!=, respectively, to key2.
							// May be NULL, in which case the == operator is
							// used.  If you're hashing strings, you can use
							// strcmp or stricmp.
	
	HASH_COUNT
			KeyCount,		// Number of keys currently stored.
			DeleteCount,	// Number of buckets marked as deleted.
			Size,		   	// Number of buckets.
			RebuildSize;	// Use count when to rebuild

	MemPoolStructure
			*MemPool;
	
	HASH_ENTRY_PTR
			LastBucket;		// &table[ size - 1 ]

	int		SizeIndex;		// Size index

} HASH;


/*
**	Structure used for dealing with hash sorted arrays
*/

struct HASHPAIR
{
	HASH_KEY
			Key;

	HASH_VALUE
			Value;
			
	HASH_BUCKET
			Bucket;

	HASH	*t;

};

typedef HASHPAIR* HASHPAIR_PTR;
	
/*
**	Structure for holding hash statistics
*/

struct HASH_STATISTICS
{
	int		HashAllocWithPool,
			HashAllocNoPool,
			HashDestroyWithPool,
			HashDestroyNoPool;
			
};


struct HASH_USABILITY
{
	double	ChainMean,
			ChainSigma;

	unsigned int
			ChainLongest;

	HASH_ENTRY_PTR
			LongestChainStart;

};

/*
**	Typedefs for functions used by hash tables
*/

// Comparison function that must return zero if
// key1 == key2, negative if key1 < key2, and 
// positive if key1 > key2. 
//
// May be NULL, in which case the cmp function for
// the hash table is used.  If that is also NULL, the
// following function is used:
// 
// default_cmp( long key1, long key2) 
// { 
//		return( key1 - key2); 
// }

typedef int
		(*HASH_PAIR_CMP)( const HASHPAIR_PTR key1, const HASHPAIR_PTR key2 );


/*
**	Helper macros for common hash compare functions
*/

#define	HashStrcmp				((HASH_CMP) strcmp)
#define	HashStricmp				((HASH_CMP) stricmp)


/*
**	Macros for enumerating hash table contents
*/

#define	HASH_FOR(p,t)	 		for( (p) = (t)->Table; (p) <= (t)->LastBucket; (p)++ ) if( HASH_IN_USE == (p)->Status )

#define	HashEnd(HashEnum)		(!(HashEnum))
#define	HashKey(HashEnum)		((HashEnum)->Key)
#define	HashValue(HashEnum)		((HashEnum)->Value)
#define	HashBucket(HashEnum)	((HashEnum)->Bucket)

#define HASH_SORTED_FOR(p,h,s,o)	for( (o)=(p)=HashSortedArray((h),(s)); (p)->Key; (p)++ )
#define HASH_SORTED_FREE(o)			MemPoolFree( (o)->t->MemPool, (o) )


/*
**	Prototypes
*/

EXPORT_CPP_KOOL_ADE HASH
		*HashCreate(			const char *title, HASH_FUNC hash, HASH_CMP cmp, HASH_COUNT InitSize, struct MemPoolStructure *MemPool );

EXPORT_CPP_KOOL_ADE HASH_BUCKET
		HashIntHash(			HASH_KEY Key	),
		HashStrHash(			HASH_KEY Key	);

EXPORT_CPP_KOOL_ADE void
		HashDestroy(			HASH *t );

EXPORT_CPP_KOOL_ADE int		 
		HashInsert(				HASH *t, HASH_KEY Key, HASH_VALUE Value ),
		HashInsertByBucket(		HASH *t, HASH_KEY Key, HASH_BUCKET Bucket, HASH_VALUE Value );
		
EXPORT_CPP_KOOL_ADE HASH_VALUE
		HashDelete(				HASH *t, HASH_KEY Key ),
		HashDeleteByBucket(		HASH *t, HASH_KEY Key, HASH_BUCKET Bucket ),
		HashDeleteEntry(		HASH *t, HASH_ENTRY_PTR Entry );
		
EXPORT_CPP_KOOL_ADE HASH_VALUE
		HashLookup(				HASH *t, HASH_KEY Key ),
		HashLookupByBucket(		HASH *t, HASH_KEY Key, HASH_BUCKET Bucket );
		
EXPORT_CPP_KOOL_ADE HASH_ENTRY_PTR
		HashLookupPtr(			HASH *t, HASH_KEY Key ),
		HashLookupPtrByBucket(	HASH *t, HASH_KEY Key, HASH_BUCKET Bucket );

EXPORT_CPP_KOOL_ADE HASHPAIR_PTR
		HashSortedArray(		HASH *t, HASH_PAIR_CMP cmp );

EXPORT_CPP_KOOL_ADE HASH_ENTRY_PTR
		HashFirst(				HASH *t ),
		HashNext(				HASH *t, HASH_ENTRY_PTR eh );

EXPORT_CPP_KOOL_ADE double
		HashStat(				HASH *t, HASH_USABILITY *Usability, void *Handle, HASH_PRINT_FUNC Print );

EXPORT_CPP_KOOL_ADE void
		HashStatistics(			HASH_STATISTICS *Stats );

EXPORT_CPP_KOOL_ADE size_t
		HashMemSize(			HASH *t );

#endif

