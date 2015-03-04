#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_ren.c,v 1.50 2010/04/08 16:55:27 ellist Exp $"
/****************************************************************
**
**	SDB_REN.C	- Security Database rename security function
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_ren.c,v $
**	Revision 1.50  2010/04/08 16:55:27  ellist
**	Support constrained (sparse) indices
**
**	Revision 1.49  2000/09/14 15:02:41  piriej
**	Clear err stack on successful rename
**
**	Revision 1.48  2000/07/17 23:34:47  goodfj
**	Invalidate Security Data on successful rename
**	
**	Revision 1.47  2000/04/24 11:04:24  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.46  2000/02/08 04:50:26  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.45  2000/01/08 01:04:47  hsuf
**	renamed CacheOnlyImpliedName to CacheOnlyInferredName
**	
**	Revision 1.44  2000/01/07 23:36:29  hsuf
**	SDB_MSG_RENAME has a new flag: CacheOnlyInferredName
**	
**	Revision 1.43  1999/09/07 15:02:40  singhki
**	oops, that was stupid!
**	
**	Revision 1.42  1999/09/01 15:29:32  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.41  1999/08/23 15:29:02  singhki
**	Death to SDB_DB_ID in Index API
**	
**	Revision 1.40  1999/08/07 01:40:11  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.39  1999/06/28 22:51:55  singhki
**	reject names >= SDB_SEC_NAME_SIZE in get
**	
**	Revision 1.38  1999/02/12 03:12:06  singhki
**	Fix DbSets of SecPtr's nodes after rename to a different database
**	
**	Revision 1.37  1998/11/16 22:59:59  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.36  1998/11/06 14:40:10  lundek
**	Minor change to make head revision prodverable without branch
**	
**	Revision 1.35  1998/11/03 15:27:34  lundek
**	BugFix: complain if name is too long/too short
**	
**	Revision 1.34  1998/10/30 19:27:33  singhki
**	BugFix: gracefully fail if can't attach to db
**	
**	Revision 1.33.2.1  1998/10/30 00:03:18  singhki
**	DiddleColors: Initial revision
**	
**	Revision 1.33  1998/10/29 21:57:27  singhki
**	BugFix: initialize NewName
**	
**	Revision 1.32  1998/10/29 21:24:53  singhki
**	Use NewNamePtr in rename message, SDB_M_DATA back down to 80 bytes
**	
**	Revision 1.31  1998/10/29 20:24:44  singhki
**	Support Db!Sec syntax in SecDbRename
**	
**	Revision 1.30  1998/10/14 18:20:53  singhki
**	New search paths
**	
**	Revision 1.29.4.1  1998/08/28 23:34:02  singhki
**	search paths, index merge working, some problems with ~casts
**	
**	Revision 1.29.2.1  1998/07/08 03:08:17  singhki
**	checkpoint
**	
**	Revision 1.29  1998/05/13 19:58:07  brownb
**	SecDbTransMimic returns TransId
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
#include	<gmt.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secindex.h>
#include	<secerror.h>
#include	<secdrv.h>
#include	<sectrans.h>
#include	<sdb_int.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<sdbdbset.h>



/****************************************************************
**	Routine: FixNodeDbSets
**	Returns: void
**	Action : Changes DbSets of all Nodes belonging to this SecPtr
**			 to now point to it's new Db
****************************************************************/

void FixNodeDbSets(
	SDB_OBJECT	*SecPtr,
	SDB_DB		*OldDb,
	SDB_DB		*NewDb
)
{
	SDB_ENUM_PTR
			EnumPtr;

	SDB_NODE
			*Node;

	DT_VALUE
			TmpValue;

	// Fix DbSet of Security Literal
	TmpValue.DataType = DtSecurity;
	TmpValue.Data.Pointer = SecPtr;
	// This shouldn't really ever fail, but just to be on the safe side
	if(( Node = SDB_NODE::FindLiteral( &TmpValue, OldDb )))
		Node->DbSetSet( NewDb->ParentDb->DbSingleton );

	// Now fix DbSet of all Terminal Nodes for the Security
	// The hope is that this will also fix the non terminals because they should eventually end up depending on the terminals
	for( Node = SecDbNodeEnumFirst( SecPtr, SDB_NODE_ENUM_TERMINALS, &EnumPtr ); Node; Node = SecDbNodeEnumNext( EnumPtr ))
	{
		Node->DbSetSet( SDB_DBSET_PLUS( Node->DbSetGet(), NewDb ));
		Node->RemoveDbFromParents( OldDb );
	}
	SecDbNodeEnumClose( EnumPtr );
}


/****************************************************************
**	Routine: SecDbRename
**	Returns: TRUE	- Security renamed without error
**			 FALSE	- Error renaming security
**	Summary: Change the name of a security
****************************************************************/

int SecDbRename(
	SDB_OBJECT	*SecPtr,	// Security to change the name of
	const char	*NewName	// New name, NULL ask the security for a name
)
{
	return SecDbRenameWithFlags( SecPtr, NewName, FALSE );
}


