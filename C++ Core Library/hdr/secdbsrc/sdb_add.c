#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_add.c,v 1.67 2010/04/08 16:55:12 ellist Exp $"
/****************************************************************
**
**	SDB_ADD.C	- Security Database add security function
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_add.c,v $
**	Revision 1.67  2010/04/08 16:55:12  ellist
**	Support constrained (sparse) indices
**
**	Revision 1.66  2008/07/02 16:13:26  khodod
**	New way to copy db source id in SecurityCopy and seccopy.
**	iBug: #56731
**
**	Revision 1.64  2002/04/16 22:03:19  mahonp
**	Add hooks to prevent add/update/deletes on termserv
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<time.h>
#include	<ctype.h>
#include	<gmt.h>
#include	<hash.h>
#include	<date.h>
#include	<kool_ade.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<sectrans.h>
#include	<secdrv.h>
#include	<sdb_int.h>
#include	<sectrace.h>
#include	<secdb/SdbSelfDesc.h>

CC_USING_NAMESPACE( Gs );


/****************************************************************
**	Routine: SecDbAdd
**	Returns: TRUE	- Security added without error
**			 FALSE	- Security couldn't be added
**	Summary: Add a new security to a database
****************************************************************/

int SecDbAdd(
	SDB_OBJECT		*SecPtr				// Pointer to security to add
)
{
	return SecDbAddWithFlags( SecPtr, FALSE, FALSE );
}


/****************************************************************
**	Routine: SecDbAddWithFlags
**	Returns: TRUE	- Security added without error
**			 FALSE	- Security couldn't be added
**	Summary: Add a new security to a database
**	Action : Used by SecDbAdd, SecDbCopy, and inside the subdb
**			 driver to add wrapped securities.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbAddWithFlags"

int SecDbAddWithFlags(
	SDB_OBJECT			*SecPtr,			// Pointer to security to add
	int					NoSideEffects, 		// Use SDB_MSG_MEM message
	int					AddFlags            
)
{
	SDB_DB	*pDb,
			*pDbUpdate;

	SDB_INDEX_POINTER
			*IndexPtr	= NULL;

	SDB_M_DATA
			SdbMsgData;

	SDB_TRANS_PART
			TransPart;

	int		Status = TRUE,
			MsgStatus,
			AllowTradeAdd;

/*
**	Make sure name is legal
*/

	if( SecPtr->SecData.Name[0] == '\0' )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - SecName = \"\"" );
	if( StrBegins( SecPtr->SecData.Name, SecDbNewSecurityPrefix ))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): @ - Must rename first", SecPtr->SecData.Name );
	if( strlen( SecPtr->SecData.Name ) != strcspn( SecPtr->SecData.Name, SDB_FORBIDDEN_CHARS ) )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ) @\nNew name cannot contain any of \"\\t\\n" SDB_PRINTABLE_FORBIDDEN_CHARS "\"", SecPtr->SecData.Name );
	{
		const char
				*p;


		for( p = SecPtr->SecData.Name; *p; p++ )
			if( !isprint((unsigned char)*p ) )
				return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ) @\nNew name cannot contain unprintable characters", SecPtr->SecData.Name );
	}

	if( ( AddFlags & SDB_ADD_PRESERVE_SRC_DB_ID ) && !( AddFlags & SDB_ADD_PRESERVE_DISK_INFO ) )
	{
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %d ) @\nInconsistent AddFlags: preserve disk info must be set", AddFlags );
	}

/*
**	Determine which database will store the object
*/

	pDb = SecPtr->Db;
	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, ARGCHECK_FUNCTION_NAME "( %s, %s ) @", SecPtr->SecData.Name, pDb->FullDbName.c_str() );

/*
**  We'd like to turn off updates to certain types of trades under
**	some circumstances.  Check the variable SECDB_PRODUCTION_TRADE_ENTRY != FALSE
*/

	AllowTradeAdd = !stricmp( SecDbConfigurationGet( "SECDB_PRODUCTION_TRADE_ENTRY", NULL, NULL, "TRUE" ), "TRUE" );
	if( !AllowTradeAdd )
		if( SecDbIsClassProtected( SecPtr->SecData.Type ))
			return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": Adds of trades or market data securities are not allowed in current environment" );

	// check to see if class is unstorable
	static SDB_CLASS_INFO* un_storable_iface = SecDbClassInfoFromType( SecDbClassTypeFromName( "SecDb UnStorable" ) );

	if( un_storable_iface &&
		SecPtr->Class->ImplementedInterfaces->find( un_storable_iface ) != SecPtr->Class->ImplementedInterfaces->end() )
		return Err( ERR_UNSUPPORTED_OPERATION,
					ARGCHECK_FUNCTION_NAME "(%s, %s): the security is of class %s which cannot be written to the database because it implements the interface '%s'",
					SecPtr->SecData.Name, pDb->FullDbName.c_str(), SecPtr->Class->Name, un_storable_iface->Name
				  );

	if( SecPtr->Class->Temporary && ( pDb->DbAttributes & SDB_DB_ATTRIB_PROD ))
	{
		Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME "( %s, %s ) not allowed in a production database", SecPtr->SecData.Name, pDb->FullDbName.c_str() );
		ErrMore( "Class '%s' has been defined (or redefined) within this session", SecPtr->Class->Name );
		return( FALSE );
	}


