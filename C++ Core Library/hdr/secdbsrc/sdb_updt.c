#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_updt.c,v 1.79 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SDB_UPDT.C	- Security Database update security functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_updt.c,v $
**	Revision 1.79  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.78  2010/04/08 16:55:39  ellist
**	Support constrained (sparse) indices
**
**	Revision 1.77  2002/04/17 15:23:00  mahonp
**	Fix rev 1.76 which was incorrectly built on top of 1.67 not 1.75
**
**	Revision 1.75  2001/07/31 19:54:06  vengrd
**	Revert to 1.73
**	
**	Revision 1.74  2001/07/31 19:50:01  vengrd
**	Take out .c_str() for prodver.
**	
**	Revision 1.73  2001/07/17 00:40:51  vengrd
**	Operation tags for self-desc streams and support for deletion
**	
**	Revision 1.72  2001/06/29 15:22:15  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.71  2001/06/20 13:48:24  vengrd
**	Compression of self-desc streams and tagging of classes as self-desc.
**	
**	Revision 1.70  2001/06/13 21:49:46  vengrd
**	Support self-desc on add as well as update.
**	
**	Revision 1.69  2001/06/13 21:35:43  vengrd
**	SecDb objects no longer know about self-desc streaming.  It is external to them.
**	
**	Revision 1.68  2001/06/12 18:56:51  vengrd
**	self-desc transaction support.
**	
**	Revision 1.67  2001/06/11 15:26:25  vengrd
**	First hooks for self desc streaming.  Disabled by default.
**	
**	Revision 1.66  2000/12/20 14:23:30  shahia
**	Bug Fix: SecDbUpdate should restore Sec.SdbDisk info when fails
**	
**	Revision 1.65  2000/10/26 10:22:29  goodfj
**	Prevent other instances of NULL pointer being passed to Err()
**	
**	Revision 1.64  2000/10/25 20:47:29  singhki
**	fix NULL pointer passed to Err
**	
**	Revision 1.63  2000/10/24 11:28:15  goodfj
**	Return better errormsg when db lacks DbUpdate
**	
**	Revision 1.62  2000/08/29 14:13:05  goodfj
**	Replaced SDB_DB_SEARCH PATH with SDB_DB_UNION
**	
**	Revision 1.61  2000/08/11 10:35:27  shahia
**	FIX Bug: mem leak in UpdateSecurity when it exceeds MaxMemSize
**	
**	Revision 1.60  2000/07/26 13:38:29  singhki
**	Fix semantics of DbUpdated
**	
**	Revision 1.59  2000/06/14 13:42:36  goodfj
**	Look for security to update in SecPtr->SourceDb
**	
**	Revision 1.58  2000/06/13 14:41:08  goodfj
**	Update in the correct database
**	
**	Revision 1.57  2000/06/12 14:38:59  goodfj
**	Do SecDbDefaultDbUpdateGet properly
**	
**	Revision 1.56  2000/06/12 13:13:06  goodfj
**	SecDbDefaultDbUpdateGet made public
**	
**	Revision 1.55  2000/06/12 12:02:41  goodfj
**	Fix SecDbDbUpdateGet to work with ClassFilterUnion dbs
**	
**	Revision 1.54  2000/02/11 21:48:51  singhki
**	SDB_OBJECTs now point to the SourceDb from which they were loaded. It
**	is only different if a Union/Qualified/NonPhysical Db.
**	
**	We use the new Trans APIs to get the disk info and then only load them
**	in the top database, as opposed to loading them in every level where
**	they were found.
**	
**	Now we no longer need to reload/removefromdeadpool in all members of a
**	union/qualified Db, since the load only happened at the top. Much
**	better, also no bogus failures.
**	
**	Revision 1.53  2000/02/08 22:24:09  singhki
**	No more references to DbID for virtual databases. All virtual
**	databases have the same DbID. This removes the limit on the number of
**	virtual databases you can create in memory.
**	
**	Revision 1.52  1999/12/07 15:32:27  gribbg
**	Fix doc
**	
**	Revision 1.51  1999/12/06 13:50:34  goodfj
**	DbUpdate is now private. New api to access it (for the time-being)
**	
**	Revision 1.50  1999/09/01 15:29:33  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.49  1999/08/23 15:29:02  singhki
**	Death to SDB_DB_ID in Index API
**	
**	Revision 1.48  1999/08/07 01:40:12  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.47  1999/07/15 16:01:02  singhki
**	remove nasty linked list of open dbs, use DatabaseIDHash and OpenCount instead.
**	
**	Revision 1.46  1998/12/11 20:14:41  singhki
**	BugFix: set transaction DbID to DbUpdate->DbID instead of virtual search path Db's DbID
**	
**	Revision 1.45  1998/12/08 21:23:50  vengrd
**	Put DbIdUpdated check back into update check.
**	
**	Revision 1.44  1998/12/04 20:25:50  vengrd
**	Support for links and bug fixes for slang user areas in subdbs.
**	
**	Revision 1.43  1998/10/14 18:20:54  singhki
**	New search paths
**	
**	Revision 1.42.2.1  1998/08/28 23:34:08  singhki
**	search paths, index merge working, some problems with ~casts
**	
**	Revision 1.42  1998/07/09 02:32:41  nauntm
**	casts and fixes to quiet compiler
**	
**	Revision 1.41  1998/06/17 22:55:50  gribbg
**	BugFix: Change %d to %s in error message
**	
**	Revision 1.40  1998/05/27 06:19:31  gribbg
**	Use DateToday()
**	
**	Revision 1.39  1998/05/13 19:58:41  brownb
**	SecDbTransMimic returns TransId
**	
**	Revision 1.38  1998/01/02 21:33:13  rubenb
**	added GSCVSID
**	
**	Revision 1.37  1997/11/25 14:28:09  lundek
**	LastTransaction set by TransCommit
**	
**	Revision 1.36  1997/11/03 21:52:31  lundek
**	Limit object stream size by class (default: 32711)
**	
**	Revision 1.35  1997/11/03 16:42:09  lundek
**	Fix secsyncing of huge objects (move paging-intercept from SecDbTransactionAdd level to SecDbAdd/Update/Delete/Rename level).
**	
**	Revision 1.34  1997/10/29 20:44:00  lundek
**	casts
**	
**	Revision 1.33  1997/08/15 17:17:03  lundek
**	LastTransaction in SDB_DISK
**	
**	Revision 1.32  1997/08/14 21:46:54  lundek
**	SecDbIsNew: in lieu of deprecated practice of testing TimeCreated
**
**	Revision 1.31  1997/07/24 19:25:07  gribbg
**	BugFix: Handle objects with 0 length streams when a new InStream value
**	        is added.
**	
**	Revision 1.30  1996/12/23 03:06:27  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<time.h>
#include	<gmt.h>
#include	<hash.h>
#include	<date.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<sectrans.h>
#include	<secdrv.h>
#include	<sdb_int.h>
#include	<secdb/SdbSelfDesc.h>

