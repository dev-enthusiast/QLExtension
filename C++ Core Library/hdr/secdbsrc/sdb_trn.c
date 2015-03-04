#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_trn.c,v 1.91 2013/08/23 16:55:31 khodod Exp $"
// #define DBUG
/****************************************************************
**
**	SDB_TRN.C	- SecDb Transaction API
**
**	Copyright 1993-1999 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_trn.c,v $
**	Revision 1.91  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.90  2004/07/27 19:52:29  khodod
**	Removed unnecessary header.
**
**	Revision 1.89  2004/07/22 17:42:36  khodod
**	Moved the write check code to osecserv.
**	iBug #12504
**	
**	Revision 1.88  2004/07/21 17:00:55  khodod
**	Check code safety only for transaction commits.
**	
**	Revision 1.87  2004/06/10 19:46:31  khodod
**	First stab at SOX compliance.
**	iBug #12504
**	
**	Revision 1.86  2004/01/30 15:36:46  mahonp
**	Fix bugfix
**	
**	Revision 1.85  2004/01/29 23:22:24  khodod
**	Properly terminate the string; return FALSE if not updated.
**	
**	Revision 1.84  2004/01/28 19:56:31  mahonp
**	Add new function to change transaction name mid-transaction
**	
**	Revision 1.83  2001/11/27 23:23:38  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.80  2001/10/01 19:57:29  hsuf
**	SDB_TRANS_PART now has a constructor that memsets this union structure.
**	This allows us to add more fields to the union in the future without
**	modifying existing code that use this union.
**	In SecDbTransMimic, copy TransFlags field from header to SDB_TRANS_PART.
**	SubDbTransCommit copies TransFlags field from subdb header to root db header.
**	
**	Revision 1.79  2001/08/27 15:23:10  hsuf
**	Implemented Async Transaction to improve secsync performance.
**	Added Slang "TransactionAsync()" and its interaction with other trnx fns.
**	Support query/viewing of async flag in slang/secview.
**	Secdb server/client use version number to turn on/off async behavior.
**	Server's transaction header now has async flag (taken from reserved fields)
**	
**	Revision 1.78  2001/06/29 15:22:15  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.77  2001/06/12 18:56:49  vengrd
**	self-desc transaction support.
**	
**	Revision 1.76  2001/06/11 16:51:53  goodfj
**	Added SecDbTransCommiteConditional
**	
**	Revision 1.75  2001/05/24 19:06:40  lundek
**	fix commit comment for 1.74 to reference 1.38-1.39 of sdb_trna.c instead of the wrong one
**	
**	Revision 1.74  2001/05/24 15:19:10  lundek
**	Fill in Prev pointers in translist returned from SecDbTransLogDetail so fix to huge
**  page crashing in SecDbTransListToArray (sdb_trna.c/1.38-1.39) will work
**	
**	Revision 1.73  2000/12/19 23:36:27  simpsk
**	More cleanups.  Also added loop for autochildren for the dbchanged case.
**	
**	Revision 1.72  2000/08/30 13:36:17  goodfj
**	SecDbTransProtectedDiskInfo takes optional pDbPhysical arg
**	
**	Revision 1.71  2000/07/11 17:05:33  goodfj
**	TransactionDbUpdateGet only handles ClassFilterUnions. Sets correct DbID in TransHead
**	
**	Revision 1.70  2000/07/06 16:56:34  goodfj
**	Added SecDbTransPartCount/Description
**	
**	Revision 1.69  2000/06/15 11:23:37  goodfj
**	Fix TransactionDbUpdateGet not to crash on Huge Page updates
**	
**	Revision 1.68  2000/06/14 13:37:54  goodfj
**	SecDbGetByName and SecDbTransProtectedGet take DbPhysical arg: look for obj in this db
**	
**	Revision 1.67  2000/06/13 14:55:33  goodfj
**	Added TransactionDbUpdateGet, to determine which db to send the trx to
**	
**	Revision 1.66  2000/06/12 14:42:42  goodfj
**	DbUpdate fixes
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<hash.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<secdrive.h>
#include	<sectrans.h>
#include	<secdrv.h>
#include	<sdb_int.h>


/*
**	Define constants
*/

#define		BACKOUT_IN_PROGRESS		-99


/*
**	String table
*/

const char *SecDbTransStrTab[] =
{
	"Begin",
	"Commit",
	"Abort",
	"Insert",
	"Update",
	"Delete",
	"Rename",
	"Index Insert",
	"Index Update",
	"Index Delete",
	"Index Rename",
	"Index Create",
	"Index Destroy",
	"Incremental",
	"Index Incremental",
	"File Delete",
	"File Write",
	"File Info Set",
	"Source Info",				// used only by secsync inside SecDbTransMimic
	"Self Describing Stream",
	NULL
};


#if defined( DBUG )
DT_VALUE *SecDbTransListToArray( SDB_TRANS_PART *TransList, int RawMem );

/****************************************************************
**	Routine: DumpTransList
**	Returns: Nothing
**	Action : Dumps transaction list
****************************************************************/

static void DumpTransList(
	SDB_TRANS_PART	*TransList
)
{
	DT_VALUE
			*Array,
			StringValue;

	static int
			DumpOrdinal = 0;


	Array = SecDbTransListToArray( TransList, FALSE );
	StringValue.DataType = DtString;
	if( DTM_TO( &StringValue, Array ))
		fprintf( stderr, "%d:\n%s\n\n", DumpOrdinal++, StringValue.Data.Pointer );
	else
		fprintf( stderr, "%d:\n%s\n\n", DumpOrdinal++, "Could not convert to string" );
	DTM_FREE( &StringValue );
	DTM_FREE( Array );
	free( Array );
}
#else
#define DumpTransList(x)
#endif



/****************************************************************
**	Routine: TransSecPtr
**	Returns: SDB_OBJECT or NULL
**			 Returns the Db in which is SecPtr was loaded
**	Action : Put SecData, Mem, and MemSize in Target structure
**			 (to mimic pfGetByName)
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "TransSecPtr"

static SDB_DB *TransSecPtr(
	SDB_DB				*pDb,
	SDB_STORED_OBJECT	*Target,
	SDB_STORED_OBJECT	*Source,
	int					Raw
)
{
	if( !pDb )
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb missing" );

	*Target = *Source;
	if( Source->MemSize > 0 )
	{
		if( !ERR_MALLOC( Target->Mem, void, Source->MemSize ))
			return NULL;
		memcpy( Target->Mem, (char *) Source->Mem, Source->MemSize );
	}
	if( Raw )
		return pDb;
	return SecDbHugeLoad( pDb, -1, Target );
}



/****************************************************************
**	Routine: IncrementalSecPtr
**	Returns: Db in which the SecPtr was loaded NULL for failure
**	Action : Gets current value from disk (if present) and applies
**			 provisional increment to it--theory being ones as
**			 good as another as long as all the increments are done.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "IncrementalSecPtr"

static SDB_DB *IncrementalSecPtr(
	SDB_STORED_OBJECT	*Target,
	SDB_DB				*pDb,
	const char			*SecName,
	SDB_TRANS_PART		*TransList
)
{
	SDB_TRANS_PART
			*TransPart;

	SDB_DRIVER_INCREMENTAL
			Incremental;


	if( !pDb )
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb missing" );

/*
**	Get the sec data from disk
*/

	if( !SecDbGetFromDatabase( pDb, SecName, Target, FALSE ))
	{
		Target->Mem		= NULL;
		Target->MemSize	= 0U;
	}