/*
**	Begin the transaction
*/

	strcpy( TransPart.Data.Begin.SecName, SecPtr->SecData.Name );
	TransPart.Data.Begin.TransType		= SDB_TRAN_INSERT;
	TransPart.Data.Begin.SecType		= SecPtr->SecData.Type;
	pDbUpdate = SecDbDbUpdateGet( pDb, SecPtr->SecData.Type );
	if( !pDbUpdate )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get update db" );

	TransPart.Data.Begin.DbID			= pDbUpdate->DbID;
	TransPart.Data.Begin.SourceTransID	= 0;

	if( AddFlags & SDB_ADD_PRESERVE_SRC_DB_ID )
	{
		TransPart.Data.Begin.DbID = SecPtr->SecData.DbIDUpdated;
		TransPart.Data.Begin.SourceTransID = SecPtr->SecData.LastTransaction;
	}
	
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL ))
		return FALSE;


/*
**	Fix the disk data
*/

	TransPart.Data.Op.OldSdbDisk	= SecPtr->SecData;
	if( AddFlags & SDB_ADD_PRESERVE_DISK_INFO )
	{
		// If there are no values for Created/Updated make sure to fill
		// in some values for these fields.
		if( !SecPtr->SecData.DateCreated )
		{
			SecPtr->SecData.DateCreated = (unsigned short) DateToday();
			SecPtr->SecData.UpdateCount = 0L;
		}
		if( !SecPtr->SecData.TimeUpdated )
		{
			SecPtr->SecData.DbIDUpdated = pDbUpdate->DbID;
			SecPtr->SecData.TimeUpdated = GMTGetTime();
		}
	}
	else
	{
		SecPtr->SecData.DateCreated = (unsigned short) DateToday();
		SecPtr->SecData.TimeUpdated = GMTGetTime();
		SecPtr->SecData.DbIDUpdated = pDbUpdate->DbID;
		SecPtr->SecData.UpdateCount = 0L;
	}
	SecPtr->SecData.LastTransaction = -1;


/*
**	Send the 'ADD' message to the security
**	(Do not fail until INSERT part is in the transaction so backout will work.)
*/

	TransPart.Data.Op.StoredObject.MemSize = 0;
	TransPart.Data.Op.StoredObject.Mem	   = NULL;
	memset( &SdbMsgData, 0, sizeof( SdbMsgData ));
	if( NoSideEffects )
	{
		M_SETUP_VERSION( &SdbMsgData.Mem.Version, SecPtr, pDb );\
		SdbMsgData.Mem.Stream = SecDbBStreamCreateDefault();
		MsgStatus = SDB_MESSAGE( SecPtr, SDB_MSG_MEM, &SdbMsgData );
		if( MsgStatus )
			MsgStatus = SecDbDiskInfoSetVersion( &SecPtr->SecData, &SdbMsgData.Mem.Version );
		if( MsgStatus )
		{
			TransPart.Data.Op.StoredObject.MemSize = BStreamDataUsed( SdbMsgData.Mem.Stream );
			TransPart.Data.Op.StoredObject.Mem	   = SdbMsgData.Mem.Stream->DataStart;
		}
		// Free data unless it has been placed in the transaction
		SecDbBStreamDestroy( SdbMsgData.Mem.Stream, !MsgStatus );
	}
	else
	{
		M_SETUP_VERSION( &SdbMsgData.Add.Version, SecPtr, pDb );
		SdbMsgData.Add.Stream = SecDbBStreamCreateDefault();
		MsgStatus = SDB_MESSAGE( SecPtr, SDB_MSG_ADD, &SdbMsgData );
		if( MsgStatus )
			MsgStatus = SecDbDiskInfoSetVersion( &SecPtr->SecData, &SdbMsgData.Add.Version );
		if( MsgStatus )
		{
			TransPart.Data.Op.StoredObject.MemSize = BStreamDataUsed( SdbMsgData.Add.Stream );
			TransPart.Data.Op.StoredObject.Mem	   = SdbMsgData.Add.Stream->DataStart;
			if( TransPart.Data.Op.StoredObject.MemSize > SecPtr->Class->MaxStreamSize )
				Status = Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME ": Object too large, %ld bytes in stream; max %ld", TransPart.Data.Op.StoredObject.MemSize, SecPtr->Class->MaxStreamSize );
		}
		// Free data unless it has been placed in the transaction
		SecDbBStreamDestroy( SdbMsgData.Add.Stream, !MsgStatus );
	}
	if( !MsgStatus )
		Status = FALSE;