CC_USING_NAMESPACE( Gs );

static SDB_DB
		*SecDbDefaultClassFilterDbUpdateGet( SDB_DB *pDb ),
		*SecDbDbUpdateGetByClass( SDB_DB *pDb, SDB_SEC_TYPE ClassId );


/****************************************************************
**	Routine: SecDbUpdate
**	Returns: TRUE	- Security updated without error
**			 FALSE	- Error updating the security
**	Summary: Update an existing security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbUpdate"

int SecDbUpdate(
	SDB_OBJECT	*SecPtr		// Security to update
)
{
	SDB_STORED_OBJECT
			OldSec;

	SDB_DB	*pDb,
			*DbUpdated;

	SDB_M_DATA
			SdbMsgData;


	SDB_INDEX_POINTER
			*IndexPtr		= NULL;

	int		Result			= TRUE,
			FreeData		= TRUE,
			AllowTradeUpdate= TRUE;
	
	SDB_TRANS_PART
			TransPart;

	SDB_TRANS_TYPE
			TransType;

	OldSec.Mem = NULL;
	SdbMsgData.Update.Stream = NULL;

/*
**	Determine which database should be updated
*/

	pDb = SecPtr->Db;
	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, ARGCHECK_FUNCTION_NAME "( %s, %s ) @", SecPtr->SecData.Name, pDb->FullDbName.c_str() );


