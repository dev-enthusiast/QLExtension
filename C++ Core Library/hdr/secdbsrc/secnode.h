//#define SDB_LOCAL_PARENTS		// Turns on LocalParents optimization
/****************************************************************
**
**	SECNODE.H	- SecDb calculation tree nodes
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secnode.h,v 1.73 2001/11/27 23:23:45 procmon Exp $
**
****************************************************************/

#ifndef IN_SECNODE_H
#define IN_SECNODE_H

#include <secdb.h>
#include <secnodei.h>

#include <string>

/*
**  Forward declare node structure which is actually defined in secnodei.h
*/

class SDB_NODE;
struct SDB_DBSET;

/*
**	Enum flags for SecDbNodeEnum*
*/

int const SDB_NODE_ENUM_TERMINALS		= 0x0001;		// Include terminals in enumeration (default if flags == 0)
int const SDB_NODE_ENUM_NON_TERMINALS	= 0x0002;		// Include non-terminals in enumeration
int const SDB_NODE_ENUM_LITERALS		= 0x0004;		// Include literals in enumeration


/*
**	Memory management
*/

#define	NODE_LIST_ALLOC( NodeList_, Size_ )		ERR_MALLOC( NodeList_, SDB_NODE *, sizeof( SDB_NODE * ) * (Size_) )
#define NODE_LIST_FREE( NodeList_ )				free( NodeList_ )
#define	NODE_LIST_REALLOC( NodeList_, Size_ )	ERR_REALLOC( NodeList_, SDB_NODE *, sizeof( SDB_NODE * ) * (Size_) )


/*
**	Other macros
*/

int const SDB_VECTOR_MAX_LEVEL = 10;


/*
**	Special tokens passed internally as string literal arguments
*/

char const* const NODE_PASS_ERRORS_ARG  = "Pass Errors";
char const* const NODE_ARRAY_RETURN_ARG = "Array Return";


/*
**	DtNodeFlags
*/

int const   SDB_PASS_ERRORS			= 0x0001;
int const   SDB_ARRAY_RETURN		= 0x0002;
int const   SDB_PREPEND_ARGS		= 0x0004;

typedef int DT_NODE_FLAGS;


/*
**	Structures
*/

struct SDB_DIDDLE_HEADER
{
	SDB_NODE
			*Node;

	SDB_DIDDLE
			*DiddleList;

};


struct SDB_DIDDLE
{
	SDB_DIDDLE
			*Next;

	SDB_VECTOR_CONTEXT
			*VectorContext;

	DT_VALUE
			Value;

	unsigned
			Phantom:1,
			SetValue:1,
			NotMyDiddle:1;

	SDB_DIDDLE_ID
			DiddleID;

	SDB_SET_FLAGS
			CacheFlags;

};

struct SDB_NODE_LOCKER
{
	SDB_NODE_LOCKER
			*Next;

	const char
			*Name;

};


/*
**	Variables
*/

extern DT_DATA_TYPE
		DtNodeFlags;

extern SDB_NODE
		*SubscriptObject,
		*CastObject;

EXPORT_C_SECDB SDB_NODE_LOCKER			// Are all node locked for invalidation?
		*NodeLockers;					// See also NodeLockAll/NodeUnlockAll


/*
**	Function prototypes
*/

int		NodeInitialize(				void ),
		NodeChildren(				SDB_NODE *Node, HASH **Hash, SDB_CHILD_ENUM_FILTER *Filter, SDB_DB *Db );

int		DtValueTypeInfoFromVti( DT_VALUE *DtVti, SDB_VALUE_TYPE_INFO *Vti );

void	NodeDestroyCache(			SDB_OBJECT *SecPtr, int DestroyLiteral ),
		NodeChildrenHashDestroy(	HASH *Hash );

long	NodeMemoryUsage(			DT_VALUE *ResultsStructure );

inline CC_NS(std,string)
		SecDbNodeText(				SDB_NODE *Node, int AppendVal )
{
	return Node->GetText( AppendVal );
}

EXPORT_C_SECDB SDB_NODE
		*SecDbNodeEnumFirst(		SDB_OBJECT *SecPtr, int EnumFlags, SDB_ENUM_PTR *EnumPtr ),
		*SecDbNodeEnumNext(			SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB void
		SecDbNodeEnumClose(			SDB_ENUM_PTR EnumPtr );

int		SecDbAutoExport(			SDB_OBJECT *SecPtr, SDB_VALUE_TYPE_INFO	*Vti );

EXPORT_C_SECDB HASH
		*SecDbNodeSanityCheck( 		void );

EXPORT_C_SECDB int
		NodeLockAll(				const char *Locker ),
		NodeUnlockAll(				const char *Locker );

#endif

