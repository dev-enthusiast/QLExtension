#define GSCVSID "$Header: /home/cvs/src/secdb/src/seccopy.c,v 1.64 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SECCOPY.C	- Program to transfer securities from one database
**				  to another
**
**	$Log: seccopy.c,v $
**	Revision 1.64  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.63  2008/07/02 16:13:26  khodod
**	New way to copy db source id in SecurityCopy and seccopy.
**	iBug: #56731
**
**	Revision 1.62  2005/05/27 20:19:29  e45019
**	SecDbCopyByName now takes Dup argument which if true causes a duplicate security to be created, and if false causes a new security to be created.
**	iBug: #25421
**	
****************************************************************/

#include	<portable.h>
#include	<stdio.h>
#include	<errno.h>
#include	<ctype.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<malloc.h>
#include	<time.h>
#include	<date.h>
#include	<trap.h>
#include	<outform.h>
#include	<kool_ade.h>

#include	<seccopy.h>
#include	<secexpr.h>
#include	<secindex.h>
#include	<sectrans.h>
#include	<ccmain.h>

#if defined( OS2 ) || defined( WIN32 )
#	include	<conio.h>
#endif

#if defined( __unix )
#	define	getch()	getchar()
#endif

/*
**	Define constants
*/

#define	MAX_NAMES_TO_BUFFER		250		// Number of names/attempt for ForSecurity
#define	DEFAULT_BUNDLE_SIZE		100

#define	ARG_TYPE_SECURITY		1		// Single security
#define	ARG_TYPE_SECTYPE		2		// All of a given type
#define ARG_TYPE_SECURITY_LIST	3		// List of securities
#define ARG_TYPE_RANGE			4		// Range of securities
#define ARG_TYPE_BUNDLE_SIZE	5		// Bundle size for transactions
#define ARG_TYPE_SECPREFIX		6		// All securities whose name has a specified prefix

/*
**	Dup flags
*/

#define	COPY_FLAG_NO_DUPES			1
#define	COPY_FLAG_COPY_DUPES		2
#define	COPY_FLAG_PROMPT_DUPES		3

/*
**	Variables
*/

int	CopyFlags             = COPY_FLAG_PROMPT_DUPES;
int DupSrcDbId            = FALSE;  // Duplicate src db id (to mimic secsync).
int PreserveDiskInfo      = TRUE;
int NoWorkFlag            = FALSE;	// -0 flag
int DateCopyFlag          = FALSE;	// -d flag
int Verbose               = FALSE;
int BundleSize            = DEFAULT_BUNDLE_SIZE;
int ExistingOnlyFlag      = FALSE;	// -x

/*
** Default is to copy all securities from a File data source.
** Will be set to FALSE if security is specifies by -s
*/
int		CopyAllSecFromFile		= TRUE;

/*
**	Prototype functions
*/

int	main(						int argc, char *argv[] );

static void
		ShowUsage(				void ),
		Message(				const char *Msg );

static int
		SecCopy(				SDB_DB *DbSource, SDB_DB *DbTarget, FILE *FileDataDest, char *SecName ),
		SecCopyFromFile( 		FILE *FileDataSource, SDB_DB *DbTarget, char *SecName ),
		SecCopyStoredObject(	SDB_STORED_OBJECT *StoredObj, SDB_DB *DbTarget ),
		WildSecCopy(			SDB_DB *DbSource, SDB_DB *DbTarget, FILE *FileDataDest, char *SecName );



/****************************************************************
**	Routine: MathTrapHandler
**	Returns: None
**	Action : Handle math errors
****************************************************************/

static void MathTrapHandler(
	const char	*ErrText,
	int			Continuable
)
{
	if( Continuable )
	{
		fprintf( stderr, "%s\n\nProcessing will continue, but floating point results may be incorrect", ErrText );
		fprintf( stdout, "%s\n\nProcessing will continue, but floating point results may be incorrect", ErrText );
	}
	else
	{
		fprintf( stderr, "%s\n\nProcessing will NOT continue", ErrText );
		fprintf( stdout, "%s\n\nProcessing will NOT continue", ErrText );
	}
}