/*
**	Apply all the increments
*/

	for( TransPart = TransList; TransPart; TransPart = TransPart->Prev )
	{
		if( SDB_TRAN_INCREMENTAL == TransPart->Type )
		{
			if( 0 == stricmp( TransPart->Data.Incremental.SdbDisk.Name, SecName ))
			{
				if( !SecDbDriverIncremental(
						&Incremental,
						&Target->SdbDisk,
						Target->Mem,
						Target->MemSize,
						&TransPart->Data.Incremental.SdbDisk,
						TransPart->Data.Incremental.MsgMem,
						TransPart->Data.Incremental.MsgMemSize,
						TransPart->Data.Incremental.Direction ))
					return NULL;
				free( Target->Mem );
				Target->SdbDisk	= Incremental.StoredObject.SdbDisk;
				Target->Mem		= Incremental.StoredObject.Mem;
				Target->MemSize	= Incremental.StoredObject.MemSize;
			}
		}
		else if( SDB_TRAN_UPDATE == TransPart->Type || SDB_TRAN_INSERT == TransPart->Type )
		{
			if( 0 == stricmp( TransPart->Data.Op.StoredObject.SdbDisk.Name, SecName ))
			{
				free( Target->Mem );
				Err( ERR_UNSUPPORTED_OPERATION, "@: '%s'\nProtectedGet on transaction with INCREMENTAL mixed with INSERT or UPDATE", SecName );
				return NULL;
			}
		}
	}
	SecDbDriverIncrementalFree( &Incremental );
	return pDb;
}



/****************************************************************
**	Routine: SecDbTransProtectedGet
**	Returns: SDB_OBJECT from current transaction or disk
**	Action : pfGetByName except simulate anything already done
**			 in current transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbTransProtectedGet"

SDB_DB *SecDbTransProtectedGet(
	SDB_DB				*pDb,
	const char			*SecName,
	SDB_STORED_OBJECT	*Ret,
	int					Raw,
	SDB_DB				*pDbPhysical		// (optional, defaults to NULL)
)
{
	SDB_TRANS_PART
			*RenameTrans = NULL,
			*TransPart;

	SDB_DB	*pDbUpdate,
	        *pDbWhereRenamed = 0;


/*
**	If we fail to find the security in the transaction, look for it in pDbPhysical.
**	pDbPhysical defaults to NULL, so use pDb in that case.
*/
	if( NULL == pDbPhysical )
		pDbPhysical = pDb;

/*
**	Look for last intratransactional change
*/

	for( TransPart = pDb->TransTail; TransPart; TransPart = TransPart->Prev )
	{
		switch( TransPart->Type )
		{
			case SDB_TRAN_INSERT:
				if( 0 == stricmp( TransPart->Data.Op.StoredObject.SdbDisk.Name, SecName ))
                {
					if( !( pDbUpdate = SecDbDbUpdateGet( pDb, TransPart->Data.Op.StoredObject.SdbDisk.Type ) ) )
						return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get update db" );
					else
						return TransSecPtr(	pDbUpdate, Ret, &TransPart->Data.Op.StoredObject, Raw );
				}
                break;

			case SDB_TRAN_UPDATE:
				if( 0 == stricmp( TransPart->Data.Op.StoredObject.SdbDisk.Name, SecName ))
                {
					if( !( pDbUpdate = SecDbDbUpdateGet( pDb, TransPart->Data.Op.StoredObject.SdbDisk.Type ) ) )
						return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get update db" );
					else
						return TransSecPtr(	pDbUpdate, Ret, &TransPart->Data.Op.StoredObject, Raw );
                }
				break;

			case SDB_TRAN_INCREMENTAL:
				if( 0 == stricmp( TransPart->Data.Incremental.SdbDisk.Name, SecName ))
                {
					if( !( pDbUpdate = SecDbDbUpdateGet( pDb, TransPart->Data.Incremental.SdbDisk.Type ) ) )
						return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get update db" );
					else
						return IncrementalSecPtr( Ret, pDbUpdate, SecName, TransPart );
                }
				break;

			case SDB_TRAN_RENAME:
				if( 0 == stricmp( TransPart->Data.Rename.NewSdbDisk.Name, SecName ))
				{
					RenameTrans	= TransPart;
					SecName		= TransPart->Data.Rename.OldSdbDisk.Name;
					if( !( pDbWhereRenamed = SecDbDbUpdateGet( pDb, TransPart->Data.Rename.OldSdbDisk.Type ) ) )
						return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get db where renamed" );
				}
				else if( 0 == stricmp( TransPart->Data.Rename.OldSdbDisk.Name, SecName ))
					return (SDB_DB *) ErrN( SDB_ERR_OBJECT_NOT_FOUND, "@" );
				break;

			case SDB_TRAN_DELETE:
				if( 0 == stricmp( TransPart->Data.Delete.SdbDisk.Name, SecName ))
					return (SDB_DB * ) ErrN( SDB_ERR_OBJECT_NOT_FOUND, "@" );
				break;

			default:
				break;	// otherwise, skip it
		}
	}


/*
**	If rename found, but not an earlier update, the data is on disk
**	(hopefully--if not, the transaction is going to fail anyway with
**	an object modified or object not found error)
*/

	if( RenameTrans )
	{
		// FIX- for renames of objects not in DbUpdate, should store a rename object and a delete object
		if( SecDbGetFromDatabase( pDbWhereRenamed, SecName, Ret, Raw ))
		{
			if(		Ret->SdbDisk.DateCreated != RenameTrans->Data.Rename.OldSdbDisk.DateCreated ||
					Ret->SdbDisk.TimeUpdated != RenameTrans->Data.Rename.OldSdbDisk.TimeUpdated ||
					Ret->SdbDisk.DbIDUpdated != RenameTrans->Data.Rename.OldSdbDisk.DbIDUpdated ||
					Ret->SdbDisk.UpdateCount != RenameTrans->Data.Rename.OldSdbDisk.UpdateCount )
				return (SDB_DB *) ErrN( SDB_ERR_OBJECT_MODIFIED, "@" );

			Ret->SdbDisk = RenameTrans->Data.Rename.NewSdbDisk;
		}
		return pDb;
	}


/*
**	Didn't find it in transaction -- get from driver
*/

	return SecDbGetFromDatabase( pDbPhysical, SecName, Ret, Raw );
}



/****************************************************************
**	Routine: VDBDiskInfo
**	Returns: Db in which GetDiskInfo succeeds, NULL if none
**	Action : first successful pfGetDiskInfo from child dbs
****************************************************************/

static SDB_DB *VDBDiskInfo(
	SDB_DB		*pDb,
	const char	*SecName,
	SDB_DISK	*DiskInfo
)
{
	SDB_DB	*GottenDb;

	return ( pDb->IsVirtual()
			 ? ( ( GottenDb = VDBDiskInfo( pDb->Left, SecName, DiskInfo )) || ( pDb->Right && ( GottenDb = VDBDiskInfo( pDb->Right, SecName, DiskInfo ))))
			 : (*pDb->pfGetDiskInfo)( GottenDb = pDb, SecName, DiskInfo )) ? GottenDb : NULL;
}



/****************************************************************
**	Routine: SecDbTransProtectedDiskInfo
**	Returns: SDB_OBJECT from current transaction or disk
**	Action : pfGetDiskInfo except simulate anything already done
**			 in current transaction
****************************************************************/