/*
**  We'd like to turn off updates to certain types of trades under
**	some circumstances.  Check the variable SECDB_PRODUCTION_TRADE_ENTRY != FALSE
*/

	AllowTradeUpdate = !stricmp( SecDbConfigurationGet( "SECDB_PRODUCTION_TRADE_ENTRY", NULL, NULL, "TRUE" ), "TRUE" );
	if( !AllowTradeUpdate )
		if( SecDbIsClassProtected( SecPtr->SecData.Type ))
			return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": Updates of trades or market data securities are not allowed in current environment" );

/*
**	The value-cache has to be wiped out to insure integrity
*/

	// FIX -- why?  SecDbRemoveCachedValue( SecPtr, NULL );


/*
**	The object gets told to update first, in case it has to
**	change the memory pointer or update some underlying database.
**	If all goes well, then the security master updates.
*/

	DbUpdated = SecDbDbUpdateGet( pDb, SecPtr->SecData.Type );
	if( pDb->IsVirtual() )
	{
		TransPart.Data.Begin.DbID = DbUpdated->DbID;
		TransType =
				TransPart.Data.Begin.TransType = SecPtr->SourceDb != DbUpdated ? SDB_TRAN_INSERT : SDB_TRAN_UPDATE;
	}
	else
	{
		TransPart.Data.Begin.DbID			= pDb->DbID;
		TransType =
				TransPart.Data.Begin.TransType = SDB_TRAN_UPDATE;
	}
	strcpy( TransPart.Data.Begin.SecName, SecPtr->SecData.Name );
	TransPart.Data.Begin.SecType		= SecPtr->SecData.Type;
	TransPart.Data.Begin.SourceTransID	= 0;
	if( !( Result = SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL )))
		goto CLEANUP;

	if( !SecDbTransProtectedGet( pDb, SecPtr->SecData.Name, &OldSec, FALSE, pDb ))
		goto ERROR_ABORT;
		
	if(		OldSec.SdbDisk.DateCreated != SecPtr->SecData.DateCreated ||
			OldSec.SdbDisk.TimeUpdated != SecPtr->SecData.TimeUpdated ||
		 	OldSec.SdbDisk.DbIDUpdated != SecPtr->SecData.DbIDUpdated ||
			OldSec.SdbDisk.UpdateCount != SecPtr->SecData.UpdateCount )
	{
		Err( SDB_ERR_OBJECT_MODIFIED, ARGCHECK_FUNCTION_NAME ": @" );
		goto ERROR_ABORT;

	}

	// Save the OldSdbDisk before sending the UPDATE message
	TransPart.Data.Op.OldSdbDisk	= SecPtr->SecData;

	SdbMsgData.Update.CurObject			= OldSec;
	SdbMsgData.Update.NothingChanged	= FALSE;
	M_SETUP_VERSION( &SdbMsgData.Update.Version, SecPtr, pDb );
	SdbMsgData.Update.Stream = SecDbBStreamCreateDefault();
	if( !(SDB_MESSAGE( SecPtr, SDB_MSG_UPDATE, &SdbMsgData )))
		goto ERROR_ABORT;
	
	if( !SecDbDiskInfoSetVersion( &SecPtr->SecData, &SdbMsgData.Update.Version ))
		goto ERROR_ABORT;

	// If nothing has changed, don't bother with the update
	if( SdbMsgData.Update.NothingChanged )
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		goto CLEANUP;
	}

	SecPtr->SecData.TimeUpdated 	= GMTGetTime( );
	SecPtr->SecData.DbIDUpdated 	= DbUpdated->DbID;
	SecPtr->SecData.UpdateCount++;

	TransPart.Data.Op.StoredObject.MemSize	= BStreamDataUsed( SdbMsgData.Update.Stream );
	TransPart.Data.Op.StoredObject.Mem		= SdbMsgData.Update.Stream->DataStart;

	if( TransPart.Data.Op.StoredObject.MemSize > SecPtr->Class->MaxStreamSize )
	{
		Err( ERR_OVERFLOW, ARGCHECK_FUNCTION_NAME ": Object too large, %ld bytes in stream; max %ld", TransPart.Data.Op.StoredObject.MemSize, SecPtr->Class->MaxStreamSize );
		// FIX: fix mem leak when error occurs
		SdbMsgData.Update.Stream->FreeData = TRUE;
		goto ERROR_ABORT;
	}
	if( !SecDbHugeUpdate(
			pDb,
			&TransPart.Data.Op.OldSdbDisk,
			&SecPtr->SecData,
			&TransPart.Data.Op.StoredObject.MemSize,
			&TransPart.Data.Op.StoredObject.Mem ))
		goto ERROR_ABORT;

	// Update the main database table
	TransPart.Data.Op.StoredObject.SdbDisk	= SecPtr->SecData;
	TransPart.Data.Op.SecPtr				= SecDbIncrementReference( SecPtr );
	if( !SecDbTransactionAdd( pDb, TransType, &TransPart, NULL ))
		goto ERROR_ABORT;
	FreeData = FALSE;	// Now owned by transaction, so don't free it
	
	// Update the indices
	for( IndexPtr = SecPtr->Class->IndexList; IndexPtr; IndexPtr = IndexPtr->Next )
		if( IndexPtr->Index->pDb == pDb )
			if( !SecDbIndexUpdateIfMatch( IndexPtr->Index, SecPtr ))
				goto ERROR_ABORT;

	// Put in the transaction part for the self-describing stream.

	if( SecDbSelfDescribingStreams() && SecPtr->Class->SelfDescStream )
		if( !SecDbSelfDescribe( pDb, SecPtr, GS_SELF_DESC_UPDATE ) )
			goto ERROR_ABORT;

	// Commit the transaction and clean up
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		Result = ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecPtr->SecData.Name );

	if( TransType == SDB_TRAN_INSERT )
	{
		// SourceDb has changed
		// !!!!FIX!!!!- this is not transactionalized. If this is backed out, we end up with the
		// wrong SourceDb which could be very bad
		SecPtr->SourceDb = DbUpdated;
	}

