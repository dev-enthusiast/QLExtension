/* $Header: /home/cvs/src/osecserv/src/secserv/secserv.h,v 1.72 2014/09/03 18:58:21 c038571 Exp $ */
/****************************************************************
**
**	SECSERV.H	- SecDb server
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.72 $
**
****************************************************************/

#if !defined( IN_SECSERV_H )
#define IN_SECSERV_H

#include	<tcpip.h>
#include	<kool_ade.h>
#include	<heap.h>
#include	<bstream.h>
#include	<secdb.h>
#include	<secindex.h>
#include	<secdrv.h>
#include	<secdrive.h>
#include	<sectrans.h>
#include	<secfile.h>
#include	<skiplist.h>

// Use the POSIX large file API if possible.
#ifdef __unix
#   define BIG_OFFSET           off64_t
#   define BIG_FOPEN            fopen64
#   define BIG_FSEEKO           fseeko64
#   define BIG_FTELLO           ftello64
#   define BIG_TRUNCATE         truncate64
#   define BIG_OFFSET_FORMAT    "%lld"
#elif defined WIN32 //covers WIN32, WIN64
#	define BIG_OFFSET		    int64_t
#	define BIG_FOPEN		    fopen
#	define BIG_FSEEKO		    _fseeki64
#	define BIG_FTELLO		    _ftelli64
#	define BIG_TRUNCATE	        truncate_not_supported
#	define BIG_OFFSET_FORMAT	"%I64d"
#else 
#   error Compiler not supported.
#endif

#   define READ_XHEADER_OFFSET( xheader )                                  \
		( (uint32_t) ( xheader ).DetailKey2 +							   \
          ( ( ( BIG_OFFSET ) ( xheader ).ReservedForFutureUse[0] ) << 32 ) )

#   define WRITE_XHEADER_OFFSET( xheader, offset )                         \
        ( xheader ).DetailKey2 = ( offset & 0xffffffffU );                 \
		( xheader ).ReservedForFutureUse[0] = ( offset >> 32 );



// When running on a Unix box, activate the asynchronous syncpoint code.

#if defined( __unix )
#	define ASYNC_SYNC_POINT
#endif


// Server version.

#define	SRV_SERVER_VERSION		1
#define	SRV_SERVER_REVISION		8


/*
**	Define constants
*/

#define	SRV_STREAM_SIZE				35000U

#if defined( __unix )
#define SRV_MAX_SEND				SRV_STREAM_SIZE
#else
#define	SRV_MAX_SEND				32000U		// This is stupidity required by brain-dead IBM TCP/IP
#endif

#define	SRV_APP_NAME_SIZE			32
#define	SRV_USER_NAME_SIZE			32
#define	SRV_MAX_INDICES				256
#define	SRV_MAX_INDEX_DESCRIPTORS	512
#define	SRV_MAX_INDEX_PARTS			16
#define	SRV_MAX_TRANS_MAP_ENTRIES	500
#define	SRV_SERVICE_NAME_SIZE		128
#define	SRV_HOST_NAME_SIZE			32
#define	SRV_TIMEOUT_SECONDS			3L


/*
**	Define message types
*/

// FIXME: This is one of three data structures that must be modified in
// parallel -- the SRV_MSG enumeration in secserv.h, MsgFuncTable in
// srv_pkt.c, and StrTabSecSrvMessages in srv_str.c.  All these should be put
// in one place, and the second two should be merged into one data structure.

