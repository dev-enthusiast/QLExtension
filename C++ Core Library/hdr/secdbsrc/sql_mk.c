#define GSCVSID "$Header: /home/cvs/src/secdb/src/sql_mk.c,v 1.4 1999/09/01 15:29:36 singhki Exp $"
/****************************************************************
**
**	SQL_MK.C	- SecDb SQL security master creation function
**
**	$Log: sql_mk.c,v $
**	Revision 1.4  1999/09/01 15:29:36  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**
**	Revision 1.3  1998/01/02 21:33:27  rubenb
**	added GSCVSID
**	
**	Revision 1.2  1996/12/23 03:06:38  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**	
**	Revision 1.1  1994/05/13 00:08:46  GRIBBG
**	Add portable.h
**
**	   Rev 1.0   16 Nov 1993 12:34:32   CHAVEM
**	Initial revision.
**
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<os2.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<malloc.h>
#include	<memory.h>
#include	<secdb.h>
#include	<secobj.h>
#include	<secerror.h>
#include	<secindex.h>
#include	<sdb_int.h>

#define	DBMSOS2		// Identify operating system for SQL
#include	<sybfront.h>
#include	<sybdb.h>
#include	<sdb_sql.h>



/****************************************************************
**	Routine: SqlDrvDbCreate
**	Returns: TRUE 	- Database created without error
**			 FALSE	- Error creating database
**	Action : Create an SQL security master
****************************************************************/

int SqlDrvDbCreate(
	SDB_DB	*pDb
)
{
	SQL_DB	*DbHandle;

	char	FileNameMK[ _MAX_PATH ],
			FileNameSP[ _MAX_PATH ];

	int		Success;


	if( Success = SqlDrvDbOpen( pDb ))
	{
		DbHandle = pDb->Handle;

		sprintf( FileNameMK, "%sSDB_MK.SQL", SecDbTypesPath );
		sprintf( FileNameSP, "%sSDB_SP.SQL", SecDbTypesPath );

		Success = FileToSql( FileNameMK, DbHandle->q_dbproc ) && FileToSql( FileNameSP, DbHandle->q_dbproc );

		SqlDrvDbClose( DbHandle, NULL );
	}

	if( !Success )
		return ErrMore( "SqlCreate" );

	return TRUE;
}



/****************************************************************
**	Routine: FileToSql
**	Returns: TRUE	- Success
**			 FALSE	- Failure
**	Action : Submit a file to the SQL server
****************************************************************/

int FileToSql(
	char		*FileName,
	DBPROCESS	*DbProc
)
{
	char	Buffer[ 2048 ];

	FILE	*fpSql;


	// Clear any existing error text
	SqlDrvClearError();

	if( !(fpSql = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, "'%s' - @", FileName );

	while( fgets( Buffer, sizeof( Buffer ), fpSql ))
	{
		StrTrim( Buffer );

		// If its not the execute keyword, then just send the command
		if( stricmp( Buffer, "GO" ))
			dbfcmd( DbProc, "%s\n", Buffer );
		else 
		{
			// FIX - Better error handler
			if( SUCCEED != dbsqlexec( DbProc ))
			{
				fclose( fpSql );
				return Err( ERR_DATABASE_FAILURE, "SqlCreate @ - %s", SqlErrorString );
			}
			while( SUCCEED == dbresults( DbProc ))
				dbcanquery( DbProc );
		}
	}

	fclose( fpSql );

	return TRUE;
}