SDB_DB *SecDbTransProtectedDiskInfo(
	SDB_DB		*pDb,
	const char	*SecName,
	SDB_DISK	*DiskInfo,
	SDB_DB		*pDbPhysical		// (optional, defaults to NULL)
)
{
	SDB_TRANS_PART
			*TransPart;


/*
**	If we fail to find the diskinfo in the transaction, look for it in pDbPhysical.
**	pDbPhysical defaults to NULL, so use pDb in that case.
*/
	if( NULL == pDbPhysical )
		pDbPhysical = pDb;

/*
**	Look for last intratransactional change
*/

	for( TransPart = pDb->TransTail; TransPart; TransPart = TransPart->Prev )
	{
		switch( TransPart->Type )
		{
			case SDB_TRAN_INSERT:
				if( 0 == stricmp( TransPart->Data.Op.StoredObject.SdbDisk.Name, SecName ))
				{
					*DiskInfo = TransPart->Data.Op.StoredObject.SdbDisk;
					return pDb;
				}
				break;

			case SDB_TRAN_UPDATE:
				if( 0 == stricmp( TransPart->Data.Op.StoredObject.SdbDisk.Name, SecName ))
				{
					*DiskInfo = TransPart->Data.Op.StoredObject.SdbDisk;
					return pDb;
				}
				break;

			case SDB_TRAN_INCREMENTAL:
				if( 0 == stricmp( TransPart->Data.Incremental.SdbDisk.Name, SecName ))
				{
					*DiskInfo = TransPart->Data.Incremental.SdbDisk;
					return pDb;
				}
				break;

			case SDB_TRAN_RENAME:
				if( 0 == stricmp( TransPart->Data.Rename.NewSdbDisk.Name, SecName ))
				{
					*DiskInfo = TransPart->Data.Rename.NewSdbDisk;
					return pDb;
				}
				else if( 0 == stricmp( TransPart->Data.Rename.OldSdbDisk.Name, SecName ))
				{
					Err( SDB_ERR_OBJECT_NOT_FOUND, "@" );
					return NULL;
				}
				break;

			case SDB_TRAN_DELETE:
				if( 0 == stricmp( TransPart->Data.Delete.SdbDisk.Name, SecName ))
				{
					Err( SDB_ERR_OBJECT_NOT_FOUND, "@" );
					return NULL;
				}
				break;

			default:
				break;	// otherwise, skip it
		}
	}


/*
**	Didn't find it in transaction -- get from driver
*/

	return VDBDiskInfo( pDbPhysical, SecName, DiskInfo );
}



/****************************************************************
**	Routine: SecDbTransPartInit
**	Returns: TRUE/FALSE
**	Action : Set TransType, allocate stuff if necessary
****************************************************************/

int SecDbTransPartInit(
	SDB_TRANS_PART	*TransPart,
	SDB_TRANS_TYPE	Type
)
{
	TransPart->Type = Type;
	switch( Type )
	{
		case SDB_TRAN_INDEX_CREATE:
			TransPart->Data.IndexCreate.IndexDescriptor = (SDB_INDEX_DESCRIPTOR *) calloc( 1, sizeof( *TransPart->Data.IndexCreate.IndexDescriptor ));
			if( !TransPart->Data.IndexCreate.IndexDescriptor )
				return Err( ERR_MEMORY, "@" );
			break;

		case SDB_TRAN_INDEX_INSERT:
		case SDB_TRAN_INDEX_UPDATE:
			TransPart->Data.IndexOp.Data = (SDB_DRV_INDEX_VALUES *) calloc( 1, sizeof( *TransPart->Data.IndexOp.Data ));
			if( !TransPart->Data.IndexOp.Data )
				return Err( ERR_MEMORY, "@" );
			break;

		default:
			break;	// otherwise, skip it
	}

	return TRUE;
}



/****************************************************************
**	Routine: SecDbTransPartFree
**	Returns: Nothing
**	Action :
****************************************************************/

void SecDbTransPartFree(
	SDB_TRANS_PART	*TransPart
)
{
	DT_VALUE
			*Values;

	int		PartCount,
			PartNum;


	switch( TransPart->Type )
	{
		case SDB_TRAN_INSERT:
			SecDbFree( TransPart->Data.Op.SecPtr );
			free( TransPart->Data.Op.StoredObject.Mem );
			break;

		case SDB_TRAN_UPDATE:
			SecDbFree( TransPart->Data.Op.SecPtr );
			free( TransPart->Data.Op.StoredObject.Mem );
			break;

		case SDB_TRAN_INCREMENTAL:
			SecDbFree( TransPart->Data.Incremental.SecPtr );
			free( TransPart->Data.Incremental.MsgMem );
			break;

		case SDB_TRAN_RENAME:
			SecDbFree( TransPart->Data.Rename.SecPtr );
			break;

		case SDB_TRAN_INDEX_CREATE:
			free( TransPart->Data.IndexCreate.IndexDescriptor );
			break;

		case SDB_TRAN_INDEX_INSERT:
		case SDB_TRAN_INDEX_UPDATE:
			Values		= TransPart->Data.IndexOp.Data->Values;
			PartCount	= TransPart->Data.IndexOp.Data->Index->PartCount;
			for( PartNum = 0; PartNum < PartCount; PartNum++ )
				DTM_FREE( &Values[ PartNum ] );
			free( TransPart->Data.IndexOp.Data );
			break;

		case SDB_TRAN_FILE_WRITE:
			free( (void *)TransPart->Data.FileWrite.Data );
			break;

		case SDB_TRAN_SELF_DESC_DATA:
			free( (void *)TransPart->Data.SelfDesc.Data );
			break;

		default:
			break;	// otherwise, skip it
	}

	free( TransPart );
}


/****************************************************************
**	Routine: SecDbTransPartCount
**	Returns: Number of current transaction parts
**	Action : Loop over the trx list
****************************************************************/

int SecDbTransPartCount(
	SDB_DB			*pDb
)
{
	int		PartCount;

	SDB_TRANS_PART
			*TransPart;


	if( !pDb )
		return -1;

	for( TransPart = pDb->TransHead, PartCount = 0; TransPart; TransPart = TransPart->Next )
		if( TransPart->Type != SDB_TRAN_BEGIN && TransPart->Type != SDB_TRAN_COMMIT )
			PartCount++;

	return PartCount;
}


/****************************************************************
**	Routine: FreeTransaction
**	Returns: Nothing
**	Action : Frees up the transaction parts
****************************************************************/

static void FreeTransaction(
	SDB_DB			*pDb
)
{
	SDB_TRANS_PART
			*TransPart;


	for( TransPart = pDb->TransTail; TransPart; TransPart = pDb->TransTail )
	{
		pDb->TransTail =  TransPart->Prev;
		SecDbTransPartFree( TransPart );
	}
	pDb->TransHead = NULL;
	pDb->TransDepth = 0;
	pDb->TransFlags = 0;
}



/****************************************************************
**	Routine: BackoutTransaction
**	Returns: Nothing
**	Action : Restores in-memory data structures and frees backed
**	         out transaction parts
****************************************************************/

