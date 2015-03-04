#define GSCVSID "$Header: /home/cvs/src/secdb/src/secexpr.c,v 1.89 2012/02/07 17:32:21 khodod Exp $"
/****************************************************************
**
**	SECEXPR.C	-- SecDb expression evaluation program
**
**	$Log: secexpr.c,v $
**	Revision 1.89  2012/02/07 17:32:21  khodod
**	secview.dll -> sd_secview.dll.
**	AWR: #257683
**
**	Revision 1.88  2006/07/14 20:03:14  khodod
**	Slang Abort() should return FALSE!
**	iBug: #37707
**
**	Revision 1.87  2001/11/27 23:23:44  procmon
**	Reverted to CPS_SPLIT.
**	
****************************************************************/

#define INCL_TIMEZONE
#define INCL_FILEIO
#include	<portable.h>
#include	<hash.h>
#include	<dynalink.h>
#include	<trap.h>
#include	<err.h>
#include	<secexpr.h>
#include	<secdb/sdb_exception.h>
#include	<secview.h>
#include	<secprof.h>
#include	<kool_ade.h>
#include	<window.h>
#include	<wevent.h>
#if defined (WIN32)
#include	<windows.h>
#endif
#include	<ccmain.h>


/*
**	Define constants
*/

int const ARG_TYPE_INVALID		= 0;
int const ARG_TYPE_SECURITY		= 1;
int const ARG_TYPE_FILENAME		= 2;
int const ARG_TYPE_EXPRESSION	= 3;
int const ARG_TYPE_PROFILER		= 4;
int const ARG_TYPE_TRACE		= 5;
int const ARG_TYPE_TIMER		= 6;
int const ARG_TYPE_SOURCEDB		= 7;

int const FLAG_COMMAND_PROC		= 0x0001;
int const FLAG_COMMAND_PROF		= 0x0002;
int const FLAG_COMMAND_QUIET	= 0x0004;
int const FLAG_COMMAND_NORES	= 0x0008;
int const FLAG_COMMAND_WINDOW	= 0x0010;
int const FLAG_COMMAND_RETURN	= 0x0020;
int const FLAG_COMMAND_PROMPT	= 0x0040;


/*
**	Variables
*/

SDB_DB	*SdbPublic;

char	Buffer[ 4096 ],
		*ProfilerFile;

int		ControlFlags		= 0,
		SomethingWasPrinted	= FALSE,
		ProgramReturnCode	= 0,
		AbortRequested		= FALSE,
		FastAndLoose		= TRUE;

static FILE
		*TraceFile = NULL;


/*
**	Pointer to functions dynalinked in from secview
*/

typedef	void	(*PF_SEC_VIEW_INIT_WINDOWS)(		void );
typedef	void	(*PF_SEC_VIEW_KILL_WINDOWS)(		void );
typedef	void	(*PF_SEC_VIEW_MESSAGE)(				char const* Msg );
typedef	void	(*PF_SEC_VIEW_ERROR_MESSAGE)(		char const* Msg );
typedef	void	(*PF_SEC_VIEW_SLANG_SET_CONTEXT)(	void );
typedef	int     (*PF_SEC_VIEW_AVAILABLE)(			void );

PF_SEC_VIEW_INIT_WINDOWS
		pfSecViewInitWindows;

PF_SEC_VIEW_KILL_WINDOWS
		pfSecViewKillWindows;

PF_SEC_VIEW_MESSAGE
		pfSecViewMessage;

PF_SEC_VIEW_ERROR_MESSAGE
		pfSecViewErrorMessage;

PF_SEC_VIEW_SLANG_SET_CONTEXT
		pfSecViewSlangSetContext;

PF_SEC_VIEW_AVAILABLE
		pfSecViewAvailable;

int		EnableEventQueue = FALSE;

/*
**	Prototype functions
*/

