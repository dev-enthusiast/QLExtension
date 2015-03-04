/****************************************************************
**
**	sectrans.h - SecDb transaction information
**
**	Copyright 1993-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/sectrans.h,v 1.59 2004/11/30 20:14:58 khodod Exp $
**
****************************************************************/

#if !defined( IN_SECTRANS_H )
#define IN_SECTRANS_H

#include	<date.h>
#include	<secindex.h>
#include	<secfile.h>
#include	<secdrv.h>


/*
**	Types which can be overridden by driver.  Pointers to these values
**	are provided in some of the SDB_TRANS_PART.Data.* structures.
*/

#if !defined( SDB_DRV_INDEX )
#define		SDB_DRV_INDEX			SDB_INDEX
#endif
#if !defined( SDB_DRV_RECORD )
#define		SDB_DRV_RECORD			void
#endif
#if !defined( SDB_DRV_INCREMENTAL )
#define		SDB_DRV_INCREMENTAL		void
#endif
#if !defined( SDB_DRV_INDEX_VALUES )
#define		SDB_DRV_INDEX_VALUES	SDB_INDEX_VALUES
#endif


/*
**	Define constants
*/

SDB_TRANS_ID const SDB_TRANS_ID_ERROR   =  (0L);
SDB_TRANS_ID const SDB_TRANS_ID_UNKNOWN = (-1L);


/*
**	Define transaction map layout
*/

struct SDB_TRANS_MAP
{
	DATE	Date;

	short	Version;

	SDB_TRANS_ID
			TransID;

	SDB_TRANS_MAP*
			Next;

};


/*
**	Define transaction types
**
**	DO NOT CHANGE THE ORDER OF THIS ENUM -- THESE VALUES ARE STORED ON DISK
*/

enum SDB_TRANS_TYPE
{
	SDB_TRAN_BEGIN,
	SDB_TRAN_COMMIT,
	SDB_TRAN_ABORT,
	SDB_TRAN_INSERT,
	SDB_TRAN_UPDATE,
	SDB_TRAN_DELETE,
	SDB_TRAN_RENAME,
	SDB_TRAN_INDEX_INSERT,
	SDB_TRAN_INDEX_UPDATE,
	SDB_TRAN_INDEX_DELETE,
	SDB_TRAN_INDEX_RENAME,
	SDB_TRAN_INDEX_CREATE,
	SDB_TRAN_INDEX_DESTROY,
	SDB_TRAN_INCREMENTAL,
	SDB_TRAN_INDEX_INCREMENTAL,
	SDB_TRAN_FILE_DELETE,
	SDB_TRAN_FILE_WRITE,
	SDB_TRAN_FILE_INFO_SET,
	SDB_TRAN_SOURCE_INFO,

	// For self-describing data.
	SDB_TRAN_SELF_DESC_DATA,

	SDB_TRAN_LAST

};


//	Define transaction flags
//	DO NOT CHANGE THE ORDER OF THIS ENUM -- THESE VALUES ARE STORED ON DISK

#define 	SDB_TRAN_FLAG_ALLOW_ASYNC 	( (unsigned long) 0x0001 )


/*
**	Structure for transaction header info
*/

struct SDB_TRANS_HEADER
{
	SDB_TRANS_ID
			TransID;			// Transaction id

	int32_t	DetailKey1,			// Where to find the transaction detail
			DetailKey2,
			DetailParts,		// Number of parts in transaction detail
			DetailBytes;		// Number of bytes in transaction detail

	time_t	GMTime;				// Time transaction was committed

	char	UserName[ SDB_USER_NAME_SIZE ],
			ApplicationName[ SDB_APP_NAME_SIZE ];

	char	SecName[ SDB_SEC_NAME_SIZE ];	// Primary security

	SDB_SEC_TYPE
			SecType;			// Type of primary security

	SDB_TRANS_TYPE
			TransType;			// Primary transaction type

	SDB_DB_ID
			DbID;				// Originating database

	SDB_TRANS_TYPE
			Type;				// SDB_TRAN_COMMIT or SDB_TRAN_ABORT

	SDB_TRANS_ID
			SourceTransID;		// Transaction ID in originating database

	uint32_t
			NetworkAddress;		// Network address

    // Use an integer field instead of bit fields, so that additional flags
    // do not require code changes to the osecserv and secdb projects.