CLEANUP:

	SecDbBStreamDestroy( SdbMsgData.Update.Stream, 			FreeData );
	free( OldSec.Mem );

	// Bug fix, if update fails, change back the , in memory values
	// Note : the equality of these fields are a pre condition for the update and checked above
	if (!Result)
	{
		SecPtr->SecData.TimeUpdated = OldSec.SdbDisk.TimeUpdated;
  	 	SecPtr->SecData.DbIDUpdated = OldSec.SdbDisk.DbIDUpdated;
   		SecPtr->SecData.UpdateCount = OldSec.SdbDisk.UpdateCount;
	}
	return Result;


/*
**	Error cleanup logic
*/

ERROR_ABORT:
	// FIX - We can get here without cleaning up streams properly.

	SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
	Result = ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecPtr->SecData.Name );
	goto CLEANUP;
}



/****************************************************************
**	Routine: SecDbUpdateIncremental
**	Returns: TRUE	- Security updated without error
**			 FALSE	- Error updating the security
**	Summary: Incrementally update an existing security
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbUpdateIncremental"

int SecDbUpdateIncremental(
	SDB_OBJECT		*SecPtr,	// Security to update
	unsigned int	MemSize,	// Size of memory/msg to send
	void			*Mem		// Pointer to memory
)
{
	SDB_DB	*pDb,
			*pDbUpdate;

	SDB_INDEX_POINTER
			*IndexPtr		= NULL;

	int		Result			= TRUE;
	
	SDB_TRANS_PART
			TransPart;


/*
**	Determine which database should be updated
*/

	pDb = SecPtr->Db;
	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, ARGCHECK_FUNCTION_NAME "( %s, %s ) @", SecPtr->SecData.Name, pDb->FullDbName.c_str() );