/****************************************************************
**	Routine: FormatDiskInfo
**	Returns: static formatted string
**	Action : pretty format disk info
****************************************************************/

static char *
FormatDiskInfo(
	SDB_DISK *Info
)
{
	static char buffer[ 256 ];

	char b1[80];

	sprintf( buffer, 
		"%-32.32s%-15.15s %-26.26s %3d",
		Info->Name,
		SecDbClassNameFromType( Info->Type ),
		TimeToString( b1, Info->TimeUpdated, NULL ),
		Info->UpdateCount );

	return( buffer );
}


/****************************************************************
**	Routine: main
**	Returns: None
**	Action : Copy securities
****************************************************************/

int main(
	int		argc,		// Command line argument count
	char	*argv[]		// Command line argument vectors
)
{
	SDB_SEC_TYPE
			SecType;

	SDB_DB
			*DbSource = NULL,
			*DbTarget = NULL;

	FILE	*SecurityFile	= 0;		// File of security names

	FILE	*FileDataDest = 0;
	FILE	*FileDataSource = 0;

	char	SecName[ SDB_SEC_NAME_SIZE ],
			*DbName,
			*Arg,
			LineBuf[ SDB_SEC_NAME_SIZE + 1 ],
			TransTitle[ 32 ],
			*UserName;

	int		ArgType 	= ARG_TYPE_SECTYPE,
			LineNumber	= 0,
			Failed		= FALSE,
			UseWildCards= FALSE,
			ArgNum,
			BufSize;


	TIMEZONE_SETUP();


/*
**	The first argument is the source database
*/
	UserName = getenv( "USER_NAME" );
	if (UserName == NULL)
		UserName = getenv( "USERNAME" );
	if( !SecDbStartup( "Null", SDB_DEFAULT_DEADPOOL_SIZE, Message, Message, "SecCopy", UserName))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		return 1;
	}

	if( argc-- > 1 )
	{
		DbName = *++argv;
		if( *DbName == '-' )
			ShowUsage();
		if( DbName[0] == '@' )
		{
			FileDataSource = fopen( DbName + 1, "rb" );
			if( !FileDataSource )
			{
				fprintf( stderr, "%s: %s\n", DbName+1, strerror( errno ) );
				return 1;
			}
		}
		else 
		{
			char *ReadOnlyDbName;

			
			ReadOnlyDbName = StrPaste( "ReadOnly{", DbName, "}", NULL );

			DbSource = SecDbAttach( ReadOnlyDbName, SDB_DEFAULT_DEADPOOL_SIZE, SDB_FRONT );

			free( ReadOnlyDbName );

			if( !DbSource )
			{
				fprintf( stderr, "%s\n", ErrGetString() );
				return 1;
			}			
		}
	}
	else
		ShowUsage();

/*
**	The second argument is the target database
*/

	if( argc-- > 1 )
	{
		DbName = *++argv;
		if( *DbName == '-' )
			ShowUsage();
		else if( *DbName == '@' )
		{
			if( FileDataSource )
			{
				fprintf( stderr, "%s: source is already a file\n", DbName+1 );
				return 1;
			}

			FileDataDest = fopen( DbName+1, "wb" );
			if( !FileDataDest )
			{
				fprintf( stderr, "%s: %s\n", DbName+1, strerror( errno ) );
				return 1;
			}
		}
		else if( !(DbTarget = SecDbAttach( DbName, SDB_DEFAULT_DEADPOOL_SIZE, SDB_FRONT | SDB_READWRITE )))
		{
			fprintf( stderr, "%s\n", ErrGetString() );
			return 1;
		}
	}
	else
		ShowUsage();


/*
**	Trap math errors
*/

	TrapMathError( MathTrapHandler );


/*
**	Wrap all the copies into a single transaction
*/

	BufSize = sizeof( TransTitle ) - sprintf( TransTitle, "seccopy" );
	for( ArgNum = 1; BufSize >= 2 && ArgNum < argc; ArgNum++ )
		BufSize -= sprintf( TransTitle + ( sizeof( TransTitle ) - BufSize ), " %.*s", BufSize - 2, argv[ ArgNum ] );

	if( DbTarget && !SecDbTransBegin( DbTarget, TransTitle ))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		return 1;
	}

