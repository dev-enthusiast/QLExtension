#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_trna.c,v 1.46 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SDB_TRNA.C	Convert a transaction to an DT_VALUE array of structures
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_trna.c,v $
**	Revision 1.46  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.45  2010/04/08 16:55:33  ellist
**	Support constrained (sparse) indices
**
**	Revision 1.44  2001/08/27 15:23:10  hsuf
**	Implemented Async Transaction to improve secsync performance.
**	Added Slang "TransactionAsync()" and its interaction with other trnx fns.
**	Support query/viewing of async flag in slang/secview.
**	Secdb server/client use version number to turn on/off async behavior.
**	Server's transaction header now has async flag (taken from reserved fields)
**
**	Revision 1.43  2001/07/17 00:40:51  vengrd
**	Operation tags for self-desc streams and support for deletion
**	
**	Revision 1.42  2001/06/20 13:48:24  vengrd
**	Compression of self-desc streams and tagging of classes as self-desc.
**	
**	Revision 1.41  2001/06/13 21:35:43  vengrd
**	SecDb objects no longer know about self-desc streaming.  It is external to them.
**	
**	Revision 1.40  2001/06/05 18:24:14  lundek
**	few changes on huge object reading from translist per sensitive code committee
**	
**	Revision 1.39  2001/05/24 18:56:51  lundek
**	More work on reconstituting huge objects from translist.
**	Now, huge objects that are subsequently updated in the transaction are done
**	correctly.
**	Huge objects that do not have all the pages available in the transaction are
**	failed with meaningful error message.
**	Added some meaningful comments.
**	
**	Revision 1.38  2001/05/24 15:17:58  lundek
**	Fix crashing/corruption in SecDbTransListToArray for huge objects that are updated more than once in the same translist
**	
**	Revision 1.37  2001/01/24 15:57:32  abramjo
**	Added DbID to header structure in SecDbTransHeaderToStructure()
**	
**	Revision 1.36  2000/09/06 13:52:40  goodfj
**	Move (duplicated) huge obj defines to sdb_int.h
**	
**	Revision 1.35  2000/07/06 16:58:19  goodfj
**	Added SecDbTransSize
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<malloc.h>
#include	<kool_ade.h>

#include	<secdb.h>
#include	<mempool.h>
#include	<secexpr.h>
#include	<secindex.h>
#include	<sectrans.h>
#include	<secerror.h>

#include	<secver.h>
#include	<sdb_int.h>

#include	<bstream.h>
#include 	<datatype/SelfDescStream.h>


CC_USING_NAMESPACE( Gs );


/****************************************************************
**	Routine: TranslateSdbDisk
**	Returns: TRUE or FALSE
**	Action : Put SdbDisk fields in a substructure
****************************************************************/

static int TranslateSdbDisk(
	DT_VALUE	*Struct,
	const char	*Tag,
	SDB_DISK	*SdbDisk
)
{
	DT_VALUE
			SubStruct;


	if( SecDbDiskInfoToStructure( SdbDisk, &SubStruct ))
	{
		DtComponentSet( Struct, Tag, &SubStruct );
		DTM_FREE( &SubStruct );
	}
	else
		DtComponentSetNumber( Struct, Tag, DtNull, 0 );

	return TRUE;
}



/****************************************************************
**	Routine: TranslateMem
**	Returns:
**	Action : Dump the IN_STREAM values for a security
****************************************************************/

static int TranslateMem(
	DT_VALUE			*Struct,
	const char			*Tag,
	SDB_OBJECT			*SecPtr
)
{
	SDB_ENUM_PTR
			EnumPtr;

	SDB_VALUE_TYPE_INFO
			*Vti;

	DT_VALUE
			*TempValue,
			SubStruct;


	DTM_ALLOC( &SubStruct, DtStructure );

	for(	Vti = SecDbValueTypeEnumFirst( SecPtr, &EnumPtr );
			Vti;
			Vti = SecDbValueTypeEnumNext( EnumPtr ))
	{
		if( Vti->ValueFlags & SDB_IN_STREAM )
		{
			TempValue = SecDbGetValue( SecPtr, Vti->ValueType );
			if( TempValue )
				DtComponentSet( &SubStruct, Vti->ValueType->Name, TempValue );
		}
	}
	SecDbValueTypeEnumClose( EnumPtr );

	DtComponentSet( Struct, Tag, &SubStruct );
	DTM_FREE( &SubStruct );
	return( TRUE );
}



/****************************************************************
**	Routine: TranslateStoredObject
**	Returns: TRUE or FALSE
**	Action : Put StoredObject fields in a substructure
****************************************************************/

