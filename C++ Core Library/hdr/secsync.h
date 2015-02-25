/* $Header: /home/cvs/src/secsync/src/secsync.h,v 1.8 2001/11/27 23:26:25 procmon Exp $ */
/****************************************************************
**
**	secsync.h	- Header file for secsync callback API
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secsync/src/secsync.h,v 1.8 2001/11/27 23:26:25 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECSYNC_H )
#define IN_SECSYNC_H

// Messages sent (return values) from the dynalink'd DLL to SecSync
#define SECSYNC_EXIT		0		// Shutdown secsync at earliest convenience
#define SECSYNC_CONTINUE	1		// Normal return value

// Messages sent from SecSync to the dynalink'd DLL
typedef enum
{
	SECSYNC_DLL_START,
	SECSYNC_DLL_TRANSACTION,
	SECSYNC_DLL_STOP

} SECSYNC_DLL_MSG;

typedef	int (*SECSYNC_CALLBACK)(
		SDB_DB				*DestDatabase,
		SECSYNC_DLL_MSG		Message,
		SDB_TRANS_ID		DestTransaction
);

#endif