/*
**	The value-cache has to be wiped out to insure integrity
*/

	// FIX -- why? SecDbRemoveCachedValue( SecPtr, NULL );


	
/*
**	Begin the transaction
*/

	strcpy( TransPart.Data.Begin.SecName, SecPtr->SecData.Name );
	TransPart.Data.Begin.TransType		= SDB_TRAN_INCREMENTAL;
	TransPart.Data.Begin.SecType		= SecPtr->SecData.Type;
	if( !( pDbUpdate = SecDbDbUpdateGet( pDb, SecPtr->SecData.Type ) ) )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get update db" );
	TransPart.Data.Begin.DbID			= pDbUpdate->DbID;
	TransPart.Data.Begin.SourceTransID	= 0;
	if( !( Result = SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL )))
		return ErrMore( ARGCHECK_FUNCTION_NAME );

		
/*
**	Send an update message to the object.
*/

	// Save OldSdbDisk before sending message
	TransPart.Data.Incremental.OldSdbDisk	= SecPtr->SecData;

	if( !(SDB_MESSAGE( SecPtr, SDB_MSG_INCREMENTAL, NULL )))
		goto ERROR_ABORT;

/*
**	Fix SdbDisk data
*/

	if( SecDbIsNew( SecPtr ))
	{
		SecPtr->SecData.DateCreated = (unsigned short) DateToday();
		SecPtr->SecData.UpdateCount = 0;
	}
	else
		SecPtr->SecData.UpdateCount++;
	SecPtr->SecData.TimeUpdated = GMTGetTime( );
	SecPtr->SecData.DbIDUpdated = pDbUpdate->DbID;


/*
**	Incremental update added to transaction
*/

	TransPart.Data.Incremental.Direction	= 1;
	TransPart.Data.Incremental.SdbDisk		= SecPtr->SecData;
	TransPart.Data.Incremental.SecPtr		= SecDbIncrementReference( SecPtr );
	if( !( TransPart.Data.Incremental.MsgMem = malloc( MemSize )))
	{
		Err( ERR_MEMORY, "@" );
		goto ERROR_ABORT;
	}
	TransPart.Data.Incremental.MsgMemSize = MemSize;
	memcpy( TransPart.Data.Incremental.MsgMem, Mem, MemSize );
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_INCREMENTAL, &TransPart, NULL ))
	{
		free( TransPart.Data.Incremental.MsgMem );
		goto ERROR_ABORT;
	}
   
	
/*
**	Update the indices
*/

	for( IndexPtr = SecPtr->Class->IndexList; IndexPtr; IndexPtr = IndexPtr->Next )
		if( IndexPtr->Index->pDb == pDb )
			if( !SecDbIndexIncrementalIfMatch( IndexPtr->Index, SecPtr ))
				goto ERROR_ABORT;


/*
**	Commit the transaction
*/

	if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		Result = ErrMore( "Update( %s )", SecPtr->SecData.Name );

	return( Result );


/*
**	Error cleanup logic
*/

ERROR_ABORT:
	SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
	Result = ErrMore( ARGCHECK_FUNCTION_NAME "( %s )", SecPtr->SecData.Name );
	return( Result );
}