static int TranslateStoredObject(
	DT_VALUE			*Struct,
	const char			*Tag,
	SDB_STORED_OBJECT	*StoredObject,
	int					Raw
)
{
	DT_VALUE
			SubStruct;

	SDB_OBJECT
			*SecPtr;


	DTM_ALLOC( &SubStruct, DtStructure );

	TranslateSdbDisk( &SubStruct, "SdbDisk", &StoredObject->SdbDisk );

	if( StoredObject->MemSize > 0 )
	{
		if( Raw )
		{
			DT_BINARY
					Binary;


			DtComponentSetNumber( &SubStruct, "Mem Size", 	DtDouble, (double) StoredObject->MemSize );
			Binary.Size 	= StoredObject->MemSize;
			Binary.Memory 	= StoredObject->Mem;
			DtComponentSetPointer( &SubStruct, "Mem", DtBinary, &Binary );
		}
		else
		{
			SecPtr = SecDbNew( NULL, StoredObject->SdbDisk.Type, SecDbRootDb );
			if( SecPtr )
			{
				if( SecDbNewLoad( SecPtr, StoredObject ))
					TranslateMem( &SubStruct, "Mem", SecPtr );
				SecDbFree( SecPtr );
			}
		}
	}

	DtComponentSet( Struct, Tag, &SubStruct );
	DTM_FREE( &SubStruct );
	return( TRUE );
}



/****************************************************************
**	Routine: TranslateValues
**	Returns: TRUE or FALSE
**	Action : Copies values into array
****************************************************************/

static int TranslateValues(
	DT_VALUE			*Struct,
	const char			*Tag,
	DT_VALUE			*Values,
	int					ValueCount
)
{
	DT_VALUE
			Array;

	int		Elem;


	DTM_ALLOC( &Array, DtArray );
	for( Elem = 0; Elem < ValueCount; Elem++ )
		DtSubscriptSet( &Array, Elem, &Values[ Elem ] );
	DtComponentSet( Struct, Tag, &Array );
	DTM_FREE( &Array );
	return( TRUE );
}



/****************************************************************
**	Routine: TranslateParts
**	Returns: TRUE or FALSE
**	Action : Copies index parts into array
****************************************************************/

static int TranslateParts(
	DT_VALUE					*Struct,
	const char					*Tag,
	SDB_INDEX_DESCRIPTOR_PART	*Parts,
	int							PartCount
)
{
	DT_VALUE
			SubStruct,
			Array;

	int		PartNum;


	DTM_ALLOC( &Array, DtArray );
	for( PartNum = 0; PartNum < PartCount; PartNum++ )
	{
		DTM_ALLOC( &SubStruct, DtStructure );
		DtComponentSetPointer( &SubStruct, "Value Type Name", DtString, Parts[ PartNum ].ValueTypeName );
		DtComponentSetPointer( &SubStruct, "Data Type Name",  DtString, Parts[ PartNum ].DataTypeName  );
		DtComponentSetNumber(  &SubStruct, "Flags",           DtDouble, (double) Parts[ PartNum ].Flags         );
		DtComponentSetNumber(  &SubStruct, "Byte Width",      DtDouble, (double) Parts[ PartNum ].ByteWidth     );
		DtComponentSetNumber(  &SubStruct, "Offset",          DtDouble, (double) Parts[ PartNum ].Offset		);
		DtSubscriptSet( &Array, PartNum, &SubStruct );
	}
	DtComponentSet( Struct, Tag, &Array );
	DTM_FREE( &Array );
	return( TRUE );
}



/****************************************************************
**	Routine: SecDbTransListToObjectNames
**	Returns: Allocated DT_VALUE with structure of object names
**	Action : Return an structure of object names referenced
****************************************************************/

DT_VALUE *SecDbTransListToObjectNames(
	SDB_TRANS_PART	*TransList
)
{
	SDB_TRANS_PART
			*Trans;

	DT_VALUE
			*Struct;

	const char
			*SecName1,
			*SecName2,
			*Type1,
			*Type2 = NULL;


	if( !(Struct = (DT_VALUE *) malloc( sizeof( *Struct ))))
	{
		Err( ERR_MEMORY, "@" );
		return NULL;
	}

	DTM_ALLOC( Struct, DtStructure );
	for( Trans = TransList; Trans; Trans = Trans->Next )
	{
		SecName1 	= 
		SecName2 	= NULL;
		Type1		= SecDbTransStrTab[ Trans->Type ];

		switch( Trans->Type )
		{
			case SDB_TRAN_INSERT:
				SecName1 = Trans->Data.Op.StoredObject.SdbDisk.Name;
				break;

			case SDB_TRAN_UPDATE:
				SecName1 = Trans->Data.Op.StoredObject.SdbDisk.Name;
				break;

			case SDB_TRAN_INCREMENTAL:
				SecName1 = Trans->Data.Incremental.SdbDisk.Name;
				break;

			case SDB_TRAN_DELETE:
				SecName1 = Trans->Data.Delete.SdbDisk.Name;
				break;

			case SDB_TRAN_RENAME:
				SecName1 = Trans->Data.Rename.NewSdbDisk.Name;
				Type1 = "RenameTo";
				SecName2 = Trans->Data.Rename.OldSdbDisk.Name;
				Type2 = "RenameFrom";
				break;

			default:
				break;
		}

		if( SecName1 )
			DtComponentSetPointer( Struct, SecName1, DtString, Type1 );
		if( SecName2 )
			DtComponentSetPointer( Struct, SecName2, DtDouble, Type2 );
	}

	return( Struct );
}


