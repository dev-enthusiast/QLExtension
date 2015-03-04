#define GSCVSID "$Header: /home/cvs/src/secdb/src/seccheck.c,v 1.25 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	SECCHECK.C	- Program to validate security database objects
**
**	$Log: seccheck.c,v $
**	Revision 1.25  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.24  2010/10/25 19:13:19  c03833
**	Include ccmain.h to handle errors for the entire application
**
**	Revision 1.23  2000/06/07 14:05:28  goodfj
**	Fix includes
**
**	Revision 1.22  1999/08/07 01:40:14  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.21  1999/05/21 21:13:01  gribbg
**	Const correctness for TrapMathError
**	
**	Revision 1.20  1998/05/20 19:40:42  brownb
**	fix gcc 2.8.1 warning -- main() should return int
**	
**	Revision 1.19  1998/01/02 21:33:22  rubenb
**	added GSCVSID
****************************************************************/

#define INCL_TIMEZONE
#include	<portable.h>
#include	<limits.h>
#include	<trap.h>
#include	<heapinfo.h>
#include	<err.h>
#include	<ccmain.h>
#include	<secdb.h>


/*
**	Define constants
*/

#define	ARG_TYPE_SECURITY		1
#define	ARG_TYPE_SECTYPE		2
#define	ARG_TYPE_VALUE_COUNT	3
#define	ARG_TYPE_SECURITY_COUNT	4

#define	FLAGS_CLEAR_DEADPOOL	0x0001
#define	FLAGS_VALIDATE			0x0002


/*
**	Variables
*/

SDB_DB  *SdbPublic;

int		ErrorCount		= 0,
		WarningCount	= 0,
		MaxValueCount	= INT_MAX;


/*
**	Prototype functions
*/

int	main(					int argc, char *argv[] );

static void
		SecCheck(				char *SecName, unsigned int Flags ),
		SecCheckShowSizes(		void ),
		SecCheckShowClasses(	void ),
		ShowUsage(				void ),
		Message(				const char *Msg ),
		MathTrapHandler(		const char *ErrText, int Continuable );


/****************************************************************
**	Routine: main
**	Returns: None
**	Action : Validate security fitness
****************************************************************/

int main(
	int		argc,		// Command line argument count
	char	*argv[]		// Command line argument vectors
)
{
	SDB_SEC_TYPE
			SecType;

	char	SecName[ SDB_SEC_NAME_SIZE ];

	unsigned int
			ArgType		= ARG_TYPE_SECTYPE,
			Count,
			MaxSecCount	= INT_MAX,
			Flags		= FLAGS_CLEAR_DEADPOOL | FLAGS_VALIDATE;

	char	*Arg,
			*DatabaseName;


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

	if( !(SdbPublic = SecDbStartup( DatabaseName, SDB_DEFAULT_DEADPOOL_SIZE, Message, Message, "SecCheck", getenv( "USER_NAME" ))))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		exit( 1 );
	}


/*
**	Trap math errors
*/

	TrapMathError( MathTrapHandler );


/*
**	Parse the command line
*/

	while( argc-- > 1 )
	{
		Arg = *++argv;
		if( *Arg == '-' )
			switch( tolower( Arg[1] ))
			{
				case 'd':	Flags ^= FLAGS_CLEAR_DEADPOOL;			break;
				case 'm':	ArgType = ARG_TYPE_SECURITY_COUNT;		break;
				case 'n':	ArgType = ARG_TYPE_VALUE_COUNT;			break;
				case 'l':	SecCheckShowClasses();					break;
				case 's':	ArgType = ARG_TYPE_SECURITY;			break;
				case 't':	ArgType = ARG_TYPE_SECTYPE;				break;
				case 'v':	Flags ^= FLAGS_VALIDATE;				break;
				case 'z':	SecCheckShowSizes();					break;
				default :	ShowUsage();
			}

		else
		{
			switch( ArgType )
			{
				case ARG_TYPE_VALUE_COUNT:
					MaxValueCount = atoi( Arg );
					break;

				case ARG_TYPE_SECURITY_COUNT:
					MaxSecCount = atoi( Arg );
					break;

				case ARG_TYPE_SECURITY:	// Individual security
					SecCheck( Arg, Flags );
					break;

				case ARG_TYPE_SECTYPE:	// All securities of a certain type
					SecType = SecDbClassTypeFromName( Arg );
					if( !SecType )
						fprintf( stderr, "Security class '%s' couldn't be found\n", Arg );
					else
					{
						*SecName = '\0';
						Count = 0;
						while( SecDbNameLookup( SecName, SecType, SDB_GET_GREATER, SdbPublic ) && ( ++Count <= MaxSecCount ))
							SecCheck( SecName, Flags );
					}
					break;
			}
		}
	}


