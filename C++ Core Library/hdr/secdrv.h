/****************************************************************
**
**	SECDRV.H	- SecDb database driver information
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdrv.h,v 1.92 2013/08/28 20:59:14 c01713 Exp $
**
****************************************************************/

#ifndef IN_SECDRV_H
#define IN_SECDRV_H

#ifndef IN_SECFILE_H
#include <secfile.h>
#endif
#ifndef IN_SECDB_H
#include <secdb.h>
#endif
#ifndef IN_HASH_H
#include <hash.h>
#endif
#ifndef IN_ERR_H
#include <err.h>		// For errors in inline code
#endif

#include <string> // for std::string

/*
**	Define constants
*/

int const SDB_DB_DRIVER_NAME_SIZE   = 32;
int const SDB_DB_DATABASE_BUF_SIZE	= 32767;


/*
**	Database attributes
*/

int const SDB_DB_ATTRIB_PROD	    = 0x0001;
int const SDB_DB_ATTRIB_PRODONLY	= 0x0002;
int const SDB_DB_ATTRIB_DEV			= 0x0004;
int const SDB_DB_ATTRIB_READONLY	= 0x0008;
int const SDB_DB_ATTRIB_OLD_STREAM  = 0x0010;	// Uncompressed BStream format
int const SDB_DB_ATTRIB_OBSOLETE	= 0x0020;	// Old-do not connect
int const SDB_DB_ATTRIB_QUALIFIERS	= 0x0040;	// Accepts qualifiers
int const SDB_DB_ATTRIB_ALIAS		= 0x0080;	// Is really an ALIAS database
int const SDB_DB_ATTRIB_DUPLICATED	= 0x0100;	// Duplicated db


/*
**	Stream versions
*/

int const SDB_STREAM_OLD			= 0;
int const SDB_STREAM_NEW			= 1;
int const SDB_STREAM_GZIP			= 2;


/*
**	DeadPool node numbers
*/

int const DEADPOOL_USED_NODES		= 0;
int const DEADPOOL_FREE_NODES		= 1;
int const DEADPOOL_FIRST_NODE		= 2;


/*
**	Define types
*/

struct SLANG_SCOPE_DIDDLES;
struct DT_DIDDLE_SCOPE;

typedef	void (*SDB_DB_INIT_FUNC)( SDB_DB *SdbDb );

struct SDB_DBSET;
struct SDB_DB_SEARCH_PATH;


/*
**	If you change this enum, please remember to change SecDbDbOperatorName in sdb_pars.c
*/

enum SDB_DB_OPERATOR
{
	SDB_DB_LEAF = 0,		// non-virtual database
							// Left and Right are NULL
							// DbID <= SDB_DB_ID_MAX
							// in database.dat

	SDB_DB_QUALIFIER,		// qualified database
							// Left and Right are NULL
							// Qualifier is non-NULL
							// Arbitrarily assigned DbID with SDB_DB_VIRTUAL bit set

	SDB_DB_UNION,			// union database
							// Left and Right hold the two unioned databases
							// Arbitrarily assigned DbID with SDB_DB_VIRTUAL bit set

	SDB_DB_READONLY,		// read-only version of any database which is write-permissible
							// Left is the READWRITE version of the database, Right is NULL
							// Arbitrarily assigned DbID with SDB_DB_VIRTUAL bit set

	SDB_DB_ALIAS,			// alias database
							// Left is the target database, Right is NULL
							// Arbitrarily assigned DbID with SDB_DB_VIRTUAL bit set

	SDB_DB_SUBDB,			// Subdatabase of an existing database.
							// Left is the database that we are a subdb of.
							// DbID set to SDB_DB_VIRTUAL
							// Qualifier is name of subdatabase.

	SDB_DB_DIDDLE_SCOPE,	// A diddle scope
							// Left & Right are NULL
							// Arbitrarily assigned DbID with SDB_DB_VIRTUAL bit set

	SDB_DB_CLASSFILTER,		// ClassFiltered database. Database dispensing only certain classes
							// Left is the underlying physical database, Right is NULL.
							// Arbitrarily assigned DbID with SDB_DB_VIRTUAL bit set

	SDB_DB_CLASSFILTERUNION,// ClassFiltered database union. Different classes from different dbs
						   	// Left and Right are underlying ClassFiltered dbs.
						   	// Arbitrarily assigned DbID with SDB_DB_VIRTUAL bit set

	SDB_DB_DUPLICATED		// A duplicated database allows program to have multiple
							// connections to the database server from different threads.
							// This needs to be used with care, as most secdb libraries
							// are not thread-safe.
};