/****************************************************************
**	Routine: PageTableAssembleStreamTrans
**	Returns: TRUE or FALSE
**	Action : Given a page table, reads the pages from the translist
**			 and pastes them together into a single BStream
****************************************************************/

static int PageTableAssembleStreamTrans(
	SDB_TRANS_PART	*TransList,
	SDB_TRANS_PART	*TransListEnd,		// Last trans part to consider as possible huge page
	const char		*PageTable,
	unsigned long	*pMemSize,
	void			**pMem
)
{
	int		Count,
			PageSize;

	char	*Mem,
			*CurMem;

	const char
			*SecName;

	SDB_STORED_OBJECT
			*StoredObject;


	Count = 0;
	for( SecName = PageTable; *SecName; SecName += strlen( SecName ) + 1 )
		Count++;

	if( !ERR_MALLOC( Mem, char, Count * SDB_HUGE_PAGE_SIZE ))
		return FALSE;
	CurMem = Mem;


/*
**	Idea is to retrieve all the page objects from the translist and piece them together
**	to form the memory stream for the huge object.  The page objects are named in the
**	PageTable (which came from the raw binary stream of the huge object).
**
**	This only works if all of the referenced page objects are in the translist.  If not
**	there we have no idea where, i.e., what database and as of when, to get them from.
**	This situation is entirely possible, since the huge object support does not bother
**	writing out unchanged pages.  In this case this routine fails and causes
**	SecDbTransListToArray to exclude the Stored Object.Mem component from its translation
**	of that transaction part.
*/

	for( SecName = PageTable; *SecName; SecName += strlen( SecName ) + 1 )
	{
		// For each page look backward through translist for latest version of it
		{
			SDB_TRANS_PART	*Part;

			StoredObject = NULL;
			for( Part = TransListEnd; Part; Part = Part->Prev )
			{
				if( Part->Type != SDB_TRAN_UPDATE && Part->Type != SDB_TRAN_INSERT )
					continue;
				if( 0 != stricmp( Part->Data.Op.StoredObject.SdbDisk.Name, SecName ))
					continue;

				StoredObject = &Part->Data.Op.StoredObject;
				break;
			}
			if( !StoredObject )
			{
				free( Mem );
				return Err( ERR_UNKNOWN, "Cannot construct complete object for '%s' from translist\nnot all huge pages are in transaction", SecName );
			}
		}
		PageSize = StoredObject->MemSize - SDB_SEC_NAME_SIZE;
		memcpy( CurMem, StoredObject->Mem, PageSize );
		CurMem += PageSize;
	}
	*pMem 		= Mem;
	*pMemSize 	= CurMem - Mem;
	return TRUE;
}



/****************************************************************
**	Routine: SecDbHugeTranslate
**	Returns: TRUE or FALSE
**	Action : If the object mem contains a page table for a huge
**			 object, constructs the stream and keeps track of the
**			 page table.
****************************************************************/

static int SecDbHugeTranslate(
	SDB_TRANS_PART		*TransList,
	SDB_TRANS_PART		*TransListEnd,
	SDB_STORED_OBJECT	*Object
)
{
	char	*PageTable;


	if( SDB_IS_HUGE( Object->SdbDisk.VersionInfo ))
	{
		// Construct expanded stream while keeping track of page table in the hash
		PageTable = (char *) Object->Mem;
		if( !PageTableAssembleStreamTrans( TransList, TransListEnd, PageTable, &Object->MemSize, &Object->Mem ))
		{
			return ErrMore( "Assembling pages for %s, version %d", Object->SdbDisk.Name, Object->SdbDisk.UpdateCount );
		}
		free( PageTable );

		// Mark disk as not being huge since its pagetable is now in the hash
		SDB_SET_HUGE( Object->SdbDisk.VersionInfo, FALSE );
	}
	return TRUE;
}



/****************************************************************
**	Routine: SecDbTransListToArray
**	Returns: Allocated DT_VALUE with array of trans list
**	Action : Creates an Array value from given TransList
****************************************************************/

