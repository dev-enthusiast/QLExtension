#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_uniq.c,v 1.20 2001/11/27 23:23:39 procmon Exp $"
/****************************************************************
**
**	SDB_UNIQ.C	- Security database unique ID functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_uniq.c,v $
**	Revision 1.20  2001/11/27 23:23:39  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.18  2000/06/12 14:42:42  goodfj
**	DbUpdate fixes
**	
**	Revision 1.17  1999/12/06 13:50:34  goodfj
**	DbUpdate is now private. New api to access it (for the time-being)
**	
**	Revision 1.16  1999/09/01 15:29:33  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.15  1999/08/10 19:58:08  singhki
**	Register None and Null as diddle scopes in order to preserve behaviour
**	
**	Revision 1.14  1999/08/07 01:40:12  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.13  1999/04/23 00:03:33  nauntm
**	Unused arg/uninitialized variable extermination
**	
**	Revision 1.12  1998/11/16 23:00:00  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.11.2.2  1998/11/05 01:01:26  singhki
**	TempRev: hacks to support OS/2 IBM compiler in C++ mode
**	
**	Revision 1.11.2.1  1998/10/30 00:03:19  singhki
**	DiddleColors: Initial revision
**	
**	Revision 1.11  1998/10/14 18:20:54  singhki
**	New search paths
**	
**	Revision 1.10.2.1  1998/08/28 23:34:06  singhki
**	search paths, index merge working, some problems with ~casts
**	
**	Revision 1.10  1998/01/02 21:33:12  rubenb
**	added GSCVSID
**	
**	Revision 1.9  1996/12/23 03:06:26  gribbg
**	Update documentation for New Trees (autodoc for SecDb functions)
**	Change SecDbError to Err and SDB_ERR to ERR (where appropriate)
**	Minor format and 'fix' cleanup
**	
**	Revision 1.8  1996/01/16 18:16:22  ERICH
**	NT Compatibility
**
**	   Rev 1.7   13 Jan 1995 20:46:44   DUBNOM
**	UniqueID now takes a database ID
**
**	   Rev 1.6   07 Apr 1994 19:11:20   LUNDEK
**	Split off datatype project
**
****************************************************************/

#define BUILDING_SECDB
#define	INCL_FILEIO
#include	<portable.h>

#if defined( OS2 )
//#	define	INCL_DOS
#	define	INCL_NOPM
#	include	<os2.h>
#	include	<io.h>
#	include	<share.h>
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>

#if defined( MICROSOFT )
#	include	<sys/locking.h>
#endif

#if defined( WIN32 )
#	include	<sys/locking.h>
#	include	<share.h>
#endif

#include	<secdb.h>
#include	<secerror.h>
#include	<secindex.h>
#include	<secdrv.h>
#include	<sdb_int.h>


// FIX - VERY VERY DANGEROUS BLATANT DISREGARD FOR LOCKING
#if defined( __unix ) || defined( IBM_CPP )

#define	LK_LOCK		1
#define	LK_UNLCK	2


/****************************************************************
**	Routine: locking
**	Returns:
**	Action :
****************************************************************/

static int locking(
	int		, //fhFile,
	int		, //LockMode,
	long	  //LockSize
)
{
	return 0;
}
#endif


/****************************************************************
**	Routine: SecDbUniqueID
**	Returns: Unique identifier or 0 if error
**	Summary: Retrieve a unique identifier from the SECDB system
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbUniqueID"

SDB_UNIQUE_ID SecDbUniqueID(
	SDB_DB	*pDb						// Database to use
)
{
	SDB_UNIQUE_ID
			UniqueID;

	int		fhUniqueID,
			BytesWritten;

	SDB_DB	*DbUpdate;


/*
**	Check for a database supplied unique ID generator
*/

	if( !( pDb = DbToPhysicalDb( pDb )))
	{
		Err( ERR_DATABASE_INVALID, ARGCHECK_FUNCTION_NAME "() @" );
		return 0L;
	}
    DbUpdate = SecDbDefaultDbUpdateGet( pDb );
	if( DbUpdate != pDb )
		return SecDbUniqueID( DbUpdate );

	if( pDb->pfGetUniqueID )
		return (*pDb->pfGetUniqueID)( pDb );


/*
**	Retrieve unique identifier
*/

	fhUniqueID = sopen( SecDbUniqueIDFileName, O_BINARY | O_RDWR, SH_DENYNO, S_IREAD | S_IWRITE );
	if( -1 == fhUniqueID )
	{
		Err( ERR_FILE_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) @", SecDbUniqueIDFileName );
		return 0L;
	}
	if( -1 == locking( fhUniqueID, LK_LOCK, sizeof( SDB_UNIQUE_ID )) || sizeof( SDB_UNIQUE_ID ) != read( fhUniqueID, &UniqueID, sizeof( SDB_UNIQUE_ID )))
	{
		Err( ERR_FILE_FAILURE, ARGCHECK_FUNCTION_NAME "( %s ) Locking - @", SecDbUniqueIDFileName );
		close( fhUniqueID );
		return 0L;
	}
	++UniqueID;
	lseek( fhUniqueID, 0L, SEEK_SET );
	BytesWritten = write( fhUniqueID, &UniqueID, sizeof( SDB_UNIQUE_ID ));
	lseek( fhUniqueID, 0L, SEEK_SET );
	locking( fhUniqueID, LK_UNLCK, sizeof( SDB_UNIQUE_ID ));
	close( fhUniqueID );

	if( sizeof( SDB_UNIQUE_ID ) != BytesWritten )
	{
		Err( ERR_FILE_FAILURE, ARGCHECK_FUNCTION_NAME "( %s ) Writing - @", SecDbUniqueIDFileName );
		return 0L;
	}

	return UniqueID;
}
