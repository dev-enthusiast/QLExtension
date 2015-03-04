#define GSCVSID "$Header: /home/cvs/src/secdb/src/of_secdb.c,v 1.44 2010/04/08 16:55:04 ellist Exp $"
/****************************************************************
**
**	OF_SECDB.C	- Security database global object function
**
**	$Log: of_secdb.c,v $
**	Revision 1.44  2010/04/08 16:55:04  ellist
**	Support constrained (sparse) indices
**
**	Revision 1.43  2001/06/20 21:20:09  singhki
**	fix Current Date such that it does not cache its value in the global variable SecDbCurrentDate. Arghhh.
**	This lets it work correctly in diddle scopes.
**
**	Revision 1.42  1999/11/01 22:41:51  singhki
**	death to all link warnings
**	
**	Revision 1.41  1999/10/01 16:44:10  singhki
**	Use BuildingIndex to determine if we can add classes to an index
**	
**	Revision 1.40  1999/09/01 15:29:26  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.39  1999/08/23 15:29:01  singhki
**	Death to SDB_DB_ID in Index API
**	
**	Revision 1.38  1999/08/07 01:40:06  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.37  1999/07/15 16:01:00  singhki
**	remove nasty linked list of open dbs, use DatabaseIDHash and OpenCount instead.
**	
**	Revision 1.36  1999/06/21 03:04:26  gribbg
**	More const correctness
**	
**	Revision 1.35  1999/04/23 00:03:30  nauntm
**	Unused arg/uninitialized variable extermination
**	
**	Revision 1.34  1999/02/09 20:03:02  singhki
**	Added SecDb Base Class which all GOBs inherit from instead of hacks for special VTs. Also careful to not create nodes for special VTs on load message
**	
**	Revision 1.33  1998/05/27 06:19:26  gribbg
**	Use DateToday()
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<hash.h>
#include	<kool_ade.h>
#include	<bstream.h>
#include	<date.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<secdrv.h>
#include	<sdb_int.h>


// FIX - BSTREAM OPERATIONS USE MEMORY INSTEAD OF STRING OPERATIONS
// FIX - BUILD MEMORY BLOCK MAKES NASTY ASSUMPTION OF STREAM SIZE

EXPORT_C_SECDB int DbObjFuncSecDb(
	SDB_OBJECT		*SecPtr,		// Pointer to security to operate on
	int				Msg,			// Type of operation
	SDB_M_DATA		*MsgData		// Data used for operation
);

/****************************************************************
**	Routine:	ValueFuncCurrentDate
**	Returns:	TRUE or FALSE indicating success or failure, resp.
**
**	Value Type:	Current Date( "Security Database" )
**
**	Action:
****************************************************************/

static int ValueFuncCurrentDate(
	SDB_OBJECT			*, //SecPtr,
	int					Msg,
	SDB_M_DATA			*MsgData,
	SDB_VALUE_TYPE_INFO	* //ValueTypeInfo
)
{
	switch( Msg )
	{
		case SDB_MSG_GET_VALUE:
			MsgData->GetValue.Value->Data.Number = (double) SecDbCurrentDate;
			return TRUE;

		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Date()" );

		default:
			return TRUE;
	}
}



/****************************************************************
**	Routine:	ValueFuncPricingDate
**	Returns:	TRUE or FALSE indicating success or failure, resp.
**
**	Value Type:	Pricing Date( "Security Database" )
**
**	Action:
****************************************************************/

static int ValueFuncPricingDate(
	SDB_OBJECT			*, //SecPtr,
	int					Msg,
	SDB_M_DATA			*MsgData,
	SDB_VALUE_TYPE_INFO	*  //ValueTypeInfo
)
{
	switch( Msg )
	{
		case SDB_MSG_GET_VALUE:
			MsgData->GetValue.Value->Data.Number = (double) SecDbCurrentDate;
			return TRUE;

		case SDB_MSG_DESCRIPTION:
			MsgData->Description = strdup( "Date()" );
		default:
			return TRUE;
	}
}



/****************************************************************
**	Routine: BuildMemoryBlock
**	Returns: TRUE or Err()
**	Action : Store the index information within the 'Security Database'
**			 record of the database
****************************************************************/

