/* $Header: /home/cvs/src/window/src/wmenu.h,v 1.7 2004/10/22 19:59:19 khodod Exp $ */
/****************************************************************
**
**	wmenu.h	- Window system menu constants and structures
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#ifndef IN_WMENU_H
#define IN_WMENU_H

#include <window/base.h>


/*
**  Define menu control flags
*/

#define WM_FUNC		0x01		// Function being passed
#define WM_HIDE		0x02		// Hide calling menu
#define WM_HMENU	0x04		// Horizontal menu
#define	WM_VMENU	0x08		// Vertical menu
#define WM_QUIT		0x10		// Quit option on menu


/*
**  Declare structure for a vertical menu
*/

struct W_HMENU;

struct W_VMENU
{
    char    keycode,
			*option;

    union
    {
        int     (*func)( int );
		W_VMENU  *vmenu;
        W_HMENU  *hmenu;
    } oper;
    char    op_type;

};


/*
**	Declare structure for a horizontal menu
*/

struct W_HMENU
{
    char    keycode;
    char    *option;
    char    *desc;
    union
    {
        int     (*func)( int );
        W_VMENU  *vmenu;
        W_HMENU  *hmenu;
    } oper;
    char    op_type;

};



/*
**  Declare types for all window routines (keep 'em honest)
*/

EXPORT_CPP_WINDOW int
		w_vmenu(  	W_VMENU *, int, int, long ),
		w_hmenu(  	W_HMENU *, int, int, long );

#endif