/*
**	Print totals
*/

	printf( "\n\nError Count  : %d\nWarning Count: %d\n",
			ErrorCount,
			WarningCount );


/*
**	Shutdown SecDb
*/

	SecDbShutdown( TRUE );

	return 0;
}



/****************************************************************
**	Routine: SecCheck
**	Returns: TRUE	- Security passed the check process
**			 FALSE	- Security failed the check process
**	Action : Validate a security
****************************************************************/

static void SecCheck(
	char			*SecName,	// Name of security to check
	unsigned int	Flags		// Control flags
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_STATISTICS
			Stats;

	SDB_VALUE_TYPE_INFO
			*ValueTypeInfo;

	SDB_ENUM_PTR
			EnumPtr;

	SDB_VALIDATE
			*Validate;

	unsigned long
			MemInUseBeforeLoad,
			MemInUseAfterLoad,
			MemInUseAfterValues,
			MemInUseAfterMoreValues,
			MemInUseAfterFree,
			MemInUseAfterReFree,
			Bytes;

	long	RefCountBeforeLoad;

	int		Try,
			ValueCount;


	SecDbStatistics( &Stats, SdbPublic );
	RefCountBeforeLoad = Stats.ObjectRefCount;
	MemInUseBeforeLoad = GSHeapUsed();
	printf( "[%s]:\n", SecName );


/*
**	Load the security
*/

	SecPtr = SecDbGetByName( SecName, SdbPublic, 0 );
	MemInUseAfterLoad = GSHeapUsed();


/*
**	Enumerate the value types for a security, and try to get each value
*/

	if( !SecPtr )
	{
		printf( "\tError --- Couldn't be loaded: %s\n", ErrGetString() );
		ErrorCount++;
	}
	else
	{
		for( ValueCount = 0, ValueTypeInfo = SecDbValueTypeEnumFirst( SecPtr, &EnumPtr );
				( ValueCount < MaxValueCount ) && ValueTypeInfo;
				ValueCount++, ValueTypeInfo = SecDbValueTypeEnumNext( EnumPtr ))
		{
			if( !SecDbGetValue( SecPtr, ValueTypeInfo->ValueType ))
			{
				printf( "\tWarning --- Value '%s' failed: %s\n",
						SecDbValueNameFromType( ValueTypeInfo->ValueType ),
						ErrGetString() );
				WarningCount++;
			}
		}

		if( ValueCount == 0 )
		{
			printf( "\tWarning --- No value types are supported\n" );
			WarningCount++;
		}

		if( ValueTypeInfo )
			printf( "\tValue checking stopped at %s\n", ValueTypeInfo->ValueType->Name );
		SecDbValueTypeEnumClose( EnumPtr );
		// FIX -- SecDbRemoveCachedValue( SecPtr, NULL );

		// Validate
		if( Flags & FLAGS_VALIDATE )
		{
			for( Validate = SecDbValidateEnumFirst( SecPtr, &EnumPtr );
				 Validate;
				 Validate = SecDbValidateEnumNext( EnumPtr ))
			{
				if( Validate->Flags & SDB_VALIDATE_WARNING )
				{
					printf( "\tValidate Warning --- " );
					WarningCount++;
				}
				else if( Validate->Flags & SDB_VALIDATE_ERROR )
				{
					printf( "\tValidate Error --- " );
					ErrorCount++;
				}

				if( Validate->ValueType )
					printf( "%s", Validate->ValueType->Name );

				if( Validate->Flags & SDB_VALIDATE_MISSING )
					printf( " missing" );
				else if( Validate->Flags & SDB_VALIDATE_INVALID )
					printf( " invalid" );
				else if( Validate->Flags & SDB_VALIDATE_RANGE )
					printf( " out of range" );

				if( Validate->Text )
					printf( " -- %s", Validate->Text );

				printf( "\n" );

			}

			SecDbValidateEnumClose( EnumPtr );
		}
	}

	MemInUseAfterValues = GSHeapUsed();
	SecDbFree( SecPtr );
	if( Flags & FLAGS_CLEAR_DEADPOOL )
		SecDbRemoveFromDeadPool( SecDbRootDb, 0, NULL );
	MemInUseAfterFree = GSHeapUsed();


/*
**	Reload the security to see if everything is O.K.
*/


	SecPtr = SecDbGetByName( SecName, SdbPublic, 0 );


/*
**	Enumerate the value types for a security, and try to get each value
*/

	if( !SecPtr )
	{
		printf( "\tError --- Couldn't be reloaded: %s\n", ErrGetString() );
		ErrorCount++;
	}
	else
	{
		for( Try = 2; Try; Try-- )
		{
			for( ValueCount = 0, ValueTypeInfo = SecDbValueTypeEnumFirst( SecPtr, &EnumPtr );
					(ValueCount < MaxValueCount) && ValueTypeInfo;
					ValueCount++, ValueTypeInfo = SecDbValueTypeEnumNext( EnumPtr ))
			{
				if( !SecDbGetValue( SecPtr, ValueTypeInfo->ValueType ))
				{
					printf( "\tWarning --- Value '%s' failed after reload: %s\n",
							SecDbValueNameFromType( ValueTypeInfo->ValueType ),
							ErrGetString() );
					WarningCount++;
				}

			}
			SecDbValueTypeEnumClose( EnumPtr );
			// FIX -- SecDbRemoveCachedValue( SecPtr, NULL );
		}
	}

	MemInUseAfterMoreValues = GSHeapUsed();


/*
**	Free the security
*/

	SecDbFree( SecPtr );
	if( Flags & FLAGS_CLEAR_DEADPOOL )
		SecDbRemoveFromDeadPool( SecDbRootDb, 0, NULL );
	MemInUseAfterReFree = GSHeapUsed();


/*
**	Determine how the object fared with memory and reference count
*/

	if( MemInUseAfterFree != MemInUseAfterReFree )
	{
		Bytes = (MemInUseAfterMoreValues - MemInUseAfterReFree) - (MemInUseAfterValues - MemInUseAfterFree );
		if( Bytes )
			printf( "\tWarning --- There was a %ld byte difference between valuing\n", Bytes );
		printf( "\tWarning --- There was a %ld byte difference between freeing\n",
				MemInUseAfterReFree - MemInUseAfterFree );
		WarningCount++;
	}

	SecDbStatistics( &Stats, SdbPublic );
	if( Stats.ObjectRefCount != RefCountBeforeLoad )
	{
		printf( "\tWarning --- There was a %ld difference in the reference count\n", Stats.ObjectRefCount - RefCountBeforeLoad );
		WarningCount++;
	}

	printf( "\t%5ld bytes used in loading       %5ld bytes used in valuing\n",
			MemInUseAfterLoad - MemInUseBeforeLoad,
			MemInUseAfterValues - MemInUseAfterLoad );
}



