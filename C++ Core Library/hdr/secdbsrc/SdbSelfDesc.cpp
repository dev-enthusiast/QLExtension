#define GSCVSID "$Header: /home/cvs/src/secdb/src/SdbSelfDesc.cpp,v 1.14 2013/10/07 19:33:17 khodod Exp $"
/****************************************************************
**
**	SdbSelfDesc.cpp	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: SdbSelfDesc.cpp,v $
**	Revision 1.14  2013/10/07 19:33:17  khodod
**	Address obvious cases of format-string vulnerabilities.
**	#315536
**
**	Revision 1.13  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.12  2011/05/16 22:02:01  khodod
**	Placate gcc warning.
**	AWR: #177463
**
**	Revision 1.11  2001/08/02 14:40:26  vengrd
**	Allow SDS writes in prod, but still require an env var to enable.
**	
**	Revision 1.10  2001/07/31 13:24:26  vengrd
**	Grab an unused bit to tag VTs for sniffer indexing.
**	
**	Revision 1.9  2001/07/17 00:40:50  vengrd
**	Operation tags for self-desc streams and support for deletion
**	
**	Revision 1.8  2001/07/16 22:43:48  vengrd
**	Leave out Security Name for contained securities, which tend to have useless temp names.
**	
**	Revision 1.7  2001/07/10 00:06:32  vengrd
**	Put Security Name in the self desc stream.
**	
**	Revision 1.6  2001/07/03 19:28:03  vengrd
**	Class name missing from Err()
**	
**	Revision 1.5  2001/07/03 17:53:50  vengrd
**	Use DtValueToStream to stream a structure.
**	
**	Revision 1.4  2001/06/21 18:46:16  vengrd
**	Be a little safer by turing self-desc off in production environments.
**	
**	Revision 1.3  2001/06/21 14:21:13  vengrd
**	Put in Seucrity Type and skip ~Obsolete *
**	
**	Revision 1.2  2001/06/20 13:48:23  vengrd
**	Compression of self-desc streams and tagging of classes as self-desc.
**	
**	Revision 1.1  2001/06/13 21:49:45  vengrd
**	Support self-desc on add as well as update.
**	
**
****************************************************************/

#define BUILDING_SECDB
#include <portable.h>
#include <secdb/SdbSelfDesc.h>
#include <sectrans.h>
#include <secobj.h>
#include <zlib.h>

CC_BEGIN_NAMESPACE( Gs )



static int SdbSelfDescStreamWriteVersion(
	SDB_OBJECT *SecPtr,
	BSTREAM	   *BStream,
	GS_SELF_DESC_VERSION SelfDescVersion
);



/****************************************************************
**	Routine: SdbSelfDescDefaultVersion
**	Returns: The default version to write.
**	Action : Return the default version to write.  Could look
**           at e.g. an env var to decide.
****************************************************************/

static GS_SELF_DESC_VERSION SdbSelfDescDefaultVersion(
)
{
	return GS_SELF_DESC_VERSION_GZIP;
}



/****************************************************************
**	Routine: SdbSelfDescDefaultCompressedVersion
**	Returns: The default version to write inside compression.
**	Action : Return the default version to write.  Could look
**           at e.g. an env var to decide.
****************************************************************/

static GS_SELF_DESC_VERSION SdbSelfDescDefaultCompressedVersion(
)
{
	return GS_SELF_DESC_VERSION_STRUCTURE;
}



/****************************************************************
**	Routine: SdbSelfDescInstreamsToStructure
**	Returns: Success
**	Action : Put the instreams into a structure.
****************************************************************/

