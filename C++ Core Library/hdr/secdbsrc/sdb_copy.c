#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_copy.c,v 1.23 2008/07/02 16:13:26 khodod Exp $"
/****************************************************************
**
**	SDB_COPY.C	- SecDbCopy - copy security from one Db to another
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_copy.c,v $
**	Revision 1.23  2008/07/02 16:13:26  khodod
**	New way to copy db source id in SecurityCopy and seccopy.
**	iBug: #56731
**
**	Revision 1.22  2005/05/27 20:19:28  e45019
**	SecDbCopyByName now takes Dup argument which if true causes a duplicate security to be created, and if false causes a new security to be created.
**	iBug: #25421
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<errno.h>
#include	<date.h>
#include	<secdb.h>
#include	<seccopy.h>
#include	<sectrace.h>
#include	<secdrv.h>
#include	<secerror.h>
#include	<sdb_int.h>


/*
**	Size of a streamed SDB_STORED_OBJECT
*/

#define STORED_OBJECT_STREAM_SIZE	64						// Size of fixed-size part of file layout
#define SDB_FILE_VERSION_OFFSET		( (BSTREAM_POS) 60 )	// Offset into stream of file layout version number
#define SDB_FILE_VERSION			1						// Current version of file layout


/****************************************************************
**	Routine: SecDbCopyByName
**	Returns: TRUE	- Security copied without error
**			 FALSE	- Error copying security (check Err())
**	Action : Copy a security from SrcDb to DstDb, optionally renaming it
**			 along the way. AddFlags controls whether the security should
**           be duplicated or a new security created.
**           AddFlags is passed along to SecDbAddWithFlags.
**           AddFlags may include a flag to set the src db id in the 
**           transaction that adds the object. This is used to simulate 
**           a secsync-generated transaction. See SecDbAddWithFlags.
** 
****************************************************************/
int SecDbCopyByName(
	const char	*SecName,
	SDB_DB		*SrcDb,
	SDB_DB		*DstDb,
	const char	*NewName,	// Optional (defaults to SecName).
	int         AddFlags	// Make exact duplicate, otherwise create new security. See SecDbAddWithFlags(). 
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_DISK
			SrcData;

	SDB_STORED_OBJECT
			StoredObj;

	char	SaveName[ SDB_SEC_NAME_SIZE ];

	int		Status = TRUE;

	SDB_M_DATA
			SdbMsgData;			

	if( !NewName )
		NewName = SecName;
	if(		SrcDb == DstDb
		&&	0 == stricmp( SecName, NewName ))
		return Err( ERR_INVALID_ARGUMENTS, "SecDbCopyByName( %s ), Can't copy to same database without changing name", SecName );

	// Get memory from first object, using stream version for destination 
	SecPtr = SecDbGetByName( SecName, SrcDb, SDB_IGNORE_PATH );
	if( !SecPtr )
		return ErrMore( "SecDbCopyByName( '%s' ), Can't find security", SecName );
	SrcData = SecPtr->SecData;
	M_SETUP_VERSION0( &SdbMsgData.Mem.Version, SecPtr, SDB_STREAM_NEW );
	if( !(SdbMsgData.Mem.Stream = SecDbBStreamCreateDefault()))
		Status = FALSE;
	else
		Status = SDB_MESSAGE( SecPtr, SDB_MSG_MEM, &SdbMsgData );
	SecDbFree( SecPtr );
	if( !Status )
		return ErrMore( "SecDbCopyByName( %s ), Can't get memory", SecName );

	SecPtr = SecDbNew( NewName, SrcData.Type, DstDb );
	if( !SecPtr )
		return ErrMore( "SecDbCopyByName( '%s' ), Can't create new security", NewName );
		
	// Make the SecData the same as before, but leave the name alone
	strcpy( SaveName, SecPtr->SecData.Name );
	SecPtr->SecData = SrcData;
	strcpy( SecPtr->SecData.Name, SaveName );
	SecDbDiskInfoSetVersion( &SecPtr->SecData, &SdbMsgData.Mem.Version );
	
	StoredObj.SdbDisk = SecPtr->SecData;
	StoredObj.MemSize = BStreamDataUsed( SdbMsgData.Mem.Stream );
	StoredObj.Mem     = SdbMsgData.Mem.Stream->DataStart;
	SecDbNewLoad( SecPtr, &StoredObj );
	SecDbBStreamDestroy( SdbMsgData.Mem.Stream, TRUE );
	Status = SecDbAddWithFlags( SecPtr, TRUE, AddFlags );
	SecDbFree( SecPtr );

	if( !Status )
		return ErrMore( "SecDbCopyByName( %s->%s ), Add failed", SecName, NewName );
	return TRUE;
}


/****************************************************************
**	Routine: SecDbAddStoredObject
**	Returns: TRUE	- Security Added without error
**			 FALSE	- Error copying security (check Err())
**	Action : Copy a security from StoredObj to DbIdTarget.
****************************************************************/