static int BuildMemoryBlock(
	SDB_DB		*pDb,
	BSTREAM		*Output
)
{
	SDB_INDEX
			*Index;

	HASH_ENTRY_PTR
			Ptr;

	char	*ValueName,
			*DataName;

	const char
			*ClassName;

	short	NumIndices	= 0,
			ClassNum,
			PartNum,
			ConstraintNum;


/*
**	Calculate the number of indices
*/

	HASH_FOR( Ptr, SecDbIndexHash )
	{
		Index = (SDB_INDEX *) HashValue( Ptr );
		if( Index->pDb == pDb && !Index->DescriptorOnly )
			NumIndices++;
	}


/*
**	Convert the table of indices into a stream
*/

	BStreamPutInt16( Output, NumIndices );

	HASH_FOR( Ptr, SecDbIndexHash )
	{
		Index = (SDB_INDEX *) HashValue( Ptr );
		if( Index->pDb != pDb || Index->DescriptorOnly )
			continue;

		// Index name and database names
		BStreamPutMemory( Output, Index->Name, sizeof( Index->Name ));

		// We have taken over the first 2 bytes of the old 32-byte obsolete
		// portion for use as index-level flags
		BStreamPutInt16( Output, Index->Flags );
		BStreamPutMemory( Output, Index->Obsolete, sizeof( Index->Obsolete ) - sizeof( int16_t ));

		// Total byte width
		BStreamPutInt16( Output, Index->TotalByteWidth );

		// Parts
		BStreamPutInt16( Output, Index->PartCount );
		for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		{
			ValueName	= Index->Parts[ PartNum ].ValueType->Name;
			DataName	= Index->Parts[ PartNum ].ValueType->DataType->Name;

			BStreamPutMemory(	Output, ValueName, SDB_VALUE_NAME_SIZE );
			BStreamPutMemory(	Output, DataName,  DT_DATA_TYPE_NAME_SIZE );
			BStreamPutInt16(	Output, Index->Parts[ PartNum ].Flags );
			BStreamPutInt16(	Output, Index->Parts[ PartNum ].ByteWidth );
		}

		// Classes
		BStreamPutInt16( Output, Index->ClassCount );
		for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
		{
			if( !(ClassName = SecDbClassNameFromType( Index->Classes[ ClassNum ] )) )
				return ErrMore( "Class not found for index '%s'\nAdd class to sectypes.dat and clear index", Index->Name );
			BStreamPutMemory( Output, ClassName, SDB_CLASS_NAME_SIZE );
		}

		// Constraints, if present
		if( Index->Flags & SDB_INDEX_CONSTRAINED )
		{
			BStreamPutInt16( Output, Index->ConstraintCount );
			for( ConstraintNum = 0; ConstraintNum < Index->ConstraintCount; ConstraintNum++ )
			{
				BStreamPutMemory(	Output, Index->Constraints[ ConstraintNum ].VtName,			SDB_VALUE_NAME_SIZE );
				BStreamPutMemory(	Output, Index->Constraints[ ConstraintNum ].DataType->Name,	DT_DATA_TYPE_NAME_SIZE );

				BStreamPutInt16( Output, Index->Constraints[ ConstraintNum ].ConstraintType );

				if( Index->Constraints[ ConstraintNum ].DataType == DtDouble )
					BStreamPutDouble( Output, Index->Constraints[ ConstraintNum ].Value.NumericValue );
				else if( Index->Constraints[ ConstraintNum ].DataType == DtString )
					BStreamPutMemory( Output, Index->Constraints[ ConstraintNum ].Value.StringValue,	SDB_CONSTRAINT_SIZE );
				else
				{
					DataName = Index->Constraints[ ConstraintNum ].DataType->Name;	
					Err( ERR_INVALID_NUMBER, "Cannot stream %s value", DataName );
					return ErrMore( "Streaming index '%s' description", Index->Name );
				}	
			}
		}
	}

	return TRUE;
}



/****************************************************************
**	Routine: ParseMemoryBlock
**	Returns: TRUE	- Memory block parsed without error
**			 FALSE	- Error parsing memory block
**	Action : Load the index information from the 'Security Database'
**			 record of the database
****************************************************************/

