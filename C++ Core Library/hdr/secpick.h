/****************************************************************
**
**	secpick.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secview/src/secpick.h,v 1.3 2000/08/08 15:55:15 goodfj Exp $
**
****************************************************************/

#if !defined( IN_SECPICK_H )
#define IN_SECPICK_H


#ifndef _IN_SECDB_H
#include <secdb.h>
#endif
#ifndef _IN_WINDOW_H
#include <window.h>
#endif

/*
**	Structure used for picking securities
*/

typedef struct
{
	char	SecName[ SDB_SEC_NAME_SIZE ],
			TopName[ SDB_SEC_NAME_SIZE ];

} SEC_PICK_STATE;

#define SECPICK_CLASS_NAME_SEPARATOR ','

#define MAX_SECPICK_TYPES 10


typedef struct
{
	// These values are private and should not be accessed by the user
	WINDOW	Wnd;

	SDB_SEC_TYPE
			OldSecType;

	char	*Title;

	char	SearchName[ SDB_SEC_NAME_SIZE ];

	char	Prefix[ SDB_SEC_NAME_SIZE ];		// Prefix to add/ignore

	SDB_SEC_NAME_ENTRY
			*SecNames;

	int		FirstTime,
			LastLine;

	SDB_DB	*Db;

	// These values are public and may be set and read by the user
	SEC_PICK_STATE
			State;

	SDB_SEC_TYPE
			SecTypes[ MAX_SECPICK_TYPES ];	// Types of securities or SecTypes[0] = 0 for all

	int		ExitKey,						// Key that caused Select() to exit
			AutoHide 		: 1,			// Hide after Select()
			RefreshFlag		: 1,			// Refresh on next Select()
			RefreshAlways	: 1,			// Refresh on every Select()
			ExtraEditting	: 1,			// Edit/insert/delete on every Select()
			UseEnterKey		: 1,			// Use Enter to invoke security edit?
			PassUnusedKeys	: 1,			// Pass unused Keys back? (or beep)
			DoNotPushFKeys	: 1,			// Caller will display FKeys
			NumSecTypes; 		    		// Number of elements in SecTypes array

} SEC_PICK;


DLLEXPORT SEC_PICK
		*SecPickOpen(				const char *Title, int xLow, int yLow, int yHigh, SDB_DB *Db = NULL );

DLLEXPORT int
		SecPickCreateObject(        SDB_SEC_TYPE SecType, char *SecName, SDB_DB *Db = NULL ),
		SecPickSelect(		  		SEC_PICK *SecPick ),
		SecPickSelectSecClass(		const char *SecClass );

DLLEXPORT void
		SecPickClose(				SEC_PICK *SecPick ),
		SecPickHide(				SEC_PICK *SecPick ),
		SecPickShow(				SEC_PICK *SecPick );


/*
**	Yuck!  These should secpick APIs?
*/

DLLEXPORT SDB_SEC_TYPE
		SecViewShowObjectClasses(	void );

DLLEXPORT void
		SecViewShowClassInfo(			SDB_CLASS_INFO *ClassInfo ),
		SecViewShowClassInfoFromType(	SDB_SEC_TYPE SecType );

#endif

