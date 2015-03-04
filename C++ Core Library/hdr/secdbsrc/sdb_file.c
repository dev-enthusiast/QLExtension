#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_file.c,v 1.14 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SDB_FILE.C	- SecDbFile functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_file.c,v $
**	Revision 1.14  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.13  2001/08/10 09:02:51  goodfj
**	Better errors
**
**	Revision 1.12  2000/12/19 17:20:44  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.11  1999/09/01 15:29:30  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<secdb.h>
#include	<secerror.h>
#include	<secdrv.h>
#include	<secdrive.h>
#include	<sectrans.h>
#include	<sdb_int.h>
#include	<secfile.h>



/****************************************************************
**	Routine: SecDbFileInfoToStructure
**	Returns: TRUE/Err()
**	Action : Convert a SDB_FILE_INFO to a DtStructure
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileInfoToStructure"

int SecDbFileInfoToStructure(
	SDB_FILE_INFO	*Info,
	DT_VALUE		*Value
)
{
	return	DTM_ALLOC( Value, DtStructure )
		&&	DtComponentSetNumber( Value, "LastAccess", DtTime,   Info->LastAccess )
		&&	DtComponentSetNumber( Value, "LastUpdate", DtTime,   Info->LastUpdate )
		&&	DtComponentSetNumber( Value, "Length",     DtDouble, Info->Length     );
}



/****************************************************************
**	Routine: SecDbFileInfoFromStructure
**	Returns: TRUE/Err()
**	Action : Convert a DtStructure to a SDB_FILE_INFO
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileInfoFromStructure"

int SecDbFileInfoFromStructure(
	SDB_FILE_INFO	*Info,
	DT_VALUE		*Value
)
{
	memset( Info, 0, sizeof( *Info ));
	Info->LastAccess = (SDB_TIME)      DtComponentGetNumber( Value, "LastAccess", DtTime,   -1 );
	Info->LastUpdate = (SDB_TIME)      DtComponentGetNumber( Value, "LastUpdate", DtTime,   -1 );
	Info->Length     = (SDB_FILE_SIZE) DtComponentGetNumber( Value, "Length",     DtDouble, -1 );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbFileWrite
**	Returns: TRUE/Err()
**	Action : Add FILE_WRITE transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileWrite"

int SecDbFileWrite(
	SDB_DB				*pDb,
	SDB_FILE_HANDLE		File,
	const void			*Data,
	SDB_FILE_SIZE		Length,
	SDB_FILE_SIZE		Position
)
{
	SDB_TRANS_PART
			TransPart;

	void	*DataCopy;


	if( !pDb )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": NULL db" );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Physical Db was NULL", pDb->DbName );
	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, ARGCHECK_FUNCTION_NAME "( %s ): @", pDb->DbName );

	if( !(DataCopy = malloc( Length )))
		return Err( ERR_MEMORY, "@" );

	sprintf( TransPart.Data.Begin.SecName, "%ld", (long) File );
	TransPart.Data.Begin.TransType		= SDB_TRAN_FILE_WRITE;
	TransPart.Data.Begin.SecType		= 0;
	TransPart.Data.Begin.DbID			= pDb->DbID;
	TransPart.Data.Begin.SourceTransID	= 0;
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );

	memcpy( DataCopy, Data, Length );
	TransPart.Data.FileWrite.File     = File;
	TransPart.Data.FileWrite.Data     = DataCopy;
	TransPart.Data.FileWrite.Length   = Length;
	TransPart.Data.FileWrite.Position = Position;
	if(	!SecDbTransactionAdd( pDb, SDB_TRAN_FILE_WRITE, &TransPart, NULL ))
	{
		free( DataCopy );
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );
	}
	
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );
		
	return TRUE;
}



/****************************************************************
**	Routine: SecDbFileInfoSet
**	Returns: TRUE/Err()
**	Action : Add FILE_INFO_SET transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileInfoSet"

int SecDbFileInfoSet(
	SDB_DB				*pDb,
	SDB_FILE_HANDLE		File,
	SDB_FILE_INFO		*Info
)
{
	SDB_TRANS_PART
			TransPart;


	if( !pDb )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": NULL db" );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Physical Db was NULL", pDb->DbName );
	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, ARGCHECK_FUNCTION_NAME "( %s ): @", pDb->DbName );

	sprintf( TransPart.Data.Begin.SecName, "%ld", (long) File );
	TransPart.Data.Begin.TransType		= SDB_TRAN_FILE_INFO_SET;
	TransPart.Data.Begin.SecType		= 0;
	TransPart.Data.Begin.DbID			= pDb->DbID;
	TransPart.Data.Begin.SourceTransID	= 0;
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );

	TransPart.Data.FileInfoSet.File = File;
	TransPart.Data.FileInfoSet.Info = *Info;
	if(	!SecDbTransactionAdd( pDb, SDB_TRAN_FILE_INFO_SET, &TransPart, NULL ))
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );
	}
	
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );
		
	return TRUE;
}



/****************************************************************
**	Routine: SecDbFileOpen
**	Returns: File handle if successful
**			 0 if file can't be found
**	Action : Get a file handle for a file
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileOpen"

SDB_FILE_HANDLE SecDbFileOpen(
	SDB_DB*     pDb,
	char const* FileName
)
{
	if( !pDb )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": NULL db" );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Physical Db was NULL", pDb->DbName );

	return (*pDb->pfFileOpen)( pDb, FileName );
}



/****************************************************************
**	Routine: SecDbFileClose
**	Returns: TRUE/FALSE
**	Action : Close a file
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileClose"

int SecDbFileClose(
	SDB_DB				*pDb,
	SDB_FILE_HANDLE		File
)
{
	if( !pDb )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": NULL db" );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Physical Db was NULL", pDb->DbName );

	return (*pDb->pfFileClose)( pDb, File );
}



/****************************************************************
**	Routine: SecDbFileDelete
**	Returns: TRUE/Err()
**	Action : Add FILE_DELETE transaction
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileDelete"

int SecDbFileDelete(
	SDB_DB				*pDb,
	SDB_FILE_HANDLE		File
)
{
	SDB_TRANS_PART
			TransPart;


	if( !pDb )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": NULL db" );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Physical Db was NULL", pDb->DbName );
	if( pDb->DbType & SDB_READONLY )
		return Err( ERR_DATABASE_READ_ONLY, ARGCHECK_FUNCTION_NAME "( %s ): @", pDb->DbName );

	sprintf( TransPart.Data.Begin.SecName, "%ld", (long) File );
	TransPart.Data.Begin.TransType		= SDB_TRAN_FILE_DELETE;
	TransPart.Data.Begin.SecType		= 0;
	TransPart.Data.Begin.DbID			= pDb->DbID;
	TransPart.Data.Begin.SourceTransID	= 0;
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_BEGIN, &TransPart, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );

	TransPart.Data.FileDelete.File = File;
	if(	!SecDbTransactionAdd( pDb, SDB_TRAN_FILE_DELETE, &TransPart, NULL ))
	{
		SecDbTransactionAdd( pDb, SDB_TRAN_ABORT, NULL, NULL );
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );
	}
	
	if( !SecDbTransactionAdd( pDb, SDB_TRAN_COMMIT, NULL, NULL ))
		return ErrMore( ARGCHECK_FUNCTION_NAME "( %s, %ld )", pDb->DbName, (long) File );
		
	return TRUE;
}



/****************************************************************
**	Routine: SecDbFileInfoGet
**	Returns: TRUE/Err()
**	Action : Call database driver
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileInfoGet"

int SecDbFileInfoGet(
	SDB_DB				*pDb,
	SDB_FILE_HANDLE		File,
	SDB_FILE_INFO		*Info
)
{
	if( !pDb )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": NULL db" );
	if( !( pDb = DbToPhysicalDb( pDb )))
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Physical Db was NULL", pDb->DbName );

	return (*pDb->pfFileInfoGet)( pDb, File, Info );
}



/****************************************************************
**	Routine: SecDbFileRead
**	Returns: Number of bytes read or -1 for error
**	Action : Call database driver
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbFileRead"

SDB_FILE_SIZE SecDbFileRead(
	SDB_DB				*pDb,
	SDB_FILE_HANDLE		File,
	void				*Data,
	SDB_FILE_SIZE		Length,
	SDB_FILE_SIZE		Position
)
{
	if( !pDb )
	{
		Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": NULL db" );
		return -1;
	}
	if( !( pDb = DbToPhysicalDb( pDb )))
	{
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME "( %s ): Physical Db was NULL", pDb->DbName );
		return -1;
	}	

	return (*pDb->pfFileRead)( pDb, File, Data, Length, Position );
}
