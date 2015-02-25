/****************************************************************
**
**	SDB_INT.H	- Security Database internal header file
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sdb_int.h,v 1.105 2010/04/08 16:55:54 ellist Exp $
**
****************************************************************/

#if !defined( IN_SDB_INT_H )
#define IN_SDB_INT_H

#ifndef IN_SDB_BASE_H
#include <sdb_base.h>
#endif
#ifndef IN_HASH_H
#include <hash.h>
#endif
#ifndef IN_SECDB_H
#include <secdb.h>
#endif
#ifndef IN_SECDRV_H
#include <secdrv.h>
#endif

#include <ccstl.h>
#include <vector>
#include <map>
#include <string>


/*
**	Forward declare structures
*/

struct SLANG_SCOPE;
struct HashStructure;

typedef CC_STL_MAP_WITH_COMP ( SDB_DB*, CC_STL_VECTOR( SDB_DB* ), SDB_DB::db_less ) db_to_db_vec_t;

/*
**	Define constants
*/

unsigned const SDB_OBJECT_CACHE_INITIAL_SIZE	= 5000;
unsigned const SDB_OBJECT_PRECACHE_INITIAL_SIZE	= 2000;
unsigned const SDB_PAGETABLE_INITIAL_SIZE	    = 0;

unsigned long const SDB_MAX_MEM_SIZE		    = ( 32767 - sizeof( SDB_DISK ));

unsigned const SDB_SYMLINK_HASH_SIZE		    = 500;



/*
**	New security prefix string
*/

extern const char
	SecDbNewSecurityPrefix[];			// "_New_"


/*
**	Message function pointers
*/

EXPORT_C_SECDB SDB_MSG_FUNC
		SecDbMessageFunc,
		SecDbErrorMessageFunc;


/*
**	Define structure for holding data in the value flags info hash table
*/

struct SDB_VALUE_FLAGS_INFO
{
	SDB_SEC_TYPE
			SecType;

	SDB_VALUE_TYPE
			ValueType;

	SDB_VALUE_FLAGS
			ValueFlags;

};

typedef char const* ccharp; // XXX is this used? glypse doesn't think so

/*
**	Variables
*/

extern int
		DiddleCount,					// Number of active diddles
		TotalRefCount,					// Total number of references to secs
		SecDbUseDbPath;					// if Get/New/NameLookup & Delete should use fully qualified SecNames or not

extern HashStructure
		*SecDbValueTypeHash,			// Hash table of value type codes
		*SecDbIndexHash,				// Hash table of indices
		*SecDbSecTypeHash,				// Hash table of classes by type
		*SecDbSecDescHash,				// Hash table of classes by name
		*SecDbDatabaseHash;				// Hash table of databases by name

extern HASH
		*SecDbGlobalSymlinks;			// Hash table of global links from strings to db names.

extern db_to_db_vec_t
		sdb_reusable_diddle_scopes;			// reusable diddle scopes by base db

EXPORT_C_SECDB SDB_DB
		*SecDbClassObjectsDb;			// The Db which stores Class objects

EXPORT_CPP_SECDB CC_STL_VECTOR( CC_NS(std,string ))
		SecDbCleanupDiddleScopes();


/*
**	Macros
*/

#define M_SETUP_VERSION0(Version,SecPtr,StreamVer)		\
{														\
	(Version)->Object = (SecPtr)->Class->ObjectVersion;	\
	(Version)->Stream = (StreamVer);					\
}
	
#define M_SETUP_VERSION(Version,SecPtr,pDb)				\
{														\
	(Version)->Object = (SecPtr)->Class->ObjectVersion;	\
	(Version)->Stream = (pDb)->StreamVersion;			\
}


/*
** resolve qualified SecNames into Db & SecName
*/

