/* $Header: /home/cvs/src/secview/src/wgenpick.h,v 1.3 1998/10/09 22:26:41 procmon Exp $ */
/****************************************************************
**
**	WGENPICK.H	- Window Generic Pick Routine
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_WGENPICK_H )
#define IN_WGENPICK_H


/*
**	Prototype functions
*/

DLLEXPORT int		w_generic_pick(			WINDOW *w, int MinLine, int MaxLine, int SearchMode );

#endif


