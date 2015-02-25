/* $Header: /home/cvs/src/tap/src/tapskey.h,v 1.6 2001/11/27 23:31:12 procmon Exp $ */
/****************************************************************
**
**	TAPSKEY.H	- Tap Sort Keys types & functions
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#ifndef IN_TAPKEY_H
#define IN_TAPKEY_H

#include <hash.h>
#include <outform.h>
#include <secedit.h>


/*
**	Sort key type.  Used to display values on pick screen
*/

struct TAP_SORT_KEY
{
	SDB_VALUE_TYPE
			ValueType;

	char	*Prompt;

	EDIT_INFO_TYPE
			Type;

	OUTPUT_FORMAT
			Format;

	char	**StrTab;	// for STRTABNUM fields

};



/*
**	Functions
*/


// in TAPKEY.C

int		TapSortKeyStartup(	void );

char	**TapSortKeyStrTab( void );

TAP_SORT_KEY
		*TapSortKeyLookup(	const char *KeyName );

int		TapSortKeyAdd(		EDIT_INFO_TYPE Type, char *ValueType, char *Prompt, OUTPUT_FORMAT *Fmt, char **StrTab ),
		TapSortKeysToValue( DT_VALUE *Value );

#endif

