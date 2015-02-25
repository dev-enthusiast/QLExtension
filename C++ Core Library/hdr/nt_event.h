/* $Header: /home/cvs/src/window/src/nt_event.h,v 1.5 2004/10/22 19:59:14 khodod Exp $ */
/****************************************************************
**
**	nt_event.h	- User event posting and dispatching
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#ifndef IN_NT_EVENT_H
#define IN_NT_EVENT_H

#include <window/base.h>
#include <window.h>
#include <wevent.h>

#define WM_USER_EVENT					WM_USER

// define first message id for ntgrid
#define	WM_FIRST_NTGRID_MESSAGE			WM_USER + 0x100
// define first message id for win glb
#define WM_FIRST_GLB_MESSAGE			WM_USER + 0x100

typedef void (*UserEventHandlerFunc)( void *HandlerData );

/*
**	Variables
*/

DLLEXPORT HWND
		UserEventHandlerHWnd;
		
/*
**	Macros
*/

#define	NT_USER_EVENT_POST( HandlerFunc, HandlerData )	\
	( UserEventHandlerHWnd ?							\
		PostMessage( UserEventHandlerHWnd, WM_USER_EVENT, (WPARAM) (HandlerData), (LPARAM) (HandlerFunc) ), TRUE \
		: FALSE )
		
		
#define NT_USER_EVENT_REGISTER_HANDLER( HWnd )	UserEventHandlerHWnd = ( HWnd )

#define NT_USER_EVENT_DISPATCH( wParam, lParam )	\
	( * (UserEventHandlerFunc) (lParam) )( (void *) (wParam) )

#endif