enum SRV_MSG
{
	SRV_MSG_UNDEFINED,
	SRV_MSG_ATTACH,
	SRV_MSG_CHANGE_INIT_DATA,
	SRV_MSG_GET_UNIQUE_ID,
	SRV_MSG_GET_BY_NAME,
	SRV_MSG_GET_DISK_INFO,
	SRV_MSG_INSERT,
	SRV_MSG_UPDATE,
	SRV_MSG_DELETE,
	SRV_MSG_RENAME,
	SRV_MSG_INCREMENTAL,
	SRV_MSG_NAME_LOOKUP,
	SRV_MSG_NAME_LOOKUP_BY_TYPE,
	SRV_MSG_TRANS_BEGIN,
	SRV_MSG_GET_STATISTICS,
	SRV_MSG_GET_TIME,
	SRV_MSG_SET_TIME,
	SRV_MSG_INDEX_CREATE,
	SRV_MSG_INDEX_DESTROY,
	SRV_MSG_INDEX_OPEN,
	SRV_MSG_INDEX_CLOSE,
	SRV_MSG_INDEX_INSERT,
	SRV_MSG_INDEX_DELETE,
	SRV_MSG_INDEX_UPDATE,
	SRV_MSG_INDEX_RENAME,
	SRV_MSG_INDEX_INCREMENTAL,
	SRV_MSG_INDEX_GET,
	SRV_MSG_DEBUG,
	SRV_MSG_SYNC_POINT_SAVE,
	SRV_MSG_SYNC_POINT_CLEANUP,
	SRV_MSG_DB_VALIDATE,
	SRV_MSG_GET_CONN_INFO,
	SRV_MSG_KILL_CONNECTION,
	SRV_MSG_TRANS_GET_HEAD,
	SRV_MSG_TRANS_GET_PARTS,
	SRV_MSG_TRANS_GET_STATE,
	SRV_MSG_GET_INDEX_DESCRIPTOR,
	SRV_MSG_SHUTDOWN,
	SRV_MSG_ERROR_LOG_GET_STATE,
	SRV_MSG_ERROR_LOG_GET_PART,
	SRV_MSG_DB_CHECK_SUM,
	SRV_MSG_GET_FROM_SYNCPOINT,
	SRV_MSG_INDEX_CHECK_SUM,
	SRV_MSG_TRANS_MAP,
	SRV_MSG_OBJECT_CHECK_SUM,
	SRV_MSG_FILE_OPEN,
	SRV_MSG_FILE_CLOSE,
	SRV_MSG_FILE_READ,
	SRV_MSG_FILE_INFO_GET,
	SRV_MSG_SYNC_POINT_ASYNC_SAVE,
	SRV_MSG_SYNC_POINT_ASYNC_ABORT,
	SRV_MSG_GET_UTILIZATION,
	SRV_MSG_INDEX_GET_SECURITY,
	SRV_MSG_SOURCE_INFO,
	SRV_MSG_SELF_DESCRIBING,
	SRV_MSG_INDEX_GET_BY_NAME,
	SRV_MSG_COLLECT_CLIENT_STATS,    

	SRV_MSG_END_OF_TABLE_MARKER

};


/*
**	Define return codes for packet functions
*/

enum SRV_PKT_RESULT
{
	SRV_PKT_OK		= 0x100,	// Function worked correctly
	SRV_PKT_ERROR	= 0x101,	// Function failed, send error to client
	SRV_PKT_DETACH	= 0x102,	// Connection hosed, disconnect client
	SRV_PKT_EXIT	= 0x103		// Reply to client and exit

};


/*
**	Define types
*/

typedef int					SRV_CONNECTION_ID;
typedef unsigned short		SRV_MSG_TYPE;
typedef unsigned short		SRV_MSG_SIZE;


/*
**	Structure used to hold an individual database's information
*/

struct SRV_DATABASE
{
	SKIP_LIST
			*IndexByName,
			*IndexByType;

};


/*
**	Structure used to hold individual database records (objects)
*/

struct SRV_STORED_OBJECT
{
	SDB_DISK
			SdbDisk;

	unsigned short
			MemSize;

	void	*Mem;

};

struct SRV_RECORD
{
	SRV_STORED_OBJECT
			Obj;

	BIG_OFFSET
			FilePos;

};

#define SDB_STORED_OBJECT_FROM_SRV( Sdb_, Srv_ )		{ (Sdb_)->SdbDisk = (Srv_)->SdbDisk; (Sdb_)->MemSize = (Srv_)->MemSize; (Sdb_)->Mem = (Srv_)->Mem; }
#define SDB_STORED_OBJECT_TO_SRV( Sdb_, Srv_ )			{ (Srv_)->SdbDisk = (Sdb_)->SdbDisk; (Srv_)->MemSize = (unsigned short) (Sdb_)->MemSize; (Srv_)->Mem = (Sdb_)->Mem; }