inline int ResolveDbPath( char const* SecName, SDB_DB*& Db, char const*& FinalSecName)
{
	if( (( SecDbUseDbPath == -1 && (SecDbUseDbPath = !stricmp( SecDbConfigurationGet( "SECDB_DB_PATH_FOR_GET", NULL, NULL, "True" ), "True" )))
		 || SecDbUseDbPath )
		&& strchr( SecName, SDB_SECNAME_DELIMITER ))
	{
		char	*DbName = strdup( SecName ),
				*ObjPtr;

		ObjPtr = strrchr( DbName, SDB_SECNAME_DELIMITER );
		*ObjPtr++ = '\0';
		Db = SecDbAttach( DbName, 0, 0 );
		free( DbName );

		if( !Db )
			return ErrMore( "Cannot resolve Db ref in %s", SecName );

		FinalSecName = SecName + ( ObjPtr - DbName );
	}
	else if( !Db )
		return Err( ERR_DATABASE_INVALID, "ResolveDbPath( NULL Db )" );
	else
		FinalSecName = SecName;
	return TRUE;
}

inline int ResolveDbPath( char* SecName, SDB_DB*& Db, char*& FinalSecName )
{
	return ResolveDbPath( SecName, Db, const_cast<char const*&>( FinalSecName ));
}

/*
** Get physical Db
*/

inline SDB_DB *DbToPhysicalDb( SDB_DB *Db )
{
	return Db ? Db->DbPhysical : 0;
}


/*
**	Defines for huge objects
*/

#define SDB_HUGE_PAGE_NAME_FORMAT	"~Huge~%010ld"
int const SDB_HUGE_PAGE_NAME_SIZE	= sizeof( "~Huge~0123456789" );
int const SDB_HUGE_PAGE_SEC_TYPE	= 0002;
unsigned long const SDB_HUGE_PAGE_SIZE		= ( SDB_MAX_MEM_SIZE - SDB_SEC_NAME_SIZE );

#define	SDB_IS_HUGE( VerInfo )				(( (VerInfo) >> SDB_DIS_HUGE_BIT ) & SDB_DIM_HUGE_BIT )
#define	SDB_SET_HUGE( VerInfo, Value )		( (VerInfo) = ( (VerInfo) & ~SDB_DIBS_HUGE_BIT ) | ( !!(Value) << SDB_DIS_HUGE_BIT ))


/*
**	Prototype functions
*/

int		SearchPathDrvInit(				SDB_DB *Db );

int		DtFuncSecurity(					int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b ),
		DtFuncSecurityList(				int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b ),
		DtFuncSlang(					int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b ),
		DtFuncPointer(					int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b ),
		DtFuncDatabase(					int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b ),
		DtFuncDiddleScope(				int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b ),
		DtFuncDiddleScopeList(			int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b );

SDB_DB	*SecDbGetFromDatabase(			SDB_DB* pDb, SDB_SEC_NAME const* SecName, SDB_STORED_OBJECT* StoredObject, int Raw );

void	SecDbFlushPreCache(				SDB_DB* pDb );

int		SecDbIndexAddIfMatch(			SDB_INDEX* Index, SDB_OBJECT* SecPtr ),
		SecDbIndexDeleteIfMatch(		SDB_INDEX* Index, SDB_SEC_NAME const* SecName ),
		SecDbIndexUpdateIfMatch(		SDB_INDEX* Index, SDB_OBJECT* SecPtr ),
		SecDbIndexIncrementalIfMatch(	SDB_INDEX* Index, SDB_OBJECT* SecPtr ),
		SecDbIndexRenameIfMatch(		SDB_INDEX* Index, SDB_OBJECT* NewSecPtr, SDB_SEC_NAME const* OldSecName ),
		SecDbIndexAddSecurity(			SDB_INDEX* Index, SDB_OBJECT* SecPtr ),
		SecDbIndexDeleteSecurity(		SDB_INDEX* Index, SDB_SEC_NAME const* SecName ),
		SecDbIndexUpdateSecurity(		SDB_INDEX* Index, SDB_OBJECT* SecPtr ),
		SecDbIndexIncremental(			SDB_INDEX* Index, SDB_OBJECT* SecPtr ),
		SecDbIndexRenameSecurity(		SDB_INDEX* Index, SDB_SEC_NAME const* NewSecName, SDB_SEC_NAME const* OldSecName ),
		SecDbStoreIndices(				SDB_DB_ID DbID ),
		SecDbIndexCompareFunction(		HASH_KEY Index1, HASH_KEY Index2 ),
		SecDbIndexGetByNameDriver(		SDB_INDEX_POS* IndexPos, int GetType, const char *SecName ),
		SecDbIndexGetDriver(			SDB_INDEX_POS* IndexPos, int GetType );