/*
**	Add to the security master database
*/

	if( Status && !SecDbHugeUpdate(
			pDb,
			&TransPart.Data.Op.OldSdbDisk,
			&SecPtr->SecData,
			&TransPart.Data.Op.StoredObject.MemSize,
			&TransPart.Data.Op.StoredObject.Mem ))
		Status = FALSE;	// So it gets backed out

	// SecPtr->SecData gets written to somewhere above, again.
	// So we must restore the DbIDUpdated.

	if( AddFlags & SDB_ADD_PRESERVE_SRC_DB_ID )
	{
		SecPtr->SecData.DbIDUpdated = TransPart.Data.Op.OldSdbDisk.DbIDUpdated;
	}
		
	TransPart.Data.Op.StoredObject.SdbDisk	= SecPtr->SecData;
	TransPart.Data.Op.SecPtr				= SecDbIncrementReference( SecPtr );
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_INSERT, &TransPart, NULL ))
	{
		free( TransPart.Data.Op.StoredObject.Mem );
		goto ERROR_ABORT;
	}
	if( !Status )
		goto ERROR_ABORT;


/*
**	Add the security to any related indices
*/

	for( IndexPtr = SecPtr->Class->IndexList; IndexPtr; IndexPtr = IndexPtr->Next )
		if( IndexPtr->Index->pDb == SecPtr->Db )
			if( !SecDbIndexAddIfMatch( IndexPtr->Index, SecPtr ))
				goto ERROR_ABORT;

	// Put in the transaction part for the self-describing stream.

	if( SecDbSelfDescribingStreams() && SecPtr->Class->SelfDescStream )
		if( !SecDbSelfDescribe( pDb, SecPtr, GS_SELF_DESC_ADD ) )
			goto ERROR_ABORT;

	if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecPtr->SecData.Name, pDb->FullDbName.c_str() );

	return( TRUE );



/*
**	Error cleanup logic
*/

ERROR_ABORT:
	SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
	return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecPtr->SecData.Name, pDb->FullDbName.c_str() );
}



/****************************************************************
**	Routine: SecDbSelfDescAdd
**	Returns: TRUE	- Security updated without error
**			 FALSE	- Error updating the security
**	Summary: Add a self-describing part in the transaction.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbSelfDescAdd"

int SecDbSelfDescAdd(
	SDB_OBJECT	*SecPtr		// Security to update
)
{
	// Make sure we are supposed to be doing this at all.

	if( !SecDbSelfDescribingStreams() || !SecPtr->Class->SelfDescStream )
		return TRUE;


	SDB_DB	
			*pDb = SecPtr->Db;

	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, ARGCHECK_FUNCTION_NAME "( %s, %s ) @", SecPtr->SecData.Name, pDb->FullDbName.c_str() );
	
	SDB_DB	
			*DbUpdated = SecDbDbUpdateGet( pDb, SecPtr->SecData.Type );

	SDB_TRANS_PART
			TransPart;

	// Begin the transaction

	if( pDb->IsVirtual() )
		TransPart.Data.Begin.DbID = DbUpdated->DbID;
	else
		TransPart.Data.Begin.DbID			= pDb->DbID;

	strcpy( TransPart.Data.Begin.SecName, SecPtr->SecData.Name );
	TransPart.Data.Begin.SecType		= SecPtr->SecData.Type;
	TransPart.Data.Begin.SourceTransID	= 0;
	TransPart.Data.Begin.TransType 		= SDB_TRAN_SELF_DESC_DATA;

	if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL ) )
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecPtr->SecData.Name );
	
	// Make sure we have the most up to date version of the sec, in case we are
	// inside an outer transaction that updated it.

	SDB_STORED_OBJECT
			OldSec;

	if( !SecDbTransProtectedGet( pDb, SecPtr->SecData.Name, &OldSec, FALSE, pDb ) )
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecPtr->SecData.Name );
	}

	// Put in the transaction part for the self-describing stream.

	if( !SecDbSelfDescribe( pDb, SecPtr, GS_SELF_DESC_ADD_OR_UPDATE ) )
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecPtr->SecData.Name );		
	}

	// Commit the transaction

	if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecPtr->SecData.Name );

	return TRUE;
}