/*
**	Parse the rest of the command line
*/

	while( argc-- > 1 )
	{
		Arg = *++argv;
		if( *Arg == '-' )
			switch( tolower( Arg[1] ))
			{
				case 'n':	// Assume don't copy for duplicates
					CopyFlags = COPY_FLAG_NO_DUPES;
					break;

				case 'a':	// Assume copy for duplicates
					CopyFlags = COPY_FLAG_COPY_DUPES;
					break;

				case 'r':
					ArgType = ARG_TYPE_RANGE;
					break;

				case 's':
					ArgType = ARG_TYPE_SECURITY;
					CopyAllSecFromFile = FALSE;
					break;

				case 'e':
					ArgType = ARG_TYPE_SECPREFIX;
					CopyAllSecFromFile = FALSE;
					break;

				case 't':
					ArgType = ARG_TYPE_SECTYPE;
					break;

				case '0':
					NoWorkFlag = TRUE;
					break;

				case 'd':
					DateCopyFlag = TRUE;
					break;

				case 'v':
					Verbose = TRUE;
					if( DbSource )
						printf( "Copying from source Db '%s'\n", DbSource->FullDbName.c_str() );
					if( DbTarget )
						printf( "Copying to target Db '%s'\n", DbTarget->FullDbName.c_str() );
					break;

				case 'f':
					ArgType = ARG_TYPE_SECURITY_LIST;
					break;

				case 'p':
					fprintf( stderr, "-p is now the default\n" );
					break;

				case 'q':	// Don't preserve source disk info for better TradSyncing
					if( DupSrcDbId )
					{
						fprintf( stderr, "ERROR: -q and -i cannot be combined\n" );
						return 1;
					}
					else 
						PreserveDiskInfo = FALSE;
					break;

				case 'i':
					if( !PreserveDiskInfo )
					{
						fprintf( stderr, "ERROR: -q and -i cannot be combined\n" );
						return 1;
					}
					else
						DupSrcDbId = TRUE;
					break;

				case 'w':	// SecNames are wildcards
					UseWildCards = TRUE;
					break;

				case 'b':
					ArgType = ARG_TYPE_BUNDLE_SIZE;
					break;

				case 'u':
					BundleSize = 1;
					break;

				case 'x':
					ExistingOnlyFlag = TRUE;
					break;

				default :
					ShowUsage();
			}

		else
		{
			switch( ArgType )
			{
				case ARG_TYPE_SECURITY:	// Individual security
					if( FileDataSource && DbTarget )
						Failed = !SecCopyFromFile( FileDataSource, DbTarget, Arg ) || Failed; 
					else 
					{
						if( UseWildCards )
							Failed = !WildSecCopy( DbSource, DbTarget, FileDataDest, Arg ) || Failed;
						else
							Failed = !SecCopy( DbSource, DbTarget, FileDataDest, Arg ) || Failed;
					}
					break;

				case ARG_TYPE_SECPREFIX:	// Securities with a specific prefix
					{
						char *Temp;
						
						Temp = ( char * ) malloc( strlen( Arg ) + 2 * sizeof( char ) );
						strcpy( Temp, Arg );
						strcat( Temp, "*" );

						printf( "Copying all Securities that match %s\n", Temp );
						if( FileDataSource && DbTarget )
							Failed = !SecCopyFromFile( FileDataSource, DbTarget, Temp ) || Failed; 
						else 
							Failed = !WildSecCopy( DbSource, DbTarget, FileDataDest, Temp ) || Failed;
					}
					break;

				case ARG_TYPE_SECTYPE:	// All securities of a certain type
					SecType = SecDbClassTypeFromName( Arg );
					if( !SecType )
					{
						fprintf( stderr, "Security class '%s' couldn't be found\n", Arg );
						Failed = TRUE;
					}
					else
					{
						*SecName = '\0';
						while( SecDbNameLookup( SecName, SecType, SDB_GET_GREATER, DbSource ))
						{
							Failed = !SecCopy( DbSource, DbTarget, FileDataDest, SecName ) || Failed;
						}
					}
					break;

				case ARG_TYPE_RANGE:
					{
						char	RangeBegin[ SDB_SEC_NAME_SIZE ],
								*RangeEnd;

						SDB_SEC_NAME_ENTRY
								SecNamesArray[ 100 ];

						int		GetType,
								Count,
								Elem,
								Done = FALSE;


						// Get beginning of range
						strncpy( RangeBegin, Arg, sizeof( RangeBegin ));
						RangeBegin[ SDB_SEC_NAME_SIZE - 1 ] = '\0';

						// Get end of range
						if( argc-- <= 1 )
							ShowUsage();
						RangeEnd = *++argv;

						// Get names from secdb
						GetType = SDB_GET_GE;
						while( 0 != ( Count = SecDbNameLookupMany( RangeBegin, SecNamesArray, sizeof( SecNamesArray ) / sizeof( *SecNamesArray ), 0, GetType, DbSource )))
						{
							for( Elem = 0; Elem < Count; Elem++ )
							{
								if( stricmp( RangeEnd, SecNamesArray[ Elem ] ) < 0 )
								{
									Done = TRUE;
									break;
								}
								Failed = !SecCopy( DbSource, DbTarget, FileDataDest, SecNamesArray[ Elem ] ) || Failed;
							}
							if( Done )
								break;
							GetType = SDB_GET_GREATER;
							strcpy( RangeBegin, SecNamesArray[ Count - 1 ] );
						}
					}
					break;

				case ARG_TYPE_SECURITY_LIST:	// File of security names
					// Open security name file
					if ( strcmp( Arg, "STDIN" ) == 0 )	// Means stdin
						SecurityFile = stdin;
					else
					{
						SecurityFile = fopen( Arg, "r" );
						if ( !SecurityFile )
						{
							fprintf( stderr, "Cannot open security file '%s'\n", Arg );
							return 13;
						}
					}

					// Read security names from file and copy them
					while ( fgets( LineBuf, SDB_SEC_NAME_SIZE+1, SecurityFile ) )
					{
						int	Last;

						LineNumber++;

						Last = strlen( LineBuf ) - 1;	// Punt trailing newline from fgets
						if ( Last >= 0 && LineBuf[Last] == '\n' )
							LineBuf[Last] = '\0';

						if ( *LineBuf == '\0' )		// Blank line -- quietly ignore it
							continue;

						if ( isspace( *LineBuf ) )	// Begins w/ whitespace - complain and ignore
						{
							fprintf( stderr,
							         "Line #%d in file '%s' begins with whitespace - ignoring it:\n[%s]\n",
							         LineNumber, Arg, LineBuf );
							continue;
						};
						Failed = !SecCopy( DbSource, DbTarget, FileDataDest, LineBuf ) || Failed;
					}
					break;

				case ARG_TYPE_BUNDLE_SIZE:
					if(( BundleSize = atoi( Arg )) <= 0 )
					{
						fprintf( stderr, "Invalid -b argument, '%s', must be positive integer\n", Arg );
						ShowUsage();
					}
					break;
			}
		}
	}

	if( FileDataSource && DbTarget && CopyAllSecFromFile )
		Failed = !SecCopyFromFile( FileDataSource, DbTarget, NULL ) || Failed;