static int SdbSelfDescInstreamsToStructure(
	SDB_OBJECT *SecPtr,
	DT_VALUE *InstreamStruct,
	bool IncludeSecurityName
)
{
	DTM_ALLOC( InstreamStruct, DtStructure );

	SDB_VALUE_TYPE_INFO
			*ValueTable = SecPtr->Class->ValueTable,
			*Value;


	for( Value = ValueTable; Value->Name; Value++ )
		if(    ( ( Value->ValueFlags & ( SDB_IN_STREAM | SDB_SELF_DESC_INDEXED ) ) && ( !StrIBegins( Value->Name, "~Obsolete" ) ) )    
			|| ( !stricmp( Value->Name, "Security Type" ) )
			|| ( IncludeSecurityName && ( !stricmp( Value->Name, "Security Name" ) ) ) )
		{
			DT_VALUE
					*ValueTypeCopy,
					*ValueTypeValue;

			if( !( ValueTypeValue = SecDbGetValue( SecPtr, Value->ValueType ) ) )
			{
				DTM_FREE( InstreamStruct );

				return ErrMore( "SdbSelfDescInstreamsToStructure(): Unable to get instream '%s'.", Value->Name );
			}

			ValueTypeCopy = (DT_VALUE *)malloc( sizeof( DT_VALUE ) );

			if( DtSecurity == ValueTypeValue->DataType )
			{
				// Special case for security.  Nest it.

				if( !SdbSelfDescInstreamsToStructure( (SDB_OBJECT *)DT_VALUE_TO_POINTER( ValueTypeValue ), ValueTypeCopy, false ) )
				{
					free( ValueTypeCopy );
					return ErrMore( "SdbSelfDescInstreamsToStructure(): Recursion failed for instream security '%s'.", Value->Name );
				}
			}
			else if( DtSecurity == ValueTypeValue->DataType )
			{				
				DT_VALUE
						SecListAsArray;

				DTM_ALLOC( &SecListAsArray, DtArray );

				DTM_TO( &SecListAsArray, ValueTypeValue );

				size_t
						NumElems = ( (DT_ARRAY *)DT_VALUE_TO_POINTER( &SecListAsArray ) )->Size,
						Elem;

				DT_ARRAY
						*ArrayOfStructs = DtArrayAlloc( NumElems ),
						*ArrayOfSecs    = (DT_ARRAY *)DT_VALUE_TO_POINTER( &SecListAsArray );


				for( Elem = 0; Elem < NumElems; ++Elem )
					if( !SdbSelfDescInstreamsToStructure( (SDB_OBJECT *)DT_VALUE_TO_POINTER( &( ArrayOfSecs->Element[    Elem ] ) ), &( ArrayOfStructs->Element[ Elem ] ), false ) )
					{
						free( ValueTypeCopy );	

						for( size_t FreeElem = 0; FreeElem < Elem; ++FreeElem )
							DTM_FREE( &( ArrayOfStructs->Element[ FreeElem ] ) );

						free( ArrayOfStructs );

						DTM_FREE( &SecListAsArray );
						return ErrMore( "SdbSelfDescInstreamsToStructure(): Recursion failed for instream security list '%s'.", Value->Name );
					}
				
				DTM_FREE( &SecListAsArray );

				DT_POINTER_TO_VALUE( ValueTypeCopy, ArrayOfStructs, DtArray );
			}
			else
				DTM_ASSIGN( ValueTypeCopy, ValueTypeValue );

			DT_COMPONENT_GIVE( InstreamStruct, Value->Name, ValueTypeCopy );
		}

	return TRUE;
}



/****************************************************************
**	Routine: SdbSelfDescStreamWriteAsStructure
**	Returns: TRUE/Err
**	Action : Write a Self-Describing stream to the
**			 the given bstream by putting all the 
**			 instreams and specially tagged VTs into
**			 a structure and streaming that.
****************************************************************/

static int SdbSelfDescStreamWriteAsStructure(
	SDB_OBJECT *SecPtr,
	BSTREAM	   *BStream
)
{
	// This initial approach just packs all the instreams into a
	// structure.  It is a placeholder until such time as we decide on
	// a better approach to streaming in a self-describing way.

	DT_VALUE
			InstreamStruct;

	if( !SdbSelfDescInstreamsToStructure( SecPtr, &InstreamStruct, true ) )
		return ErrMore( "SdbSelfDescStreamWriteAsStructure(): Unable to construct structure of instreams." );		

	if( !DtValueToStream( BStream, DtStructure, &InstreamStruct ) )
	{
		DTM_FREE( &InstreamStruct );
				
		return ErrMore( "SdbSelfDescStreamWriteAsStructure(): Unable to stream structure of instreams." );
	}

	DTM_FREE( &InstreamStruct );

	return TRUE;
}