/****************************************************************
**	Routine: SecDbRenameWithFlags
**	Returns: TRUE	- Security renamed without error
**			 FALSE	- Error renaming security
**	Summary: Change the name of a security
**           This version of SecdbRename() allows caller specify
**           Cache-Only Implied Name.  Under this flag, we will 
**           ask the security object to compute its implied name
**           without consulting physical database, avoiding any IO operations.
**           You should use this flag, only if you are sure that 
**           the security is never persisted to database.
****************************************************************/

int SecDbRenameWithFlags(
	SDB_OBJECT	*SecPtr,				// Security to change the name of
	const char	*NewName,				// New name, NULL ask the security for a name
	int			CacheOnlyInferredName	// Compute implied name without talking to disk
)
{
	SDB_DB	*pDb,
			*pOldDb,
			*pNewDb;

	SDB_M_DATA
			SdbMsgData;

	SDB_INDEX_POINTER
			*IndexPtr;

	SDB_TRANS_PART
			TransPart;
			
	SDB_STORED_OBJECT
			OldObject;

	char	OldName[ SDB_SEC_NAME_SIZE ],
			*FullName,
			*Name;

	int		Len,
			Transaction;
			

/*
**	Get the database information
*/

	pDb = DbToPhysicalDb( SecPtr->Db );


/*
**	Send the rename message to the object
*/

	strcpy( OldName, SecPtr->SecData.Name );
	if( !SecDbIsNew( SecPtr ) && !NewName )
		return Err( ERR_INVALID_ARGUMENTS, "Rename( %s, NULL ) @\nCan not rename existing object using NULL--use InferredName", OldName );

	if( !SecDbIsNew( SecPtr) && 
		 CacheOnlyInferredName )
		 return Err( ERR_INVALID_ARGUMENTS, "Rename( %s, NULL ) @\nCannot use Cache Only Inferred Name flag on a persisted security", OldName );
	SdbMsgData.Rename.CacheOnlyInferredName = CacheOnlyInferredName;

	pNewDb = pOldDb = pDb;
	if( NewName )
	{
		const char *NewSecName;

		if( !ResolveDbPath( NewName, pNewDb, NewSecName )) // FIX- shouldnt cast away const
			return ErrMore( "Rename()" );
		if( strlen( NewSecName ) >= SDB_SEC_NAME_SIZE )
			return Err( ERR_INVALID_ARGUMENTS, "Rename( %s, %s ) @\nNew name '%s' exceeds %d characters", OldName, NewName, NewSecName, SDB_SEC_NAME_SIZE - 1 );
		if( !*NewSecName )
			return Err( ERR_INVALID_ARGUMENTS, "Rename( %s, %s ) @\nNew name is blank", OldName, NewName );

		strcpy( SdbMsgData.Rename.NewName, NewSecName );
		SdbMsgData.Rename.CreateNewName = FALSE;
	}
	else
	{
		strcpy( SdbMsgData.Rename.NewName, SecPtr->SecData.Name );
		SdbMsgData.Rename.CreateNewName = TRUE;
	}
	SdbMsgData.Rename.NewNamePtr = NULL;

	if( !SecDbIsNew( SecPtr ))
	{
		// Start up a transaction (at least in SecPtr->Db) to capture anything written during the RENAME message

		if( pNewDb != pDb )
			return Err( ERR_UNSUPPORTED_OPERATION, "Cannot rename existing object into another database" );

		if( pDb->DbType & SDB_READONLY )
			return Err( ERR_DATABASE_READ_ONLY, "Rename( %s, %s ) %s @", OldName, SdbMsgData.Rename.NewName, pDb->DbName );

		strcpy( TransPart.Data.Begin.SecName, OldName );
		TransPart.Data.Begin.TransType		= SDB_TRAN_RENAME;
		TransPart.Data.Begin.SecType		= SecPtr->SecData.Type;
		TransPart.Data.Begin.DbID			= SecPtr->Db->DbID;
		TransPart.Data.Begin.SourceTransID	= 0;
		if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL ))
			return ErrMore( "Rename( %s, %s )", OldName, SdbMsgData.Rename.NewName );
			
		Transaction = TRUE;
	}
	else
		Transaction = FALSE;


	// Alright, finally send the RENAME message
	if( !SDB_MESSAGE( SecPtr, SDB_MSG_RENAME, &SdbMsgData ))
	{
		if( Transaction )
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return FALSE;
	}

	// The object func can either use the NewName buffer that is provided, or pass back a malloced string in NewNamePtr
	FullName = SdbMsgData.Rename.NewNamePtr ? SdbMsgData.Rename.NewNamePtr : SdbMsgData.Rename.NewName;

	// The new name might be in a different database
	pNewDb = pDb;
	if( !ResolveDbPath( FullName, pNewDb, Name ))
	{
		if( Transaction )
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return ErrMore( "Rename(): Could not resolve db path of new name" );
	}

	// Check the validity of the name (before moving into the NewName buffer)
	if( strlen( Name ) >= SDB_SEC_NAME_SIZE )
	{
		if( Transaction )
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return Err( ERR_INVALID_ARGUMENTS, "Rename( %s, %s ) @\nNew name '%s' exceeds %d characters", OldName, FullName, Name, SDB_SEC_NAME_SIZE );
	}
	if( !*Name )
	{
		if( Transaction )
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return Err( ERR_INVALID_ARGUMENTS, "Rename( %s, %s ) @\nNew name is blank", OldName, FullName );
	}

	if( Name != SdbMsgData.Rename.NewName )
	{
		char TmpBuf[ SDB_SEC_NAME_SIZE ];

		strcpy( TmpBuf, Name );
		strcpy( SdbMsgData.Rename.NewName, TmpBuf );
	}
	Name = SdbMsgData.Rename.NewName;
	free( SdbMsgData.Rename.NewNamePtr );

	if( Transaction && pNewDb != pDb )
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return Err( ERR_UNSUPPORTED_OPERATION, "Cannot rename existing object into another database" );
	}

	pDb = pNewDb;

	for( Len = strlen( Name ); --Len >= 0 && Name[ Len ] == ' '; )
		Name[ Len ] = '\0';
	if( !strcmp( Name, SecPtr->SecData.Name ) && pDb == SecPtr->Db )
	{
		if( Transaction )
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return Err( ERR_NOTHING_CHANGED, "Rename( %s, %s ) @", OldName, Name );
	}

	if( HashLookup( pDb->CacheHash, Name ))
	{
		if( Transaction )
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return Err( SDB_ERR_OBJECT_ALREADY_EXISTS, "Rename( %s, %s )\n'%s' @", OldName, NewName ? NewName : "NULL", Name );
	}