static void BackoutTransaction(
	SDB_DB		*pDb
)
{
	int		SaveDepth,
			Depth = 1;

	unsigned long
			SaveFlags;

	SDB_TRANS_PART
			*TransPart;

	SDB_M_DATA
			MsgData;

	SDB_OBJECT
			*SecPtr;


/*
**	Lockout all other transactions during backout
*/

	ERR_OFF();
	SaveDepth = pDb->TransDepth;
	SaveFlags = pDb->TransFlags;
	pDb->TransDepth = BACKOUT_IN_PROGRESS;


/*
**	Backout each part in reverse order
*/


	for( TransPart = pDb->TransTail; TransPart; TransPart = pDb->TransTail )
	{
		pDb->TransTail = TransPart->Prev;
		switch( TransPart->Type )
		{
			case SDB_TRAN_BEGIN:
				if( --Depth <= 0 )
				{
					SecDbTransPartFree( TransPart );
NormalExit:			if( pDb->TransTail )
						pDb->TransTail->Next = NULL;
					else
						pDb->TransHead = NULL;
					pDb->TransDepth = SaveDepth;
					pDb->TransFlags = SaveFlags;
					ERR_ON( );
					return;
				}
				break;

			case SDB_TRAN_COMMIT:
				Depth++;
				break;

			case SDB_TRAN_INSERT:
				SecPtr = TransPart->Data.Op.SecPtr;
				if( SecPtr )
				{
					SecDbHugeUnload( SecDbDbUpdateGet( pDb, TransPart->Data.Op.OldSdbDisk.Type ), TransPart->Data.Op.OldSdbDisk.Name );
					SecPtr->SecData = TransPart->Data.Op.OldSdbDisk;
					SDB_MESSAGE( SecPtr, SDB_MSG_ROLL_BACK_ADD, NULL );
				}
				break;

			case SDB_TRAN_UPDATE:
				SecPtr = TransPart->Data.Op.SecPtr;
				if( SecPtr )
				{
					SecDbHugeUnload( SecDbDbUpdateGet( pDb, TransPart->Data.Op.OldSdbDisk.Type ), TransPart->Data.Op.OldSdbDisk.Name );
					SecPtr->SecData = TransPart->Data.Op.OldSdbDisk;
					SDB_MESSAGE( SecPtr, SDB_MSG_ROLL_BACK_UPDATE, NULL );
				}
				break;

			case SDB_TRAN_INCREMENTAL:
				SecPtr = TransPart->Data.Incremental.SecPtr;
				if( SecPtr )
				{
					SecPtr->SecData = TransPart->Data.Incremental.OldSdbDisk;
					SDB_MESSAGE( SecPtr, SDB_MSG_ROLL_BACK_INCREMENTAL, NULL );
				}
				break;

			case SDB_TRAN_DELETE:
				SecDbHugeUnload( SecDbDbUpdateGet( pDb, TransPart->Data.Delete.SdbDisk.Type ), TransPart->Data.Delete.SdbDisk.Name );
				if( !( SecPtr = SecDbGetByName( TransPart->Data.Delete.SdbDisk.Name, pDb, SDB_IGNORE_PATH )))
					break;
				SDB_MESSAGE( SecPtr, SDB_MSG_ROLL_BACK_DELETE, NULL );
				SecDbFree( SecPtr );
				break;

			case SDB_TRAN_RENAME:
				SecPtr = TransPart->Data.Rename.SecPtr;
				if( SecPtr )
				{
					SecDbHugeUnload( SecDbDbUpdateGet( pDb, TransPart->Data.Rename.NewSdbDisk.Type ), TransPart->Data.Rename.NewSdbDisk.Name );
					HashDelete( pDb->CacheHash, TransPart->Data.Rename.NewSdbDisk.Name );
					strcpy( MsgData.Rename.NewName, TransPart->Data.Rename.NewSdbDisk.Name );
					SecPtr->SecData = TransPart->Data.Rename.OldSdbDisk;
					HashInsert( pDb->CacheHash, SecPtr->SecData.Name, SecPtr );
					SDB_MESSAGE( SecPtr, SDB_MSG_ROLL_BACK_RENAME, &MsgData );
				}
				break;

			default:
				break; // otherwise, skip it
		}
		SecDbTransPartFree( TransPart );
	}


/*
**	Should never reach this code -- only if trans list runs out before
**	level returns.  Don't see anything else to do about it but clean it
**	up and return.
*/

	goto NormalExit;
}



/****************************************************************
**	Routine: ApplyTransId
**	Returns: Nothing
**	Action : Places TransId in LastTransaction of each updated SecPtr
****************************************************************/

static void ApplyTransId(
	SDB_DB			*pDb,
	SDB_TRANS_ID	TransId
)
{
	SDB_TRANS_PART
			*TransPart;

	SDB_OBJECT
			*SecPtr;


	for( TransPart = pDb->TransHead; TransPart; TransPart = TransPart->Next )
	{
		SecPtr = NULL;
		switch( TransPart->Type )
		{
			case SDB_TRAN_INSERT:
			case SDB_TRAN_UPDATE:
				SecPtr = TransPart->Data.Op.SecPtr;
				break;

			case SDB_TRAN_INCREMENTAL:
				SecPtr = TransPart->Data.Incremental.SecPtr;
				break;

			case SDB_TRAN_RENAME:
				SecPtr = TransPart->Data.Rename.SecPtr;
				break;
				
			default:
				break; // otherwise, skip it
		}
		if( SecPtr )
			SecPtr->SecData.LastTransaction = TransId;
	}
}



/****************************************************************
**	Routine: SecDbTransDepth
**	Returns: Current depth of transaction tree
**	Action : Find the database and pull out its trans depth
****************************************************************/

int SecDbTransDepth(
	SDB_DB	*pDb
)
{
	return pDb ? pDb->TransDepth : 0;
}

/****************************************************************
**	Routine: SecDbTransFlags
**	Returns: Current transaction flags
**	Action : Find the database and pull out its trans flags
****************************************************************/

unsigned long SecDbTransFlags(
	SDB_DB	*pDb
)
{
	return pDb ? pDb->TransFlags : 0;
}


/****************************************************************
**	Routine: SecDbTransGetCurrent
**	Returns: non-NULL	Pointer to current uncommitted trans list
**			 NULL		Error or no current transaction (check ErrNum)
**	Action : Finds pDb for given ID and returns its current trans
**			 list.  NOTE: Do not free returned trans list as it
**			 is a direct pointer to current trans list.
****************************************************************/

const SDB_TRANS_PART *SecDbTransGetCurrent(
	SDB_DB	*pDb
)
{
	if( !pDb )
		return NULL;

	if( 0 == pDb->TransDepth )
	{
		Err( ERR_NOTHING_CHANGED, "No current transaction" );
		return NULL;
	}

	return pDb->TransHead;
}



/****************************************************************
**	Routine: SecDbTransBegin
**	Returns: TRUE	- Transaction begun without error
**			 FALSE	- Error trying to begin transaction
**	Summary: Begin a transaction
****************************************************************/

int SecDbTransBegin(
	SDB_DB		*pDb,			// Database to operate on
	const char	*Description	// Description of transaction
)
{
	return( SecDbTransBeginWithFlags( pDb, Description, 0 ) );
}

/****************************************************************
**	Routine: SecDbTransBeginWithFlags
**	Returns: TRUE	- Transaction begun without error
**			 FALSE	- Error trying to begin transaction
**	Summary: Begin a transaction
****************************************************************/

int SecDbTransBeginWithFlags(
	SDB_DB			*pDb,			// Database to operate on
	const char		*Description,   // Description of transaction
	unsigned long	Flags
)
{
	SDB_TRANS_PART
			TransPart;

	TransPart.Type = SDB_TRAN_BEGIN;
	strncpy( TransPart.Data.Begin.SecName, Description, sizeof( TransPart.Data.Begin.SecName ));
	TransPart.Data.Begin.SecName[ sizeof( TransPart.Data.Begin.SecName ) - 1 ] = '\0';
	TransPart.Data.Begin.SecType		= 0;
	TransPart.Data.Begin.DbID			= pDb->DbID;
	TransPart.Data.Begin.TransType		= SDB_TRAN_BEGIN;
	TransPart.Data.Begin.SourceTransID	= 0;
	TransPart.Data.Begin.TransFlags	   	= Flags;

	return SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL );
}