	uint32_t
			TransFlags;

    // These bits are saved to disk as zeros.

	int32_t ReservedForFutureUse[ 5 ];
};


/*
**	Structure used to hold transaction parts
**
**	Values flagged "Stored" are primary input/output values from all
**	transaction functions, but are not necessarily the values stored
**	in the transaction log.  For example, IndexID rather than IndexName
**	is usually stored in the transaction log.
*/

struct SDB_TRANS_PART
{
	// We have to resort to a memset, because we can't have constructors
	// for union structures. 

	SDB_TRANS_PART() { memset( this, 0, sizeof( SDB_TRANS_PART ) ); }

	SDB_TRANS_PART
			*Next,
			*Prev;

	SDB_TRANS_TYPE
			Type;

	SDB_TRANS_TYPE
			ActionType;		// Provided for driver

	void	*Handle;		// Handle reserved for driver implementation

	union
	{
		struct
		{
			char	SecName[ SDB_SEC_NAME_SIZE ];

			SDB_SEC_TYPE
					SecType;

			SDB_TRANS_TYPE
					TransType;

			SDB_DB_ID
					DbID;

			SDB_TRANS_ID
					SourceTransID;

			unsigned long
					TransFlags;

		} Begin;

		struct
		{
			char	UserName[ SDB_USER_NAME_SIZE ],
					ApplicationName[ SDB_APP_NAME_SIZE ];

			unsigned long
					NetworkAddress;

		} SourceInfo;

		struct
		{
			SDB_STORED_OBJECT
					StoredObject;						// Stored

			SDB_DISK
					OldSdbDisk;							// Necessary for restore if commit fails

			SDB_OBJECT
					*SecPtr;							// Necessary for restore if commit fails

			SDB_DRV_RECORD
					*ObjRecord;							// Provided for driver

		} Op;

		struct
		{
			SDB_DISK
					SdbDisk;							// Stored

			unsigned short
					MsgMemSize;							// Stored

			void	*MsgMem;							// Stored

			short	Direction;							// Stored: 1 or -1

			SDB_DISK
					OldSdbDisk;							// Stored, necessary for restore if commit fails

			SDB_OBJECT
					*SecPtr;							// Necessary for restore if commit fails

			SDB_DRV_INCREMENTAL
					*Incremental;						// for driver

		} Incremental;

		struct
		{
			SDB_DISK
					SdbDisk;							// Stored

			short	IgnoreErrors;						// Stored

		} Delete;

		struct
		{
			SDB_DISK
					NewSdbDisk,							// Stored
					OldSdbDisk;							// Stored

			SDB_OBJECT
					*SecPtr;							// Necessary for restore if commit fails

		} Rename;

		struct SdbTransIndexOpStructure
		{
			SDB_DRV_INDEX_VALUES
					*Data;

		} IndexOp;

		struct SdbTransIndexIncrementalStructure
		{
			char	SecName[ SDB_SEC_NAME_SIZE ];		// Stored

			SDB_DRV_INDEX
					*Index;								// Stored

		} IndexIncremental;

		struct SdbTransIndexDeleteStructure
		{
			char	IndexName[ SDB_INDEX_NAME_SIZE ],	// Stored
					SecName[ SDB_SEC_NAME_SIZE ];		// Stored

			short	IgnoreErrors;						// Stored

			SDB_DRV_INDEX
					*Index;								// Provided for driver

			SDB_INDEX_ID
					IndexID;							// for driver

		} IndexDelete;

		struct SdbTransIndexRenameStructure
		{
			char	IndexName[ SDB_INDEX_NAME_SIZE ],	// Stored
					OldSecName[ SDB_SEC_NAME_SIZE ],	// Stored
					NewSecName[ SDB_SEC_NAME_SIZE ];	// Stored

			SDB_DRV_INDEX
					*Index;								// Provided for driver

			SDB_INDEX_ID
					IndexID;							// for driver

		} IndexRename;

		struct
		{
			SDB_INDEX_DESCRIPTOR
					*IndexDescriptor;					// Stored

		} IndexCreate;

		struct
		{
			char	IndexName[ SDB_INDEX_NAME_SIZE ];	// Stored

			SDB_DRV_INDEX
					*Index;								// for driver

		} IndexDestroy;