/*
**	If the security exists in a database...
*/

	if( Transaction )
	{
		if( !SecDbTransProtectedGet( pDb, OldName, &OldObject, FALSE ))
		{
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
			return FALSE;
		}
		if(		OldObject.SdbDisk.DateCreated != SecPtr->SecData.DateCreated ||
				OldObject.SdbDisk.TimeUpdated != SecPtr->SecData.TimeUpdated ||
		 		OldObject.SdbDisk.DbIDUpdated != SecPtr->SecData.DbIDUpdated ||
				OldObject.SdbDisk.UpdateCount != SecPtr->SecData.UpdateCount )
		{
			free( OldObject.Mem );
			Err( SDB_ERR_OBJECT_MODIFIED, "@" );
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
			return FALSE;
		}
	  	free( OldObject.Mem );

		TransPart.Data.Rename.SecPtr		= SecDbIncrementReference( SecPtr );
		TransPart.Data.Rename.OldSdbDisk	= SecPtr->SecData;
		
		// Change SecPtr->SecData
		strcpy( SecPtr->SecData.Name, Name );
		SecPtr->SecData.TimeUpdated = GMTGetTime( );
		SecPtr->SecData.DbIDUpdated = pDb->DbID;
		SecPtr->SecData.UpdateCount++;
		
		if( !SecDbHugeUpdate(
				pDb,
				&TransPart.Data.Rename.OldSdbDisk,
				&SecPtr->SecData,
				NULL,
				NULL ))
		{
			TransPart.Data.Rename.NewSdbDisk	= SecPtr->SecData;
			SecDbTransactionAdd( pDb, SDB_TRAN_RENAME, &TransPart, NULL );	// So it gets backed out
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
			return FALSE;
		}

		TransPart.Data.Rename.NewSdbDisk	= SecPtr->SecData;
		
		if( !SecDbTransactionAdd( pDb, SDB_TRAN_RENAME, &TransPart, NULL ))
		{
			SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
			return FALSE;
		}
		SecDbRemoveFromDeadPool( pDb, 0, OldName );
		for( IndexPtr = SecPtr->Class->IndexList; IndexPtr; IndexPtr = IndexPtr->Next )
			if( IndexPtr->Index->pDb == pDb )
				if( !SecDbIndexRenameIfMatch( IndexPtr->Index, SecPtr, OldName ))
				{
					SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
					return ErrMore( "Rename( %s, %s )", SecPtr->SecData.Name, pDb->FullDbName.c_str() );
				}

		if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
			return( FALSE );
	}

/*
**	Update the hash table, and clear all of the cached values
*/

	strcpy( SecPtr->SecData.Name, OldName );
	HashDelete( pOldDb->CacheHash, OldName );

	strcpy( SecPtr->SecData.Name, Name );
	HashInsert( pDb->CacheHash, SecPtr->SecData.Name, SecPtr );
	SecPtr->Db = pDb;

	if( pOldDb != pDb )
		FixNodeDbSets( SecPtr, pOldDb, pDb );

	if( SecDbTraceFunc )
		(*SecDbTraceFunc)( SDB_TRACE_RENAME_OBJECT, SecPtr, OldName );

	// FIX -- SecDbRemoveCachedValue( SecPtr, NULL );

	if( !SecDbInvalidate( SecPtr, SecDbValueSecurityData, 0, NULL ) )
		return ErrMore( "Failed to invalidate Security Data" );

	// If rename succeeded, don't return spurious errors
	ErrClear();

	return TRUE;
}		