/****************************************************************
**	Routine: SecCheckShowSizes
**	Returns: None
**	Action : Display the sizes of various SecDb structures and
**			 unions
****************************************************************/

static void SecCheckShowSizes( void )
{
	printf( "SDB_CHILD_INFO              : %ld\n", sizeof( SDB_CHILD_INFO				));
	printf( "SDB_CLASS_INFO              : %ld\n", sizeof( SDB_CLASS_INFO				));
	printf( "SDB_DISK                    : %ld\n", sizeof( SDB_DISK				   		));
	printf( "SDB_ENUM_STRUCT             : %ld\n", sizeof( SDB_ENUM_STRUCT		   		));
	printf( "SDB_OBJECT                  : %ld\n", sizeof( SDB_OBJECT			   		));
	printf( "SDB_STATISTICS              : %ld\n", sizeof( SDB_STATISTICS				));
	printf( "SDB_VALUE_TYPE_STRUCTURE    : %ld\n", sizeof( SDB_VALUE_TYPE_STRUCTURE		));
	printf( "DT_DATA_TYPE_STRUCTURE      : %ld\n", sizeof( DT_DATA_TYPE_STRUCTURE		));
	printf( "DT_VALUE                    : %ld\n", sizeof( DT_VALUE			   		));
	printf( "DT_VALUE_RESULTS            : %ld\n", sizeof( DT_VALUE_RESULTS	   		));
	printf( "DT_CURVE                    : %ld\n", sizeof( DT_CURVE			   		));
	printf( "DT_CURVE_KNOT               : %ld\n", sizeof( DT_CURVE_KNOT		   		));
	printf( "\n\n" );
}



/****************************************************************
**	Routine: SecCheckShowClasses
**	Returns: None
**	Action : List security classes
****************************************************************/

static void SecCheckShowClasses( void )
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
**	Action : Display usage of SecCheck
****************************************************************/

static void ShowUsage( void )
{
	fprintf( stderr, "Usage: SecCheck [Database] [-s Security Name ] [-t Security Class] [-z] ...\n" );
	fprintf( stderr, "\t-d                   Toggle deadpool clearing\n" );
	fprintf( stderr, "\t-l                   List security classes\n" );
	fprintf( stderr, "\t-m Security Count    Max of m securities with each -t class\n" );
	fprintf( stderr, "\t-n Value Count       Only get first n values\n" );
	fprintf( stderr, "\t-s Security Name     Check an individual security\n" );
	fprintf( stderr, "\t-t Security Class    Check an entire class of securities\n" );
	fprintf( stderr, "\t-v                   Toggle detailed validation\n" );
	fprintf( stderr, "\t-z                   Display structure sizes\n" );

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