/*
**	Close SecDb
*/

	if( DbTarget && !SecDbTransCommitForce( DbTarget ))
	{
		fprintf( stderr, "%s", ErrGetString() );
		Failed = TRUE;
	}
	if( DbSource ) SecDbDetach( DbSource );
	if( DbTarget ) SecDbDetach( DbTarget );
	return Failed;
}

/****************************************************************
**	Routine: DeltaTimeToString
**	Returns: passed buffer
**	Action : Convert a delta time to a human readable value
****************************************************************/

char *DeltaTimeToString(
	long	Delta,
	char	*Buffer
)
{
	static char
			TmpBuffer[ 64 ];

	int		Tmp;

	char	*s;


	if( Buffer == NULL )
		Buffer = TmpBuffer;
	s = Buffer;

	if( Delta < 0 )
	{
		Delta = -Delta;
		*s++ = '-';
	}

	if( Delta > 24L * 60L * 60L )
	{
		Tmp = (int) (Delta / (24L * 60L * 60L));
		Delta -= Tmp * 24L * 60L * 60L;
		if( Tmp > 1 )
			s += sprintf( s, "%d days ", Tmp );
		else
			s += sprintf( s, "%d day ", Tmp );
	}
	s += sprintf( s, "%02d:", Tmp = (int) (Delta / 60 / 60) );
	Delta -= Tmp * 60L * 60L;
	s += sprintf( s, "%02d:", Tmp = (int) (Delta / 60) );
	Delta -= Tmp * 60;
	s += sprintf( s, "%02d",  Tmp = (int) (Delta) );

	return Buffer;
}