/*
**	Structure used to contain deadpool elements
*/

struct SDB_DEAD_POOL_ELEMENT
{
	SDB_OBJECT
			*SecPtr;			// Pointer to security

	int		Prev,				// Index of previous deadpool element
			Next;				// Index of next deadpool element

};


/*
**	Structure used to handle security databases
*/

struct SDB_DB
{
	SDB_DB()
    : Next                  ( 0 ),
	  DbName                ( 0 ),
      DbID                  ( 0 ),
      DbType                ( 0 ),
      DbAttributes          ( 0 ),
      OpenCount             ( 0 ),
      PreCacheDepth         ( 0 ),
      StreamVersion         ( 0 ),
      CacheHash             ( 0 ),
      PreCacheHash          ( 0 ),
      PageTableHash         ( 0 ),
      DeadPoolCount         ( 0 ),
      DeadPoolSize          ( 0 ),
      DeadPool              ( 0 ),
      TransHead             ( 0 ),
      TransTail             ( 0 ),
      TransDepth            ( 0 ),
	  TransFlags			( 0 ),
      Handle                ( 0 ),
      HandleIndex           ( 0 ),
      HandleTransaction     ( 0 ),
      HandleLock            ( 0 ),
      pfInit                ( 0 ),
      pfGetByName           ( 0 ),
      pfGetFromSyncPoint    ( 0 ),
      pfGetManyByName       ( 0 ),
      pfCreate              ( 0 ),
      pfOpen                ( 0 ),
      pfSetTimeout          ( 0 ),
      pfClose               ( 0 ),
      pfNameLookup          ( 0 ),
      pfNameLookupByType    ( 0 ),
      pfGetDiskInfo         ( 0 ),
      pfIndexOpen           ( 0 ),
      pfIndexClose          ( 0 ),
      pfIndexGet            ( 0 ),
      pfIndexGetByName      ( 0 ),
      pfIndexGetSecurity    ( 0 ),
      pfIndexPosCreate      ( 0 ),
      pfIndexPosDestroy     ( 0 ),
      pfTransCommit         ( 0 ),
      pfSetRetryCommits     ( 0 ),
      pfGetUniqueID         ( 0 ),
      pfTransLogLast        ( 0 ),
      pfTransLogHeader      ( 0 ),
      pfTransLogDetail      ( 0 ),
      pfChangeInitData      ( 0 ),
      pfDbValidate          ( 0 ),
      pfShutdown            ( 0 ),
      pfSyncPointSave       ( 0 ),
      pfSyncPointAsyncSave  ( 0 ),
      pfSyncPointAsyncAbort ( 0 ),
      pfSyncPointCleanup    ( 0 ),
      pfGetTime             ( 0 ),
      pfSetTime             ( 0 ),
      pfTransMap            ( 0 ),
      pfGetObjCheckSum      ( 0 ),
      pfFileOpen            ( 0 ),
      pfFileClose           ( 0 ),
      pfFileInfoGet         ( 0 ),
      pfFileRead            ( 0 ),
      MemUsed               ( 0 ),
      DbOperator            ( SDB_DB_LEAF ),
      Left                  ( 0 ),
      Right                 ( 0 ),
      DbUpdateNowPrivate    ( 0 ),
      DbPhysical            ( 0 ),
      SubDbUniqueID         ( 0 ),
      ParentDb              ( 0 ),
      SecDbSearchPath		( 0 ),
      DbSingleton           ( 0 ),
      DbDepth               ( 0 ),
      FilteredClasses       ( 0 ),
      MaxMemSize            ( 0 ),
      TemporaryDS           ( 0 )
	{}

	SDB_DB* Next;							// Next database

	char const*
			DbName;							// Database name

	SDB_DB_ID
			DbID;							// Database ID number

	int		DbType,							// Database type
			DbAttributes,					// Database attributes
			OpenCount,						// Number of times attach was called
			PreCacheDepth,					// Depth of precache calls
			StreamVersion;					// Based on Attributes/OldStream

	HASH
			*CacheHash,   					// Object cache
			*PreCacheHash,					// Object precache
			*PageTableHash;					// Page tables for huge objects

	int		DeadPoolCount,					// Number of used deadpool elements
			DeadPoolSize;					// Total size of deadpool

	SDB_DEAD_POOL_ELEMENT
			*DeadPool;						// Deadpool of unreferenced elements

	SDB_TRANS_PART
			*TransHead,
			*TransTail;						// Current transaction

	int		TransDepth;

