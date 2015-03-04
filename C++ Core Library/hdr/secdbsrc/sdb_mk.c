#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_mk.c,v 1.24 2004/08/12 20:52:17 khodod Exp $"
/****************************************************************
**
**	SDB_MK.C	- Create the security master test databases
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_mk.c,v $
**	Revision 1.24  2004/08/12 20:52:17  khodod
**	Check the return code from SecDbInitialize().
**	iBug #12504
**
**	Revision 1.23  2001/11/27 23:23:38  procmon
**	Reverted to CPS_SPLIT.
**	
**	Revision 1.21  2001/04/16 18:01:43  simpsk
**	eliminate "void main"
**	
**	Revision 1.20  1999/09/01 15:29:31  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.19  1999/08/09 12:13:37  goodfj
**	Changes to secdb api
**	
****************************************************************/

// BTRIEVE databases can only be created on the OS/2 platform.
#if defined( OS2 )

#define BUILDING_SECDB
#define INCL_TIMEZONE
#include	<portable.h>
#include	<os2.h>
#include	<stdio.h>
#include	<fcntl.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<share.h>
#include	<io.h>
#include	<errno.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<malloc.h>
#include	<math.h>
#include	<time.h>
#include	<date.h>
#include	<btrv.h>
#include	<kool_ade.h>
#include	<secdb.h>


/*
**	Prototype functions
*/

int 	SecDbMasterCreate(	char *fname);

static int
		CreateUniqueIDFile(	char *FileName );

static void
		Message( 			const char *Msg );


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
	SDB_SEC_TYPE
			SecTypeSecDb;

	SDB_DB
			*SdbPublic;

	SDB_OBJECT
			*SecPtr;

	CONFIGURATION
			*SecDbConfiguration;

	int		ReturnCode,
			Len;

	char	*MasterName,
			*UniqueName,
			*DatabaseName,
			*Path;


	TIMEZONE_SETUP();


/*
**	Parse the command line
*/

	if( argc != 4 )
	{
		fprintf( stderr, "Usage: SDB_MK {BtrieveFile} {UniqueFile} {DatabaseName}\n" );
		exit( 1 );
	}

	if( --argc > 0 )
		MasterName = *++argv;

	if( --argc > 0 )
		UniqueName = *++argv;

	if( --argc > 0 )
		DatabaseName = *++argv;


/*
**	Create the btrieve version of the security master database
*/

	ReturnCode = SecDbMasterCreate( MasterName );
	printf( "Return Code = %d\n", ReturnCode );


/*
**	Create the unique identifier file
*/

	if( CreateUniqueIDFile( UniqueName ))
		printf( "Unique file '%s' created\n", UniqueName );
	else
		printf( "Error creating '%s'\n", UniqueName );


/*
**	Initialize secdb
*/

	if( !SecDbInitialize( Message, Message, "sdb_mk", getenv( "USER_NAME" ) ) ) 
    {
        fprintf( stderr, "%s\n", ErrGetString() );
        exit( 1 );
    };

/*
**	Get the path(s)
*/

	if( (Path = getenv( "SECDB_PATH" )) || (Path = ConfigurationGet( SecDbConfiguration, "SECDB_PATH" )))
	{
		strcpy( SecDbPath, Path );
		Len = strlen( SecDbPath );
		if( SecDbPath[ Len - 1 ] != ':' & SecDbPath[ Len - 1 ] != '\\' & SecDbPath[ Len - 1 ] != '/' )
		{
			SecDbPath[ Len++ ] = '\\';
			SecDbPath[ Len ] = '\0';
		}
	}
	else
		*SecDbPath = '\0';

	if( (Path = getenv( "SECDB_TYPES" )) || (Path = ConfigurationGet( SecDbConfiguration, "SECDB_TYPES" )))
	{
		strcpy( SecDbTypesPath, Path );
		Len = strlen( SecDbTypesPath );
		if( SecDbTypesPath[ Len - 1 ] != ':' & SecDbTypesPath[ Len - 1 ] != '\\' & SecDbTypesPath[ Len - 1 ] != '/' )
		{
			SecDbTypesPath[ Len++ ] = '\\';
			SecDbTypesPath[ Len ] = '\0';
		}
	}
	else
		strcpy( SecDbTypesPath, SecDbPath );


/*
**	Read the configuration information
*/

	SecDbConfiguration = ConfigurationLoad( "SECDB.CFG", NULL, NULL );


/*
**	Load the database table
*/

	if( !(Path = getenv( "SECDB_DATABASE_TABLE" )) && !(Path = ConfigurationGet( SecDbConfiguration, "SECDB_DATABASE_TABLE" )))
		Path = "database.dat";
	if( !SecDbLoadDatabaseTable( SecDbTypesPath, Path ))
	{
		printf( "Couldn't read database.dat\n" );
		exit( 1 );
	}


/*
**	Load the security object table
*/

	if( !SecDbLoadObjTable( SecDbTypesPath, "sectypes.dat" ))
	{
		printf( "Couldn't read sectypes.dat\n" );
		exit( 1 );
	}


/*
**	Load the data types table
*/

	if( !DtLoadTable( SecDbTypesPath, "datatype.dat" ))
	{
		printf( "Couldn't read datatype.dat\n" );
		exit( 1 );
	}


