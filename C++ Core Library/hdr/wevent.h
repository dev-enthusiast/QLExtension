/* $Header: /home/cvs/src/window/src/wevent.h,v 1.20 2004/10/22 19:59:15 khodod Exp $ */
/****************************************************************
**
**	wevent.h	- Window library events
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.20 $
**
****************************************************************/

#ifndef IN_WEVENT_H
#define IN_WEVENT_H

#include <window/base.h>

enum W_EVENT_TYPE
{
	WEV_TIMEOUT,
	WEV_KEY,
	WEV_MOUSE,
	WEV_USER,				// Invoke UserFunc when pulled from queue

	WEV_LAST
};


enum W_POST_TYPE
{
	WPOST_HEAD,
	WPOST_TAIL

};


struct W_EVENT
{
	W_EVENT	*Next;		// Don't touch, only w_EventPost can use this

	W_EVENT_TYPE
			Type;

	// Type == WEV_TIMEOUT

	// Type == WEV_KEY
	int		KeyCode,
			Ungot;		// TRUE if event generated via w_unget APIs


	// Type == WEV_MOUSE
	int		MouseX,
			MouseY,
			MouseButtons;

	WINDOW	*MouseWindow;


	// Type == WEV_USER
	void	(*UserFunc)( W_EVENT *Event );

	void	*UserData;

};

struct WEV_GLOBAL
{
	int		inp_wait;					// Number of tasks waiting for input
	int		initialized;				// w_EventSetup() has been called
};

EXPORT_CPP_WINDOW W_EVENT
		*w_EventNew(		W_EVENT_TYPE Type ),
		*w_EventNext(		double Timeout );

EXPORT_CPP_WINDOW void
		w_EventFree(		W_EVENT *Event ),
		w_EventPost(		W_EVENT *Event, W_POST_TYPE Where ),
		w_EventSetup(       void );		// Called internally by wi_start

DLLEXPORT WEV_GLOBAL
		wev_global;

/*
**	It's quite a bit of a hack, but we must have low level access to
**	interleave GUI and console events.
*/

#if defined( WIN32 )
#include 	<windows.h>

struct W_EVENT_GUI_HOOK_INFO
{
	HANDLE	WEventSema;		// This Semaphore gets signalled by w_EventPost

};

#else

struct W_EVENT_GUI_HOOK_INFO
{
	int		Nothing;

};

#endif

// Gui Hook should return true if there is an event for us to process
// false means that it hasn't checked for an event and we must do
// a wait on the semaphore

typedef int (*W_EVENT_GUI_HOOK)( W_EVENT_GUI_HOOK_INFO *Info );

// The following Priorities are currently used:
//
// 0		NtQuote uses this to process window messages
// 100		d_win.c uses this to process dialog messages

EXPORT_CPP_WINDOW void
		w_EventGuiHookAdd(	W_EVENT_GUI_HOOK NewHook, int Priority );
		
EXPORT_CPP_WINDOW void
		w_EventGuiHookRemove( W_EVENT_GUI_HOOK Hook );

EXPORT_CPP_WINDOW void
		w_SetWindowingState( int State );

EXPORT_CPP_WINDOW int 
		w_GetCurrentThreadID(),
		w_SetBlockHooks( int State );

EXPORT_CPP_WINDOW void 
		w_GetSecDbAccess( int threadID );

EXPORT_CPP_WINDOW void 
		w_ReleaseSecDbAccess();

//#define DEBUG_EVENTS
#if defined( DEBUG_EVENTS )
EXPORT_CPP_WINDOW
		void dump( char * s );
#define EV_DEBUG(x) dump(x)
#else
#define EV_DEBUG(x)
#endif

#endif