/****************************************************************
**	Routine: SecDbDefaultClassFilterDbUpdateGet
**	Returns: Pointer to Db
**	Summary: Database in which objects would be updated, by default
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDefaultClassFilterDbUpdateGet"

SDB_DB *SecDbDefaultClassFilterDbUpdateGet(
	SDB_DB			*pDb
)
{
	SDB_DB	*pDbUpdate;


	if( !pDb )
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb arg missing" );

	if( SDB_DB_CLASSFILTERUNION == pDb->DbOperator )	// Disallow this for the moment at least
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Nested %s dbs not allowed", pDb->FullDbName.c_str(), SecDbDbOperatorName( pDb->DbOperator ) );

	if( SDB_DB_UNION == pDb->DbOperator ) 	// Look in Left, then Right
    {
        if( ( pDbUpdate = SecDbDefaultClassFilterDbUpdateGet( pDb->Left ) ) )
			return pDbUpdate;
		else if( ERR_NOT_FOUND != ErrNum )		// Carry on if it's just a NOT_FOUND error
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ): Failed to find default update db in Left db", pDb->FullDbName.c_str() );
		else if( ( pDbUpdate = SecDbDefaultClassFilterDbUpdateGet( pDb->Right ) ) )
			return pDbUpdate;
		else
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s ): Failed to find default update db in Left or Right db", pDb->FullDbName.c_str() );
    }
	if( SDB_DB_CLASSFILTER != pDb->DbOperator )
		return pDb->DbUpdateNowPrivate;
	
	return (SDB_DB *) ErrN( ERR_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ): Db is not default update db", pDb->FullDbName.c_str() );
}


/****************************************************************
**	Routine: SecDbDbUpdateGetByClass
**	Returns: Pointer to Db
**	Summary: Database in which object of this class would be updated
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDbUpdateGetByClass"

SDB_DB *SecDbDbUpdateGetByClass(
	SDB_DB			*pDb,
   	SDB_SEC_TYPE	ClassId
)
{
	SDB_DB	*pDbUpdate;


	if( !pDb )
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb arg missing" );

	if( SDB_DB_CLASSFILTERUNION == pDb->DbOperator )	// Disallow this for the moment at least
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, %s ): Nested %s dbs not allowed", pDb->FullDbName.c_str(), SecDbClassNameFromType( ClassId ), SecDbDbOperatorName( pDb->DbOperator ) );

	if( SDB_DB_UNION == pDb->DbOperator )	// Look in Left, then Right
    {
        if( ( pDbUpdate = SecDbDbUpdateGetByClass( pDb->Left, ClassId ) ) )
			return pDbUpdate;
		else if( ERR_NOT_FOUND != ErrNum )
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to find update db in Left db", pDb->FullDbName.c_str(), SecDbClassNameFromType( ClassId ) );
		else if( ( pDbUpdate = SecDbDbUpdateGetByClass( pDb->Right, ClassId ) ) )
			return pDbUpdate;
		else
			return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to find update db in Left or Right db", pDb->FullDbName.c_str(), SecDbClassNameFromType( ClassId ) );
    }

	if( SDB_DB_CLASSFILTER == pDb->DbOperator )
		if( HashLookup( pDb->FilteredClasses, (HASH_KEY) ClassId ) )
			return pDb->DbUpdateNowPrivate;
	
	return (SDB_DB *) ErrN( ERR_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s, %s ): Db is not update db", pDb->FullDbName.c_str(), SecDbClassNameFromType( ClassId ) );
}


/****************************************************************
**	Routine: SecDbDefaultDbUpdateGet
**	Returns: Pointer to Db
**	Summary: Database in which object would be updated
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDefaultDbUpdateGet"

SDB_DB *SecDbDefaultDbUpdateGet(
	SDB_DB	*pDb
)
{
	SDB_DB	*pDbUpdate;


	if( !( pDbUpdate = SecDbDbUpdateGet( pDb, (SDB_SEC_TYPE) 0 ) ) )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME );

	return pDbUpdate;
}


/****************************************************************
**	Routine: SecDbDbUpdateGet
**	Returns: Pointer to Db
**	Summary: Database in which object would be updated
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDbUpdateGet"

SDB_DB *SecDbDbUpdateGet(
	SDB_DB			*pDb,
   	SDB_SEC_TYPE	ClassId
)
{
	SDB_DB	*pDbUpdate;

	const char
			*ClassName;


	if( !pDb )
		return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": pDb arg missing" );

/*
**	Use ClassName variable to avoid passing NULL into Err()
*/
	if( !ClassId )
		ClassName = "<No Class>";
	else if( !( ClassName = SecDbClassNameFromType( ClassId ) ) )
		ClassName = "<Unknown Class>";

	switch( pDb->DbOperator )
	{
/*
**	If pDb is a ClassFilterUnion, pDb->Left is a searchpath of ClassFilter dbs, plus
**	a default db.
*/
		case SDB_DB_CLASSFILTERUNION:
			if( (SDB_SEC_TYPE) 0 == ClassId )
			{
				if( ( pDbUpdate = SecDbDefaultClassFilterDbUpdateGet( pDb->Left ) ) )
					return pDbUpdate;
				return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to find default update db", pDb->FullDbName.c_str(), ClassName );
			}
			else
			{
				if( ( pDbUpdate = SecDbDbUpdateGetByClass( pDb->Left, ClassId ) ) )
					return pDbUpdate;

				if( ERR_NOT_FOUND != ErrNum )
					return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to find update db", pDb->FullDbName.c_str(), ClassName );

				if( ( pDbUpdate = SecDbDefaultDbUpdateGet( pDb->Left ) ) )
					return pDbUpdate;

				return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME "( %s, %s ): Failed to find default update db", pDb->FullDbName.c_str(), ClassName );
			}

		default:
		{
			if( !pDb->DbUpdateNowPrivate )
				return (SDB_DB *) ErrN( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s, %s ): No update db for %s", pDb->FullDbName.c_str(), ClassName, pDb->FullDbName.c_str() );

			return pDb->DbUpdateNowPrivate;
		}
	}
}