/****************************************************************
**	Routine: SdbSelfDescStreamWriteGZip
**	Returns: success
**	Action : 
****************************************************************/

static int SdbSelfDescStreamWriteGZip(
	SDB_OBJECT *SecPtr,
	BSTREAM	   *BStream
)
{
	// Stream to an intermediate stream using the default method for 
	// use inside a compressed stream.

	BSTREAM
			*BStreamUncompressed = NULL;

	if( !(BStreamUncompressed = SecDbBStreamCreateDefault() ) )
		return ErrMore( "SecDbSelfDescStreamWriteGZip(): Unable to allocate stream." );	

	if( !SdbSelfDescStreamWriteVersion( SecPtr, BStreamUncompressed, SdbSelfDescDefaultCompressedVersion() ) )
		return ErrMore( "SecDbSelfDescStreamWriteGZip(): Unable to write uncompressed stream." );	
	
	// Hold onto the memory from call to call so we don't have to 
	// alloc every time.

	static unsigned char
			*Memory = NULL;

	static size_t
			MemorySize;

	// Alloc the mem the first time.

	if( !Memory )
		if( !( Memory = (unsigned char *)malloc( MemorySize = 0x10000 ) ) )
			return Err( ERR_MEMORY, "GsSelfDescStreamGzipToStruct(): Unable to allocate intermediate memory." );		

	// Allocate as much memory as the size of the stream we are going
	// to try to compress.

	if( MemorySize >= (size_t) BStreamDataUsed( BStreamUncompressed ) )
		if( !( Memory = (unsigned char *)realloc( Memory, MemorySize = BStreamDataUsed( BStreamUncompressed ) ) ) )
			return Err( ERR_MEMORY, "GsSelfDescStreamGzipToStruct(): Unable to reallocate intermediate memory." );		

	z_stream 
			ZStream;

	ZStream.zalloc = (alloc_func) Z_NULL;
	ZStream.zfree  = (free_func)  Z_NULL;
	ZStream.opaque = (voidpf) Z_NULL;
	ZStream.msg    = (char*) "";
    
	ZStream.next_in  = (Byte *)( BStreamUncompressed->DataStart );
	ZStream.avail_in = (uInt) BStreamDataUsed( BStreamUncompressed );

	if( Z_OK != deflateInit( &ZStream, Z_DEFAULT_COMPRESSION ) )
		return Err( ERR_UNKNOWN, "GsSelfDescStreamGzipToStruct(): GZIP deflateInit() failed: %s", ZStream.msg );

    ZStream.next_out  = (Byte *) Memory;
    ZStream.avail_out = (uInt) MemorySize;

	if( Z_STREAM_END == deflate( &ZStream, Z_FINISH ) )
	{
		if( Z_OK != deflateEnd( &ZStream ) )
			return Err( ERR_UNKNOWN, "GsSelfDescStreamGzipToStruct(): GZIP deflateEnd() failed: %s", ZStream.msg );

		BStreamPutInt32( BStream, ZStream.total_out );
		
		BStreamPutMemory( BStream, Memory, ZStream.total_out );
	}
	else
	{
		// We did not actually compress it, so just write the uncompressed version.

		deflateEnd( &ZStream );

		BStreamPutInt32( BStream, -1 );

		BStreamPutMemory( BStream, BStreamUncompressed->DataStart, BStreamDataUsed( BStreamUncompressed ) );
	}

	return TRUE;
}



/****************************************************************
**	Routine: SdbSelfDescStreamWriteVersion
**	Returns: success
**	Action : Write with a specific version
****************************************************************/