int SecDbAddStoredObject(
	SDB_STORED_OBJECT	*StoredObj,
	SDB_DB				*DbTarget
)
{
	SDB_OBJECT
		*SecPtr;

	int
		Status;

	SecPtr = SecDbNew( StoredObj->SdbDisk.Name, StoredObj->SdbDisk.Type, DbTarget );
	if( !SecPtr )
		return ErrMore( "SecDbAddStoredObject( '%s' ), Can't create new security", StoredObj->SdbDisk.Name );

	SecPtr->SecData = StoredObj->SdbDisk;
	Status = SecDbNewLoad( SecPtr, StoredObj );
	Status = Status && SecDbAddWithFlags( SecPtr, TRUE, SDB_ADD_PRESERVE_DISK_INFO );
	SecDbFree( SecPtr );
	return( Status );
}


/****************************************************************
**	Routine: BStreamGetStoredObj
**	Returns: void
**	Action : Unstream a SDB_STORED_OBJECT
****************************************************************/

static int BStreamGetStoredObj(
	BSTREAM *BStream,
	SDB_STORED_OBJECT *StoredObj
)
{
	BSTREAM_POS
			*Start;

	int		FileVersion;


	Start = BStreamTell( BStream );
	BStreamSeek( BStream, Start + SDB_FILE_VERSION_OFFSET );
	FileVersion = BStreamGetByte( BStream );
	BStreamSeek( BStream, Start );

	if( FileVersion == 0 )
	{
		memset( StoredObj, 0, sizeof( SDB_STORED_OBJECT ));
		BStreamGetMemory( BStream, StoredObj->SdbDisk.Name, SDB_SEC_NAME_SIZE  );
		StoredObj->SdbDisk.Type = BStreamGetInt16( BStream );
		StoredObj->SdbDisk.DateCreated = (unsigned short) TIME_TO_DATE( BStreamGetInt32( BStream ));		// Old TimeCreated
		StoredObj->SdbDisk.TimeUpdated = BStreamGetInt32( BStream );
		StoredObj->SdbDisk.UpdateCount = BStreamGetInt32( BStream );
		(void) BStreamGetInt16( BStream );		// Discard old DbIdCreated
		StoredObj->SdbDisk.DbIDUpdated = BStreamGetInt16( BStream );
		StoredObj->SdbDisk.VersionInfo = BStreamGetInt16( BStream );
		StoredObj->MemSize = BStreamGetInt16( BStream );
		return TRUE;
	}
	else if( FileVersion != SDB_FILE_VERSION )
		return Err( SDB_ERR_OBJECT_INVALID, "SecDb File Object: Version %d unknown", FileVersion );

	memset( StoredObj, 0, sizeof( SDB_STORED_OBJECT ));
	BStreamGetMemory( BStream, StoredObj->SdbDisk.Name, SDB_SEC_NAME_SIZE  );
	StoredObj->SdbDisk.Type = BStreamGetInt16( BStream );
	StoredObj->SdbDisk.LastTransaction = BStreamGetInt32( BStream );
	StoredObj->SdbDisk.TimeUpdated = BStreamGetInt32( BStream );
	StoredObj->SdbDisk.UpdateCount = BStreamGetInt32( BStream );
	StoredObj->SdbDisk.DateCreated = BStreamGetInt16( BStream );
	StoredObj->SdbDisk.DbIDUpdated = BStreamGetInt16( BStream );
	StoredObj->SdbDisk.VersionInfo = BStreamGetInt16( BStream );
	(void) BStreamGetInt16( BStream ); // Skip intentional size-fouling
	StoredObj->MemSize = BStreamGetUInt32( BStream );
	return TRUE;
}


/****************************************************************
**	Routine: BStreamPutStoredObj
**	Returns: void
**	Action : Stream a SDB_STORED_OBJECT
****************************************************************/

static void BStreamPutStoredObj(
	BSTREAM *BStream,
	SDB_STORED_OBJECT *StoredObj
)
{
	BSTREAM_POS
			*Start;


	Start = BStreamTell( BStream );
	BStreamPutMemory( BStream, StoredObj->SdbDisk.Name, SDB_SEC_NAME_SIZE  );
	BStreamPutInt16( BStream, StoredObj->SdbDisk.Type );
	BStreamPutInt32( BStream, StoredObj->SdbDisk.LastTransaction );
	BStreamPutInt32( BStream, StoredObj->SdbDisk.TimeUpdated );
	BStreamPutInt32( BStream, StoredObj->SdbDisk.UpdateCount );
	BStreamPutInt16( BStream, StoredObj->SdbDisk.DateCreated );
	BStreamPutInt16( BStream, StoredObj->SdbDisk.DbIDUpdated );
	BStreamPutInt16( BStream, StoredObj->SdbDisk.VersionInfo );
	BStreamPutInt16( BStream, -999 );	// Intentional size-fouling to guarantee version 0 failing
	BStreamPutUInt32( BStream, StoredObj->MemSize );
	BStreamSeek( BStream, Start + SDB_FILE_VERSION_OFFSET );
	BStreamPutByte( BStream, SDB_FILE_VERSION );
}