/****************************************************************
**	Routine: SecCopy
**	Returns: None
**	Action : Copy a security from the source database to the
**			 target database
****************************************************************/

static int SecCopy(
	SDB_DB	*DbSource,
	SDB_DB	*DbTarget,
	FILE	*FileDataDest,
	char	*SecName
)
{
	static int
			Count		= 0,
			TotalCount	= 0;

	char	TransTitle[ 32 ];

	SDB_DISK
			TargetInfo,
			SourceInfo;

	int		Copy = FALSE,
			DeleteOld = FALSE,
			Key,
			AgeValid = FALSE,
			AddFlags = SDB_ADD_PRESERVE_DISK_INFO,
			Status = TRUE;

	long	Age = 0;	// >0 -> Target is newer

	if( !PreserveDiskInfo )	
		AddFlags &= ~SDB_ADD_PRESERVE_DISK_INFO;
	
	if( DupSrcDbId )
		AddFlags |= SDB_ADD_PRESERVE_SRC_DB_ID;

/*
**	Check to see if the security already exists in the target database
*/
	if(	!SecDbGetDiskInfo( &SourceInfo, SecName, DbSource, SDB_REFRESH_CACHE ))
	{
		fprintf( stderr, "Could not get diskinfo for '%s' because:\n%s\n", SecName, ErrGetString());
		return FALSE;
	}

	if( DbTarget && SecDbNameUsed( SecName, DbTarget ))
	{
		if( DateCopyFlag )
		{
			if( !SecDbGetDiskInfo( &TargetInfo, SecName, DbTarget, SDB_REFRESH_CACHE ))
			{
				fprintf( stderr, "Could not get diskinfo for '%s' because:\n%s\n", SecName, ErrGetString());
				return FALSE;
			}

			if( 	TargetInfo.DateCreated == SourceInfo.DateCreated )
			{
				AgeValid = TRUE;
				Age = TargetInfo.TimeUpdated;
				Age -= SourceInfo.TimeUpdated;
			}
			else
				AgeValid = FALSE;

			if( AgeValid && Age >= 0 )
			{
				if( Verbose )
				{
					printf( "%s: skipped, ", SecName );
					if( Age == 0 )
						printf( "identical objects\n" );
					else
						printf( "target newer by %s\n", DeltaTimeToString( Age, NULL ) );
				}
				return TRUE;
			}
		}

		if( CopyFlags == COPY_FLAG_NO_DUPES )
			Copy = FALSE;

		else if( CopyFlags == COPY_FLAG_COPY_DUPES )
			Copy = TRUE;

		else if( CopyFlags == COPY_FLAG_PROMPT_DUPES )
		{
			printf( "'%s' exists in target Db", SecName );
			if( DateCopyFlag )
            {
				if( AgeValid )
					printf( " (older by %s)", DeltaTimeToString( -Age, NULL ));
				else
					printf( " (different obj)" );
            }
			printf( ", copy anyway [y/n/a]?" );
			fflush( stdout );
			for(;;)
			{
				Key = getch();
				if( 'Y' == Key || 'y' == Key )
				{
					printf( "Yes\n" );
					Copy = TRUE;
					DeleteOld = TRUE;
					break;
				}
				else if( 'N' == Key || 'n' == Key )
				{
					printf( "No\n" );
					Copy = FALSE;
					break;
				}
				else if( 'A' == Key || 'a' == Key )
				{
					printf( "All\n" );
					CopyFlags = COPY_FLAG_COPY_DUPES;
					Copy = TRUE;
					break;
				}
			}
		}
	}
	else if( !ExistingOnlyFlag )
	{
		Copy = TRUE;
		DeleteOld = FALSE;
	}

	if( !Copy )
		return Status;

/*
**	Get from the source, put into the target
*/

	if( NoWorkFlag )
	{
		if( DeleteOld )
			printf( "%s\n", FormatDiskInfo( &SourceInfo ) );
		else
			printf( "%s\n", FormatDiskInfo( &SourceInfo ) );
		return TRUE;
	}
	
	if( DbTarget )
	{
		Status = Status && SecDbCopyByName( SecName, DbSource, DbTarget, NULL, AddFlags );
		if( Status && ++Count >= BundleSize )
		{
			sprintf( TransTitle, "seccopy [continued: %d...]", TotalCount + Count + 1 );
			Status = ( SecDbTransCommitForce( DbTarget ) && SecDbTransBegin( DbTarget, TransTitle ));
			if( Status )
			{
				TotalCount += Count;
				Count = 0;
			}
		}
	}
	else if ( FileDataDest )
	{
		Status = Status && SecDbCopyToFile( SecName, DbSource, FileDataDest );
	}

	if( Status )
		printf( "%s\n", FormatDiskInfo( &SourceInfo ) );
	else
		fprintf( stderr, "Failed to seccopy '%s'\n%s\n", SecName, ErrGetString( ));
	return Status;
}