/****************************************************************
**	Routine: SecDbIsClassProtected
**	Returns: True or False
**	Summary: Under certain circumstances we prevent adds/updates
**           to various security types
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbIsClassProtected"

bool SecDbIsClassProtected(
   	SDB_SEC_TYPE	ClassId
)
{
	// This process should also check a predefined container for
	// additional updates

   	switch( ClassId )
		{
			case  5000:	// Trade
			case  1610: // Market Curve
			case  1611: // Market USD
			case  1612: // Market Forward Curve
			case  1613: // Market Volatility Curve
			case  1616: // Market Prefix
			case  1620: // Market Model Parameters
			case  1621: // Market Correlation Curve
			case  3206: // Container
			case  3664: // Market IR Parameters
			case  3851: // Market Euro Curve
			case  4711: // Futures Strip Data
			case  4712: // Location Matrix Data
			case  4719: // Volatility Matrix Data
			case  4735: // Commod Matrix Data
			case  4742: // Commod Correlation Data
			case  4746: // Commod Correlation Info
			case  4748: // Commod TimeCorr Info
			case  4922: // Location Corr Data
			case  4928: // Commod Vol Model Market Data
			case  5150: // Gnf Model Specs Generic
			case  5151: // Gnf Corr Specs Generic
			case  5160: // Gnf Model Object Map
			case  5161: // Gnf Corr Object Map
			case  5177: // Gnf Calibr Setup Data
			case 10002: // Elec Location Matrix Data
			case 10009: // Elec Volatility Data
			case 10107: // Elec Price Matrix Data
			case 10130: // Compressed Container
			case 10238: // Elec Temperature Matrix Data
			case 10239: // Elec Volatility Marks					  
			case 10270: // Elec Greek Data
			case 10600: // Elec Correlation Data
			case 10605: // Elec Load Forecast Parameters
			case 10610: // Elec Unit Matrix Data
			case 10616: // Elec MCC Location Matrix Datacase
				return true;
				break;
			default:
				return false;
		}
}