/*
**	Structures for holding indices
*/

struct SRV_INDEX
{
	SDB_INDEX_DESCRIPTOR
			*Descriptor;

	int32_t	DescriptorNumber;

	char	FileName[ 32 ];

	int		UseCount;

	int		ModificationCount;

	short	IndexNumber;

	SKIP_LIST
			*IndexByName,
			*IndexByData;

	HEAP	*RecHeap;

};


/*
**	Structures used to hold pending transaction components
*/

// FIX - Should be split into a structure containing unions
struct SRV_TRANS
{
	SRV_TRANS
			*Next,
			*Prev;

	int32_t	Type,
			ActionType;

	SRV_RECORD
			*SrvRecord;

	SDB_DISK
			OldSdbDisk,
			NewSdbDisk;

	char	SecName[	SDB_SEC_NAME_SIZE ],
			NewName[	SDB_SEC_NAME_SIZE ],
			IndexName[	SDB_INDEX_NAME_SIZE ];

	int32_t	DescriptorNumber;

	SRV_INDEX
			*SrvIndex;

	SDB_INDEX_DESCRIPTOR
			*IndexDescriptor;

	int32_t	RecordLength;

	void	*RecBuf,
			*OldRecBuf;

	SDB_DRIVER_INCREMENTAL
			*Incremental;

	int32_t	IgnoreErrors;

	union
	{
		struct
		{
			SDB_DISK
					SdbDisk;
					
			uint32_t
					MsgMemSize;
					
			void	*MsgMem;
			
			int32_t	Direction;

		} Incremental;

		struct
		{
			SDB_FILE_HANDLE
					Handle;

			void	*Data;

			int32_t	Length,
					Position;

			int32_t LastAccess,
					LastUpdate;

		} File;

		struct
		{
			uint32_t
					DataSize;

			void	*Data;

		} SelfDescData;

	} Data;
	
	// FIX - yucky
	SRV_STORED_OBJECT
			SrvStoredObject;

};


class SRV_VERSION
{
public:

	short	Version;
	short	Revision;

	SRV_VERSION( short V = 0, short R = 0 ) : Version( V ), Revision( R ) {}

	inline bool operator <= ( const SRV_VERSION & Other ) const 
	{
		return( ( ( Version << 8 ) | Revision ) <=
				( ( Other.Version << 8 ) | Other.Revision ) );
	}

	inline bool operator >= ( const SRV_VERSION & Other ) const 
	{
		return( ( ( Version << 8 ) | Revision ) >= 
				( ( Other.Version << 8 ) | Other.Revision ) );
	}
};


/*
**	Define structure used to hold connection information
*/

struct SRV_CONNECTION
{

    // Amount of time the server has spent processing this
    // client's requests.
    
    double
		    ServerTime;

	unsigned char
			*StreamInput,
			*StreamOutput,
			*StreamOutputPos;

	char	ApplicationName[	SRV_APP_NAME_SIZE + 1 ],
			UserName[			SRV_USER_NAME_SIZE + 1 ];

	time_t	ConnectionTime,
			LastActivityTime;

	SRV_CONNECTION_ID
			ConnectionID;

	unsigned short
			PendingMsgSize,
			BytesInInput,
			BytesInOutput;

	SOCKET	Socket;

	unsigned long
			NetworkAddress;

	SRV_TRANS
			*TransHead,
			*TransTail;

	SDB_TRANS_HEADER
			TransInfo;

	int		PendingPartCount,
			ErrorCode;

	const char
			*ErrorText;

	int		MsgCount[			SRV_MSG_END_OF_TABLE_MARKER ],
			AckTrueCount[		SRV_MSG_END_OF_TABLE_MARKER ];

	int		SourceInfoAvailable;

	SRV_VERSION
			ClientVersion;

};


/*
**	Structure used to keep track of syncpoint state
*/

struct SRV_STATE
{
	int		Year,
			Month,
			Day;

	int		Version;

};