/****************************************************************
**	Routine: SecCopyFromFile
**	Returns: None
**	Action : Stream a security from file, and insert into
**			 target database
****************************************************************/

static int SecCopyFromFile(
	FILE	*FileDataSource,
	SDB_DB	*DbTarget,
	char	*SecName
)
{
	int		Status = TRUE,
			SecNameFound = FALSE,
			CopyByPrefix = FALSE,
			CopyFinished = FALSE,
			SecNameLen = 0;
	
	SDB_STORED_OBJECT
			StoredObj;

	
	if( SecName )
	{
		SecNameLen = strlen( SecName );
		if ( SecName[ SecNameLen-1 ] == '*' )
		{
			SecName[ SecNameLen-1 ] = '\0';
			CopyByPrefix = TRUE;
		}
	}

	while( CopyFinished == FALSE )
	{
		SecNameFound = FALSE;
		if ( SecDbStoredObjectFromFile( FileDataSource, &StoredObj ) )
		{			
			if( CopyByPrefix )
			{
				if( SecName && ( strnicmp( SecName, StoredObj.SdbDisk.Name, SecNameLen - 1 ) == 0 ) )
				{
					SecNameFound = TRUE;
				}
			}
			else
			{
				if( SecName && ( stricmp( SecName, StoredObj.SdbDisk.Name ) == 0 ) )
				{
					SecNameFound = TRUE;
					CopyFinished = TRUE;
				}
			}

			if( SecNameFound || !SecName )
			{
				if( !SecCopyStoredObject( &StoredObj, DbTarget ) )
				{
					Status = FALSE;
					free( StoredObj.Mem );
					break;
				}
			}
			free( StoredObj.Mem );
		}
		else
		{	
			CopyFinished = TRUE;
		}
	}
	return( Status );
}


/****************************************************************
**	Routine: SecCopyStoredObject
**	Returns: TRUE/FALSE
**	Action : Copy stored object to target DB
****************************************************************/