/****************************************************************
**	Routine: SecDbCopyToFile
**	Returns: TRUE/FALSE
**	Action : Append (i.e. stream) named security to File
****************************************************************/

int SecDbCopyToFile( 
	const SDB_FULL_SEC_NAME *SecName,
	SDB_DB					*DbSource,
	FILE					*File
)
{
	SDB_M_DATA
			SdbMsgData;

	SDB_STORED_OBJECT
			StoredObj;

	SDB_OBJECT
			*SecPtr;

	int
			Status = TRUE;

	// Get memory from first object, using stream version for destination 
	SecPtr = SecDbGetByName( SecName, DbSource, SDB_IGNORE_PATH );
	if( !SecPtr )
		return ErrMore( "SecDbCopyToFile( '%s' ), Can't find security", SecName );
	if( !(SdbMsgData.Mem.Stream = SecDbBStreamCreateDefault()))
	{
		SecDbFree( SecPtr );
		return( FALSE );
	}

	M_SETUP_VERSION0( &SdbMsgData.Mem.Version, SecPtr, SDB_STREAM_NEW );
	Status = SDB_MESSAGE( SecPtr, SDB_MSG_MEM, &SdbMsgData );
	memset( &StoredObj, 0, sizeof( StoredObj ));
	StoredObj.SdbDisk = SecPtr->SecData;
	SecDbFree( SecPtr );

	if( !Status )
		return Status;

	SecDbDiskInfoSetVersion( &StoredObj.SdbDisk, &SdbMsgData.Mem.Version );
	StoredObj.MemSize = BStreamDataUsed( SdbMsgData.Mem.Stream );
	StoredObj.Mem     = SdbMsgData.Mem.Stream->DataStart;

	Status = SecDbStoredObjectToFile( File, &StoredObj );

	SecDbBStreamDestroy( SdbMsgData.Mem.Stream, TRUE );

	return Status;
}


/****************************************************************
**	Routine: SecDbStoredObjectToFile
**	Returns: TRUE/FALSE
**	Action : Fill in Stored Object from File.
**           Note: caller is responsible for freeing StoredObj.Mem
****************************************************************/

int SecDbStoredObjectToFile(
	FILE	*File,
	SDB_STORED_OBJECT *StoredObj
)
{
	BSTREAM
			BStream;

	unsigned char
			StreamMem[ STORED_OBJECT_STREAM_SIZE ];

	memset( StreamMem, 0, STORED_OBJECT_STREAM_SIZE );
	BStreamInit( &BStream, StreamMem, STORED_OBJECT_STREAM_SIZE );
	BStreamPutStoredObj( &BStream, StoredObj );

	if( fwrite( StreamMem, STORED_OBJECT_STREAM_SIZE, 1, File ) != 1 )
		return Err( ERR_FILE_FAILURE, "File Write: %s", strerror( errno ));
	if(( StoredObj->MemSize > 0 )
	&& ( fwrite( StoredObj->Mem, StoredObj->MemSize, 1, File ) != 1 ))
		return Err( ERR_FILE_FAILURE, "File Write: %s", strerror( errno ));
	return TRUE;
}


/****************************************************************
**	Routine: SecDbStoredObjectFromFile
**	Returns: TRUE/FALSE
**	Action : Fill in Stored Object from File.
**           Note: caller is responsible for freeing StoredObj.Mem
****************************************************************/

int SecDbStoredObjectFromFile(
	FILE	*File,
	SDB_STORED_OBJECT *StoredObj
)
{
	BSTREAM
			BStream;

	unsigned char
			StreamMem[ STORED_OBJECT_STREAM_SIZE ];

	memset( StoredObj, 0, sizeof( SDB_STORED_OBJECT ));
	if( fread( StreamMem, STORED_OBJECT_STREAM_SIZE, 1, File ) != 1 )
		return Err( ERR_FILE_FAILURE, "File Read: %s", strerror( errno));
	BStreamInit( &BStream, StreamMem, STORED_OBJECT_STREAM_SIZE );
	if( !BStreamGetStoredObj( &BStream, StoredObj ))
		return FALSE;

	StoredObj->Mem = malloc( StoredObj->MemSize );
	if( !StoredObj->Mem )
		return FALSE;

	if(( StoredObj->MemSize > 0 )
	&& ( fread( StoredObj->Mem, StoredObj->MemSize, 1, File ) != 1 ))
	{
		free( StoredObj->Mem );
		return Err( ERR_FILE_FAILURE, "File Read: %s", strerror( errno));
	}

	return( TRUE );
}