static void
		Message(				const char *Msg ),
		ErrorMessage(			const char *Msg ),
		DisplaySlangError(		const char *Type ),
		ShowUsage(				void ),
		PrintProfilerStatistics(SLANG_NODE *Expression ),
		MathTrapHandler(		const char *ErrText, int Continuable ),
		AbortTrapHandler(		void ),
		UseSecViewFunctions(	void );

static int
		EvaluateAndShowResults(	SLANG_NODE *Root, SLANG_EVAL_FLAG EvalFlag, SLANG_RET *Ret, SLANG_SCOPE *Scope, int TimeItLoops ),
		PrintFunction(			void *Handle, char *String ),
		OutputBufferSetFore(	void	*Handle,	long Color ),
		OutputBufferSetBack(	void	*Handle,	long Color );

static void
		ErrorTraceFunc(			ERR_NUM ErrNum, ERR_C_CHAR *ErrText );


/****************************************************************
**	Routine: main
**	Returns: None
**	Action : Main module of command line script evaluator
****************************************************************/

int main(
	int		argc,		// Command line argument count
	char	*argv[]		// Command line argument vectors
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_VALUE_TYPE
			ValueTypeExpression;

	DT_VALUE
			ArrayValue;

	SLANG_NODE
			*Expression;

	SLANG_RET
			Ret;

	SLANG_SCOPE
			*Scope;
			
	DT_VALUE
			*Script;

	int		ArgType			= ARG_TYPE_INVALID,
			InteractiveMode	= TRUE,
			ExitWithError	= FALSE,
			TimeItLoops		= 0,
			ArgNum;

	char	FileName[ 256 ],
			*Arg,
			*DatabaseName,
			*UserName,
			*ProcmonProcessName,
			*Scratch = NULL;

	const char
			*AppName = "SecExpr";


	TIMEZONE_SETUP();


/*
**	Startup SecDb
*/

	// Check for database name
	if( argc > 1 && *argv[1] != '-' )
	{
		DatabaseName = *++argv;
		--argc;
	}
	else
		DatabaseName = NULL;

	if ((UserName = getenv( "USER_NAME" )) == NULL)
		UserName = getenv( "USERNAME" );
	if( ( ProcmonProcessName = getenv( "PROCMON_PROCESS" ) ) )
		AppName = ProcmonProcessName;

	// Check for app name which must immediately follow Database Name
	if( argc > 2 && stricmp( argv[1], "-a" ) == 0 )
	{
		AppName = argv[ 2 ];
		argv += 2;
		argc -= 2;
	}
	if( !(SdbPublic = SecDbStartup( DatabaseName, SDB_DEFAULT_DEADPOOL_SIZE, Message, ErrorMessage, AppName, UserName)))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		return 1;
	}


/*
**	Trap math errors
*/

	TrapMathError( MathTrapHandler );


/*
**	Trap fatal signals
*/

	TrapFatalSignals();


/*
**	Setup context & scope
*/

	SlangContextSet( "Print Handle",	(void *) stdout			);
	SlangContextSet( "Print Function",	(void *) PrintFunction	);
	SlangContextSet( "OutputBufferSetFore",	(void *) OutputBufferSetFore );
	SlangContextSet( "OutputBufferSetBack",	(void *) OutputBufferSetBack );

	Scope = SlangScopeCreate();


/*
**	Create an Argv variable
*/

	DTM_ALLOC( &ArrayValue, DtArray );
	SlangVariableSet( "Argv", &ArrayValue, NULL, FALSE );
	ArgNum = 0;