static int ParseMemoryBlock(
	SDB_DB		*pDb,
	BSTREAM		*Stream
)
{
	SDB_CLASS_INFO
			*ClassInfo;

	SDB_INDEX_POINTER
			**IndexPtrPtr,
			*DeleteIndexPtr,
			*IndexPtr;

	SDB_INDEX
			*Index,
			*OldIndex;

	DT_DATA_TYPE
			DataType;

	char	DataName[ DT_DATA_TYPE_NAME_SIZE ],
			ValueName[ SDB_VALUE_NAME_SIZE ],
			ClassName[ SDB_CLASS_NAME_SIZE ];

	int		NumIndices,
			IndexNumber,
			PartNum,
			ClassNum,
			ConstraintNum;


/*
**	Close all open indices
*/

	SecDbIndexClose( NULL, pDb, FALSE );


/*
**	Fill in the hash tables of indices
*/

	NumIndices = BStreamGetInt16( Stream );

	for( IndexNumber = 0; IndexNumber < NumIndices; IndexNumber++ )
	{
		Index = (SDB_INDEX *) calloc( 1, sizeof( SDB_INDEX ));

		Index->pDb				= pDb;

		// Index name and database name
		BStreamGetMemory( Stream, Index->Name, sizeof( Index->Name ));
	
		// We have taken over the first 2 bytes of the old 32-byte obsolete
		// portion for use as index-level flags
		Index->Flags = BStreamGetInt16( Stream );
		BStreamGetMemory( Stream, Index->Obsolete, sizeof( Index->Obsolete ) - sizeof( int16_t ));

		// Total byte width
		Index->TotalByteWidth = BStreamGetInt16( Stream );

		// Get the list of parts
		Index->PartCount = BStreamGetInt16( Stream );
		for( PartNum = 0; PartNum < Index->PartCount; PartNum++ )
		{
			BStreamGetMemory( Stream, ValueName, SDB_VALUE_NAME_SIZE );
			BStreamGetMemory( Stream, DataName,  DT_DATA_TYPE_NAME_SIZE );
			Index->Parts[ PartNum ].Flags		= BStreamGetInt16( Stream );
			Index->Parts[ PartNum ].ByteWidth	= BStreamGetInt16( Stream );

			DataType = DtFromName( DataName );
			Index->Parts[ PartNum ].ValueType	= SecDbValueTypeFromName( ValueName, DataType );
			if( !Index->Parts[ PartNum ].ValueType )
				return ErrMore( "Loading index '%s' description", Index->Name );
		}

		// Get the list of classes
		Index->ClassCount = BStreamGetInt16( Stream );
		if( Index->ClassCount )
		{
			Index->Classes = (SDB_SEC_TYPE *) calloc( Index->ClassCount, sizeof( SDB_SEC_TYPE ));
			for( ClassNum = 0; ClassNum < Index->ClassCount; ClassNum++ )
			{
				BStreamGetMemory( Stream, ClassName, SDB_CLASS_NAME_SIZE );

				ClassInfo = (SDB_CLASS_INFO *) HashLookup( SecDbSecDescHash, ClassName );
				if( ClassInfo )
				{
					Index->Classes[ ClassNum ] = ClassInfo->Type;

					IndexPtr = (SDB_INDEX_POINTER *) calloc( 1, sizeof( SDB_INDEX_POINTER ));
					IndexPtr->Index = Index;
					IndexPtr->Next	= ClassInfo->IndexList;
					ClassInfo->IndexList = IndexPtr;
				}
				else
				{
					Err( SDB_ERR_CLASS_NOT_FOUND, "%s, @", ClassName );
					return ErrMore( "Loading index '%s' description", Index->Name );
				}
			}
		}

		// Get the list of constraints, if present
		if( Index->Flags & SDB_INDEX_CONSTRAINED )
		{
			Index->ConstraintCount = BStreamGetInt16( Stream );
			for( ConstraintNum = 0; ConstraintNum < Index->ConstraintCount; ConstraintNum++ )
			{
				BStreamGetMemory( Stream, Index->Constraints[ ConstraintNum ].VtName,	SDB_VALUE_NAME_SIZE );
				BStreamGetMemory( Stream, DataName,										DT_DATA_TYPE_NAME_SIZE );

				Index->Constraints[ ConstraintNum ].ConstraintType	= BStreamGetInt16( Stream );

				DataType = DtFromName( DataName );
				Index->Constraints[ ConstraintNum ].DataType		= DataType;

				if( DataType == DtDouble )
					Index->Constraints[ ConstraintNum ].Value.NumericValue	= BStreamGetDouble( Stream );
				else if( DataType == DtString )
					BStreamGetMemory( Stream, Index->Constraints[ ConstraintNum ].Value.StringValue,	SDB_CONSTRAINT_SIZE );
				else
				{
					Err( ERR_INVALID_NUMBER, "Invalid data type '%s'", DataType->Name );
					return ErrMore( "Loading index '%s' description", Index->Name );
				};
			}
		}

		// If the index already exists, cleanup and replace
		OldIndex = (SDB_INDEX *) HashLookup( SecDbIndexHash, Index );
		if( OldIndex )
		{
			for( ; OldIndex->ClassCount > 0; OldIndex->ClassCount-- )
			{
				ClassInfo = (SDB_CLASS_INFO *) HashLookup( SecDbSecTypeHash, (HASH_KEY) (long) OldIndex->Classes[ OldIndex->ClassCount - 1 ] );
				if( ClassInfo )
					for( IndexPtrPtr = &ClassInfo->IndexList; *IndexPtrPtr; IndexPtrPtr = &(*IndexPtrPtr)->Next )
						if( (*IndexPtrPtr)->Index == Index )
						{
							DeleteIndexPtr = *IndexPtrPtr;
							*IndexPtrPtr = (*IndexPtrPtr)->Next;
							free( DeleteIndexPtr );
							break;
						}
			}
			free( OldIndex->Classes );
			*OldIndex = *Index;
			free( Index );
		}
		else
			HashInsert( SecDbIndexHash, Index, Index );
	}

	return TRUE;
}



