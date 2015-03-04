#define GSCVSID "$Header: /home/cvs/src/secdb/src/secdel.c,v 1.24 2010/10/25 19:13:33 c03833 Exp $"
/****************************************************************
**
**	SECDEL.C	-- Delete all the securities of a given class
**
**	$Log: secdel.c,v $
**	Revision 1.24  2010/10/25 19:13:33  c03833
**	Include ccmain.h to handle errors for the entire application
**
**	Revision 1.23  2000/06/07 14:05:28  goodfj
**	Fix includes
**
**	Revision 1.22  1999/11/01 21:58:59  singhki
**	oops, no BUILDING_SECDB in executables which link with secdb
**	
**	Revision 1.21  1999/09/01 15:29:36  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.20  1999/08/07 01:40:15  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.19  1999/02/02 22:58:40  brownb
**	eliminate GCC warnings about gets() calls
**	
**	Revision 1.18  1998/05/20 20:32:01  brownb
**	fix NT warning - main() must return, not call exit()
**	
**	Revision 1.17  1998/05/20 19:40:45  brownb
**	fix gcc 2.8.1 warning -- main() should return int
**	
****************************************************************/

#define INCL_TIMEZONE
#include	<portable.h>
#include	<err.h>
#include	<ccmain.h>
#include	<secdb.h>


/*
**	Define constants
*/

#define	ARG_TYPE_INVALID		0
#define	ARG_TYPE_SECURITY		1
#define	ARG_TYPE_SECTYPE		2


/*
**	Define global
*/

static int 
		Force = FALSE;


/*
**	Prototype functions
*/

static int
		DeleteByClass(	SDB_DB *Db, char *SecDbClass ),
		DeleteByName(	SDB_DB *Db, char *SecName );

static void
		ShowClasses(	void ),
		ShowUsage(		void ),
		Message(		const char *Msg );


/****************************************************************
**	Routine: Main
**	Returns: 
**	Action : 
****************************************************************/

int main(
	int 	argc,
	char	*argv[]
)
{
	unsigned int
			ArgType		= ARG_TYPE_INVALID;

	char	*Arg,
			*DatabaseName;
			
	int		Failed		= FALSE;


	TIMEZONE_SETUP();


/*
**	Initial check of the command line
*/

	if( argc < 2 )
		ShowUsage();

	// Check for database name
	if( *argv[1] != '-' )
	{
		DatabaseName = *++argv;
		--argc;
	}
	else
		DatabaseName = NULL;


/*
**	Startup SecDb
*/

	if( !SecDbStartup( DatabaseName, SDB_DEFAULT_DEADPOOL_SIZE, Message, Message, "SecDel", getenv( "USER_NAME" )))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		return 1;
	}
	

/*
**	Parse the command line
*/

	while( argc-- > 1 )
	{
		Arg = *++argv;
		if( *Arg == '-' )
			switch( tolower( Arg[1] ))
			{
				case 'l':	ShowClasses();							break;
				case 's':	ArgType = ARG_TYPE_SECURITY;			break;
				case 't':	ArgType = ARG_TYPE_SECTYPE;				break;
				case 'f':	Force = TRUE;							break;
				default :	ShowUsage();
			}
		
		else
		{
			switch( ArgType )
			{
				case ARG_TYPE_SECURITY:	// Individual security
					Failed = !DeleteByName( SecDbRootDb, Arg );
					break;

				case ARG_TYPE_SECTYPE:	// All securities of a certain type
					Failed = !DeleteByClass( SecDbRootDb, Arg );
					break;
			}

			ArgType = ARG_TYPE_INVALID;
		}
	}


/*
**	Shutdown SecDb
*/

	SecDbShutdown( TRUE );
	return Failed;
}



/****************************************************************
**	Routine: DeleteByClass
**	Returns: TRUE	- Class instances deleted without error
**			 FALSE	- Error deleting class instances
**	Action : Delete all instances of a given class
****************************************************************/