	unsigned long
			TransFlags;

	void	*Handle,						// Handle for use by database functions
			*HandleIndex,					// Handle for use by index functions
			*HandleTransaction,				// Handle for use by transaction functions
			*HandleLock;					// Handle for use by lock functions

	SDB_DB_INIT_FUNC
			pfInit;							// Database initialization function

	int		(*pfGetByName)(			SDB_DB *pDb, const SDB_SEC_NAME *Name, SDB_STORED_OBJECT *Ret ),
			(*pfGetFromSyncPoint)(	SDB_DB *pDb, const SDB_SEC_NAME *Name, int SyncPointOffset, SDB_STORED_OBJECT *Ret ),
			(*pfGetManyByName)(		SDB_DB *pDb, const SDB_SEC_NAME **Names, int Count, SDB_STORED_OBJECT **RetArray );

	int		(*pfCreate)(			SDB_DB *pDb ),
			(*pfOpen)(				SDB_DB *pDb ),
			(*pfSetTimeout)(		SDB_DB *pDb, int Seconds ),
			(*pfClose)(				SDB_DB *pDb ),
			(*pfNameLookup)(	  	SDB_DB *pDb, int GetType, const SDB_SEC_NAME *Name, SDB_SEC_NAME_ENTRY *SecNames, int TableSize ),
			(*pfNameLookupByType)(	SDB_DB *pDb, int GetType, const SDB_SEC_NAME *Name, SDB_SEC_TYPE SecType, SDB_SEC_NAME_ENTRY *SecNames, int TableSize ),
			(*pfGetDiskInfo)(		SDB_DB *pDb, const SDB_SEC_NAME *Name, SDB_DISK *Disk );

	int		(*pfIndexOpen)(			SDB_INDEX *Index ),
			(*pfIndexClose)(		SDB_INDEX *Index ),
			(*pfIndexGet)(			SDB_INDEX_POS *IndexPos, int GetType ),
			(*pfIndexGetByName)(	SDB_INDEX_POS *IndexPos, int GetType, const char *SecName ),
			(*pfIndexGetSecurity)(	SDB_INDEX_POS *IndexPos, int GetType,
									SDB_STORED_OBJECT *ReturnObject ),
			(*pfIndexPosCreate)(	SDB_INDEX_POS *IndexPos ),
			(*pfIndexPosDestroy)(	SDB_INDEX_POS *IndexPos );

	SDB_TRANS_ID
			(*pfTransCommit)(		SDB_DB *pDb, SDB_TRANS_PART *TransList );
	int		(*pfSetRetryCommits)(	SDB_DB *pDb, int AllowRetries );

	SDB_UNIQUE_ID
			(*pfGetUniqueID)(		SDB_DB *pDb );

	SDB_TRANS_ID
			(*pfTransLogLast)(		SDB_DB *pDb );

	int		(*pfTransLogHeader)(	SDB_DB *pDb, SDB_TRANS_ID TransID, SDB_TRANS_HEADER *Header );

	SDB_TRANS_PART
			*(*pfTransLogDetail)(	SDB_DB *pDb, SDB_TRANS_HEADER *Header );

	void	(*pfChangeInitData)(	SDB_DB *pDb );

	int		(*pfDbValidate)(		SDB_DB *pDb );

	int		(*pfShutdown)(			SDB_DB *pDb );

	int		(*pfSyncPointSave)(		SDB_DB *pDb ),
			(*pfSyncPointAsyncSave)(SDB_DB *pDb ),
			(*pfSyncPointAsyncAbort)(SDB_DB *pDb ),
			(*pfSyncPointCleanup)(	SDB_DB *pDb, int MaxNumberToRetain, int DeleteTransLogs );

	time_t	(*pfGetTime)(			SDB_DB *pDb );

	int		(*pfSetTime)(			SDB_DB *pDb, time_t Time );

	SDB_TRANS_MAP
			*(*pfTransMap)(			SDB_DB *pDb );

	SDB_OBJECT_CHECK_SUM
			*(*pfGetObjCheckSum)(	SDB_DB *pDb, SDB_SEC_NAME const* SecName );

	SDB_FILE_HANDLE
			(*pfFileOpen)(			SDB_DB *pDb, char const* FileName );

	int		(*pfFileClose)(			SDB_DB *pDb, SDB_FILE_HANDLE File ),
			(*pfFileInfoGet)(		SDB_DB *pDb, SDB_FILE_HANDLE File, SDB_FILE_INFO *Info );