#ifdef ASYNC_SYNC_POINT

// Is the server process currently forked?

extern int
		ServerIsForked;

// When forked for an asynchronous syncpoint, the ID of the process performing
// the syncpoint.

extern pid_t
		ChildProcessId;

#endif


/*
**	Structure used to hold unique ID information on disk
*/

struct SRV_UNIQUE_ID_INFO
{
	SDB_UNIQUE_ID
			UniqueID;

	int32_t	ShutdownFlag;

};


/*
**	Structures used for checksum functions
*/

struct SRV_DB_CHECK_SUM
{
	SDB_SEC_TYPE
			Type;

	uint32_t
			RecordCount,
			RecordByteCount,
			RecordByteSum;

	uint16_t
			RecordCrc,
			RecordSizeCrc,
			NameCrc;

};

struct SRV_INDEX_CHECK_SUM
{
	char	IndexName[ SDB_INDEX_NAME_SIZE ];

	uint32_t
			RecordCount;

	uint16_t
			RecordCrc;

}; 


/*
**	Variables
*/

extern SRV_DATABASE
		SrvDatabase;

extern SRV_INDEX
		*SrvIndexTable[ SRV_MAX_INDICES ],
		*SrvIndexCurrent;

extern SDB_INDEX_DESCRIPTOR
		*SrvIndexDescriptorTable[ SRV_MAX_INDEX_DESCRIPTORS ];

extern SRV_STATE
		SrvState;

extern time_t
		SrvStartTime,
		SrvCurrentTime;

extern int
		SrvServiceOffset;

extern HEAP
		*SrvRecordHeap,
		*SrvTransHeap;

extern int
		SrvTransactionStart,
		SrvTransactionNumber,
		SrvMsgCount[	SRV_MSG_END_OF_TABLE_MARKER ],
		SrvMsgAckTrue[	SRV_MSG_END_OF_TABLE_MARKER ];

extern int
		SrvShutdownRequested,
		SrvShutdownInteractive;

DLLEXPORT const char
		*StrTabSecSrvMessages[];

extern int
		SrvLaggedLoad;

extern int
		ChangeKeepClientStats,
		KeepClientStats;
    

extern double
		KeepStatsStopTime;
extern FILE
		*fpErrorLog;


/*
**	Macros to operate on SrvRecords
*/

#define	SRV_RECORD_RESOLVE_MEM(sr)		\
		(( NULL == (sr)->Obj.Mem && 0 != (sr)->Obj.MemSize ) ? SrvRecordLaggedLoad(sr) : TRUE)


/*
**	Macros to operate on transaction lists
*/

#define	SRV_TRANS_UNLINK(c,t)	\
		{										\
			if( (t)->Prev )						\
				(t)->Prev->Next = (t)->Next;	\
			else								\
				(c)->TransHead = (t)->Next;		\
			if( (t)->Next )						\
				(t)->Next->Prev = (t)->Prev;	\
			else								\
				(c)->TransTail = (t)->Prev;		\
		}

#define	SRV_TRANS_APPEND(c,t)	\
		{										\
			if( (t)->Prev = (c)->TransTail )	\
				(t)->Prev->Next = (t);			\
			else								\
				(c)->TransHead = (t);			\
			(c)->TransTail = (t);				\
			(t)->Next = NULL;					\
		}


/*
**	Prototype functions
*/

void	SrvReply(					SRV_CONNECTION *SrvConn, int ReplySize ),
		SrvTransLogClose(			void ),
		SrvMessage(					const char *Format, ... ) ARGS_LIKE_PRINTF(1);

char	*SrvStateDirectory(			const SRV_STATE *State );

int		SrvPacketProcess(			SRV_CONNECTION *SrvConn ),
		SrvInitialize(				void ),
		SrvSyncPointSave(			void ),
		SrvSyncPointLoad(			char *Directory ),
		SrvSyncPointCleanup(		int MaxNumberToRetain, int DeleteTransLogs ),
		SrvSyncPointDelete(			const char *Directory, int DeleteTransLogs ),
		SrvConfigSave(				const char *Directory ),
		SrvDirectoryConfigSave(		const char *Directory );