static int DeleteByClass(
	SDB_DB	*Db,		// SecDb handle
	char	*ClassName	// Class to iterate through
)
{
	char	SecName[	SDB_SEC_NAME_SIZE ],
			Buffer[		256 ];
			
	SDB_SEC_TYPE
			SecType;
			
	int		All = Force;
			

	SecName[ 0 ] = '\0';
	SecType = SecDbClassTypeFromName( ClassName );
	if( SecType )
	{
		while( SecDbNameLookup( SecName, SecType, SDB_GET_GREATER, Db ))
		{
			if( !All )
			{
				printf( "Delete %s? [n] ", SecName );
				fflush( stdout );

				if ( fgets( Buffer, sizeof( Buffer), stdin ) == NULL )
					continue;

				if( Buffer[ 0 ] == 'A' || Buffer[ 0 ] == 'a' )
					All = TRUE;
				else if( Buffer[ 0 ] != 'Y' && Buffer[ 0 ] != 'y' )
					continue;
			}
				
			if( SecDbDeleteByName( SecName, Db, TRUE ))
				printf( "%s deleted\n", SecName);
			else
				fprintf( stderr, "Couldn't delete %s\n%s\n", SecName, ErrGetString() );
		}
	}
	else
	{
		fprintf( stderr, "Error: %s\n", ErrGetString() );
		return FALSE;
	}
	
	return TRUE;
}



/****************************************************************
**	Routine: DeleteByName
**	Returns: TRUE	- No error
**			 FALSE	- Errors
**	Action : Delete a security by name
****************************************************************/

static int DeleteByName(
	SDB_DB	*Db,		// SecDb handle
	char	*SecName	// Name of security to delete
)
{
	char	Buffer[ 256 ];
			

	if( !Force )
	{
		printf( "Delete %s? [n] ", SecName );
		fflush( stdout );

		if ( fgets( Buffer, sizeof( Buffer), stdin ) == NULL
			 || (Buffer[ 0 ] != 'Y' && Buffer[ 0 ] != 'y') )
			return TRUE;
	}

	if( !SecDbDeleteByName( SecName, Db, TRUE ))
	{
		fprintf( stderr, "Couldn't delete %s\n%s\n", SecName, ErrGetString() );
		return FALSE;
	}
		
	printf( "%s deleted\n", SecName );
	return TRUE;
}



/****************************************************************
**	Routine: ShowClasses
**	Returns: None
**	Action : List security classes
****************************************************************/

static void ShowClasses( void )
{
	SDB_CLASS_INFO
			*ClassInfo;

	SDB_ENUM_PTR
			EnumPtr;


/*
**	Format the data into the window
*/

	for( ClassInfo = SecDbClassEnumFirst( &EnumPtr );
		ClassInfo;
		ClassInfo = SecDbClassEnumNext( EnumPtr ))
	{
		printf( "%5d - %s\n",
				ClassInfo->Type,
				ClassInfo->Name );
	}

	SecDbClassEnumClose( EnumPtr );
}



/****************************************************************
**	Routine: ShowUsage
**	Returns: None
**	Action : Display usage
****************************************************************/

static void ShowUsage( void )
{
	fprintf( stderr, "Usage: SecDel [Database] [-l] [-s Security Name ] [-t Security Class] ...\n" );
	fprintf( stderr, "\t-f                   Force, i.e., don't ask\n" );
	fprintf( stderr, "\t-l                   List security classes\n" );
	fprintf( stderr, "\t-s Security Name     Delete an individual security\n" );
	fprintf( stderr, "\t-t Security Class    Delete an entire class of securities\n" );

	exit( 1 );
}



/****************************************************************
**	Routine: Message
**	Returns: None
**	Action : Display a message from SecDb on the screen
****************************************************************/

static void Message(
	const char	*Msg
)
{
	if( !Msg )
		Msg = "";

	fprintf( stderr, "%-78.78s\r", Msg );
}