	SDB_FILE_SIZE
			(*pfFileRead)(			SDB_DB *pDb, SDB_FILE_HANDLE File, void *Data, SDB_FILE_SIZE Length, off_t Position );
			
	long	MemUsed;
	
	SDB_DB_OPERATOR
			DbOperator;				// The operation on Left and Right that this SDB_DB represents

	SDB_DB  *Left,
			*Right,
			*DbUpdateNowPrivate,	// Db which gets updated for virtual datbaases
			*DbPhysical;			// Physical Db, gets sent all the database object messages

	INT32							// If this is a subdb then it has a unique number.
			SubDbUniqueID;

	SDB_DB	*ParentDb;				// FIX - Moribund

	SDB_DB_SEARCH_PATH
			*SecDbSearchPath;		// FIX - Moribund

	SDB_DBSET
			*DbSingleton;			// the db set [ Db ]

	int		DbDepth;				// depth of Db Tree

/*
**	This is for ClassFilter{} dbs
*/
	HASH
			*FilteredClasses;		// Classes served by this db

	unsigned long
			MaxMemSize;

	SDB_DB	*TemporaryDS;				// The temporary diddle scope to be used for this DB

	CC_NS(std,string)
			FullDbName;					// Database name

	CC_NS(std,string)
			Region,						// Global region (NYC/LDN/TKO/SGP/HKG)
			Location,					// Physical location of the database
			MirrorGroup,				// Mirror group of the database
			DllPath,					// Dll path
			FuncName,					// Main database function
			DatabasePath,				// Database path
			Argument,					// Optional argument
			LockServer;					// Server where locks are kept	

	CC_NS(std,string)
			Qualifier;					// Qualifier from dbname: DbName/Qualifiers

	DT_DIDDLE_SCOPE*
			DiddleScope()
	{
		if( Left && Left->DbOperator == SDB_DB_DIDDLE_SCOPE )
			return static_cast<DT_DIDDLE_SCOPE*>( Handle );
		else
			return 0;
	}

	bool IsVirtual() const
	{
		return (DbOperator != SDB_DB_LEAF) && (DbOperator != SDB_DB_SUBDB);
	}

	struct db_less { 
    	bool operator() ( SDB_DB const *const p, SDB_DB const *const q ) const
        {
            return p->FullDbName < q->FullDbName; // use the default string comparator
        }
	};

};



/*
**	FIX - Moribund
**	Search path for databases in which to look for objects (one list per db;
**	typically and by default just itself)
*/

struct SDB_DB_SEARCH_PATH
{
	SDB_DB_SEARCH_PATH
			*Next;
			
	SDB_DB	*Db;
	
};


/*
**	Variables
*/

EXPORT_C_SECDB void
		*SecDbDatabaseBuffer;		// General purpose database buffer

extern int
		SecDbConfigAttributes;		// SDB_DB_ATTRIB from secdb.cfg

EXPORT_C_SECDB HASH
		*SecDbDatabaseIDHash;		// Hash table of databases by ID

EXPORT_C_SECDB int 
		DbAttach( SDB_DB *Db, int DeadPoolSize, int Flags );


/*
**	Prototypes
*/

int		SecDbCreateDeadPool(			SDB_DB *pDb, int DeadPoolSize );

EXPORT_C_SECDB SDB_DB
		*SecDbDbFromDbIDFunc(			SDB_DB_ID_WITH_FLAGS DbID );

inline SDB_DB *SecDbDbFromDbID( SDB_DB_ID_WITH_FLAGS DbID )
{
	SDB_DB_ID
			SearchID = DbID & ~(SDB_REFRESH_CACHE | SDB_CACHE_ONLY | SDB_IGNORE_PATH );

	SDB_DB	*pDb;

	if( SearchID == SDB_ROOT_DATABASE )
		return SecDbRootDb;

	if( !( pDb = (SDB_DB *) HashLookup( SecDbDatabaseIDHash, (HASH_KEY) SearchID )))
		return (SDB_DB *) ErrN( ERR_DATABASE_INVALID, "SecDbDbFromDbID( %d ): unknown Db", SearchID );
	if( pDb->OpenCount > 0 )
		return pDb;

	return (SDB_DB *) ErrN( ERR_DATABASE_INVALID, "SecDbDbFromDbID( %s ): database is closed", pDb->FullDbName.c_str() );
}

EXPORT_C_SECDB SDB_DB
		*SecDbDbRegister(				SDB_DB *NewDb );

#if defined( IN_SECTRANS_H )
EXPORT_C_SECDB void
		SecDbTransPartFree(				SDB_TRANS_PART *TransPart );
#endif

#endif
