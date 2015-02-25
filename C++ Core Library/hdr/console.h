/****************************************************************
**
**	CONSOLE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**  Portability wrapper for Get/SetConsoleTitle api on unix.  Creates
**  two global function pointers that get overriden when the unix
**  dubwindow library starts up.  The header file contains macros that
**  wrap the function pointers to look like Get/SetConsoleTitle().
**
**	$Id: console.h,v 1.3 2000/07/16 03:50:47 turokm Exp $
**
****************************************************************/

#ifndef IN_PORTABLE_CONSOLE_H
#define IN_PORTABLE_CONSOLE_H

#if defined( __unix )

#include <portable.h>

extern bool ( *SetConsoleTitlePtr )( const char *Title );
extern bool ( *GetConsoleTitlePtr )( char *Title, int Size );

#define SetConsoleTitle( Title )       ( *SetConsoleTitlePtr )( Title )
#define GetConsoleTitle( Title, Size ) ( *GetConsoleTitlePtr )( Title, Size )

#endif // __unix

#endif // IN_PORTABLE_CONSOLE_H