unsigned long
		SecDbIndexHashFunction(			HASH_KEY Index );

void	SecDbErrStartup(				void ),
		SecDbErrConstants(				void );

int		SecDbReattachOld(				SDB_DB_ID DbID ),
		SecDbReattach(					SDB_DB *Db ),
		SecDbInitializeMore(			void );

SDB_DB	*SecDbTransProtectedGet(		SDB_DB* pDb, SDB_SEC_NAME const* SecName, SDB_STORED_OBJECT* StoredObject, int Raw, SDB_DB* pDbPhysical = 0 ),
		*SecDbTransProtectedDiskInfo(	SDB_DB* pDb, SDB_SEC_NAME const* SecName, SDB_DISK* DiskInfo, SDB_DB* pDbPhysical = 0 );

int		SecDbTransProtectedIndexGet(	SDB_DB* pDb, SDB_INDEX_POS* IndexPos, int GetType ),
		SecDbTransProtectedIndexGetByName(	SDB_DB* pDb, SDB_INDEX_POS* IndexPos, int GetType, const char *SecName );

void	SecDbTransPartFree(				SDB_TRANS_PART* TransPart );

int 	SecDbValueFuncGetSecurity(		SDB_OBJECT *SecPtr, int Msg, SDB_M_DATA *MsgData, SDB_VALUE_TYPE_INFO *ValueTypeInfo );

void	SecDbVtiDelete(   				SDB_VALUE_TYPE_INFO* Vti );

int		SecDbMergeMax(					int GetType );

SDB_DB	*SecDbHugeLoad(					SDB_DB *pDb, int SyncPointOffset, SDB_STORED_OBJECT *StoredObject );

int		SecDbHugeUpdate(				SDB_DB *pDb, SDB_DISK *OldDisk, SDB_DISK *NewDisk, unsigned long *pMemSize, void **pMem ),
		SecDbHugeDelete(				SDB_DB *pDb, SDB_SEC_NAME const* SecName );
		
void	SecDbHugeUnload(				SDB_DB *pDb, SDB_SEC_NAME const* SecName );

SDB_DB *SecDbParseAndAttach( 			char const** Path, int DeadPoolSize, int Flags );
SDB_DB *DbFromDbName(					char const* DbName );
int 	DbStructure(		   			SDB_DB *pDb, DT_VALUE *Struct );

void	SecDbClassObjectRemove(			SDB_OBJECT *SecPtr );
void	SecDbClassObjectInsert(			SDB_OBJECT *SecPtr );

SDB_DIDDLE_ID
		SlangScopeSetDiddleWithFlags(	SLANG_SCOPE *Scope, SDB_OBJECT *SecPtr, SDB_VALUE_TYPE ValueType, int Argc, DT_VALUE **Argv, DT_VALUE *Value,
										SDB_DIDDLE_ID DiddleID, int Flags );

int		ClassSpecialVtGen(				SDB_OBJECT *Object, SDB_VALUE_TYPE ValueType ),
		ValueTypeSpecialVtGen(			SDB_NODE *ValueTypeNode );

int		DiddleScopeSuffix(				DT_VALUE *Src, DT_VALUE *Suffix ),
		DiddleScopeSuffix(				SDB_DB *Db, DT_VALUE *Value );

int		SecDbLiteralHashCmp(			HASH_KEY KeyA, HASH_KEY KeyB );

SDB_DB *SecDbNameLookupInternal( 		char* SecName, char* FoundName, SDB_SEC_TYPE SecType, int GetType, SDB_DB *Db );

const char
		*SecDbDbOperatorName( SDB_DB_OPERATOR Operator );

DT_ARRAY*SecDbDiddleScopeToArray( DT_DIDDLE_SCOPE* DiddleScope );

#endif

