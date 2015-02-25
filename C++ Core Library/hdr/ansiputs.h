/* $Header: /home/cvs/src/kool_ade/src/ansiputs.h,v 1.3 2001/11/27 22:48:57 procmon Exp $ */
/****************************************************************
**
**	ANSIPUTS.H	- Puts for NT that does ANSI colors
**	
**	Copied from src/tools/src/ansiputs.h, which is used only locally
**	to tools project, to avoid dll dependencies.
**
**	If you make any changes here, please also make those changes in
**	tools project.
**
****************************************************************/

#if !defined( IN_ANSIPUTS_H )
#	define IN_ANSIPUTS_H

#	ifdef WIN32
		DLLEXPORT int  AnsiPuts(	const char *Str );
		DLLEXPORT void AnsiPrintError( const char * );
#	else
#		define AnsiPuts(_Str)			fputs( _Str, stdout )
#		define AnsiPrintError(_Str)		perror(_Str);
#	endif  /* WIN32 */

#endif  /* IN_ANSIPUTS_H */