DT_VALUE *SecDbTransListToArray(
	SDB_TRANS_PART	*TransList,
	int				RawMem
)
{
	SDB_TRANS_PART
			*Trans;

	int		Elem = 0;

	DT_VALUE
			Struct,
			*Array;

	DT_BINARY
			Binary;


	if( !( Array = (DT_VALUE *) malloc( sizeof( *Array ))))
		return (DT_VALUE *) ErrN( ERR_MEMORY, "@" );

	DTM_ALLOC( Array, DtArray );
	for( Trans = TransList; Trans; Trans = Trans->Next )
	{
		DTM_ALLOC( &Struct, DtStructure );

		DtComponentSetPointer( &Struct, "Type", DtString, SecDbTransStrTab[ Trans->Type ] );

		switch( Trans->Type )
		{
			case SDB_TRAN_INSERT:
				if( !RawMem )
					SecDbHugeTranslate( TransList, Trans, &Trans->Data.Op.StoredObject );
				TranslateStoredObject( &Struct, "Stored Object", &Trans->Data.Op.StoredObject, RawMem );
				break;

			case SDB_TRAN_UPDATE:
				if( !RawMem )
					SecDbHugeTranslate( TransList, Trans, &Trans->Data.Op.StoredObject );
				TranslateStoredObject( &Struct, "Stored Object", &Trans->Data.Op.StoredObject, RawMem );
				TranslateSdbDisk( &Struct, "OldSdbDisk", &Trans->Data.Op.OldSdbDisk );
				break;

			case SDB_TRAN_INCREMENTAL:
				TranslateSdbDisk( &Struct, "SdbDisk", &Trans->Data.Incremental.SdbDisk );
				DtComponentSetNumber( &Struct, "Msg Mem Size", 	DtDouble, (double) Trans->Data.Incremental.MsgMemSize );
				Binary.Size 	= Trans->Data.Incremental.MsgMemSize;
				Binary.Memory 	= Trans->Data.Incremental.MsgMem;
				DtComponentSetPointer( &Struct, "Msg Mem", DtBinary, &Binary );
				DtComponentSetNumber( &Struct, "Direction", DtDouble, (double) Trans->Data.Incremental.Direction );
				break;

			case SDB_TRAN_DELETE:
				TranslateSdbDisk( &Struct, "SdbDisk", &Trans->Data.Delete.SdbDisk );
				DtComponentSetNumber( &Struct, "Ignore Errors", DtDouble, (double) Trans->Data.Delete.IgnoreErrors );
				break;

			case SDB_TRAN_RENAME:
				TranslateSdbDisk( &Struct, "New SdbDisk", &Trans->Data.Rename.NewSdbDisk );
				TranslateSdbDisk( &Struct, "Old SdbDisk", &Trans->Data.Rename.OldSdbDisk );
				break;

			case SDB_TRAN_INDEX_CREATE:
				DtComponentSetPointer( &Struct, "Index Name", DtString, Trans->Data.IndexCreate.IndexDescriptor->Name );
				DtComponentSetNumber( &Struct, "Total Byte Width", DtDouble, (double) Trans->Data.IndexCreate.IndexDescriptor->TotalByteWidth );
				TranslateParts( &Struct, "Parts", Trans->Data.IndexCreate.IndexDescriptor->Parts, Trans->Data.IndexCreate.IndexDescriptor->PartCount );
				break;

			case SDB_TRAN_INDEX_DESTROY:
				DtComponentSetPointer( &Struct, "Index Name", DtString, Trans->Data.IndexDestroy.IndexName );
				break;

			case SDB_TRAN_INDEX_INSERT:
			case SDB_TRAN_INDEX_UPDATE:
				DtComponentSetPointer( &Struct, "Index Name", DtString, Trans->Data.IndexOp.Data->Index->Name );
				DtComponentSetNumber(  &Struct, "Part Count", DtDouble, (double) Trans->Data.IndexOp.Data->Index->PartCount );
				DtComponentSetPointer( &Struct, "SecName", DtString, Trans->Data.IndexOp.Data->SecName );
				TranslateValues( &Struct, "Values", Trans->Data.IndexOp.Data->Values, Trans->Data.IndexOp.Data->Index->PartCount );
				break;

			case SDB_TRAN_INDEX_INCREMENTAL:
				DtComponentSetPointer( &Struct, "Index Name", DtString, Trans->Data.IndexIncremental.Index->Name );
				DtComponentSetPointer( &Struct, "SecName",    DtString, Trans->Data.IndexIncremental.SecName );
				break;

			case SDB_TRAN_INDEX_DELETE:
				DtComponentSetPointer( &Struct, "Index Name", DtString, Trans->Data.IndexDelete.IndexName );
				DtComponentSetPointer( &Struct, "SecName", DtString, Trans->Data.IndexDelete.SecName );
				DtComponentSetNumber( &Struct, "Ignore Errors", DtDouble, (double) Trans->Data.IndexDelete.IgnoreErrors );
				break;

			case SDB_TRAN_INDEX_RENAME:
				DtComponentSetPointer( &Struct, "Index Name", DtString, Trans->Data.IndexRename.IndexName );
				DtComponentSetPointer( &Struct, "New SecName", DtString, Trans->Data.IndexRename.NewSecName );
				DtComponentSetPointer( &Struct, "Old SecName", DtString, Trans->Data.IndexRename.OldSecName );
				break;

			case SDB_TRAN_FILE_DELETE:
				DtComponentSetNumber( &Struct, "File", DtDouble, Trans->Data.FileDelete.File );
				break;

			case SDB_TRAN_FILE_WRITE:
				DtComponentSetNumber( &Struct, "File",     DtDouble, Trans->Data.FileWrite.File );
				Binary.Size 	= Trans->Data.FileWrite.Length;
				Binary.Memory 	= (void *)Trans->Data.FileWrite.Data;
				DtComponentSetPointer( &Struct, "Data",    DtBinary, &Binary );
				DtComponentSetNumber( &Struct, "Length",   DtDouble, Trans->Data.FileWrite.Length );
				DtComponentSetNumber( &Struct, "Position", DtDouble, Trans->Data.FileWrite.Position );
				break;

			case SDB_TRAN_FILE_INFO_SET:
				DtComponentSetNumber( &Struct, "File",       DtDouble, Trans->Data.FileInfoSet.File );
				DtComponentSetNumber( &Struct, "LastAccess", DtTime,   Trans->Data.FileInfoSet.Info.LastAccess );
				DtComponentSetNumber( &Struct, "LastUpdate", DtTime,   Trans->Data.FileInfoSet.Info.LastUpdate );
				DtComponentSetNumber( &Struct, "Length",     DtDouble, Trans->Data.FileInfoSet.Info.Length     );
				break;

			case SDB_TRAN_SELF_DESC_DATA:
			{
				BSTREAM 
						InputStream;

				DT_VALUE
						OutputStruct;

				unsigned
						StreamOperation,
						StreamType;

				BStreamInit( &InputStream, Trans->Data.SelfDesc.Data, Trans->Data.SelfDesc.DataSize );

				// We just happen to know that the stream operation and stream version and cached here.

				StreamOperation = BStreamGetUInt16( &InputStream ); 
				StreamType 	    = BStreamGetUInt16( &InputStream ); 

				DtComponentSetNumber( &Struct, "Stream Operation", DtDouble, StreamOperation );
				DtComponentSetNumber( &Struct, "Stream Version",   DtDouble, StreamType );
				DtComponentSetNumber( &Struct, "Stream Size",      DtDouble, Trans->Data.SelfDesc.DataSize );

				// Don't try to read the operation again, but do read the type.
				BStreamSeek( &InputStream, InputStream.DataStart + sizeof( UINT16 ) );

				if( !GsSelfDescStreamToStruct( &InputStream, &OutputStruct ) )
					DtComponentSetPointer( &Struct, "Contents", DtString, "Failed to unstream contents."  );
				else
					DtComponentSetPointer( &Struct, "Contents", DtStructure, DT_VALUE_TO_POINTER( &OutputStruct ) );

				break;
			}

			default:
				break;	// otherwise, skip it.
		}
		DtSubscriptSet( Array, Elem++, &Struct );
		DTM_FREE( &Struct );
	}
	return( Array );
}