static int SecCopyStoredObject(
	SDB_STORED_OBJECT	*StoredObj,
	SDB_DB				*DbTarget
)
{
	static int
			Count		= 0,
			TotalCount	= 0;

	char	TransTitle[ 32 ];

	SDB_DISK
			TargetInfo;

	int		Copy = FALSE,
			DeleteOld = FALSE,
			Key,
			AgeValid = FALSE,
			Status = TRUE;

	long	Age = 0;	// >0 -> Target is newer

/*
**	Check to see if the security already exists in the target database
*/

	if( SecDbNameUsed( StoredObj->SdbDisk.Name, DbTarget ))
	{
		if( DateCopyFlag )
		{
			if( 	!SecDbGetDiskInfo( &TargetInfo, StoredObj->SdbDisk.Name, DbTarget, SDB_REFRESH_CACHE ))
			{
				fprintf( stderr, "Could not get diskinfo for '%s' because:\n%s\n", StoredObj->SdbDisk.Name, ErrGetString());
				return FALSE;
			}

			if( TargetInfo.DateCreated == StoredObj->SdbDisk.DateCreated )
			{
				AgeValid = TRUE;
				Age = TargetInfo.TimeUpdated;
				Age -= StoredObj->SdbDisk.TimeUpdated;
			}
			else
				AgeValid = FALSE;

			if( AgeValid && Age >= 0 )
			{
				if( Verbose )
				{
					printf( "%s: skipped, ", StoredObj->SdbDisk.Name );
					if( Age == 0 )
						printf( "identical objects\n" );
					else
						printf( "target newer by %s\n", DeltaTimeToString( Age, NULL ) );
				}
				return TRUE;
			}
		}

		if( CopyFlags == COPY_FLAG_NO_DUPES )
			Copy = FALSE;

		else if( CopyFlags == COPY_FLAG_COPY_DUPES )
			Copy = TRUE;

		else if( CopyFlags == COPY_FLAG_PROMPT_DUPES )
		{
			printf( "'%s' exists in target Db", StoredObj->SdbDisk.Name );
			if( DateCopyFlag )
            {
				if( AgeValid )
					printf( " (older by %s)", DeltaTimeToString( -Age, NULL ));
				else
					printf( " (different obj)" );
            }
			printf( ", copy anyway [y/n/a]?" );
			fflush( stdout );
			for(;;)
			{
				Key = getch();
				if( 'Y' == Key || 'y' == Key )
				{
					printf( "Yes\n" );
					Copy = TRUE;
					DeleteOld = TRUE;
					break;
				}
				else if( 'N' == Key || 'n' == Key )
				{
					printf( "No\n" );
					Copy = FALSE;
					break;
				}
				else if( 'A' == Key || 'a' == Key )
				{
					printf( "All\n" );
					CopyFlags = COPY_FLAG_COPY_DUPES;
					Copy = TRUE;
					break;
				}
			}
		}
	}
	else if( !ExistingOnlyFlag )
	{
		Copy = TRUE;
		DeleteOld = FALSE;
	}

	if( !Copy )
		return Status;

/*
**	Get from the source, put into the target
*/

	if( NoWorkFlag )
	{
		if( DeleteOld )
			printf( "%s\n", FormatDiskInfo( &StoredObj->SdbDisk ) );
		else
			printf( "%s\n", FormatDiskInfo( &StoredObj->SdbDisk ) );
		return TRUE;
	}

	Status = SecDbAddStoredObject( StoredObj, DbTarget );

	if( Status && ++Count >= BundleSize )
	{
		sprintf( TransTitle, "seccopy [continued: %d...]", TotalCount + Count + 1 );
		Status = ( SecDbTransCommitForce( DbTarget ) && SecDbTransBegin( DbTarget, TransTitle ));
		if( Status )
		{
			TotalCount += Count;
			Count = 0;
		}
	}

	if( Status )
		printf( "%s\n", FormatDiskInfo( &StoredObj->SdbDisk ) );
	else
		fprintf( stderr, "Failed to seccopy '%s'\n%s\n", StoredObj->SdbDisk.Name, ErrGetString( ));
	return Status;
}


/****************************************************************
**	Routine: WildSecCopy
**	Returns: None
**	Action : Copy a securities from the source database to the
**			 target database
****************************************************************/