/****************************************************************
**	Routine: SecDbTransDescription
**	Returns: Pointer to allocated buffer, with copy of description
**			 		of current transaction
**			 NULL	- Description not found
**	Summary: Find the description
****************************************************************/

char *SecDbTransDescription(
	SDB_DB		*pDb			// Database to operate on
)
{
	SDB_TRANS_PART
			*TransPart;


	for( TransPart = pDb->TransHead; TransPart; TransPart = TransPart->Next )
		if( SDB_TRAN_BEGIN == TransPart->Type )
			return strdup( TransPart->Data.Begin.SecName );

	return NULL;
}

/****************************************************************
**	Routine: SecDbTransDescriptionSet
**	Returns: Success/Failure
**	Summary: Change the description of a transaction
****************************************************************/

int SecDbTransDescriptionSet(
	SDB_DB		*pDb,			// Database to operate on
	const char        *Description
)
{
	SDB_TRANS_PART
			*TransPart;

	if( !Description || !( *Description ) )
  	    return Err( ERR_INVALID_STRING, "@: Empty string encountered" );

	for( TransPart = pDb->TransHead; TransPart; TransPart = TransPart->Next )
		if( SDB_TRAN_BEGIN == TransPart->Type )
		{
			strncpy( TransPart->Data.Begin.SecName, Description, sizeof( TransPart->Data.Begin.SecName ));
			TransPart->Data.Begin.SecName[ sizeof( TransPart->Data.Begin.SecName ) - 1 ] = '\0';
			return TRUE;
		}

	return FALSE;
}


/****************************************************************
**	Routine: SecDbTransCommitForce
**	Returns: TRUE or FALSE
**	Action : Transforms the current transaction to a force by:
**				- Changing Updates to Inserts
**				- Changing IndexeUpdates to IndexInserts
**				- Prepending a Delete for each Insert
**				- Prepending a IndexDelete for each IndexInsert
**				- Setting the IgnoreErrors bit for each Delete
**				- Setting the IgnoreErrors bit for each IndexDelete
****************************************************************/

int SecDbTransCommitForce(
	SDB_DB	*pDb
)
{
	SDB_TRANS_PART
			*Begin,
			*New = NULL,
			*Cur,
			*Trans;

	int		Depth;

	if( SecDbTransDepth( pDb ) > 0 &&
		SecDbTransFlags( pDb ) & SDB_TRAN_FLAG_ALLOW_ASYNC )
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return Err( ERR_UNSUPPORTED_OPERATION, 
				"@ - Cannot commit with force within an async transaction.  Transaction aborted." );
	}

/*
**	Find beginning of transaction
*/

	Depth = 1;
	for( Begin = pDb->TransTail; Begin; Begin = Begin->Prev )
	{
		if( SDB_TRAN_BEGIN == Begin->Type )
			Depth--;
		else if( SDB_TRAN_COMMIT == Begin->Type )
			Depth++;
		if( Depth <= 0 )
			break;
	}
	if( !Begin || !Begin->Next )
		return TRUE;

	// Disconnect it from current trans
	Cur = Begin->Next;
	pDb->TransTail = Cur->Prev;
	if( pDb->TransHead == Cur )
		pDb->TransHead = NULL;
	else
		Cur->Prev->Next = NULL;
	Cur->Prev = NULL;


/*
**	Make the transformations
*/

	for( Trans = Cur; Trans; Trans = Cur )
	{
		if( SDB_TRAN_UPDATE == Trans->Type )
			Trans->Type = SDB_TRAN_INSERT;
		else if( SDB_TRAN_INDEX_UPDATE == Trans->Type )
			Trans->Type = SDB_TRAN_INDEX_INSERT;

		if( SDB_TRAN_INSERT == Trans->Type || SDB_TRAN_INDEX_INSERT == Trans->Type )
		{
			if( !( New = (SDB_TRANS_PART *) malloc( sizeof( *New ))))
				return Err( ERR_MEMORY, "@" );

			if( SDB_TRAN_INSERT == Trans->Type )
			{
				(void) SecDbHugeDelete( pDb, Trans->Data.Op.StoredObject.SdbDisk.Name );
				New->Type = SDB_TRAN_DELETE;
				New->Data.Delete.SdbDisk = Trans->Data.Op.StoredObject.SdbDisk;
			}
			else
			{
				New->Type = SDB_TRAN_INDEX_DELETE;
				strcpy( New->Data.IndexDelete.IndexName, Trans->Data.IndexOp.Data->Index->Name );
				strcpy( New->Data.IndexDelete.SecName, Trans->Data.IndexOp.Data->SecName );
				New->Data.IndexDelete.Index = Trans->Data.IndexOp.Data->Index;
			}
		}
		else if( SDB_TRAN_DELETE == Trans->Type )
		{
			(void) SecDbHugeDelete( pDb, Trans->Data.Delete.SdbDisk.Name );
			Trans->Data.Delete.IgnoreErrors = TRUE;
		}
		else if( SDB_TRAN_INDEX_DELETE == Trans->Type )
			Trans->Data.IndexDelete.IgnoreErrors = TRUE;

		// Reattach
		if( New )
		{
			New->Next = NULL;
			if( pDb->TransTail )
				pDb->TransTail->Next = New;
			else
				pDb->TransHead = New;
			New->Prev	= pDb->TransTail;
			pDb->TransTail	= New;
			New = NULL;
		}
		Cur = Trans->Next;
		Trans->Next = NULL;
		if( pDb->TransTail )
			pDb->TransTail->Next = Trans;
		else
			pDb->TransHead = Trans;
		Trans->Prev	= pDb->TransTail;
		pDb->TransTail	= Trans;
	}
	for( Trans = Begin->Next; Trans; Trans = Trans->Next )
	{
		if( SDB_TRAN_DELETE == Trans->Type )
			Trans->Data.Delete.IgnoreErrors = TRUE;
		else if( SDB_TRAN_INDEX_DELETE == Trans->Type )
			Trans->Data.IndexDelete.IgnoreErrors = TRUE;
	}


/*
**	Add the COMMIT
*/

	return SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL );
}



/****************************************************************
**	Routine: SecDbTransCommit
**	Returns: TRUE	- Transaction committed without error
**			 FALSE	- Error committing transaction
**	Summary: Commit a transaction
****************************************************************/

int SecDbTransCommit(
	SDB_DB	*pDb
)
{
	return SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL );
}



/****************************************************************
**	Routine: SecDbTransAbort
**	Returns: TRUE	- Transaction aborted without error
**			 FALSE	- Error aborting transaction
**	Summary: Abort a transaction
****************************************************************/

int SecDbTransAbort(
	SDB_DB	*pDb						// Database to operate on
)
{
	return SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
}



/****************************************************************
**	Routine: SecDbRefreshSecurityDatabase
**	Returns: TRUE, if successful; FALSE, otherwise
**	Action : Refresh the "Security Database" object from Database, which
**			 has the side effect of regenerating the index structures
**			 for all of Database's indices.
****************************************************************/

