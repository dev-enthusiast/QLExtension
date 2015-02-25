/* $Header: /home/cvs/src/window/src/wlocks.h,v 1.10 2004/10/22 19:59:19 khodod Exp $ */
/****************************************************************
**
**	WLOCKS.H	- Multitasking locks for window system
**
**	$Revision: 1.10 $
**
****************************************************************/

#ifndef IN_WLOCKS_H
#define	IN_WLOCKS_H

#include <window/base.h>
#include <gsthread/synch.h>

#ifndef GSTHREAD_KLUDGE
#define GSTHREAD_KLUDGE
#endif
#include <gsthread/backward.h>

#define MUTEX DLLEXPORT CC_NS(Gs,GsMutex)*
#define RMUTEX DLLEXPORT CC_NS(Gs,GsRecursiveMutex)*

RMUTEX  LK_CRITICAL;			/* Critical lock						*/
RMUTEX  LK_MEMORY;	            /* Lock memory allocation			   **/
RMUTEX  LK_LLIST;	            /* Lock linked list operations			*/
MUTEX   LK_BEEP;	 			/* Lock beep tone generator		       **/
MUTEX   LK_UNGET;				/* Lock unget system					*/
MUTEX	LK_INPUT;				/* Lock input system					*/
MUTEX	LK_REF;					/* Lock refresh function				*/
MUTEX	LK_OFF;					/* Lock refresh off state				*/
MUTEX 	LK_MOUSE;   			/* Lock mouse redraw				   **/
MUTEX	LK_KEYBOARD;   			/* Lock keyboard access					*/
MUTEX   LK_X;	    			/* Lock X protocol wire					*/
MUTEX   LK_BUF;					/* Lock double-buffering				*/
MUTEX   LK_EVENT_Q;				/* Lock event queue for writing		   **/
//#define LK_EVENT_E	11			/* Wait for event queue when empty		*/

MUTEX 	LK_USER;	   			/* Lock - user defined from here up		*/
#undef MUTEX
#undef RMUTEX
#endif