/****************************************************************
**	Routine: DbObjFuncSecDb
**	Returns: TRUE	- Operation was successful
**			 FALSE	- Operation failed
**	Action : Security database's own security object.  This one-
**			 instance security is used to contain global information
**			 such as todays date, etc...
****************************************************************/

int DbObjFuncSecDb(
	SDB_OBJECT		*SecPtr,		// Pointer to security to operate on
	int				Msg,			// Type of operation
	SDB_M_DATA		*MsgData		// Data used for operation
)
{
/*
**	Define Value Tables
*/

	static SDB_VALUE_TYPE_INFO
			ValueTable[] =
			{	 
				{ "Current Date",	&DtDate,	ValueFuncCurrentDate,	NULL,	SDB_EXPORT | SDB_STATIC	| SDB_SET_RETAINED },
				{ "Pricing Date",	&DtDate,	ValueFuncPricingDate,	NULL,	SDB_EXPORT | SDB_EXTERNAL },
				{ NULL }
			};


	switch( Msg )
	{
		case SDB_MSG_START:
			MsgData->Start.Class->ValueTable = ValueTable;
			if( !SecDbValueTypeRegisterClassOld( MsgData->Start.Class, ValueTable ))
				return ErrMore( "Registering Class Value Types for Class %s", MsgData->Start.Class->Name );
			return TRUE;

		case SDB_MSG_LOAD:
			// Get the current date
			SecDbCurrentDate = DateToday();
			return ParseMemoryBlock( SecPtr->Db, MsgData->Load.Stream );

		case SDB_MSG_MEM:
			return BuildMemoryBlock( SecPtr->Db, MsgData->Mem.Stream );

		case SDB_MSG_UPDATE:
			return BuildMemoryBlock( SecPtr->Db, MsgData->Update.Stream );

		case SDB_MSG_DELETE:
			return Err( ERR_UNSUPPORTED_OPERATION, "@: Cannot delete 'Security Database'" );
	}

	return SecDbDefaultObjFunc( SecPtr, Msg, MsgData );
}