int SecDbRefreshSecurityDatabase(
	SDB_DB	*Db
)
{
	SDB_OBJECT
			*SecurityDatabase;


	SecurityDatabase
		= SecDbGetByName( "Security Database",
						  Db, SDB_IGNORE_PATH | SDB_REFRESH_CACHE );
	if( SecurityDatabase == NULL )
		return FALSE;

	if( !SecDbReload( SecurityDatabase ))
		return FALSE;
	SecDbFree( SecurityDatabase );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbTransMimic
**	Returns: If the transaction is mimicked successfully, TRUE is returned and
**			 the transaction's ID number is returned in *TransId.  Otherwise,
**			 FALSE is returned and *TransId is unchanged.
**	Action : Mimic one transaction into the database indicated by DbID.
****************************************************************/

int SecDbTransMimic(
	SDB_DB				*SourceDatabase,
	SDB_DB				*TargetDatabase,
	SDB_TRANS_HEADER	*Header,		// Header of transaction to mimic
	SDB_TRANS_PART		*TransList,		// Detail of transaction to mimic
	SDB_TRANS_ID		*TransId		// return transaction ID
)
{
	static int
			PreserveGetenvDone = FALSE,
			PreserveSourceInfo = FALSE;

	SDB_TRANS_PART
			TransPart,
			*Trans;

	const char
			*IndexName = NULL;

	SDB_INDEX
			*Index = NULL,
			**IndexHandle;


	if( SDB_TRAN_COMMIT != Header->Type )
		return TRUE;

	if( !SourceDatabase || !TargetDatabase )
		return Err( ERR_DATABASE_INVALID, "TransMimic(): NULL Source/Dest Database" );

	if( TargetDatabase->TransDepth )
	{
		Err( ERR_INVALID_ARGUMENTS, "Transaction depth (%d) must be zero", TargetDatabase->TransDepth );
		return ErrMore( "TransMimic" );
	}

	strcpy( TransPart.Data.Begin.SecName, Header->SecName );
	TransPart.Data.Begin.SecType		= Header->SecType;
	TransPart.Data.Begin.TransType		= Header->TransType;
	TransPart.Data.Begin.DbID			= Header->DbID;
	TransPart.Data.Begin.SourceTransID	= Header->TransID;
	TransPart.Data.Begin.TransFlags		= Header->TransFlags;
	if( !SecDbTransactionAdd( TargetDatabase, SDB_TRAN_BEGIN, &TransPart, NULL ))
		return FALSE;

	if( !PreserveGetenvDone )
	{
		char	*Value;

		Value = getenv("SECDB_PRESERVE_SOURCE_INFO");
		PreserveGetenvDone = TRUE;
		PreserveSourceInfo = ( Value && stricmp( Value, "true" ) == 0 );
	}

	if( PreserveSourceInfo )
	{
		strcpy( TransPart.Data.SourceInfo.UserName, Header->UserName );
		strcpy( TransPart.Data.SourceInfo.ApplicationName,
				Header->ApplicationName );
		TransPart.Data.SourceInfo.NetworkAddress = Header->NetworkAddress;
		if( !SecDbTransactionAdd( TargetDatabase,
								  SDB_TRAN_SOURCE_INFO, &TransPart, NULL ))
			return FALSE;
	}


	for( Trans = TransList; Trans; Trans = Trans->Next )
	{

/*
**	Get the index handle on index operations
*/

		IndexHandle = NULL;
		switch( Trans->Type )
		{
			case SDB_TRAN_INDEX_INSERT:
			case SDB_TRAN_INDEX_UPDATE:
				IndexHandle = &Trans->Data.IndexOp.Data->Index;
				IndexName	= (*IndexHandle)->Name;
				break;

			case SDB_TRAN_INDEX_INCREMENTAL:
				IndexHandle = &Trans->Data.IndexIncremental.Index;
				IndexName	= (*IndexHandle)->Name;
				break;

			case SDB_TRAN_INDEX_DELETE:
				IndexHandle = &Trans->Data.IndexDelete.Index;
				IndexName	= Trans->Data.IndexDelete.IndexName;
				break;

			case SDB_TRAN_INDEX_RENAME:
				IndexHandle = &Trans->Data.IndexRename.Index;
				IndexName	= Trans->Data.IndexRename.IndexName;
				break;

			case SDB_TRAN_INDEX_DESTROY:
				Index = SecDbIndexFromName( Trans->Data.IndexDestroy.IndexName, TargetDatabase );
				if( Index )
					SecDbIndexClose( Index, TargetDatabase, TRUE );
				IndexHandle = NULL;
				break;

			case SDB_TRAN_INDEX_CREATE:
				Index = SecDbIndexNew( Trans->Data.IndexCreate.IndexDescriptor->Name, TargetDatabase );
				if( Index )
					HashInsert( SecDbIndexHash, Index, Index );
				else
					return FALSE;
				IndexHandle		= NULL;
				break;


/*
**	Special case for deleting huge objects when not relying on db driver version validation
**	(since we must also delete huge pages in target db)
*/

			case SDB_TRAN_DELETE:
				if( Trans->Data.Delete.IgnoreErrors )
					if( !SecDbHugeDelete( TargetDatabase, Trans->Data.Delete.SdbDisk.Name ))
					{
						SecDbTransactionAdd( TargetDatabase, SDB_TRAN_ABORT, NULL, NULL );
						return ErrMore( "TransMimic could not delete local huge pages" );
					}
				break;

			default:
				break;
		}

		if( IndexHandle )
			if( !( *IndexHandle = SecDbIndexFromName( IndexName, TargetDatabase )))
			{
				SecDbTransactionAdd( TargetDatabase, SDB_TRAN_ABORT, NULL, NULL );
				return ErrMore( "TransMimic could not get index handle" );
			}


/*
**	Add the part
*/

		if( !SecDbTransactionAdd( TargetDatabase, Trans->Type, Trans, NULL ))
		{
			SecDbTransactionAdd( TargetDatabase, SDB_TRAN_ABORT, NULL, NULL );
			return ErrMore( "TransMimic could not add transaction part" );
		}
	}


/*
**	Commit transaction to target daatabase
*/

	if( 1 != TargetDatabase->TransDepth )
	{
		Err( SDB_ERR_TRANSACTION_FAILURE, "Transaction depth (%d) was not reset to zero", TargetDatabase->TransDepth );
		ERR_OFF();
		SecDbTransactionAdd( TargetDatabase, SDB_TRAN_ABORT, NULL, NULL );
		FreeTransaction( TargetDatabase );
		ERR_ON();
		return ErrMore( "TransMimic" );
	}

	if( !SecDbTransactionAdd( TargetDatabase, SDB_TRAN_COMMIT, NULL, TransId ))
		return ErrMore( "TransMimic could not commit transaction" );

	// If we created or destroyed an index, then reload the security database
	// object.  Doing so regenerates our internal representation of the
	// indices.

	if( Index )
	{
		SecDbRefreshSecurityDatabase( SourceDatabase );
		SecDbRefreshSecurityDatabase( TargetDatabase );
	}

	return TRUE;
}



/****************************************************************
**	Routine: TransactionDbUpdateGet
**	Returns: Pointer to database, or NULL on error
**	Action : Find the database to be updated from the current
**			 transaction. Return an error if there are transaction
**			 parts requiring updates in multiple databases.
**
**			 Also validate that index transparts have valid
**			 Handle (gets reset to NULL if you reload Security
**			 Database mid-transaction, and will otherwise cause
**			 a crash.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "TransactionDbUpdateGet"

static SDB_DB *TransactionDbUpdateGet(
	SDB_DB	*pDb
)
{
	SDB_TRANS_PART
			*TransPart;

	SDB_DB	*pDbUpdate = NULL,
			*pThisUpdateDb,
			*pOldUpdateDb;

	const char
			*OpDesc = NULL,
			*SecName = NULL;

	SDB_SEC_TYPE
			OldSecType = 0,
			SecType = 0;

	SDB_INDEX
			*Index;


	if( !pDb )
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb missing" );
	if( !pDb->TransHead )
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": No transaction in db %s", pDb->FullDbName.c_str() );

	if( SDB_DB_CLASSFILTERUNION != pDb->DbOperator )		// Just return default update db
		return SecDbDbUpdateGet( pDb, (SDB_SEC_TYPE) 0 );

	for( TransPart = pDb->TransHead; TransPart; TransPart = TransPart->Next )
	{
		SecName		= NULL;
		Index		= NULL;
		SecType		= 0;
		OldSecType	= 0;

		switch( TransPart->Type )
		{
			case SDB_TRAN_INSERT:
			case SDB_TRAN_UPDATE:
				OpDesc	= SDB_TRAN_INSERT == TransPart->Type ? "insert" : "update";
				if( TransPart->Data.Op.SecPtr )
				{
					SecName	= TransPart->Data.Op.SecPtr->SecData.Name;
					SecType	= TransPart->Data.Op.SecPtr->SecData.Type;
					OldSecType = TransPart->Data.Op.OldSdbDisk.Type;
				}
				break;

			case SDB_TRAN_DELETE:
				OpDesc	= "delete";
				SecName	= TransPart->Data.Delete.SdbDisk.Name;
				SecType	= TransPart->Data.Delete.SdbDisk.Type;
				break;

			case SDB_TRAN_RENAME:
				OpDesc	= "rename";
				SecName		= TransPart->Data.Rename.OldSdbDisk.Name;
				OldSecType	= TransPart->Data.Rename.NewSdbDisk.Type;
				SecType		= TransPart->Data.Rename.OldSdbDisk.Type;
				break;

			case SDB_TRAN_INCREMENTAL:
				OpDesc	= "incremental";
				if( TransPart->Data.Op.SecPtr )
				{
					SecName	= TransPart->Data.Incremental.SecPtr->SecData.Name;
					SecType	= TransPart->Data.Incremental.SecPtr->SecData.Type;
				}
				break;

			case SDB_TRAN_INDEX_INSERT:
			case SDB_TRAN_INDEX_UPDATE:
				OpDesc	= SDB_TRAN_INSERT == TransPart->Type ? "indexinsert" : "indexupdate";
				Index	= TransPart->Data.IndexOp.Data->Index;
				SecName	= TransPart->Data.IndexOp.Data->SecName;
				break;

			case SDB_TRAN_INDEX_DELETE:
				OpDesc	= "indexdelete";
				Index	= TransPart->Data.IndexDelete.Index;
				SecName	= TransPart->Data.IndexDelete.SecName;
				break;

			case SDB_TRAN_INDEX_RENAME:
				OpDesc	= "indexrename";
				Index	= TransPart->Data.IndexRename.Index;
				SecName	= TransPart->Data.IndexRename.OldSecName;
				break;

			case SDB_TRAN_INDEX_INCREMENTAL:
				OpDesc	= "indexincremental";
				Index	= TransPart->Data.IndexIncremental.Index;
				SecName	= TransPart->Data.IndexIncremental.SecName;
				break;

	  		default:
				break;
		}

		if( Index && !Index->Handle )
			return (SDB_DB *) ErrN( SDB_ERR_TRANSACTION_FAILURE, ARGCHECK_FUNCTION_NAME ": Found NULL Index->Handle on %s %s: was index invalidated during transaction?", OpDesc, SecName );

		if( 0 == SecType )	// Found no object update
			continue;

		pThisUpdateDb = SecDbDbUpdateGet( pDb, SecType );

		if( OldSecType && OldSecType != SecType )
		{
/*
**	Disallow updates/renames that change security type, AND therefore change update db. If you 
**	need to do this, you have to copy the obj to the other db.
*/
			pOldUpdateDb = SecDbDbUpdateGet( pDb, OldSecType );
			if( pOldUpdateDb != pThisUpdateDb )
				return (SDB_DB *) ErrN( SDB_ERR_TRANSACTION_FAILURE, ARGCHECK_FUNCTION_NAME ": Cannot change db from %s to %s in %s( %s )", pOldUpdateDb->FullDbName.c_str(), pThisUpdateDb->FullDbName.c_str(), OpDesc, SecName );
		}

		if( !pDbUpdate )
			pDbUpdate = pThisUpdateDb;
		else if( pDbUpdate != pThisUpdateDb )
			return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Can't %s %s in %s (current transaction is for %s)", OpDesc, SecName, pThisUpdateDb->FullDbName.c_str(), pDbUpdate->FullDbName.c_str() );
	}