static int SdbSelfDescStreamWriteVersion(
	SDB_OBJECT *SecPtr,
	BSTREAM	   *BStream,
	GS_SELF_DESC_VERSION SelfDescVersion
)
{
	// Spit out the stream version, then let the appropriate handler
	// actually write the data.

	BStreamPutUInt16( BStream, SelfDescVersion );

	switch( SelfDescVersion )
	{
		case GS_SELF_DESC_VERSION_STRUCTURE:
			return SdbSelfDescStreamWriteAsStructure( SecPtr, BStream );

		case GS_SELF_DESC_VERSION_GZIP:
			return SdbSelfDescStreamWriteGZip( SecPtr, BStream );
			
	}	

	return Err( ERR_INVALID_ARGUMENTS, "Unknown stream type %d.\n", (int)SelfDescVersion );
}



/****************************************************************
**	Routine: SdbSelfDescStreamWrite
**	Returns: TRUE/Err
**	Action : Write a Self-Describing stream to the
**			 the given bstream.
****************************************************************/

int SdbSelfDescStreamWrite(
	SDB_OBJECT *SecPtr,
	BSTREAM	   *BStream
)
{	
	GS_SELF_DESC_VERSION 
			SelfDescVersion = SdbSelfDescDefaultVersion();

	return SdbSelfDescStreamWriteVersion( SecPtr, BStream, SelfDescVersion );
}



/****************************************************************
**	Routine: SecDbSelfDescribe
**	Returns: success/err
**	Action : Add a transaction part with a self desc stream
**           to the current transaction.  Called when adding
**           or updating a security.
****************************************************************/

int SecDbSelfDescribe(
	SDB_DB	   				*pDb,
	SDB_OBJECT 				*SecPtr,
	GS_SELF_DESC_OPERATION	StreamOperation
)
{
	SDB_TRANS_PART
			TransPartSelfDesc;

	BSTREAM
			*BStream = NULL;

	void
			*SelfDescData;

	// Make sure this object's class wants to participate.

	if( !SecPtr->Class->SelfDescStream )
		return Err( ERR_INVALID_ARGUMENTS, "SecDbSelfDescribe(): Class '%s' does not support self-desc streaming.", SecPtr->Class->Name );

	if( !(BStream = SecDbBStreamCreateDefault() ) )
		return ErrMore( "SecDbSelfDescribe(): Unable to allocate stream." );

	BStreamPutUInt16( BStream, StreamOperation );

	if( !SdbSelfDescStreamWrite( SecPtr, BStream ) )
	{
		SecDbBStreamDestroy( BStream, TRUE );
		return ErrMore( "SecDbSelfDescribe(): Failed to write self description to stream." );		
	}

	TransPartSelfDesc.Data.SelfDesc.DataSize = BStreamDataUsed( BStream );

	if( !( SelfDescData = malloc( TransPartSelfDesc.Data.SelfDesc.DataSize ) ) )
	{
		SecDbBStreamDestroy( BStream, TRUE );
		return Err( ERR_MEMORY, "Unable to allocate self description of %u bytes.", (unsigned) TransPartSelfDesc.Data.SelfDesc.DataSize );
	}

	memcpy( SelfDescData, BStream->DataStart, TransPartSelfDesc.Data.SelfDesc.DataSize );

	TransPartSelfDesc.Data.SelfDesc.Data = SelfDescData;

	if( !SecDbTransactionAdd( pDb, SDB_TRAN_SELF_DESC_DATA, &TransPartSelfDesc, NULL ) )
	{
		free( SelfDescData );
		SecDbBStreamDestroy( BStream, TRUE );
		return ErrMore( "SecDbSelfDescribe(): Unable to add transaction part." );
	}

	SecDbBStreamDestroy( BStream, TRUE );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbSelfDescribingStreams
**	Returns: Are self-describing streams enabled?
**	Action : Check with a config var.
****************************************************************/

bool SecDbSelfDescribingStreams(
)
{
	// Be a little safer by turing self-desc off in production
	// environments.

	static bool 
			SelfDescribing = ( NULL != SecDbConfigurationGet( "SECDB_SELF_DESCRIBING_STREAMS_ENABLED", NULL, NULL, NULL ) );

	return SelfDescribing;
}



CC_END_NAMESPACE
