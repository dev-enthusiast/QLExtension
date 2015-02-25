/* $Header: /home/cvs/src/gmt/src/gmt.h,v 1.4 1998/10/09 22:12:15 procmon Exp $ */
/****************************************************************
**
**	GMT.H	- Greenwich mean time (GMT) stuff for OS/2
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_GMT_H )
#define IN_GMT_H

#include <time.h>

/*
**	What server is used for synchronization
**		NULL for none found in GMT.CFG
*/

DLLEXPORT char
		*GMTServer;


/*
**	GMT API functions (mostly disabled for Unix)
*/

DLLEXPORT time_t
		GMTGetTime( void );

DLLEXPORT int
		GMTResync( void );

DLLEXPORT long
		GMTGetOffset();

#endif	// IN_GMT_H