/*
**	If pDbUpdate is still NULL, then just return default update db
*/
	if( !pDbUpdate )
		pDbUpdate = SecDbDbUpdateGet( pDb, (SDB_SEC_TYPE) 0 );

/*
**	Now we need to make sure that the transaction has the correct DbID in TransHead
*/
	if( SDB_TRAN_BEGIN != pDb->TransHead->Type )
		return (SDB_DB *) ErrN( SDB_ERR_TRANSACTION_FAILURE, ARGCHECK_FUNCTION_NAME ": Expected SDB_TRAN_BEGIN in TransHead, found %d", pDb->TransHead->Type );

	pDb->TransHead->Data.Begin.DbID = pDbUpdate->DbID;

	return pDbUpdate;
}


/****************************************************************
**	Routine: SecDbTransactionAdd
**	Returns: TRUE for success; FALSE otherwise
**			 In addition, if Type is SDB_TRAN_COMMIT and the transaction commit
**			 is successful and TransId is non-NULL, then the transaction's ID
**			 is returned in *TransId.
**	Action : Add a link to a transaction chain.  If we're adding a commit part
**			 and TransDepth hits zero, then commit the transaction.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbTransactionAdd"

int SecDbTransactionAdd(
	SDB_DB			*pDb,
	SDB_TRANS_TYPE	Type,
	SDB_TRANS_PART	*NewTransPart,
	SDB_TRANS_ID	*TransId
)
{
	SDB_TRANS_PART
			*TransPart;


	if( BACKOUT_IN_PROGRESS == pDb->TransDepth )
		return Err( SDB_ERR_TRANSACTION_FAILURE, "@: Transaction backout in progress" );

	switch( Type )
	{
		case SDB_TRAN_ABORT:
			if( pDb->TransDepth > 0 )
				pDb->TransDepth--;
			else
				return Err( SDB_ERR_TRANSACTION_FAILURE, "@: Abort: No matching SDB_TRANS_BEGIN" );
			if( pDb->TransDepth == 0 )
				pDb->TransFlags = 0;
			BackoutTransaction( pDb );
			return TRUE;

		case SDB_TRAN_BEGIN:
			if( NewTransPart && ( NewTransPart->Data.Begin.DbID & SDB_DB_VIRTUAL ))
			{
				SDB_DB	*pDbUpdate = SecDbDefaultDbUpdateGet( pDb );


				if( !pDbUpdate )
					return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to find update db" );

				NewTransPart->Data.Begin.DbID = pDbUpdate->DbID;
			}
			if( pDb->TransDepth == 0 )
				pDb->TransFlags = NewTransPart->Data.Begin.TransFlags;
			pDb->TransDepth++;
			break;

		case SDB_TRAN_COMMIT:
				
			if( pDb->TransDepth > 0 )
				pDb->TransDepth--;
			else
				return Err( SDB_ERR_TRANSACTION_FAILURE, "@: Commit: No matching SDB_TRANS_BEGIN" );
			if( pDb->TransDepth == 0 )
			{
				SDB_TRANS_ID
						Result;

				pDb->TransFlags = 0;

				SDB_DB	*pDbUpdate = TransactionDbUpdateGet( pDb );		// Find which db to send trx to


				if( !pDbUpdate )
				{
					BackoutTransaction( pDb );
					return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to find update db" );
				}

				DumpTransList( pDb->TransHead );
				Result = (*pDbUpdate->pfTransCommit)( pDbUpdate, pDb->TransHead );
				if( Result != SDB_TRANS_ID_ERROR )
				{
					ApplyTransId( pDb, Result );
					FreeTransaction( pDb );

					if( TransId != NULL )
						*TransId = Result;
					return TRUE;
				}
				else
				{
					BackoutTransaction( pDb );
					return FALSE;
				}
			}
			break;

		default:
			if( pDb->TransDepth <= 0 )
				return Err( SDB_ERR_TRANSACTION_FAILURE, "@: Cannot add transaction part without SDB_TRANS_BEGIN" );
			break;
	}


/*
**	Add a link to the transaction chain
*/

	TransPart = (SDB_TRANS_PART *) malloc( sizeof( *TransPart ));
	if( !TransPart )
	{
		BackoutTransaction( pDb );
		return Err( ERR_MEMORY, "@" );
	}

	if( NewTransPart )
		*TransPart = *NewTransPart;
	TransPart->Next =
	TransPart->Prev = NULL;
	TransPart->Type = Type;

	if( pDb->TransTail )
		pDb->TransTail->Next = TransPart;
	else
		pDb->TransHead = TransPart;

	TransPart->Prev	= pDb->TransTail;
	pDb->TransTail	= TransPart;