static int WildSecCopy(
	SDB_DB	*DbSource,
	SDB_DB	*DbTarget,
	FILE	*FileDataDest,
	char	*Pattern
)
{
	char	LowName[	SDB_SEC_NAME_SIZE ],
			HighName[	SDB_SEC_NAME_SIZE ],
			*SecName;

	int		Status = TRUE;

	SDB_SEC_TYPE
			SecType = 0;

	SDB_SEC_NAME_ENTRY
			*SecNames;

	int		GetType,
			NameNumber,
			NamesInBuffer,
			Tmp;


	// Compute Low & High names to limit range to check
	strncpy( LowName, Pattern, SDB_SEC_NAME_SIZE );
	LowName[ SDB_SEC_NAME_SIZE-1 ] = '\0';

	// Find first MAGIC character (if no ? or *, just call seccopy)
	Tmp = strcspn( LowName, "?*" );
	if( LowName[ Tmp ] == '\0' )
		return SecCopy( DbSource, DbTarget, FileDataDest, LowName );
	LowName[ Tmp ] = '\0';

	// HighName is LowName~~~~~~~~~~~~~
	memset( HighName, '~', SDB_SEC_NAME_SIZE );
	HighName[ SDB_SEC_NAME_SIZE-1 ] = '\0';
	strncpy( HighName, LowName, strlen( LowName ));


	// Loop from LowName to HighName and check against pattern
	SecNames = (SDB_SEC_NAME_ENTRY *) malloc( sizeof( SDB_SEC_NAME_ENTRY ) * MAX_NAMES_TO_BUFFER );
	GetType  = SDB_GET_GE;
	SecName  = LowName;
	while( 0 != ( NamesInBuffer = SecDbNameLookupMany( SecName, SecNames, MAX_NAMES_TO_BUFFER, SecType, GetType, DbSource )))
	{
		for( NameNumber = 0; NameNumber < NamesInBuffer; NameNumber++ )
		{
			SecName = SecNames[ NameNumber ];
			if( stricmp( SecName, HighName ) > 0 )
				goto DoneWhile;
			if( !StrWildMatch( Pattern, SecName ))
				continue;

			if( !SecCopy( DbSource, DbTarget, FileDataDest, SecName ))
				Status = FALSE;
		}

		// Save one I/O if we're at the end of the database, or greater than
		// the optional upper bound
		if( NameNumber < MAX_NAMES_TO_BUFFER )
			break;

		GetType = SDB_GET_GREATER;
	}
DoneWhile:
 	free( SecNames );

 	return Status;
}



/****************************************************************
**	Routine: ShowUsage
**	Returns: None
**	Action : Display usage of SecCheck
****************************************************************/

static void ShowUsage( void )
{
	fprintf( stderr,
			"Usage: SecCopy Source Target [Copy Options] [-s Security Name ] [-t Security Class] ...\n"
			"\tCOPY OPTIONS\n"
			"\t-0                   Show only - don't copy anything\n"
			"\t-a                   Copy all duplicates without prompting\n"
			"\t-b BundleSize        Number of copies in each transaction [default %d]\n"
			"\t-d                   Turn on date comparison\n"
			"\t-n                   Copy only non-duplicate objects\n"
			"\t-q                   Don't preserve disk info (times/db/count)\n"
			"\t-i                   Duplicate source db id (used to mimic secsync behavior)\n"
			"\t-u                   Unbundled - same as -b 1\n"
			"\t-v                   Turn on verbose output\n"
			"\t-w                   *SecNames can contain wild cards (* & ?)\n"
			"\t-x                   Copy existing securities only\n"
			"\n"
			"\tCONTENT SWITCHES**\n"
			"\t-e Security Prefix   Copy all securities that begin with a prefix\n" 
			"\t                     (same as -w -s SecurityPrefix*)\n"
			"\t-r First Last        *Copy a range of securities\n"
			"\t-s Security Name     Copy an individual security\n"
			"\t-t Security Class    *Copy an entire class of securities\n"
			"\n"
			"\tSource and Target are database names or filenames;\n"
			"\tfilenames are indicated by a prepended '@' character\n"
			"\n"
			"\t*  This option is not available for copying when source is a file\n"
			"\t** If no option is specified and source is a file, ALL securities\n"
			"\t   in that file will be copied\n" 
		   , DEFAULT_BUNDLE_SIZE );

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