/****************************************************************
**	Routine: SdbDiskSize
**	Returns: Size of SdbDisk
**	Action : 
****************************************************************/

static int SdbDiskSize(
	SDB_DISK	*SdbDisk
)
{
	return 4 * sizeof( short ) + 3 * sizeof( long ) + strlen( SdbDisk->Name ) + 1;
}


/****************************************************************
**	Routine: StoredObjectSize
**	Returns: Size of StoredObject
**	Action : 
****************************************************************/

static int StoredObjectSize(
	SDB_STORED_OBJECT	*Obj
)
{
	return SdbDiskSize( &Obj->SdbDisk ) + sizeof( short ) + Obj->MemSize;
}


/****************************************************************
**	Routine: IndexPartsSize
**	Returns: Size of Index Parts
**	Action : 
****************************************************************/

static int IndexPartsSize(
	SDB_INDEX_DESCRIPTOR_PART	*Parts,
	int							PartCount
)
{
	int		PartNum,
			Size = 0;


	for( PartNum = 0; PartNum < PartCount; ++PartNum )
		Size += 3 * sizeof( short ) + strlen( Parts[ PartNum ].ValueTypeName ) + strlen( Parts[ PartNum ].DataTypeName );

	return Size;
}


/****************************************************************
**	Routine: IndexValuesSize
**	Returns: Size of Index values
**	Action : 
****************************************************************/

