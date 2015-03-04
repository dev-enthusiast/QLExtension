#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_err.c,v 1.23 2000/06/07 14:05:28 goodfj Exp $"
/****************************************************************
**
**	SDB_ERR.C	- Security database error handling functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_err.c,v $
**	Revision 1.23  2000/06/07 14:05:28  goodfj
**	Fix includes
**
**	Revision 1.22  1999/09/01 15:29:29  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.21  1999/04/23 00:03:31  nauntm
**	Unused arg/uninitialized variable extermination
**	
**	Revision 1.20  1999/03/22 16:33:12  gribbg
**	SecDbStartup now works more like SecDbInitialize--the second call
**	  still processes the arguments (sets AppName, UserName, Message
**	  functions, and attaches to the named database).
**	Default message functions now write to stderr.
**	Use SecDbNoMessage for no messages.
**	Allow "DEFAULT" as a username to cause SecDb to through environment.
**	
**	Revision 1.19  1999/01/21 19:40:12  gribbg
**	Use ERR_C_CHAR (safe to prodver)
**	
**	Revision 1.18  1998/01/02 21:32:55  rubenb
**	added GSCVSID
**
****************************************************************/

#define BUILDING_SECDB
#define		INCL_FILEIO
#include	<portable.h>
#include	<secdb.h>
#include	<secexpr.h>
#include	<secdrive.h>
#include	<sdb_int.h>



/*
**	Forward declare functions used to initialize variables
*/

static void
	DefaultErrorMsgFunc(	const char* Msg ),
	DefaultMsgFunc(			const char* Msg );


/*
**	Variables
*/

SDB_MSG_FUNC
		SecDbMessageFunc	  = DefaultMsgFunc,		// Application overridable msg function
		SecDbErrorMessageFunc = DefaultErrorMsgFunc;// Application overridable error msg function



/****************************************************************
**	Routine: DefaultMsgFunc
**	Returns: None
**	Action : Display a message from SecDb to stderr
****************************************************************/

static void DefaultMsgFunc(
	const char	*Msg
)
{
	if( !Msg )
		Msg = "";

	if( !isatty( fileno( stderr )))
		fprintf( stderr, "%s\n", Msg );
	else
		fprintf( stderr, "%-78.78s\r", Msg );
	fflush( stderr );
}



/****************************************************************
**	Routine: DefaultErrorMsgFunc
**	Returns: None
**	Action : Display a error message from SecDb to stderr
****************************************************************/

static void DefaultErrorMsgFunc(
	const char	*Msg
)
{
	fprintf( stderr, "ERROR: %s\n", Msg ? Msg : "" );
	fflush( stderr );
}



/****************************************************************
**	Routine: SecDbNoMessage
**	Returns: None
**	Action : Don't display the message
****************************************************************/

void SecDbNoMessage(
	const char	*
)
{
}



/****************************************************************
**	Routine: SecDbErrMsgHook
**	Returns: Level
**	Action : Displays message
****************************************************************/

static int SecDbErrMsgHook(
	ERR_LEVEL	Level,
	ERR_C_CHAR	*Msg
)
{
	if( BETWEEN( Level, ERR_LEVEL_INFO, ERR_LEVEL_ERROR - 1 ))
		(*SecDbMessageFunc)( Msg );
	else
		(*SecDbErrorMessageFunc)( Msg );
		
	return Level;
}



/****************************************************************
**	Routine: SecDbErrStartup
**	Returns: Nothing
**	Action : Hooks up Err API
****************************************************************/

void SecDbErrStartup( void )
{
	/* Only change error message vector if it has not already been done */
	if( ErrMsgHookFunc == NULL )
	{
		ErrMsgHookFunc = SecDbErrMsgHook;
		ErrLevel = ERR_LEVEL_INFO;
	}
	SecDbDriverErrStartup( );
}



/****************************************************************
**	Routine: SecDbErrConstants
**	Returns: Nothing
**	Action : Hooks up Err API
****************************************************************/

void SecDbErrConstants( void )
{
	// FIX - finish migration to only ERR_ enums
#define REGISTER_SDB(Name,Desc)		SlangRegisterConstant( "_" #Name, SDB_##Name ); SlangRegisterEnum( "ERROR", #Name, SDB_##Name )
#define REGISTER_ERR(Name)			SlangRegisterConstant( "_" #Name, Name ); SlangRegisterEnum( "ERROR", #Name, Name )
#include	<sdb_err.h>	
#undef	REGISTER_SDB
#undef	REGISTER_ERR
}