		struct
		{
			SDB_FILE_HANDLE
					File;								// Stored

		} FileDelete;

		struct
		{
			SDB_FILE_HANDLE
					File;								// Stored

			const void
					*Data;								// Stored

			SDB_FILE_SIZE
					Length,								// Stored
					Position;							// Stored

		} FileWrite;

		struct
		{
			SDB_FILE_HANDLE
					File;								// Stored

			SDB_FILE_INFO
					Info;								// Stored

		} FileInfoSet;

		struct
		{
			size_t DataSize;							// Stored

			void *Data;									// Stored

		} SelfDesc;

	} Data;

};


/*
**	String tables
*/

EXPORT_C_SECDB char const*
		SecDbTransStrTab[];


/*
**	Prototypes
*/

EXPORT_C_SECDB SDB_TRANS_ID
		SecDbTransLogLast(	  			SDB_DB *pDb );

EXPORT_C_SECDB void
		SecDbTransMapFree(				SDB_TRANS_MAP *TransMap );

EXPORT_C_SECDB SDB_TRANS_MAP
		*SecDbTransMap(					SDB_DB *pDb );

EXPORT_C_SECDB int
		SecDbTransLogHeader( 			SDB_DB *pDb, SDB_TRANS_ID TransID, SDB_TRANS_HEADER *Header );

EXPORT_C_SECDB SDB_TRANS_PART
		*SecDbTransLogDetail(			SDB_DB *pDb, SDB_TRANS_HEADER *Header );

EXPORT_C_SECDB const SDB_TRANS_PART
		*SecDbTransGetCurrent(			SDB_DB *pDb );

EXPORT_C_SECDB int
		SecDbTransPartInit(				SDB_TRANS_PART *Part, SDB_TRANS_TYPE Type );

EXPORT_C_SECDB void
		SecDbTransFree(					SDB_TRANS_PART *TransList );

EXPORT_C_SECDB int
		SecDbTransBegin(				SDB_DB *pDb, const char *Description ),
		SecDbTransBeginWithFlags(		SDB_DB *pDb, const char *Description, unsigned long Flags ),
		SecDbTransCommit(				SDB_DB *pDb ),
		SecDbTransAbort(				SDB_DB *pDb ),
		SecDbTransDescriptionSet(       SDB_DB *pDb, const char *Description ),
		SecDbTransDepth(				SDB_DB *pDb ),
		SecDbTransPartCount(			SDB_DB *pDb ),
		SecDbTransCommitConditional(	SDB_DB *pDb, int MaxCount, long MaxSize, int Force, int *TransCommitted, int *TransFailed ),
		SecDbTransCommitForce(			SDB_DB *pDb );

EXPORT_C_SECDB unsigned long
		SecDbTransFlags(				SDB_DB *pDb );

EXPORT_C_SECDB long
		SecDbTransSize(					SDB_DB *pDb );

EXPORT_C_SECDB char *
		SecDbTransDescription(			SDB_DB *pDb );

EXPORT_C_SECDB int
		SecDbTransMimic(				SDB_DB *SourceDatabase,
										SDB_DB *TargetDatabase,
										SDB_TRANS_HEADER *Header,
										SDB_TRANS_PART *TransList,
										SDB_TRANS_ID *TransId );

EXPORT_C_SECDB int
		SecDbRefreshSecurityDatabase(	SDB_DB *Database );

EXPORT_C_SECDB DT_VALUE
		*SecDbTransMapToArray(			SDB_TRANS_MAP *MapList ),
		*SecDbTransListToArray(			SDB_TRANS_PART *TransList, int RawMemFlag ),
		*SecDbTransListToStructure(		SDB_TRANS_PART *TransList ),
		*SecDbTransHeaderToStructure(	SDB_TRANS_HEADER *Header ),
		*SecDbTransListToObjectNames(	SDB_TRANS_PART *TransList );

EXPORT_C_SECDB SDB_TRANS_ID
		SecDbLastTransFromDiskInfo(		SDB_DISK *DiskInfo );

EXPORT_C_SECDB int		
		SecDbTransactionAdd(			SDB_DB *pDb, SDB_TRANS_TYPE Type, 
										SDB_TRANS_PART *TransPart,
										SDB_TRANS_ID *TransId );
#endif