static int IndexValuesSize(
	DT_VALUE	*Values,
	int			ValueCount
)
{
	DT_VALUE
			StringValue;

	int		ValueNum,
			Size = 0;


	for( ValueNum = 0; ValueNum < ValueCount; ++ValueNum )
		if( Values[ ValueNum ].DataType->Flags & DT_FLAG_NUMERIC )
			Size += sizeof( double );
		else if( Values[ ValueNum ].DataType == DtString )
			Size += strlen( (char *) Values[ ValueNum ].Data.Pointer ) + 1;
		else
		{
			StringValue.DataType = DtString;
			if( DTM_TO( &StringValue, &Values[ ValueNum ] ) )
			{
				Size += strlen( (char *) StringValue.Data.Pointer ) + 1;
				DTM_FREE( &StringValue );
			}
		}

	return Size;
}


/****************************************************************
**	Routine: SecDbTransSize
**	Returns: Size of binary in transaction
**	Action : Sum the size of all transaction parts
**			 Try to emulate src/osecserv/sec/svr_tapi.c/SecSrvTransCommit
****************************************************************/

long SecDbTransSize(
	SDB_DB	*pDb
)
{
	SDB_TRANS_PART
			*TransPart;

	long	TransSize = 0L;


	if( 0 == SecDbTransPartCount( pDb ) )
		return TransSize;

	for( TransPart = pDb->TransHead; TransPart; TransPart = TransPart->Next )
	{
		if( (TransPart->Type == SDB_TRAN_BEGIN && TransPart != pDb->TransHead) || TransPart->Type == SDB_TRAN_COMMIT )
			continue;

		TransSize += sizeof( short ) + sizeof( short );	// PartSize + MsgNum

		switch( TransPart->Type )
		{
			case SDB_TRAN_BEGIN:		// This info goes in TransLogHeader
				TransSize += 4 * sizeof( short ) + sizeof( int )  + strlen( TransPart->Data.Begin.SecName ) + 1;
				break;

			case SDB_TRAN_INSERT:
				TransSize += StoredObjectSize( &TransPart->Data.Op.StoredObject );
				break;

			case SDB_TRAN_UPDATE:
				TransSize += StoredObjectSize( &TransPart->Data.Op.StoredObject );
				TransSize += SdbDiskSize( &TransPart->Data.Op.OldSdbDisk );
				break;

			case SDB_TRAN_INCREMENTAL:
				TransSize += SdbDiskSize( &TransPart->Data.Incremental.SdbDisk );
				TransSize += sizeof( short );		// MsgMemSize
				TransSize += TransPart->Data.Incremental.MsgMemSize;
				TransSize += sizeof( short );		// Direction
				break;

			case SDB_TRAN_DELETE:
				TransSize += SdbDiskSize( &TransPart->Data.Delete.SdbDisk );
                TransSize += sizeof( short );		// IgnoreErrors
				break;

			case SDB_TRAN_RENAME:
				TransSize += SdbDiskSize( &TransPart->Data.Rename.NewSdbDisk );
				TransSize += SdbDiskSize( &TransPart->Data.Rename.OldSdbDisk );
				break;

			case SDB_TRAN_INDEX_CREATE:
				TransSize += strlen( TransPart->Data.IndexCreate.IndexDescriptor->Name ) + 1;
				TransSize += sizeof( short );			// TotalByteWidth
				TransSize += IndexPartsSize( TransPart->Data.IndexCreate.IndexDescriptor->Parts, TransPart->Data.IndexCreate.IndexDescriptor->PartCount );
				break;

			case SDB_TRAN_INDEX_DESTROY:
				TransSize += strlen( TransPart->Data.IndexDestroy.IndexName ) + 1;
				break;

			case SDB_TRAN_INDEX_INSERT:
			case SDB_TRAN_INDEX_UPDATE:
				TransSize += sizeof( short );			// IndexNumber
				TransSize += strlen( TransPart->Data.IndexOp.Data->SecName ) + 1;
				TransSize += IndexValuesSize( TransPart->Data.IndexOp.Data->Values, TransPart->Data.IndexOp.Data->Index->PartCount );
				break;

			case SDB_TRAN_INDEX_INCREMENTAL:
				TransSize += sizeof( short );			// IndexNumber
				TransSize += strlen( TransPart->Data.IndexIncremental.SecName ) + 1;
				break;

			case SDB_TRAN_INDEX_DELETE:
				TransSize += sizeof( short );			// IndexNumber
				TransSize += strlen( TransPart->Data.IndexDelete.SecName ) + 1;
                TransSize += sizeof(short );			// IgnoreErrors
				break;

			case SDB_TRAN_INDEX_RENAME:
				TransSize += sizeof( short );			// IndexNumber
				TransSize += strlen( TransPart->Data.IndexRename.NewSecName ) + 1;
				TransSize += strlen( TransPart->Data.IndexRename.OldSecName ) + 1;
				break;

			case SDB_TRAN_SOURCE_INFO:	// This info goes in TransLogHeader
				break;

			case SDB_TRAN_FILE_DELETE:
			case SDB_TRAN_FILE_WRITE:
			case SDB_TRAN_FILE_INFO_SET:
				break;

			default:
				break;	// otherwise, skip it.
		}
	}

	return TransSize;
}