/*
**	Parse the command line
*/

	while( argc-- > 1 )
	{
		Arg = *++argv;
		if( *Arg == '-' )
			switch( tolower( Arg[1] ))
			{
				case 'c':	TrapAbort( AbortTrapHandler );			break;
				case 'd':	SlangVariableDestroyByName( NULL, NULL );break;
				case 'e':	ArgType = ARG_TYPE_EXPRESSION;			break;
				case 'f':	ArgType = ARG_TYPE_FILENAME;			break;
				case 'i':
				  /*
				   *  Send all messages to standard output, and prompt the
				   *  user for input.
				   */
				  ControlFlags ^= FLAG_COMMAND_PROMPT;
				  break;
				case 'j':   
					EnableEventQueue = TRUE; 
					w_EventSetup(); 
					break;
				case 'l':	ControlFlags ^= FLAG_COMMAND_RETURN;	break;
				case 'p':	ArgType = ARG_TYPE_PROFILER;			break;
				case 'q':	ControlFlags ^= FLAG_COMMAND_QUIET;		break;
				case 'r':	ControlFlags ^= FLAG_COMMAND_NORES;		break;
				case 's':	if( !stricmp( Arg + 1, "source" ) )
								ArgType = ARG_TYPE_SOURCEDB;
							else
								ArgType = ARG_TYPE_SECURITY;
							break;
				case 't':	
					if( Arg[2] == '\0' )
					{
						TraceFile	= stderr; 
						ArgType		= ARG_TYPE_TRACE; 
						ErrHookFunc	= ErrorTraceFunc; 
					}	
					else if( stricmp( Arg, "-timeit" ) == 0 )
					{
						ArgType 	= ARG_TYPE_TIMER;
						TimeItLoops	= (int) 1e6;
					}
					else
						ShowUsage();
					break;
				case 'w':	UseSecViewFunctions();					break;
				case 'x':	ControlFlags ^= FLAG_COMMAND_PROC;		break;
				case 'z':	FastAndLoose = !FastAndLoose;			break;
				default :	ShowUsage();
			}

		else
		{
			switch( ArgType )
			{
				case ARG_TYPE_TIMER:
					TimeItLoops = (int) atof( Arg );
					continue;
	
				case ARG_TYPE_TRACE:
					if( !( TraceFile = fopen( Arg, "w" )))
					{
						fprintf( stderr, "Cannot open %s: %s", Arg, strerror( errno ));
						ShowUsage();
					}
					ArgType = ARG_TYPE_INVALID;
					continue;

				case ARG_TYPE_SOURCEDB:
				{
					if( !SecDbSourceDbSet( Arg ) )
					{
						fprintf( stderr, "Failed to set SourceDb to %s: %s\n", Arg, ErrGetString() );
						ExitWithError = TRUE;
						break;
					}
					ArgType = ARG_TYPE_INVALID;
					continue;
				}
				case ARG_TYPE_SECURITY:
				{
					SDB_DB *SourceDb = SecDbSourceDbGet();

					
					if( SourceDb )
						SecPtr = SecDbGetByName( Arg, SourceDb, 0 );
					if( SourceDb && SecPtr )
					{
						ValueTypeExpression = SecDbValueTypeFromName( "Expression", NULL );
						if( !( Script = SecDbGetValue( SecPtr, ValueTypeExpression )))
						{
							fprintf( stderr, "Error in %s: %s\n", Arg, ErrGetString() );
							if( ControlFlags & FLAG_COMMAND_RETURN )
								ExitWithError = TRUE;
						}
						else
						{
							Expression = SlangParse( (char *) Script->Data.Pointer, Arg, SLANG_MODULE_OBJECT );
							if( Expression )
							{
								if( !EvaluateAndShowResults( Expression, SLANG_EVAL_RVALUE, &Ret, Scope, TimeItLoops )
										&& ( ControlFlags & FLAG_COMMAND_RETURN ) )
									ExitWithError = TRUE;
								SlangFree( Expression );
							}
							else
							{
								DisplaySlangError( "Parsing" );
								if( ControlFlags & FLAG_COMMAND_RETURN )
									ExitWithError = TRUE;
							}
						}
						SecDbFree( SecPtr );
					}
					else
					{
						fprintf( stderr, "%s\n", ErrGetString() );
						if( ControlFlags & FLAG_COMMAND_RETURN )
							ExitWithError = TRUE;
					}
					break;
				}

				case ARG_TYPE_FILENAME:
					if( ControlFlags & FLAG_COMMAND_PROC )
					{
						FILE	*fpSource,
								*fpTarget;

						int		HeaderRemoved;

						char	*Path,
								*Directory;


						if( !(fpSource = fopen( Arg, "r" )))
						{
							// Search the path...
							Path = strdup( getenv( "PATH" ));
							for( Directory = strtok( Path, ";" ); Directory; Directory = strtok( NULL, ";" ))
							{
								if( Directory[ strlen( Directory ) - 1 ] == '\\' || Directory[ strlen( Directory ) - 1 ] == '/' )
									sprintf( FileName, "%s%s", Directory, Arg );
								else
									sprintf( FileName, "%s/%s", Directory, Arg );
								fpSource = fopen( FileName, "r" );
								if( fpSource )
									break;
							}
							free( Path );

							if( !fpSource )
							{
								fprintf( stderr, "Couldn't open %s\n", Arg );
								if( ControlFlags & FLAG_COMMAND_RETURN )
									ExitWithError = TRUE;
								break;
							}
						}
						if( !(Scratch = TempFileName( NULL, "s" )))
						{
							fprintf( stderr, "Couldn't create temporary file\n" );
							fclose( fpSource );
							if( ControlFlags & FLAG_COMMAND_RETURN )
								ExitWithError = TRUE;
							break;
						}
						if( !(fpTarget = fopen( Scratch, "w" )))
						{
							fprintf( stderr, "Couldn't write to temporary file '%s'\n", Scratch );
							remove( Scratch );
							fclose( fpSource );
							if( ControlFlags & FLAG_COMMAND_RETURN )
								ExitWithError = TRUE;
							break;
						}

						HeaderRemoved = FALSE;
						while( fgets( Buffer, sizeof( Buffer ), fpSource ))
						{
							if( !HeaderRemoved )
							{
								fputs( "// ", fpTarget );
								if( Buffer[ 0 ] == '#' && Buffer[ 1 ] == '!' )
									HeaderRemoved = TRUE;
							}
					 		fputs( Buffer, fpTarget );
					 	}

						fclose( fpSource );
						fclose( fpTarget );
					}

					Expression = SlangParseFile( (ControlFlags & FLAG_COMMAND_PROC ? Scratch : Arg), Arg, SLANG_MODULE_FILE );
					if( Expression )
					{
						if( !EvaluateAndShowResults( Expression, SLANG_EVAL_RVALUE, &Ret, Scope, TimeItLoops )
								&& ( ControlFlags & FLAG_COMMAND_RETURN ) )
							ExitWithError = TRUE;
						SlangFree( Expression );
					}
					else
					{
						DisplaySlangError( "Parsing" );
						if( ControlFlags & FLAG_COMMAND_RETURN )
							ExitWithError = TRUE;
					}
					if( ControlFlags & FLAG_COMMAND_PROC )
						remove( Scratch );
					break;

				case ARG_TYPE_EXPRESSION:
					Expression = SlangParse( Arg, Arg, SLANG_MODULE_STRING );
					if( Expression )
					{
						if( !EvaluateAndShowResults( Expression, SLANG_EVAL_RVALUE, &Ret, Scope, TimeItLoops )
								&& ( ControlFlags & FLAG_COMMAND_RETURN ) )
							ExitWithError = TRUE;
						SlangFree( Expression );
					}
					else
					{
						DisplaySlangError( "Parsing" );
						if( ControlFlags & FLAG_COMMAND_RETURN )
							ExitWithError = TRUE;
					}
					break;

				case ARG_TYPE_PROFILER:
					ControlFlags |= FLAG_COMMAND_PROF;
					ProfilerFile = Arg;
					break;

				case ARG_TYPE_INVALID:
					DtSubscriptSetPointer( &ArrayValue, ArgNum++, DtString, Arg );
					SlangVariableSet( "Argv", &ArrayValue, NULL, FALSE );
					break;
			}
			ArgType			= ARG_TYPE_INVALID;
			InteractiveMode	= FALSE;
			if( ExitWithError )
			{
				ProgramReturnCode = 1;
				goto ShutDown;
			}
		}
	}
	DTM_FREE( &ArrayValue );


