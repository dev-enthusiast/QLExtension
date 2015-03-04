#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_del.c,v 1.42 2013/10/03 00:48:30 khodod Exp $"
/****************************************************************
**
**	SDB_DEL.C	- Security Database delete security function
**
**	Copyright 1994,1995,1996 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_del.c,v $
**	Revision 1.42  2013/10/03 00:48:30  khodod
**	Adding validation to the SecDb API to prevent buffer overflows and crashes
**	for evaluating strings with embedded percent signs.
**	AWR: #315292
**
**	Revision 1.41  2010/04/08 16:55:18  ellist
**	Support constrained (sparse) indices
**
**	Revision 1.40  2002/04/16 22:03:19  mahonp
**	Add hooks to prevent add/update/deletes on termserv
**
**	Revision 1.39  2001/07/31 20:03:39  vengrd
**	revert to 1.37.
**	
**	Revision 1.38  2001/07/31 19:59:47  vengrd
**	Got rid of .c_str() for prodver.
**	
**	Revision 1.37  2001/07/17 00:40:50  vengrd
**	Operation tags for self-desc streams and support for deletion
**	
**	Revision 1.36  2001/06/29 15:22:13  singhki
**	use string in SDB_DB instead of char* so they actually get freed properly.
**	Free Dbs which participate in diddle scopes when the diddle scope is detached
**	
**	Revision 1.35  2001/04/18 13:11:20  lundek
**	Clarify use and meaning of NoLoadFlag (including changing the name to Forcefully)
**	
**	Revision 1.34  2000/12/19 23:36:26  simpsk
**	More cleanups.  Also added loop for autochildren for the dbchanged case.
**	
**	Revision 1.33  1999/09/01 15:29:28  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.32  1999/08/23 15:29:01  singhki
**	Death to SDB_DB_ID in Index API
**	
**	Revision 1.31  1999/08/10 19:58:08  singhki
**	Register None and Null as diddle scopes in order to preserve behaviour
**	
**	Revision 1.30  1999/08/07 01:40:08  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.29  1999/02/08 08:29:34  schlae
**	Check for name buffer overflow on delete.
**	
**	Revision 1.28  1998/11/16 22:59:56  singhki
**	DiddleColors: Merge from branch
**
**	Revision 1.27.2.1  1998/10/30 00:03:16  singhki
**	DiddleColors: Initial revision
**
**	Revision 1.27  1998/10/22 21:10:28  singhki
**	SecDbNew & SecDbDeleteByName now support qualified sec names
**
**	Revision 1.26  1998/10/14 18:20:52  singhki
**	New search paths
**
**	Revision 1.25.4.1  1998/08/28 23:33:55  singhki
**	search paths, index merge working, some problems with ~casts
**
**	Revision 1.25.2.1  1998/07/08 03:08:11  singhki
**	checkpoint
**
**	Revision 1.25  1998/05/13 19:56:21  brownb
**	SecDbTransMimic returns TransId
**
**	Revision 1.24  1998/01/02 21:32:52  rubenb
**	added GSCVSID
**
**	Revision 1.23  1997/11/12 13:35:34  lundek
**	Force commits of no-load deletes
**
**	Revision 1.22  1997/11/03 16:42:06  lundek
**	Fix secsyncing of huge objects (move paging-intercept from SecDbTransactionAdd level to SecDbAdd/Update/Delete/Rename level).
**
**	Revision 1.21  1997/02/24 23:30:44  lundek
**	SDB_IGNORE_PATH
**
**	Revision 1.20  1996/12/23 03:06:14  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**
**	Revision 1.19  1996/09/18 02:20:44  lundek
**	Clean up SecDb trace stuff
**
**	Revision 1.18  1995/01/27 18:49:40  GRIBBG
**	Const changes
**	Remove old ...From() functions (mostly)
**
**	   Rev 1.17   09 Jan 1995 12:36:00   DUBNOM
**	Changed database search mechanism
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<time.h>
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


/****************************************************************
**	Routine: SecDbDeleteByName
**	Returns: TRUE	- Security deleted without error
**			 FALSE	- Couldn't delete security
**	Action : Delete a security in the security database by name
**				A. Get the current record from the database, 
**				   fail if it doesn't exist
**				B. Load the object and send it the SDB_MSG_DELETE, 
**				   fail if unloadable or msg rejected
**				C. At commit-time (on server), check that the
**				   object still exists and that the version (i.e.,
**				   update count) matches, if not, fail the whole
**				   transaction
**
**			The Forcefully flag (formerly known as NoLoad) turns
**			off behavior (B) and (C) above.
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDeleteByName"

int SecDbDeleteByName(
	const char		*DelSecName,		// Name of security to delete
	SDB_DB			*pDb,				// Database to delete from
	int				Forcefully			// TRUE - 1) do not load or send DELETE msg, 2) set ignore errors on this part of transaction
)
{
	SDB_INDEX_POINTER
			*IndexPtr;

	SDB_CLASS_INFO
			*ClassInfo;

	SDB_OBJECT
			*SecPtr;

	bool	Result = false;

	SDB_TRANS_PART
			TransPartBegin,
			TransPart;

	char	NameBuffer[ SDB_FULL_SEC_NAME_SIZE ],
			*SecName;

	int		AllowDelete;

	SDB_DB_ID
			DbID;


	if( strlen( DelSecName ) >= SDB_FULL_SEC_NAME_SIZE )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - DelSecName exceeds %d characters", SDB_FULL_SEC_NAME_SIZE );

/*
**	Strip blanks off the end of the security name
*/

	strcpy( NameBuffer, DelSecName );
	SecName = NameBuffer;
	if( !ResolveDbPath( SecName, pDb, SecName ))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": ( %s )", DelSecName );

	int     Len = strlen( SecName );
	for( ; --Len >= 0 && SecName[ Len ] == ' '; ) /* nothing */;
	SecName[ Len + 1 ] = '\0';
	if( Len == -1 )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ":  @ - Name is blank" );

	if( Len >= int( SDB_SEC_NAME_SIZE - 1 ) )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - Name %s exceeds %d characters", SecName, SDB_SEC_NAME_SIZE );


