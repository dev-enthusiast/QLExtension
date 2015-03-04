#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_un.c,v 1.9 2001/04/16 18:01:44 simpsk Exp $"
/****************************************************************
**
**	SDB_UN.C	- Set the unique ID in the unique file
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_un.c,v $
**	Revision 1.9  2001/04/16 18:01:44  simpsk
**	eliminate "void main"
**
**	Revision 1.8  1999/09/01 15:29:33  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.7  1998/01/02 21:33:11  rubenb
**	added GSCVSID
**	
**	Revision 1.6  1995/01/09 17:36:24  DUBNOM
**	Changed database search mechanism
**	
**	
**	   Rev 1.6   09 Jan 1995 12:36:24   DUBNOM
**	Changed database search mechanism
**	
**	   Rev 1.5   20 Oct 1994 21:45:46   LUNDEK
**	Get rid of unnecessary header files
**	
**	   Rev 1.4   07 Apr 1994 19:11:20   LUNDEK
**	Split off datatype project
**	
**	   Rev 1.3   16 Feb 1994 13:53:40   GRIBBG
**	Use INCL_TIMEZONE
**	
**	   Rev 1.2   14 Feb 1994 20:17:02   GRIBBG
**	Use TIMEZONE_SETUP()
**	
**	   Rev 1.1   13 Apr 1993 15:56:30   DUBNOM
**	Minor modifications to enhance portability
**	
**	   Rev 1.0   23 Nov 1992 20:21:18   DUBNOM
**	Initial revision.
**
****************************************************************/

#define BUILDING_SECDB
#define INCL_TIMEZONE
#include	<portable.h>
#include	<stdio.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/locking.h>
#include	<share.h>
#include	<io.h>
#include	<stdlib.h>
#include	<string.h>
#include	<secdb.h>

/*
**	Prototype functions
*/

static void
		SetUniqueID( char *UniqueName, SDB_UNIQUE_ID NewUniqueID );
		
		
/****************************************************************
**	Routine: main
**	Returns: None
**	Action : Create security master databases
****************************************************************/

int main(
	int 	argc,
	char	*argv[]
)
{
	char	
			*UniqueName;

	SDB_UNIQUE_ID
			UniqueID;	


	TIMEZONE_SETUP();


/*
**	Parse the command line
*/

	if( argc < 1 || argc > 3 )
	{
		fprintf( stderr, "Usage: SDB_UN {UniqueFile} [{New Unique Value}]\n" );
		exit( 1 );
	}

	UniqueName = argv[1];

	if( argc == 3 )
	{
		UniqueID = atol( argv[2] );
		if( UniqueID == 0 )
		{
			fprintf( stderr, "SDB_UN: UniqueID == 0, not allowed\n" );
			exit( 1 );
		}
		SetUniqueID( UniqueName, UniqueID );
	}
	SetUniqueID( UniqueName, 0 );
	return 0;
}


/****************************************************************
**	Routine: SecDbUniqueID
**	Returns: Unique identifier
**	Action : Returns an identifier guaranteed to be unique within
**			 the security database
****************************************************************/

static void SetUniqueID(
	char			*UniqueName,
	SDB_UNIQUE_ID	NewUniqueID
)
{
	int		fhUniqueID;

	SDB_UNIQUE_ID
			UniqueID;


/*
**	Open the file
*/
	
	fhUniqueID = sopen( UniqueName, O_BINARY | O_RDWR, SH_DENYNO, S_IREAD | S_IWRITE );
	if( -1 == fhUniqueID )
	{
		perror( UniqueName );
		fprintf( stderr, "Unable to open unique file\n" );
		exit( 1 );
	}
	if( -1 == locking( fhUniqueID, LK_LOCK, sizeof( SDB_UNIQUE_ID )) || sizeof( SDB_UNIQUE_ID ) != read( fhUniqueID, &UniqueID, sizeof( SDB_UNIQUE_ID )))
	{
		perror( UniqueName );
		fprintf( stderr, "Unable to lock or read unique file\n" );
		exit( 1 );
	}
	printf( "UniqueID in %s is %lu\n", UniqueName, UniqueID );
	if( NewUniqueID )
	{
		UniqueID = NewUniqueID;
		lseek( fhUniqueID, 0L, SEEK_SET );
		write( fhUniqueID, &UniqueID, sizeof( SDB_UNIQUE_ID ));
	}
	lseek( fhUniqueID, 0L, SEEK_SET );
	locking( fhUniqueID, LK_UNLCK, sizeof( SDB_UNIQUE_ID ));
	close( fhUniqueID );
}