/*
**	Interactive mode?
*/

	if( InteractiveMode )
	{
		if( !(ControlFlags & FLAG_COMMAND_PROMPT ))
		{
			fprintf( stderr, "Slang Expression Evaluator:\n" );
			fflush( stderr );
		}

		if( ControlFlags & FLAG_COMMAND_PROMPT )
		{
			fputs( "slang> ", stdout );
			fflush( stdout );
		}
		while( fgets( Buffer, sizeof( Buffer ), stdin ))
		{
			Expression = SlangParse( Buffer, "Keyboard", SLANG_MODULE_KEYBOARD );
			if( Expression )
			{
				EvaluateAndShowResults( Expression, SLANG_EVAL_RVALUE, &Ret, Scope, TimeItLoops );
				SlangFree( Expression );
			}
			else
				DisplaySlangError( "Parsing" );

			if( ControlFlags & FLAG_COMMAND_PROMPT )
			{
				fputs( "slang> ", stdout );
				fflush( stdout );
			}
		}
	}


/*
**	Shutdown SecDb
*/

ShutDown:
	SlangScopeDestroy( Scope );
	SecDbShutdown( FastAndLoose );
	if( ControlFlags & FLAG_COMMAND_WINDOW )
		(*pfSecViewKillWindows)();
	return ProgramReturnCode;
}