/*
**	Get the database information
*/

	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_DATABASE_INVALID, "SecDbDeleteByName( %s ) - @", SecName );
	DbID = pDb->DbID;

	// Make sure the application is allowed to do this
	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, "Delete( %s, %s ) @", SecName, pDb->FullDbName.c_str() );


/*
**  We'd like to turn off deletes incircumstances.  
**  Check the variable SECDB_PRODUCTION_TRADE_ENTRY != FALSE
*/

	AllowDelete = !stricmp( SecDbConfigurationGet( "SECDB_PRODUCTION_TRADE_ENTRY", NULL, NULL, "TRUE" ), "TRUE" );
	if( !AllowDelete )
		return Err( ERR_UNSUPPORTED_OPERATION, ARGCHECK_FUNCTION_NAME ": Deletes are not allowed in current environment" );

/*
**	Don't load, just delete
*/

	if( Forcefully )
	{
		// Even with NoLoad we have to get the disk info in order to determine the class
		// We need the class in order to determine the index records to discard, if any
		if( !SecDbGetDiskInfo( &TransPart.Data.Delete.SdbDisk, SecName, pDb, SDB_IGNORE_PATH ))
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );

		if( !(ClassInfo = SecDbClassInfoFromType( TransPart.Data.Delete.SdbDisk.Type )))
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );

		strcpy( TransPartBegin.Data.Begin.SecName, SecName );
		TransPartBegin.Data.Begin.TransType		= SDB_TRAN_DELETE;
		TransPartBegin.Data.Begin.SecType		= 0;
		TransPartBegin.Data.Begin.DbID			= DbID;
		TransPartBegin.Data.Begin.SourceTransID	= 0;
		if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPartBegin, NULL ))
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );

		// Attempt to dispose of huge pages also
		(void) SecDbHugeUpdate( pDb, &TransPart.Data.Delete.SdbDisk, NULL, NULL, NULL );

		if( !SecDbTransactionAdd( pDb, SDB_TRAN_DELETE, &TransPart, NULL ))
		{
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
		}
	}


/*
**	Get the record & delete it
*/

	else if( ( SecPtr = SecDbGetByName( SecName, pDb, SDB_IGNORE_PATH )) )
	{
		ClassInfo = SecPtr->Class;

		strcpy( TransPartBegin.Data.Begin.SecName, SecName );
		TransPartBegin.Data.Begin.TransType	= SDB_TRAN_DELETE;
		TransPartBegin.Data.Begin.SecType	= SecPtr->SecData.Type;
		TransPartBegin.Data.Begin.DbID		= DbID;
		TransPartBegin.Data.Begin.SourceTransID	= 0;
		Result = SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPartBegin, NULL );
		if( Result )
		{
			Result = SecDbHugeUpdate(
					pDb,
					&SecPtr->SecData,
					NULL,
					NULL,
					NULL );

			memcpy( &TransPart.Data.Delete.SdbDisk, &SecPtr->SecData, sizeof( SDB_DISK ));
			TransPart.Data.Delete.IgnoreErrors = FALSE;

			if( 	!Result ||
					!SDB_MESSAGE( SecPtr, SDB_MSG_DELETE, NULL ) ||
					!SecDbTransactionAdd( pDb, SDB_TRAN_DELETE, &TransPart, NULL ))
			{
				SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
				Result = FALSE;
			}
		}

		// Put in the transaction part for the self-describing stream.

		if( SecDbSelfDescribingStreams() && SecPtr->Class->SelfDescStream )
			if( !SecDbSelfDescribe( pDb, SecPtr, GS_SELF_DESC_DELETE ) )
				return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
				

		SecDbFree( SecPtr );
		if( !Result )
			return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
	}
	else
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );


/*
**	Delete the security from any related indices
*/

	for( IndexPtr = ClassInfo->IndexList; IndexPtr; IndexPtr = IndexPtr->Next )
		if( IndexPtr->Index->pDb == pDb )
			if( !SecDbIndexDeleteIfMatch( IndexPtr->Index, SecName ) && !Forcefully )
			{
				SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
				return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %s )", SecName, pDb->FullDbName.c_str() );
			}

	if( Forcefully )
	{
		if( !SecDbTransCommitForce( pDb ))
			return FALSE;
	}
	else if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		return FALSE;
	SecDbRemoveFromDeadPool( pDb, 0, SecName );
	return TRUE;
}