int		SrvConnectionDelete(		SOCKET Socket ),
		SrvConnectionCount(			void );
void	SrvConnectionClearStats(    void );
SRV_CONNECTION
		*SrvConnectionFromIndex(	int Index ),
		*SrvConnectionFromSocket(	SOCKET Socket );

int		SrvDatabaseLoad(			char *Directory ),
		SrvDatabaseSave(			char *DatabaseFileName, char *OffsetsFileName ),
		SrvIndexCreate(				SDB_INDEX_DESCRIPTOR *Descriptor ),
		SrvIndexDestroy(			SRV_INDEX *SrvIndex ),
		SrvIndexLoad(				char *Directory, char *Name ),
		SrvIndexSave(				SRV_INDEX *SrvIndex, char *FileName ),
		SrvIndexDescriptorsSave(	char *FileName ),
		SrvIndexDescriptorsLoad(	char *FileName ),
		SrvError(					const char *Format, ... ) ARGS_LIKE_PRINTF(1);

void	SrvIndexPutRecordToStream(	BSTREAM *Output, SDB_INDEX_DESCRIPTOR *Descriptor, void *RecBuf );

SRV_INDEX
		*SrvIndexFromName(			const char *IndexName );

int		SrvTransactionState(		SDB_TRANS_ID *StartingTransaction,
                                    SDB_TRANS_ID *CurrentTransaction );

SRV_PKT_RESULT
		SrvTransAdd(				SRV_CONNECTION *SrvConn, SRV_TRANS *SrvTrans, BSTREAM *Output );

int		SrvTransWriteLog(			SRV_CONNECTION *SrvConn ),
		SrvTransSkipLog(			SDB_TRANS_ID TransID, FILE *fpLog ),
		SrvTransLoad(				char *Directory ),
		SrvTransGetHead(			SDB_TRANS_ID TransactionID, 
									SDB_TRANS_HEADER *SrvTransInfo );

void	SrvTransCleanup(			SRV_CONNECTION *SrvConn, int FreeContents ),
		SrvTransFree(				SRV_TRANS *SrvTrans ),
		SrvTransCloseFiles(			void );

SRV_TRANS
		*SrvTransGetPartByNumber(	SDB_TRANS_ID TransID, int PartNumber,
									BIG_OFFSET *NextOffset ),
		*SrvTransGetPartByOffset(	SDB_TRANS_ID TransID, BIG_OFFSET PartOffset,
									BIG_OFFSET *NextOffset );

#ifdef ASYNC_SYNC_POINT
int		SrvTransCopyTransactions(	SDB_TRANS_ID First,
									const char *TargetDirectory ),
		SrvTransTruncateLog(		SDB_TRANS_ID LowestToDelete );
#endif

SDB_UNIQUE_ID
		SrvUniqueIDGet(				void );

int		SrvUniqueIDLoad(			void ),
		SrvUniqueIDSave(   			int CleanShutdown );


int		SrvRecordLaggedLoad(		SRV_RECORD *SrvRecord );

SRV_RECORD
		*SrvRecordGetFromSyncPoint(	int SyncPointOffset, const char *SecName );

		
int		SrvFileInitialize(			void );

SDB_FILE_HANDLE
SrvFileHandleFromName(				char const* FileName, int CreateIfNew );

void	SrvFileClose(				SDB_FILE_HANDLE FileHandle );

int		SrvFileDelete(				SDB_FILE_HANDLE Handle ),
		SrvFileExists(				SDB_FILE_HANDLE	Handle ),
		SrvFileRead(				SDB_FILE_HANDLE	Handle, void *Data, size_t Length, off_t Position, size_t *LengthRead ),
		SrvFileWrite(				SDB_FILE_HANDLE	Handle, const void *Data, size_t Length, off_t Position ),
		SrvFileInfoSet( 			SDB_FILE_HANDLE	Handle, SDB_FILE_INFO *Info ),
		SrvFileInfoGet( 			SDB_FILE_HANDLE	Handle, SDB_FILE_INFO *Info );


#endif