/****************************************************************
**	Routine: PrintFunction
**	Returns:  0 - success
**			 -1 - failure
**	Action : Slang evaluator print function
****************************************************************/

static int PrintFunction(
	void	*Handle,
	char	*String
)
{
	SomethingWasPrinted = TRUE;

	if( ControlFlags & FLAG_COMMAND_QUIET )
		return 0;

	// IBM Bug Fix + Minor optimization
	// The IBM C compiler returns -1 for fputs( "", stdio )
	if( '\0' == *String )
		return 0;

	if( fputs( String, (FILE *) Handle ) >= 0 )
	{
		fflush( stdout );	// FIX??
		return 0;
	}

	Err( ERR_FILE_FAILURE, "fputs failed: %s", strerror( errno ) );
	return -1;
}



/****************************************************************
**	Routine: EvaluateAndShowResults
**	Returns: Success
**	Action : Evaluate an expression, and print the results
**			 Fails if SlangEvaluate returns SLANG_ERROR
****************************************************************/

static int EvaluateAndShowResults(
	SLANG_NODE		*Expression,
	SLANG_EVAL_FLAG	EvalFlag,
	SLANG_RET 		*Ret,
	SLANG_SCOPE		*Scope,
	int				TimeItLoops
)
{
	DT_VALUE
	 		StringValue;

	int		RetCode,
			Count	= 0,
			Success = TRUE;

	clock_t	Before,
			After;


	// If profiler is enabled, attach it to the expression
	if( ControlFlags & FLAG_COMMAND_PROF )
    {
        SlangProfilerInitialize();
		SlangProfilerAttach( Expression );
    }

	// Evaluate
	Message( "Evaluating..." );
	SomethingWasPrinted = FALSE;
	AbortRequested		= FALSE;
	Before				= clock();
	do
	{
		RetCode = SlangEvaluate( Expression, EvalFlag, Ret, Scope );
	} while( ++Count < TimeItLoops && RetCode == SLANG_OK );
	After				= clock();

	Message( "" );
	switch( RetCode )
	{
		case SLANG_OK:
		case SLANG_EXIT:
			if( !SomethingWasPrinted && !(ControlFlags & FLAG_COMMAND_NORES) )
			{
				StringValue.DataType = DtString;
				if( DTM_TO( &StringValue, &Ret->Data.Value ) )
				{
					if( ControlFlags & FLAG_COMMAND_PROMPT )
					{
						if ( *(char *) StringValue.Data.Pointer != '\0' )
							puts( (char *) StringValue.Data.Pointer );
					}
					else
						fprintf( stderr, "Evaluated: %s\n", (char*) StringValue.Data.Pointer );
					DTM_FREE( &StringValue );
				}
			}
			if( (ControlFlags & FLAG_COMMAND_RETURN) && Ret->Data.Value.DataType == DtDouble )
				ProgramReturnCode = (int) Ret->Data.Value.Data.Number;

			if ( EnableEventQueue )
			{
				pfSecViewAvailable = (PF_SEC_VIEW_AVAILABLE) DynaLink( "sd_secview", "_SecViewAvailable" );
				
				for( ;!(*pfSecViewAvailable)(); )
				{
					W_EVENT *Event;
					Event = w_EventNext( -1 );
					w_EventFree( Event );
				}
				SleepDouble( 1000*60*60*24*7 );
			}

			SLANG_RET_FREE( Ret );
			break;

		case SLANG_ERROR:
			DisplaySlangError( "Evaluation" );
			Success = FALSE;
			break;

		case SLANG_ABORT:
            {
                fprintf( stderr, "Slang Abort() encountered\n" );
                Success = FALSE;
                break;
            }
		case SLANG_BREAK:
			fprintf( stderr, "Slang Break encountered\n" );
			break;

		case SLANG_RETURN:
			fprintf( stderr, "Slang Return encountered\n" );
			break;

		case SLANG_EXCEPTION:
			{
				CC_NS(Gs, GsString) sErr;
				sErr = SecDbExceptionToString(*Ret);
				SLANG_RET_EXCEPT_FREE(Ret);
				fprintf( stderr, "Slang Exception encountered\n %s \n", sErr.c_str());
				Success = FALSE;
			}
			break;

		case SLANG_CONTINUE:
			fprintf( stderr, "Slang Continue encountered\n" );
			break;

		case SLANG_RETRY:
			fprintf( stderr, "SLANG_RETRY?\n" );
			break;

		case SLANG_BACKUP:
			fprintf( stderr, "SLANG_BACKUP?\n" );
			break;

		case SLANG_TRANS_ABORT:
			fprintf( stderr, "Slang TransactionAbort() encountered\n" );
			break;

		case SLANG_RDB_TRANS_ABORT:
			fprintf( stderr, "Slang RdbTransactionAbort() encountered\n" );
			break;
	}

	// Timer output
	if( TimeItLoops > 0 )
		fprintf( stderr, "(TimeIt: %.2f/%d == %.3f usec)\n", (After - Before) / (double)CLOCKS_PER_SEC, Count, (After - Before) * 1e6 / Count / (double)CLOCKS_PER_SEC );

	// Profiler output
	if( ControlFlags & FLAG_COMMAND_PROF )
	{
		PrintProfilerStatistics( Expression );
		SlangProfilerDetach( Expression );
	}
	return( Success );
}



