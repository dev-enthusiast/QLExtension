/* $Header: /home/cvs/src/window/src/wfkeys.h,v 1.11 2004/10/22 19:59:16 khodod Exp $ */
/****************************************************************
**
**	wfkeys.h	- Window library Function key display
**
**	$Revision: 1.11 $
**
****************************************************************/

#ifndef IN_WFKEYS_H
#define IN_WFKEYS_H

#include <window/base.h>

/*
**	Function key structure
*/

struct FKEYS
{
	const char
			*key,
			*line1,
			*line2;

	char	active;

};


/*
**	Prototype functions
*/

EXPORT_CPP_WINDOW int
		push_fkeys(	const FKEYS *f ),
		pop_fkeys(	void );

#endif