/****************************************************************
**	Routine: SecDbTransListToStructure
**	Returns: Allocated DT_VALUE with structure by object names
**	Action : Return an structure tagged by object name with
**			 Type, PriorTransaction, SdbDisk, and Mem
****************************************************************/

DT_VALUE *SecDbTransListToStructure(
	SDB_TRANS_PART	*TransList
)
{
	SDB_TRANS_PART
			*Trans;

	DT_VALUE
			*Struct,
			SubStruct;

	const char
			*SecName1,
			*SecName2,
			*Type1,
			*Type2 = NULL;

	SDB_DISK
			*SdbDisk1 = NULL,
			*SdbDisk2 = NULL;

	SDB_STORED_OBJECT
			*StoredObj;

	SDB_OBJECT
			*SecPtr;

	SDB_TRANS_ID
			PriorTrans = 0;


	if( !(Struct = (DT_VALUE *) malloc( sizeof( *Struct ))))
	{
		Err( ERR_MEMORY, "@" );
		return NULL;
	}

	DTM_ALLOC( Struct, DtStructure );
	for( Trans = TransList; Trans; Trans = Trans->Next )
	{
		SecName1 	= 
		SecName2 	= NULL;
		Type1		= SecDbTransStrTab[ Trans->Type ];
		StoredObj	= NULL;

		switch( Trans->Type )
		{
			case SDB_TRAN_INSERT:
			case SDB_TRAN_UPDATE:
				SecName1 	= Trans->Data.Op.StoredObject.SdbDisk.Name;
				StoredObj	= &Trans->Data.Op.StoredObject;
				SdbDisk1	= &StoredObj->SdbDisk;
				PriorTrans	= Trans->Data.Op.OldSdbDisk.LastTransaction;
				break;

			case SDB_TRAN_INCREMENTAL:
				SecName1 	= Trans->Data.Incremental.SdbDisk.Name;
				SdbDisk1 	= &Trans->Data.Incremental.SdbDisk;
				PriorTrans	= Trans->Data.Incremental.OldSdbDisk.LastTransaction;
				break;

			case SDB_TRAN_DELETE:
				SecName1 	= Trans->Data.Delete.SdbDisk.Name;
				SdbDisk1 	= &Trans->Data.Delete.SdbDisk;
				PriorTrans	= Trans->Data.Delete.SdbDisk.LastTransaction;
				break;

			case SDB_TRAN_RENAME:
				SecName1	= Trans->Data.Rename.NewSdbDisk.Name;
				Type1 		= "RenameTo";
				SdbDisk1	= &Trans->Data.Rename.NewSdbDisk;
				PriorTrans	= Trans->Data.Rename.OldSdbDisk.LastTransaction;

				SecName2 	= Trans->Data.Rename.OldSdbDisk.Name;
				Type2 		= "RenameFrom";
				SdbDisk2	= &Trans->Data.Rename.OldSdbDisk;
				break;

			default:
				break;
		}

		if( SecName1 )
		{
			DTM_ALLOC( &SubStruct, DtStructure );

			DtComponentSetPointer( &SubStruct, "Type", DtString, Type1 );
			DtComponentSetNumber( &SubStruct, "PriorTransaction", DtDouble, PriorTrans );
			TranslateSdbDisk( &SubStruct, "SdbDisk", SdbDisk1 );
			if( StoredObj )
			{
				SecDbHugeTranslate( TransList, Trans, StoredObj );
				SecPtr = SecDbNew( NULL, StoredObj->SdbDisk.Type, SecDbRootDb );
				if( SecPtr )
				{
					if( SecDbNewLoad( SecPtr, StoredObj ))
						TranslateMem( &SubStruct, "Mem", SecPtr );
					SecDbFree( SecPtr );
				}
			}

			DtComponentSet( Struct, SecName1, &SubStruct );
			DTM_FREE( &SubStruct );
		}

		if( SecName2 )
		{
			DTM_ALLOC( &SubStruct, DtStructure );

			DtComponentSetPointer( &SubStruct, "Type", 	DtString, Type2 );
			TranslateSdbDisk( &SubStruct, "SdbDisk", SdbDisk2 );
			DtComponentSetNumber( &SubStruct, "PriorTransaction", DtDouble, PriorTrans );

			DtComponentSet( Struct, SecName2, &SubStruct );
			DTM_FREE( &SubStruct );
		}
	}

	return( Struct );
}