/****************************************************************
**	Routine: PrintProfilerStatistics
**	Returns: None
**	Action : Print profiler statistics
****************************************************************/

static void PrintProfilerStatistics(
	SLANG_NODE	*Expression
)
{
	FILE	*fpProf;


	if( !(fpProf = fopen( ProfilerFile, "w" )))
		fpProf = stderr;

	SlangProfilerOutputStats( Expression, fpProf );

	if( fpProf != stderr )
		fclose( fpProf );
}



/****************************************************************
**	Routine: UseSecViewFunctions
**	Returns: None
**	Action : Use SecView functions (windows, dialogs, etc...)
****************************************************************/

static void UseSecViewFunctions( void )
{


/*
**	Dynalink secview
*/

	if( 	   (pfSecViewInitWindows		= (PF_SEC_VIEW_INIT_WINDOWS)		DynaLink( "sd_secview", "_SecViewInitWindows"		))
			&& (pfSecViewKillWindows		= (PF_SEC_VIEW_KILL_WINDOWS)		DynaLink( "sd_secview", "_SecViewKillWindows"		))
			&& (pfSecViewSlangSetContext	= (PF_SEC_VIEW_SLANG_SET_CONTEXT)	DynaLink( "sd_secview", "_SecViewSlangSetContext"	))
			&& (pfSecViewMessage			= (PF_SEC_VIEW_MESSAGE)				DynaLink( "sd_secview", "_SecViewMessage"			))
			&& (pfSecViewErrorMessage		= (PF_SEC_VIEW_ERROR_MESSAGE)		DynaLink( "sd_secview", "_SecViewErrorMessage"		)))
	{
		ControlFlags |= FLAG_COMMAND_WINDOW;
		(*pfSecViewInitWindows)();
		(*pfSecViewSlangSetContext)();

		SlangContextSet( "Debug Function", NULL );
		SlangContextSet( "Error Function", NULL );
	}
	else
		fprintf( stderr, "Error linking SecView functions - %s", ErrGetString( ));
}