//	DumpTransList( pDb->TransHead );

	return TRUE;
}



/****************************************************************
**	Routine: SecDbTransLogLast
**	Returns: ID of last completed transaction
**			 SDB_TRANS_ID_ERROR if error
**	Summary: Get the ID of the last transaction in the log
****************************************************************/

SDB_TRANS_ID SecDbTransLogLast(
	SDB_DB	*pDb		// Database to read from
)
{
	if( !pDb )
		return SDB_TRANS_ID_ERROR;

	pDb = SecDbDefaultDbUpdateGet( pDb );

	if( !pDb )
		return SDB_TRANS_ID_ERROR;

	return (*pDb->pfTransLogLast)( pDb );
}



/****************************************************************
**	Routine: SecDbTransLogHeader
**	Returns: TRUE	- Header retrieved without error
**			 FALSE	- Header not found for specified transaction
**	Summary: Retrieve transaction header
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbTransLogHeader"

int SecDbTransLogHeader(
	SDB_DB 				*pDb,		// Database to read from
	SDB_TRANS_ID		TransID,	// Transaction number to get
	SDB_TRANS_HEADER	*Header		// Returned header information
)
{
	if( !pDb )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb missing" );

	pDb = SecDbDefaultDbUpdateGet( pDb );

	if( !pDb )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get default update db" );

	return (*pDb->pfTransLogHeader)( pDb, TransID, Header );
}



/****************************************************************
**	Routine: SecDbTransLogDetail
**	Returns: Transaction list or NULL
**	Summary: Retrieve transaction detail
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbTransLogDetail"

SDB_TRANS_PART *SecDbTransLogDetail(
	SDB_DB				*pDb,		// Database to read from
	SDB_TRANS_HEADER	*Header		// From SecDbTransLogHeader
)
{
	SDB_TRANS_PART
			*TransList;


	if( !pDb )
		return (SDB_TRANS_PART *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb missing" );

	pDb = SecDbDefaultDbUpdateGet( pDb );

	if( !pDb )
		return (SDB_TRANS_PART *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get default update db" );

	TransList = (*pDb->pfTransLogDetail)( pDb, Header );

	// FIX: correct bad-citizen drivers that do not fill in Prev pointers
	if( TransList )
		for( SDB_TRANS_PART *Part = TransList; Part->Next; Part = Part->Next )
			Part->Next->Prev = Part;

	return( TransList );
}



/****************************************************************
**	Routine: SecDbTransFree
**	Returns: Nothing
**	Summary: Free memory in transaction list from SecDbTransLogDetail
****************************************************************/

void SecDbTransFree(
	SDB_TRANS_PART	*TransList		// Transaction list to free
)
{
	SDB_TRANS_PART
			*TransPart;


	for( TransPart = TransList; TransPart; TransPart = TransList )
	{
		TransList =  TransPart->Next;
		SecDbTransPartFree( TransPart );
	}
}



/****************************************************************
**	Routine: SecDbTransMap
**	Returns: TRUE or FALSE
**	Action : Retrieves transaction map for a given database
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbTransMap"

SDB_TRANS_MAP *SecDbTransMap(
	SDB_DB	*pDb
)
{
	if( !pDb )
		return (SDB_TRANS_MAP *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb missing" );

	pDb = SecDbDefaultDbUpdateGet( pDb );

	if( !pDb )
		return (SDB_TRANS_MAP *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Failed to get default update db" );

	return (*pDb->pfTransMap)( pDb );
}



/****************************************************************
**	Routine: SecDbTransMapFree
**	Returns: Nothing
**	Action : Free a transaction-map structure
****************************************************************/

void SecDbTransMapFree(
	SDB_TRANS_MAP	*TransMap
)
{
	SDB_TRANS_MAP	*Map,
					*Next;


	for( Map = TransMap; Map; Map = Next )
	{
		Next = Map->Next;
		free( Map );
	}
}


/****************************************************************
**	Routine: SecDbTransCommitConditional
**	Returns: TRUE for success, FALSE on error
**	Action : If called with TransDepth 1, and the size of the current
**			 transaction exceed the parameters, commit the transaction
**			 and start a new one.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbTransCommitConditional"

int SecDbTransCommitConditional(
	SDB_DB	*pDb,
	int		MaxCount,
	long	MaxSize,
	int		Force,
	int		*TransCommitted,		// Returned by ref
	int		*TransFailed			// Returned by ref
)
{
	int		TransDepth,
			CommitTrans,
			TransCount;

	long    TransSize;

	char	*Description;


	*TransCommitted	= FALSE;
	*TransFailed	= FALSE;
	TransDepth		= SecDbTransDepth( pDb );

	if( TransDepth > 0 &&
		SecDbTransFlags( pDb ) & SDB_TRAN_FLAG_ALLOW_ASYNC )
	{
		*TransFailed = TRUE;
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return Err( ERR_UNSUPPORTED_OPERATION, 
				"@ - Cannot conditionally commit within an async transaction.  Transaction aborted." );
	}

	if( 0 == TransDepth )
		return ErrMore( ARGCHECK_FUNCTION_NAME " can only be called from within a transaction" );

	if( 1 != TransDepth )
		return TRUE;

	CommitTrans = FALSE;
	if( !CommitTrans && MaxCount > 0 )
	{
		TransCount = SecDbTransPartCount( pDb );
		if( TransCount >= MaxCount )
			CommitTrans = TRUE;
	}
	if( !CommitTrans && MaxSize > 0L )
	{
		TransSize = SecDbTransSize( pDb );
		if( TransSize >= MaxSize )
			CommitTrans = TRUE;
	}

	if( !CommitTrans )
		return TRUE;

/*
**	Save the current transaction description
*/
	Description = SecDbTransDescription( pDb );		// Don't expect this to fail!

	if( !Description )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to find transaction description" );

/*
**	Commit transaction (unless already aborted)
*/
	if( Force )
		*TransCommitted	= SecDbTransCommitForce( pDb );
	else
		*TransCommitted	= SecDbTransCommit( pDb );

	if( !*TransCommitted )
	{
		free( Description );
		*TransFailed = TRUE;
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to commit transaction" );
	}

/*
**	Begin transaction and save trans depth
*/
	if( !SecDbTransBegin( pDb, Description ) )
	{
		free( Description );
		*TransFailed = TRUE;
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to start new transaction" );
	}

	free( Description );
	return TRUE;
}

