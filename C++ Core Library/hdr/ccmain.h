/****************************************************************
**
**	ccmain.h	- Redefine main() to catch exceptions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/ccmain.h,v 1.9 2013/06/07 21:54:41 khodod Exp $
**
****************************************************************/

#if defined( IN_CCMAIN_H )
#error ccmain.h must only be included once
#endif

#define IN_CCMAIN_H

#include <exception>

#ifdef WIN32
#include <windows.h>
#include <trap.h>
#endif


/*
**	User must declare main exactly like this prototype
*/

int RealMain( int Argc, char **Argv );

/****************************************************************
**	Routine: main
**	Returns: 0 - success or exit code
**	Action : Call RealMain with try/catch/ErrMsg wrapping
****************************************************************/

int main(
	int		Argc,
	char	**Argv
)
{
#ifdef CC_NO_EXCEPTIONS
	return RealMain( Argc, Argv );
#else
	try
	{
#ifdef WIN32
		TrapFatalSignals();
#endif
		return RealMain( Argc, Argv );
	}
	catch( std::exception& e )
	{
		ErrMsg( ERR_LEVEL_ERROR, "Exception caught\n%s", e.what() );
	}
#endif
	return 255;
}

#define main RealMain