/****************************************************************
**	Routine: Message
**	Returns: None
**	Action : Display SecDb messages
****************************************************************/

static void Message(
	const char	*Msg
)
{
	if( ControlFlags & FLAG_COMMAND_PROMPT )
		return;

	if( ControlFlags & FLAG_COMMAND_WINDOW )
		(*pfSecViewMessage)( Msg );
	else
	{
		if( !isatty( fileno( stderr )))
			fprintf( stderr, "%s\n", (Msg ? Msg : "" ));
		else
			fprintf( stderr, "%-78.78s\r", (Msg ? Msg : "" ));
		fflush( stderr );
	}
}



/****************************************************************
**	Routine: ErrorMessage
**	Returns: None
**	Action : Display SecDb messages
****************************************************************/

static void ErrorMessage(
	const char	*Msg
)
{
	if( ControlFlags & FLAG_COMMAND_WINDOW )
		(*pfSecViewErrorMessage)( Msg );
	else
	{
		fprintf( stderr, "ERROR: %s\n", (Msg ? Msg : "" ));
		fflush( stderr );
	}
}



/****************************************************************
**	Routine: DisplayParseError
**	Returns: None
**	Action : Display a parser error
****************************************************************/

static void DisplaySlangError(
	const char	*Type
)
{
	fprintf( stderr, "%s failed @ %s( %s ) - (%d,%d) - (%d,%d)\n%s\n",
			Type,
			SlangModuleNameFromType( SlangErrorInfo.ModuleType ),
			NULLSTR(SlangErrorInfo.ModuleName),
			SlangErrorInfo.BeginningLine,
			SlangErrorInfo.BeginningColumn,
			SlangErrorInfo.EndingLine,
			SlangErrorInfo.EndingColumn,
			ErrGetString() );
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



/****************************************************************
**	Routine: AbortTrapHandler
**	Returns: None
**	Action : Handle abort keys (Control-C and Break)
****************************************************************/

static void AbortTrapHandler( void )
{
	AbortRequested = TRUE;
}



/****************************************************************
**	Routine: ErrorTraceFunc
**	Returns: None
**	Action : Hook function to intercept all error messages and
**			 display to TraceFile
****************************************************************/

static void ErrorTraceFunc(
	ERR_NUM		ErrNum,
	ERR_C_CHAR	*ErrText
)
{
	if( ErrText )
	{
		fprintf( TraceFile, "*** (%d) %s ***\n", (int) ErrNum, ErrText );
		fflush( TraceFile );
	}
}



/****************************************************************
**	Routine: ShowUsage
**	Returns: None
**	Action : Display help and exit
****************************************************************/

static void ShowUsage( void )
{
	fprintf( stderr,
			"Usage: SecExpr [Database] [-a AppName] [-e Expression] [-f Filename] [-s Security] ...\n"
			"\t-a                   Set Application Name to server (is SecExpr by default). Note must be first parameter\n"
			"\t-c                   Trap Control-C and Break keys\n"
			"\t-d                   Destroy all variables\n"
			"\t-e Expression        Evaluate expression from the command line\n"
			"\t-f Filename          Evaluate script from a file\n"
			"\t-i                   Run interactively and prompt the user for input\n"
			"\t-j                   Fall into wevent loop rather than exit at end\n"
			"\t-l                   Set return value of program to that of script\n"
			"\t-p OutputFile        Enable profiler\n"
			"\t-q                   Quiet mode (disable print function)\n"
			"\t-r                   Disable printing of final results\n"
			"\t-s Security          Evaluate Slang security\n"
			"\t-source SourceDb     Set SourceDatabase\n"
			"\t-t [TraceFile]       Trace all error messages\n"
			"\t-timeit [n]          Evaluate each expr n times and write time stats to stderr\n"
			"\t-w                   Use windows (allow dialogs & windows)\n"
			"\t-x                   Act as a command processor\n"
			"\t-z                   Free all memory at shutdown (for purify use)\n"
			);

	exit( 1 );
}

/****************************************************************
**	Routine: OutputBufferSetFore
**	Returns: Always TRUE
**	Action : set foreground color
****************************************************************/

static int OutputBufferSetFore(
	void	*Handle,
	long Color
)
{
	if (isatty( fileno( stdout )))
	{
#if defined( WIN32 )
		HANDLE hConsole;
		WORD Mode = 0;
		int R, G, B;
		CONSOLE_SCREEN_BUFFER_INFO cbi;

		hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		if (hConsole == INVALID_HANDLE_VALUE) 
			return FALSE;
		if (GetConsoleScreenBufferInfo( hConsole, &cbi ))
			Mode = cbi.wAttributes & ~(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		R = ((unsigned long) Color >> 16) & 0xff;
		G = ((unsigned long) Color >> 8) & 0xff;
		B = ((unsigned long) Color) & 0xff;
		if (R >= 128)
			Mode |= FOREGROUND_RED;
		if (G >= 128)
			Mode |= FOREGROUND_GREEN;
		if (B >= 128)
			Mode |= FOREGROUND_BLUE;
		if (R > 128 || G > 128 || B > 128)
			Mode |= FOREGROUND_INTENSITY;
		if( !SetConsoleTextAttribute( hConsole, Mode ))
			return FALSE;
#endif
	}
	return TRUE;
}

/****************************************************************
**	Routine: OutputBufferSetBack
**	Returns: Always TRUE
**	Action : set background color
****************************************************************/

static int OutputBufferSetBack(
	void	*Handle,
	long Color
)
{
	if (isatty( fileno( stdout )))
	{
#if defined (WIN32)
		HANDLE hConsole;
		WORD Mode = 0;
		int R, G, B;
		CONSOLE_SCREEN_BUFFER_INFO cbi;

		hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
		if (hConsole == INVALID_HANDLE_VALUE) 
			return FALSE;
		if (GetConsoleScreenBufferInfo( hConsole, &cbi ))
			Mode = cbi.wAttributes & ~(BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
		R = ((unsigned long) Color >> 16) & 0xff;
		G = ((unsigned long) Color >> 8) & 0xff;
		B = ((unsigned long) Color) & 0xff;
		if (R >= 128)
			Mode |= BACKGROUND_RED;
		if (G >= 128)
			Mode |= BACKGROUND_GREEN;
		if (B >= 128)
			Mode |= BACKGROUND_BLUE;
		if (R > 128 || G > 128 || B > 128)
			Mode |= BACKGROUND_INTENSITY;
		if( !SetConsoleTextAttribute( hConsole, Mode ))
			return FALSE;
#endif
	}
	return TRUE;
}

