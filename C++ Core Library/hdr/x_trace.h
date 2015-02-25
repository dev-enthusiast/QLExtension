/* $Header: /home/cvs/src/secview/src/x_trace.h,v 1.5 1999/07/20 16:41:13 singhki Exp $ */
/****************************************************************
**
**	X_TRACE.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_X_TRACE_H )
#define IN_X_TRACE_H

#define TRACE_VERBOSE		0x0001		// Trace picayune
#define	TRACE_STEP			0x0002		// Stop at each trace node
#define	TRACE_PRINT			0x0004		// Use PrintHandle (default if !SecViewAvailable)
#define TRACE_MESSAGE		0x0008		// Write tracing to message popup
#define TRACE_SKIP_ERRORS	0x0010		// Do not automatically stop on errors
#define	TRACE_NO_REFRESH	0x0020		// Do not refresh screen while tracing
#define TRACE_STOP			0x0040		// stop tracing
#define TRACE_START_ON_ERROR 0x0080		// Start tracing only on error

#endif