/****************************************************************
**	Routine: SecDbTransHeaderToStructure
**	Returns: Allocated DT_VALUE with structure of trans header
**	Action : Creates a Structure value from given header
****************************************************************/

DT_VALUE *SecDbTransHeaderToStructure(
	SDB_TRANS_HEADER	*Header
)
{
	SDB_DB_INFO
			DbInfo;

	char	Buffer[ 80 ];

	const char
			*SecType;

	DT_VALUE
			*Structure;


	if( !( Structure = (DT_VALUE *) malloc( sizeof( *Structure ))))
	{
		Err( ERR_MEMORY, "@" );
		return NULL;
	}

	DTM_ALLOC( Structure, DtStructure );
	DtComponentSetNumber( Structure, "Trans ID",    		DtDouble, (double) Header->TransID			);
	DtComponentSetNumber( Structure, "DbID", 				DtDouble, (double) Header->DbID 			);
	DtComponentSetNumber( Structure, "Detail Key1",			DtDouble, (double) Header->DetailKey1		);
	DtComponentSetNumber( Structure, "Detail Key2",			DtDouble, (double) Header->DetailKey2		);
	DtComponentSetNumber( Structure, "Detail Parts",		DtDouble, (double) Header->DetailParts		);
	DtComponentSetNumber( Structure, "Detail Bytes",		DtDouble, (double) Header->DetailBytes		);
	DtComponentSetNumber( Structure, "Source Trans ID",		DtDouble, (double) Header->SourceTransID	);
	DtComponentSetNumber( Structure, "GM Time",				DtTime,   (double) Header->GMTime			);
	DtComponentSetPointer( Structure, "User Name",			DtString, Header->UserName					);
	DtComponentSetPointer( Structure, "Application Name",	DtString, Header->ApplicationName			);
	DtComponentSetPointer( Structure, "Type",				DtString, Header->Type == SDB_TRAN_COMMIT ? "Commit" : "Abort" );
	DtComponentSetPointer( Structure, "SecName", 			DtString, Header->SecName 					);

	SecType = SecDbClassNameFromType( Header->SecType );
	if( SecType )
		DtComponentSetPointer( Structure, "SecType",		  	DtString, SecType );
	else
		DtComponentSetNumber( Structure, "SecType",			DtNull, 0 );

	if( SecDbDbInfoFromID( Header->DbID, &DbInfo ))
		DtComponentSetPointer( Structure, "Database", DtString, DbInfo.DbName );

	DtComponentSetPointer( Structure, "Trans Type", DtString, SecDbTransStrTab[ Header->TransType ] );

	sprintf( Buffer, "%d.%d.%d.%d",
			(int) ((Header->NetworkAddress & 0xFF000000) >> 24),
			(int) ((Header->NetworkAddress & 0x00FF0000) >> 16),
			(int) ((Header->NetworkAddress & 0x0000FF00) >> 8 ),
			(int) ((Header->NetworkAddress & 0x000000FF) >> 0 ));
	DtComponentSetPointer( Structure, "Network Address",		DtString, Buffer );

	DtComponentSetNumber( Structure, "Allow Async Transaction", DtDouble, (double) ( Header->TransFlags & SDB_TRAN_FLAG_ALLOW_ASYNC ) );

	return( Structure );
}



/****************************************************************
**	Routine: SecDbTransMapToArray
**	Returns: Allocated DT_VALUE with array of trans map
**	Action : Creates an Array value from given TransMap
****************************************************************/

DT_VALUE *SecDbTransMapToArray(
	SDB_TRANS_MAP	*MapList
)
{
	int
			Elem = 0;

	DT_VALUE
			Struct,
			*Array;

	SDB_TRANS_MAP
			*Map;


	if( !( Array = (DT_VALUE *) malloc( sizeof( *Array ))))
	{
		Err( ERR_MEMORY, "@" );
		return NULL;
	}

	DTM_ALLOC( Array, DtArray );
	for( Map = MapList; Map; Map = Map->Next )
	{
		DTM_ALLOC( &Struct, DtStructure );

		DtComponentSetNumber(	&Struct, "Date", DtDate, (double) Map->Date );
		DtComponentSetNumber(	&Struct, "Version", DtDouble, (double) Map->Version );
		DtComponentSetNumber(	&Struct, "Transaction ID", DtDouble, (double) Map->TransID );
		DtSubscriptSet( Array, Elem++, &Struct );
		DTM_FREE( &Struct );
	}
	return Array;
}