/*
**	Attach to the security database
*/

	if( !(SdbPublic = SecDbAttach( DatabaseName, 0, SDB_FRONT | SDB_READWRITE )))
		printf( "Attached to database...\n" );
	else
	{
		printf( "Couldn't attach to security master database\n" );
		exit( 1 );
	}


/*
**	Create the one (and only) instance of a security database
**	security.
*/

	SecTypeSecDb = SecDbClassTypeFromName( "Security Database" );

	SecPtr = SecDbNew( "Security Database",	SecTypeSecDb, SdbPublic );

	if( SecDbAdd( SecPtr ))
		printf( "Created 'Security Database' object\n" );
	else
		printf( "Error, couldn't create 'Security Database' object\nError Text: %s\n", ErrGetString() );

	SecDbFree( SecPtr );


/*
**	Close the database
*/

	SecDbDetach( SdbPublic );
	return 0;
}



/****************************************************************
**	Routine: CreateUniqueIDFile
**	Returns: TRUE	- Unique ID file created without error
**			 FALSE	- Error creating file for unique ID's
**	Action : Create a file that contains the unique identfier.
****************************************************************/

static int CreateUniqueIDFile(
char	*FileName			// Name of file to create
)
{
	int		fhUniqueID;

	SDB_UNIQUE_ID
			UniqueID = time( NULL );


	fhUniqueID = open( FileName, O_BINARY | O_CREAT | O_TRUNC | O_RDWR, S_IREAD | S_IWRITE );
	if( -1 == fhUniqueID )
		return FALSE;
	if( sizeof( UniqueID ) != write( fhUniqueID, &UniqueID, sizeof( UniqueID )))
	{
		close( fhUniqueID );
		return FALSE;
	}
	close( fhUniqueID );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbMasterCreate
**	Returns: TRUE if created Database
**			 FALSE otherwise
**	Action : Create a btrieve security master file
****************************************************************/

int SecDbMasterCreate(char *fname)
{
	struct
	{
		btrv_FILE_SPEC
				f;

		btrv_KEY_SPEC
				k1a,
				k2a,
				k2b;

		btrv_COLLATE
				AltCol;

	} setup1;

	btrv_POS_BLOCK
			pos_block;

	SDB_DISK
			SecDbDisk;

	int		i,
			CharValue;

			
	SHUSH_UNREF_WARNING( SecDbDisk );

	// Record data
	setup1.f.record_length	= sizeof(SDB_DISK);
	setup1.f.page_size		= 1024;
	setup1.f.nindices 		= 2;
	setup1.f.nrecs 			= 0L;
	setup1.f.flags 			= btrv_VARIABLE | btrv_TRUNCATE | btrv_FREE10;
	setup1.f.reserved 		= 0;
	setup1.f.allocation		= 0;

	// Key #1A
	setup1.k1a.pos 			= STRUCT_OFFSET(SDB_DISK,Name) + 1;
	setup1.k1a.length		= sizeof(SecDbDisk.Name);
	setup1.k1a.flags 		= btrv_EXTENDED | btrv_MODIFY | btrv_ALT_COLL;
	setup1.k1a.nkeys 		= 0L;
	setup1.k1a.type  		= btrv_ZSTRING;
	setup1.k1a.null_value	= '\0';
	setup1.k1a.reserved 	= 0L;

	// Key #2A
	setup1.k2a.pos 			= STRUCT_OFFSET(SDB_DISK,Type) + 1;
	setup1.k2a.length		= sizeof(SecDbDisk.Type);
	setup1.k2a.flags 		= btrv_EXTENDED | btrv_MODIFY | btrv_DUPLICATE | btrv_SEGS;
	setup1.k2a.nkeys 		= 0L;
	setup1.k2a.type  		= btrv_INT;
	setup1.k2a.null_value	= '\0';
	setup1.k2a.reserved 	= 0L;

	// Key #2B
	setup1.k2b.pos 			= STRUCT_OFFSET(SDB_DISK,Name) + 1;
	setup1.k2b.length		= sizeof(SecDbDisk.Name);
	setup1.k2b.flags 		= btrv_EXTENDED | btrv_MODIFY | btrv_DUPLICATE | btrv_ALT_COLL;
	setup1.k2b.nkeys 		= 0L;
	setup1.k2b.type  		= btrv_ZSTRING;
	setup1.k2b.null_value	= '\0';
	setup1.k2b.reserved 	= 0L;
	
	// Alternate collating sequence (upper & lower are the same)
	setup1.AltCol.signature	= 0xAC;
	strcpy( setup1.AltCol.name, "Stricmp" );
	for( CharValue = 0; CharValue < 256; CharValue++ )
		setup1.AltCol.table[ CharValue ] = (char) (CharValue < 'a' || CharValue > 'z' ? CharValue : CharValue - ('a' - 'A'));

	// Create the database
	i = sizeof(setup1);
	return(BTRV(btrv_CREATE,pos_block,(char *)&setup1,&i,fname,0));
}



/****************************************************************
**	Routine: Message
**	Returns: None
**	Action : Display messages for SecDb
****************************************************************/

static void Message(
const char	*Msg
)
{
	if( !Msg )
		Msg = "";

	fprintf( stderr, "%-78.78s\r", Msg );
}

#else

#include	<stdio.h>


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
	fprintf( stderr, "sdb_mk runs only on OS/2\n" );
	return 0;
}

#endif


